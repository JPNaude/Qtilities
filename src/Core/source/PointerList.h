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

#ifndef POINTERLIST_H
#define POINTERLIST_H

#include <QObject>
#include <QList>
#include <QtDebug>

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {
        //! The PointerListDeleter is a base class from which PointerList inherits.
        /*!
        The PointerListDeleter class provides signal and slot functionality to the PointerList class which is
        a template class, and therefore can't provide the signal and slot functionality directly.
        */
        class QTILIITES_CORE_SHARED_EXPORT PointerListDeleter : public QObject
        {
            Q_OBJECT
        protected:
            virtual void removeThisObject(QObject * obj) = 0;

        public:
            //! Constructor.
            PointerListDeleter() : QObject() {}
            //! Copy constructor.
            PointerListDeleter(const PointerListDeleter & other) : QObject() { Q_UNUSED(other) }

        signals:
            //! Signal which is emitted when an object is removed.
            void objectDestroyed(QObject* object);

        private Q_SLOTS:
            //! A slot which will remove the sender object. This slot will be connected to the destroyed(QObject *) signal of all objects added to PointerList.
            void removeSender() {
                removeThisObject(QObject::sender());
            }
        };

        //! The PointerList class monitors the lifetime of objects attached to it.
        /*!
        The PointerList provides monitoring and accessing of object pointers which are attached to it. Other than
        QObjectCleanupHandler the PointerList class provides access functions to the objects attached to it and
        emits signals to indicate its activity. PointerList is a template class, with the requirement that
        T must inherit from QObject.

        When creating a new PointerList object with the cleanup_when_done constructor parameter set to true, the PointerList
        will delete all objects attached to it when it is destructed.

        For example:
        \code
        QObject* test = new QObject;
        QObject* test2 = new QObject;
        PointerList<QObject>* test_list = new PointerList<QObject>(true);
        test_list->addObject(test);
        test_list->addObject(test2);

        delete test;
        delete test_list;
        \endcode

        The above code will result in the following actions inside test_list
        \code
        test_list: Adding object test.
        test_list: Adding object test2.
        test_list: Removing object test from the pointer list when it is destructed.
        test_list: Delete object test2 during destruction of test_list. Deleting it will also result in test2 being removed from the pointer list before it is destructed.
        \endcode
        */

        template<class T>
        class QTILIITES_CORE_SHARED_EXPORT PointerList : public PointerListDeleter, public QList<T*>
        {

        public:
            //! Constructs a new PointerList.
            /*!
             \param cleanup_when_done When true, the PointerList will delete all objects attached to it when it is destructed.
            */
            PointerList(bool cleanup_when_done = false, QObject *parent = 0) : QList<T*>() {
                Q_UNUSED(parent)

                cleanup_enabled = cleanup_when_done;
            }

            //! Copy constructor.
            PointerList(const PointerList<T*> & other): PointerListDeleter(), QList<T*>(other)
            {
                for (int i = 0;i < other.count();i++)
                {
                    addThisObject(other.at(i));
                }
            }

            ~PointerList() {
                if (cleanup_enabled) {
                    deleteAll();
                }
            }

            //! Appends a new instance of T to the PointerList.
            void append(T* object) {
                addThisObject(object);
                QList<T*>::append(object);
            }

            //! Inserts a new instance of T to the PointerList at position i.
            typename QList<T*>::iterator insert(typename QList<T*>::iterator before,  T*  object)
            {
                addThisObject(object);
                return QList<T*>::insert(before, object);
            }

            //! Implementation of overloaded + operator.
            PointerList<T> operator+ (const PointerList<T> & other) const
            {
                PointerList<T> m = *this;
                m += other;
                return m;
            }
            //! Implementation of overloaded += (Pointerlist) operator.
            PointerList<T> & operator+= (const PointerList<T> & other)
            {
                Q_FOREACH(T*t, other)
                {
                    *this << t;
                }
                return *this;
            }
            //! Implementation of overloaded += operator.
            PointerList<T> & operator+= (T*  object)
            {
                addThisObject(object);
                QList<T*>::operator+=(object);
                return *this;
            }
            //! Implementation of overloaded << (Pointerlist) operator.
            PointerList<T> & operator<< (const PointerList<T> & other)
            {
                *this += other;
                return *this;
            }
            //! Implementation of overloaded << operator.
            PointerList<T> & operator<< (T* object)
            {
                *this += object;
                return *this;
            }
            //! Implementation of overloaded = operator.
            PointerList<T> & operator= (const PointerList<T> & other)
            {
                QList<T*>::clear();
                Q_FOREACH(T*t, other)
                {
                    *this << t;
                }
                return *this;
            }
            //! Deletes all objects from the list.
            void deleteAll() {
                int total = QList<T*>::count();
                for (int i = 0; i < total; i++) {
                    delete QList<T*>::at(0);
                }
                QList<T*>::clear();
            }

        protected:
            virtual void removeThisObject(QObject * object)
            {
                removeAll(reinterpret_cast<T*>(object));
                emit objectDestroyed(object);
            }

            virtual void addThisObject(QObject * obj)
            {
                QObject::connect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(removeSender()));
            }

        private:
            bool cleanup_enabled;
        };
    }
}

#endif // POINTERLIST_H
