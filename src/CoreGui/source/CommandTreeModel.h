/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
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
