/****************************************************************************
**
** Copyright 2010-2013, CSIR
** Author: JP Naude, jpnaude@csir.co.za
**
****************************************************************************/

#include "Browser.h"

#include <QtWebKit>
#include <QtGui>
#include <QtNetwork>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

struct Qtilities::Plugins::Help::BrowserData {
    BrowserData() : web_view(0),
        btnSearch(0),
        btnBack(0),
        btnForward(0),
        btnReload(0),
        locationBar(0),
        txtLocationEdit(0) {}

    QPointer<QWebView> web_view;
    QPushButton* btnSearch;
    QToolButton* btnBack;
    QToolButton* btnForward;
    QPushButton* btnReload;
    QWidget* locationBar;
    QLineEdit* txtLocationEdit;
    SearchBoxWidget* searchBoxWidget;
    QString errorMsg;
};

Qtilities::Plugins::Help::Browser::Browser(const QUrl &url, QWidget* parent) : QWidget(parent) {
    d = new BrowserData;

    //----------------------
    // Init d pointers
    //----------------------
    d->btnBack = new QToolButton;
    d->btnForward = new QToolButton;
    d->btnReload = new QPushButton;
    d->locationBar = new QWidget;
    d->txtLocationEdit = new QLineEdit;
    d->txtLocationEdit->setReadOnly(true);

    d->web_view = new QWebView;
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    d->web_view->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->web_view->settings()->globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    loadUrl(url);

    //----------------------
    // Set Button Attributes
    //----------------------
    d->btnBack->setText(tr("Back"));
    d->btnBack->setArrowType(Qt::LeftArrow);
    d->btnBack->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->btnForward->setText(tr("Forward"));
    d->btnForward->setArrowType(Qt::RightArrow);
    d->btnForward->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->btnReload->setText(tr("Reload"));
    d->btnReload->setIcon(QIcon(qti_icon_REFRESH_16x16));

    //----------------------
    // Searching
    //----------------------
    SearchBoxWidget::SearchOptions search_options = 0;
    search_options |= SearchBoxWidget::CaseSensitive;
    SearchBoxWidget::ButtonFlags button_flags = 0;
    button_flags |= SearchBoxWidget::HideButtonUp;
    button_flags |= SearchBoxWidget::NextButtons;
    button_flags |= SearchBoxWidget::PreviousButtons;
    d->searchBoxWidget = new SearchBoxWidget(search_options,SearchBoxWidget::SearchOnly,button_flags);
    connect(d->searchBoxWidget,SIGNAL(btnClose_clicked()),SLOT(handleSearchClose()));
    d->searchBoxWidget->hide();

    d->btnSearch = new QPushButton;
    d->btnSearch->setIcon(QIcon(qti_icon_FIND_16x16));
    connect(d->btnSearch,SIGNAL(clicked()),SLOT(showSearchBox()));
    connect(d->searchBoxWidget,SIGNAL(searchStringChanged(QString)),SLOT(handleSearchStringChanged(QString)));
    connect(d->searchBoxWidget,SIGNAL(searchOptionsChanged()),SLOT(handleSearchOptionsChanged()));

    //----------------------
    // Config SearchBar Layout
    //----------------------
    if (d->locationBar->layout())
        delete d->locationBar->layout();

    QHBoxLayout* Hlayout = new QHBoxLayout;
    Hlayout->setMargin(0);
    Hlayout->addWidget(d->btnBack);
    Hlayout->addWidget(d->btnForward);
    Hlayout->addWidget(d->btnReload);
    Hlayout->addWidget(d->btnSearch);
    Hlayout->addWidget(d->txtLocationEdit);
    d->locationBar->setLayout(Hlayout);
    d->locationBar->setFixedHeight(25);

    //----------------------
    // Config Documentation Tab Layout
    //----------------------
    if (layout())
        delete layout();

    QVBoxLayout* Vlayout = new QVBoxLayout(this);
    Vlayout->setMargin(0);
    Vlayout->setSpacing(0);
    Vlayout->addWidget(d->locationBar);
    Vlayout->addWidget(d->searchBoxWidget);
    Vlayout->addWidget(d->web_view);

    //----------------------
    // Connect SearchBar to WebView
    //----------------------
    connect(d->btnBack,SIGNAL(clicked()),d->web_view,SLOT(back()));
    connect(d->btnForward,SIGNAL(clicked()),d->web_view,SLOT(forward()));
    connect(d->btnReload,SIGNAL(clicked()),d->web_view,SLOT(reload()));
    connect(d->txtLocationEdit,SIGNAL(returnPressed()),this,SLOT(handle_loadUrl()));
    connect(d->web_view,SIGNAL(urlChanged(QUrl)),this,SLOT(handle_urlChanged(QUrl)));
    connect(d->web_view,SIGNAL(loadFinished(bool)),this,SLOT(handle_finished(bool)));
    d->web_view->show();

    // The error message:
    d->errorMsg = QString("<html><body><h1>Error, could not load the page you requested.</h1></body></html>");
}

