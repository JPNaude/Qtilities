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

#ifndef IOBJECTMANAGER_H
#define IOBJECTMANAGER_H

#include "QtilitiesCore_global.h"
#include "IExportable.h"
#include "IFactory.h"
#include "AbstractSubjectFilter.h"
#include "Factory.h"

#include <QList>
#include <QMap>
#include <QPointer>

namespace Qtilities {
    namespace Core {
        class Observer;
        class ObserverRelationalTable;

        namespace Interfaces {
            using namespace Qtilities::Core;
            /*!
            \class IObjectManager
            \brief Interface used to communicate with the observer manager.
              */
            class QTILIITES_CORE_SHARED_EXPORT IObjectManager : public QObject, virtual public IFactory
            {
                Q_OBJECT

            public:
                IObjectManager(QObject* parent = 0) : QObject(parent) {}
                virtual ~IObjectManager() {}

                //! Possible property types.
                enum PropertyTypes {
                    ObserverProperties = 1,     /*!< Observer properties. \sa ObserverProperty */
                    SharedProperties = 2,       /*!< Shared observer properties. \sa SharedObserverProperty */
                    AllPropertyTypes = ObserverProperties | SharedProperties
                };
                Q_DECLARE_FLAGS(PropertyTypeFlags, PropertyTypes);
                Q_FLAGS(PropertyTypeFlags);

                // ---------------------------------
                // Global Object Pool Functionality
                // ---------------------------------
                //! Gets the reference to an observer.
                virtual Observer* observerReference(int id) const = 0;
                //! Function which returns a reference to the global object pool.
                /*!
                  Care should be taken when using the object pool observer. You should for example not delete all the
                  subjects in the pool unless you know what you are doing.

                  It is recommended to use registerObject() and registeredInterfaces() instead of the object pool directly,
                  unless you require the use of additional functions on the object pool, like displaying it using an
                  ObserverWidget.

                  \note The global object pool is in a processing cycle by default. Thus if you want to display it, you
                  need to call endProcessingCycle() on it and refreshViewsLayout().
                  */
                virtual Observer* const objectPool() = 0;
                //! A function which moves a list of objects from one observer to another observer.
                /*!
                  This function will attempt to move subjects from one observer context to another. If any of the subjects
                  cannot be attached, it will be skipped and attempt to move the next subject in the list. If any of the
                  subjects could not be attached, the function will return false. If all subjects was moved succesfully the
                  function will return true.

                  \param objects The objects which must be moved.
                  \param source_observer_id The source observer ID.
                  \param destination_observer_id The destination observer ID.
                  \param silent When true the subjects must be moved without showing any dialogs.
                  \return True if all objects were moved successfully, false if some of the objects failed.
                  */
                virtual bool moveSubjects(QList<QObject*> objects, int source_observer_id, int destination_observer_id, bool silent = false) = 0;
                //! Move subjects by providing the objects as a list with smart pointers.
                /*!
                  This function will attempt to move subjects from one observer context to another. If any of the subjects
                  cannot be attached, it will be skipped and attempt to move the next subject in the list. If any of the
                  subjects could not be attached, the function will return false. If all subjects was moved succesfully the
                  function will return true.

                  \param objects The objects which must be moved.
                  \param source_observer_id The source observer ID.
                  \param destination_observer_id The destination observer ID.
                  \param silent When true the subjects must be moved without showing any dialogs.
                  \return True if all objects were moved successfully, false if some of the objects failed.
                  */
                virtual bool moveSubjects(QList<QPointer<QObject> > objects, int source_observer_id, int destination_observer_id, bool silent = false) = 0;
                //! Registers an observer in the observer manager.
                virtual int registerObserver(Observer* observer) = 0;
                //! Registers an object to be included in the global object pool. The integer value returned will be the object's unique ID in the global object pool.
                virtual void registerObject(QObject* obj, QtilitiesCategory category = QtilitiesCategory()) = 0;
                //! Returns all objects in the global object pool which implements the specified interface.
                virtual QList<QObject*> registeredInterfaces(const QString& iface) const = 0;

