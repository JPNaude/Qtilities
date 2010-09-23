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

#ifndef IFACTORY
#define IFACTORY

#include "QtilitiesCore_global.h"
#include "ObserverProperty.h"
#include "PointerList.h"

#include <Logger.h>

#include <QObject>
#include <QStringList>
#include <QDataStream>

using namespace Qtilities::Logging;

class QDomDocument;
class QDomElement;

namespace Qtilities {
    namespace Core {
        /*!
          \struct IFactoryTag
          \brief The IFactoryTag struct contains all the information required to create an object instance using the object manager.
            The IFactoryTag struct contains all the information required to create an instance through the
            Qtilities::Core::Interfaces::IFactory::createInstance() method.

            That is:
            - The factory which must be used.
            - The tag to use in that factory.
            - And the name that must be given to the reconstructed object.

            \sa Factory, Interfaces::IFactory
         */
        struct QTILIITES_CORE_SHARED_EXPORT IFactoryTag {
        public:
            IFactoryTag() {
                d_factory_tag = QString();
                d_instance_tag = QString();
                d_instance_name = QString();
            }
            IFactoryTag(QDataStream& stream) {
                importBinary(stream);
            }
            IFactoryTag(QDomDocument* doc, QDomElement* object_node);
            /*!
              \param factory_tag The factory tag which identifies the factory to be used when constructing the new instance. The Qtilities::Core::Interfaces::IObjectManager::factoryReference() function takes this tag
              as a parameter. Make sure the factory you want to use is registered in the object manager under this tag name.
              \param instance_tag The instance tag which identifies the tag to be used in the factory identified by factory_tag.
              \param instance_name The name that must be given to the newly created object.
              */
            IFactoryTag(const QString& factory_tag, const QString& instance_tag, const QString& instance_name = QString()) {
                d_factory_tag = factory_tag;
                d_instance_tag = instance_tag;
                d_instance_name = instance_name;
            }
            IFactoryTag(const IFactoryTag& ref) {
                d_factory_tag = ref.d_factory_tag;
                d_instance_tag = ref.d_instance_tag;
                d_instance_name = ref.d_instance_name;
            }
            void operator=(const IFactoryTag& ref) {
                d_factory_tag = ref.d_factory_tag;
                d_instance_tag = ref.d_instance_tag;
                d_instance_name = ref.d_instance_name;
            }
            virtual bool exportBinary(QDataStream& stream) const {
                stream << (quint32) 0xDDDDDDDD;
                stream << d_factory_tag;
                stream << d_instance_name;
                stream << d_instance_tag;
                stream << (quint32) 0xDDDDDDDD;
                return true;
            }
            virtual bool importBinary(QDataStream& stream) {
                quint32 ui32;
                stream >> ui32;
                if (ui32 != (quint32) 0xDDDDDDDD) {
                    LOG_ERROR("IFactoryTag binary import failed to detect start marker. Import will fail.");
                    return false;
                }
                stream >> d_factory_tag;
                stream >> d_instance_name;
                stream >> d_instance_tag;
                stream >> ui32;
                if (ui32 != (quint32) 0xDDDDDDDD) {
                    LOG_ERROR("IFactoryTag binary import failed to detect end marker. Import will fail.");
                    return false;
                }
                return true;
            }
            /*!
              In the case of IFactoryTag, this function will add the factory tag, instance tag etc. as
              attributes on \p object_node.
              */
            virtual bool exportXML(QDomDocument* doc, QDomElement* object_node) const;
            /*!
              \note If \p object_node does not have an \p FactoryTag attribute associated with it,
              the &Qtilities factory tag is used by default.
              */
            virtual bool importXML(QDomDocument* doc, QDomElement* object_node);

            //! Returns true if this object contains the neccessary information to be used during object construction.
            bool isValid() {
                if (d_factory_tag.isEmpty())
                    return false;
                if (d_instance_tag.isEmpty())
                    return false;
                return true;
            }

            //! The name of the factory which must be used to create the instance.
            QString d_factory_tag;
            //! The tag that must be used in the factory during instance creation.
            QString d_instance_tag;
            //! The name that must be given to the instance once it is created. This is done by calling setObjectName().
            QString d_instance_name;
        };

        namespace Interfaces {
            /*!
            \class IFactory
            \brief Objects managing instances of factories can implement this interface if they want to expose these factories to the object manager.

            Objects managing instances of factories can implement this interface if they want to expose these factories to the object manager using the
            Qtilities::Core::Interfaces::IObjectManager::registerIFactory() function. The interface can be used to represent multiple factories
            represented by QString values.  To get a list of all factories provided through the interface, see the factoryTags() function.
            */
            class QTILIITES_CORE_SHARED_EXPORT IFactory {
            public:
                IFactory() {}
                virtual ~IFactory() {}

                //! Provides the names of all the factories exposed through this interface.
                virtual QStringList factoryNames() const = 0;
                //! Provides the tags in a specific factory.
                virtual QStringList factoryTags(const QString& factory_name) const = 0;
                //! Constructs an instance in a specified factory and return the
                virtual QObject* createInstance(const IFactoryTag& ifactory_data) = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IFactory,"com.Qtilities.Core.IFactory/1.0")

#endif // IFACTORY
