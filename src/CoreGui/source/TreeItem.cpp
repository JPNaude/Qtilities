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

#include "TreeItem.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesCoreConstants.h"

#include <IFactoryProvider>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core;

#include <QDomElement>

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QObject, TreeItem> TreeItem::factory;
    }
}

struct Qtilities::CoreGui::TreeItemPrivateData {
    TreeItemPrivateData() { }
};

Qtilities::CoreGui::TreeItem::TreeItem(const QString& name, QObject* parent) : TreeItemBase(name,parent) {
    d = new TreeItemPrivateData;
    setObjectName(name);
}

Qtilities::CoreGui::TreeItem::~TreeItem() {
    delete d;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::CoreGui::TreeItem::instanceFactoryInfo() const {
    InstanceFactoryInfo instanceFactoryInfo(qti_def_FACTORY_QTILITIES,qti_def_FACTORY_TAG_TREE_ITEM,objectName());
    return instanceFactoryInfo;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::CoreGui::TreeItem::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeItem::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    Q_UNUSED(stream)

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeItem::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(stream)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeItem::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    IExportable::ExportResultFlags result = saveFormattingToXML(doc,object_node,exportVersion());
    return result;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeItem::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(object_node)
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    return loadFormattingFromXML(doc,object_node,exportVersion());
}

