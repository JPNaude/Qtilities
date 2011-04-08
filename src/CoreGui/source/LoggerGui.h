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

#ifndef LOGGER_GUI_H
#define LOGGER_GUI_H

#include "QtilitiesCoreGui_global.h"
#include "LoggerConfigWidget.h"
#include "WidgetLoggerEngine.h"
#include "WidgetLoggerEngineFrontend.h"

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

        This class allows the creation of log widgets through static functions. These widgets includes:
        - A configuration widget for the logger.
        - Log widget. When creating a log widget, you can specify which message types must be displayed in your widget. The function will automatically create a new widget logger engine for you and the widget which is returned will be ready to use.
        - Log dock widget. Same as log widget, except that it returns a dock widget with the log widget already inside it.

        Below is an example dock log widget. Note the ready to use Qtilities::CoreGui::SearchBoxWidget at the bottom of the log window.

        \image html log_dock_widget.jpg "Log Dock Widget"
        \image latex log_dock_widget.eps "Log Dock Widget" width=\textwidth
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT LoggerGui {
        public:
            LoggerGui() {}
            virtual ~LoggerGui() {}

            //! Creates a new logger widget which logs messages of the specified verbosity. The user must manage the widget instance.
            /*!
                \param engine_name The name of the engine.
                \param window_title The created window's title. By default an empty string is passed which will result in the engine_name being used.
                \param is_active Indicates if the engine must be active after it was created.
                \param message_types The message types

                \sa createTempLogWidget(), createLogDockWidget()
              */
            static QWidget* createLogWidget(const QString& engine_name, const QString& window_title = QString(), bool is_active = true, Logger::MessageTypeFlags message_types = Logger::AllLogLevels) {
                // Create a new logger widget engine and add it to the logger.
                WidgetLoggerEngine* new_widget_engine = new WidgetLoggerEngine;

                // Install a formatting engine for the new logger engine
                QString formatter = qti_def_FORMATTING_ENGINE_RICH_TEXT;
                AbstractFormattingEngine* formatting_engine = Log->formattingEngineReference(formatter);
                if (formatting_engine) {
                    new_widget_engine->installFormattingEngine(formatting_engine);
                } else {
                    delete new_widget_engine;
                    LOG_ERROR(QString(QObject::tr("Failed to create log widget engine. The specified formatting engine could not be found: %1")).arg(formatter));
                    return 0;
                }

                new_widget_engine->setName(engine_name);
                if (Log->attachLoggerEngine(new_widget_engine)) {
                    new_widget_engine->setActive(is_active);
                    new_widget_engine->setEnabledMessageTypes(message_types);
                    if (window_title.isEmpty())
                        new_widget_engine->setWindowTitle(engine_name);
                    else
                        new_widget_engine->setWindowTitle(window_title);
                    return new_widget_engine->getWidget();
                } else
                    return 0;
            }

            //! Creates a temporary logger widget which logs messages of the specified verbosity. The user must manage the widget instance.
            /*!
                Temporary log widgets are usefull when you want to given updates to the user during intensive processing. In that case you can create a temp log widget using this function and when you are done with your processing you can simply hide the widget causing it to be deleted.

                The returned widget is essentially the same as the widget created using createLogWidget(), except for the following:
                - It is active by default.
                - It has a parameter which you can specify its size explicity, the default is 1000x600
                - The widget is positioned in the middle of the screen.
                - The widget has its Qt::WA_QuitOnClose attribute set to false, thus if the temporary widget is still opened when the application is closed it will be destructed and the application will close. Note that this depends on the way widgets are managed in your application, by default the mentioned behavior will happen.
                - The widget is shown automatically.

                \param engine_name The name of the engine.
                \param window_title The created window's title. By default an empty string is passed which will result in the engine_name being used.
                \param is_active Indicates if the engine must be active after it was created.
                \param message_types The message types

              \sa createLogWidget(), createLogDockWidget()
              */
            static QWidget* createTempLogWidget(const QString& engine_name, const QString& window_title = QString(), Logger::MessageTypeFlags message_types = Logger::AllLogLevels, const QSize& size = QSize(1000,600)) {
                QWidget* new_widget = createLogWidget(engine_name,window_title,true,message_types);
                if (new_widget) {
                    // Resize:
                    new_widget->resize(size);

                    // Put the widget in the center of the screen:
                    QRect qrect = QApplication::desktop()->availableGeometry(new_widget);
                    new_widget->move(qrect.center() - new_widget->rect().center());

                    // Set its Qt::WA_QuitOnClose attribute to false
                    new_widget->setAttribute(Qt::WA_QuitOnClose,false);

                    // Show by default:
                    new_widget->show();
                }
                return new_widget;
            }

            //! Creates a new logger dock widget which logs messages of the specified verbosity.
            /*!
                This function is similar to createLogWidget() but it wraps the widget produced by createLogWidget() with a QDockWidget(). The user must manage the widget instance.

                \param engine_name The name of the engine.
                \param window_title The created window's title. By default an empty string is passed which will result in the engine_name being used.
                \param is_active Indicates if the engine must be active after it was created.
                \param message_types The message types

              \sa createLogWidget(), createTempLogWidget()
              */
            static QDockWidget* createLogDockWidget(const QString& engine_name, const QString& window_title = QString(), bool is_active = true, Logger::MessageTypeFlags message_types = Logger::AllLogLevels) {
                QDockWidget* log_dock_widget = new QDockWidget(window_title);
                QWidget* log_widget = createLogWidget(engine_name,window_title,is_active,message_types);
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
