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

#ifndef BROWSER_H
#define BROWSER_H

#include <IMode>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtWebKit>
#else
#include <QtWebKitWidgets>
#endif

namespace Qtilities {
    namespace Plugins {
        namespace Help {
            /*!
              \struct BrowserData
              \brief The BrowserData struct stores private data used by the Browser class.
             */
            struct BrowserData;

            /*!
            \class Browser
            \brief A very simple browser wrapper around QWebView used to display help pages.
              */
            class Browser : public QWidget
            {
                Q_OBJECT
                public:
                    Browser(const QUrl &url = QUrl(), QWidget* parent = 0);
                    ~Browser();

                    //! Loads the specified url.
                    void loadUrl(const QUrl &url = QUrl());

                    //! Returns a pointer to the contained QWebView.
                    QWebView* webView();

                private slots:
                    void handle_loadUrl();
                    void handle_urlChanged(QUrl url);
                    void handle_finished(bool ok);
                    void showSearchBox();
                    void handleSearchStringChanged(const QString& search_string);
                    void handleSearchOptionsChanged();
                    void handleSearchForward();
                    void handleSearchBackwards();
                    void handleSearchClose();
                    void handleLoadProgress(int value);

                signals:
                    void newUrlEntered(const QUrl& url);

                private:
                    BrowserData* d;
            };
        }
    }
}

#endif // BROWSER_H
