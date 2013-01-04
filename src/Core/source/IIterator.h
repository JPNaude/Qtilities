/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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
