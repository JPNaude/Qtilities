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

#ifndef SIDEWIDGETWRAPPER_H
#define SIDEWIDGETWRAPPER_H

#include "QtilitiesCoreGui_global.h"
#include "ISideViewerWidget.h"

#include <QMainWindow>
#include <QMap>
#include <QString>

namespace Ui {
    class DynamicSideWidgetWrapper;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct DynamicSideWidgetWrapperPrivateData
          \brief The DynamicSideWidgetWrapperPrivateData class stores private data used by the DynamicSideWidgetWrapper class.
         */
        struct DynamicSideWidgetWrapperPrivateData;

        /*!
        \class DynamicSideWidgetWrapper
        \brief A wrapper for side viewer widgets.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT DynamicSideWidgetWrapper : public QWidget {
            Q_OBJECT
        public:
            //! Constructs a dynamic side widget wrapper widget.
            /*!
              \param text_iface_map The string-interface map which must be shown in this widget.
              \param current_text The current widget that is shown.
              \param is_exclusive Indicates if this wrapper is part of an exclusive side widget viewer.
              \param parent The parent of this widget.
              */
            DynamicSideWidgetWrapper(QMap<QString, ISideViewerWidget*> text_iface_map, const QString& current_text, const bool is_exclusive, QWidget* parent = 0);
            ~DynamicSideWidgetWrapper();

            //! Static function to access the style this widget uses for its combo boxes.
            /*!
             * <i>This function  was added in %Qtilities v1.2.</i>
             */
            static QLatin1String comboBoxStyle();

            //! Function which returns the current text in the combo box.
            QString currentText() const;
            //! Returns the current widget displayed in this wrapper.
            /*!
             * \return The current widget displayed in this wrapper.
             *
             * <i>This function  was added in %Qtilities v1.2.</i>
             */
            QWidget* currentWidget() const;

        public slots:
            void handleCurrentIndexChanged(const QString& text);
            //! This function expects a map of widgets which can still be produced.
            /*!
              \note The current widget should not be in map list.
              */
            void updateAvailableWidgets(QMap<QString, ISideViewerWidget*> text_iface_map);
            void close();

        signals:
            void currentTextChanged(const QString& text);
            void newSideWidgetRequest();
            void aboutToBeDestroyed(QWidget* widget);

        private:
            //! Refreshes the new widget action's state.
            void refreshNewWidgetAction();

            Ui::DynamicSideWidgetWrapper *ui;
            DynamicSideWidgetWrapperPrivateData* d;
        };
    }
}

#endif // SIDEWIDGETWRAPPER_H
