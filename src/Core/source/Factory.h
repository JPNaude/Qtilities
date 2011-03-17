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

#ifndef FACTORY_H
#define FACTORY_H

#include "QtilitiesCategory.h"

#include <QString>
#include <QStringList>
#include <QMap>

namespace Qtilities {
    namespace Core {
        //! Factory interface which is used by factories to create instances of registered class types.
        /*!
          This class defines the interface used by the Factory class. The FactoryItem class is an implementation
          of this interface.

          For more information see the \ref page_factories article.

          \sa Factory, FactoryItem
        */
        template <class BaseClass>
        class FactoryInterface
           {
           public:
              FactoryInterface() {}
              virtual ~FactoryInterface() {}
              virtual BaseClass *createInstance() = 0;
           };

        //! Factory item class which is used inside classes which can register themselves as items in factories.
        /*!
          This class is an implementation of the FactoryInterface interface used by the Factory class. Use this class
          if you want a factory to be able to create instances of your class.

          For more information see the \ref page_factories article.

          \sa Factory, FactoryInterface
        */
        template <class BaseClass,class ActualClass>
        class FactoryItem : public FactoryInterface<BaseClass>
           {
           public:
              FactoryItem() {}
              virtual ~FactoryItem() {}
              inline virtual BaseClass *createInstance() {return new ActualClass;}
           protected:
              QString tag;
           };

        /*!
        \struct FactoryItemID
        \brief A structure storing data related to a factory interface.

        The FactoryItemID struct is used to store information about a factory interface registered within a factory.

        For more information see the \ref page_factories article.
          */
        struct FactoryItemID {
            //! Constructs an empty FactoryItemID structure.
            FactoryItemID() {}
            ~FactoryItemID() {}
            //! Creates a new factory data object.
            /*!
              \param iface_tag The tag which can be used to produce a new instance of the interface using the Factory::newInstance() method.
              \param iface_category The category to which this interface belongs. Categories can be used to order the available interfaces registered in a factory in a categorized manner.
              */
            FactoryItemID(const QString& iface_tag, const QtilitiesCategory& iface_category = QtilitiesCategory()) {
                tag = iface_tag;
                category = iface_category;
            }
            //! FactoryItemID copy constructor.
            FactoryItemID(const FactoryItemID& ref) {
                tag = ref.tag;
                category = ref.category;
            }
            //! Overload of the = operator.
            void operator=(const FactoryItemID& ref) {
                tag = ref.tag;
                category = ref.category;
            }

            QString             tag;
            QtilitiesCategory   category;
        };

        //! A factory class which can produce class instances through registered factory interfaces.
        /*!
          The Qtilities::Core::Factory class is a template based Qt implementation of the factory programming pattern. The class allows you to register factory interfaces, defined by the Qtilities::Core::FactoryInterface class. New instances of factory items (defined by Qtilities::Core::FactoryItem) can then be created by only providing the factory with the needed interface ID defined by Qtilities::Core::FactoryItemID.

          For more information see the \ref page_factories article.

          \sa FactoryInterface, FactoryItem, Interfaces::IFactoryProvider
        */
        template <class BaseClass>
        class Factory
           {
           public:
              Factory() {}
              ~Factory() {}

              //! Registers a new factory interface implementation. The string 'factory_tag' can be used to generate instances of this implementation.
              /*!
                \param interface The factory item interface.
                \param iface_data A structure providing information about the factory item interface. If another item interface with the same tag already exists, the function call will fail and false will be returned. The tag must also contain a value.
                \returns True if the interface was registered successfully, false otherwise.
                */
              bool registerFactoryInterface(FactoryInterface<BaseClass>* interface, FactoryItemID iface_data) {
                  if (!iface_data.tag.isEmpty()) {
                      // Check that multiple tags don't exist
                      // Don't check the interface itself, sometimes it is desirable to
                      // to register the same interface using different tags.
                      if (!reg_ifaces.keys().contains(iface_data.tag)) {
                          reg_ifaces[iface_data.tag] = interface;
                          data_ifaces[iface_data.tag] = iface_data;
                          return true;
                      } else
                          return false;
                  } else {
                      Q_ASSERT(!iface_data.tag.isEmpty());
                      return false;
                  }
              }
              //! Unregister a factory interface implementation.
              inline void unregisterFactoryInterface(const QString& tag) {
                  reg_ifaces.remove(tag);
                  data_ifaces.remove(tag);
              }
              //! Returns a list of registered tags for a given context. By default all contexts are returned.
              QStringList tags(const QString& context = QString()) const {
                if (!context.isEmpty()) {
                    QStringList tags;
                    for (int i = 0; i < data_ifaces.count(); i++) {
                        if (data_ifaces.values().at(i).contexts.contains(context))
                            tags << data_ifaces.values().at(i).tag;
                    }
                    return tags;
                } else {
                    QStringList tags;
                    for (int i = 0; i < data_ifaces.count(); i++) {
                        tags << data_ifaces.values().at(i).tag;
                    }
                    return tags;
                }
              }             
              //! Returns a tag-category map of registered tags for a given context. By default all contexts are returned.
              QMap<QString, QtilitiesCategory> tagCategoryMap(const QString& context = QString()) const {
                  if (!context.isEmpty()) {
                        QMap<QString, QtilitiesCategory> tag_category_map;
                        for (int i = 0; i < data_ifaces.count(); i++) {
                            if (data_ifaces.values().at(i).contexts.contains(context))
                                tag_category_map[data_ifaces.values().at(i).tag] = data_ifaces.values().at(i).category;
                        }
                        return tag_category_map;
                    } else {
                        QMap<QString, QtilitiesCategory> tag_category_map;
                        for (int i = 0; i < data_ifaces.count(); i++) {
                            tag_category_map[data_ifaces.values().at(i).tag] = data_ifaces.values().at(i).category;
                        }
                        return tag_category_map;
                    }
              }
              //! Function which verifies the validity of a new tag. If the tag is already present, false is returend.
              inline bool isTagValid(const QString& tag) const {
                  return data_ifaces.keys().contains(tag);
              }
              //! Creates an instance of the factory interface implementation registered with the specified tag. If an invalid tag is specified, null will be returned.
              BaseClass* createInstance(const QString& tag) {
                  for (int i = 0; i < data_ifaces.count(); i++) {
                      if (tag == data_ifaces.values().at(i).tag)
                          return reg_ifaces.values().at(i)->createInstance();
                  }
                  Q_ASSERT(!isTagValid(tag));
                  return 0;
              }

           private:
              QMap<QString,FactoryInterface<BaseClass>* > reg_ifaces;
              QMap<QString,FactoryItemID> data_ifaces;
           };
    }
}

#endif // FACTORY_H
