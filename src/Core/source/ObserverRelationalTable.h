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

#ifndef OBSERVERRELATIONALTABLE_H
#define OBSERVERRELATIONALTABLE_H

#include "QtilitiesCore_global.h"
#include "QtilitiesCoreConstants.h"
#include "Observer.h"
#include "IExportable.h"

#include <QObject>

using namespace Qtilities::Core::Properties;

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;

        // -------------------------------------------------------
        // RelationalTableEntry
        // -------------------------------------------------------
        /*!
        \struct RelationalTableEntryData
        \brief The RelationalTableEntryData stores private data used by the RelationalTableEntry class.
          */
        struct RelationalTableEntryData;

        /*!
          \class RelationalTableEntry
          \brief The RelationalTableEntry class represents a single entry in an observer relational table.
         */
        class QTILIITES_CORE_SHARED_EXPORT RelationalTableEntry  : public IExportable {
        public:
            RelationalTableEntry();
            RelationalTableEntry(int visitorID, int sessionID, const QString& name, int ownership, QObject* obj = 0);
            RelationalTableEntry(const RelationalTableEntry& other);
            bool operator==(const RelationalTableEntry& other) const;
            bool operator!=(const RelationalTableEntry& other) const;

            //! Get the parents of the entry.
            QList<int> parents() const;
            //! Set the parents of the entry.
            void setParents(QList<int> parents);
            //! Adds a parent to the entry.
            void addParent(int parent_id);
            //! Get the children of the entry.
            QList<int> children() const;
            //! Set the children of the entry.
            void setChildren(QList<int> children);
            //! Adds a child to the entry.
            void addChild(int child_id);
            //! Gets the visitor ID of the entry.
            int visitorID() const;
            //! Sets the visitor ID of the entry.
            void setVisitorID(int visitor_id);
            //! Gets the session ID of the entry.
            int sessionID() const;
            //! Sets the session ID of the entry.
            void setSessionID(int session_id);
            //! Gets the previous session ID of the entry.
            int previousSessionID() const;
            //! Sets the previous session ID of the entry.
            void setPreviousSessionID(int session_id);
            //! Gets the name of the entry.
            QString name() const;
            //! Sets the name of the entry.
            void setName(QString name);
            //! Gets the ownership of the entry.
            int ownership() const;
            //! Sets the ownership of the entry.
            void setOwnership(int ownership);
            //! Gets the parent visitor ID of the entry.
            int parentVisitorID() const;
            //! Sets the parent visitor ID of the entry.
            void setParentVisitorID(int parent_visitor_id);
            //! Gets a reference to the object for which the entry was created.
            QObject* object() const;
            //! Sets a reference to the object for which the entry was created.
            void setObject(QObject* object);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            /*!
              \note RelationalTableEntry is not a QObject, thus it returns 0.
              */
            QObject* objectBase() { return 0; }
            /*!
              \note RelationalTableEntry is not a QObject, thus it returns 0.
              */
            const QObject* objectBase() const { return 0; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            IExportable::ExportModeFlags supportedFormats() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

        private:
            QString intListToString(QList<int> list) const;
            QList<int> stringToIntList(const QString& string) const;

            RelationalTableEntryData* d;
        };

        // -------------------------------------------------------
        // ObserverRelationalTable
        // -------------------------------------------------------

        /*!
        \struct ObserverRelationalTablePrivateData
        \brief The ObserverRelationalTablePrivateData stores private data used by the ObserverRelationalTable class.
          */
        struct ObserverRelationalTablePrivateData;

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
            Qtilities::Core::Properties::qti_prop_VISITOR_ID shared property (Qtilities::Core::SharedProperty) on each item. This unique ID is the most
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
          Qtilities::Core::ObserverDotWriter is an example of such a class.

          Another powerfull feature of the ObserverRelationalTable class is its functionality to compare two ObserverRelationalTable objects with each other using the compare() method or the overloaded \p == and \p != operators.
          */

        class QTILIITES_CORE_SHARED_EXPORT ObserverRelationalTable : public IExportable
        {            
            friend class Qtilities::Core::ObserverData;

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

            bool operator==(const ObserverRelationalTable& other) const {
                return compare(other);
            }
            bool operator!=(const ObserverRelationalTable& other) const {
                return !compare(other);
            }

            //! Function to refresh the table after the observer changed.
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
            bool compare(ObserverRelationalTable table) const;
            //! Returns the number of entries in the table.
            int count() const;
            //! Returns the entry with the given visitor ID.
            RelationalTableEntry* entryWithVisitorID(int visitor_id) const;
            //! Returns the entry with the given session ID.
            RelationalTableEntry* entryWithSessionID(int session_id) const;
            //! Returns the entry with the given previous session ID.
            RelationalTableEntry* entryWithPreviousSessionID(int session_id) const;
            //! Returns the entry at position index.
            RelationalTableEntry* entryAt(int index);
            //! Returns the entry at position index.
            RelationalTableEntry* entryAt(int index) const;
            
            //! Prints the table information to the debug output.
            void dumpTableInfo() const;
            //! Gets the visitor ID of an object. Returns -1 if no visitor ID exists.
            static int getVisitorID(QObject* obj);
            //! Function to remove all visitorID properties in the specified hierarchy.
            static void removeRelationalProperties(Observer* observer);
            //! Takes the parents() of an entry and converts them to their equavalent list of observerIDs.
            /*!
              \returns A map with keys being the parent visitor IDs and the repective values being the observerIDs.

              \note If ALL of the parents cannot be converted to equavalent observer IDs, an empty map is returned.
              */
            QMap<int,int> parentsToObserverIDs(RelationalTableEntry* entry) const;

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            /*!
              \note MultiContextProperty is not a QObject, thus it returns 0.
              */
            QObject* objectBase() { return 0; }
            /*!
              \note MultiContextProperty is not a QObject, thus it returns 0.
              */
            const QObject* objectBase() const { return 0; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            IExportable::ExportModeFlags supportedFormats() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

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
            //! Gets the specific parent of an object (that is, parent with ownership of SpecificObserverOwnership). Returns -1 if no specific parent exists.
            int getSpecificParent(QObject* obj) const;

            ObserverRelationalTablePrivateData* d;
        };
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::RelationalTableEntry& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::RelationalTableEntry& stream_obj);
QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::ObserverRelationalTable& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::ObserverRelationalTable& stream_obj);

#endif // OBSERVERRELATIONALTABLE_H
