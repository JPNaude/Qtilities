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

#ifndef ABSTRACTLOGGERENGINE_H
#define ABSTRACTLOGGERENGINE_H

#include "Logger.h"
#include "AbstractFormattingEngine.h"
#include "Logging_global.h"

namespace Qtilities {
    namespace Logging {
        class AbstractFormattingEngine;

        /*!
        \struct AbstractLoggerEngineData
        \brief Structure used by an AbstractLoggerEngine to store it's data.
          */
        struct AbstractLoggerEngineData {
            AbstractLoggerEngineData(): is_enabled(true),
                is_initialized(false),
                message_contexts(Logger::AllMessageContexts),
                is_removable(true) {}

            //! The enabled message types for this logger engine.
            Logger::MessageTypeFlags        enabled_message_types;
            //! The formatting engine used by this logger engine.
            AbstractFormattingEngine*       formatting_engine;
            //! Indicates if this logger engine is enabled.
            bool                            is_enabled;
            //! Indicates if this logger engine is initialized.
            bool                            is_initialized;
            //! The message contexts processed by this logger engine.
            Logger::MessageContextFlags     message_contexts;
            //! The name of this logger engine.
            QString                         engine_name;
            //! Indicates if this engine is removable by the user.
            bool                            is_removable;
        };

        /*!
        \class AbstractLoggerEngine
        \brief The base class of all logger engines.
          */
        class LOGGING_SHARED_EXPORT AbstractLoggerEngine : public QObject
        {
            Q_OBJECT
            Q_PROPERTY(Qtilities::Logging::Logger::MessageTypeFlags EnabledMessageTypes READ getEnabledMessageTypes WRITE setEnabledMessageTypes)
            Q_PROPERTY(QString FormattingEngine READ formattingEngineName)

        public:
            AbstractLoggerEngine();
            virtual ~AbstractLoggerEngine();

            //! Function which is called to initialize the logger engine.
            virtual bool initialize() = 0;
            //! Returns true if the engine is initialized.
            bool isInitialized() const;
            //! Function which receives a formatted string which needs to be logged.
            /*!
              Messages arrives at logger engines through the newMessages() slot which will format the messages and validate if they must be logged.
              If so, this function will be called with a formatted message. If you wish to handle the message formatting manually, you can reimplement the
              newMessages() function.

              \note When calling this function directly on an engine, the formatting engine will be bypassed.
              */
            virtual void logMessage(const QString& message, Logger::MessageType message_type = Logger::Info) = 0;
            //! Clears the log currently hold by the logger engine.
            /*!
              \note This is not supported by all logger engines. See the class documentation of the logger engine you are interested in to see if it is supported.
              */
            virtual void clearLog() {}

            //! Indicates if the engine is active.
            bool isActive() const;
            //! Sets the activity of the engine.
            void setActive(bool is_active);

            //! Returns the name of the engine.
            inline QString name() const { return abstractLoggerEngineData->engine_name; }
            //! Sets the name of the engine.
            void setName(const QString& name);
            //! Returns a description of the engine.
            virtual QString description() const = 0;
            //! Returns a status message for the engine.
            virtual QString status() const = 0;
            //! Gets if the user can remove this engine.
            virtual bool removable() const;
            //! Sets if the user can remove this engine.
            virtual void setRemovable(bool is_removable);

            //! Sets the enabled message types for this engine.
            virtual void setEnabledMessageTypes(Logger::MessageTypeFlags message_types);
            //! Enables all message types for this engine.
            virtual void enableAllMessageTypes();
            //! Returns the enabled message types for this engine.
            Logger::MessageTypeFlags getEnabledMessageTypes() const;

            //! Installs a formatting engine which needs to be used by this engine.
            void installFormattingEngine(AbstractFormattingEngine* engine);
            //! Returns a reference to the formatting engine used by this engine.
            AbstractFormattingEngine* getInstalledFormattingEngine();
            //! Returns the name of the installed formatting engine.
            QString formattingEngineName();

            //! Indicates if the formatting engine and/or the message contexts can be changed by the user at runtime.
            virtual bool isFormattingEngineConstant() const = 0;

            //! Returns the logging contexts for which this engine accepts messages.
            inline Logger::MessageContextFlags messageContexts() const { return abstractLoggerEngineData->message_contexts; }
            //! Sets the logging contexts for which this engine accepts messages.
            void setMessageContexts(Logger::MessageContextFlags message_contexts) { abstractLoggerEngineData->message_contexts = message_contexts; }

        public slots:
            //! Function which is called to finalize the logger engine.
            virtual void finalize() = 0;
            //! Slot which is connected to the newMessage() signal of the Logger class.
            virtual void newMessages(const QString& engine_name, Logger::MessageType message_type, Logger::MessageContextFlags message_context, const QList<QVariant>& messages);

        protected:
            AbstractLoggerEngineData* abstractLoggerEngineData;
        };
    }
}

#endif // ABSTRACTLOGGERENGINE_H
