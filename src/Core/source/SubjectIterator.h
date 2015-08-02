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

#ifndef SUBJECT_ITERATOR_H
#define SUBJECT_ITERATOR_H

#include <QObject>
#include <QString>

#include <QtilitiesLogging>

#include "IIterator.h"
#include "QtilitiesCore_global.h"
#include "Observer.h"

namespace Qtilities {
    namespace Core {
            /*!
            \class SubjectIterator
            \brief An non-const iterator which iterates throught the subjects of an Observer.

            The SubjectIterator allows you to easily iterate over the subjects in an observer in the order shown below:

            \image html trees_subject_iteration.jpg "Subject Iteration Order"

            Lets build this example tree:

\code
TreeNode node;
TreeItem* item1 = node.addItem("1");
TreeItem* item2 = node.addItem("2");
node.addItem("3");
node.addItem("4");

SubjectIterator<Qtilities::CoreGui::TreeItem> itr(item1);

qDebug() << itr.current()->objectName();
while (itr.hasNext()) {
    qDebug() << itr.next()->objectName();
}

// In this case the result would be:
// >> 1
// >> 2
// >> 3
// >> 4
\endcode

            It is also possible to specify the current location of your iterator:

\code
SubjectIterator<Qtilities::CoreGui::TreeItem> itr(item2);

// In this case item1 will be skipped:
qDebug() << itr.current()->objectName();
while (itr.hasNext()) {
    qDebug() << itr.next()->objectName();
}

// In this case the result would be:
// >> 2
// >> 3
// >> 4
\endcode

            In the simple examples above we didn't need to worry about cases where tree items can have multiple parents. If they do have multiple parents, we must specify the Observer that we are interested in:

\code
TreeNode node;
TreeNode* nodeA = node.addNode("A");
TreeNode* nodeB = node.addNode("B");
nodeA->addItem("1");
TreeItem* shared_item = nodeA->addItem("2");
nodeA->addItem("3");
nodeB->addItem("4");
nodeB->addItem("5");
nodeB->attachSubject(shared_item);
nodeB->addItem("6");

// If we want to iterate through the subjects in nodeA:
SubjectIterator<QObject> itrA(nodeA,SubjectIterator<QObject>::IterateChildren);

// In this case item1 will not be skipped (approach 1):
while (itrA.hasNext()) {
    qDebug() << itrA.current()->objectName();
    itrA.next();
}

// In this case the result would be:
// >> 1
// >> 2
// >> 3

// If we want to iterate through the subjects in nodeB:
SubjectIterator<QObject> itrB(nodeB,SubjectIterator<QObject>::IterateChildren);

// In this case item1 will be skipped (approach 2):
qDebug() << itrB.current()->objectName();
while (itrB.hasNext()) {
    qDebug() << itrB.next()->objectName();
}

// In this case the result would be:
// >> 4
// >> 5
// >> 2
// >> 6
\endcode

            \sa ConstSubjectIterator, TreeIterator

            <i>This class was added in %Qtilities v1.0.</i>
        */
        template <class T>
        class SubjectIterator : public Interfaces::IIterator<T>
        {
        public:
            //! The level to use when passing only an observer in your constructor.
            enum ObserverIterationLevel {
                IterateChildren,    /*!< Iterate on the observer's children. */
                IterateSiblings     /*!< Iterate on the observer's siblings. In this case T must be a subclass of Observer. */
            };

            //! Default constructor.
            /*!
             *\param subject The current subject where your iterator must start.
             *\param observer This is an optional parameter which is needed when your subject is observed in multiple contexts. In that case, the observer must be specified in order to know which observer parent to use.
             *\param iterator_id Internal iterator ID. You should never use this directly.
             */
            SubjectIterator(const T* subject,
                            const Observer* observer = 0,
                            int iterator_id = -1) :
                d_root(subject),
                d_parent_observer(observer)
            {
                d_current = subject;
                if (iterator_id == -1)
                    d_iterator_id = OBJECT_MANAGER->getNewIteratorID();
                else
                    d_iterator_id = iterator_id;
            }
            //! Observer based constructor.
            /*!
             *\param observer The observer that must be used in cases where multiple subjects have multiple parents.
             *\param iteration_level Indicates on which level the observer must be interated.
             *\param sibling_iteration_parent_observer When interating over siblings of an obsever (iteration_level = IterateSiblings),
             *it is necessary to specify the parent of the siblings you are iterating when any of the siblings has multiple parents.
             *\param iterator_id Internal iterator ID. You should never use this directly.
             */
            SubjectIterator(const Observer* observer,
                            ObserverIterationLevel iteration_level,
                            const Observer* sibling_iteration_parent_observer = 0,
                            int iterator_id = -1) :
                d_root(0),
                d_parent_observer(observer)
            {
                if (iteration_level == IterateChildren) {
                    QList<QObject*> subjects = observer->subjectReferences();
                    if (subjects.count() > 0) {
                        d_current = qobject_cast<T*>(subjects[0]);
                        d_root = qobject_cast<T*>(subjects[0]);
                    } else
                        d_current = 0;
                } else if (iteration_level == IterateSiblings) {
                    d_current = observer;
                    d_parent_observer = sibling_iteration_parent_observer;
                    d_root = observer;
                }

                if (iterator_id == -1)
                    d_iterator_id = OBJECT_MANAGER->getNewIteratorID();
                else
                    d_iterator_id = iterator_id;
            }

