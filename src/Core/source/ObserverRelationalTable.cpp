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

#include "ObserverRelationalTable.h"
#include "QtilitiesCoreConstants.h"
#include "QtilitiesProperty.h"

#include <Logger>

#include <QDomDocument>

using namespace Qtilities::Core::Constants;

// -------------------------------------------------------
// RelationalTableEntry
// -------------------------------------------------------

struct Qtilities::Core::RelationalTableEntryData {
    RelationalTableEntryData() {
        previousSessionID = -1;
        parentVisitorID = -1;
        visitorID = -1;
        name = "";
        ownership = -1;
        sessionID = -1;
        obj = 0;
    }

    //! The visitor IDs of all parents of this item.
    QList<int>   parents;
    //! The visitor IDs of all children of this item.
    QList<int>   children;
    //! The visitor ID of this item.
    int             visitorID;
    //! The session ID of this item. The session ID is only applicable to observers and is equal to their observer IDs.
    int             sessionID;
    //! The previous session ID of this item. The session ID is only applicable to observers and is equal to their observer IDs. This is only used during relationship reconstruction during observer export/import operations.
    int             previousSessionID;
    //! The objectName() of the object for which this table entry was created.
    QString         name;
    //! The ownership of this object in its parent observer.
    int             ownership;
    //! When \p d_ownership is equal to Qtilities::Core::Observer::SpecificObserverOwnership this field contains the visitor ID of its specific parent.
    int             parentVisitorID;
    //! A reference to the object.
    QObject*        obj;
};

Qtilities::Core::RelationalTableEntry::RelationalTableEntry() {
    d = new RelationalTableEntryData;
}

Qtilities::Core::RelationalTableEntry::RelationalTableEntry(int visitorID, int sessionID, const QString& name, int ownership, QObject* obj) {
    d = new RelationalTableEntryData;
    d->visitorID = visitorID;
    d->sessionID = sessionID;
    d->name = name;
    d->ownership = ownership;
    d->obj = obj;
}

Qtilities::Core::RelationalTableEntry::RelationalTableEntry(const RelationalTableEntry& other) {
    d = new RelationalTableEntryData;
    d->children = other.children();
    d->parents = other.parents();
    d->visitorID = other.visitorID();
    d->sessionID = other.sessionID();
    d->previousSessionID = other.previousSessionID();
    d->name = other.name();
    d->ownership = other.ownership();
    d->parentVisitorID = other.parentVisitorID();
    d->obj = other.object();
}

bool Qtilities::Core::RelationalTableEntry::operator==(const RelationalTableEntry& other) const {
    bool equal = true;
    if (equal)
        equal = (d->children == other.children());
    if (equal)
        equal = (d->children == other.children());
    if (equal)
        equal = (d->visitorID == other.visitorID());
    if (equal)
        equal = (d->name == other.name());
    if (equal)
        equal = (d->ownership == other.ownership());
    if (equal)
        equal = (d->parentVisitorID == other.parentVisitorID());
    return equal;
}

bool Qtilities::Core::RelationalTableEntry::operator!=(const RelationalTableEntry& other) const {
    return !(*this==other);
}

QList<int> Qtilities::Core::RelationalTableEntry::parents() const {
    return d->parents;
}

void Qtilities::Core::RelationalTableEntry::setParents(QList<int> parents) {
    d->parents = parents;
}

void Qtilities::Core::RelationalTableEntry::addParent(int parent_id) {
    d->parents.append(parent_id);
}

QList<int> Qtilities::Core::RelationalTableEntry::children() const {
    return d->children;
}

void Qtilities::Core::RelationalTableEntry::setChildren(QList<int> children) {
    d->children = children;
}

void Qtilities::Core::RelationalTableEntry::addChild(int child_id) {
    d->children.append(child_id);
}

int Qtilities::Core::RelationalTableEntry::visitorID() const {
    return d->visitorID;
}

void Qtilities::Core::RelationalTableEntry::setVisitorID(int visitor_id) {
    d->visitorID = visitor_id;
}

