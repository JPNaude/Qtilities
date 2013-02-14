/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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
#include "ContentWidgetFactory.h"
#include "IndexWidgetFactory.h"
#include "SearchWidgetFactory.h"
#include "HelpPluginConstants.h"
#include "Browser.h"

#include <QtilitiesCoreApplication>

#include <QtGui>
#include <QtHelp>

#include <QByteArray>
#include <QHelpContentItem>
#include <QHelpContentModel>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QHelpIndexWidget>
#include <QHelpSearchEngine>
#include <QHelpSearchQueryWidget>
#include <QHelpSearchResultWidget>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QPointer>
#include <QtDebug>
#include <QTimer>
#include <QUrl>
#include <QWebPage>
#include <QWebView>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;
using namespace Qtilities::Plugins::Help::Constants;
using namespace Qtilities::Plugins::Help;

class qti_private_HelpNetworkAccessManager : public QNetworkAccessManager {
    public:
        qti_private_HelpNetworkAccessManager(QHelpEngineCore* helpEngine, QNetworkAccessManager *manager, QObject *parentObject) :
            QNetworkAccessManager(parentObject),
            d_help_engine(helpEngine)
        {
            Q_ASSERT(manager);
            Q_ASSERT(helpEngine);

            setCache(manager->cache());
            setProxy(manager->proxy());
            setProxyFactory(manager->proxyFactory());
            setCookieJar(manager->cookieJar());
        }

    protected:
        virtual QNetworkReply *createRequest(
            Operation operation, const QNetworkRequest &request, QIODevice *device) {
                if (request.url().scheme() == "qthelp" && operation == GetOperation)
                    return new qti_private_HelpNetworkReply(request.url(), d_help_engine);
                else
                    return QNetworkAccessManager::createRequest(operation, request, device);
            }

        QPointer<QHelpEngineCore>   d_help_engine;

    private:
        Q_DISABLE_COPY(qti_private_HelpNetworkAccessManager)
};


qti_private_HelpNetworkReply::qti_private_HelpNetworkReply(const QUrl& url, QHelpEngineCore* help_engine) : QNetworkReply(help_engine) {
    Q_ASSERT(help_engine);

    d_help_engine = help_engine;
    setUrl(url);

    QTimer::singleShot(0, this, SLOT(process()));
}

void qti_private_HelpNetworkReply::process() {
    if (d_help_engine) {
        QByteArray rawData = d_help_engine->fileData(url());
        d_buffer.setData(rawData);
        d_buffer.open(QIODevice::ReadOnly);

        open(QIODevice::ReadOnly|QIODevice::Unbuffered);
        setHeader(QNetworkRequest::ContentLengthHeader, QVariant(rawData.size()));
        setHeader(QNetworkRequest::ContentTypeHeader, "text/html");
        emit readyRead();
        emit finished();
    }
}

struct Qtilities::Plugins::Help::HelpModeData {
    HelpModeData() : initialized(false),
        side_viewer_dock(0),
        side_viewer_widget(0),
        actionShowDock(0),
        content_widget(0),
        index_widget(0),
        search_widget(0),
        browser(0) {}

    bool initialized;
    QDockWidget* side_viewer_dock;
    DynamicSideWidgetViewer* side_viewer_widget;
    QAction* actionShowDock;
    ContentWidgetFactory* content_widget;
    IndexWidgetFactory* index_widget;
    SearchWidgetFactory* search_widget;
    Browser* browser;
};

HelpMode::HelpMode(QWidget *parent) :
    QMainWindow(parent)
{
    d = new HelpModeData;
    setObjectName("Help Mode");
}

bool HelpMode::eventFilter(QObject *object, QEvent *event) {
    if (object == d->side_viewer_dock && event->type() == QEvent::Close) {
        d->actionShowDock->setChecked(false);
    }

    return false;
}

