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

#include "CommandTreeModel.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"
#include "Command.h"

#include <QtGui>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

Qtilities::CoreGui::qti_private_CommandTreeModel::qti_private_CommandTreeModel(QObject* parent) : ObserverTreeModel(parent) {

}

QVariant Qtilities::CoreGui::qti_private_CommandTreeModel::data(const QModelIndex &index, int role) const {
    if ((index.column() == columnCount() - 2) && (role == Qt::DisplayRole || role == Qt::ToolTipRole)) {
        QObject* obj = getObject(index);
        if (obj) {
            Command* command = qobject_cast<Command*> (obj);
            if (command) {
                return command->text();
            }
        }
        return QVariant();
    } else if ((index.column() == columnCount() - 1) && (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)) {
        QObject* obj = getObject(index);
        if (obj) {
            Command* command = qobject_cast<Command*> (obj);
            if (command) {
                return command->keySequence().toString();
            }
        }
        return QVariant();
    } else if ((index.column() == columnCount() - 1) && (role == Qt::ForegroundRole)) {
        QObject* obj = getObject(index);
        if (obj) {
            Command* command = qobject_cast<Command*> (obj);
            if (command) {
                if (ACTION_MANAGER->commandsWithKeySequence(command->keySequence()).count() > 1)
                    return QBrush(Qt::red);
            }
        }
        return QVariant();
    } else {
        return ObserverTreeModel::data(index,role);
    }
}

Qt::ItemFlags Qtilities::CoreGui::qti_private_CommandTreeModel::flags(const QModelIndex &index) const {
    if (index.column() == columnCount() - 2) {
        Qt::ItemFlags item_flags = 0;
        item_flags |= Qt::ItemIsEnabled;
        item_flags |= Qt::ItemIsSelectable;
        return item_flags;
    } else if (index.column() == columnCount() - 1) {
        Qt::ItemFlags item_flags = 0;
        item_flags |= Qt::ItemIsEnabled;
        item_flags |= Qt::ItemIsSelectable;
        item_flags |= Qt::ItemIsEditable;
        return item_flags;
    } else {
        return ObserverTreeModel::flags(index);
    }
}

QVariant Qtilities::CoreGui::qti_private_CommandTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((section == columnCount() - 2) && (role == Qt::DisplayRole)) {
        return QString("Label");
    } else if ((section == columnCount() - 1) && (role == Qt::DisplayRole)) {
        return QString("Shortcut");
    } else {
        return ObserverTreeModel::headerData(section,orientation,role);
    }
}

bool Qtilities::CoreGui::qti_private_CommandTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.column() == columnCount() - 2) {
        return false;
    } else if (index.column() == columnCount() - 1 && role == Qt::EditRole) {
        QObject* obj = getObject(index);
        if (obj) {
            Command* command = qobject_cast<Command*> (obj);
            if (command) {
                command->setKeySequence(QKeySequence(value.toString()));
                return true;
            }
        }
        return false;
    } else {
        return ObserverTreeModel::setData(index,value,role);
    }
}

int Qtilities::CoreGui::qti_private_CommandTreeModel::rowCount(const QModelIndex &parent) const {
    return ObserverTreeModel::rowCount(parent);
}

int Qtilities::CoreGui::qti_private_CommandTreeModel::columnCount(const QModelIndex &parent) const {
    return ObserverTreeModel::columnCount(parent) + 2;
}

// -----------------------------------------------
// qti_private_ShortcutEditorDelegate
// -----------------------------------------------

Qtilities::CoreGui::qti_private_ShortcutEditorDelegate::qti_private_ShortcutEditorDelegate(QObject *parent) : QItemDelegate(parent) {

}

QWidget *Qtilities::CoreGui::qti_private_ShortcutEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(option)
    Q_UNUSED(index)

    qti_private_QtKeySequenceEdit *editor = new qti_private_QtKeySequenceEdit(parent);
    return editor;
}

void Qtilities::CoreGui::qti_private_ShortcutEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    qti_private_QtKeySequenceEdit *shortcut_editor = static_cast<qti_private_QtKeySequenceEdit*>(editor);
    shortcut_editor->setKeySequence(QKeySequence(value));
}

void Qtilities::CoreGui::qti_private_ShortcutEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    qti_private_QtKeySequenceEdit *shortcut_editor = static_cast<qti_private_QtKeySequenceEdit*>(editor);
    QString value = shortcut_editor->keySequence().toString();
    model->setData(index, value, Qt::EditRole);
}

void Qtilities::CoreGui::qti_private_ShortcutEditorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

// -----------------------------------------------
// qti_private_QtKeySequenceEdit
// -----------------------------------------------
// This class is unmodified from the Qt Property
// Browser solution.
// -----------------------------------------------

Qtilities::CoreGui::qti_private_QtKeySequenceEdit::qti_private_QtKeySequenceEdit(QWidget *parent)
    : QWidget(parent), m_num(0), m_lineEdit(new QLineEdit(this))
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->setMargin(0);
    m_lineEdit->installEventFilter(this);
    m_lineEdit->setReadOnly(true);
    m_lineEdit->setFocusProxy(this);
    setFocusPolicy(m_lineEdit->focusPolicy());
    setAttribute(Qt::WA_InputMethodEnabled);
}

