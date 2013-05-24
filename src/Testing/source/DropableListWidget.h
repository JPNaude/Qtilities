/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of the Qtilities Debug Plugin.
** See Dependencies.pri for the licensing requirements of the Debug plugin.
** Alternatively, see http://www.qtilities.org/licensing.html.
**
****************************************************************************/

#ifndef DROPABLE_LIST_WIDGET_H
#define DROPABLE_LIST_WIDGET_H

#include <QListView>

#include <Logger>

using namespace Qtilities::Logging;

namespace Qtilities {
    namespace Testing {
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

#endif // DROPABLE_LIST_WIDGET_H
