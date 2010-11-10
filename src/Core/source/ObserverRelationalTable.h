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

#ifndef OBSERVERRELATIONALTABLE_H
#define OBSERVERRELATIONALTABLE_H

#include "QtilitiesCore_global.h"
#include "QtilitiesCoreConstants.h"
#include "Observer.h"

#include <QObject>

using namespace Qtilities::Core::Properties;

namespace Qtilities {
    namespace Core {

        /*!
          \class RelationalTableEntry
          \brief The RelationalTableEntry class represents a single entry in an observer relational table.
         */
        class RelationalTableEntry {
        public:
            RelationalTableEntry() {
                d_previousSessionID = -1;
                d_parentVisitorID = -1;
                d_visitorID = -1;
                d_name = "";
                d_ownership = -1;
                d_sessionID = -1;
            }
            RelationalTableEntry(int visitorID, int sessionID, const QString& name, int ownership) {
                d_visitorID = visitorID;
                d_sessionID = sessionID;
                d_name = name;
                d_ownership = ownership;
                d_parentVisitorID = -1;
                d_previousSessionID = -1;
            }
            RelationalTableEntry(const RelationalTableEntry& other) {
                d_parents = other.d_parents;
                d_children = other.d_children;
                d_visitorID = other.d_visitorID;
                d_sessionID = other.d_sessionID;
                d_name = other.d_name;
                d_ownership = other.d_ownership;
                d_parentVisitorID = other.d_parentVisitorID;
                d_previousSessionID = other.d_previousSessionID;
            }
            bool operator==(const RelationalTableEntry& other) {
                bool equal = true;
                if (equal)
                    equal = (d_parents == other.d_parents);
                if (equal)
                    equal = (d_children == other.d_children);
                if (equal)
                    equal = (d_visitorID == other.d_visitorID);
                if (equal)
                    equal = (d_name == other.d_name);
                if (equal)
                    equal = (d_ownership == other.d_ownership);
                if (equal)
                    equal = (d_parentVisitorID == other.d_parentVisitorID);
                return equal;
            }
            bool operator!=(const RelationalTableEntry& other) {
                bool equal = false;
                if (!equal)
                    equal = (d_parents != other.d_parents);
                if (!equal)
                    equal = (d_children != other.d_children);
                if (!equal)
                    equal = (d_visitorID != other.d_visitorID);
                if (!equal)
                    equal = (d_name != other.d_name);
                if (!equal)
                    equal = (d_ownership != other.d_ownership);
                if (!equal)
                    equal = (d_parentVisitorID != other.d_parentVisitorID);
                return equal;
            }
            bool exportBinary(QDataStream& stream) const {
                stream << d_name;
                stream << d_parents;
                stream << d_children;
                stream << (qint32) d_visitorID;
                stream << (qint32) d_sessionID;
                stream << (qint32) d_ownership;
                stream << (qint32) d_parentVisitorID;
                return true;
            }
            bool importBinary(QDataStream& stream) {
                stream >> d_name;
                stream >> d_parents;
                stream >> d_children;
                qint32 qi32;
                stream >> qi32;
                d_visitorID = qi32;
                stream >> qi32;
                d_sessionID = qi32;
                stream >> qi32;
                d_ownership = qi32;
                stream >> qi32;
                d_parentVisitorID = qi32;
                return true;
            }

            QList<qint32>   d_parents;
            QList<qint32>   d_children;
            int             d_visitorID;
            int             d_sessionID;
            int             d_previousSessionID;
            QString         d_name;
            int             d_ownership;
            int             d_parentVisitorID;
        };

        /*!
        \struct ObserverRelationalTableData
        \brief The ObserverRelationalTableData stores private data used by the ObserverRelationalTable class.
          */
        struct ObserverRelationalTableData;

        /*!
          \class ObserverRelationalTable
          \brief The ObserverRelationalTable class is a data structure which stores relational data about an observer tree.

          Using this table it is possible to store the relationships between objects in an observer tree. The relational table
          is constructed by providing a top level observer, or by providing an QDataStream object with information previously
          exported by the exportBinary() function.

          For trees where all objects only occur once in the tree this process is simple and each object will only have one parent
          and any number of children. Where the table gets usefull is where an object occurs more than once in the tree. In these
          cases the table will keep track of where the object is used.

          The table support streaming data to and from QDataStream and is used whenever a binary export/import operation is performed
          on an observer object.

          \sa Observer
          */

