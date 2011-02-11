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

#ifndef DROPABLE_LIST_WIDGET_H
#define DROPABLE_LIST_WIDGET_H

#include <QListView>

#include <Logger>

namespace Qtilities {
    namespace Plugins {
        namespace Debug {
            using namespace Qtilities::Logging;

            /*!
              \class qti_private_DropableListWidget
              \brief Subclasses QListView with dropEvent reimplemented for plugin configuration set creation in the %Qtilities debug widget.
             */
            class qti_private_DropableListWidget : public QListView
            {
                Q_OBJECT

            public:
                explicit qti_private_DropableListWidget(const QString& plugin_list_type, QWidget *parent = 0);
                void mousePressEvent(QMouseEvent *event);
                void mouseMoveEvent(QMouseEvent *event);
                void dragEnterEvent(QDragEnterEvent *event);
                void dropEvent(QDropEvent *event);

            signals:
                void newMessage(Logger::MessageType type, const QString& msg);
                void listChanged(const QString& list_id);

            private:
                QString d_plugin_list_type;
                QPoint dragStartPosition;
            };

        }
    }
}

#endif // DROPABLE_LIST_WIDGET_H
