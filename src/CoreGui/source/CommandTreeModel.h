/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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

#ifndef COMMAND_TREE_MODEL_H
#define COMMAND_TREE_MODEL_H

#include "ObserverTreeModel.h"

#include <QMutex>
#include <QAbstractTableModel>
#include <QItemSelection>

namespace Qtilities {
    namespace CoreGui {
        // -----------------------------------------------
        // CommandTableModel
        // -----------------------------------------------
        class qti_private_CommandTreeModel : public ObserverTreeModel
        {
            Q_OBJECT

        public:
            qti_private_CommandTreeModel(QObject* parent = 0);
            virtual ~qti_private_CommandTreeModel() {}

            // --------------------------------
            // ObserverTableModel Implementation
            // --------------------------------
            Qt::ItemFlags flags(const QModelIndex &index) const;
            QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
            QVariant data(const QModelIndex &index, int role) const;
            int rowCount(const QModelIndex &parent = QModelIndex()) const;
            int columnCount(const QModelIndex &parent = QModelIndex()) const;
            bool setData(const QModelIndex &index, const QVariant &value, int role);
        };

        // -----------------------------------------------
        // qti_private_ShortcutEditorDelegate
        // -----------------------------------------------
        class qti_private_ShortcutEditorDelegate : public QItemDelegate
        {
            Q_OBJECT
        public:
            qti_private_ShortcutEditorDelegate(QObject *parent = 0);

            QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
            void setEditorData(QWidget *editor, const QModelIndex &index) const;
            void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
            void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        };

        // -----------------------------------------------
        // qti_private_QtKeySequenceEdit
        // -----------------------------------------------
        // This class is unmodified from the Qt Property
        // Browser solution.
        // -----------------------------------------------
        class qti_private_QtKeySequenceEdit : public QWidget
        {
            Q_OBJECT
        public:
            qti_private_QtKeySequenceEdit(QWidget *parent = 0);

            QKeySequence keySequence() const;
            bool eventFilter(QObject *o, QEvent *e);
        public Q_SLOTS:
            void setKeySequence(const QKeySequence &sequence);
        Q_SIGNALS:
            void keySequenceChanged(const QKeySequence &sequence);
        protected:
            void focusInEvent(QFocusEvent *e);
            void focusOutEvent(QFocusEvent *e);
            void keyPressEvent(QKeyEvent *e);
            void keyReleaseEvent(QKeyEvent *e);
            bool event(QEvent *e);
        private slots:
            void slotClearShortcut();
        private:
            void handleKeyEvent(QKeyEvent *e);
            int translateModifiers(Qt::KeyboardModifiers state, const QString &text) const;

            int m_num;
            QKeySequence m_keySequence;
            QLineEdit *m_lineEdit;
        };
    }
}

#endif // COMMAND_TREE_MODEL_H
