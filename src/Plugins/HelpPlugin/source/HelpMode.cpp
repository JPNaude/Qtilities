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

#include <QtilitiesCoreApplication>

#include <QtGui>
#include <QtHelp>

#include <QtWebKit/QWebView>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;
using namespace Qtilities::Plugins::Help::Constants;

//struct ExtensionMap {
//    const char *extension;
//    const char *mimeType;
//} extensionMap[] = {
//    { ".bmp", "image/bmp" },
//    { ".css", "text/css" },
//    { ".gif", "image/gif" },
//    { ".html", "text/html" },
//    { ".htm", "text/html" },
//    { ".ico", "image/x-icon" },
//    { ".jpeg", "image/jpeg" },
//    { ".jpg", "image/jpeg" },
//    { ".js", "application/x-javascript" },
//    { ".mng", "video/x-mng" },
//    { ".pbm", "image/x-portable-bitmap" },
//    { ".pgm", "image/x-portable-graymap" },
//    { ".pdf", "application/pdf" },
//    { ".png", "image/png" },
//    { ".ppm", "image/x-portable-pixmap" },
//    { ".rss", "application/rss+xml" },
//    { ".svg", "image/svg+xml" },
//    { ".svgz", "image/svg+xml" },
//    { ".text", "text/plain" },
//    { ".tif", "image/tiff" },
//    { ".tiff", "image/tiff" },
//    { ".txt", "text/plain" },
//    { ".xbm", "image/x-xbitmap" },
//    { ".xml", "text/xml" },
//    { ".xpm", "image/x-xpm" },
//    { ".xsl", "text/xsl" },
//    { ".xhtml", "application/xhtml+xml" },
//    { ".wml", "text/vnd.wap.wml" },
//    { ".wmlc", "application/vnd.wap.wmlc" },
//    { "about:blank", 0 },
//    { 0, 0 }
//};

//void NetworkAccessManager::getUrl(const QUrl &url)
//{
//    QNetworkRequest req;
//    req.setUrl(url);
//    get(req);
//}

//QNetworkReply* NetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
//{
////    QString agentStr = QString::fromLatin1("Qt-Creator/%1 (QNetworkAccessManager %2; %3; %4; %5 bit)")
////            .arg(QLatin1String("Test",
////                         QLatin1String(qVersion()),
////                         getOsString(), QLocale::system().name())
////                    .arg(QSysInfo::WordSize);
//    QNetworkRequest req(request);
//    req.setRawHeader("User-Agent", "agentStr.toLatin1()");
//    return QNetworkAccessManager::createRequest(op, req, outgoingData);
//}


//// -- HelpNetworkReply

//class HelpNetworkReply : public QNetworkReply
//{
//public:
//    HelpNetworkReply(const QNetworkRequest &request, const QByteArray &fileData,
//        const QString &mimeType);

//    virtual void abort() {}

//    virtual qint64 bytesAvailable() const
//        { return data.length() + QNetworkReply::bytesAvailable(); }

//protected:
//    virtual qint64 readData(char *data, qint64 maxlen);

//private:
//    QByteArray data;
//    qint64 dataLength;
//};

//HelpNetworkReply::HelpNetworkReply(const QNetworkRequest &request,
//        const QByteArray &fileData, const QString& mimeType)
//    : data(fileData)
//    , dataLength(fileData.length())
//{
//    setRequest(request);
//    setOpenMode(QIODevice::ReadOnly);

//    setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
//    setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(dataLength));
//    QTimer::singleShot(0, this, SIGNAL(metaDataChanged()));
//    QTimer::singleShot(0, this, SIGNAL(readyRead()));
//}

//qint64 HelpNetworkReply::readData(char *buffer, qint64 maxlen)
//{
//    qint64 len = qMin(qint64(data.length()), maxlen);
//    if (len) {
//        qMemCopy(buffer, data.constData(), len);
//        data.remove(0, len);
//    }
//    if (!data.length())
//        QTimer::singleShot(0, this, SIGNAL(finished()));
//    return len;
//}

//QNetworkReply *HelpNetworkAccessManager::createRequest(Operation op,
//    const QNetworkRequest &request, QIODevice* outgoingData)
//{
//    //if (!HelpViewer::isLocalUrl(request.url()))
//    //    return NetworkAccessManager::createRequest(op, request, outgoingData);

//    QString url = request.url().toString();
//    QHelpEngineCore* engine = HELP_MANAGER->helpEngine();
//    // TODO: For some reason the url to load is already wrong (passed from webkit)
//    // though the css file and the references inside should work that way. One
//    // possible problem might be that the css is loaded at the same level as the
//    // html, thus a path inside the css like (../images/foo.png) might cd out of
//    // the virtual folder
////    if (!engine.findFile(url).isValid()) {
////        if (url.startsWith(HelpViewer::NsNokia) || url.startsWith(HelpViewer::NsTrolltech)) {
////            QUrl newUrl = request.url();
////            if (!newUrl.path().startsWith(QLatin1String("/qdoc/"))) {
////                newUrl.setPath(QLatin1String("/qdoc/") + newUrl.path());
////                url = newUrl.toString();
////            }
////        }
////    }

