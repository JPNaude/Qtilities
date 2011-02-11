/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#include "ObserverRelationalTable.h"
#include "QtilitiesCoreConstants.h"
#include "ObserverProperty.h"

#include <Logger.h>

using namespace Qtilities::Core::Constants;

struct Qtilities::Core::ObserverRelationalTablePrivateData {
    ObserverRelationalTablePrivateData() : observer(0),
    visitor_id_count(0),
    exportable_subjects_only(false) {}
    ~ObserverRelationalTablePrivateData() {
        for (int i = 0; i < entries.count(); i++)
            delete entries.values().at(i);
    }

    Observer*                           observer;
    QMap<int, RelationalTableEntry*>    entries;
    int                                 visitor_id_count;
    bool                                exportable_subjects_only;
};

Qtilities::Core::ObserverRelationalTable::ObserverRelationalTable(Observer* observer, bool exportable_subjects_only) {
    d = new ObserverRelationalTablePrivateData;
    d->observer = observer;
    d->exportable_subjects_only = exportable_subjects_only;

    removeRelationalProperties(d->observer);
    constructTable(d->observer);

    // ToDo: Make sure multiple observer relational tables does not exist for this observer.
    // It should not really be possible since we remove the relational table properties before constructTable().
}

Qtilities::Core::ObserverRelationalTable::ObserverRelationalTable(const ObserverRelationalTable &other) {
    d = new ObserverRelationalTablePrivateData;
    d->observer = other.d->observer;
    d->exportable_subjects_only = other.d->exportable_subjects_only;
    for (int i = 0; i < other.count(); i++) {
        RelationalTableEntry* other_entry_ptr = other.entryAt(i);
        RelationalTableEntry* entry_ptr = new RelationalTableEntry(*other_entry_ptr);
        entry_ptr->d_sessionID = -1;
        d->entries[other_entry_ptr->d_visitorID] = entry_ptr;
    }
}

Qtilities::Core::ObserverRelationalTable::ObserverRelationalTable() {
    d = new ObserverRelationalTablePrivateData;
}

Qtilities::Core::ObserverRelationalTable::~ObserverRelationalTable() {
    removeRelationalProperties(d->observer);

    // Delete all entries
    for (int i = 0; i < d->entries.count(); i++) {
        delete d->entries.values().at(i);
        d->entries[d->entries.keys().at(i)] = 0;
    }
    delete d;
}

void Qtilities::Core::ObserverRelationalTable::refresh() {
    // Clear up everything:
    removeRelationalProperties(d->observer);
    // Delete all entries
    for (int i = 0; i < d->entries.count(); i++) {
        delete d->entries.values().at(i);
        d->entries[d->entries.keys().at(i)] = 0;
    }
    d->entries.clear();
    d->visitor_id_count = 0;

    // Now construct the table again:
    constructTable(d->observer);
}

bool Qtilities::Core::ObserverRelationalTable::compare(ObserverRelationalTable other) {
    bool result = true;

    // Check for the same amount of items first.
    if (d->entries.count() != other.count()) {
        LOG_ERROR(QString(QObject::tr("ObserverRelationalTable::compare() failed. Number of entries in table (%1) does not match the number of entries in the table to check (%2).")).arg(d->entries.count()).arg(other.count()));
        LOG_TRACE("Items in table:");
        for (int i = 0; i < d->entries.count(); i++) {
            if (d->entries.values().at(i))
                LOG_TRACE(d->entries.values().at(i)->d_name);
        }
        LOG_TRACE("Items in comparison table:");
        for (int i = 0; i < other.count(); i++) {
            if (other.entryAt(i))
                LOG_TRACE(other.entryAt(i)->d_name);
        }
        return false;
    }

    // We compare by looking up each item in table in this table and compare each item individially.
    for (int i = 0; i < d->entries.count(); i++) {
        if (!d->entries.values().at(i)) {
            LOG_FATAL(QObject::tr("Null entry found in current observer in method ObserverRelationalTable::compare()."));
            return false;
        }
        if (!other.entryAt(i)) {
            LOG_FATAL(QObject::tr("Null entry found in other observer in method ObserverRelationalTable::compare()."));
            return false;
        }
        if (*d->entries.values().at(i) != *other.entryAt(i)) {
            return false;
        }
    }

    return result;
}

