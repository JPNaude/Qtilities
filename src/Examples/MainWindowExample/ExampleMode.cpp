/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include "ExampleMode.h"
#include "ui_ExampleMode.h"

#include <DynamicSideWidgetViewer.h>
#include <QtilitiesCore.h>
#include <QtilitiesCoreConstants.h>
#include <QtilitiesCoreGui.h>

#include <QtGui>

using namespace Qtilities::Core::Constants;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::CoreGui::Actions;

struct ExampleModeData {
    ExampleModeData() : initialized(false),
    side_viewer_dock(0),
    side_viewer_widget(0),
    main_splitter(0),
    actionShowDock(0),
    text_editor(0) {}

    bool initialized;
    QDockWidget* side_viewer_dock;
    DynamicSideWidgetViewer* side_viewer_widget;
    QSplitter* main_splitter;
    QAction* actionShowDock;
    QTextEdit* text_editor;
};

ExampleMode::ExampleMode(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExampleMode)
{
    ui->setupUi(this);
    d = new ExampleModeData;

    // Create and dock the dynamic side widget viewer
    d->side_viewer_dock = new QDockWidget(tr("Dynamic Widgets"));
    d->side_viewer_widget = new DynamicSideWidgetViewer(MODE_EXAMPLE_ID);
    d->side_viewer_dock->setWidget(d->side_viewer_widget);
    Qt::DockWidgetAreas allowed_areas = 0;
    allowed_areas |= Qt::LeftDockWidgetArea;
    allowed_areas |= Qt::RightDockWidgetArea;
    d->side_viewer_dock->setAllowedAreas(allowed_areas);
    addDockWidget(Qt::RightDockWidgetArea,d->side_viewer_dock);
    d->side_viewer_dock->installEventFilter(this);

    // Create text editor
    d->text_editor = new QTextEdit();
    d->text_editor->setText("This is an example mode with a text editor, and a set of dynamically loaded widgets in the dock window.");

    // Create splitters
    if (ui->splitterParent->layout())
        delete ui->splitterParent->layout();

    // Create new layout with new widget
    d->main_splitter = new QSplitter(Qt::Horizontal);
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,ui->splitterParent);
    layout->addWidget(d->text_editor);
    layout->setMargin(0);
    layout->setSpacing(0);

    // Actions
    d->actionShowDock = new QAction(QIcon(),"Example Dynamic Dock Widget",this);
    d->actionShowDock->setCheckable(true);
    d->actionShowDock->setChecked(true);
    connect(d->actionShowDock,SIGNAL(triggered(bool)),SLOT(toggleDock(bool)));

    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(CONTEXT_STANDARD));
    Command* command = ACTION_MANAGER->registerAction("ExampleMode.DynamicDockWidget",d->actionShowDock,context);
    bool existed;
    ActionContainer* view_menu = QtilitiesCoreGui::instance()->actionManager()->createMenu(MENU_VIEW,existed);
    view_menu->addAction(command);
}

bool ExampleMode::eventFilter(QObject *object, QEvent *event) {
    if (object == d->side_viewer_dock && event->type() == QEvent::Close) {
        d->actionShowDock->setChecked(false);
    }

    return false;
}

void ExampleMode::toggleDock(bool toggle) {
    if (toggle) {
        d->side_viewer_dock->show();
    } else {
        d->side_viewer_dock->hide();
    }
}

void ExampleMode::loadFile(const QString& file_name) {
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString file_string = file.readAll();
    d->text_editor->setText(file_string);
}

ExampleMode::~ExampleMode()
{
    delete ui;
}

QWidget* ExampleMode::widget() {
    return this;
}

void ExampleMode::initialize() {
    if (d->initialized)
        return;

    // Check all objects in the global object pool.
    QMap<QString, ISideViewerWidget*> text_iface_map;
    QList<QObject*> widgets = OBJECT_MANAGER->registeredInterfaces("ISideViewerWidget");
    for (int i = 0; i < widgets.count(); i++) {
        ISideViewerWidget* side_viewer_widget = qobject_cast<ISideViewerWidget*> (widgets.at(i));
        if (side_viewer_widget) {
            text_iface_map[side_viewer_widget->text()] = side_viewer_widget;
        }
    }

    d->side_viewer_widget->setIFaceMap(text_iface_map);
    d->side_viewer_widget->show();
    d->initialized = true;
}

QIcon ExampleMode::icon() const {
    return QIcon(ICON_QTILITIES_SYMBOL_64x64);
}

QString ExampleMode::text() const {
    return tr("Example Mode");
}

void ExampleMode::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
