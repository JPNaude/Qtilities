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
    IFactoryData factoryData(FACTORY_QTILITIES,FACTORY_TAG_TREE_NODE,objectName());
    setFactoryData(factoryData);

    // Tree nodes always uses display hints:
    useDisplayHints();
}

Qtilities::CoreGui::TreeNode::~TreeNode() {
    delete nodeData;
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

Qtilities::Core::ActivityPolicyFilter* Qtilities::CoreGui::TreeNode::enableActivityControl(ObserverHints::ActivityDisplay activity_display,
                           ObserverHints::ActivityControl activity_control,
                           ActivityPolicyFilter::ActivityPolicy activity_policy,
                           ActivityPolicyFilter::MinimumActivityPolicy minimum_activity_policy,
                           ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy) {

    nodeData->activity_policy_filter = new ActivityPolicyFilter();
    nodeData->activity_policy_filter->setMinimumActivityPolicy(minimum_activity_policy);
    nodeData->activity_policy_filter->setNewSubjectActivityPolicy(new_subject_activity_policy);
    nodeData->activity_policy_filter->setActivityPolicy(activity_policy);
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

bool Qtilities::CoreGui::TreeNode::addItem(TreeItem* item, const QtilitiesCategory& category) {
    if (!item)
        return false;
    item->setCategory(category,this);
    return attachSubject(item);
}

bool Qtilities::CoreGui::TreeNode::addNode(TreeNode* node, const QtilitiesCategory& category) {
    if (!node)
        return false;
    node->setCategory(category,this);
    return attachSubject(node);
}

bool Qtilities::CoreGui::TreeNode::removeItem(const QString& name) {
    QObject* obj = subjectReference(name);
    if (obj)
        return detachSubject(obj);
    else
        return false;
}

bool Qtilities::CoreGui::TreeNode::removeItem(TreeItem* item) {
    return detachSubject(item);
}

bool Qtilities::CoreGui::TreeNode::removeNode(TreeNode* node) {
    return detachSubject(node);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeNode::saveToFile(const QString& file_name) const {
    QFile file(file_name);
    if(!file.open(QFile::WriteOnly))
        return IExportable::Failed;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Build up the doc:
    QString doc_title = QString("Qtilities::XML::ExportVersion::%1").arg(QTILITIES_XML_EXPORT_FORMAT);
    QDomDocument doc(doc_title);
    QDomElement root = doc.createElement("TreeNodeExport");
    doc.appendChild(root);
    QDomElement this_node = doc.createElement("InstanceProperties");
    root.appendChild(this_node);
    QDomCDATASection instanceName = doc.createCDATASection(objectName());
    this_node.appendChild(instanceName);
    // Do XML export in observer base class:
    QDomElement children = doc.createElement("Children");
    root.appendChild(children);
    exportXML(&doc,&children);

    // Put the complete doc in a string and save it to the file:
    QString docStr = doc.toString(2);
    file.write(docStr.toAscii());
    file.close();

    QApplication::restoreOverrideCursor();

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeNode::loadFromFile(const QString& file_name, bool clear_first) {
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
        LOG_ERROR(QString(tr("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string));
        file.close();
        QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    file.close();

    // Interpret the loaded doc:
    QDomElement root = doc.documentElement();
    QDomNodeList childNodes = root.childNodes();
    for(int i = 0; i < childNodes.count(); i++) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "InstanceProperties") {
            for(int i = 0; i < child.childNodes().count(); i++) {
                if (child.tagName() == "InstanceProperties") {
                    QDomNode node = child.childNodes().item(i);
                    QDomCDATASection cdata = node.toCDATASection();
                    if(cdata.isNull())
                        continue;

                    if (i == 0)
                        setObjectName(cdata.data());
                }
            }
        }

        if (child.tagName() == "Children") {
            // Do XML import in observer base class:
            importXML(&doc,&child);
        }
    }

    QApplication::restoreOverrideCursor();
    return IExportable::Complete;
}
