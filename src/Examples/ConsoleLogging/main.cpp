/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include <QtCore/QCoreApplication>

#include <QtilitiesLogging>
using namespace QtilitiesLogging;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Jaco Naude");
    QCoreApplication::setOrganizationDomain("Qtilities");
    QCoreApplication::setApplicationName("Console Logging Example");
    QCoreApplication::setApplicationVersion("1.0");

    // Initialize the logger:
    // When using Qtilities Core as well, we should add this line:
    // Log->setLoggerSessionConfigPath(QtilitiesCoreApplication::applicationSessionPath());
    // However for this example we are only using Qtilities Logging.
    LOG_INITIALIZE();
    // We want to capture all messages <= Trace level:
    Log->setGlobalLogLevel(Logger::Trace);

    // Create a file engine to verify the results.
    QString xml_example = QCoreApplication::applicationDirPath() + "/XML_Log.xml";
    AbstractLoggerEngine* xml_file_engine = Log->newFileEngine("XML File Example",xml_example);
    QString plain_example = QCoreApplication::applicationDirPath() + "/Plain_Log.log";
    AbstractLoggerEngine* plain_file_engine = Log->newFileEngine("Plain File Example",plain_example);
    QString html_example = QCoreApplication::applicationDirPath() + "/HTML_Log.html";
    AbstractLoggerEngine* html_file_engine = Log->newFileEngine("HTML File Example",html_example);

    // Enable the Qt Message logger engine:
    Log->toggleQtMsgEngine(true);

    // Log messages to all engines:
    LOG_INFO("Information Message");
    LOG_WARNING("Warning Message");
    LOG_ERROR("Error Message");
    LOG_FATAL("Fatal Message");
    LOG_DEBUG("Debug Message");
    LOG_TRACE("Trace Message");

    // Enable the console logger engine:
    Log->toggleConsoleEngine(true);
    // Disable the Qt Message logger engine:
    Log->toggleQtMsgEngine(false);

    // Log messages to each engine seperately:
    LOG_INFO_E("XML File Example","XML Information Message");
    LOG_WARNING_E("XML File Example","XML Warning Message");
    LOG_ERROR_E("XML File Example","XML Error Message");
    LOG_FATAL_E("XML File Example","XML Fatal Message");
    LOG_DEBUG_E("XML File Example","XML Debug Message");
    LOG_TRACE_E("XML File Example","XML Trace Message");

    LOG_INFO_E("Plain File Example","Plain Information Message");
    LOG_WARNING_E("Plain File Example","Plain Warning Message");
    LOG_ERROR_E("Plain File Example","Plain Error Message");
    LOG_FATAL_E("Plain File Example","Plain Fatal Message");
    LOG_DEBUG_E("Plain File Example","Plain Debug Message");
    LOG_TRACE_E("Plain File Example","XML Trace Message");

    LOG_INFO_E("HTML File Example","HTML Information Message");
    LOG_WARNING_E("HTML File Example","HTML Warning Message");
    LOG_ERROR_E("HTML File Example","HTML Error Message");
    LOG_FATAL_E("HTML File Example","HTML Fatal Message");
    LOG_DEBUG_E("HTML File Example","HTML Debug Message");
    LOG_TRACE_E("HTML File Example","XML Trace Message");

    // Disable the console logger engine:
    Log->toggleConsoleEngine(false);

    // Capture messages from the Qt Message System:
    Log->setIsQtMessageHandler(true);
    qDebug() << "Message from the Qt Message System";
    Log->setIsQtMessageHandler(false);
    qDebug() << "Message from the Qt Message System which will not be catched.";

    // Do some context related logging:
    if (xml_file_engine && plain_file_engine && html_file_engine) {
        xml_file_engine->setMessageContexts(Logger::SystemWideMessages);
        plain_file_engine->setMessageContexts(Logger::EngineSpecificMessages);
        html_file_engine->setMessageContexts(Logger::PriorityMessages);

        // Now log a system wide message: It will only appear in xml_file_engine.
        LOG_INFO("System widge message");

        // Now log some engine specific messages. Only the plain_file_engine will get the second message:
        LOG_INFO_E(xml_file_engine->name(),"Engine specific message to XML.");
        LOG_INFO_E(plain_file_engine->name(),"Engine specific message to PLAIN.");
        LOG_INFO_E(html_file_engine->name(),"Engine specific message to HTML.");

        // Now log a priority message: It will only appear in the html_file_engine.
        LOG_INFO_P("Priority message");
    }

    // Finalize the logger.
    LOG_FINALIZE();

    return a.exec();
}