//    const QString &path = QUrl(url).path();
//    const int index = path.lastIndexOf(QLatin1Char('.'));
//    const QByteArray &ext = path.mid(index).toUtf8().toLower();

//    const ExtensionMap *e = extensionMap;
//    QString mimeType;
//    while (e->extension) {
//        if (ext == e->extension) {
//            mimeType = QLatin1String(e->mimeType);
//            break;
//        }
//        ++e;
//    }
//    mimeType = QLatin1String("");

//    const QByteArray &data = engine->fileData(url);

//    return new HelpNetworkReply(request, data, mimeType.isEmpty()
//        ? QLatin1String("application/octet-stream") : mimeType);
//}

//// -------------------------------------------------------



//HelpPage::HelpPage(QObject *parent)
//    : QWebPage(parent)
//    , closeNewTabIfNeeded(false)
//    , m_pressedButtons(Qt::NoButton)
//    , m_keyboardModifiers(Qt::NoModifier)
//{
//}

//QWebPage *HelpPage::createWindow(QWebPage::WebWindowType)
//{
////    HelpPage* newPage = static_cast<HelpPage*>(OpenPagesManager::instance()
////        .createPage()->page());
//    HelpPage* newPage = new HelpPage(this);
//    newPage->closeNewTabIfNeeded = closeNewTabIfNeeded;
//    closeNewTabIfNeeded = false;
//    return newPage;
//}

//void HelpPage::triggerAction(WebAction action, bool checked)
//{
//    switch (action) {
//        case OpenLinkInNewWindow:
//            closeNewTabIfNeeded = true;
//        default:        // fall through
//            QWebPage::triggerAction(action, checked);
//            break;
//    }
//}

//bool HelpPage::acceptNavigationRequest(QWebFrame *,
//    const QNetworkRequest &request, QWebPage::NavigationType type)
//{
//    const bool closeNewTab = closeNewTabIfNeeded;
//    closeNewTabIfNeeded = false;

////    const QUrl &url = request.url();
////    if (HelpViewer::launchWithExternalApp(url)) {
////        if (closeNewTab)
////            QMetaObject::invokeMethod(&OpenPagesManager::instance(), "closeCurrentPage");
////        return false;
////    }

//    if (type == QWebPage::NavigationTypeLinkClicked
//        && (m_keyboardModifiers & Qt::ControlModifier || m_pressedButtons == Qt::MidButton)) {
//            m_pressedButtons = Qt::NoButton;
//            m_keyboardModifiers = Qt::NoModifier;
////            OpenPagesManager::instance().createPage(url);
//            return false;
//    }

//    return true;
//}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

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

    QWebView* web;
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

    d->web = new QWebView();

//    d->web->setPage(new HelpPage(this));

//    HelpNetworkAccessManager *manager = new HelpNetworkAccessManager(this);
//    d->web->page()->setNetworkAccessManager(manager);
//    connect(manager, SIGNAL(finished(QNetworkReply*)), this,
//        SLOT(slotNetworkReplyFinished(QNetworkReply*)));

//    QNetworkProxyFactory::setUseSystemConfiguration(true);
//    d->web->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    setCentralWidget(d->web);
//    connect(d->web,SIGNAL(loadFinished(bool)),SLOT(handleBrowserLoad(bool)));
//    d->web->load(QUrl("http://www.google.com"));
//    d->web->show();

    // - Help Browser
    d->help_browser = new HelpBrowser(HELP_MANAGER->helpEngine());
    setCentralWidget(d->help_browser);
    d->help_browser->show();

    // - Register Contents Widget Factory
    d->content_widget = new ContentWidgetFactory(HELP_MANAGER->helpEngine());
    d->content_widget->setObjectName("Help Plugin: Content Widget");
    connect(d->content_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->content_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));

    // - Register Index Widget Factory
    d->index_widget = new IndexWidgetFactory(HELP_MANAGER->helpEngine());
    d->index_widget->setObjectName("Help Plugin: Index Widget");
    connect(d->index_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->index_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));

    // - Register Search Widget Factory
    QHelpSearchEngine* helpSearchEngine = HELP_MANAGER->helpEngine()->searchEngine();
    d->search_widget = new SearchWidgetFactory(helpSearchEngine);
    d->search_widget->setObjectName("Help Plugin: Search Engine Widget");
    connect(d->search_widget,SIGNAL(newWidgetCreated(QWidget*)),SLOT(handleNewHelpWidget(QWidget*)));
    OBJECT_MANAGER->registerObject(d->search_widget,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));
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
    d->web->load(url);
}


