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

#ifndef FACTORY_H
#define FACTORY_H

#include <QString>
#include <QStringList>
#include <QMap>

namespace Qtilities {
    namespace Core {

        //! Factory interface which is used by factories to create instances of registered class types.
        /*!
          This class defines the interface used by the Factory class. The FactoryItem class is an implementation
          of this interface.

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
        \struct FactoryData
        \brief A structure storing data related to a factory interface.
          */
        struct FactoryInterfaceData {
            FactoryInterfaceData() {}
            ~FactoryInterfaceData() {}
            FactoryInterfaceData(const QString& iface_tag, const QStringList& iface_category = QStringList(), const QStringList& iface_contexts = QStringList()) {
                tag = iface_tag;
                contexts = iface_contexts;
                category = iface_category;
            }
            FactoryInterfaceData(const FactoryInterfaceData& ref) {
                tag = ref.tag;
                contexts = ref.contexts;
                category = ref.category;
            }
            void operator=(const FactoryInterfaceData& ref) {
                tag = ref.tag;
                contexts = ref.contexts;
                category = ref.category;
            }

            QString         tag;
            QStringList     contexts;
            QStringList     category;
        };

        //! A factory class which can produce class instances through registered factory interfaces.
        /*!
          The Factory class is a template based Qt implementation of the factory progamming pattern. The class allows
          you to register factory interfaces, defined by the FactoryInterface class, where each interface is defined by
          a tag in the form of a QString. New instances of factory items (defined by FactoryItem) can then be created
          by only providing the factory with the needed interface tag.

          For more information see the \ref page_factories article.
          \sa FactoryInterface, FactoryItem
        */
        template <class BaseClass>
        class Factory
           {
           public:
              Factory() {}
              ~Factory() {}

              //! Registers a new factory interface implementation. The string 'factory_tag' can be used to generate instances of this implementation.
              /*!
                \param contexts The contexts in which the factory can produce a specific interface.
                */
              void registerFactoryInterface(FactoryInterface<BaseClass>* interface, FactoryInterfaceData iface_data) {
                  if (!iface_data.tag.isEmpty()) {
                      // Check that multiple tags don't exist
                      // Don't check the interface itself, sometimes it is desirable to
                      // to register the same interface using different tags.
                      if (!reg_ifaces.keys().contains(iface_data.tag)) {
                          reg_ifaces[iface_data.tag] = interface;
                          data_ifaces[iface_data.tag] = iface_data;
                      } else
                          return;
                 } else
                     Q_ASSERT(!iface_data.tag.isEmpty());
              }
              //! Unregister a factory interface implementation.
              inline void unregisterFactoryInterface(const QString& tag) {
                  reg_ifaces.remove(tag);
                  data_ifaces.remove(tag);
              }
              //! Returns a list of registered tags for a given context. By default all contexts are returned.
              QStringList Tags(const QString& context = QString()) const {
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
              QMap<QString, QStringList> tagCategoryMap(const QString& context = QString()) const {
                  if (!context.isEmpty()) {
                        QMap<QString, QStringList> tag_category_map;
                        for (int i = 0; i < data_ifaces.count(); i++) {
                            if (data_ifaces.values().at(i).contexts.contains(context))
                                tag_category_map[data_ifaces.values().at(i).tag] << data_ifaces.values().at(i).category;
                        }
                        return tag_category_map;
                    } else {
                        QMap<QString, QStringList> tag_category_map;
                        for (int i = 0; i < data_ifaces.count(); i++) {
                            tag_category_map[data_ifaces.values().at(i).tag] << data_ifaces.values().at(i).category;
                        }
                        return tag_category_map;
                    }
              }
              //! Function which verifies the validity of a new tag. If the tag is already present, false is returend.
              inline bool isTagValid(const QString& tag) const {
                  return data_ifaces.keys().contains(tag);
              }
              //! Creates an instance of the factory interface implementation registered with the specified tag. If an invalid tag is specified, and assertion will be raised.
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
              QMap<QString,FactoryInterfaceData> data_ifaces;
           };
    }
}

#endif // FACTORY_H
