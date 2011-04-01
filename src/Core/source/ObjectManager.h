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
            bool operator==(const SubjectTypeInfo& ref) {
                if (d_meta_type != ref.d_meta_type)
                    return false;
                if (d_name != ref.d_name)
                    return false;

                return true;
            }
            bool operator!=(const SubjectTypeInfo& ref) {
                return !(*this==ref);
            }

            QString d_meta_type;
            QString d_name;
        };

        /*!
          \class ObjectManagerPrivateData
          \brief The ObjectManagerPrivateData class stores data used by the ObjectManager class.
         */
        class ObjectManagerPrivateData;

        /*!
          \class ObjectManager
          \brief The ObjectManager provides object management features to the developer.

          For more information about object management in %Qtilities, see the \ref page_object_management article.
         */
        class QTILIITES_CORE_SHARED_EXPORT ObjectManager : public IObjectManager
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IObjectManager)
            Q_INTERFACES(Qtilities::Core::Interfaces::IFactoryProvider)
            friend class Observer;

        public:
            ObjectManager(QObject* parent = 0);
            ~ObjectManager();

        public:
            // -----------------------------------------
            // IFactoryProvider Implementation
            // -----------------------------------------
            QStringList providedFactories() const;
            QObject* createInstance(const InstanceFactoryInfo& ifactory_data);
            QStringList providedFactoryTags(const QString& factory_name) const;

            // --------------------------------
            // IObjectManager Implemenation
            // --------------------------------
            Observer* observerReference(int id) const;             
            Observer* objectPool();
            int registerObserver(Observer* observer);
            bool moveSubjects(QList<QObject*> objects, int source_observer_id, int destination_observer_id, bool silent = false);
            bool moveSubjects(QList<QPointer<QObject> > objects, int source_observer_id, int destination_observer_id, bool silent = false);
            void registerObject(QObject* obj, QtilitiesCategory category = QtilitiesCategory());
            void removeObject(QObject* obj);
            void registerFactoryInterface(FactoryInterface<QObject>* interface, FactoryItemID iface_tag);
            bool registerIFactoryProvider(IFactoryProvider* factory_iface);
            IFactoryProvider* referenceIFactoryProvider(const QString& factory_name) const;
            QStringList allFactoryNames() const;
            QStringList tagsForFactory(const QString& factory_name) const;
            QList<QObject*> registeredInterfaces(const QString& iface) const;
            QList<QPointer<QObject> > metaTypeActiveObjects(const QString& meta_type) const;
            void setMetaTypeActiveObjects(QList<QObject*> objects, const QString& meta_type);
            void setMetaTypeActiveObjects(QList<QPointer<QObject> > objects, const QString& meta_type);
            bool exportObjectProperties(QObject* obj, QDataStream& stream, Qtilities::ExportVersion version, PropertyTypeFlags property_types = AllPropertyTypes) const;
            bool importObjectProperties(QObject* new_instance, QDataStream& stream, Qtilities::ExportVersion version) const;      

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            ObjectManagerPrivateData* d;
        };
    }
}

#endif // OBJECTMANAGER_H
