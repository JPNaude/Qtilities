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

#ifndef ABSTRACTFORMATTINGENGINE_H
#define ABSTRACTFORMATTINGENGINE_H

#include <QString>
#include <QList>
#include <QVariant>

#include "Logger.h"
#include "Logging_global.h"

namespace Qtilities {
    namespace Logging {
        /*!
          \struct CustomFormattingHint
          \brief The CustomFormattingHint structure is used to define custom formatting hints for logger formatting engines.

          For more information, see AbstractFormattingEngine_custom_hints.
         */
        struct CustomFormattingHint {
        public:
            CustomFormattingHint(const QRegExp& regexp, const QString& hint,Logger::MessageTypeFlags message_type_flags = Logger::AllLogLevels) {
                d_regexp = regexp;
                d_hint = hint;
                d_message_type_flags = message_type_flags;
            }
            CustomFormattingHint(const CustomFormattingHint& ref) {
                d_hint = ref.d_hint;
                d_message_type_flags = ref.d_message_type_flags;
                d_regexp = ref.d_regexp;
            }
            CustomFormattingHint& operator=(const CustomFormattingHint& ref) {
                if (this==&ref) return *this;

                d_hint = ref.d_hint;
                d_message_type_flags = ref.d_message_type_flags;
                d_regexp = ref.d_regexp;

                return *this;
            }
            bool operator==(const CustomFormattingHint& ref) const {
                if (d_hint != ref.d_hint)
                    return false;
                if (d_message_type_flags != ref.d_message_type_flags)
                    return false;
                if (d_regexp != ref.d_regexp)
                    return false;

                return true;
            }
            bool operator!=(const CustomFormattingHint& ref) const {
                return !(*this==ref);
            }

            QString                     d_hint;
            QRegExp                     d_regexp;
            Logger::MessageTypeFlags    d_message_type_flags;
        };


        /*!
        \class AbstractFormattingEngine
        \brief The base class of all formatting engines.

        Formatting engines gets a list of messages as a QList<QVariant> list through formatMessage() and returns a formatted string.

        Each formatting engine can return its custom initialization string through initializeString(), a finalization string through finalizeString() and
        a custom end of line character through endOfLineChar(). Lastly, a formatting engine can return a fileExtension() which guides %Qtilities on the
        type of files that the log engine represent (if any).

        All the default formatting engines in %Qtilities are constructed for you during initialization of the Logger. All logger engine will share
        the instances of these formatting engines with each other.

        \section AbstractFormattingEngine_custom_hints Custom Formatting Hints

        Formatting engines allows you to set custom color formatting hints by providing QRegExp expressions with matching colors. This allows you to
        format messages using different color schemes without having to create a complete new formatting engine. Formatting engines must build in support for
        this functionality, and it won't apply to all engines. For example, the FormattingEngine_Default engine does not use coloring, therefore
        it does not support this functionality. Other engine however, such as FormattingEngine_Rich_Text, does
        use coloring information and therefore supports these custom color rules.

        By default engines would use colors that you would expect for different messages. For example, errors are colored in red, warnings in orange etc. However you
        can color messages by their contents using these rules. For example if you want to color messages which represents a succesfull operation in green, you can do
        this by using the correct matching expression.

        Lets look at an example where we color all messages starting with translations of "Successfully" to be green. We add this custom color hint to the
        FormattingEngine_Rich_Text engine after log initialization. Thus, all logger engines that uses the Rich Text formatting engine will make use of this custom hint.

\code
// Initialize the logger:
Log->setLoggerSessionConfigPath(QtilitiesApplication::applicationSessionPath());
LOG_INITIALIZE();

// Add a formatting hint to the RichText formatting engine.
AbstractFormattingEngine* rich_text_engine = Log->formattingEngineReference(qti_def_FORMATTING_ENGINE_RICH_TEXT);
if (rich_text_engine) {
    QRegExp reg_exp_success_color = QRegExp(QObject::tr("Successfully") + "*",Qt::CaseInsensitive,QRegExp::Wildcard);
    CustomFormattingHint success_message_hint(reg_exp_success_color,"green",Logger::Info);
    rich_text_engine->addColorFormattingHint(success_message_hint);
}
\endcode

        Since all the default %Qtilities provided formatting engines are shared by all engines its only neccesarry to add custom hints to the single instance and the rule
        will be applied to all logger engines that uses that formatting engine.

        Note that previous versions of %Qtilities had the above rule built in to FormattingEngine_Rich_Text in order to match successfull messages.
        Since that design was flawed because it depended on English as your language, this built in functionality was removed and you must now set the above rule
        manually if you depended on the previous mode of operation.
          */
        class LOGGING_SHARED_EXPORT AbstractFormattingEngine : public QObject
        {
        Q_OBJECT
        Q_PROPERTY(QString FileExtension READ fileExtension)

