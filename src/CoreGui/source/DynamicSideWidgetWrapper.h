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
          \struct DynamicSideWidgetWrapperData
          \brief The DynamicSideWidgetWrapperData class stores private data used by the DynamicSideWidgetWrapper class.
         */
        struct DynamicSideWidgetWrapperData;

        /*!
        \class DynamicSideWidgetWrapper
        \brief A wrapper for side viewer widgets.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT DynamicSideWidgetWrapper : public QMainWindow {
            Q_OBJECT
        public:
            DynamicSideWidgetWrapper(QMap<QString, ISideViewerWidget*> text_iface_map, QString current_text, QWidget* parent = 0);
            ~DynamicSideWidgetWrapper();

        public slots:
            void handleCurrentIndexChanged(const QString& text);
            void handleActionClose_triggered();

        signals:
            void newSideWidgetRequest();

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::DynamicSideWidgetWrapper *ui;
            DynamicSideWidgetWrapperData* d;
        };
    }
}

#endif // SIDEWIDGETWRAPPER_H