            T* first() {
                QList<QObject*> subjects = getSubjects(getParent());
                if(subjects.count() != 0)
                    d_current = qobject_cast<T*>(subjects[0]);
                else
                    d_current = NULL;
                return const_cast<T*> (d_current);
            }

            T* last() {
                QList<QObject*> subjects = getSubjects(getParent());
                if(subjects.count() != 0)
                    d_current = qobject_cast<T*>(subjects[subjects.count() - 1]);
                else
                    d_current = NULL;
                return const_cast<T*> (d_current);
            }

            T* current() const {
                return const_cast<T*> (d_current);
            }

            void setCurrent(const T* current) {
                d_current = current;
            }

            T* next() {
                QList<QObject*> subjects = getSubjects(getParent());
                int current_index = getCurrentIndex(subjects);

                // The subject was found:
                if (current_index != -1) {
                    // Check if there is another subject after this one:
                    if (current_index < (subjects.count() - 1)) {
                        d_current = qobject_cast<T*>(subjects[current_index + 1]);
                        return const_cast<T*> (d_current);
                    }
                }

                d_current = 0;
                return const_cast<T*> (d_current);
            }

            T* previous() {
                QList<QObject*> subjects = getSubjects(getParent());
                int current_index = getCurrentIndex(subjects);
                if (current_index > 0) {
                    d_current = qobject_cast<T*>(subjects[current_index - 1]);
                    return const_cast<T*> (d_current);
                }

                d_current = NULL;
                return const_cast<T*> (d_current);
            }

            Observer* iterationContext() const {
                return d_parent_observer;
            }

        protected:
            const Observer* getParent() {
                QList<Observer*> parents = Observer::parentReferences(d_current);
                if (parents.count() > 1) {
                    if (d_parent_observer)
                        return d_parent_observer;
                    else {
                        Observer* parent_obs = 0;
                        MultiContextProperty prop = ObjectManager::getMultiContextProperty(d_current,qti_prop_TREE_ITERATOR_SOURCE_OBS);
                        if (prop.isValid()) {
                            if (prop.hasContext(d_iterator_id)) {
                                int obs_id = prop.value(d_iterator_id).toInt();
                                parent_obs = OBJECT_MANAGER->observerReference(obs_id);
                                if (parent_obs)
                                    return parent_obs;
                            }
                        }
                        if (!parent_obs) {
                            QString warning_string;
                            warning_string.append(Q_FUNC_INFO);
                            warning_string.append("- A subject was found with multiple parents during subject iteratrion. In such cases, you must specify the parent observer in which context you are iterating. Please see the SubjectIterator documentation for more information.");
                            QStringList parents_string;
                            foreach (Observer* obs, parents)
                                parents_string << QString("\"" + obs->observerName() + "\"");
                            warning_string.append("\nDetails: object name = \"" + d_current->objectName() + "\" parents = " + parents_string.join(","));

                            qWarning() << warning_string;
                            LOG_FATAL(warning_string);
                            Q_ASSERT(parent_obs);
                        }
                    }
                    return 0;
                } else if (parents.count() == 1)
                    return parents.front();
                else
                    return 0;
            }

            QList<QObject*> getSubjects(const Observer* parent) {
                QList<QObject*> subjects;
                if (parent)
                    subjects = parent->subjectReferences();

                return subjects;
            }

            int getCurrentIndex(const QList<QObject*>& subjects) {
                const QObject* obj = d_current;
                return subjects.indexOf(const_cast<QObject*>(obj));
            }

        private:
            const T* d_current;
            const T* d_root;
            const Observer* d_parent_observer;
            int d_iterator_id;
        };

        // -----------------------------------------------
        // ConstSubjectIterator
        // -----------------------------------------------
        /*!
          \class ConstSubjectIterator
          \brief An non-const iterator which iterates throught the subjects of an Observer.

           \sa SubjectIterator, TreeIterator

           <i>This class was added in %Qtilities v1.0.</i>
         */
        template <class T>
        class ConstSubjectIterator : public Interfaces::IConstIterator<T>
        {
        public:
            //! The level to use when passing only an observer in your constructor.
            enum ObserverIterationLevel {
                IterateChildren,    /*!< Iterate on the observer's children. */
                IterateSiblings     /*!< Iterate on the observer's siblings. In this case T must be a subclass of Observer. */
            };

