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
            FactoryItemID& operator=(const FactoryItemID& ref) {
                if (this==&ref) return *this;

                tag = ref.tag;
                category = ref.category;

                return *this;
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
                \param factory_interface The factory item interface.
                \param iface_data A structure providing information about the factory item interface. If another item interface with the same tag already exists, the function call will fail and false will be returned. The tag must also contain a value.
                \returns True if the interface was registered successfully, false otherwise.
                */
              bool registerFactoryInterface(FactoryInterface<BaseClass>* factory_interface, FactoryItemID iface_data) {
                  if (!iface_data.tag.isEmpty()) {
                      // Check that multiple tags don't exist
                      // Don't check the interface itself, sometimes it is desirable to
                      // to register the same interface using different tags.
                      if (!reg_ifaces.contains(iface_data.tag)) {
                          reg_ifaces[iface_data.tag] = factory_interface;
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
              /*!
                \param category_filter When a valid category is provided, only tags in that category are returned.

                \sa tagCategoryMap()
                */
              QStringList tags(const QtilitiesCategory& category_filter = QtilitiesCategory()) const {
                    QStringList tags;
                    for (int i = 0; i < data_ifaces.count(); i++) {
                        if (category_filter.isValid()) {
                            if (data_ifaces.values().at(i).category == category_filter)
                                tags << data_ifaces.values().at(i).tag;
                        } else
                            tags << data_ifaces.values().at(i).tag;
                    }
                    return tags;
              }             
              //! Returns a tag-category map of registered tags.
              QMap<QString, QtilitiesCategory> tagCategoryMap() const {
                    QMap<QString, QtilitiesCategory> tag_category_map;
                    for (int i = 0; i < data_ifaces.count(); i++) {
                        tag_category_map[data_ifaces.values().at(i).tag] = data_ifaces.values().at(i).category;
                    }
                    return tag_category_map;
              }
              //! Function which verifies the validity of a new tag. If the tag is already present, false is returned.
              inline bool isTagValid(const QString& tag) const {
                  return data_ifaces.contains(tag);
              }
              //! Function which returns the category under which the specified tag was registered.
              /*!
                <i>This function was added in %Qtilities v1.2.</i>
                */
              inline QtilitiesCategory categoryForTag(const QString& tag) const {
                  if (data_ifaces.contains(tag)) {
                      FactoryItemID item_data = data_ifaces[tag];
                      return item_data.category;
                  }
                  return QtilitiesCategory();
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