int Qtilities::Core::RelationalTableEntry::sessionID() const {
    return d->sessionID;
}

void Qtilities::Core::RelationalTableEntry::setSessionID(int session_id) {
    d->sessionID = session_id;
}

int Qtilities::Core::RelationalTableEntry::previousSessionID() const {
    return d->previousSessionID;
}

void Qtilities::Core::RelationalTableEntry::setPreviousSessionID(int session_id) {
    d->previousSessionID = session_id;
}

QString Qtilities::Core::RelationalTableEntry::name() const {
    return d->name;
}

void Qtilities::Core::RelationalTableEntry::setName(QString name) {
    d->name = name;
}

int Qtilities::Core::RelationalTableEntry::ownership() const {
    return d->ownership;
}

void Qtilities::Core::RelationalTableEntry::setOwnership(int ownership) {
    d->ownership = ownership;
}

int Qtilities::Core::RelationalTableEntry::parentVisitorID() const {
    return d->parentVisitorID;
}

void Qtilities::Core::RelationalTableEntry::setParentVisitorID(int parent_visitor_id) {
    d->parentVisitorID = parent_visitor_id;
}

QObject* Qtilities::Core::RelationalTableEntry::object() const {
    return d->obj;
}

void Qtilities::Core::RelationalTableEntry::setObject(QObject* object) {
    d->obj = object;
}

QString Qtilities::Core::RelationalTableEntry::intListToString(QList<int> list) const {
    QStringList string_list;
    for (int i = 0; i < list.count(); ++i)
        string_list << QString::number(list.at(i));
    return string_list.join(",");
}

QList<int> Qtilities::Core::RelationalTableEntry::stringToIntList(const QString& string) const {
    QStringList string_list = string.split(",");
    QList<int> int_list;
    foreach (const QString& one_string, string_list)
        int_list << (int) one_string.toInt();
    return int_list;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::RelationalTableEntry::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::RelationalTableEntry::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream << d->name;
    stream << d->parents;
    stream << d->children;
    stream << (qint32) d->visitorID;
    stream << (qint32) d->sessionID;
    stream << (qint32) d->ownership;
    stream << (qint32) d->parentVisitorID;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::RelationalTableEntry::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream >> d->name;
    stream >> d->parents;
    stream >> d->children;
    qint32 qi32;
    stream >> qi32;
    d->visitorID = qi32;
    stream >> qi32;
    d->sessionID = qi32;
    stream >> qi32;
    d->ownership = qi32;
    stream >> qi32;
    d->parentVisitorID = qi32;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::RelationalTableEntry::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (!object_node)
        return IExportable::Failed;

    object_node->setAttribute("Name",d->name);
    if (d->parents.count() > 0)
        object_node->setAttribute("Parents",intListToString(d->parents));
    if (d->children.count() > 0)
        object_node->setAttribute("Children",intListToString(d->children));
    object_node->setAttribute("VisitorID",QString::number(d->visitorID));
    object_node->setAttribute("SessionID",QString::number(d->sessionID));
    object_node->setAttribute("Ownership",QString::number(d->ownership));
    object_node->setAttribute("ParentVisitorID",QString::number(d->parentVisitorID));

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::RelationalTableEntry::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (!object_node)
        return IExportable::Failed;

    if (object_node->hasAttribute("Name"))
        d->name = object_node->attribute("Name");
    else
        return IExportable::Failed;
    if (object_node->hasAttribute("Parents"))
        d->parents = stringToIntList(object_node->attribute("Parents"));
    if (object_node->hasAttribute("Children"))
        d->children = stringToIntList(object_node->attribute("Children"));
    if (object_node->hasAttribute("VisitorID"))
        d->visitorID = object_node->attribute("VisitorID").toInt();
    else
        return IExportable::Failed;
    if (object_node->hasAttribute("SessionID"))
        d->sessionID = object_node->attribute("SessionID").toInt();
    else
        return IExportable::Failed;
    if (object_node->hasAttribute("Ownership"))
        d->ownership = object_node->attribute("Ownership").toInt();
    else
        return IExportable::Failed;
    if (object_node->hasAttribute("ParentVisitorID"))
        d->parentVisitorID = object_node->attribute("ParentVisitorID").toInt();
    else
        return IExportable::Failed;

    return IExportable::Complete;
}