void HelpMode::initiallize() {
    // Create and dock the dynamic side widget viewer
    d->side_viewer_dock = new QDockWidget(tr("Help Widgets"));
    d->side_viewer_widget = new DynamicSideWidgetViewer(MODE_HELP_ID);
    connect(d->side_viewer_widget,SIGNAL(toggleVisibility(bool)),SLOT(toggleDock(bool)));
    d->side_viewer_dock->setWidget(d->side_viewer_widget);
    Qt::DockWidgetAreas allowed_areas = 0;
    allowed_areas |= Qt::LeftDockWidgetArea;
    allowed_areas |= Qt::RightDockWidgetArea;
    d->side_viewer_dock->setAllowedAreas(allowed_areas);
    addDockWidget(Qt::LeftDockWidgetArea,d->side_viewer_dock);
    d->side_viewer_dock->installEventFilter(this);

    // Actions
    d->actionShowDock = new QAction(QIcon(),tr("Help Widgets"),this);
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
        menu_bar->addMenu(view_menu,qti_action_ABOUT);
        view_menu->addAction(command);
    } else
        view_menu->addAction(command);

    d->browser = new Browser(QUrl(),this);
    setCentralWidget(d->browser);

    QNetworkAccessManager *current_manager = d->browser->webView()->page()->networkAccessManager();
    qti_private_HelpNetworkAccessManager* newManager = new qti_private_HelpNetworkAccessManager(HELP_MANAGER->helpEngine(), current_manager, this);
    d->browser->webView()->page()->setNetworkAccessManager(newManager);
    d->browser->webView()->page()->setForwardUnsupportedContent(false);

    // - Register Contents Widget Factory
    d->content_widget = new ContentWidgetFactory(HELP_MANAGER->helpEngine());
    d->content_widget->setObjectName("Help Plugin: Content Widget");
    d->content_widget->setObjectOriginID("Help Plugin");
    connect(d->content_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->content_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));

    // - Register Index Widget Factory
    d->index_widget = new IndexWidgetFactory(HELP_MANAGER->helpEngine());
    d->index_widget->setObjectName("Help Plugin: Index Widget");
    d->index_widget->setObjectOriginID("Help Plugin");
    connect(d->index_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->index_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));

    // - Register Search Widget Factory
    QHelpSearchEngine* helpSearchEngine = HELP_MANAGER->helpEngine()->searchEngine();
    d->search_widget = new SearchWidgetFactory(helpSearchEngine);
    d->search_widget->setObjectOriginID("Help Plugin");
    d->search_widget->setObjectName("Help Plugin: Search Engine Widget");
    connect(d->search_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->search_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));

    // Load the home page:
    if (HELP_MANAGER->homePage().isValid())
        d->browser->webView()->load(HELP_MANAGER->homePage());

    connect(HELP_MANAGER,SIGNAL(forwardRequestUrlDisplay(QUrl,bool)),SLOT(handleUrlRequest(QUrl,bool)),Qt::UniqueConnection);
}

void HelpMode::toggleDock(bool toggle) {
    if (toggle) {
        d->side_viewer_dock->show();
    } else {
        d->side_viewer_dock->hide();
    }
}

HelpMode::~HelpMode() {
    delete d;
}

QWidget* HelpMode::modeWidget() {
    return this;
}

void HelpMode::initializeMode() {
    if (d->initialized)
        return;

    // Check all objects in the global object pool.
    QMap<QString, ISideViewerWidget*> text_iface_map;
    QList<QObject*> widgets = OBJECT_MANAGER->registeredInterfaces("ISideViewerWidget");
    for (int i = 0; i < widgets.count(); ++i) {
        ISideViewerWidget* side_viewer_widget = qobject_cast<ISideViewerWidget*> (widgets.at(i));
        if (side_viewer_widget) {
            text_iface_map[side_viewer_widget->widgetLabel()] = side_viewer_widget;
        }
    }

    d->side_viewer_widget->setIFaceMap(text_iface_map,true);
    d->initialized = true;
}

QIcon HelpMode::modeIcon() const {
    return QIcon(HELP_MODE_ICON_48x48);
}

QString HelpMode::modeName() const {
    return tr("Help");
}

void HelpMode::handleNewHelpWidget(QWidget* widget) {
    // Check which widget was created:
    QHelpContentWidget* content_widget = qobject_cast<QHelpContentWidget*> (widget);
    if (content_widget) {
        connect(content_widget,SIGNAL(linkActivated(const QUrl&)),SLOT(handleUrl(const QUrl&)),Qt::UniqueConnection);
        return;
    }
    QHelpIndexWidget* index_widget = qobject_cast<QHelpIndexWidget*> (widget);
    if (index_widget) {
        connect(index_widget,SIGNAL(linkActivated(const QUrl&, const QString&)),SLOT(handleUrl(const QUrl&)),Qt::UniqueConnection);
        return;
    }
    QHelpSearchResultWidget* result_widget = qobject_cast<QHelpSearchResultWidget*> (widget);
    if (result_widget) {
        connect(result_widget,SIGNAL(requestShowLink(QUrl)),SLOT(handleUrl(const QUrl&)),Qt::UniqueConnection);
        return;
    }
}

void HelpMode::handleUrl(const QUrl& url) {
    d->browser->webView()->load(url);
}

void HelpMode::handleUrlRequest(const QUrl &url, bool ensure_visible) {
    handleUrl(url);
    if (ensure_visible) {
        QtilitiesMainWindow* main_window = qobject_cast<QtilitiesMainWindow*> (QtilitiesApplication::mainWindow());
        if (main_window) {
            if (main_window->modeManager()) {
                main_window->modeManager()->setActiveMode(modeName());
            }
        }
    }
}
