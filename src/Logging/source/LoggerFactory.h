/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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