// -------------------------------------------------------
// ObserverRelationalTable
// -------------------------------------------------------

struct Qtilities::Core::ObserverRelationalTablePrivateData {
    ObserverRelationalTablePrivateData() : observer(0),
    visitor_id_count(0),
    exportable_subjects_only(false) {}
    ~ObserverRelationalTablePrivateData() {
        for (int i = 0; i < entries.count(); ++i)
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
    for (int i = 0; i < other.count(); ++i) {
        RelationalTableEntry* other_entry_ptr = other.entryAt(i);
        RelationalTableEntry* entry_ptr = new RelationalTableEntry(*other_entry_ptr);
        entry_ptr->setSessionID(-1);
        d->entries[other_entry_ptr->visitorID()] = entry_ptr;
    }
}

Qtilities::Core::ObserverRelationalTable::ObserverRelationalTable() {
    d = new ObserverRelationalTablePrivateData;
}

Qtilities::Core::ObserverRelationalTable::~ObserverRelationalTable() {
    removeRelationalProperties(d->observer);

    // Delete all entries
    for (int i = 0; i < d->entries.count(); ++i) {
        delete d->entries.values().at(i);
        d->entries[d->entries.keys().at(i)] = 0;
    }
    delete d;
}

void Qtilities::Core::ObserverRelationalTable::refresh() {
    // Clear up everything:
    removeRelationalProperties(d->observer);
    // Delete all entries
    for (int i = 0; i < d->entries.count(); ++i) {
        delete d->entries.values().at(i);
        d->entries[d->entries.keys().at(i)] = 0;
    }
    d->entries.clear();
    d->visitor_id_count = 0;

    // Now construct the table again:
    constructTable(d->observer);
}

bool Qtilities::Core::ObserverRelationalTable::compare(ObserverRelationalTable other) const {
    bool result = true;

    // Check for the same amount of items first.
    if (d->entries.count() != other.count()) {
        LOG_TRACE(QString("ObserverRelationalTable::compare() failed. Number of entries in table (%1) does not match the number of entries in the table to check (%2).").arg(d->entries.count()).arg(other.count()));
        LOG_TRACE("Items in table:");
        for (int i = 0; i < d->entries.count(); ++i) {
            if (d->entries.values().at(i))
                LOG_TRACE(d->entries.values().at(i)->name());
        }
        LOG_TRACE("Items in comparison table:");
        for (int i = 0; i < other.count(); ++i) {
            if (other.entryAt(i))
                LOG_TRACE(other.entryAt(i)->name());
        }
        return false;
    }

    // We compare by looking up each item in table in this table and compare each item individially.
    for (int i = 0; i < d->entries.count(); ++i) {
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

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryWithVisitorID(int visitor_id) const {
    for (int i = 0; i < d->entries.count(); ++i) {
        if (d->entries.values().at(i)->visitorID() == visitor_id)
            return d->entries.values().at(i);
    }

    return 0;
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryWithSessionID(int session_id) const {
    for (int i = 0; i < d->entries.count(); ++i) {
        if (d->entries.values().at(i)->sessionID() == session_id)
            return d->entries.values().at(i);
    }

    return 0;
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryWithPreviousSessionID(int session_id) const {
    for (int i = 0; i < d->entries.count(); ++i) {
        if (d->entries.values().at(i)->previousSessionID() == session_id)
            return d->entries.values().at(i);
    }

    return 0;
}

Qtilities::Core::RelationalTableEntry* Qtilities::Core::ObserverRelationalTable::entryAt(int index) {
    if (index < 0 || index >= d->entries.count())
        return 0;

    return d->entries.values().at(index);
}

int Qtilities::Core::ObserverRelationalTable::getVisitorID(QObject* obj) {
    if (!obj)
        return -1;

    QVariant prop_variant = obj->property(qti_prop_VISITOR_ID);
    if (prop_variant.isValid() && prop_variant.canConvert<SharedProperty>()) {
        SharedProperty prop = prop_variant.value<SharedProperty>();
        if (prop.isValid()) {
             return prop.value().toInt();
        }
    }
    return -1;
}

void Qtilities::Core::ObserverRelationalTable::removeRelationalProperties(Observer* observer) {
    if (!observer)
        return;

    observer->setProperty(qti_prop_VISITOR_ID,QVariant());
    observer->setProperty(qti_prop_LIMITED_EXPORTS,QVariant());

    for (int i = 0; i < observer->subjectCount(); ++i) {
        QObject* obj = observer->subjectAt(i);
        bool is_iface = false;
        bool is_observer = false;
        bool has_child_observer = false;

        // We need to iterate through the hierarchy in the same way
        // that constructTable() does it.
        IExportable* exportable_iface = qobject_cast<IExportable*> (obj);
        if (exportable_iface)
            is_iface = true;

        Observer* obs = qobject_cast<Observer*> (obj);
        if (obs)
            is_observer = true;

        if (is_iface && is_observer) {
            removeRelationalProperties(obs);
        } else if (!is_iface && has_child_observer) {
            removeRelationalProperties(obs);
        } else if ((is_iface && !is_observer) || (!is_iface)) {
            obj->setProperty(qti_prop_VISITOR_ID,QVariant());
            obj->setProperty(qti_prop_LIMITED_EXPORTS,QVariant());
        }
    }
}

QMap<int,int> Qtilities::Core::ObserverRelationalTable::parentsToObserverIDs(RelationalTableEntry* entry) const {
    if (!entry)
        return QMap<int,int>();

    QMap<int,int> observerIDs;
    for (int i = 0; i < entry->parents().count(); ++i) {
        // Get the entry for this visitor ID;
        RelationalTableEntry* parent_entry = entryWithVisitorID(entry->parents().at(i));
        if (parent_entry) {
            observerIDs[entry->parents().at(i)] = parent_entry->sessionID();
        }
    }

    if (observerIDs.count() == entry->parents().count())
        return observerIDs;
    else
        return QMap<int,int>();
}

bool Qtilities::Core::ObserverRelationalTable::compareObjects(QList<QPointer<QObject> >& objects) const {
    // Check for the same amount of items first.
    if (d->entries.count() != objects.count()) {
        LOG_ERROR(QString("ObserverRelationalTable::compareObjects() failed. Number of entries in table (%1) does not match the number of objects in list to check (%2).").arg(d->entries.count()).arg(objects.count()));
        LOG_TRACE("Items in relational table:");
        for (int i = 0; i < d->entries.count(); ++i) {
            LOG_TRACE(d->entries.values().at(i)->name());
        }
        LOG_TRACE("Items in object list:");
        for (int i = 0; i < objects.count(); ++i) {
            LOG_TRACE(objects.at(i)->objectName());
        }
        return false;
    }

    bool success = true;
    // Compare the visitor IDs of all objects:
    for (int i = 0; i < objects.count(); ++i) {
        int other_id = getVisitorID(objects.at(i));
        // Now compare it against the key at entry i
        if (!d->entries.contains(other_id)) {
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
    for (int i = 0; i < d->entries.count(); ++i) {
        RelationalTableEntry* entry = d->entries.values().at(i);
        if (!entry) {
            LOG_INFO(QObject::tr("Null entry found..."));
            break;
        }

        LOG_INFO(QString("> Table Entry %1 START:").arg(i));
        LOG_INFO("> -------------------------------------");
        LOG_INFO(QString("> Name:                   %1").arg(entry->name()));
        LOG_INFO(QString("> Visitor ID:             %1").arg(entry->visitorID()));
        LOG_INFO(QString("> Session ID:             %1").arg(entry->sessionID()));
        LOG_INFO(QString("> Previous Session ID:    %1").arg(entry->previousSessionID()));
        LOG_INFO(QString("> Owner Visitor ID:       %1").arg(entry->parentVisitorID()));
        LOG_INFO(QString("> Child count:            %1").arg(entry->children().count()));
        for (int c = 0; c < entry->children().count(); c++) {
            RelationalTableEntry* child = d->entries[entry->children().at(c)];
            if (child) {
                LOG_INFO(QString(">> Child No.   %1").arg(c));
                LOG_INFO(QString(">> Name        %1").arg(child->name()));
                LOG_INFO(QString(">> Visitor ID  %1").arg(child->visitorID()));
                LOG_INFO(QString(">> Ownership   %1").arg(child->ownership()));
            } else
                LOG_WARNING("Null child found...");
        }
        LOG_INFO(QString("> Parent count: %1").arg(entry->parents().count()));
        for (int c = 0; c < entry->parents().count(); c++) {
            RelationalTableEntry* parent = d->entries[entry->parents().at(c)];
            if (parent) {
                LOG_INFO(QString(">> Parent No.  %1").arg(c));
                LOG_INFO(QString(">> Name        %1").arg(parent->name()));
                LOG_INFO(QString(">> Visitor ID  %1").arg(parent->visitorID()));
            } else
                LOG_WARNING("Null parent found...");
        }
        LOG_INFO("> -------------------------------------");
        LOG_INFO(QString("> Table Entry %1 END:").arg(i));
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
    RelationalTableEntry* observer_entry = new RelationalTableEntry(d->visitor_id_count,observer->observerID(),observer->observerName(),observer_ownership,observer);
    // Add the unique visitor ID to the observer.
    int observer_id = addVisitorID(observer);
    if (observer_id == -1)
        observer_id = getVisitorID(observer);
    observer_entry->setSessionID(observer->observerID());
    observer_entry->setOwnership(getOwnership(observer));

    // ---------------------------------------
    // HANDLE THE SUBJECTS
    // ---------------------------------------
    // Now loop through all subjects in observer.
    int subject_id;
    int subject_ownership;
    RelationalTableEntry* subject_entry;
    for (int i = 0; i < observer->subjectCount(); ++i) {
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
            subject_id = subject_entry->visitorID();
            // Now add this observer as a parent to the subject
            if (subject_entry)
                subject_entry->addParent(observer_id);
            else
                break;
        } else if (!is_iface && has_child_observer) {
            subject_entry = constructTable(obs);
            subject_id = subject_entry->visitorID();
            // Now add this observer as a parent to the subject
            if (subject_entry)
                subject_entry->addParent(observer_id);
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
                    subject_entry->addParent(observer_id);
                else
                    LOG_FATAL(QObject::tr("ObserverRelationalTable::constructTable(): subject_entry can't be zero."));
            } else {
                // Did not exist:
                // Add the subject to the table entries map:
                subject_entry = new RelationalTableEntry(subject_id,-1,observer->subjectNameInContext(obj),subject_ownership,obj);
                d->entries[subject_id] = subject_entry;
                // Now add this observer as a parent to the subject
                subject_entry->addParent(observer_id);
            }
        }

        // Now add this subject as a child to the observer_entry.
        observer_entry->addChild(subject_id);

        // Now check if this observer is the parent of this subject (SpecificObserverOwnership).
        // If so, add the visitor ID of this observer as the parent visitor ID.
        if (getSpecificParent(obj) != -1) {
            if (!subject_entry)
                LOG_FATAL(QObject::tr("ObserverRelationalTable::constructTable(): subject_entry can't be zero."));
            else
                subject_entry->setParentVisitorID(observer_id);
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
    if (prop_variant.isValid() && prop_variant.canConvert<SharedProperty>()) {
        SharedProperty prop = prop_variant.value<SharedProperty>();
        if (prop.isValid()) {
             return prop.value().toInt();
        }
    }
    return -1;
}

int Qtilities::Core::ObserverRelationalTable::addVisitorID(QObject* obj) {
    if (!ObjectManager::propertyExists(obj, qti_prop_VISITOR_ID)) {
        // We need to create the property and add it to the object
        SharedProperty new_prop(qti_prop_VISITOR_ID,d->visitor_id_count);
        QVariant new_prop_variant = qVariantFromValue(new_prop);
        obj->setProperty(new_prop.propertyNameString().toUtf8().data(),new_prop_variant);
        if (ObjectManager::propertyExists(obj,qti_prop_VISITOR_ID))
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
    if (!ObjectManager::propertyExists(obj, qti_prop_LIMITED_EXPORTS)) {
        // We need to create the property and add it to the object
        SharedProperty new_prop(qti_prop_LIMITED_EXPORTS,0);
        QVariant new_prop_variant = qVariantFromValue(new_prop);
        obj->setProperty(new_prop.propertyNameString().toUtf8().data(),new_prop_variant);
        return d->visitor_id_count;
    }
    return -1;
}

int Qtilities::Core::ObserverRelationalTable::getSpecificParent(QObject* obj) const {
    int ownership = getOwnership(obj);
    Observer::ObjectOwnership ownership_cast = (Observer::ObjectOwnership) ownership;
    if (ownership_cast == Observer::SpecificObserverOwnership) {
        QVariant prop_variant = obj->property(qti_prop_PARENT_ID);
        if (prop_variant.isValid() && prop_variant.canConvert<SharedProperty>()) {
            SharedProperty prop = prop_variant.value<SharedProperty>();
            if (prop.isValid()) {
                 return prop.value().toInt();
            }
        }
    }
    return -1;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::ObserverRelationalTable::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ObserverRelationalTable::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    // Stream the entries one after another:
    stream << (quint32) count();
    bool all_successful = true;
    for (int i = 0; i < count(); ++i) {
        if (entryAt(i)) {
            entryAt(i)->setExportVersion(exportVersion());
            if (entryAt(i)->exportBinary(stream) != IExportable::Complete)
                all_successful = false;
        } else {
            LOG_ERROR(QString("Internal error, ObserverRelationalTable::exportBinary(stream) found null object in entry position %1/%2").arg(i).arg(count()));
            return IExportable::Failed;
        }
    }

    if (all_successful)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ObserverRelationalTable::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    quint32 qi32;
    stream >> qi32;

    int entry_count = qi32;
    for (int i = 0; i < entry_count; ++i) {
        RelationalTableEntry entry;
        entry.setExportVersion(exportVersion());
        if (entry.importBinary(stream,import_list) == IExportable::Complete) {
            RelationalTableEntry* entry_ptr = new RelationalTableEntry(entry);
            d->entries[entry.visitorID()] = entry_ptr;
        }
    }

    if (d->entries.count() == (int) qi32)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ObserverRelationalTable::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (!object_node)
        return IExportable::Failed;

    if (!doc)
        return IExportable::Failed;

    object_node->setAttribute("EntryCount",d->entries.count());
    bool all_successful = true;
    for (int i = 0; i < d->entries.count(); ++i) {
        QDomElement entry = doc->createElement("Entry_" + QString::number(i));
        object_node->appendChild(entry);
        if (d->entries.values().at(i)) {
            d->entries.values().at(i)->setExportVersion(exportVersion());
            d->entries.values().at(i)->exportXml(doc,&entry);
        }
    }

    if (all_successful)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ObserverRelationalTable::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    int depth_readback = 0;
    if (object_node->hasAttribute("EntryCount"))
        depth_readback = object_node->attribute("EntryCount").toInt();

    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); ++i)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName().startsWith("Entry_")) {
            RelationalTableEntry* new_entry = new RelationalTableEntry;
            new_entry->setExportVersion(exportVersion());
            if (new_entry->importXml(doc,&child,import_list) == IExportable::Complete)
                d->entries[new_entry->visitorID()] = new_entry;
            continue;
        }
    }

    if (d->entries.count() == depth_readback)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::RelationalTableEntry& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::RelationalTableEntry& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::ObserverRelationalTable& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::ObserverRelationalTable& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}

