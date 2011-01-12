/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

        This struct is defined in the header because it is use by inherited classes.
          */
        struct AbstractLoggerEngineData {
            Logger::MessageTypeFlags enabled_message_types;
            AbstractFormattingEngine* formatting_engine;
            bool is_enabled;
            bool is_initialized;
        };

        /*!
        \class AbstractLoggerEngine
        \brief The base class of all logger engines.

        Note: Set the name of your logger engine in the constructor of your engine using setObjectName(). This name will be used in the logger.
          */
        class LOGGING_SHARED_EXPORT AbstractLoggerEngine : public QObject
        {
            Q_OBJECT
            Q_PROPERTY(Qtilities::Logging::Logger::MessageTypeFlags EnabledMessageTypes READ getEnabledMessageTypes WRITE setEnabledMessageTypes);
            Q_PROPERTY(QString FormattingEngine READ formattingEngineName);

        public:
            AbstractLoggerEngine();
            virtual ~AbstractLoggerEngine();

            //! Function which is called to initialize the logger engine.
            virtual bool initialize() = 0;
            //! Returns true if the engine is initialized.
            bool isInitialized() const;
            //! Function which received a formatted string which needs to be logged.
            virtual void logMessage(const QString& message) = 0;

            //! Indicates if the engine is active.
            bool isActive() const;
            //! Sets the activity of the engine.
            void setActive(bool is_active);

            //! Returns the name of the engine.
            virtual QString name() const = 0;
            //! Returns a description of the engine.
            virtual QString description() const = 0;
            //! Returns a status message for the engine.
            virtual QString status() const = 0;
            //! Indicates if the user can remove this engine.
            virtual bool removable() const = 0;

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
            //! Indicates if the formatting engine can be changed by the user at runtime.
            virtual bool isFormattingEngineConstant() const = 0;

        public slots:
            //! Function which is called to finalize the logger engine.
            virtual void finalize() = 0;
            //! Slot which is connected to the newMessage() signal of the Logger class.
            void newMessages(const QString& engine_name, Logger::MessageType message_type, const QList<QVariant>& messages);

        protected:
            AbstractLoggerEngineData* abstractLoggerEngineData;
        };
    }
}

#endif // ABSTRACTLOGGERENGINE_H
