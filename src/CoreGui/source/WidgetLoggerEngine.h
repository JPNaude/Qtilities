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

#ifndef WIDGETLOGGERENGINE_H
#define WIDGETLOGGERENGINE_H

#include "QtilitiesCoreGui_global.h"

#include <AbstractLoggerEngine.h>
#include <Factory.h>

#include <QList>
#include <QVariant>
#include <QPlainTextEdit>

class QWidget;

namespace Ui {
    class WidgetLoggerEngine;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Logging;
        using namespace Qtilities::Core;

        // ------------------------------------
        // Widget Logger Engine
        // ------------------------------------
        /*!
        \struct WidgetLoggerEnginePrivateData
        \brief A struct used by the WidgetLoggerEngine class to store data.
          */
        struct WidgetLoggerEnginePrivateData;

        /*!
        \class WidgetLoggerEngine
        \brief A logger engine which shows logged messages in a widget with a QPlainTextEdit widget.

        A logger engine which shows logged messages in a widget with a QPlainTextEdit widget.

        \note Clearing the log through clearLog() is supported by this logger engine.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT WidgetLoggerEngine : public AbstractLoggerEngine
        {
            Q_OBJECT

        public:
            WidgetLoggerEngine();
            ~WidgetLoggerEngine();

            //! Sets the window title used for this logger engine.
            /*!
              \note This function only does something usefull after the engine has been initialized.
              */
            void setWindowTitle(const QString& window_title);
            //! Gets the window title used for this logger engine.
            /*!
              \note This function only does something usefull after the engine has been initialized.
              */
            QString windowTitle() const;

            // AbstractLoggerEngine implementation
            bool initialize();
            void finalize();
            bool isInitialized() const;
            QString description() const;
            QString status() const;      
            // Logger widgets will always be created after LOG_INITIALIZE(), thus we won't be able to restore their settings.
            bool isFormattingEngineConstant() const { return true; }

            // WidgetLoggerEngine implementation
            QWidget* getWidget();

            // Make this class a factory item
            static Qtilities::CoreGui::FactoryItem<AbstractLoggerEngine, WidgetLoggerEngine> factory;
            //! Returns the QPlainTextEdit used by this widget logger engine. Through this reference you can add your own custom syntax highligter etc.
            QPlainTextEdit* plainTextEdit() const;

        public slots:
            void logMessage(const QString& message);
            void clearLog();

        private:
            WidgetLoggerEnginePrivateData* d;
        };
    }
}


#endif // WIDGETLOGGERENGINE_H