            //! Default constructor.
            /*!
             *\param subject The current subject where your iterator must start.
             *\param observer This is an optional parameter which is needed when your subject is observed in multiple contexts. In that case, the observer must be specified in order to know which observer parent to use.
             *\param iterator_id Internal iterator ID. You should never use this directly.
             */
            ConstSubjectIterator(const T* subject,
                                 Observer* observer = 0,
                                 int iterator_id = -1) :
                d_root(subject),
                d_parent_observer(observer)
            {
                d_current = subject;
                if (iterator_id == -1)
                    d_iterator_id = OBJECT_MANAGER->getNewIteratorID();
                else
                    d_iterator_id = iterator_id;
            }
            //! Observer based constructor.
            /*!
             *\param observer The observer that must be used in cases where multiple subjects have multiple parents.
             *\param iteration_level Indicates on which level the observer must be interated.
             *\param sibling_iteration_parent_observer When interating over siblings of an obsever (iteration_level = IterateSiblings),
             *it is necessary to specify the parent of the siblings you are iterating when any of the siblings has multiple parents.
             *\param iterator_id Internal iterator ID. You should never use this directly.
             */
            ConstSubjectIterator(const Observer* observer,
                            ObserverIterationLevel iteration_level,
                            const Observer* sibling_iteration_parent_observer = 0,
                            int iterator_id = -1) :
                d_root(0),
                d_parent_observer(observer)
            {
                if (iteration_level == IterateChildren) {
                    QList<QObject*> subjects = observer->subjectReferences();
                    if (subjects.count() > 0) {
                        d_current = qobject_cast<T*>(subjects[0]);
                        d_root = qobject_cast<T*>(subjects[0]);
                    } else
                        d_current = 0;
                } else if (iteration_level == IterateSiblings) {
                    d_current = observer;
                    d_parent_observer = sibling_iteration_parent_observer;
                    d_root = observer;
                }

                if (iterator_id == -1)
                    d_iterator_id = OBJECT_MANAGER->getNewIteratorID();
                else
                    d_iterator_id = iterator_id;
            }

            const T* first() {
                QList<QObject*> subjects = getSubjects(getParent());
                if(subjects.count() != 0)
                    d_current = qobject_cast<T*>(subjects[0]);
                else
                    d_current = NULL;
                return d_current;
            }

            const T* last() {
                QList<QObject*> subjects = getSubjects(getParent());
                if(subjects.count() != 0)
                    d_current = qobject_cast<T*>(subjects[subjects.count() - 1]);
                else
                    d_current = NULL;
                return d_current;
            }

            const T* current() const {
                return d_current;
            }

            void setCurrent(const T* current) {
                d_current = current;
            }

            const T* next() {
                QList<QObject*> subjects = getSubjects(getParent());
                int current_index = getCurrentIndex(subjects);

                // The subject was found:
                if (current_index != -1) {
                    // Check if there is another subject after this one:
                    if (current_index < (subjects.count() - 1)) {
                        d_current = qobject_cast<T*>(subjects[current_index + 1]);
                        return d_current;
                    }
                }

                d_current = NULL;
                return d_current;
            }

            const T* previous() {
                QList<QObject*> subjects = getSubjects(getParent());
                int current_index = getCurrentIndex(subjects);
                if (current_index > 0) {
                    d_current = qobject_cast<T*>(subjects[current_index - 1]);
                    return d_current;
                }

                d_current = NULL;
                return d_current;
            }

            Observer* iterationContext() const {
                return d_parent_observer;
            }

        protected:
            Observer* getParent() {
                QList<Observer*> parents = Observer::parentReferences(d_current);
                if (parents.count() > 1) {
                    if (d_parent_observer)
                        return d_parent_observer;
                    else {
                        Observer* parent_obs = 0;
                        MultiContextProperty prop = ObjectManager::getMultiContextProperty(d_current,qti_prop_TREE_ITERATOR_SOURCE_OBS);
                        if (prop.isValid()) {
                            if (prop.hasContext(d_iterator_id)) {
                                int obs_id = prop.value(d_iterator_id).toInt();
                                parent_obs = OBJECT_MANAGER->observerReference(obs_id);
                                if (parent_obs)
                                    return parent_obs;
                            }
                        }
                        if (!parent_obs) {
                            QString warning_string;
                            warning_string.append(Q_FUNC_INFO);
                            warning_string.append("- A subject was found with multiple parents during subject iteratrion. In such cases, you must specify the parent observer in which context you are iterating. Please see the SubjectIterator documentation for more information.");
                            QStringList parents_string;
                            foreach (Observer* obs, parents)
                                parents_string << QString("\"" + obs->observerName() + "\"");
                            warning_string.append("\nDetails: object name = \"" + d_current->objectName() + "\" parents = " + parents_string.join(","));

                            qWarning() << warning_string;
                            LOG_FATAL(warning_string);
                            Q_ASSERT(parent_obs);
                        }
                    }
                    return 0;
                } else if (parents.count() == 1)
                    return parents.front();
                else
                    return 0;
            }

            QList<QObject*> getSubjects(Observer* parent) {
                QList<QObject*> subjects;
                if (parent)
                    subjects = parent->subjectReferences();

                return subjects;
            }

            int getCurrentIndex(QList<QObject*> subjects) {
                const QObject* obj = d_current;
                return subjects.indexOf(const_cast<QObject*>(obj));
            }

        private:
            const T* d_current;
            const T* d_root;
            const Observer* d_parent_observer;
            int d_iterator_id;
        };
    }
}
#endif // SUBJECT_ITERATOR_H

