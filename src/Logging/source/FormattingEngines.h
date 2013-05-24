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
          The default formatting engine. A preview of the formatting applied is shown below:

          \image html log_formatting_preview_default.jpg "Default Formatting Engine Preview"

          \sa Qtilities::Logging::FormattingEngine_HTML, Qtilities::Logging::FormattingEngine_XML, Qtilities::Logging::FormattingEngine_QtMsgEngineFormat, Qtilities::Logging::FormattingEngine_Rich_Text
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
            QString name() const { return qti_def_FORMATTING_ENGINE_DEFAULT; }
            QString endOfLineChar() const { return QString("\n"); }
        };

        //! Rich Text Formatting Engine.
        /*!
          This formatting engine prepares the logged messages in rich text format suited for widget based logger
          engines. All QVariants in the input list of formatMessage() are converted to strings and one single string is constructed.

          A preview of the formatting applied is shown below:

          \image html log_formatting_preview_rich_text.jpg "Rich Text Formatting Engine Preview"

          \note This formatting engine supports custom color formatting hints by matching the match expressions against the first string (the first QVariant in the list passed to formatMessage()).

          \sa Qtilities::Logging::FormattingEngine_HTML, Qtilities::Logging::FormattingEngine_XML, Qtilities::Logging::FormattingEngine_QtMsgEngineFormat, Qtilities::Logging::FormattingEngine_Default
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
            QString name() const { return qti_def_FORMATTING_ENGINE_RICH_TEXT; }
            QString endOfLineChar() const { return QString("<br>"); }
        };

        //! XML Formatting Engine.
        /*!
          This formatting engine prepares the logged messages in XML format.

          A preview of the formatting applied is shown below:

          \image html log_formatting_preview_xml.jpg "XML Formatting Engine Preview"

          \sa Qtilities::Logging::FormattingEngine_HTML, Qtilities::Logging::FormattingEngine_Rich_Text, Qtilities::Logging::FormattingEngine_QtMsgEngineFormat, Qtilities::Logging::FormattingEngine_Default
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
            QString name() const { return qti_def_FORMATTING_ENGINE_XML; }
            QString endOfLineChar() const { return QString("\n"); }
        };

        //! HTML Formatting Engine.
        /*!
          This formatting engine prepares the logged messages in a very basic HTML format.

          A preview of the formatting applied is shown below:

          \image html log_formatting_preview_html.jpg "HTML Formatting Engine Preview"

          \sa Qtilities::Logging::FormattingEngine_XML, Qtilities::Logging::FormattingEngine_Rich_Text, Qtilities::Logging::FormattingEngine_QtMsgEngineFormat, Qtilities::Logging::FormattingEngine_Default
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
            QString name() const { return qti_def_FORMATTING_ENGINE_HTML; }
            QString endOfLineChar() const { return QString("<br>"); }
        };

        //! A formatting engine used to format messages for the QtMsgLoggerEngine.
        /*!
          This formatting engine is not a factory item, since it is used to format messages in a format which the
          QtMsgLoggerEngine will interpret.

          This formatting engine is an internal engine.

          A preview of the formatting applied is shown below:

          \image html log_formatting_preview_qt_message.jpg "Qt Message Formatting Engine Preview"

          \sa Qtilities::Logging::FormattingEngine_XML, Qtilities::Logging::FormattingEngine_Rich_Text, Qtilities::Logging::FormattingEngine_HTML, Qtilities::Logging::FormattingEngine_Default
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
            QString name() const { return qti_def_FORMATTING_ENGINE_QT_MSG; }
            QString endOfLineChar() const { return QString("\n"); }
        };
    }
}

#endif // FORMATTINGENGINES_H
