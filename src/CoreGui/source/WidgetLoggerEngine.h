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
            Q_ENUMS(MessageDisplays)
            Q_DECLARE_FLAGS(MessageDisplaysFlag, MessageDisplays)
            Q_FLAGS(MessageDisplaysFlag)

            //! Constructor for WidgetLoggerEngine
            /*!
             * \param message_displays_flag The tabs to be shown in the log widget.
             * \param toolbar_area The toolbar area to use for action toolbars in the specified tabs.
             * If no toolbars should be displayed, use Qt::NoToolBarArea.
             */
            WidgetLoggerEngine(MessageDisplaysFlag message_displays_flag = (MessageDisplaysFlag) (DefaultDisplays),
                               Qt::ToolBarArea toolbar_area = Qt::TopToolBarArea);
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
            void setLineWrapMode(QPlainTextEdit::LineWrapMode mode);

        private:
            WidgetLoggerEnginePrivateData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(WidgetLoggerEngine::MessageDisplaysFlag)
    }
}


#endif // WIDGETLOGGERENGINE_H
