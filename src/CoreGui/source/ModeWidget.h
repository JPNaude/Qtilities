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

#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include "QtilitiesCoreGuiConstants.h"
#include "IMode.h"

#include <QWidget>

namespace Ui {
    class ModeWidget;
}

class QListWidgetItem;

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct ModeWidgetData
          \brief The ModeWidgetData class stores private data used by the ModeWidget class.
         */
        struct ModeWidgetData;

        /*!
        \class ModeWidget
        \brief The mode widget.
          */
        class ModeWidget : public QWidget {
            Q_OBJECT
        public:
            ModeWidget(QWidget *parent = 0);
            ~ModeWidget();

            //! Adds a mode to the mode widget.
            bool addMode(IMode* mode, bool initialize_mode = true);
            //! Adds a list of modes to the main window.
            void addModes(QList<IMode*> modes, bool initialize_modes = true);
            //! Adds a list of modes to the main window. This call will attempt to cast each object in the list to IMode* and add the successfull interfaces to the main window.
            void addModes(QList<QObject*> modes, bool initialize_modes = true);
            //! A list of the modes in this mode widget.
            QList<IMode*> modes();

        private slots:
            void handleModeListCurrentItemChanged(QListWidgetItem * item);
            void handleModeChangeRequest(int new_mode);

        signals:
            void changeCentralWidget(QWidget* new_central_widget);

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::ModeWidget *ui;
            ModeWidgetData* d;
        };
    }
}

#endif // MODEWIDGET_H
