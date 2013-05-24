/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of the Qtilities Debug Plugin.
** See Dependencies.pri for the licensing requirements of the Debug plugin.
** Alternatively, see http://www.qtilities.org/licensing.html.
**
****************************************************************************/

#include "DropableListWidget.h"

#include <QtilitiesExtensionSystem>

#include <QDropEvent>
#include <QStringListModel>

using namespace QtilitiesExtensionSystem;

Qtilities::Testing::qti_private_DropableListWidget::qti_private_DropableListWidget(const QString& plugin_list_type, QWidget *parent) : QListView(parent) {
    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);
    d_plugin_list_type = plugin_list_type;
}

void Qtilities::Testing::qti_private_DropableListWidget::mousePressEvent(QMouseEvent *event)
 {
     if (event->button() == Qt::LeftButton)
         dragStartPosition = event->pos();

     QListView::mousePressEvent(event);
 }

void Qtilities::Testing::qti_private_DropableListWidget::mouseMoveEvent(QMouseEvent *event)
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

void Qtilities::Testing::qti_private_DropableListWidget::dragEnterEvent(QDragEnterEvent *event)
 {
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
 }

void Qtilities::Testing::qti_private_DropableListWidget::dropEvent(QDropEvent *event) {
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
                if (d_plugin_list_type == QLatin1String("Inactive")) {
                    if (EXTENSION_SYSTEM->corePlugins().contains(plugin_iface->pluginName())) {
                        QString msg = "Plugin \"" + plugin_iface->pluginName() + "\" is a core plugin, it does not belong in the list of inactive plugins.";
                        emit newMessage(Logger::Warning, msg);
                    }
                } else if (d_plugin_list_type == QLatin1String("Filtered")) {
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
