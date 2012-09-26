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

#ifndef WIDGETLOGGERENGINE_H
#define WIDGETLOGGERENGINE_H

#include "QtilitiesCoreGui_global.h"

#include <AbstractLoggerEngine.h>
#include <Factory.h>

#include <QList>
#include <QVariant>
#include <QPlainTextEdit>

class QWidget;

namespace Ui {
    class WidgetLoggerEngine;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Logging;
        using namespace Qtilities::Core;

        // ------------------------------------
        // Widget Logger Engine
        // ------------------------------------
        /*!
        \struct WidgetLoggerEnginePrivateData
        \brief A struct used by the WidgetLoggerEngine class to store data.
          */
        struct WidgetLoggerEnginePrivateData;

        /*!
        \class WidgetLoggerEngine
        \brief A logger engine which shows logged messages in a widget with a QPlainTextEdit widget.

        A logger engine which shows logged messages in a widget with a QPlainTextEdit widget.

        \note Clearing the log through clearLog() is supported by this logger engine.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT WidgetLoggerEngine : public AbstractLoggerEngine
        {
            Q_OBJECT

        public:
            //! This enumeration provides the possible ways that messages can be displayed in the WidgetLoggerEngineFrontend used by this engine.
            /*!
              The default is DefaultDisplays.

              \note When only one message display is used in the engine, that display will not be tabbed.
              */
            enum MessageDisplays {
                NoMessageDisplays           = 0,   /*!< No message displays. */
                AllMessagesPlainTextEdit    = 1,   /*!< Displays all messages under an "All Messages" tab using a QPlainTextEdit. */
                IssuesPlainTextEdit         = 2,   /*!< Displays all issues (warnings, errors etc.) under an "Issues" tab using a QPlainTextEdit. */
                WarningsPlainTextEdit       = 4,   /*!< Displays all warnings under a "Warnings" tab using a QPlainTextEdit. */
                ErrorsPlainTextEdit         = 8,   /*!< Displays all errors under a "Errors" tab using a QPlainTextEdit. */
//                AllMessagesListWidget       = 16,  /*!< Displays all messages under an "All Messages" tab using a QListView. */
//                IssuesListWidget            = 32,  /*!< Displays all issues (warnings, errors etc.) under an "Issues" tab using a QListView. */
//                WarningsListWidget          = 64,  /*!< Displays all warnings under a "Warnings" tab using a QListView. */
//                ErrorsListWidget            = 128  /*!< Displays all errors under a "Errors" tab using a QListView. */
                DefaultDisplays             = AllMessagesPlainTextEdit,
                DefaultTaskDisplays         = AllMessagesPlainTextEdit | IssuesPlainTextEdit
            };
            Q_ENUMS(MessageDisplays);
            Q_DECLARE_FLAGS(MessageDisplaysFlag, MessageDisplays);
            Q_FLAGS(MessageDisplaysFlag);

            WidgetLoggerEngine(MessageDisplaysFlag message_displays_flag = (MessageDisplaysFlag) (DefaultDisplays));
            ~WidgetLoggerEngine();

            //! Sets the window title used for this logger engine.
            /*!
              \note This function only does something usefull after the engine has been initialized.
              */
            void setWindowTitle(const QString& window_title);
            //! Gets the window title used for this logger engine.
            /*!
              \note This function only does something usefull after the engine has been initialized.
              */
            QString windowTitle() const;

            // AbstractLoggerEngine implementation
            bool initialize();
            void finalize();
            bool isInitialized() const;
            QString description() const;
            QString status() const;      
            // Logger widgets will always be created after LOG_INITIALIZE(), thus we won't be able to restore their settings.
            bool isFormattingEngineConstant() const { return false; }

            // WidgetLoggerEngine implementation
            QWidget* getWidget();

            // Make this class a factory item
            static Qtilities::CoreGui::FactoryItem<AbstractLoggerEngine, WidgetLoggerEngine> factory;

            //! Returns the QPlainTextEdit used by this widget logger engine. Through this reference you can add your own custom syntax highligter etc.
            /*!
              \note Only available when your MessageDisplayFlags includes MessagesPlainTextEdit.
              */
            QPlainTextEdit* plainTextEdit(MessageDisplaysFlag message_display) const;

        public slots:
            void logMessage(const QString& message, Logger::MessageType message_type);
            void clearLog();

        private:
            WidgetLoggerEnginePrivateData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(WidgetLoggerEngine::MessageDisplaysFlag)
    }
}


#endif // WIDGETLOGGERENGINE_H
