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

#include "TreeItemBase.h"

struct Qtilities::CoreGui::TreeItemBasePrivateData {
    TreeItemBasePrivateData() : is_modified(false) { }

    bool is_modified;
};

Qtilities::CoreGui::TreeItemBase::TreeItemBase(const QString& name, QObject* parent) : QObject(parent), AbstractTreeItem() {
    itemBaseData = new TreeItemBasePrivateData;
    setObjectName(name);
}

Qtilities::CoreGui::TreeItemBase::~TreeItemBase() {
    delete itemBaseData;
}

bool Qtilities::CoreGui::TreeItemBase::isModified() const {
    return itemBaseData->is_modified;
}

void Qtilities::CoreGui::TreeItemBase::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    itemBaseData->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

bool Qtilities::CoreGui::TreeItemBase::setCategory(const QtilitiesCategory& category, TreeNode* tree_node) {
    if (AbstractTreeItem::setCategory(category,tree_node)) {
        QtilitiesCategory previous_category = AbstractTreeItem::getCategory();
        if (previous_category != category)
            setModificationState(true);
        return true;
    } else
        return false;
}

bool Qtilities::CoreGui::TreeItemBase::setCategory(const QtilitiesCategory& category, int observer_id) {
    QtilitiesCategory previous_category = AbstractTreeItem::getCategory();
    if (AbstractTreeItem::setCategory(category,observer_id)) {
        if (previous_category != AbstractTreeItem::getCategory())
            setModificationState(true);
        return true;
    } else
        return false;
}

Qtilities::Core::QtilitiesCategory Qtilities::CoreGui::TreeItemBase::getCategory(TreeNode* tree_node) const {
    return AbstractTreeItem::getCategory(tree_node);
}

Qtilities::Core::QtilitiesCategory Qtilities::CoreGui::TreeItemBase::getCategory(int observer_id) const {
    return AbstractTreeItem::getCategory(observer_id);
}

QString Qtilities::CoreGui::TreeItemBase::getCategoryString(const QString& sep, int observer_id) const {
    return AbstractTreeItem::getCategoryString(sep,observer_id);
}

bool Qtilities::CoreGui::TreeItemBase::setCategoryString(const QString& category_string, const QString& sep) {
    QtilitiesCategory previous_category = AbstractTreeItem::getCategory();
    if (AbstractTreeItem::setCategoryString(category_string,sep)) {
        if (previous_category != AbstractTreeItem::getCategory())
            setModificationState(true);
        return true;
    } else
        return false;
}

bool Qtilities::CoreGui::TreeItemBase::hasCategory() const {
    return AbstractTreeItem::hasCategory();
}

bool Qtilities::CoreGui::TreeItemBase::removeCategory(int observer_id) {
    if (AbstractTreeItem::removeCategory(observer_id)) {
        setModificationState(true);
        return true;
    } else
        return false;
}
