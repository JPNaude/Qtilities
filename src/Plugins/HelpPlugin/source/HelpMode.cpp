/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

    QHelpEngine* helpEngine;
    //QWebView* web;
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
    context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));
    Command* command = ACTION_MANAGER->registerAction("HelpMode.DynamicDockWidget",d->actionShowDock,context);
    bool existed;
    ActionContainer* view_menu = ACTION_MANAGER->createMenu(qti_action_VIEW,existed);
    if (!existed) {
        ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(qti_action_MENUBAR_STANDARD,existed);
        menu_bar->addMenu(view_menu);
        view_menu->addAction(command,"Help");
    } else
        view_menu->addAction(command);

    // Delete the current help collection file:
    QFile::remove(QtilitiesApplication::applicationSessionPath() + "/help_collection.qhc");
    d->helpEngine = new QHelpEngine(QtilitiesApplication::applicationSessionPath() + "/help_collection.qhc",this);
    connect(d->helpEngine,SIGNAL(warning(QString)),SLOT(logMessage(QString)));

//    d->web = new QWebView(this);
//    setCentralWidget(d->web);
//    connect(d->web,SIGNAL(loadFinished(bool)),SLOT(handleBrowserLoad(bool)));
//    d->web->load(QUrl("http://qt.nokia.com/"));
//    d->web->show();

    // Search engine indexing:
    Task* indexing = new Task("Indexing Registered Documentation");
    indexing->setTaskLifeTimeFlags(Task::LifeTimeDestroyWhenSuccessful);
    OBJECT_MANAGER->registerObject(indexing);
    connect(d->helpEngine->searchEngine(),SIGNAL(indexingStarted()),indexing,SLOT(startTask()));
    connect(d->helpEngine->searchEngine(),SIGNAL(indexingFinished()),indexing,SLOT(completeTask()));

    if (!d->helpEngine->setupData())
        LOG_ERROR("Failed to setup help engine: " + d->helpEngine->error());

    // Register the correct documentation:
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Help");
    QStringList files = settings.value("registered_files").toStringList();
    settings.endGroup();
    settings.endGroup();

    foreach (QString file, files) {
        qDebug() << file;
        if (!d->helpEngine->registerDocumentation(file))
            LOG_ERROR(tr("Failed to register documentation from file: ") + file);
        else
            LOG_INFO(tr("Successfully registered documentation from file: ") + file);
    }

    // - Help Browser
    d->help_browser = new HelpBrowser(d->helpEngine);
    setCentralWidget(d->help_browser);
    d->help_browser->show();

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
    QHelpSearchEngine* helpSearchEngine = d->helpEngine->searchEngine();
    d->search_widget = new SearchWidgetFactory(helpSearchEngine);
    d->search_widget->setObjectName("Help Plugin: Search Engine Widget");
    connect(d->search_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->search_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));
}

void Qtilities::Plugins::Help::HelpMode::logMessage(const QString& message) {
    LOG_WARNING(message);
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

Qtilities::Plugins::Help::HelpMode::~HelpMode() {
    delete d;
}

QWidget* Qtilities::Plugins::Help::HelpMode::modeWidget() {
    return this;
}

void Qtilities::Plugins::Help::HelpMode::initializeMode() {
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

QIcon Qtilities::Plugins::Help::HelpMode::modeIcon() const {
    return QIcon(HELP_MODE_ICON_48x48);
}

QString Qtilities::Plugins::Help::HelpMode::modeName() const {
    return tr("Documentation");
}

void Qtilities::Plugins::Help::HelpMode::handleNewHelpWidget(QWidget* widget) {
    // Check which widget was created:
    QHelpContentWidget* content_widget = qobject_cast<QHelpContentWidget*> (widget);
    if (content_widget) {
        connect(content_widget,SIGNAL(linkActivated(const QUrl&)),d->help_browser,SLOT(setSource(const QUrl&)),Qt::UniqueConnection);
        connect(content_widget,SIGNAL(linkActivated(const QUrl&)),SLOT(handleUrl(const QUrl&)),Qt::UniqueConnection);
        return;
    }
    QHelpIndexWidget* index_widget = qobject_cast<QHelpIndexWidget*> (widget);
    if (index_widget) {
        connect(index_widget,SIGNAL(linkActivated(const QUrl&, const QString&)),d->help_browser,SLOT(setSource(const QUrl&)),Qt::UniqueConnection);
        return;
    }
    QHelpSearchResultWidget* result_widget = qobject_cast<QHelpSearchResultWidget*> (widget);
    if (result_widget) {
        connect(result_widget,SIGNAL(requestShowLink(QUrl)),d->help_browser,SLOT(setSource(const QUrl&)),Qt::UniqueConnection);
        return;
    }
}

void Qtilities::Plugins::Help::HelpMode::handleUrl(const QUrl& url) {
    //d->web->load(QUrl("http:://www.google.com"));
}


