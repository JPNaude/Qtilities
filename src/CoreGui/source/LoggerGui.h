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

#ifndef LOGGER_GUI_H
#define LOGGER_GUI_H

#include "QtilitiesCoreGui_global.h"
#include "LoggerConfigWidget.h"
#include "WidgetLoggerEngine.h"

#include <Logger.h>
#include <LoggingConstants.h>

#include <QWidget>
#include <QDockWidget>
#include <QPointer>
#include <QApplication>
#include <QDesktopWidget>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Logging;
        using namespace Qtilities::Logging::Constants;

        /*!
        \class LoggerGui
        \brief A class providing static member functions to acccess and create GUIs related to the %Qtilities Logging library.

        This class allows easy creation of log widgets through static functions. For example:
\code
// Create the widget:
QString engine_name = "Session Log"
QDockWidget* session_log_dock = LoggerGui::createLogDockWidget(&engine_name);
session_log_dock->show();

// We can then access the AbstractLoggerEngine created using the engine name:
AbstractLoggerEngine* engine = Log->loggerEngineReference(*engine_name);
\endcode

        The \p QDockWidget produced by the above code is shown below. Note the ready to use Qtilities::CoreGui::SearchBoxWidget at the bottom of the log window.

        \image html log_dock_widget.jpg "Log Dock Widget"
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT LoggerGui {
        public:
            LoggerGui() {}
            virtual ~LoggerGui() {}

            //! Creates and returns a new logger widget.
            /*!
                When creating new logger widgets through the LoggerGui class the name assigned to the log can change if a engine with the same name already exists. Therefore you pass
                the name to the logger as a pointer which will be updated with the new name if it was changed. When a engine with the same name exists the name will be changed by
                adding " (1)" to the engine (where the number will match an unused number).

                \param engine_name The name of the engine. If an engine with the same name already exists this function will assign an unique name to the new widget by appending a number to \p engine_name.
                \param message_displays_flag The message display flags indicating which message tabs should be shown in the logger widget.
                \param window_title The created window's title. By default an empty string is passed which will result in the engine_name being used.
                \param is_active Indicates if the engine must be active after it was created.
                \param message_types The message types which must be logged in this log widget.
                \param toolbar_area The toolbar area to use for action toolbars in the specified tabs. If no toolbars should be displayed, use Qt::NoToolBarArea.

                For example:

\code
// Get a widget that shows all log messages:
QString log_widget_name = tr("Session Log");
QWidget* log_widget = LoggerGui::createLogWidget(&log_widget_name);

// We can now access the AbstractLoggerEngine instance like this:
AbstractLoggerEngine* log_engine = Log->loggerEngineReference(log_widget_name);
Q_ASSERT(log_engine);
// As an example, lets change the message context to log only messages specifically logged to this engine:
log_engine->setMessageContexts(Logger::EngineSpecificMessages);
\endcode

                \note The user must manage the new widget instance.

                \sa createTempLogWidget(), createLogDockWidget()
              */
            static QWidget* createLogWidget(QString* engine_name,
                                            WidgetLoggerEngine::MessageDisplaysFlag message_displays_flag = WidgetLoggerEngine::DefaultDisplays,
                                            const QString& window_title = QString(),
                                            bool is_active = true,
                                            Logger::MessageTypeFlags message_types = Logger::AllLogLevels,
                                            Qt::ToolBarArea toolbar_area = Qt::TopToolBarArea) {
                if (!engine_name)
                    return 0;

                QString new_logger_name = *engine_name;
                int logger_count = -1;
                QStringList attached_logger_engine_names = Log->attachedLoggerEngineNames();
                while (attached_logger_engine_names.contains(new_logger_name)) {
                    if (logger_count > -1)
                        new_logger_name.chop(3 + QString::number(logger_count).length());
                    ++logger_count;
                    new_logger_name.append(" (" + QString::number(logger_count) + ")");
                }

                // Create a new logger widget engine and add it to the logger.
                WidgetLoggerEngine* new_widget_engine = new WidgetLoggerEngine(message_displays_flag,
                                                                               toolbar_area);

                // Install a formatting engine for the new logger engine
                QString formatter = qti_def_FORMATTING_ENGINE_RICH_TEXT;
                AbstractFormattingEngine* formatting_engine = Log->formattingEngineReference(formatter);
                if (formatting_engine) {
                    new_widget_engine->installFormattingEngine(formatting_engine);
                } else {
                    delete new_widget_engine;
                    LOG_DEBUG(QString("Failed to create log widget engine. The specified formatting engine could not be found: %1").arg(formatter));
                    return 0;
                }

                *engine_name  = new_logger_name;
                new_widget_engine->setName(new_logger_name);
                if (Log->attachLoggerEngine(new_widget_engine)) {
                    new_widget_engine->setActive(is_active);
                    new_widget_engine->setEnabledMessageTypes(message_types);
                    if (window_title.isEmpty())
                        new_widget_engine->setWindowTitle(new_logger_name);
                    else
                        new_widget_engine->setWindowTitle(window_title);
                    return new_widget_engine->getWidget();
                } else
                    return 0;
             }

            //! Creates and returns a temporary logger widget.
            /*!
                Temporary log widgets are usefull when you want to given updates to the user during intensive processing. In that case you can create a temp log widget using this function and when you are done with
                your processing you can simply close the widget causing it to be deleted (The Qt::WA_DeleteOnClose flag is set to true on the widget to enable this behaviour).

                The returned widget is essentially the same as the widget created using createLogWidget(), except for the following:
                - It is active by default.
                - It has a parameter which you can specify its size explicity, the default is 1000x600.
                - The widget is positioned in the middle of the screen.
                - The widget has its Qt::WA_QuitOnClose attribute set to false, thus if the temporary widget is still open when the application is closed it will be destructed
                  and the application will close. Note that this depends on the way widgets are managed in your application, by default the mentioned behavior will happen.

                \param engine_name The name of the engine. If an engine with the same name already exists this function will assign an unique name to the new widget by appending a number to \p engine_name.
                \param message_displays_flag The message display flags indicating which message tabs should be shown in the logger widget.
                \param window_title The created window's title. By default an empty string is passed which will result in the engine_name being used.
                \param message_types The message types which must be logged in this log widget.
                \param size The size of the widget.
                \param toolbar_area The toolbar area to use for action toolbars in the specified tabs. If no toolbars should be displayed, use Qt::NoToolBarArea.

                \note The user must manage the new widget instance.

              \sa createLogWidget(), createLogDockWidget()
              */
            static QWidget* createTempLogWidget(QString* engine_name,
                                                WidgetLoggerEngine::MessageDisplaysFlag message_displays_flag = WidgetLoggerEngine::DefaultDisplays,
                                                const QString& window_title = QString(),
                                                Logger::MessageTypeFlags message_types = Logger::AllLogLevels,
                                                const QSize& size = QSize(1000,600),
                                                Qt::ToolBarArea toolbar_area = Qt::TopToolBarArea) {
                QWidget* new_widget = createLogWidget(engine_name,
                                                      message_displays_flag,
                                                      window_title,
                                                      true,
                                                      message_types,
                                                      toolbar_area);
                if (new_widget) {
                    // Resize:
                    new_widget->resize(size);

                    // Put the widget in the center of the screen:
                    QRect qrect = QApplication::desktop()->availableGeometry(new_widget);
                    new_widget->move(qrect.center() - new_widget->rect().center());

                    // Set its Qt::WA_QuitOnClose attribute to false
                    new_widget->setAttribute(Qt::WA_QuitOnClose,false);
                    new_widget->setAttribute(Qt::WA_DeleteOnClose, true);
                }
                return new_widget;
            }

            //! Creates and returns a new logger dock widget.
            /*!
                This function is similar to createLogWidget() but it wraps the widget produced by createLogWidget() with a QDockWidget().

                \param engine_name The name of the engine. If an engine with the same name already exists this function will assign an unique name to the new widget by appending a number to \p engine_name.
                \param message_displays_flag The message display flags indicating which message tabs should be shown in the logger widget.
                \param window_title The created window's title. By default an empty string is passed which will result in the engine_name being used.
                \param is_active Indicates if the engine must be active after it was created.
                \param message_types The message types which must be logged in this log widget.
                \param toolbar_area The toolbar area to use for action toolbars in the specified tabs. If no toolbars should be displayed, use Qt::NoToolBarArea.

                \note The user must manage the new widget instance.

              \sa createLogWidget(), createTempLogWidget()
              */
            static QDockWidget* createLogDockWidget(QString* engine_name,
                                                    WidgetLoggerEngine::MessageDisplaysFlag message_displays_flag = WidgetLoggerEngine::DefaultDisplays,
                                                    const QString& window_title = QString(),
                                                    bool is_active = true,
                                                    Logger::MessageTypeFlags message_types = Logger::AllLogLevels,
                                                    Qt::ToolBarArea toolbar_area = Qt::TopToolBarArea) {
                if (!engine_name)
                    return 0;

                QString dock_name = window_title;
                if (window_title.isEmpty())
                    dock_name = *engine_name;

                QDockWidget* log_dock_widget = new QDockWidget(dock_name);
                QWidget* log_widget = createLogWidget(engine_name,
                                                      message_displays_flag,
                                                      window_title,
                                                      is_active,
                                                      message_types,
                                                      toolbar_area);
                if (log_widget) {
                    log_dock_widget->setWidget(log_widget);
                    QObject::connect(log_widget,SIGNAL(destroyed()),log_dock_widget,SLOT(deleteLater()));
                    return log_dock_widget;
                } else {
                    delete log_dock_widget;
                    return 0;
                }
            }

            //! Return a settings widget for the logger.
            static QWidget* createLoggerConfigWidget(bool applyButtonVisible = true) {
                LoggerConfigWidget* config_widget = new LoggerConfigWidget(applyButtonVisible);
                return config_widget;
            }
        };
    }
}

#endif // LOGGER_GUI_H
