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

#include "TreeNode.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QApplication>
#include <QtXml>

using namespace Qtilities::CoreGui::Constants;

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QObject, TreeNode> TreeNode::factory;
    }
}

struct Qtilities::CoreGui::TreeNodeData {
    TreeNodeData() : naming_policy_filter(0) { }

    QPointer<NamingPolicyFilter> naming_policy_filter;
    QPointer<ActivityPolicyFilter> activity_policy_filter;
};

Qtilities::CoreGui::TreeNode::TreeNode(const QString& name) : Observer(name,""), AbstractTreeItem() {
    nodeData = new TreeNodeData;
    setObjectName(name);

    // Set the factory data in the observer base class:
    InstanceFactoryInfo instanceFactoryInfo(FACTORY_QTILITIES,FACTORY_TAG_TREE_NODE,objectName());
    setFactoryData(instanceFactoryInfo);

    // Tree nodes always use display hints:
    useDisplayHints();
}

Qtilities::CoreGui::TreeNode::~TreeNode() {
    delete nodeData;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeNode::exportFormattingXML(QDomDocument* doc, QDomElement* object_node) const {
    return saveFormattingToXML(doc,object_node);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeNode::importFormattingXML(QDomDocument* doc, QDomElement* object_node) {
    return loadFormattingFromXML(doc,object_node);
}

void Qtilities::CoreGui::TreeNode::setCategorizedDisplayEnabled(bool is_enabled) {
    displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
}

bool Qtilities::CoreGui::TreeNode::getCategorizedDisplayEnabled() const {
    if (displayHints()->hierarchicalDisplayHint() & ObserverHints::CategorizedHierarchy)
        return true;
    else
        return false;
}

Qtilities::CoreGui::NamingPolicyFilter* Qtilities::CoreGui::TreeNode::enableNamingControl(ObserverHints::NamingControl naming_control,
                         NamingPolicyFilter::UniquenessPolicy uniqueness_policy,
                         NamingPolicyFilter::ResolutionPolicy resolution_policy) {

    if (!nodeData->naming_policy_filter) {
        nodeData->naming_policy_filter = new NamingPolicyFilter();
        nodeData->naming_policy_filter->setUniquenessPolicy(uniqueness_policy);
        nodeData->naming_policy_filter->setUniquenessResolutionPolicy(resolution_policy);
        nodeData->naming_policy_filter->setValidityResolutionPolicy(resolution_policy);
        if (installSubjectFilter(nodeData->naming_policy_filter)) {
            displayHints()->setNamingControlHint(naming_control);
        } else {
            delete nodeData->naming_policy_filter;
        }
    }

    return nodeData->naming_policy_filter;
}

void Qtilities::CoreGui::TreeNode::disableNamingControl() {
    if (nodeData->naming_policy_filter) {
         delete nodeData->naming_policy_filter;
         displayHints()->setNamingControlHint(ObserverHints::NoNamingControlHint);
    }
}

Qtilities::CoreGui::NamingPolicyFilter* Qtilities::CoreGui::TreeNode::namingPolicyFilter() const {
    // If the pointer we have is 0, the activity filter might have been set using the observer base class.
    // Therefore we must check the subject filters in the base class here first:
    if (!nodeData->naming_policy_filter) {
        for (int i = 0; i < subjectFilters().count(); i++) {
            NamingPolicyFilter* naming_filter = qobject_cast<NamingPolicyFilter*> (subjectFilters().at(i));
            if (naming_filter)
                return naming_filter;
        }
    } else
        return nodeData->naming_policy_filter;

    return 0;
}

Qtilities::Core::ActivityPolicyFilter* Qtilities::CoreGui::TreeNode::enableActivityControl(ObserverHints::ActivityDisplay activity_display,
                           ObserverHints::ActivityControl activity_control,
                           ActivityPolicyFilter::ActivityPolicy activity_policy,
                           ActivityPolicyFilter::ParentTrackingPolicy parent_tracking_policy,
                           ActivityPolicyFilter::MinimumActivityPolicy minimum_activity_policy,
                           ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy) {

    nodeData->activity_policy_filter = new ActivityPolicyFilter();
    nodeData->activity_policy_filter->setMinimumActivityPolicy(minimum_activity_policy);
    nodeData->activity_policy_filter->setNewSubjectActivityPolicy(new_subject_activity_policy);
    nodeData->activity_policy_filter->setActivityPolicy(activity_policy);
    nodeData->activity_policy_filter->setParentTrackingPolicy(parent_tracking_policy);
    if (installSubjectFilter(nodeData->activity_policy_filter)) {
        displayHints()->setActivityControlHint(activity_control);
        displayHints()->setActivityDisplayHint(activity_display);
    } else {
        delete nodeData->activity_policy_filter;
    }

    return nodeData->activity_policy_filter;
}

void Qtilities::CoreGui::TreeNode::disableActivityControl() {
    if (nodeData->activity_policy_filter) {
         delete nodeData->activity_policy_filter;
         displayHints()->setActivityControlHint(ObserverHints::NoActivityControlHint);
         displayHints()->setActivityDisplayHint(ObserverHints::NoActivityDisplayHint);
    }
}

Qtilities::Core::ActivityPolicyFilter* Qtilities::CoreGui::TreeNode::activityPolicyFilter() const {
    // If the pointer we have is 0, the activity filter might have been set using the observer base class.
    // Therefore we must check the subject filters in the base class here first:
    if (!nodeData->activity_policy_filter) {
        for (int i = 0; i < subjectFilters().count(); i++) {
            ActivityPolicyFilter* activity_filter = qobject_cast<ActivityPolicyFilter*> (subjectFilters().at(i));
            if (activity_filter)
                return activity_filter;
        }
    } else
        return nodeData->activity_policy_filter;

    return 0;
}


void Qtilities::CoreGui::TreeNode::startProcessingCycle() {
    if (nodeData->naming_policy_filter)
        nodeData->naming_policy_filter->startValidationCycle();
    Observer::startProcessingCycle();
}

void Qtilities::CoreGui::TreeNode::endProcessingCycle() {
    if (nodeData->naming_policy_filter)
        nodeData->naming_policy_filter->endValidationCycle();
    Observer::endProcessingCycle();
}

Qtilities::CoreGui::TreeItem* Qtilities::CoreGui::TreeNode::addItem(const QString& name, const QtilitiesCategory& category) {
    TreeItem* new_item = new TreeItem(name);
    new_item->setCategory(category,this);
    if (attachSubject(new_item,Observer::ObserverScopeOwnership)) {
        return new_item;
    } else {
        delete new_item;
        return 0;
    }
}

Qtilities::CoreGui::TreeNode* Qtilities::CoreGui::TreeNode::addNode(const QString& name, const QtilitiesCategory& category) {
    TreeNode* new_node = new TreeNode(name);
    new_node->setCategory(category,this);
    if (attachSubject(new_node,Observer::ObserverScopeOwnership)) {
        return new_node;
    } else {
        delete new_node;
        return 0;
    }
}

bool Qtilities::CoreGui::TreeNode::addNodeFromFile(QString file_name, const QtilitiesCategory& category, QString* errorMsg) {
    TreeNode* new_node = new TreeNode();
    if (new_node->loadFromFile(file_name,errorMsg,false) != IExportable::Failed) {
        new_node->setCategory(category,this);
        return attachSubject(new_node,Observer::ObserverScopeOwnership);
    } else {
        delete new_node;
        return false;
    }
}

bool Qtilities::CoreGui::TreeNode::addItem(TreeItemBase* item, const QtilitiesCategory& category) {
    if (!item)
        return false;
    item->setCategory(category,this);
    return attachSubject(item,Observer::ObserverScopeOwnership);
}

bool Qtilities::CoreGui::TreeNode::addNode(TreeNode* node, const QtilitiesCategory& category) {
    if (!node)
        return false;
    node->setCategory(category,this);
    return attachSubject(node,Observer::ObserverScopeOwnership);
}

bool Qtilities::CoreGui::TreeNode::removeItem(const QString& name) {
    QObject* obj = subjectReference(name);
    if (obj)
        return detachSubject(obj);
    else
        return false;
}

bool Qtilities::CoreGui::TreeNode::removeItem(TreeItemBase* item) {
    return detachSubject(item);
}

bool Qtilities::CoreGui::TreeNode::removeNode(TreeItemBase* node) {
    return detachSubject(node);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeNode::saveToFile(const QString& file_name, QString* errorMsg) const {
    QFile file(file_name);
    if(!file.open(QFile::WriteOnly)) {
        if (errorMsg)
            *errorMsg = QString(tr("TreeNode could not be saved to file. File \"%1\" could not be opened in WriteOnly mode.")).arg(file_name);
        return IExportable::Failed;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Create the QDomDocument:
    QDomDocument doc("QtilitiesTreeDoc");
    QDomElement root = doc.createElement("QtilitiesTree");
    root.setAttribute("DocumentVersion",QTILITIES_XML_EXPORT_FORMAT);
    doc.appendChild(root);

    // Do XML export in observer base class:
    QDomElement rootItem = doc.createElement("Root");
    root.appendChild(rootItem);
    IExportable::Result result = exportXML(&doc,&rootItem);
    if (result == IExportable::Failed && errorMsg)
        *errorMsg = QString(tr("XML exporting on the observer base class failed. Please see the log for details."));

    // Put the complete doc in a string and save it to the file:
    // Still write it even if it fails so that we can check the output file for debugging purposes.
    QString docStr = doc.toString(2);
    file.write(docStr.toAscii());
    file.close();

    QApplication::restoreOverrideCursor();
    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeNode::loadFromFile(const QString& file_name, QString* errorMsg, bool clear_first) {
    if (clear_first)
        deleteAll();

    // Load the file into doc:
    QDomDocument doc("QtilitiesTreeExport");
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
        return IExportable::Failed;
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString docStr = file.readAll();
    QString error_string;
    int error_line;
    int error_column;
    if (!doc.setContent(docStr,&error_string,&error_line,&error_column)) {
        if (errorMsg)
            *errorMsg = QString(tr("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string);
        LOG_ERROR(QString(tr("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string));
        file.close();
        QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    file.close();

    startProcessingCycle();

    // Interpret the loaded doc:
    QDomElement root = doc.documentElement();

    // Check the document version:
    if (root.hasAttribute("DocumentVersion")) {
        QString document_version = root.attribute("DocumentVersion");
        if (document_version.toInt() > QTILITIES_XML_EXPORT_FORMAT) {
            if (errorMsg)
                *errorMsg = QString(tr("The DocumentVersion of the input file is not supported by this version of your application. The document version of the input file is %1, while supported versions are versions up to %2. The document will not be parsed.")).arg(document_version.toInt()).arg(QTILITIES_XML_EXPORT_FORMAT);
            LOG_ERROR(QString(tr("The DocumentVersion of the input file is not supported by this version of your application. The document version of the input file is %1, while supported versions are versions up to %2. The document will not be parsed.")).arg(document_version.toInt()).arg(QTILITIES_XML_EXPORT_FORMAT));
            QApplication::restoreOverrideCursor();
            return IExportable::Failed;
        }
    } else {
        if (errorMsg)
            *errorMsg = QString(tr("The DocumentVersion of the input file could not be determined. This might indicate that the input file is in the wrong format. The document will not be parsed."));
        LOG_ERROR(QString(tr("The DocumentVersion of the input file could not be determined. This might indicate that the input file is in the wrong format. The document will not be parsed.")));
        QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }

    // Now check out all the children below the root node:
    IExportable::Result result = IExportable::Complete;
    QList<QPointer<QObject> > internal_import_list;
    QDomNodeList childNodes = root.childNodes();
    for(int i = 0; i < childNodes.count(); i++) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "Root") {
            // Restore the instance name:
            if (child.hasAttribute("Name"))
                setObjectName(child.attribute("Name"));

            // Do import on observer base class:
            IExportable::Result intermediate_result = importXML(&doc,&child,internal_import_list);
            if (intermediate_result == IExportable::Failed) {
                if (errorMsg)
                    *errorMsg = QString(tr("XML importing on the observer base class failed. Please see the log for details."));
                result = intermediate_result;
                // Handle deletion of internal_import_list;
                // Delete the first item in the list (the top item) and the rest should be deleted.
                // For the subjects with manual ownership we delete the remaining items in the list manually.
                while (internal_import_list.count() > 0) {
                    if (internal_import_list.at(0) != 0) {
                        delete internal_import_list.at(0);
                        internal_import_list.removeAt(0);
                    } else{
                        internal_import_list.removeAt(0);
                    }
                }
                break;
            } else if (intermediate_result == IExportable::Incomplete)
                result = intermediate_result;
        }
    }

    endProcessingCycle();
    refreshViewsLayout();

    QApplication::restoreOverrideCursor();
    return result;
}
