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

#include "FormattingEngines.h"

// -----------------------------------
// Default Formatting Engine
// -----------------------------------
QString Qtilities::Logging::FormattingEngine_Default::initializeString() const {
    return QString(tr("%1 Session Log:\nDate: %2\n")).arg(QCoreApplication::applicationName()).arg(QDateTime::currentDateTime().toString());
}

QString Qtilities::Logging::FormattingEngine_Default::formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const {
    QString message = QTime::currentTime().toString();
    if (message_type == Logger::Debug)
        message.append(QString(" [%1] ").arg("Debug",-8,QChar(' ')));
    else if (message_type == Logger::Trace)
        message.append(QString(" [%1] ").arg("Trace",-8,QChar(' ')));
    else if (message_type == Logger::Warning)
        message.append(QString(" [%1] ").arg("Warning",-8,QChar(' ')));
    else if (message_type == Logger::Error)
        message.append(QString(" [%1] ").arg("Error",-8,QChar(' ')));
    else if (message_type == Logger::Fatal)
        message.append(QString(" [%1] ").arg("Fatal",-8,QChar(' ')));
    else if (message_type == Logger::Info)
        message.append(QString(" [%1] ").arg("Info",-8,QChar(' ')));

    message.append(messages.front().toString());

    for (int i = 1; i < messages.count(); ++i) {
        message.append("\n            %1").arg(messages.at(i).toString());
    }
    return message;
}

QString Qtilities::Logging::FormattingEngine_Default::finalizeString() const {
    return QString(tr("\nEnd of session log.\n%1")).arg(QDateTime::currentDateTime().toString());
}

// -----------------------------------
// Rich Text Formatting Engine
// -----------------------------------
 QString Qtilities::Logging::FormattingEngine_Rich_Text::initializeString() const {
    return QString(tr("%1 Session Log:<br>Date: %2<br>")).arg(QCoreApplication::applicationName()).arg(QDateTime::currentDateTime().toString());
 }

QString Qtilities::Logging::FormattingEngine_Rich_Text::formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const {
    QString message;

    message.append(QTime::currentTime().toString());
    message.append(QString(" [%1] ").arg(Log->logLevelToString(message_type),-8,QChar(QChar::Nbsp)));

    // Since we convert it to rich text, < and > characters must be converted.
    QString formatted_string = messages.front().toString();
    formatted_string.replace("<","&#60;");
    formatted_string.replace(">","&#62;");
    message.append(formatted_string);
    for (int i = 1; i < messages.count(); ++i) {
        QString formatted_string = messages.at(i).toString();
        formatted_string.replace("<","&#60;");
        formatted_string.replace(">","&#62;");
        message.append("<br>            %1").arg(formatted_string);
    }
    message.append("</font>");

    QString custom_color_hint;
    if (messages.count() > 0)
        custom_color_hint = matchColorFormattingHint(messages.front().toString(),message_type);

    // Start with the correct font:
    switch (message_type) {
    case Logger::Info:
        if (custom_color_hint.isEmpty())
            message.prepend("<font color='black'>");
        else
            message.prepend(QString("<font color='%1'>").arg(custom_color_hint));
        break;

    case Logger::Warning:
        if (custom_color_hint.isEmpty())
            message.prepend("<font color='orange'>");
        else
            message.prepend(QString("<font color='%1'>").arg(custom_color_hint));
        break;

    case Logger::Error:
        if (custom_color_hint.isEmpty())
            message.prepend("<font color='red'>");
        else
            message.prepend(QString("<font color='%1'>").arg(custom_color_hint));
        break;

    case Logger::Fatal:
        if (custom_color_hint.isEmpty())
            message.prepend("<font color='purple'>");
        else
            message.prepend(QString("<font color='%1'>").arg(custom_color_hint));
        break;

    case Logger::Debug:
        if (custom_color_hint.isEmpty())
            message.prepend("<font color='grey'>");
        else
            message.prepend(QString("<font color='%1'>").arg(custom_color_hint));
        break;

    case Logger::Trace:
        if (custom_color_hint.isEmpty())
            message.prepend("<font color='lightgrey'>");
        else
            message.prepend(QString("<font color='%1'>").arg(custom_color_hint));
        break;

    default:
        break;
    }

    // If the message matches a custom color regexp we use that color, otherwise
    // we use the color of the message.

    if (message_type == Logger::Fatal)
        message.append("</b>");

    return message;
}

