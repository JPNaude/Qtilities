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

#ifndef OBJECTSCOPEWIDGET_H
#define OBJECTSCOPEWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "IActionProvider.h"

#include <Observer>
#include <IContext>

#include <QWidget>
#include <QObject>
#include <QVariant>
#include <QPointer>
#include <QTableWidgetItem>

namespace Ui {
    class ObjectScopeWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \struct ObjectScopeWidgetPrivateData
        \brief The ObjectScopeWidgetPrivateData stores data used by the ObjectScopeWidget class.
          */
        struct ObjectScopeWidgetPrivateData;

        /*!
        \class ObjectScopeWidget
        \brief The ObjectScopeWidget class provides details about the contexts to which an object is attached to.

        As explained in the \ref page_observers article, an object can be attached to multiple observers.
        In such cases it be useful to display the scope of an object within the application using the
        ObjectScopeWidget widget. An example object scope widget is shown below. Tooltips provide more information about
        specific contexts to which an object is attached and different columns are available to
        provide details about each context.

        \image html object_scope_widget_without_owner.jpg "Object Scope Widget Without Owner"

        When an object is attached to an observer context using Qtilities::Core::Observer::SpecificObserverOwnership
        a new column appear in the ObjectScopeWidget indicating object ownership as shown below.

        \image html object_scope_widget_with_owner.jpg "Object Scope Widget With Owner"

        \todo
        - Does not catch detachment if last scoped observer is detached under manual ownership.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObjectScopeWidget : public QWidget, public IContext {
            Q_OBJECT
            Q_DISABLE_COPY(ObjectScopeWidget)
            Q_ENUMS(ColumnIDs)
            Q_INTERFACES(Qtilities::Core::Interfaces::IContext)

        public:
            explicit ObjectScopeWidget(QWidget *parent = 0);
            virtual ~ObjectScopeWidget();

            enum ColumnIDs {
                NameColumn = 0,
                UsesInstanceNameColumn = 1,
                OwnerColumn = 2
            };

            //! When true, the object name will be shown above the object scope table.
            void setNameVisible(bool visible);
            bool eventFilter(QObject *object, QEvent *event);
            //! Action provider interface through which this widget provides its actions.
            IActionProvider* actionProvider();

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QWidget, ObjectScopeWidget> factory;

            // --------------------------------
            // IContext Implementation
            // --------------------------------
            QString contextString() const;
            QString contextHelpId() const;

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        protected:
            virtual void changeEvent(QEvent *e);
            void constructActions();
            void refreshActions();

        public slots:
            //! Sets the object for which the scope must be shown.
            /*!
              \param obj The object which must be used.
              */
            void setObject(QObject* obj);
            //! Sets the object for which the scope must be shown.
            /*!
              \param obj The object which must be used.
              */
            void setObject(QPointer<QObject> obj);
            //! Sets the object for which the scope must be shown.
            /*!
              Function which allows this widget to be connected to the Qtilities::Core::Interfaces::IObjectManager::metaTypeActiveObjectsChanged() signal.

              \param objects A list of objects. When the list contains 1 item, it will be used in this widget.
              */
            void setObject(QList<QObject*> objects);
            //! Sets the object by providing a list of smart pointers.
            void setObject(QList<QPointer<QObject> > objects);
            //! Refreshes the view.
            void updateContents();
            //! Handles the event where the current object is destroyed.
            void handleObjectDestroyed();

            void handle_observerWidgetSelectionChange(QList<QObject*> objects);
            void handle_actionRemoveContext_triggered();
            void handle_actionDetachToSelection_triggered();
            void handle_currentItemChanged(QTableWidgetItem * current);

        private:
            Ui::ObjectScopeWidget *ui;
            ObjectScopeWidgetPrivateData* d;
        };
    }
}

#endif // OBJECTSCOPEWIDGET_H
