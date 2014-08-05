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

#include "TreeNode.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"

#include <QApplication>
#include <QtXml>

using namespace Qtilities::CoreGui::Constants;

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QObject, TreeNode> TreeNode::factory;
    }
}

struct Qtilities::CoreGui::TreeNodePrivateData {
    TreeNodePrivateData() { }

    QPointer<NamingPolicyFilter>    naming_policy_filter;
    QPointer<ActivityPolicyFilter>  activity_policy_filter;
    QPointer<SubjectTypeFilter>     subject_type_filter;
};

Qtilities::CoreGui::TreeNode::TreeNode(const QString& name, QObject* parent) : Observer(name,QString(),parent), AbstractTreeItem() {
    nodeData = new TreeNodePrivateData;
    setObjectName(name);

    // Set the factory data in the observer base class:
    InstanceFactoryInfo instanceFactoryInfo(qti_def_FACTORY_QTILITIES,qti_def_FACTORY_TAG_TREE_NODE,objectName());
    setFactoryData(instanceFactoryInfo);

    // Tree nodes always use display hints:
    useDisplayHints();
}

Qtilities::CoreGui::TreeNode::~TreeNode() {
    delete nodeData;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeNode::exportFormattingXML(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version) const {
    return saveFormattingToXML(doc,object_node,version);
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeNode::importFormattingXML(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version) {
    return loadFormattingFromXML(doc,object_node,version);
}

bool Qtilities::CoreGui::TreeNode::setCategory(const QtilitiesCategory& category, TreeNode* tree_node) {
    if (AbstractTreeItem::setCategory(category,tree_node)) {
        setModificationState(true);
        return true;
    } else
        return false;
}

bool Qtilities::CoreGui::TreeNode::setCategory(const QtilitiesCategory& category, int observer_id) {
    if (AbstractTreeItem::setCategory(category,observer_id)) {
        setModificationState(true);
        return true;
    } else
        return false;
}

void Qtilities::CoreGui::TreeNode::enableCategorizedDisplay(ObserverHints::CategoryEditingFlags category_editing_flags) {
    displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    if (category_editing_flags != ObserverHints::CategoriesNoEditingFlags)
        displayHints()->setCategoryEditingFlags(category_editing_flags);
}

void Qtilities::CoreGui::TreeNode::disableCategorizedDisplay() {
    displayHints()->setHierarchicalDisplayHint(ObserverHints::FlatHierarchy);
}

bool Qtilities::CoreGui::TreeNode::getCategorizedDisplayEnabled() const {
    if (displayHints()->hierarchicalDisplayHint() & ObserverHints::CategorizedHierarchy)
        return true;
    else
        return false;
}

Qtilities::CoreGui::NamingPolicyFilter* Qtilities::CoreGui::TreeNode::enableNamingControl(ObserverHints::NamingControl naming_control,
                         NamingPolicyFilter::UniquenessPolicy uniqueness_policy,
                         NamingPolicyFilter::ResolutionPolicy uniqueness_resolution_policy,
                         NamingPolicyFilter::ResolutionPolicy validity_resolution_policy) {

    if (!nodeData->naming_policy_filter) {
        nodeData->naming_policy_filter = new NamingPolicyFilter();
        nodeData->naming_policy_filter->setUniquenessPolicy(uniqueness_policy);
        nodeData->naming_policy_filter->setUniquenessResolutionPolicy(uniqueness_resolution_policy);
        nodeData->naming_policy_filter->setValidityResolutionPolicy(validity_resolution_policy);
        if (installSubjectFilter(nodeData->naming_policy_filter)) {
            displayHints()->setNamingControlHint(naming_control);
        } else {
            delete nodeData->naming_policy_filter;
            nodeData->naming_policy_filter = 0;
        }
    }

    return nodeData->naming_policy_filter;
}

void Qtilities::CoreGui::TreeNode::disableNamingControl() {
    if (nodeData->naming_policy_filter) {
        uninstallSubjectFilter(nodeData->naming_policy_filter);
        delete nodeData->naming_policy_filter;
        displayHints()->setNamingControlHint(ObserverHints::NoNamingControlHint);
    }
}

Qtilities::CoreGui::NamingPolicyFilter* Qtilities::CoreGui::TreeNode::namingPolicyFilter() const {
    // If the pointer we have is 0, the activity filter might have been set using the observer base class.
    // Therefore we must check the subject filters in the base class here first:
    if (!nodeData->naming_policy_filter) {
        for (int i = 0; i < subjectFilters().count(); ++i) {
            NamingPolicyFilter* naming_filter = qobject_cast<NamingPolicyFilter*> (subjectFilters().at(i));
            if (naming_filter) {
                nodeData->naming_policy_filter = naming_filter;
                return naming_filter;
            }
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
                           ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy,
                           bool monitor_modification_state) {

    nodeData->activity_policy_filter = new ActivityPolicyFilter();
    nodeData->activity_policy_filter->setMinimumActivityPolicy(minimum_activity_policy);
    nodeData->activity_policy_filter->setNewSubjectActivityPolicy(new_subject_activity_policy);
    nodeData->activity_policy_filter->setActivityPolicy(activity_policy);
    nodeData->activity_policy_filter->setParentTrackingPolicy(parent_tracking_policy);
    nodeData->activity_policy_filter->setIsModificationStateMonitored(monitor_modification_state);
    if (installSubjectFilter(nodeData->activity_policy_filter)) {
        displayHints()->setActivityControlHint(activity_control);
        displayHints()->setActivityDisplayHint(activity_display);
    } else {
        delete nodeData->activity_policy_filter;
        nodeData->activity_policy_filter = 0;
    }

    return nodeData->activity_policy_filter;
}

void Qtilities::CoreGui::TreeNode::disableActivityControl() {
    if (nodeData->activity_policy_filter) {
        uninstallSubjectFilter(nodeData->activity_policy_filter);
        delete nodeData->activity_policy_filter;
        displayHints()->setActivityControlHint(ObserverHints::NoActivityControlHint);
        displayHints()->setActivityDisplayHint(ObserverHints::NoActivityDisplayHint);
    }
}

Qtilities::Core::ActivityPolicyFilter* Qtilities::CoreGui::TreeNode::activityPolicyFilter() const {
    // If the pointer we have is 0, the activity filter might have been set using the observer base class.
    // Therefore we must check the subject filters in the base class here first:
    if (!nodeData->activity_policy_filter) {
        for (int i = 0; i < subjectFilters().count(); ++i) {
            ActivityPolicyFilter* activity_filter = qobject_cast<ActivityPolicyFilter*> (subjectFilters().at(i));
            if (activity_filter) {
                nodeData->activity_policy_filter = activity_filter;
                return activity_filter;
            }
        }
    } else
        return nodeData->activity_policy_filter;

    return 0;
}

Qtilities::Core::SubjectTypeFilter* Qtilities::CoreGui::TreeNode::setChildType(const QString& child_group_name) {
    nodeData->subject_type_filter = new SubjectTypeFilter(child_group_name);
    nodeData->subject_type_filter->enableInverseFiltering(true);
    if (!installSubjectFilter(nodeData->subject_type_filter)) {
        delete nodeData->subject_type_filter;
        nodeData->subject_type_filter = 0;
    }

    return nodeData->subject_type_filter;
}

void Qtilities::CoreGui::TreeNode::clearChildType() {
    if (nodeData->subject_type_filter) {
         delete nodeData->subject_type_filter;
    }
}

Qtilities::Core::SubjectTypeFilter* Qtilities::CoreGui::TreeNode::subjectTypeFilter() const {
    // If the pointer we have is 0, the subject type filter might have been set using the observer base class.
    // Therefore we must check the subject filters in the base class here first:
    if (!nodeData->subject_type_filter) {
        for (int i = 0; i < subjectFilters().count(); ++i) {
            SubjectTypeFilter* type_filter = qobject_cast<SubjectTypeFilter*> (subjectFilters().at(i));
            if (type_filter) {
                nodeData->subject_type_filter = type_filter;
                return type_filter;
            }
        }
    } else
        return nodeData->subject_type_filter;

    return 0;
}

void Qtilities::CoreGui::TreeNode::startProcessingCycle() {
    Observer::startProcessingCycle();
    if (isProcessingCycleActive()) {
        if (nodeData->naming_policy_filter)
            nodeData->naming_policy_filter->startValidationCycle();
    }
}

void Qtilities::CoreGui::TreeNode::endProcessingCycle(bool broadcast) {
    Observer::endProcessingCycle(broadcast);
    if (!isProcessingCycleActive()) {
        if (nodeData->naming_policy_filter)
            nodeData->naming_policy_filter->endValidationCycle();
    }
}

void Qtilities::CoreGui::TreeNode::startTreeProcessingCycle() {
    Observer::startTreeProcessingCycle();
    if (isProcessingCycleActive()) {
        if (nodeData->naming_policy_filter)
            nodeData->naming_policy_filter->startValidationCycle();
    }
}

void Qtilities::CoreGui::TreeNode::endTreeProcessingCycle(bool broadcast) {
    Observer::endTreeProcessingCycle(broadcast);
    if (!isProcessingCycleActive()) {
        if (nodeData->naming_policy_filter)
            nodeData->naming_policy_filter->endValidationCycle();
    }
}

Qtilities::CoreGui::TreeItem* Qtilities::CoreGui::TreeNode::addItem(const QString& name, const QtilitiesCategory& category) {
    TreeItem* new_item = new TreeItem(name);
    if (attachSubject(new_item,Observer::SpecificObserverOwnership)) {
        new_item->setCategory(category,this);
        new_item->setModificationState(false);
        return new_item;
    } else {
        delete new_item;
        return 0;
    }
}

void Qtilities::CoreGui::TreeNode::addItems(const QStringList& items, const QtilitiesCategory& category) {
    startProcessingCycle();
    foreach (const QString& item, items) {
        addItem(item,category);
    }
    endProcessingCycle();
}

Qtilities::CoreGui::TreeNode* Qtilities::CoreGui::TreeNode::addNode(const QString& name, const QtilitiesCategory& category) {
    TreeNode* new_node = new TreeNode(name);
    if (attachSubject(new_node,Observer::SpecificObserverOwnership)) {
        new_node->setCategory(category,this);
        new_node->setModificationState(false);
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
        new_node->setModificationState(false);
        return attachSubject(new_node,Observer::ObserverScopeOwnership);
    } else {
        delete new_node;
        return false;
    }
}

bool Qtilities::CoreGui::TreeNode::addItem(TreeItemBase* item, const QtilitiesCategory& category) {
    if (!item)
        return false;
    bool attach_success = attachSubject(item,Observer::ObserverScopeOwnership);
    item->setCategory(category,this);
    return attach_success;
}

bool Qtilities::CoreGui::TreeNode::addNode(TreeNode* node, const QtilitiesCategory& category) {
    if (!node)
        return false;
    bool attach_success = attachSubject(node,Observer::ObserverScopeOwnership);
    node->setCategory(category,this);
    return attach_success;
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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeNode::saveToFile(const QString& file_name, QString* errorMsg, ObserverData::ExportItemFlags export_flags) const {
    QFile file(file_name);
    if (!file.open(QFile::WriteOnly)) {
        if (errorMsg)
            *errorMsg = QString(tr("TreeNode could not be saved to file. File \"%1\" could not be opened in WriteOnly mode.")).arg(file_name);
        LOG_ERROR(QString(tr("TreeNode could not be saved to file. File \"%1\" could not be opened in WriteOnly mode.")).arg(file_name));
        return IExportable::Failed;
    }

    //QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Create the QDomDocument:
    QDomDocument doc("QtilitiesTreeExport");
    QDomElement root = doc.createElement("QtilitiesTree");
    doc.appendChild(root);

    // ---------------------------------------------------
    // Save file format information:
    // ---------------------------------------------------
    root.setAttribute("ExportVersion",QString::number(exportVersion()));
    root.setAttribute("QtilitiesVersion",CoreGui::QtilitiesApplication::qtilitiesVersionString());
    root.setAttribute("ApplicationExportVersion",QString::number(applicationExportVersion()));
    root.setAttribute("ApplicationVersion",QApplication::applicationVersion());
    root.setAttribute("ApplicationName",QApplication::applicationName());

    // ---------------------------------------------------
    // Do the actual export:
    // ---------------------------------------------------
    QDomElement rootItem = doc.createElement("Root");
    root.appendChild(rootItem);
    IExportable::ExportResultFlags result = exportXmlExt(&doc,&rootItem,export_flags);
    if (result == IExportable::Failed && errorMsg)
        *errorMsg = QString(tr("XML exporting on the observer base class failed. Please see the log for details."));

    // Put the complete doc in a string and save it to the file:
    // Still write it even if it fails so that we can check the output file for debugging purposes.
    QString docStr = doc.toString(2);
    docStr.prepend("<!--Created by " + QApplication::applicationName() + " v" + QApplication::applicationVersion() + " on " + QDateTime::currentDateTime().toString() + "-->\n");
    docStr.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    if (file.write(docStr.toUtf8()) == -1) {
        file.close();
        if (errorMsg)
            *errorMsg = QString(tr("Failed to write to output file during tree node export:")).arg(file_name);
        LOG_ERROR(QString(tr("Failed to write to output file during tree node export:")).arg(file_name));
        return IExportable::Failed;
    }

    file.close();

    //QApplication::restoreOverrideCursor();
    return result;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeNode::loadFromFile(const QString& file_name, QString* errorMsg, bool clear_first) {
    if (clear_first)
        deleteAll();

    // Load the file into doc:
    QDomDocument doc("QtilitiesTreeExport");
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMsg)
            *errorMsg = QString(tr("TreeNode could not be loaded from file. File \"%1\" could not be opened in ReadOnly mode.")).arg(file_name);
        LOG_ERROR(QString(tr("TreeNode could not be loaded from file. File \"%1\" could not be opened in ReadOnly mode.")).arg(file_name));
        return IExportable::Failed;
    }
    
    //QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString docStr = file.readAll();
    QString error_string;
    int error_line;
    int error_column;
    if (!doc.setContent(docStr,&error_string,&error_line,&error_column)) {
        if (errorMsg)
            *errorMsg = QString(tr("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string);
        LOG_ERROR(QString(tr("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string));
        file.close();
        //QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    file.close();

    startProcessingCycle();

    // Interpret the loaded doc:
    QDomElement root = doc.documentElement();

    // ---------------------------------------------------
    // Inspect file format:
    // ---------------------------------------------------
    Qtilities::ExportVersion read_version;
    if (root.hasAttribute("ExportVersion")) {
        read_version = (Qtilities::ExportVersion) root.attribute("ExportVersion").toInt();
        LOG_INFO(QString(tr("Inspecting tree node file format: Qtilities export format version: %1")).arg(read_version));
    } else {
        if (errorMsg)
            *errorMsg = QString(tr("The export version of the input file could not be determined. This might indicate that the input file is in the wrong format. The tree node file will not be parsed."));
        LOG_ERROR(QString(tr("The export version of the input file could not be determined. This might indicate that the input file is in the wrong format. The tree node file will not be parsed.")));
        //QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    if (root.hasAttribute("QtilitiesVersion"))
        LOG_INFO(QString(tr("Inspecting tree node file format: Qtilities version used to save the file: %1")).arg(root.attribute("QtilitiesVersion")));
    quint32 application_read_version = 0;
    if (root.hasAttribute("ApplicationExportVersion")) {
        application_read_version = root.attribute("ApplicationExportVersion").toInt();
        LOG_INFO(QString(tr("Inspecting tree node file format: Application export format version: %1")).arg(application_read_version));
    } else {
        if (errorMsg)
            *errorMsg = QString(tr("The application export version of the input file could not be determined. This might indicate that the input file is in the wrong format. The tree node file will not be parsed."));
        LOG_ERROR(QString(tr("The application export version of the input file could not be determined. This might indicate that the input file is in the wrong format. The tree node file will not be parsed.")));
        //QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    if (root.hasAttribute("ApplicationVersion"))
        LOG_INFO(QString(tr("Inspecting tree node file format: Application version used to save the file: %1")).arg(root.attribute("ApplicationVersion")));

    // ---------------------------------------------------
    // Check if input format is supported:
    // ---------------------------------------------------
    bool is_supported_format = false;
    if (IExportable::validateQtilitiesExportVersion(read_version) == IExportable::VersionSupported && application_read_version == QtilitiesApplication::applicationExportVersion())
        is_supported_format = true;

    if (!is_supported_format) {
        if (errorMsg)
            *errorMsg = QString(tr("Unsupported tree node file found with export version: %1. The file will not be parsed.")).arg(read_version);
        LOG_ERROR(QString(tr("Unsupported tree node file found with export version: %1. The file will not be parsed.")).arg(read_version));
        //QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }

    // ---------------------------------------------------
    // Do the actual import:
    // ---------------------------------------------------
    IExportable::ExportResultFlags result = IExportable::Complete;
    QList<QPointer<QObject> > internal_import_list;
    QDomNodeList childNodes = root.childNodes();
    for(int i = 0; i < childNodes.count(); ++i) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == QLatin1String("Root")) {
            // Restore the instance name:
            if (child.hasAttribute("Name"))
                setObjectName(child.attribute("Name"));

            // Do import on observer base class:
            IExportable::ExportResultFlags intermediate_result = importXml(&doc,&child,internal_import_list);
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

    //QApplication::restoreOverrideCursor();
    return result;
}

//IExportable::ExportResultFlags Qtilities::CoreGui::TreeNode::saveToString(QString* target_string, QString* errorMsg, ObserverData::ExportItemFlags export_flags) const {
//    // Save it to a temp file and read back the file:
//    if (!target_string) {
//        if (errorMsg)
//            *errorMsg = "Invalid target string pointer.";
//        return IExportable::ExportResultFlags;
//    }

//    QTemporaryFile file;
//    if (file.open()) {
//        IExportable::ExportResultFlags result = saveToFile(file.fileName(),errorMsg,export_flags);
//        *target_string = file.readAll();
//        return result;
//    }

//    if (errorMsg)
//        *errorMsg = "Failed to open a temporary file needed to produce string.";
//    return IExportable::Failed;
//}

//IExportable::ExportResultFlags Qtilities::CoreGui::TreeNode::loadFromString(QString* target_string, QString* errorMsg, bool clear_first) {
//    if (!target_string) {
//        if (errorMsg)
//            *errorMsg = "Invalid target string pointer.";
//        return IExportable::ExportResultFlags;
//    }


//    //clear_first

//    QTemporaryFile file;
//    if (file.open()) {
//        IExportable::ExportResultFlags result = saveToFile(file.fileName(),errorMsg,export_flags);
//        *target_string = file.readAll();
//        return result;
//    }

//    if (errorMsg)
//        *errorMsg = "Failed to open a temporary file needed to produce string.";
//    return IExportable::Failed;
//}