QString Qtilities::Logging::FormattingEngine_Rich_Text::finalizeString() const {
    return QString(tr("<br>End of session log.<br>%1")).arg(QDateTime::currentDateTime().toString());
}

// -----------------------------------
// XML Formatting Engine
// -----------------------------------
QString Qtilities::Logging::FormattingEngine_XML::initializeString() const {
    return QString("<Session Context=\"%1\" Date=\"%2\">").arg(QCoreApplication::applicationName()).arg(QDateTime::currentDateTime().toString());
}

QString Qtilities::Logging::FormattingEngine_XML::formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const {
    QString type_string = Log->logLevelToString(message_type);
    QString message;
    message = QString("<Log>\n<Type>%1</Type>").arg(type_string);
    for (int i = 0; i < messages.count(); ++i) {    
        QString formatted_string = messages.at(i).toString();
        formatted_string.replace("<","(");
        formatted_string.replace(">",")");
        message.append(QString("\n<Message_%1>%2</Message_%1>").arg(i).arg(formatted_string));
    }
    message.append("\n</Log>");

    return message;
}

QString Qtilities::Logging::FormattingEngine_XML::finalizeString() const {
    return QString("</Session>\n");
}

// -----------------------------------
// HTML Formatting Engine
// -----------------------------------
QString Qtilities::Logging::FormattingEngine_HTML::initializeString() const {
    return QString("<html>"
                    "<font face=\"Arial\">"
                    "<body><h2>%1 - %2</h2>\n"
                    "<table width=100% style=\"table-layout:auto; margin: auto; border-width:thin thin thin thin; border-color:#000000;>\n"
                    "<tr><td width=10%><b>Time</b></td><td><b>Message</b></td></tr>\n</font>")
                    .arg(QCoreApplication::applicationName())
                    .arg(QDate::currentDate().toString());
}

QString Qtilities::Logging::FormattingEngine_HTML::formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const {
    if (messages.count() == 0)
        return "";

    QString formatted_string = messages.front().toString();
    formatted_string.replace("<","&#60;");
    formatted_string.replace(">","&#62;");

    QString message;
    switch (message_type) {
        case Logger::Trace:
            message = QString("<td>%1</td><td><font color='grey'>%2</font></td>").arg(QTime::currentTime().toString()).arg(formatted_string);
            break;
        case Logger::Debug:
            message = QString("<td>%1</td><td><font color='grey'>%2</font></td>").arg(QTime::currentTime().toString()).arg(formatted_string);
            break;
        case Logger::Warning:
            message = QString("<td>%1</td><td><font color='orange'>%2</font></td>").arg(QTime::currentTime().toString()).arg(formatted_string);
            break;
        case Logger::Info:
            message = QString("<td>%1</td><td><font color='black'>%2</font></td>").arg(QTime::currentTime().toString()).arg(formatted_string);
            break;
        case Logger::Error:
            message = QString("<td>%1</td><td><font color='red'>%2</font></td>").arg(QTime::currentTime().toString()).arg(formatted_string);
            break;
        case Logger::Fatal:
            message = QString("<td>%1</td><td><font color='red'>%2</font></td>").arg(QTime::currentTime().toString()).arg(formatted_string);
            break;
        case Logger::None:
            return QString();
        case Logger::AllLogLevels:
            return QString();
        }

    /*for (int i = 1; i < messages.count(); ++i) {
        message.append(QString("%1").arg(messages.at(i).toString()));
    }*/

    message.prepend("<tr><font size=\"5\" face=\"verdana\">");
    message.append("</font></tr>");
    return message;
}

QString Qtilities::Logging::FormattingEngine_HTML::finalizeString() const {
    return QString(tr("</table><br>End of session log: %1</body></html>\n")).arg(QDateTime::currentDateTime().toString());
}

// -----------------------------------
// Qt Message Formatting Engine
// -----------------------------------
QString Qtilities::Logging::FormattingEngine_QtMsgEngineFormat::initializeString() const {
    return QString();
}

QString Qtilities::Logging::FormattingEngine_QtMsgEngineFormat::formatMessage(Logger::MessageType message_type, const QList<QVariant>& messages) const {
    Q_UNUSED(message_type)

    QString message = QString();
    message.append(QString("%1").arg(messages.front().toString()));
    return message;
}

QString Qtilities::Logging::FormattingEngine_QtMsgEngineFormat::finalizeString() const {
    return QString();
}

