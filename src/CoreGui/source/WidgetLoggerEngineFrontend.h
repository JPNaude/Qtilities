/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

#ifndef WIDGETLOGGERENGINEFRONTEND_H
#define WIDGETLOGGERENGINEFRONTEND_H

#include "SearchBoxWidget.h"
#include "QtilitiesCoreGui_global.h"
#include "WidgetLoggerEngine.h"

#include <IContext.h>

#include <QMainWindow>
#include <QPointer>
#include <QPrinter>

namespace Ui {
    class WidgetLoggerEngineFrontend;
}

using namespace Qtilities::Core::Interfaces;

namespace Qtilities {
    namespace CoreGui {
        struct MessagesPlainTextEditTabPrivateData;
        using namespace Qtilities::Core::Interfaces;

        /*!
        \class WidgetLoggerEngineFrontendTab
        \brief A QPlainTextEdit messages tab used in WidgetLoggerEngineFrontend.

        <i>This class was added in %Qtilities v1.2.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT MessagesPlainTextEditTab : public QMainWindow, public IContext {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContext)

        public:
            MessagesPlainTextEditTab(QWidget *parent = 0);
            ~MessagesPlainTextEditTab();
            bool eventFilter(QObject *object, QEvent *event);

            // --------------------------------
            // IContext Implementation
            // --------------------------------
            QString contextString() const { return globalMetaType(); }
            QString contextHelpId() const { return QString(); }

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            //! Sets the global meta type used for this widget.
            /*!
              \returns True if the meta_type string was valid. The validity check is done by checking if that a context with the same name does not yet exist in the context manager.

              \sa globalMetaType()
              */
            bool setGlobalMetaType(const QString& meta_type);
            //! Gets the global meta type used for this widget.
            /*!
              The global meta type is a string which defines this widget. The string must be a string which
              can be registered in the context manager. Thus, such a string must not yet exist as a context in the context
              manager.

              The global meta type is used for the following:
              - To register actions in the action manager.

              \returns The meta type used for this widget.

              \sa setGlobalMetaType()
              */
            QString globalMetaType() const;
            //! Returns the QPlainTextEdit used by this widget logger engine. Through this reference you can add your own custom syntax highligter etc.
            QPlainTextEdit* plainTextEdit() const;

            //! Clears the log.
            void clear();

        public slots:
            void appendMessage(const QString& message);

            // Slots to respond to signals from GUI elements
            void handle_FindPrevious();
            void handle_FindNext();
            void handle_Save();
            void handle_Settings();
            void handle_Print();
            void handle_PrintPDF();
            void handle_PrintPreview();
            void handle_Clear();
            void handle_Copy();
            void handle_SearchShortcut();
            void handle_SelectAll();
            void printPreview(QPrinter *printer);
            void handleSearchStringChanged(const QString& filter_string);

        private:
            void constructActions();

            MessagesPlainTextEditTabPrivateData* d;
        };

        struct WidgetLoggerEngineFrontendPrivateData;

        /*!
        \class WidgetLoggerEngineFrontend
        \brief The visual frontend of a widget logger engine.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT WidgetLoggerEngineFrontend : public QMainWindow {
            Q_OBJECT

        public:
            WidgetLoggerEngineFrontend(WidgetLoggerEngine::MessageDisplaysFlag message_displays_flag, QWidget *parent = 0);
            ~WidgetLoggerEngineFrontend();

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            //! Returns the QPlainTextEdit used by this widget logger engine for the specified message display.
            /*!
                Through this reference you can add your own custom syntax highligter etc.

                \note Only PlainTextEdit displays can be used with this function.
                */
            QPlainTextEdit* plainTextEdit(WidgetLoggerEngine::MessageDisplaysFlag message_display) const;

        public slots:
            void appendMessage(const QString& message, Logger::MessageType message_type = Logger::Info);
            void clear();
            void handle_dockVisibilityChanged(bool visible);

        private:
            MessagesPlainTextEditTab* plainTextEditTab(WidgetLoggerEngine::MessageDisplaysFlag message_display);
            WidgetLoggerEngineFrontendPrivateData* d;
        };
    }
}

#endif // WIDGETLOGGERENGINEFRONTEND_H
