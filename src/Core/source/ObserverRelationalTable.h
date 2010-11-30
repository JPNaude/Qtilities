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
        class QTILIITES_CORE_SHARED_EXPORT RelationalTableEntry {
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

            //! The visitor IDs of all parents of this item.
            QList<qint32>   d_parents;
            //! The visitor IDs of all children of this item.
            QList<qint32>   d_children;
            //! The visitor ID of this item.
            int             d_visitorID;
            //! The session ID of this item. The session ID is only applicable to observers and is equal to their observer IDs.
            int             d_sessionID;
            //! The previous session ID of this item. The session ID is only applicable to observers and is equal to their observer IDs. This is only used during relationship reconstruction during observer export/import operations.
            int             d_previousSessionID;
            //! The objectName() of the object for which this table entry was created.
            QString         d_name;
            //! The ownership of this object in its parent observer.
            int             d_ownership;
            //! When \p d_ownership is equal to Qtilities::Core::Observer::SpecificObserverOwnership this field contains the visitor ID of its specific parent.
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
          exported through the exportBinary() function.

          For trees where all objects only occur once in the tree this process is simple and each object will only have one parent
          and any number of children. Where the table gets usefull is where an object occurs more than once in the tree. In these
          cases the table will keep track of all the places where the object is used.

          The table support streaming data to and from QDataStream and is used whenever a binary export/import operation is performed
          on an observer object.

          To construct a table is easy, for example:

\code
TreeNode* rootNode = new TreeNode("Root");
TreeNode* parentNode1 = rootNode->addNode("Parent 1");
TreeNode* parentNode2 = rootNode->addNode("Parent 2");
parentNode1->addItem("Child 1");
parentNode1->addItem("Child 2");
parentNode2->addItem("Child 3");
parentNode2->addItem("Child 4");
TreeItem* item = parentNode2->addItem("Child 5");
parentNode1->addItem(item);

// Construct relational table:
ObserverRelationalTable table(rootNode);
\endcode

            The table now contains all relational information about the tree underneath \p rootNode. Note that when an item occurs more than once in the
            tree (such as "Child 1" in the above tree), it will only occur once in the table. The number of items in the table can be found using the count()
            function and items can be accessed through the entryAt() function. Each entry in the table is a Qtilities::Core::RelationalTableEntry object which
            contains information about the parents and children of the entry among other information.

            All entries in the table has an unique visitor ID which is assigned to each object in the tree by adding the
            Qtilities::Core::Properties::OBSERVER_VISITOR_ID shared property (Qtilities::Core::SharedObserverProperty) on each item. This unique ID is the most
            important field in each entry since the parents and children of the entry are defined using their respective visitor IDs. As soon as the
            ObserverRelationalTable is deleted the visitor ID properties on all object will be removed. Thus it is very important not to create multiple
            ObserverRelationalTable objects on the same observer at the same time. The class provides functions such as entryWithVisitorID() etc. to
            easily find items with different parameters.

            The following is an example where we loop through the table constructed in the above example. This functionality is provided by the
            dumpTableInfo() function.

\code
for (int i = 0; i < table.count(); i++) {
    RelationalTableEntry* entry = table.entryAt(i);
    LOG_INFO(QString("Table Entry %1 START:").arg(i));
    LOG_INFO("-------------------------------------");
    LOG_INFO(QString("Name:                   %1").arg(entry->d_name));
    LOG_INFO(QString("Visitor ID:             %1").arg(entry->d_visitorID));
    LOG_INFO(QString("Session ID:             %1").arg(entry->d_sessionID));
    LOG_INFO(QString("Previous Session ID:    %1").arg(entry->d_previousSessionID));
    LOG_INFO(QString("Owner Visitor ID:       %1").arg(entry->d_parentVisitorID));
    LOG_INFO(QString("Child count:            %1").arg(entry->d_children.count()));
    for (int c = 0; c < entry->d_children.count(); c++) {
        RelationalTableEntry* child = table.entryWithVisitorID(entry->d_children.at(c));
        LOG_INFO(QString("> Child No.   %1").arg(c));
        LOG_INFO(QString("> Name        %1").arg(child->d_name));
        LOG_INFO(QString("> Visitor ID  %1").arg(child->d_visitorID));
        LOG_INFO(QString("> Ownership   %1").arg(child->d_ownership));
    }
    LOG_INFO(QString("Parent count: %1")).arg(entry->d_parents.count()));
    for (int c = 0; c < entry->d_parents.count(); c++) {
        RelationalTableEntry* parent = table.entryWithVisitorID(entry->d_parents.at(c));
        LOG_INFO(QString("> Parent No.  %1").arg(c));
        LOG_INFO(QString("> Name        %1").arg(parent->d_name));
        LOG_INFO(QString("> Visitor ID  %1").arg(parent->d_visitorID));
    }
    LOG_INFO("-------------------------------------");
    LOG_INFO(QString("Table Entry %1 END:").arg(i));
    LOG_INFO("-------------------------------------");
}
\endcode

          As the above example shows, it is very easy to access the information about all the relationships of items in the tree. The
          ObserverRelationalTable class allows developers to easily build classes using the tree's relationships around it. The
          Qtilities::Core::ObserverDotGraph is an example of such a class.

          Another powerfull feature of the ObserverRelationalTable class is its functionality to compare two ObserverRelationalTable objects with each other using the compare() method or the overloaded \p == and \p != operators.
          */

        class QTILIITES_CORE_SHARED_EXPORT ObserverRelationalTable
        {            
            friend class Qtilities::Core::ObjectManager;

        public:
            //! Constructs an observer relational table for the given observer.
            /*!
              \param observer The observer to construct the table for.
              \param exportable_subjects_only When true, only exportable subjects (object implementing Qtilities::Core::Interfaces::IExportable) are added to the table. Otherwise all objects are added. This is usefull when the table is used to verify observer exports.
              */
            ObserverRelationalTable(Observer* observer, bool exportable_subjects_only = false);
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

            //! Function which will refresh to refresh the table after the observer changed.
            /*!
              When the observer changed you must call refresh manually.
              */
            void refresh();
            //! Compares this table with another relational table. Returns true if they match, false otherwise.
            /*!
              Note that for compare to return true, the tables must match completely. That is, the number of
              entries in the tables must be the same and each entry must be exactly the same (except for d_sessionID and
              d_previousSessionID).
              */
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

            //! Exports the relational table to the given data stream.
            bool exportBinary(QDataStream& stream) const;
            //! Imports the relational table to the given data stream.
            bool importBinary(QDataStream& stream);
            
            //! Prints the table information to the debug output.
            void dumpTableInfo() const;

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
            //! Returns true if all the objects in the pointer list matches the objects in the table using the visitor ID property on each object. This comparison does not take any relational data into account.
            bool compareObjects(QList<QPointer<QObject> >& objects) const;
            //! Recursive function to construct the relational table. Returns the ID given to observer.
            RelationalTableEntry* constructTable(Observer* observer);
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