bool Qtilities::CoreGui::qti_private_QtKeySequenceEdit::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_lineEdit && e->type() == QEvent::ContextMenu) {
        QContextMenuEvent *c = static_cast<QContextMenuEvent *>(e);
        QMenu *menu = m_lineEdit->createStandardContextMenu();
        const QList<QAction *> actions = menu->actions();
        QListIterator<QAction *> itAction(actions);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            action->setShortcut(QKeySequence());
            QString actionString = action->text();
            const int pos = actionString.lastIndexOf(QLatin1Char('\t'));
            if (pos > 0)
                actionString.remove(pos, actionString.length() - pos);
            action->setText(actionString);
        }
        QAction *actionBefore = 0;
        if (actions.count() > 0)
            actionBefore = actions[0];
        QAction *clearAction = new QAction(tr("Clear Shortcut"), menu);
        menu->insertAction(actionBefore, clearAction);
        menu->insertSeparator(actionBefore);
        clearAction->setEnabled(!m_keySequence.isEmpty());
        connect(clearAction, SIGNAL(triggered()), this, SLOT(slotClearShortcut()));
        menu->exec(c->globalPos());
        delete menu;
        e->accept();
        return true;
    }

    return QWidget::eventFilter(o, e);
}

void Qtilities::CoreGui::qti_private_QtKeySequenceEdit::slotClearShortcut()
{
    if (m_keySequence.isEmpty())
        return;
    setKeySequence(QKeySequence());
    emit keySequenceChanged(m_keySequence);
}

void Qtilities::CoreGui::qti_private_QtKeySequenceEdit::handleKeyEvent(QKeyEvent *e)
{
    int nextKey = e->key();
    if (nextKey == Qt::Key_Control || nextKey == Qt::Key_Shift ||
            nextKey == Qt::Key_Meta || nextKey == Qt::Key_Alt ||
            nextKey == Qt::Key_Super_L || nextKey == Qt::Key_AltGr)
        return;

    nextKey |= translateModifiers(e->modifiers(), e->text());
    int k0 = m_keySequence[0];
    int k1 = m_keySequence[1];
    int k2 = m_keySequence[2];
    int k3 = m_keySequence[3];
    switch (m_num) {
        case 0: k0 = nextKey; k1 = 0; k2 = 0; k3 = 0; break;
        case 1: k1 = nextKey; k2 = 0; k3 = 0; break;
        case 2: k2 = nextKey; k3 = 0; break;
        case 3: k3 = nextKey; break;
        default: break;
    }
    ++m_num;
    if (m_num > 3)
        m_num = 0;
    m_keySequence = QKeySequence(k0, k1, k2, k3);
    m_lineEdit->setText(m_keySequence.toString(QKeySequence::NativeText));
    e->accept();
    emit keySequenceChanged(m_keySequence);
}

void Qtilities::CoreGui::qti_private_QtKeySequenceEdit::setKeySequence(const QKeySequence &sequence)
{
    if (sequence == m_keySequence)
        return;
    m_num = 0;
    m_keySequence = sequence;
    m_lineEdit->setText(m_keySequence.toString(QKeySequence::NativeText));
}

QKeySequence Qtilities::CoreGui::qti_private_QtKeySequenceEdit::keySequence() const
{
    return m_keySequence;
}

int Qtilities::CoreGui::qti_private_QtKeySequenceEdit::translateModifiers(Qt::KeyboardModifiers state, const QString &text) const
{
    int result = 0;
    if ((state & Qt::ShiftModifier) && (text.size() == 0 || !text.at(0).isPrint() || text.at(0).isLetter() || text.at(0).isSpace()))
        result |= Qt::SHIFT;
    if (state & Qt::ControlModifier)
        result |= Qt::CTRL;
    if (state & Qt::MetaModifier)
        result |= Qt::META;
    if (state & Qt::AltModifier)
        result |= Qt::ALT;
    return result;
}

void Qtilities::CoreGui::qti_private_QtKeySequenceEdit::focusInEvent(QFocusEvent *e)
{
    m_lineEdit->event(e);
    m_lineEdit->selectAll();
    QWidget::focusInEvent(e);
}

void Qtilities::CoreGui::qti_private_QtKeySequenceEdit::focusOutEvent(QFocusEvent *e)
{
    m_num = 0;
    m_lineEdit->event(e);
    QWidget::focusOutEvent(e);
}

void Qtilities::CoreGui::qti_private_QtKeySequenceEdit::keyPressEvent(QKeyEvent *e)
{
    handleKeyEvent(e);
    e->accept();
}

void Qtilities::CoreGui::qti_private_QtKeySequenceEdit::keyReleaseEvent(QKeyEvent *e)
{
    m_lineEdit->event(e);
}

bool Qtilities::CoreGui::qti_private_QtKeySequenceEdit::event(QEvent *e)
{
    if (e->type() == QEvent::Shortcut ||
            e->type() == QEvent::ShortcutOverride  ||
            e->type() == QEvent::KeyRelease) {
        e->accept();
        return true;
    }
    return QWidget::event(e);
}


