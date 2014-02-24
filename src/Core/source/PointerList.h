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

#ifndef POINTERLIST_H
#define POINTERLIST_H

#if _MSC_VER > 1000
#pragma once
#endif

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
            PointerListDeleter() : QObject() {}
            PointerListDeleter(const PointerListDeleter & other) : QObject() { Q_UNUSED(other) }
            ~PointerListDeleter() {}

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
        emits signals to indicate its activity.

        When creating a new PointerList object with the cleanup_when_done constructor parameter set to true, the PointerList
        will delete all objects attached to it when it is destructed.

        For example:
\code
QObject* test = new QObject;
QObject* test2 = new QObject;
PointerList* test_list = new PointerList(true);
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

        class QTILIITES_CORE_SHARED_EXPORT PointerList : public PointerListDeleter
        {

        public:
            //! Constructs a new PointerList.
            /*!
             \param cleanup_when_done When true, the PointerList will delete all objects attached to it when it is destructed.
            */
            PointerList(bool cleanup_when_done = false, QObject *parent = 0);
            ~PointerList();

            void append(QObject* object);
            void deleteAll();
            int count() const;
            void removeOne(QObject* obj);
            QObject* at(int i) const;
            QMutableListIterator<QObject*> iterator();
            QList<QObject*> toQList() const;

        protected:
            virtual void removeThisObject(QObject * object);
            virtual void addThisObject(QObject * obj);

        private:
            bool cleanup_enabled;
            QList<QObject*> list;
        };
    }
}

#endif // POINTERLIST_H
