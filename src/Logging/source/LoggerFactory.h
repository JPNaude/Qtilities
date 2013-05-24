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

#ifndef LOGGER_FACTORY_H
#define LOGGER_FACTORY_H

#include <QString>
#include <QStringList>
#include <QMap>

namespace Qtilities {
    namespace Logging {
        template <class BaseClass>
        class LoggerFactoryInterface
           {
           public:
              LoggerFactoryInterface() {}
              virtual ~LoggerFactoryInterface() {}
              virtual BaseClass *createInstance() = 0;
           };

        template <class BaseClass,class ActualClass>
        class LoggerFactoryItem : public LoggerFactoryInterface<BaseClass>
           {
           public:
              LoggerFactoryItem() {}
              virtual ~LoggerFactoryItem() {}
              inline virtual BaseClass *createInstance() {return new ActualClass;}
           protected:
              QString tag;
           };

        template <class BaseClass>
        class LoggerFactory
           {
           public:
              LoggerFactory() {}
              ~LoggerFactory() {}

              void registerFactoryInterface(const QString& tag, LoggerFactoryInterface<BaseClass>* factory_interface) {
                  if (!tag.isEmpty()) {
                      if (!reg_ifaces.values().contains(factory_interface)) {
                          reg_ifaces[tag] = factory_interface;
                      } else
                          return;
                 } else
                     Q_ASSERT(!tag.isEmpty());
              }
              inline void unregisterFactoryInterface(const QString& tag) {
                  reg_ifaces.remove(tag);
              }
              QStringList tags() const {
                    return reg_ifaces.keys();
              }             
              inline bool isTagValid(const QString& tag) const {
                  return reg_ifaces.contains(tag);
              }
              BaseClass* createInstance(const QString& tag) {
                  if (reg_ifaces.contains(tag))
                          return reg_ifaces[tag]->createInstance();
                  Q_ASSERT(!isTagValid(tag));
                  return 0;
              }

           private:
              QMap<QString,LoggerFactoryInterface<BaseClass>* > reg_ifaces;
           };
    }
}

#endif // LOGGER_FACTORY_H