        class QTILIITES_CORE_SHARED_EXPORT ObserverRelationalTable
        {
        public:
            //! Constructs an observer relational table for the given observer.
            /*!
              \param observer The observer to construct the table for.
              \param exportable_subject_only When true, only exportable subjects are added to the table. Otherwise all objects are added. This is usefull when the table is used to verify observer exports.
              */
            ObserverRelationalTable(Observer* observer, bool exportable_subject_only = false);
            //! Copy constructor.
            ObserverRelationalTable(const ObserverRelationalTable &other);
            //! Empty constructor. Only use this constructor when you populate the table using a data stream.
            /*!
              For example:
              \code
                QDataStream test_stream_in(&test_file);    // read the data serialized from the file
                ObserverRelationalTable readback_table;
                readback_table.importBinary(test_stream_in);
              \endcode
              */
            ObserverRelationalTable();
            ~ObserverRelationalTable();

            bool operator==(const ObserverRelationalTable other) {
                return compare(other);
            }
            bool operator!=(const ObserverRelationalTable other) {
                return !compare(other);
            }

            //! Compares this table with another relational table. Returns true if they match, false otherwise.
            bool compare(ObserverRelationalTable table);
            //! Returns the number of entries in the table.
            int count() const;
            //! Returns the entry with the given visitor ID.
            RelationalTableEntry* entryWithVisitorID(int visitor_id);
            //! Returns the entry with the given session ID.
            RelationalTableEntry* entryWithSessionID(int session_id);
            //! Returns the entry with the given previous session ID.
            RelationalTableEntry* entryWithPreviousSessionID(int session_id);
            //! Returns the entry at position index.
            RelationalTableEntry* entryAt(int index);
            //! Returns the entry at position index.
            RelationalTableEntry* entryAt(int index) const;
            //! Returns true if all the objects in the pointer list matches the objects in the table using the visitor ID property on each object. This comparison does not take any relational data into account.
            bool compareObjects(QList<QPointer<QObject> >& objects) const;

            //! Exports the relational table to the given data stream.
            bool exportBinary(QDataStream& stream) const;
            //! Imports the relational table to the given data stream.
            bool importBinary(QDataStream& stream);
            
            //! Prints the table information to the debug output.
            void dumpTableInfo();

            //! Gets the visitor ID of an object. Returns -1 if no visitor ID exists.
            static int getVisitorID(QObject* obj) {
                if (!obj)
                    return -1;

                QVariant prop_variant = obj->property(OBSERVER_VISITOR_ID);
                if (prop_variant.isValid() && prop_variant.canConvert<SharedObserverProperty>()) {
                    SharedObserverProperty prop = prop_variant.value<SharedObserverProperty>();
                    if (prop.isValid()) {
                         return prop.value().toInt();
                    }
                }
                return -1;
            }          

            //! Function to remove all visitorID properties in the specified hierarchy.
            static void removeRelationalProperties(Observer* observer) {
                observer->setProperty(OBSERVER_VISITOR_ID,QVariant());
                observer->setProperty(OBJECT_LIMITED_EXPORTS,QVariant());

                for (int i = 0; i < observer->subjectCount(); i++) {
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

                    if (!obs) {
                        for (int r = 0; r < obj->children().count(); r++) {
                            obs = qobject_cast<Observer*> (obj->children().at(r));
                            if (obs) {
                                has_child_observer = true;
                                break;
                            }
                        }
                    }

                    if (is_iface && is_observer) {
                        removeRelationalProperties(obs);
                    } else if (!is_iface && has_child_observer) {
                        removeRelationalProperties(obs);
                    } else if ((is_iface && !is_observer) || (!is_iface)) {
                        obj->setProperty(OBSERVER_VISITOR_ID,QVariant());
                        obj->setProperty(OBJECT_LIMITED_EXPORTS,QVariant());
                    }
                }
            }

        private:
            //! Recursive function to construct the relational table. Returns the ID given to observer.
            RelationalTableEntry* constructTable(Observer* observer, bool exportable_subject_only = false);
            //! Gets the ownership of an object. Returns -1 if no ownership property exists.
            int getOwnership(QObject* obj) const;
            //! Add the current visitor ID to the object. Returns the visitor ID or -1 if an error occured or if the property already exists.
            int addVisitorID(QObject* obj);
            //! Add a property to the object which observer exports check to see if the object must be exported only once.
            int addLimitedExportProperty(QObject* obj);
            //! Gets the specific parent of an object. Returns -1 if no specific parent exists.
            int getSpecificParent(QObject* obj) const;

            ObserverRelationalTableData* d;
        };
    }
}

#endif // OBSERVERRELATIONALTABLE_H
