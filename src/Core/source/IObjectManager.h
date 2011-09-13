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

#ifndef IOBJECTMANAGER_H
#define IOBJECTMANAGER_H

#include "QtilitiesCore_global.h"
#include "IFactoryProvider.h"
#include "QtilitiesCategory.h"
#include "Factory.h"
#include "QtilitiesProperty.h"

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
            class QTILIITES_CORE_SHARED_EXPORT IObjectManager : public QObject, virtual public IFactoryProvider
            {
                Q_OBJECT

            public:
                IObjectManager(QObject* parent = 0) : QObject(parent) {}
                virtual ~IObjectManager() {}

                //! Possible property types.
                enum PropertyTypes {
                    MultiContextProperties = 1,         /*!< Multi context properties. \sa MultiContextProperty */
                    SharedProperties = 2,               /*!< Shared observer properties. \sa SharedProperty */
                    QtilitiesInternalProperties = 4,    /*!< Shared observer properties. \sa SharedProperty */
                    NonQtilitiesProperties = 8,         /*!< Normal QVariant properties added to objects using QObject::setProperty(). */
                    AllPropertyTypes = MultiContextProperties | SharedProperties | QtilitiesInternalProperties | NonQtilitiesProperties
                };
                Q_DECLARE_FLAGS(PropertyTypeFlags, PropertyTypes)
                Q_FLAGS(PropertyTypeFlags)

                // ---------------------------------
                // Global Object Pool Functionality
                // ---------------------------------
                //! Gets the reference to an observer.
                /*!
                  \param id The observer id of the observer for which the reference must be fetched.
                  \returns The observer with the specified id. If the id is invalid 0 is returned.
                  */
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
                virtual Observer* objectPool() = 0;
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
                //! Registers an object to be included in the global object pool.
                /*!
                    \param category The category under which the object must be registered. This parameter can
                    be left out in most cases, however it is usefull when visualizing the global object pool
                    using the Qtilities::Plugins::Debug plugin for example.
                    \returns The integer value returned will be the object's unique ID in the global object pool.
                    */
                virtual void registerObject(QObject* obj, QtilitiesCategory category = QtilitiesCategory()) = 0;
                //! Removes an object to from the global object pool.
                /*!
                    This function will detach the object from the global object pool observer. It will not however
                    delete the object, it will just detach it.
                    */
                virtual void removeObject(QObject* obj) = 0;
                //! Returns all objects in the global object pool which implements the specified interface.
                /*!
For example:
\code
// Get a list of all the project items in the object pool, that is objects
// implementing the IProjectItem interface:
QList<QObject*> projectItemObjects = OBJECT_MANAGER->registeredInterfaces("IProjectItem");
QList<IProjectItem*> projectItems;

// Cast all items:
for (int i = 0; i < projectItemObjects.count(); i++) {
    IProjectItem* part = qobject_cast<IProjectItem*> (projectItemObjects.at(i));
    if (part)
        projectItems.append(part);
}
\endcode
                  */
                virtual QList<QObject*> registeredInterfaces(const QString& iface) const = 0;

                // ---------------------------------
                // Qtilities Factory Related Functionality
                // ---------------------------------
                //! Registers a factory interface inside the &Qtilities factory.
                virtual void registerFactoryInterface(FactoryInterface<QObject>* interface, FactoryItemID iface_tag) = 0;

                // ---------------------------------
                // Factory Management Related Functionality
                // ---------------------------------
                //! Registers a factory interface in the object manager.
                /*!
                  The object manager keeps track of all IFactoryProvider interfaces registered using this function.
                  It is then possible to access the IFactoryProvider interface for a specific factory using the
                  referenceIFactoryProvider() function.

                  The way that the object manager keeps track of factories and their respective IFactoryProvider interfaces
                  is done in such a way that factory names exposed by IFactoryProvider must be unique for all IFactoryProvider
                  interfaces registered using this function.

                  If a duplicate factory name is found, an error is printed and this function returns false.
                  */
                virtual bool registerIFactoryProvider(IFactoryProvider* obj) = 0;
                //! Provides a reference to the factory interface for a specific factory.
                /*!
                  This function returns the IFactoryProvider interface which contains the specified factory.
                */
                virtual IFactoryProvider* referenceIFactoryProvider(const QString& factory_name) const = 0;
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
