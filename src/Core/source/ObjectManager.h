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

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "IObjectManager.h"
#include "QtilitiesCore_global.h"

#include <QList>
#include <QStringList>

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;
        class Observer;

        /*!
          \struct SubjectTypeInfo
          \brief The SubjectTypeInfo structure is used to define subject types.

          The SubjectTypeInfo structure defines a subject type in the context of an observer and is used by the
          Qtilities::Core::SubjectTypeFilter to filter subject types within an observer context.

          \note The d_meta_type property property must match the metaObject->className() string of the QObject.
         */
        struct SubjectTypeInfo {
        public:
            SubjectTypeInfo() {}
            SubjectTypeInfo(QString meta_type, QString name = QString()) {
                d_meta_type = meta_type;
                d_name = name;
            }
            SubjectTypeInfo(const SubjectTypeInfo& ref) {
                d_meta_type = ref.d_meta_type;
                d_name = ref.d_name;
            }
            void operator=(const SubjectTypeInfo& ref) {
                d_meta_type = ref.d_meta_type;
                d_name = ref.d_name;
            }

            QString d_meta_type;
            QString d_name;
        };

        /*!
          \class ObjectManagerData
          \brief The ObjectManagerData class stores data used by the ObjectManager class.
         */
        class ObjectManagerData;

        /*!
          \class ObjectManager
          \brief The ObjectManager provides object management features to the developer.

          For more information about object management in %Qtilities, see the \ref page_object_management article.
         */
        class QTILIITES_CORE_SHARED_EXPORT ObjectManager : public IObjectManager
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IObjectManager)
            Q_INTERFACES(Qtilities::Core::Interfaces::IFactory)
            friend class Observer;

        public:
            ObjectManager(QObject* parent = 0);
            ~ObjectManager();

            // -----------------------------------------
            // IFactory Implementation
            // -----------------------------------------
            QStringList factoryTags() const;
            QObject* createInstance(const IFactoryData& ifactory_data);

            // --------------------------------
            // IObjectManager Implemenation
            // --------------------------------
            Observer* observerReference(int id) const;
            Observer* const objectPool();
            int registerObserver(Observer* observer);
            bool moveSubjects(QList<QObject*> objects, int source_observer_id, int destination_observer_id, bool silent = false);
            bool moveSubjects(QList<QPointer<QObject> > objects, int source_observer_id, int destination_observer_id, bool silent = false);
            void registerObject(QObject* obj, QtilitiesCategory category = QtilitiesCategory());
            void registerFactoryInterface(FactoryInterface<QObject>* interface, FactoryInterfaceTag iface_tag);
            void registerIFactory(IFactory* factory_iface);
            IFactory* factoryReference(const QString& tag) const;
            QList<QObject*> registeredInterfaces(const QString& iface) const;
            QList<QPointer<QObject> > metaTypeActiveObjects(const QString& meta_type) const;
            void setMetaTypeActiveObjects(QList<QObject*> objects, const QString& meta_type);
            void setMetaTypeActiveObjects(QList<QPointer<QObject> > objects, const QString& meta_type);
            bool exportObjectProperties(QObject* obj, QDataStream& stream, PropertyTypeFlags property_types = AllPropertyTypes) const;
            bool importObjectProperties(QObject* new_instance, QDataStream& stream) const;
            bool constructRelationships(QList<QPointer<QObject> >& objects, ObserverRelationalTable& table) const;
            IExportable::Result exportObserverBinary(QDataStream& stream, Observer* obs, bool verbose_output = false, QList<QVariant> params = QList<QVariant>()) const;
            IExportable::Result importObserverBinary(QDataStream& stream, Observer* obs, bool verbose_output = false, QList<QVariant> params = QList<QVariant>());

        private:
            //! Function which returns all the observers in a QList<QObject*> input list.
            QList<Observer*> observerList(QList<QPointer<QObject> >& object_list) const;

            ObjectManagerData* d;
        };
    }
}

QDataStream &operator<<(QDataStream &ds, Qtilities::Core::SubjectTypeInfo &s);
QDataStream &operator>>(QDataStream &ds, Qtilities::Core::SubjectTypeInfo &s);

#endif // OBJECTMANAGER_H
