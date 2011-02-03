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

#include "DropableListWidget.h"

#include <QtilitiesExtensionSystem>

#include <QDropEvent>
#include <QStringListModel>

using namespace QtilitiesExtensionSystem;

Qtilities::Plugins::Debug::DropableListWidget::DropableListWidget(const QString& plugin_list_type, QWidget *parent) : QListView(parent) {
    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);
    d_plugin_list_type = plugin_list_type;
}

void Qtilities::Plugins::Debug::DropableListWidget::mousePressEvent(QMouseEvent *event)
 {
     if (event->button() == Qt::LeftButton)
         dragStartPosition = event->pos();

     QListView::mousePressEvent(event);
 }

void Qtilities::Plugins::Debug::DropableListWidget::mouseMoveEvent(QMouseEvent *event)
 {
    if (d_plugin_list_type != "Filtered") {
        if (!(event->buttons() & Qt::LeftButton))
            return;
        if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
             return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        QByteArray data(model()->data(currentIndex(),Qt::DisplayRole).toString().toStdString().data());

        mimeData->setData("text/plain", data);
        drag->setMimeData(mimeData);
        Qt::DropActions drop_actions = 0;
        drop_actions |= Qt::CopyAction;
        drop_actions |= Qt::MoveAction;
        drag->exec(drop_actions);
    }
 }

void Qtilities::Plugins::Debug::DropableListWidget::dragEnterEvent(QDragEnterEvent *event)
 {
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
 }

void Qtilities::Plugins::Debug::DropableListWidget::dropEvent(QDropEvent *event) {
    if (event) {
        const QMimeData* mime_data = event->mimeData();
        QStringListModel* string_list_model = qobject_cast<QStringListModel*> (model());
        if (string_list_model){
            QStringList list = string_list_model->stringList();
            // Find the plugin in the current session if it exists:
            IPlugin* plugin_iface = EXTENSION_SYSTEM->findPlugin(mime_data->text());
            // Check that it is not a core plugin:
            QString filter_name = mime_data->text();

            if (plugin_iface) {
                if (d_plugin_list_type == "Inactive") {
                    if (EXTENSION_SYSTEM->corePlugins().contains(plugin_iface->pluginName())) {
                        QString msg = "Plugin \"" + plugin_iface->pluginName() + "\" is a core plugin, it does not belong in the list of inactive plugins.";
                        emit newMessage(Logger::Warning, msg);
                    }
                } else if (d_plugin_list_type == "Filtered") {
                    if (EXTENSION_SYSTEM->corePlugins().contains(plugin_iface->pluginName())) {
                        QString msg = "Plugin \"" + plugin_iface->pluginName() + "\" is a core plugin, it does not belong in the list of filtered plugins.";
                        emit newMessage(Logger::Warning, msg);
                    }

                    // Get its file name:
                    QFileInfo fi(plugin_iface->pluginFileName());
                    filter_name = fi.baseName();
                    if (filter_name.endsWith("d"))
                        filter_name.chop(1);
                    filter_name.append("*");
                }
            }

            QString reg_exp = QString("%1").arg(filter_name);
            list << reg_exp;
            list.removeDuplicates();
            string_list_model->setStringList(list);
            emit listChanged(d_plugin_list_type);
        }

        event->acceptProposedAction();
    }
}
