/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#include "ConfigurationWidget.h"
#include "ui_ConfigurationWidget.h"
#include "IConfigPage.h"
#include "ObserverWidget.h"

#include <Logger.h>
#include <QTreeWidgetItem>
#include <QBoxLayout>
#include <QDesktopWidget>

using namespace Qtilities::CoreGui::Interfaces;

struct Qtilities::CoreGui::ConfigurationWidgetData {
    ConfigurationWidgetData() : config_pages("Application Settings","Manages IConfigPages in an application"),
    activity_filter(0),
    active_widget(0),
    apply_all_pages(true) {}

    //! Observer widget which is used to display the categories of the config pages.
    ObserverWidget config_pages_widget;
    //! The configuration pages observer.
    Observer config_pages;
    //! Indicates if the page have been initialized.
    bool initialized;
    //! The activity policy filter for the pages observer.
    ActivityPolicyFilter* activity_filter;
    //! Keeps track of the active widget.
    QPointer<QWidget> active_widget;
    //! The display mode of this widget.
    Qtilities::DisplayMode display_mode;
    //! The apply all pages setting for this widget.
    bool apply_all_pages;
};

Qtilities::CoreGui::ConfigurationWidget::ConfigurationWidget(DisplayMode display_mode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationWidget)
{
    ui->setupUi(this);
    d = new ConfigurationWidgetData;
    d->initialized = false;
    d->display_mode = display_mode;

    // Layout and placement of observer widget:
    if (ui->configPagesWidgetHolder->layout())
        delete ui->configPagesWidgetHolder->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->configPagesWidgetHolder);
    layout->addWidget(&d->config_pages_widget);
    layout->setMargin(0);

    // Put the widget in the center of the screen:
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());

    setAttribute(Qt::WA_QuitOnClose,false);
}

Qtilities::CoreGui::ConfigurationWidget::~ConfigurationWidget() {
    delete ui;
    delete d;
}

void Qtilities::CoreGui::ConfigurationWidget::initialize(QList<IConfigPage*> config_pages) {
    if (d->initialized)
        return;

    // Add an activity policy filter to the config pages observer:
    d->activity_filter = new ActivityPolicyFilter();
    d->activity_filter->setActivityPolicy(ActivityPolicyFilter::UniqueActivity);
    d->activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::ProhibitNoneActive);
    d->activity_filter->setNewSubjectActivityPolicy(ActivityPolicyFilter::SetNewInactive);
    connect(d->activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),SLOT(handleActiveItemChanges(QList<QObject*>)));
    d->config_pages.installSubjectFilter(d->activity_filter);
    d->config_pages.useDisplayHints();
    d->config_pages.displayHints()->setActivityControlHint(ObserverHints::FollowSelection);
    d->config_pages.displayHints()->setActivityDisplayHint(ObserverHints::NoActivityDisplay);
    d->config_pages.displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    d->config_pages.displayHints()->setActionHints(ObserverHints::ActionFindItem);

    // Figure out what the maximum sizes are in all pages.
    int max_width = -1;
    int max_height = -1;

    // Add all the pages to the config pages observer:
    for (int i = 0; i < config_pages.count(); i++) {
        IConfigPage* config_page = config_pages.at(i);
        if (config_page) {
            if (config_page->configPageWidget()) {
                // Set the object name to the page title:
                config_page->objectBase()->setObjectName(config_page->configPageTitle());

                // Add the category as a property on the object:
                if (!config_page->configPageCategory().isEmpty()) {
                    if (Observer::propertyExists(config_page->objectBase(),OBJECT_CATEGORY)) {
                        ObserverProperty category_property = Observer::getObserverProperty(config_page->objectBase(),OBJECT_CATEGORY);
                        category_property.setValue(qVariantFromValue(config_page->configPageCategory()),d->config_pages.observerID());
                        Observer::setObserverProperty(config_page->objectBase(),category_property);
                    } else {
                        ObserverProperty category_property(OBJECT_CATEGORY);
                        category_property.setValue(qVariantFromValue(config_page->configPageCategory()),d->config_pages.observerID());
                        Observer::setObserverProperty(config_page->objectBase(),category_property);
                    }
                }
                // Add the icon as a property on the object:
                if (!config_page->configPageIcon().isNull()) {
                    SharedObserverProperty icon_property(config_page->configPageIcon(),OBJECT_ROLE_DECORATION);
                    Observer::setSharedProperty(config_page->objectBase(),icon_property);
                }

                d->config_pages.attachSubject(config_page->objectBase());

                if (config_page->configPageWidget()->size().width() > max_width)
                    max_width = config_page->configPageWidget()->sizeHint().width();
                if (config_page->configPageWidget()->size().height() > max_height)
                    max_height = config_page->configPageWidget()->sizeHint().height();
            } else {
                LOG_DEBUG("Found configuration page without a valid configuration widget. This page will not be shown.");
            }
        }
    }

    // Check if the max sizes are bigger than the current size. If so we need to resize:
    int new_width = size().width();
    int new_heigth = size().height();
    if (max_width > size().width())
        new_width = max_width;
    if (max_height > size().height())
        new_heigth = max_height;
    resize(new_width,new_heigth);

    // Set up the observer widget:
    d->config_pages_widget.setDisplayMode(d->display_mode);
    d->config_pages_widget.setObserverContext(&d->config_pages);
    d->config_pages_widget.initialize();
    d->config_pages_widget.resizeTableViewRows(22);
    d->config_pages_widget.toggleSearchBox();
    if (d->config_pages_widget.searchBoxWidget())
        d->config_pages_widget.searchBoxWidget()->setButtonFlags(SearchBoxWidget::NoButtons);

    // Set up the table or tree view:
    if (d->config_pages_widget.tableView() && d->display_mode == TableView) {
        d->config_pages_widget.tableView()->horizontalHeader()->hide();
        d->config_pages_widget.tableView()->sortByColumn(1,Qt::AscendingOrder);
    } else if (d->config_pages_widget.treeView() && d->display_mode == TreeView) {
        d->config_pages_widget.treeView()->setRootIsDecorated(false);
        d->config_pages_widget.treeView()->sortByColumn(1,Qt::AscendingOrder);
    }

    d->initialized = true;
}

