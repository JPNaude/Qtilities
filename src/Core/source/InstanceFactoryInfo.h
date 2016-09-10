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

#ifndef INSTANCEFACTORYINFO_H
#define INSTANCEFACTORYINFO_H

#include "QtilitiesCore_global.h"
#include "Qtilities.h"

class QDomDocument;
class QDomElement;

namespace Qtilities {
    namespace Core {
        /*!
          \struct InstanceFactoryInfo
          \brief The InstanceFactoryInfo struct contains all the information required to create an object instance using the object manager.
            The InstanceFactoryInfo struct contains all the information required to create an instance through the
            Qtilities::Core::Interfaces::IFactoryProvider::createInstance() method.

            That is:
            - The factory which must be used.
            - The tag to use in that factory.
            - And the name that must be given to the reconstructed object.

            InstanceFactoryInfo can also store other information, like a description, for the type of object that it will be used to create.

            For more information see the \ref page_factories article.

            \sa Factory, Interfaces::IFactoryProvider
         */
        class QTILIITES_CORE_SHARED_EXPORT InstanceFactoryInfo {

        public:
            InstanceFactoryInfo() {
                d_factory_tag = QString();
                d_instance_tag = QString();
                d_instance_name = QString();
                d_icon_path = QString();
            }
            InstanceFactoryInfo(QDataStream& stream, Qtilities::ExportVersion version) {
                importBinary(stream,version);
            }
            InstanceFactoryInfo(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version);
            /*!
              \param factory_tag The factory tag which identifies the factory to be used when constructing the new instance. The Qtilities::Core::Interfaces::IObjectManager::factoryReference() function takes this tag
              as a parameter. Make sure the factory you want to use is registered in the object manager under this tag name.
              \param instance_tag The instance tag which identifies the tag to be used in the factory identified by factory_tag.
              \param instance_name The name that must be given to the newly created object.
              */
            InstanceFactoryInfo(const QString& factory_tag, const QString& instance_tag, const QString& instance_name = QString()) {
                d_factory_tag = factory_tag;
                d_instance_tag = instance_tag;
                d_instance_name = instance_name;
            }
            InstanceFactoryInfo(const InstanceFactoryInfo& ref) {
                d_factory_tag = ref.d_factory_tag;
                d_instance_tag = ref.d_instance_tag;
                d_instance_name = ref.d_instance_name;
                d_description = ref.d_description;
                d_icon_path = ref.d_icon_path;
            }
            virtual ~InstanceFactoryInfo() {}

            bool operator==(const InstanceFactoryInfo& ref) const {
                if (d_factory_tag != ref.d_factory_tag)
                    return false;
                if (d_instance_tag != ref.d_instance_tag)
                    return false;
                if (d_instance_name != ref.d_instance_name)
                    return false;
                if (d_icon_path != ref.d_icon_path)
                    return false;

                return true;
            }
            bool operator!=(const InstanceFactoryInfo& ref) const {
                return !(*this==ref);
            }
            InstanceFactoryInfo& operator=(const InstanceFactoryInfo& ref) {
                if (this==&ref) return *this;

                d_factory_tag = ref.d_factory_tag;
                d_instance_tag = ref.d_instance_tag;
                d_instance_name = ref.d_instance_name;
                d_description = ref.d_description;
                d_icon_path = ref.d_icon_path;

                return *this;
            }

            //! Returns true if this object contains the necessary information to be used during object construction.
            bool isValid();

            // -------------------------------------------------------------------------------------------------
            // Custom Exporting Functions
            // Because InstanceFactoryInfo is part of IExportable, it does not implement this interface itself.
            // -------------------------------------------------------------------------------------------------
            virtual bool exportBinary(QDataStream& stream, Qtilities::ExportVersion version) const;
            virtual bool importBinary(QDataStream& stream, Qtilities::ExportVersion version);
            /*!
              In the case of InstanceFactoryInfo, this function will add the factory tag, instance tag etc. as
              attributes on \p object_node.
              */
            virtual bool exportXml(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version) const;
            /*!
              \note If \p object_node does not have an \p FactoryTag attribute associated with it,
              the %Qtilities factory tag is used by default.
              */
            virtual bool importXml(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version);

            //! The name of the factory which must be used to create the instance.
            QString d_factory_tag;
            //! The tag that must be used in the factory during instance creation.
            QString d_instance_tag;
            //! The name that must be given to the instance once it is created. This is done by calling setObjectName().
            QString d_instance_name;
            //! A description for this type of object. This is not part of any exports.
            QString d_description;
            //! A path to an icon for this type of object.
            QString d_icon_path; // We can't use QIcon here, this class is part of QtilitiesCore which does not use QtGui.
        };
    }
}

#endif // INSTANCEFACTORYINFO_H
