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

    for (int i = 1; i < messages.count(); i++) {
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

    // Start with the correct font:
    switch (message_type) {
    case Logger::Info:
        message.append("<font color='black'>");
        break;

    case Logger::Warning:
        message.append("<font color='orange'>");
        break;

    case Logger::Error:
        message.append("<font color='red'>");
        break;

    case Logger::Fatal:
        message.append("<font color='purple'><b>");
        break;

    case Logger::Debug:
        message.append("<font color='grey'>");
        break;

    case Logger::Trace:
        message.append("<font color='lightgrey'>");
        break;

    default:
        break;
    }

    message.append(QTime::currentTime().toString());
    message.append(QString(" [%1] ").arg(Log->logLevelToString(message_type),-8,QChar(QChar::Nbsp)));
    message.append(messages.front().toString());
    for (int i = 1; i < messages.count(); i++) {
        message.append("<br>            %1").arg(messages.at(i).toString());
    }
    message.append("</font>");

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
    for (int i = 0; i < messages.count(); i++) {
        message.append(QString("\n<Message_%1>%2</Message_%1>").arg(i).arg(messages.at(i).toString()));
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
    QString message;
    switch (message_type) {
        case Logger::Trace:
            message = QString("<td>%1</td><td><font color='grey'>%2</font></td>").arg(QTime::currentTime().toString()).arg(messages.front().toString());
            break;
        case Logger::Debug:
            message = QString("<td>%1</td><td><font color='grey'>%2</font></td>").arg(QTime::currentTime().toString()).arg(messages.front().toString());
            break;
        case Logger::Warning:
            message = QString("<td>%1</td><td><font color='orange'>%2</font></td>").arg(QTime::currentTime().toString()).arg(messages.front().toString());
            break;
        case Logger::Info:
            message = QString("<td>%1</td><td><font color='black'>%2</font></td>").arg(QTime::currentTime().toString()).arg(messages.front().toString());
            break;
        case Logger::Error:
            message = QString("<td>%1</td><td><font color='red'>%2</font></td>").arg(QTime::currentTime().toString()).arg(messages.front().toString());
            break;
        case Logger::Fatal:
            message = QString("<td>%1</td><td><font color='red'>%2</font></td>").arg(QTime::currentTime().toString()).arg(messages.front().toString());
            break;
        case Logger::None:
            return QString();
        case Logger::AllLogLevels:
            return QString();
        }

    /*for (int i = 1; i < messages.count(); i++) {
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