void Qtilities::CoreGui::ConfigurationWidget::initialize(QList<QObject*> config_pages) {
    if (config_pages.isEmpty()) {
        config_pages = OBJECT_MANAGER->registeredInterfaces("IConfigPage");
        LOG_DEBUG(QString("%1 configuration page(s) found.").arg(config_pages.count()));
    }

    QList<IConfigPage*> config_ifaces;
    for (int i = 0; i < config_pages.count(); i++) {
        IConfigPage* config_iface = qobject_cast<IConfigPage*> (config_pages.at(i));
        if (config_iface)
            config_ifaces.append(config_iface);
    }

    initialize(config_ifaces);
}

void Qtilities::CoreGui::ConfigurationWidget::on_btnClose_clicked() {
    close();
}

void Qtilities::CoreGui::ConfigurationWidget::on_btnApply_clicked() {
    if (d->apply_all_pages) {
        for (int i = 0; i < d->config_pages.subjectCount(); i++) {
            IConfigPage* config_page = qobject_cast<IConfigPage*> (d->config_pages.subjectAt(i));
            if (config_page){
                config_page->configPageApply();
                appliedPage(config_page);
            }
        }
    } else {
        if (d->activity_filter->activeSubjects().count() == 1) {
            IConfigPage* config_page = qobject_cast<IConfigPage*> (d->activity_filter->activeSubjects().front());
            config_page->configPageApply();
            appliedPage(config_page);
        }
    }

}

void Qtilities::CoreGui::ConfigurationWidget::setApplyAllPages(bool apply_all_pages) {
    d->apply_all_pages = apply_all_pages;
}

bool Qtilities::CoreGui::ConfigurationWidget::applyAllPages() const {
    return d->apply_all_pages;
}

void Qtilities::CoreGui::ConfigurationWidget::handleActiveItemChanges(QList<QObject*> active_pages) {
    if (active_pages.count() != 1)
        return;

    IConfigPage* config_page = qobject_cast<IConfigPage*> (active_pages.front());
    if (config_page) {
        // Hide the current widget
        if (d->active_widget)
            d->active_widget->hide();

        ui->btnApply->setEnabled(config_page->supportsApply());
        ui->lblPageHeader->setText(config_page->configPageTitle());
        if (!config_page->configPageIcon().isNull()) {
            ui->lblPageIcon->setPixmap(QPixmap(config_page->configPageIcon().pixmap(32,32)));
            ui->lblPageIcon->setVisible(true);
        } else
            ui->lblPageIcon->setVisible(false);

        if (ui->configWidget->layout())
            delete ui->configWidget->layout();

        // Create new layout with new widget
        QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,ui->configWidget);
        layout->setMargin(0);
        d->active_widget = config_page->configPageWidget();
        layout->addWidget(d->active_widget);
        if (d->active_widget) {
            d->active_widget->show();
            if (d->active_widget->layout())
                d->active_widget->layout()->setMargin(0);
        } else {
            d->active_widget = new QLabel(tr("Failed to find configuartion page for selected item."));
            d->active_widget->show();
        }
    }
}

void Qtilities::CoreGui::ConfigurationWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Qtilities::CoreGui::ConfigurationWidget::setActivePage(const QString& active_page_name) {
    QObject* page = d->config_pages.subjectReference(active_page_name);
    if (page) {
        QList<QObject*> active_pages;
        active_pages << page;
        d->activity_filter->setActiveSubjects(active_pages);
        d->config_pages_widget.selectObjects(active_pages);
    }
}

QString Qtilities::CoreGui::ConfigurationWidget::activePageName() const {
    if (!d->initialized)
        return QString();

    IConfigPage* config_page = qobject_cast<IConfigPage*> (d->activity_filter->activeSubjects().front());
    return config_page->configPageTitle();
}

Qtilities::CoreGui::Interfaces::IConfigPage* Qtilities::CoreGui::ConfigurationWidget::activePageIFace() const {
    if (!d->initialized)
        return 0;

    IConfigPage* config_page = qobject_cast<IConfigPage*> (d->activity_filter->activeSubjects().front());
    return config_page;
}
