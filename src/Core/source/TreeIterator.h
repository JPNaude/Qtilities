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

#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include <QObject>
#include <QString>

#include <QtilitiesLogging>

#include "IIterator.h"
#include "Observer.h"
#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {
        /*!
        \class TreeIterator
        \brief An iterator which iterates throught an Observer (thus also TreeNode) tree.

        The TreeIterator allows you to easily iterate over the items in a tree as shown below.

        \image html trees_full_iteration.jpg "Tree Iteration Order"

        Lets build this example tree and iterate over it:

\code
TreeNode* rootNode = new TreeNode("1");
TreeNode* nodeA = rootNode->addNode("2");
nodeA->addItem("3");
nodeA->addItem("4");
TreeNode* nodeB = rootNode->addNode("5");
nodeB->addItem("6");
nodeB->addItem("7");

TreeIterator itr(rootNode);
qDebug() <<  itr.current()->objectName();
while (itr.hasNext()) {
    qDebug() <<  itr.next()->objectName();
}

// In this case the result would be:
// >> 1
// >> 2
// >> 3
// >> 4
// >> 5
// >> 6
// >> 7

// We can also iterate backwards:
qDebug() << itr.current()->objectName();
while (itr.hasPrevious()) {
    qDebug() << itr.previous()->objectName();
}

// In this case the result would be:
// >> 7
// >> 6
// >> 5
// >> 4
// >> 3
// >> 2
// >> 1
\endcode

        \sa SubjectIterator<T>, ConstSubjectIterator<T>

        <i>This class was added in %Qtilities v0.3.</i>
        */
        class TreeIterator : public Interfaces::IIterator<QObject>
        {
        public:
            TreeIterator(Observer* top_node = 0)
                : d_top_node(top_node)
            {
                d_current = top_node;
            }

            QObject* first()
            {
                d_current = d_top_node;
                return d_current;
            }

            QObject* last()
            {
                d_current = d_top_node->treeChildren().last();
                return d_current;
            }

            QObject* current() const
            {
                return d_current;
            }

            void setCurrent(QObject* current)
            {
                d_current = current;
            }

            QObject* next()
            {
                Observer* obs = qobject_cast<Observer*> (d_current);
                if (obs) {
                    if (obs->subjectCount() > 0) {
                        d_current = obs->subjectAt(0);
                        return d_current;
                    } else {
                        SubjectIterator<QObject> sibling_itr(obs,SubjectIterator<QObject>::IterateSiblings);
                        if (sibling_itr.hasNext()) {
                            d_current = sibling_itr.next();
                            return d_current;
                        } else {
                            QObject* obj = findParentNext(d_current);
                            if (obj)
                                d_current = obj;
                            return obj;
                        }
                    }
                } else {
                    SubjectIterator<QObject> sibling_itr(d_current);
                    if (sibling_itr.hasNext()) {
                        d_current = sibling_itr.next();
                        return d_current;
                    } else {
                        QObject* obj = findParentNext(d_current);
                        if (obj)
                            d_current = obj;
                        return obj;
                    }
                }
            }

            QObject* previous()
            {
                if (d_current == d_top_node)
                    return 0;

                Observer* obs = qobject_cast<Observer*> (d_current);
                if (obs) {
                    SubjectIterator<Observer> sibling_itr(obs,SubjectIterator<Observer>::IterateSiblings);
                    if (sibling_itr.hasPrevious()) {
                        Observer* previous_obs = sibling_itr.previous();
                        // If previous observer has children, take the last child, otherwise
                        // take previous observer.
                        QList<QObject*> previous_obs_children = previous_obs->subjectReferences();
                        if (previous_obs_children.count() > 0) {
                            d_current = previous_obs_children.last();
                            return d_current;
                        } else {
                            d_current = previous_obs;
                            return d_current;
                        }
                    } else {
                        QObject* obj = findParentPrevious(d_current);
                        if (obj)
                            d_current = obj;
                        return obj;
                    }
                } else {
                    SubjectIterator<QObject> sibling_itr(d_current);
                    if (sibling_itr.hasPrevious()) {
                        d_current = sibling_itr.previous();
                        return d_current;
                    } else {
                        QObject* obj = findParentPrevious(d_current);
                        if (obj)
                            d_current = obj;
                        return obj;
                    }
                }
            }

            Observer* topNode() const {
                return d_top_node;
            }

        protected:
            //! Finds the next parent of an object.
            /*!
              If the obj has only one parent it is easy, if not we need to handle it in someway.
              */
            QObject* findParentNext(QObject* obj) {
                QList<Observer*> parents = Observer::parentReferences(obj);
                if (parents.count() == 1) {
                    if (parents.front() == d_top_node)
                        return 0;
                    else {
                        SubjectIterator<QObject> parent_itr(parents.front(),SubjectIterator<QObject>::IterateSiblings);
                        if (parent_itr.hasNext()) {
                            return parent_itr.next();
                        } else {
                            return findParentNext(parents.front());
                        }
                    }
                } else {
                    // Handle this in some way...
                    return 0;
                }
            }
            //! Finds the previous parent of an object.
            /*!
              If the obj has only one parent it is easy, if not we need to handle it in someway.
              */
            QObject* findParentPrevious(QObject* obj) {
                QList<Observer*> parents = Observer::parentReferences(obj);
                if (parents.count() > 1) {
                    // Handle this in some way...
                    return 0;
                } else {
                    return parents.front();
                }
            }

        private:
            QObject* d_current;
            Observer* const d_top_node;
        };
    }
}
#endif // TREE_ITERATOR_H
