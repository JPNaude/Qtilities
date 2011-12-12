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

#include "HelpMode.h"
#include "HelpBrowser.h"
#include "ContentWidgetFactory.h"
#include "IndexWidgetFactory.h"
#include "SearchWidgetFactory.h"
#include "HelpPluginConstants.h"

#include <QtGui>
#include <QtHelp>

#include <QtWebKit/QWebView>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;
using namespace Qtilities::Plugins::Help::Constants;

struct Qtilities::Plugins::Help::HelpModeData {
    HelpModeData() : initialized(false),
    side_viewer_dock(0),
    side_viewer_widget(0),
    actionShowDock(0),
    help_browser(0),
    content_widget(0),
    index_widget(0),
    search_widget(0) {}

    bool initialized;
    QDockWidget* side_viewer_dock;
    DynamicSideWidgetViewer* side_viewer_widget;
    QAction* actionShowDock;
    HelpBrowser* help_browser;
    ContentWidgetFactory* content_widget;
    IndexWidgetFactory* index_widget;
    SearchWidgetFactory* search_widget;
    QWebView *view;
    QHelpEngine* helpEngine;
};

Qtilities::Plugins::Help::HelpMode::HelpMode(QWidget *parent) :
    QMainWindow(parent)
{
    d = new HelpModeData;
    setObjectName("Help Mode");

    // Create and dock the dynamic side widget viewer
    d->side_viewer_dock = new QDockWidget(tr("Dynamic Help Widgets"));
    d->side_viewer_widget = new DynamicSideWidgetViewer(MODE_HELP_ID);
    connect(d->side_viewer_widget,SIGNAL(toggleVisibility(bool)),SLOT(toggleDock(bool)));
    d->side_viewer_dock->setWidget(d->side_viewer_widget);
    Qt::DockWidgetAreas allowed_areas = 0;
    allowed_areas |= Qt::LeftDockWidgetArea;
    allowed_areas |= Qt::RightDockWidgetArea;
    d->side_viewer_dock->setAllowedAreas(allowed_areas);
    addDockWidget(Qt::RightDockWidgetArea,d->side_viewer_dock);
    d->side_viewer_dock->installEventFilter(this);

    // Actions
    d->actionShowDock = new QAction(QIcon(),tr("Dynamic Help Widgets"),this);
    d->actionShowDock->setCheckable(true);
    d->actionShowDock->setChecked(true);
    connect(d->side_viewer_widget,SIGNAL(toggleVisibility(bool)),d->actionShowDock,SLOT(setChecked(bool)));
    connect(d->actionShowDock,SIGNAL(triggered(bool)),SLOT(toggleDock(bool)));

    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(CONTEXT_STANDARD));
    Command* command = ACTION_MANAGER->registerAction("HelpMode.DynamicDockWidget",d->actionShowDock,context);
    bool existed;
    ActionContainer* view_menu = ACTION_MANAGER->createMenu(MENU_VIEW,existed);
    if (!existed) {
        ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(MENUBAR_STANDARD,existed);
        menu_bar->addMenu(view_menu);
        view_menu->addAction(command,MENU_HELP);
    } else
        view_menu->addAction(command);

    // Here we create all the help widgets:
    d->helpEngine = new QHelpEngine("D:/Projects/Qt/Qtilities/trunk/doc/output/html_packages/qtilities.qhc", this);
    d->helpEngine->setupData();
    QHelpSearchEngine* helpSearchEngine = new QHelpSearchEngine(d->helpEngine,this);

    // - Help Browser
    d->help_browser = new HelpBrowser(d->helpEngine);
    //setCentralWidget(d->help_browser);

    // - Register Contents Widget Factory
    d->content_widget = new ContentWidgetFactory(d->helpEngine);
    d->content_widget->setObjectName("Help Plugin: Content Widget");
    connect(d->content_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->content_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));

    // - Register Index Widget Factory
    d->index_widget = new IndexWidgetFactory(d->helpEngine);
    d->index_widget->setObjectName("Help Plugin: Index Widget");
    connect(d->index_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->index_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));

    // - Register Search Widget Factory
    d->search_widget = new SearchWidgetFactory(helpSearchEngine);
    d->search_widget->setObjectName("Help Plugin: Search Engine Widget");
    connect(d->search_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->search_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));

    d->view = new QWebView();
    d->view->setWindowTitle("QWebView Help");
    setCentralWidget(d->view);
    d->view->show();
}

bool Qtilities::Plugins::Help::HelpMode::eventFilter(QObject *object, QEvent *event) {
    if (object == d->side_viewer_dock && event->type() == QEvent::Close) {
        d->actionShowDock->setChecked(false);
    }

    return false;
}

void Qtilities::Plugins::Help::HelpMode::toggleDock(bool toggle) {
    if (toggle) {
        d->side_viewer_dock->show();
    } else {
        d->side_viewer_dock->hide();
    }
}

Qtilities::Plugins::Help::HelpMode::~HelpMode()
{
    delete d;
}

QWidget* Qtilities::Plugins::Help::HelpMode::widget() {
    return this;
}

void Qtilities::Plugins::Help::HelpMode::initialize() {
    if (d->initialized)
        return;

    // Check all objects in the global object pool.
    QMap<QString, ISideViewerWidget*> text_iface_map;
    QList<QObject*> widgets = OBJECT_MANAGER->registeredInterfaces("ISideViewerWidget");
    for (int i = 0; i < widgets.count(); i++) {
        ISideViewerWidget* side_viewer_widget = qobject_cast<ISideViewerWidget*> (widgets.at(i));
        if (side_viewer_widget) {
            text_iface_map[side_viewer_widget->widgetLabel()] = side_viewer_widget;
        }
    }

    d->side_viewer_widget->setIFaceMap(text_iface_map,true);
    d->side_viewer_widget->show();
    d->initialized = true;
}

QIcon Qtilities::Plugins::Help::HelpMode::icon() const {
    return QIcon(HELP_MODE_ICON_48x48);
}

QString Qtilities::Plugins::Help::HelpMode::text() const {
    return tr("Documentation");
}

void Qtilities::Plugins::Help::HelpMode::handleNewHelpWidget(QWidget* widget) {
    // Check which widget was created:
    QHelpContentWidget* content_widget = qobject_cast<QHelpContentWidget*> (widget);
    if (content_widget) {
        connect(content_widget,SIGNAL(linkActivated(const QUrl&)),d->help_browser,SLOT(setSource(const QUrl&)));
        connect(content_widget,SIGNAL(linkActivated(const QUrl&)),SLOT(updateUrl(QUrl)));
        return;
    }
    QHelpIndexWidget* index_widget = qobject_cast<QHelpIndexWidget*> (widget);
    if (index_widget) {
        connect(index_widget,SIGNAL(linkActivated(const QUrl&, const QString&)),d->help_browser,SLOT(setSource(const QUrl&)));
        connect(index_widget,SIGNAL(linkActivated(const QUrl&)),SLOT(updateUrl(QUrl)));
        return;
    }
}

void Qtilities::Plugins::Help::HelpMode::updateUrl(const QUrl& url) {
    //d->view->setHtml(d->helpEngine->fileData(url),QUrl("D:/Projects/Qt/Qtilities/trunk/doc/output/html_packages/"));
    QString relative_url = url.toString();
    QStringList item_list = relative_url.split("/");
    item_list.pop_back();
    QString path_url = item_list.join("/");
    path_url.append("/");
    d->view->setHtml(d->helpEngine->fileData(url),path_url);
}
