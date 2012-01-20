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

#ifndef HELP_MODE_H
#define HELP_MODE_H

#include <IMode.h>

#include <QMainWindow>
#include <QUrl>

#include <QNetworkProxyFactory>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QWebPage>

namespace Ui {
    class HelpMode;
}

// -- NetworkAccessManager

//class NetworkAccessManager : public QNetworkAccessManager
//{
//    Q_OBJECT
//public:
//    NetworkAccessManager(QObject *parent = 0) : QNetworkAccessManager(parent) {}

//public slots:
//    void getUrl(const QUrl &url);

//protected:
//    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
//};


//// -- HelpNetworkAccessManager

//class HelpNetworkAccessManager : public NetworkAccessManager
//{
//public:
//    HelpNetworkAccessManager(QObject *parent) : NetworkAccessManager(parent) {}

//protected:
//    virtual QNetworkReply *createRequest(Operation op,
//        const QNetworkRequest &request, QIODevice *outgoingData = 0);
//};

//// -- HelpPage

//class HelpPage : public QWebPage
//{
//public:
//    HelpPage(QObject *parent);

//protected:
//    virtual QWebPage *createWindow(QWebPage::WebWindowType);
//    virtual void triggerAction(WebAction action, bool checked = false);

//    virtual bool acceptNavigationRequest(QWebFrame *frame,
//        const QNetworkRequest &request, NavigationType type);

//private:
//    bool closeNewTabIfNeeded;

//    //friend class Help::Internal::HelpViewer;
//    Qt::MouseButtons m_pressedButtons;
//    Qt::KeyboardModifiers m_keyboardModifiers;
//};

namespace Qtilities {
    namespace Plugins {
        //! Namespace containing constants defined in the Help plugin.
        namespace Help {
            using namespace Qtilities::CoreGui::Interfaces;

            // Help Mode Parameters
            #define MODE_HELP_ID                   997
            const char * const CONTEXT_HELP_MODE   = "Context.HelpMode";

            /*!
              \struct HelpModeData
              \brief The HelpModeData class stores private data used by the HelpMode class.
             */
            struct HelpModeData;

            /*!
            \class HelpMode
            \brief An example mode widget which demonstrates the dynamic side widget architecture..
              */
            class HelpMode : public QMainWindow, public IMode {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IMode)
            public:
                HelpMode(QWidget *parent = 0);
                ~HelpMode();
                bool eventFilter(QObject *object, QEvent *event);

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------------------
                // IMode Implementation
                // --------------------------------------------
                QWidget* modeWidget();
                void initializeMode();
                QIcon modeIcon() const;
                QString modeName() const;
                QString contextString() const { return CONTEXT_HELP_MODE; }
                QString contextHelpId() const { return QString(); }
                int modeID() const { return MODE_HELP_ID; }

            public slots:
                //! Toggles the visibility of the dynamic help dock widget.
                void toggleDock(bool toggle);
                //! Handles a new help widget. This function makes the neccessary connections.
                void handleNewHelpWidget(QWidget* widget);
                //! Handles QUrl requests from the help side widgets.
                void handleUrl(const QUrl& url);

            private:
                HelpModeData* d;
            };
        }
    }
}

#endif // HELP_MODE_H
