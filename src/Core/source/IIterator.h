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

#ifndef IITERATOR_H
#define IITERATOR_H

#include "ObjectManager.h"
using namespace Qtilities::Core;

namespace Qtilities {
    namespace Core {
        namespace Interfaces {        
            /*!
            \class IIterator<T>
            \brief Abstract interface which defines standard operations for all non-const iterators in %Qtilities.

            <i>This class was added in %Qtilities v1.0.</i>
            */
            template <class T>
            class IIterator
            {
            public:
                //! The first item in the context iterated through.
                virtual T* first() = 0;
                //! The last item in the context iterated through.
                virtual T* last() = 0;
                //! Gets the current item in the context iterated through.
                virtual T* current() const = 0;
                //! Sets the current item in the context iterated through.
                virtual void setCurrent(const T* current) = 0;
                //! The next item in the context iterated through.
                virtual T* next() = 0;
                //! The previous item in the context iterated through.
                virtual T* previous() = 0;
                //! Indicates if a next item exists.
                virtual bool hasNext() {
                    T* current_T = current();
                    if (next()) {
                        setCurrent(current_T);
                        return true;
                    } else {
                        setCurrent(current_T);
                        return false;
                    }
                }
                //! Indicates if a previous item exists.
                virtual bool hasPrevious() {
                    T* current_T = current();
                    if (previous()) {
                        setCurrent(current_T);
                        return true;
                    } else {
                        setCurrent(current_T);
                        return false;
                    }
                }
                //! Prefix increment.
                virtual T* operator++() {
                    return next();
                }
                //! Postfix increment.
                virtual T* operator++(int x) {
                    Q_UNUSED(x);
                    T* tmp = current();
                    next();
                    return tmp;
                }
                //! Prefix decrement
                virtual T* operator--() {
                    return previous();
                }
                //! Postfix decrement
                virtual T* operator--(int x) {
                    Q_UNUSED(x);
                    T* tmp = current();
                    previous();
                    return tmp;
                }
            protected:
                IIterator() {}
                virtual ~IIterator() {}
            };

            /*!
            \class IConstIterator<T>
            \brief Abstract interface which defines standard operations for all const iterators in %Qtilities.

            <i>This class was added in %Qtilities v1.0.</i>
            */
            template <class T>
            class IConstIterator
            {
            public:
                //! The first item in the context iterated through.
                virtual const T* first() = 0;
                //! The last item in the context iterated through.
                virtual const T* last() = 0;
                //! The current item in the context iterated through.
                virtual const T* current() const = 0;
                //! Sets the current item in the context iterated through.
                virtual void setCurrent(const T* current) = 0;
                //! The next item in the context iterated through.
                virtual const T* next() = 0;
                //! The previous item in the context iterated through.
                virtual const T* previous() = 0;
                //! Indicates if a next item exists.
                virtual bool hasNext() {
                    T* current_T = current();
                    if (next()) {
                        setCurrent(current_T);
                        return true;
                    } else {
                        setCurrent(current_T);
                        return false;
                    }
                }
                //! Indicates if a previous item exists.
                virtual bool hasPrevious() {
                    T* current_T = current();
                    if (previous()) {
                        setCurrent(current_T);
                        return true;
                    } else {
                        setCurrent(current_T);
                        return false;
                    }
                }
                //! Prefix increment.
                virtual const T* operator++() {
                    return next();
                }
                //! Postfix increment.
                virtual const T* operator++(int x) {
                    Q_UNUSED(x);
                    const T* tmp = current();
                    next();
                    return tmp;
                }
                //! Prefix decrement
                virtual const T* operator--() {
                    return previous();
                }
                //! Postfix decrement
                virtual const T* operator--(int x) {
                    Q_UNUSED(x);
                    const T* tmp = current();
                    previous();
                    return tmp;
                }

            protected:
                IConstIterator() {}
            };
        }
    }
}

#endif // IITERATOR_H