                // ---------------------------------
                // Factory Related Functionality
                // ---------------------------------
                //! Registers a factory interface inside the Qtilities factory.
                virtual void registerFactoryInterface(FactoryInterface<QObject>* interface, FactoryTag iface_tag) = 0;
                //! Registers a factory interface in the object manager.
                /*!
                  The object manager keeps track of all IFactory interfaces registered using this function.
                  It is then possible to access the IFactory interface for a specific factory using the referenceIFactory()
                  function.

                  The way that the object manager keeps track of factories and their respective IFactory interfaces
                  is done in such a way that factory names exposed by IFactory must be unique for all IFactory
                  interfaces registered using this function.

                  If a duplicate factory name is found, an error is printed and this function returns false.
                  */
                virtual bool registerIFactory(IFactory* obj) = 0;
                //! Provides a reference to the factory interface for a specific factory.
                /*!
                  This function returns the IFactory interface which contains the specified factory.
                */
                virtual IFactory* referenceIFactory(const QString& factory_name) const = 0;
                //! Provides a list with the names of all the factories registered in the object manager.
                virtual QStringList allFactoryNames() const = 0;
                //! Provides a list of all the tags registered in a specific factory.
                virtual QStringList tagsForFactory(const QString& factory_name) const = 0;

                // ---------------------------------
                // Global Active Objects Functionality
                // ---------------------------------
                //! Updates the active object(s) for a specific meta type.
                /*!
                  For more information about see the \ref meta_type_object_management section of the \ref page_object_management article.

                  \sa metaTypeActiveObjects(), metaTypeActiveObjectsChanged()
                  */
                virtual void setMetaTypeActiveObjects(QList<QObject*> objects, const QString& meta_type) = 0;
                //! Update the active object(s) for a specific meta type by providing a list of smart pointers to the objects.
                virtual void setMetaTypeActiveObjects(QList<QPointer<QObject> > objects, const QString& meta_type) = 0;
                //! Returns the active object(s) for a specific meta type. If the meta type does not exist, an empty list is returned.
                /*!
                  For more information about see the \ref meta_type_object_management section of the \ref page_object_management article.

                  \sa setMetaTypeActiveObjects(), metaTypeActiveObjectsChanged()
                  */
                virtual QList<QPointer<QObject> > metaTypeActiveObjects(const QString& meta_type) const = 0;

                // ---------------------------------
                // Exporting & Importing Functionality
                // ---------------------------------
                //! Streams exportable dynamic properties about the object to the given QDataStream.
                /*!
                  To use this function make sure that all the QVariant properties have the streaming << / >> operators overloaded.
                  This is the case for all properties used in %Qtilities.
                  */
                virtual bool exportObjectProperties(QObject* obj, QDataStream& stream, PropertyTypeFlags property_types = AllPropertyTypes) const = 0;
                //! Streams exportable dynamic properties about the new_instance QObject from the given QDataStream.
                virtual bool importObjectProperties(QObject* new_instance, QDataStream& stream) const = 0;
                //! Construct relationships between a list of objects with the relational data being passed to the function as a RelationalObserverTable.
                virtual bool constructRelationships(QList<QPointer<QObject> >& objects, ObserverRelationalTable& relational_table) const = 0;
                //! Exports an observer along with all the information required to reconstruct and verify the observer hierarchy (relationships etc.)
                virtual IExportable::Result exportObserverBinary(QDataStream& stream, Observer* obs, bool verbose_output = false, QList<QVariant> params = QList<QVariant>()) const = 0;
                //! Imports an observer which was exported using the exportObserverBinary() function.
                virtual IExportable::Result importObserverBinary(QDataStream& stream, Observer* obs, bool verbose_output = false, QList<QVariant> params = QList<QVariant>()) = 0;

            signals:
                //! Signal which is emitted when the setMetaTypeActiveObjects() is finished.
                void metaTypeActiveObjectsChanged(QList<QPointer<QObject> > objects, const QString& meta_type);
                //! Signal which is emitted when a new object is added to the global object pool.
                void newObjectAdded(QObject* obj);
            };

            Q_DECLARE_OPERATORS_FOR_FLAGS(IObjectManager::PropertyTypeFlags)
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IObjectManager,"com.qtilities.Core.IObjectManager/1.0");

#endif // IOBJECTMANAGER_H
