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
#include "SubjectIterator.h"

namespace Qtilities {
    namespace Core {
        /*!
        \class TreeIterator
        \brief An iterator which iterates throught an Observer tree (thus also Qtilities::CoreGui::TreeNode).

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

        \section tree_iterator_multiple_parents When subjects are attached to multiple parents

        Because %Qtilities allows tree items to be attached to multiple trees care should be taken when iterating through trees in which subjects are attached to multiple trees. To explain this,
        take the following tree as an example:

        \image html observer_dot_graph_example_complex_tree_dot.jpg "Example Graph (Dot Layout Engine)"

        To construct the above tree is simple as demonstrated in the code snippet below:

\code

\endcode

        When TreeIterator iterates through this tree and it gets to the point where it must find the shared item's parent, there are two parents to choose from. The way
        that TreeIterator handles cases such as this it to check for the observer to which it is attached with Observer::SpecificObserverOwnership. If you build a tree using the
        Qtilities::CoreGui::TreeNode class's \p addItem() and \p addNode() functions this is done for you automatically. When you attach a tree item to a node using Observer::SpecificObserverOwnership
        the item's \p parent() will be set to be the observer it is attached to.

        Thus in the above tree, the parent called "TODO" will be used since it is the \p parent() of the item. When iterating through the other tree, the iteration will produce the wrong results.

        To summarize, take care when using trees like the one shown above and make sure you understand how the iterator will iterate through the tree.

        \sa SubjectIterator, ConstSubjectIterator

        <i>This class was added in %Qtilities v1.0.</i>
        */
        class TreeIterator : public Interfaces::IIterator<QObject>
        {
        public:
            TreeIterator(const Observer* top_node = 0)
                : d_top_node(top_node)
            {
                d_current = top_node;
            }

            QObject* first()
            {
                d_current = d_top_node;
                return const_cast<QObject*> (d_current);
            }

            QObject* last()
            {
                d_current = d_top_node->treeChildren().last();
                return const_cast<QObject*> (d_current);
            }

            QObject* current() const
            {
                return const_cast<QObject*> (d_current);
            }

            void setCurrent(const QObject* current)
            {
                d_current = current;
            }

            QObject* next()
            {
                Observer* obs = qobject_cast<Observer*> (const_cast<QObject*> (d_current));
                if (obs) {
                    if (obs->subjectCount() > 0) {
                        d_current = obs->subjectAt(0);
                        return const_cast<QObject*> (d_current);
                    } else {
                        if (d_current == d_top_node)
                            return 0;
                        else {
                            SubjectIterator<QObject> sibling_itr(obs,SubjectIterator<QObject>::IterateSiblings);
                            if (sibling_itr.hasNext()) {
                                d_current = sibling_itr.next();
                                return const_cast<QObject*> (d_current);
                            } else {
                                const QObject* obj = findParentNext(d_current);
                                if (obj) {
                                    d_current = obj;
                                    return const_cast<QObject*> (d_current);
                                } else
                                    return 0;
                            }
                        }
                    }
                } else {
                    SubjectIterator<QObject> sibling_itr(d_current);
                    if (sibling_itr.hasNext()) {
                        d_current = sibling_itr.next();
                        return const_cast<QObject*> (d_current);
                    } else {
                        const QObject* obj = findParentNext(d_current);
                        if (obj) {
                            d_current = obj;
                            return const_cast<QObject*> (d_current);
                        } else
                            return 0;
                    }
                }
            }

            QObject* previous()
            {
                if (d_current == d_top_node)
                    return 0;

                Observer* obs = qobject_cast<Observer*> (const_cast<QObject*> (d_current));
                if (obs) {
                    SubjectIterator<Observer> sibling_itr(obs,SubjectIterator<Observer>::IterateSiblings);
                    if (sibling_itr.hasPrevious()) {
                        Observer* previous_obs = sibling_itr.previous();
                        // If previous observer has children, take the last child, otherwise
                        // take previous observer.
                        QList<QObject*> previous_obs_children = previous_obs->treeChildren();
                        if (previous_obs_children.count() > 0) {
                            d_current = previous_obs_children.last();
                            return const_cast<QObject*> (d_current);
                        } else {
                            d_current = previous_obs;
                            return const_cast<QObject*> (d_current);
                        }
                    } else {
                        const QObject* obj = findParentPrevious(d_current);
                        if (obj) {
                            d_current = obj;
                            return const_cast<QObject*> (d_current);
                        } else
                            return 0;
                    }
                } else {
                    SubjectIterator<QObject> sibling_itr(d_current);
                    if (sibling_itr.hasPrevious()) {
                        d_current = sibling_itr.previous();
                        return const_cast<QObject*> (d_current);
                    } else {
                        const QObject* obj = findParentPrevious(d_current);
                        if (obj) {
                            d_current = obj;
                            return const_cast<QObject*> (d_current);
                        } else
                            return 0;
                    }
                }
            }

            Observer* topNode() const {
                return const_cast<Observer*> (d_top_node);
            }

        protected:
            //! Finds the next parent of an object.
            /*!
              If the obj has only one parent it is easy, if not we need to handle it recursively.
              */
            const QObject* findParentNext(const QObject* obj) {
                if (!obj)
                    return 0;

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
                    // If we have more than one parent, we search for a specific parent and use that route. If it does not have a specific parent, we assert:
                    Observer* parent_observer = qobject_cast<Observer*> (obj->parent());
                    qDebug() << QString("Cannot find specific parent for object \"" + obj->objectName() + " during tree iteration. Make sure you understand how trees are iterated when objects are attached to multiple parents. See the TreeIterator documentation for more details.");
                    LOG_ERROR("Cannot find specific parent for object \"" + obj->objectName() + " during tree iteration. Tree iteration does could not figure our which parent to use, thus tree will not be parsed completely.");
                    Q_ASSERT(parent_observer);
                    return parent_observer;
                }
            }
            //! Finds the previous parent of an object.
            /*!
              If the obj has only one parent it is easy, if not we need to handle it recursively.
              */
            const QObject* findParentPrevious(const QObject* obj) {
                QList<Observer*> parents = Observer::parentReferences(obj);
                if (parents.count() > 1) {
                    // Handle this in some way...
                    return 0;
                } else {
                    return parents.front();
                }
            }

        private:
            const QObject* d_current;
            const Observer* const d_top_node;
        };
    }
}
#endif // TREE_ITERATOR_H