Qtilities::Plugins::Help::Browser::~Browser() {
    delete d;
}

void Qtilities::Plugins::Help::Browser::loadUrl(const QUrl &url) {
    d->web_view->load(url);
}

QWebView *Plugins::Help::Browser::webView() {
    return d->web_view;
}

void Qtilities::Plugins::Help::Browser::handle_loadUrl( ){
    connect(d->web_view,SIGNAL(loadProgress(int)),SLOT(handleLoadProgress(int)));
    loadUrl(QUrl(d->txtLocationEdit->text()));
    d->locationBar->setEnabled(false);

    QUrl new_url(d->txtLocationEdit->text());

    if (new_url.scheme() == "https") {
        #ifdef Q_OS_WIN
        if (!QSslSocket::supportsSsl()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Failed To Load Webpage"));
            msgBox.setText(tr("Your system and/or Qt installation does not support websites using SSL (thus https://).\n\nThe page you are trying to load uses SSL and therefore cannot be loaded:\n") + d->web_view->url().toString());
            msgBox.exec();
            LOG_ERROR("Error while loading secure page at " + d->web_view->url().toString() + ". SSL is not supported.");
            return;
        }
        #else
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Failed To Load Webpage");
        msgBox.setText(tr("Your system and/or Qt installation does not seem to support websites using SSL (thus https://).\n\nThe page you are trying to load uses SSL and therefore cannot be loaded:\n)" + d->web_view->url().toString());
        msgBox.exec();
        LOG_ERROR("Error while loading secure page at " + d->web_view->url().toString() + ". SSL is not supported.");
        return;
        #endif
    }

    emit newUrlEntered(new_url);
}

void Qtilities::Plugins::Help::Browser::handle_urlChanged(QUrl url){
    d->txtLocationEdit->setText(url.toString());
}

void Qtilities::Plugins::Help::Browser::handle_finished(bool ok){
    if (!d->web_view)
        return;

    if (!ok) {
        if (!d->web_view->url().toString().isEmpty())
            LOG_ERROR("Error while loading page at " + d->web_view->url().toString());
    }

    d->locationBar->setEnabled(true);
}

void Qtilities::Plugins::Help::Browser::showSearchBox() {
    d->searchBoxWidget->setEditorFocus();

    if (!d->searchBoxWidget->isVisible())
        d->searchBoxWidget->show();
    else
        d->searchBoxWidget->hide();
}

void Qtilities::Plugins::Help::Browser::handleSearchStringChanged(const QString& search_string) {
    QWebPage::FindFlags find_flags = 0;

    if (d->searchBoxWidget->caseSensitive())
        find_flags |= QWebPage::FindCaseSensitively;

    find_flags |= QWebPage::FindWrapsAroundDocument;
    //find_flags |= QWebPage::HighlightAllOccurrences;

    d->web_view->findText(search_string,find_flags);
}

void Qtilities::Plugins::Help::Browser::handleSearchOptionsChanged() {
    d->web_view->findText(QString());
    handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
}

void Qtilities::Plugins::Help::Browser::handleSearchForward() {
    handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
}

void Qtilities::Plugins::Help::Browser::handleSearchBackwards() {
    QWebPage::FindFlags find_flags = 0;

    if (d->searchBoxWidget->caseSensitive())
        find_flags |= QWebPage::FindCaseSensitively;

    find_flags |= QWebPage::FindWrapsAroundDocument;
    find_flags |= QWebPage::HighlightAllOccurrences;
    find_flags |= QWebPage::FindBackward;

    d->web_view->findText(d->searchBoxWidget->currentSearchString(),find_flags);
}

void Qtilities::Plugins::Help::Browser::handleSearchClose() {
    d->searchBoxWidget->close();
    d->web_view->findText(QString());
}

void Qtilities::Plugins::Help::Browser::handleLoadProgress(int value) {
    Q_UNUSED(value);
    //if (value < 100 && d->web_view->url().scheme() == "http")
    //    LOG_INFO(tr("Loading page at ") + d->web_view->url().toString() + ": " + QString::number(value) + "%");
}
