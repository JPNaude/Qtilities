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

#ifndef PLUGININFOWIDGET_H
#define PLUGININFOWIDGET_H

#include <QWidget>
#include "IPlugin.h"

namespace Ui {
    class PluginInfoWidget;
}
using namespace Qtilities::ExtensionSystem::Interfaces;

namespace Qtilities {
    namespace ExtensionSystem {

        /*!
          \class PluginInfoWidget
          \brief A widget that shows information about a plugin.

          <i>This class was added in %Qtilities v0.2.</i>
         */
        class PluginInfoWidget : public QWidget
        {
            Q_OBJECT

        public:
            explicit PluginInfoWidget(IPlugin* plugin, QWidget *parent = 0);
            ~PluginInfoWidget();

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::PluginInfoWidget *ui;
        };
    }
}

#endif // PLUGININFOWIDGET_H