        public:
            AbstractFormattingEngine() {}
            virtual ~AbstractFormattingEngine() {}

            //! Function which is used to provide the initialization string for the logger engine. This string will be sent to the logger engine before any messages.
            virtual QString initializeString() const = 0;
            //! Function which is used to provide the finalization string for the logger engine. This string will be sent to the logger engine after all messages.
            virtual QString finalizeString() const = 0;
            //! Function which is called to format the message.
            virtual QString formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const = 0;
            //! Function which provides a name for this formatting engine.
            virtual QString name() const = 0;
            //! Function which provides a file extension which will be used if the logger engine is a File logger engine.
            /*!
                If this is not applicable just return QString().
                Example format = QString("log"). Note that the point is not part of the extension.
                */
            virtual QString fileExtension() const = 0;
            //! Function which returns a priority for this formatting engine.
            /*!
             * In cases where multiple formatting engines with the same fileExtension() are installed, the engine's priority will
             * be used to determine the formatting engine to use, where the engine with the higest priority will be used.
             *
             * \note All of the formatting engines provided by %Qtilities has the default priority of 1.
             */
            virtual uint priority() const {
                return 1;
            }
            //! Function which provides the end of line character used by the formatting engine.
            virtual QString endOfLineChar() const = 0;

            // ------------------------------------
            // Custom Formatting Hints Interface
            // ------------------------------------
            //! Adds a color formatting hint to the formatting engine.
            /*!
              \param color_formatting_hint The CustomFormattingHint describing the format (with the hint being the color, such as "#112233").
              \return True when successfull, false otherwise.

              \note If the color formatting hint has an invalid regular expression, this function will do nothing.
              \note A regular expression can only be used to specify a hint once. While duplicates can still be added to the list, the first matching expression
                    in the list will always be used.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool addColorFormattingHint(CustomFormattingHint color_formatting_hint) {
                if (!color_formatting_hint.d_regexp.isValid())
                    return false;

                color_formatting_hints.append(color_formatting_hint);
                return true;
            }
            //! Returns all color formatting hint rules specified for this engine.
            /*!
              \returns A QMap with the keys beign the expressions, and the values the CustomFormattingHint describing the format (with the hint being the color, such as "#112233").

              <i>This function was added in %Qtilities v1.1.</i>
              */
            QList<CustomFormattingHint> colorFormattingHints() const {
                return color_formatting_hints;
            }
            //! Clear all color formatting hint rules specified for this engine.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void clearColorFormattingHints() {
                color_formatting_hints.clear();
            }
            //! Removes a specific color formatting hint.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void removeColorFormattingHint(CustomFormattingHint custom_formatting_hint) {
                color_formatting_hints.removeOne(custom_formatting_hint);
            }
            //! Checks a color formatting hint against a log message to see if they match.
            /*!
              This function will check the message type and the message contents against all color
              formatting hints specified. When a match if found, the color that must be used is returned.

              \returns The color that must be used to format the message if a match was found. If not match was found and empty string is returned.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            QString matchColorFormattingHint(const QString& message, Logger::MessageTypeFlags message_type_flags) const {
                for (int i = 0; i < color_formatting_hints.count(); i++) {
                    CustomFormattingHint hint = color_formatting_hints.at(i);
                    if (hint.d_message_type_flags & message_type_flags) {
                        if (hint.d_regexp.exactMatch(message))
                            return hint.d_hint;
                    }
                }

                return "";
            }
            //! Function that does the same as QTextDocument::escape(). Since the Logging module does not depend on QtGui, we cannot use that function directly.
            static QString escape(const QString& plain) {
                // This code is exactly the same as the code found in QTextDocument::escape()
                QString rich;
                rich.reserve(int(plain.length() * 1.1));
                for (int i = 0; i < plain.length(); ++i) {
                    if (plain.at(i) == QLatin1Char('<'))
                        rich += QLatin1String("&lt;");
                    else if (plain.at(i) == QLatin1Char('>'))
                        rich += QLatin1String("&gt;");
                    else if (plain.at(i) == QLatin1Char('&'))
                        rich += QLatin1String("&amp;");
                    else if (plain.at(i) == QLatin1Char('"'))
                        rich += QLatin1String("&quot;");
                    else
                        rich += plain.at(i);
                }
                return rich;
            }

        protected:
            QList<CustomFormattingHint> color_formatting_hints;
        };
    }
}

#endif // ABSTRACTFORMATTINGENGINE_H
