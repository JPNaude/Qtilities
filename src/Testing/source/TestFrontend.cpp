/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
**
****************************************************************************/

#include "TestFrontend.h"
#include "ui_TestFrontend.h"
#include "ITestable.h"

#include <stdio.h>
#include <time.h>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

struct Qtilities::Testing::TestFrontendPrivateData {
    ObserverWidget          tests_observer_widget;
    ActivityPolicyFilter*   tests_activity_filter;
    TreeNode                tests_observer;
    char **                 argv;
    int                     argc;
    QPointer<QWidget>       log_widget;
    bool                    multiple_tests;
    int                     success_count;
    int                     error_count;

    QAction*                actionInvertSelection;
    QAction*                actionSetAllActive;
    QAction*                actionSetAllInactive;
};

Qtilities::Testing::TestFrontend::TestFrontend(int argc, char ** argv, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestFrontend)
{
    ui->setupUi(this);
    d = new TestFrontendPrivateData;
    d->argc = argc;
    d->argv = argv;
    d->error_count = 0;
    d->success_count = 0;
    d->multiple_tests = false;
    setWindowTitle(tr("Application Tester"));

    d->tests_activity_filter = d->tests_observer.enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::CheckboxTriggered);
    d->tests_activity_filter->setNewSubjectActivityPolicy(ActivityPolicyFilter::SetNewActive);

    d->tests_observer.enableCategorizedDisplay();
    d->tests_observer.setObjectName(tr("Registered Tests"));
    d->tests_observer.displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);

    d->actionInvertSelection = new QAction("Invert",this);
    connect(d->actionInvertSelection,SIGNAL(triggered()),d->tests_activity_filter,SLOT(invertActivity()));
    d->actionSetAllActive = new QAction("All Active",this);
    connect(d->actionSetAllActive,SIGNAL(triggered()),d->tests_activity_filter,SLOT(setAllActive()));
    d->actionSetAllInactive= new QAction("None Active",this);
    connect(d->actionSetAllInactive,SIGNAL(triggered()),d->tests_activity_filter,SLOT(setNoneActive()));

    d->tests_observer_widget.actionProvider()->addAction(d->actionInvertSelection,QtilitiesCategory("Activity"));
    d->tests_observer_widget.actionProvider()->addAction(d->actionSetAllActive,QtilitiesCategory("Activity"));
    d->tests_observer_widget.actionProvider()->addAction(d->actionSetAllInactive,QtilitiesCategory("Activity"));

    d->tests_observer_widget.setObserverContext(&d->tests_observer);
    d->tests_observer_widget.initialize();

    if (ui->widgetTestListHolder->layout())
        delete ui->widgetTestListHolder->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->widgetTestListHolder);
    layout->setMargin(0);
    layout->addWidget(&d->tests_observer_widget);
}

Qtilities::Testing::TestFrontend::~TestFrontend()
{
    delete ui;
}

void Qtilities::Testing::TestFrontend::addTest(ITestable* test, QtilitiesCategory category) {
    if (!test)
        return;

    if (!test->objectBase())
        return;

    if (category.isValid()) {
        MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
        category_property.setValue(qVariantFromValue(category),d->tests_observer.observerID());
        ObjectManager::setMultiContextProperty(test->objectBase(),category_property);
    }

    test->objectBase()->setObjectName(test->testName());
    d->tests_observer << test->objectBase();
    d->tests_observer_widget.viewExpandAll();
}

void Testing::TestFrontend::execute() {
    on_btnExecute_clicked();
}

int Testing::TestFrontend::numberOfFailedTests() const {
    return d->error_count;
}

void Qtilities::Testing::TestFrontend::on_btnExecute_clicked() {
    d->success_count = 0;
    d->error_count = 0;

    time_t start,end;
    time(&start);

    ui->txtResults->setText(QString(tr("Testing in progress...")));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (d->multiple_tests) {
        QList<QObject*> active_tests = d->tests_activity_filter->activeSubjects();
        for (int i = 0; i < active_tests.count(); ++i) {
            ITestable* test = qobject_cast<ITestable*> (active_tests.at(i));
            if (test) {
                SharedProperty property(qti_prop_DECORATION,QVariant(QIcon()));
                ObjectManager::setSharedProperty(active_tests.at(i), property);
            }
        }
    }

    // Execute the tests:
    QList<QObject*> active_tests = d->tests_activity_filter->activeSubjects();
    for (int i = 0; i < active_tests.count(); ++i) {
        ITestable* test = qobject_cast<ITestable*> (active_tests.at(i));
        if (test) {
            if (test->execTest(d->argc,d->argv) == 0) {
                SharedProperty property(qti_prop_DECORATION,QVariant(QIcon(qti_icon_SUCCESS_12x12)));
                ObjectManager::setSharedProperty(active_tests.at(i), property);
                ++d->success_count;
            } else {
                SharedProperty property(qti_prop_DECORATION,QVariant(QIcon(qti_icon_ERROR_12x12)));
                ObjectManager::setSharedProperty(active_tests.at(i), property);
                ++d->error_count;
            }
        }
    }

    QApplication::restoreOverrideCursor();
    time(&end);
    double diff = difftime(end,start);
    ui->txtResults->setText(QString(tr("Testing completed in %1 seconds: %2 passed, %3 failed.")).arg(diff).arg(d->success_count).arg(d->error_count));
    d->multiple_tests = true;
}

void Qtilities::Testing::TestFrontend::on_btnShowLog_clicked()
{
    if (!d->log_widget) {
        QString engine_name = "Test Log";
        d->log_widget = LoggerGui::createLogWidget(&engine_name);
    }

    d->log_widget->show();
}
