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

#ifndef BROWSER_H
#define BROWSER_H

#include <IMode>
#include <QtWebKit>

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
