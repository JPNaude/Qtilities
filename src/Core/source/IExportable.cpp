/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#include "IExportable.h"
#include "QtilitiesCoreApplication.h"

#include <QDomElement>

Qtilities::Core::Interfaces::IExportable::IExportable() {
    d_export_version = Qtilities::Qtilities_Latest;
    d_application_export_version_set = false;
    d_is_exportable = true;
    d_task = 0;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::Interfaces::IExportable::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::Interfaces::IExportable::exportBinary(QDataStream& stream ) const {
    Q_UNUSED(stream)

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::Interfaces::IExportable::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(stream)
    Q_UNUSED(import_list)

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::Interfaces::IExportable::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::Interfaces::IExportable::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)
    Q_UNUSED(import_list)

    return IExportable::Complete;
}

void Qtilities::Core::Interfaces::IExportable::setApplicationExportVersion(quint32 version) {
    d_application_export_version_set = true;
    d_export_application_version = version;
}

Qtilities::Core::Interfaces::IExportable *Qtilities::Core::Interfaces::IExportable::duplicate(int properties_to_copy, ExportResultFlags *result_flags, QString* error_msg) const {
    // Export this interface to a QDomDocument:
    QDomDocument doc("tmp");
    QDomElement root = doc.createElement("tmp");
    doc.appendChild(root);
    if (!instanceFactoryInfo().exportXml(&doc,&root,Qtilities::Qtilities_Latest)) {
        if (error_msg)
            *error_msg = QString("%1: Failed to stream InstanceFactoryInfo to QDomDocument.").arg(Q_FUNC_INFO);
        if (result_flags)
            *result_flags = IExportable::Failed;
        return 0;
    }

    ExportResultFlags result = exportXml(&doc,&root);
    if (result & IExportable::FailedResult) {
        if (error_msg)
            *error_msg = QString("%1: Failed to stream IExportable interface to QDomDocument.").arg(Q_FUNC_INFO);
        if (result_flags)
            *result_flags = result;
        return 0;
    }

    // Construct a new object using instance factory info:
    InstanceFactoryInfo info(&doc,&root,Qtilities::Qtilities_Latest);
    if (!info.isValid()) {
        if (error_msg)
            *error_msg = QString("%1: The factory info provided by InstanceFactoryInfo is not valid.").arg(Q_FUNC_INFO);
        if (result_flags)
            *result_flags = IExportable::Failed;
        return 0;
    }

    // Now construct a new scripted action:
    IExportable* new_inst = 0;
    IFactoryProvider* ifactory = OBJECT_MANAGER->referenceIFactoryProvider(info.d_factory_tag);
    if (ifactory) {
        QObject* obj = ifactory->createInstance(info);
        if (obj) {
            obj->setObjectName(info.d_instance_name);

            // Cast the object to IExportable:
            new_inst = qobject_cast<IExportable*> (obj);
            if (new_inst) {
                QList<QPointer<QObject> > import_list;
                ExportResultFlags import_result = new_inst->importXml(&doc,&root,import_list);
                if (result_flags)
                    *result_flags = import_result;

                if (import_result & IExportable::FailedResult) {
                    delete obj;
                    if (error_msg)
                        *error_msg = QString("%1: Failed to import new object from QDomDocument.").arg(Q_FUNC_INFO);
                    return 0;
                } else {
                    // Copy properties if needed:
                    ObjectManager::PropertyTypeFlags property_type_flags = (ObjectManager::PropertyTypeFlags) properties_to_copy;
                    if (property_type_flags != ObjectManager::NoProperties && this->objectBase() && new_inst->objectBase()) {
                        if (!ObjectManager::cloneObjectProperties(this->objectBase(),new_inst->objectBase(),property_type_flags)) {
                            if (error_msg)
                                *error_msg = QString("%1: The object's dynamic properties could not be cloned succesfully.").arg(Q_FUNC_INFO);
                            if (result_flags)
                                *result_flags = IExportable::Incomplete;
                        }
                    } else {
                        if (result_flags)
                            *result_flags = IExportable::Complete;
                    }
                    return new_inst;
                }
            } else {
                delete obj;
                if (error_msg)
                    *error_msg = QString("%1: Failed to cast new object to IExportable.").arg(Q_FUNC_INFO);
                if (result_flags)
                    *result_flags = IExportable::Failed;
                return 0;
            }
        }
    } else {
        if (error_msg)
            *error_msg = QString("%1: Could not locate the factory specified by the object's InstanceFactoryInfo.").arg(Q_FUNC_INFO);
        if (result_flags)
            *result_flags = IExportable::Failed;
        return 0;
    }

    if (error_msg)
        *error_msg = QString("%1: Reached end of function without newly constructed object.").arg(Q_FUNC_INFO);
    if (result_flags)
        *result_flags = IExportable::Failed;
    return 0;
}

quint32 Qtilities::Core::Interfaces::IExportable::applicationExportVersion() const {
    if (!d_application_export_version_set)
        return QtilitiesCoreApplication::applicationExportVersion();
    else
        return d_export_application_version;
}

QString Qtilities::Core::Interfaces::IExportable::exportModeToString(ExportMode export_mode) {
    if (export_mode == None) {
        return "None";
    } else if (export_mode == Binary) {
        return "Binary";
    } else if (export_mode == XML) {
        return "XML";
    }

    return QString();
}

Qtilities::Core::Interfaces::IExportable::ExportMode Qtilities::Core::Interfaces::IExportable::stringToExportMode(const QString& export_mode_string) {
    if (export_mode_string == "None") {
        return None;
    } else if (export_mode_string == "Binary") {
        return Binary;
    } else if (export_mode_string == "XML") {
        return XML;
    }

    Q_ASSERT(0);
    return None;
}

void Qtilities::Core::Interfaces::IExportable::setExportTask(ITask* task) {
    if (!task) {
        d_task_base = 0;
        d_task = 0;
    } else {
        d_task_base = task->objectBase();
        d_task = task;
    }
}

Qtilities::Core::Interfaces::ITask* Qtilities::Core::Interfaces::IExportable::exportTask() const {
    if (d_task_base)
        return d_task;
    else
        return 0;
}

void Qtilities::Core::Interfaces::IExportable::clearExportTask() {
    d_task = 0;
    d_task_base = 0;
}
