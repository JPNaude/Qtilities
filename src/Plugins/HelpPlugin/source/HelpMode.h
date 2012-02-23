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
#include <QtNetwork/QNetworkReply>
#include <QBuffer>
#include <QPointer>

namespace Ui {
    class HelpMode;
}

class QHelpEngine;
class QHelpEngineCore;
class QUrl;
class QWebView;

namespace Qtilities {
    namespace Plugins {
        namespace Help {
            using namespace Qtilities::CoreGui::Interfaces;

            class qti_private_HelpNetworkReply : public QNetworkReply
            {
                Q_OBJECT
             public:
                qti_private_HelpNetworkReply(const QUrl& url, QHelpEngineCore* helpEngine);

                virtual void abort() {}
                virtual qint64 bytesAvailable() const {
                    return d_buffer.bytesAvailable();
                }
                virtual bool isSequential() const {
                    return d_buffer.isSequential();
                }

            private slots:
                void process();

            protected:
                virtual qint64 readData(char *data, qint64 maxSize) {
                    return d_buffer.read(data, maxSize);
                }

                QPointer<QHelpEngineCore>   d_help_engine;
                QBuffer                     d_buffer;

            private:
                Q_DISABLE_COPY(qti_private_HelpNetworkReply)
            };


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
            \brief An implementation of Qtilities::CoreGui::Interfaces::IMode which make a GUI frontend for the \p HELP_MANAGER available as a mode in an application.
              */
            class HelpMode : public QMainWindow, public IMode {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IMode)
            public:
                HelpMode(QWidget *parent = 0);
                ~HelpMode();
                bool eventFilter(QObject *object, QEvent *event);

                //! Initializes the help mode.
                void initiallize();

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
                //! Display the page at \p url.
                void handleUrl(const QUrl& url);
                //! Handle requests from the help manager to display the url.
                void handleUrlRequest(const QUrl& url, bool ensure_visible);

            private:
                HelpModeData* d;
            };
        }
    }
}

#endif // HELP_MODE_H