int Qtilities::Core::ObserverRelationalTable::count() const {
    return d->entries.count();
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryWithVisitorID(int visitor_id) {
    for (int i = 0; i < d->entries.count(); i++) {
        if (d->entries.values().at(i)->d_visitorID == visitor_id)
            return d->entries.values().at(i);
    }

    return 0;
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryWithSessionID(int session_id) {
    for (int i = 0; i < d->entries.count(); i++) {
        if (d->entries.values().at(i)->d_sessionID == session_id)
            return d->entries.values().at(i);
    }

    return 0;
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryWithPreviousSessionID(int session_id) {
    for (int i = 0; i < d->entries.count(); i++) {
        if (d->entries.values().at(i)->d_previousSessionID == session_id)
            return d->entries.values().at(i);
    }

    return 0;
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryAt(int index) {
    if (index < 0 || index >= d->entries.count())
        return 0;

    return d->entries.values().at(index);
}

bool Qtilities::Core::ObserverRelationalTable::compareObjects(QList<QPointer<QObject> >& objects) const {
    // Check for the same amount of items first.
    if (d->entries.count() != objects.count()) {
        LOG_ERROR(QString(QObject::tr("ObserverRelationalTable::compareObjects() failed. Number of entries in table (%1) does not match the number of objects in list to check (%2).")).arg(d->entries.count()).arg(objects.count()));
        LOG_TRACE("Items in relational table:");
        for (int i = 0; i < d->entries.count(); i++) {
            LOG_TRACE(d->entries.values().at(i)->d_name);
        }
        LOG_TRACE("Items in object list:");
        for (int i = 0; i < objects.count(); i++) {
            LOG_TRACE(objects.at(i)->objectName());
        }
        return false;
    }

    bool success = true;
    // Compare the visitor IDs of all objects:
    for (int i = 0; i < objects.count(); i++) {
        int other_id = getVisitorID(objects.at(i));
        // Now compare it against the key at entry i
        if (!d->entries.keys().contains(other_id)) {
            LOG_TRACE(QString("Visitor ID \"%1\" on object \"%2\" does not exist in the readback table.").arg(other_id).arg(objects.at(i)->objectName()));
            success = false;
        } else {
            LOG_TRACE(QString("Visitor ID \"%1\" on object \"%2\" found in the readback table.").arg(other_id).arg(objects.at(i)->objectName()));
        }
    }

    return success;
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryAt(int index) const {
    if (index < 0 || index >= d->entries.count())
        return 0;

    return d->entries.values().at(index);
}

void Qtilities::Core::ObserverRelationalTable::dumpTableInfo() const {
    LOG_INFO("");
    LOG_INFO(QObject::tr("Table Dump Start."));
    LOG_INFO("");

    // Loop through all entries and dump info for each entry:
    if (d->observer)
        LOG_INFO(QObject::tr("Observer Relational Table Dump For Observer: ") + d->observer->observerName());
    else
        LOG_INFO(QObject::tr("Observer Relational Table Dump For Readback Table:"));
    LOG_INFO("-------------------------------------");
    for (int i = 0; i < d->entries.count(); i++) {
        RelationalTableEntry* entry = d->entries.values().at(i);
        if (!entry) {
            LOG_INFO(QObject::tr("Null entry found..."));
            break;
        }

        LOG_INFO(QString(QObject::tr("> Table Entry %1 START:")).arg(i));
        LOG_INFO("> -------------------------------------");
        LOG_INFO(QString(QObject::tr("> Name:                   %1")).arg(entry->d_name));
        LOG_INFO(QString(QObject::tr("> Visitor ID:             %1")).arg(entry->d_visitorID));
        LOG_INFO(QString(QObject::tr("> Session ID:             %1")).arg(entry->d_sessionID));
        LOG_INFO(QString(QObject::tr("> Previous Session ID:    %1")).arg(entry->d_previousSessionID));
        LOG_INFO(QString(QObject::tr("> Owner Visitor ID:       %1")).arg(entry->d_parentVisitorID));
        LOG_INFO(QString(QObject::tr("> Child count:            %1")).arg(entry->d_children.count()));
        for (int c = 0; c < entry->d_children.count(); c++) {
            RelationalTableEntry* child = d->entries[entry->d_children.at(c)];
            if (child) {
                LOG_INFO(QString(QObject::tr(">> Child No.   %1")).arg(c));
                LOG_INFO(QString(QObject::tr(">> Name        %1")).arg(child->d_name));
                LOG_INFO(QString(QObject::tr(">> Visitor ID  %1")).arg(child->d_visitorID));
                LOG_INFO(QString(QObject::tr(">> Ownership   %1")).arg(child->d_ownership));
            } else
                LOG_WARNING(QObject::tr("Null child found..."));
        }
        LOG_INFO(QString(QObject::tr("> Parent count: %1")).arg(entry->d_parents.count()));
        for (int c = 0; c < entry->d_parents.count(); c++) {
            RelationalTableEntry* parent = d->entries[entry->d_parents.at(c)];
            if (parent) {
                LOG_INFO(QString(QObject::tr(">> Parent No.  %1")).arg(c));
                LOG_INFO(QString(QObject::tr(">> Name        %1")).arg(parent->d_name));
                LOG_INFO(QString(QObject::tr(">> Visitor ID  %1")).arg(parent->d_visitorID));
            } else
                LOG_WARNING(QObject::tr("Null parent found..."));
        }
        LOG_INFO("> -------------------------------------");
        LOG_INFO(QString(QObject::tr("> Table Entry %1 END:")).arg(i));
        LOG_INFO("> -------------------------------------");
    }

    LOG_INFO("");
    LOG_INFO(QObject::tr("Table Dump End."));
    LOG_INFO("");
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::constructTable(Observer* observer) {
    if (!observer)
        return 0;

    // ---------------------------------------
    // HANDLE THE OBSERVER
    // ---------------------------------------
    // Get the ownership of this observer.
    int observer_ownership = getOwnership(observer);
    // Add the observer itself to the table entries map:
    RelationalTableEntry* observer_entry = new RelationalTableEntry(d->visitor_id_count,observer->observerID(),observer->observerName(),observer_ownership);
    // Add the unique visitor ID to the observer.
    int observer_id = addVisitorID(observer);
    if (observer_id == -1)
        observer_id = getVisitorID(observer);
    observer_entry->d_sessionID = observer->observerID();
    observer_entry->d_ownership = getOwnership(observer);

    // ---------------------------------------
    // HANDLE THE SUBJECTS
    // ---------------------------------------
    // Now loop through all subjects in observer.
    int subject_id;
    int subject_ownership;
    RelationalTableEntry* subject_entry;
    for (int i = 0; i < observer->subjectCount(); i++) {
        QObject* obj = observer->subjectAt(i);
        subject_ownership = getOwnership(obj);

        bool is_iface = false;
        bool is_observer = false;
        bool has_child_observer = false;

        IExportable* exportable_iface = qobject_cast<IExportable*> (obj);
        if (exportable_iface)
            is_iface = true;

        Observer* obs = qobject_cast<Observer*> (obj);
        if (obs)
            is_observer = true;

        if (!obs) {
            for (int r = 0; r < obj->children().count(); r++) {
                obs = qobject_cast<Observer*> (obj->children().at(r));
                if (obs) {
                    has_child_observer = true;
                    break;
                }
            }
        }

        // The way this relational table is build follows the way Observers export
        // hierarchical structures underneath them.
        // That is, IExportable interfaces gets the highest priority, then normal subjects. In the
        // case of this table, we need to cater for 3 scenarios:
        // 1) Exportable interface which is not an observer.
        // 2) Exportable interface which is an observer class.
        // 3) Not an exportable interface.
        // 3.1) We check if an observer child is present, if so we parse it.
        // 3.2) If an observer child is not present we treat it as a normal QObject.

        if (is_iface && is_observer) {
            // Now inspect the subject.
            subject_entry = constructTable(obs);
            subject_id = subject_entry->d_visitorID;
            // Now add this observer as a parent to the subject
            if (subject_entry)
                subject_entry->d_parents.append(observer_id);
            else
                break;
        } else if (!is_iface && has_child_observer) {
            // Now inspect the subject.
            for (int r = 0; r < obj->children().count(); r++) {
                obs = qobject_cast<Observer*> (obj->children().at(r));
                if (obs)
                    break;
            }

            subject_entry = constructTable(obs);
            subject_id = subject_entry->d_visitorID;
            // Now add this observer as a parent to the subject
            if (subject_entry)
                subject_entry->d_parents.append(observer_id);
            else
                break;
        } else if ((is_iface && !is_observer) || (!is_iface)) {
            // Add the unique visitor ID to this object.
            subject_id = addVisitorID(obj);

            // If only exportable subjects must be added, we need to check for that here
            if (d->exportable_subjects_only) {
                IExportable* export_iface = qobject_cast<IExportable*> (obj);
                if (!export_iface)
                    break;
            }

            if (subject_id == -1) {
                // Already existed:
                // Get the entry
                subject_id = getVisitorID(obj);
                subject_entry = d->entries[subject_id];
                addLimitedExportProperty(obj);
                // Now add this observer as a parent to the subject
                if (subject_entry)
                    subject_entry->d_parents.append(observer_id);
                else
                    LOG_FATAL(QObject::tr("ObserverRelationalTable::constructTable(): subject_entry can't be zero."));
            } else {
                // Did not exist:
                // Add the subject to the table entries map:
                subject_entry = new RelationalTableEntry(subject_id,-1,observer->subjectNameInContext(obj),subject_ownership);
                d->entries[subject_id] = subject_entry;
                // Now add this observer as a parent to the subject
                subject_entry->d_parents.append(observer_id);
            }
        }

        // Now add this subject as a child to the observer_entry.
        observer_entry->d_children.append(subject_id);

        // Now check if this observer is the parent of this subject (SpecificObserverOwnership).
        // If so, add the visitor ID of this observer as the parent visitor ID.
        if (getSpecificParent(obj) != -1) {
            if (!subject_entry)
                LOG_FATAL(QObject::tr("ObserverRelationalTable::constructTable(): subject_entry can't be zero."));
            else
                subject_entry->d_parentVisitorID = observer_id;
        }
    }

    // ---------------------------------------
    // ADD THE OBSERVER ENTRY
    // ---------------------------------------
    d->entries[observer_id] = observer_entry;
    return observer_entry;
}

int Qtilities::Core::ObserverRelationalTable::getOwnership(QObject* obj) const {
    QVariant prop_variant = obj->property(qti_prop_OWNERSHIP);
    if (prop_variant.isValid() && prop_variant.canConvert<SharedObserverProperty>()) {
        SharedObserverProperty prop = prop_variant.value<SharedObserverProperty>();
        if (prop.isValid()) {
             return prop.value().toInt();
        }
    }
    return -1;
}

int Qtilities::Core::ObserverRelationalTable::addVisitorID(QObject* obj) {
    if (!Observer::propertyExists(obj, qti_prop_VISITOR_ID)) {
        // We need to create the property and add it to the object
        SharedObserverProperty new_prop(d->visitor_id_count,qti_prop_VISITOR_ID);
        QVariant new_prop_variant = qVariantFromValue(new_prop);
        obj->setProperty(new_prop.propertyName(),new_prop_variant);
        if (Observer::propertyExists(obj,qti_prop_VISITOR_ID))
            LOG_TRACE("Added visitor ID property to object: " + obj->objectName());
        else
            LOG_TRACE("Failed to add visitor ID property to object: " + obj->objectName());
        // Increment the visitor id counter:
        ++d->visitor_id_count;
        return d->visitor_id_count-1;
    }
    return -1;
}

int Qtilities::Core::ObserverRelationalTable::addLimitedExportProperty(QObject* obj)  {
    if (!Observer::propertyExists(obj, qti_prop_LIMITED_EXPORTS)) {
        // We need to create the property and add it to the object
        SharedObserverProperty new_prop(0,qti_prop_LIMITED_EXPORTS);
        QVariant new_prop_variant = qVariantFromValue(new_prop);
        obj->setProperty(new_prop.propertyName(),new_prop_variant);
        return d->visitor_id_count;
    }
    return -1;
}

int Qtilities::Core::ObserverRelationalTable::getSpecificParent(QObject* obj) const {
    int ownership = getOwnership(obj);
    Observer::ObjectOwnership ownership_cast = (Observer::ObjectOwnership) ownership;
    if (ownership_cast == Observer::SpecificObserverOwnership) {
        QVariant prop_variant = obj->property(qti_prop_PARENT_ID);
        if (prop_variant.isValid() && prop_variant.canConvert<SharedObserverProperty>()) {
            SharedObserverProperty prop = prop_variant.value<SharedObserverProperty>();
            if (prop.isValid()) {
                 return prop.value().toInt();
            }
        }
    }
    return -1;
}

quint32 MARKER_RELATIONAL_TABLE_SECTION = 0xEEEEEEEE;

bool Qtilities::Core::ObserverRelationalTable::exportBinary(QDataStream& stream) const {
    stream << MARKER_RELATIONAL_TABLE_SECTION;

    // Stream the entries one after another:
    stream << (qint32) count();
    for (int i = 0; i < count(); i++) {
        if (entryAt(i))
            entryAt(i)->exportBinary(stream);
        else {
            LOG_ERROR(QString(QObject::tr("Internal error, ObserverRelationalTable::exportBinary(stream) found null object in entry position %1/%2")).arg(i).arg(count()));
            return false;
        }
    }
    stream << MARKER_RELATIONAL_TABLE_SECTION;
    return true;
}

bool Qtilities::Core::ObserverRelationalTable::importBinary(QDataStream& stream) {
    quint32 marker;
    stream >> marker;
    if (marker != MARKER_RELATIONAL_TABLE_SECTION) {
        LOG_ERROR("ObserverRelationalTable binary import failed to detect start marker. Import will fail.");
        return false;
    }

    qint32 qi32;
    stream >> qi32;

    int entry_count = qi32;
    for (int i = 0; i < entry_count; i++) {
        RelationalTableEntry entry;
        entry.importBinary(stream);
        RelationalTableEntry* entry_ptr = new RelationalTableEntry(entry);
        d->entries[entry.d_visitorID] = entry_ptr;
    }

    stream >> marker;
    if (marker != MARKER_RELATIONAL_TABLE_SECTION) {
        LOG_ERROR("ObserverRelationalTable binary import failed to detect end marker. Import will fail.");
        return false;
    }

    return true;
}
