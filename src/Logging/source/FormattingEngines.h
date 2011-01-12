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

#ifndef FORMATTINGENGINES_H
#define FORMATTINGENGINES_H

#include "LoggingConstants.h"
#include "AbstractFormattingEngine.h"

#include <QCoreApplication>
#include <QDateTime>

namespace Qtilities {
    namespace Logging {
        using namespace Qtilities::Logging::Constants;

        //! Default formatting engine which basically parses the QVariant messages into a single QString message.
        /*!

          */
        class FormattingEngine_Default : virtual public AbstractFormattingEngine
        {           

        public:
            static FormattingEngine_Default & instance() {
                static FormattingEngine_Default* theInstance = new FormattingEngine_Default();
                return *theInstance;
            }
            ~FormattingEngine_Default() {}

        protected:
            FormattingEngine_Default() : AbstractFormattingEngine() {
                setObjectName(name());
            }

        public:
            QString initializeString() const;
            QString finalizeString() const;
            QString formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const;
            QString fileExtension() const { return QString("log"); }
            QString name() const { return FORMATTING_ENGINE_DEFAULT; }
            QString endOfLineChar() const { return QString("\n"); }
        };

        //! Rich Text Formatting Engine.
        /*!
          This formatting engine prepares the logged messages in rich text format suited for widget based logger
          engines.
          */
        class FormattingEngine_Rich_Text : virtual public AbstractFormattingEngine
        {
        public:
            static FormattingEngine_Rich_Text & instance() {
                static FormattingEngine_Rich_Text* theInstance = new FormattingEngine_Rich_Text();
                return *theInstance;
            }
            ~FormattingEngine_Rich_Text() {}
        protected:
            FormattingEngine_Rich_Text() : AbstractFormattingEngine() {
                setObjectName(name());
            }

        public:
            QString initializeString() const;
            QString finalizeString() const;
            QString formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const;
            QString fileExtension() const { return QString(); }
            QString name() const { return FORMATTING_ENGINE_RICH_TEXT; }
            QString endOfLineChar() const { return QString("<br>"); }
        };

        //! XML formatting engines.
        /*!
          This formatting engine prepares the logged messages in XML format.
          */
        class FormattingEngine_XML : virtual public AbstractFormattingEngine
        {
        public:
            static FormattingEngine_XML & instance() {
                static FormattingEngine_XML* theInstance = new FormattingEngine_XML();
                return *theInstance;
            }
            ~FormattingEngine_XML() {}
        protected:
            FormattingEngine_XML() : AbstractFormattingEngine() {
                setObjectName(name());
            }

        public:
            QString initializeString() const;
            QString finalizeString() const;
            QString formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const;
            QString fileExtension() const { return QString("xml"); }
            QString name() const { return FORMATTING_ENGINE_XML; }
            QString endOfLineChar() const { return QString("\n"); }
        };

        //! HTML formatting engines.
        /*!
          This formatting engine prepares the logged messages in HTML format.
          */
        class FormattingEngine_HTML : virtual public AbstractFormattingEngine
        {
        public:
            static FormattingEngine_HTML & instance() {
                static FormattingEngine_HTML* theInstance = new FormattingEngine_HTML();
                return *theInstance;
            }
            ~FormattingEngine_HTML() {}
        protected:
            FormattingEngine_HTML() : AbstractFormattingEngine() {
                setObjectName(name());
            }

        public:
            QString initializeString() const;
            QString finalizeString() const;
            QString formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const;
            QString fileExtension() const { return QString("html"); }
            QString name() const { return FORMATTING_ENGINE_HTML; }
            QString endOfLineChar() const { return QString("<br>"); }
        };

        //! A formatting engine used to format messages for the QtMsgLoggerEngine.
        /*!
          This formatting engine is not a factory item, since it is used to format messages in a format which the
          QtMsgLoggerEngine will interpret.

          This formatting engine is an internal engine.
          */
        class FormattingEngine_QtMsgEngineFormat : virtual public AbstractFormattingEngine
        {
        public:
            static FormattingEngine_QtMsgEngineFormat & instance() {
                static FormattingEngine_QtMsgEngineFormat* theInstance = new FormattingEngine_QtMsgEngineFormat();
                return *theInstance;
            }
            ~FormattingEngine_QtMsgEngineFormat() {}
        protected:
            FormattingEngine_QtMsgEngineFormat() : AbstractFormattingEngine() {
                setObjectName(name());
            }

        public:
            QString initializeString() const;
            QString finalizeString() const;
            QString formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const;
            QString fileExtension() const { return QString(); }
            QString name() const { return FORMATTING_ENGINE_QT_MSG; }
            QString endOfLineChar() const { return QString("\n"); }
        };
    }
}

#endif // FORMATTINGENGINES_H
