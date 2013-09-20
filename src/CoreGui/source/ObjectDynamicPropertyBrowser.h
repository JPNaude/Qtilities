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

#ifndef ObjectDynamicPropertyBrowser_H
#define ObjectDynamicPropertyBrowser_H

#ifdef QTILITIES_PROPERTY_BROWSER
#include <QMainWindow>

#include "QtilitiesCoreGui_global.h"
#include <Factory>
#include <ObjectManager>

#include <QPointer>

#include <qtpropertybrowser.h>

class QtProperty;

using namespace Qtilities::Core;

namespace Qtilities {
    namespace CoreGui {
        /*!
          \struct Qtilities::CoreGui::ObjectDynamicPropertyBrowserPrivateData
          \brief The ObjectDynamicPropertyBrowserPrivateData class contains private data which is used by an ObjectDynamicPropertyBrowser widget.

          \sa Qtilities::CoreGui::ObjectDynamicPropertyBrowser
          */
        struct ObjectDynamicPropertyBrowserPrivateData;
        struct qti_private_MultiContextPropertyData;

        /*!
          \class Qtilities::CoreGui::ObjectDynamicPropertyBrowser
          \brief The ObjectDynamicPropertyBrowser class provides an interface to the Qt Property Editor solution.

          To view the properties of an object, set the object using the setObject() function and the current object can be obtained using the object() function. The properties can be displayed in different ways, defined by the BrowserType enumeration.

            This widget allows you to display the following type of properties:
            - Normal QVariant QObject properties set using setProperty() on any QObject.
            - Qtilities::Core::SharedProperty properties.
            - Qtilities::Core::MultiContextProperty properties. Note that the property values for all context are displayed as shown below.

          The widget embedded in the debug plugin is shown below for an example object.

          \image html debugging_dynamic_properties.jpg "Dynamic Properties Browser"

          \sa Qtilities::Core::ObjectPropertyBrowser

          <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObjectDynamicPropertyBrowser : public QMainWindow
        {
            Q_OBJECT          
            Q_ENUMS(BrowserType)

        public:
            //! This enumeration contains all the possible modes in which the property editor can be used.
            enum BrowserType {
                TreeBrowser,        /*!< Object's inherited classes and their properties are displayed in a tree.
                                    \image html property_editor_tree_browser.jpg "Property Browser (Tree Browser Mode)"
                                    \image latex property_editor_tree_browser.eps "Property Browser (Tree Browser Mode)" width=3in
                                    */
                GroupBoxBrowser,    /*!< Object's inherited classes and their properties are displayed in group boxes.
                                    \image html property_editor_groupbox_browser.jpg "Property Browser (Groupbox Browser Mode)"
                                    \image latex property_editor_groupbox_browser.eps "Property Browser (Groupbox Browser Mode)" width=3in
                                    */
                ButtonBrowser       /*!< Object's inherited classes and their properties are displayed in the form of buttons.
                                    \image html property_editor_button_browser.jpg "Property Browser (Button Browser Mode)"
                                    \image latex property_editor_button_browser.eps "Property Browser (Button Browser Mode)" width=3in
                                    */
            };

            ObjectDynamicPropertyBrowser(BrowserType browser_type = TreeBrowser, bool show_toolbar = true, Qt::ToolBarArea area = Qt::TopToolBarArea, QWidget *parent = 0);
            ~ObjectDynamicPropertyBrowser();
            bool eventFilter(QObject *object, QEvent *event);

            //! Toggles if Qtilities properties are shown.
            void toggleQtilitiesProperties(bool show_qtilities_properties);

            //! Sets if the property editor is read only.
            /*!
             *<i>This function was added in %Qtilities v1.2.</i>
             */
            void setReadOnly(bool read_only);
            //! Gets if the property editor is read only.
            /*!
             *<i>This function was added in %Qtilities v1.2.</i>
             */
            bool readOnly() const;

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QWidget, ObjectDynamicPropertyBrowser> factory;       

            //! Sets the object to inspect and display properties for.
            QObject *object() const;
            //! Reimplement size hint for our case.
            QSize sizeHint() const;
            //! Clears currently displayed properties.
            void clear();
            //! Sets what property type must be used when the user adds new properties.
            /*!
              \note Only ObjectManager::SharedProperties and ObjectManager::NonQtilitiesProperties types are allowed, when trying to set it to something else this function does nothing.
              */
            void setNewPropertyType(ObjectManager::PropertyTypes new_property_type);
            //! Gets what property type must be used when the user adds new properties.
            /*!
              \returns The property type used when the user adds new properties. Default is normal QVariants, thus ObjectManager::NonQtilitiesProperties.
              */
            ObjectManager::PropertyTypes newPropertyType() const;
            //! Function which toggle the visibility of the toolbar with add/remove actions etc. on the property browser.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             *
             * \sa isToolBarVisible()
             */
            void toggleToolBar();
            //! Function which checks if the toolbar is currently visible.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             *
             * \sa toggleToolBar()
             */
            bool isToolBarVisible() const;

            //! Return a reference to the QtAbstractPropertyBrowser property browser used internally.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            QtAbstractPropertyBrowser* abstractPropertyBrowser() const;

        public slots:
            //! Refresh function which gets all properties on the current object and refreshes the property editor.
            /*!
              \sa setObject()
              */
            void refresh();
            //! Sets the object for which the properties must be shown.
            /*!
              \param obj The object which must be used.
              \param monitor_changes When true this function will check if the object implements the Qtilities::Core::Interfaces::IModificationNotifier
                interface, and if so it will monitor it for changes. When it changes the property browser will automatically call refresh().
              */
            void setObject(QObject* obj, bool monitor_changes = true);
            //! Sets the object for which the properties must be shown.
            /*!
              \param obj The object which must be used.
              \param monitor_changes When true this function will check if the object implements the Qtilities::Core::Interfaces::IModificationNotifier
                interface, and if so it will monitor it for changes. When it changes the property browser will automatically call refresh().
              */
            void setObject(QPointer<QObject> obj, bool monitor_changes = true);
            //! Sets the object for which the properties must be shown.
            /*!
              Function which allows this widget to be connected to the Qtilities::Core::Interfaces::IObjectManager::metaTypeActiveObjectsChanged() signal.

              \param objects A list of objects. When the list contains 1 item, it will be used in this widget, if it contains more than 1 this function does nothing.
              \param monitor_changes When true this function will check if the object implements the Qtilities::Core::Interfaces::IModificationNotifier
                interface, and if so it will monitor it for changes. When it changes the property browser will automatically call refresh().
              */
            void setObject(QList<QObject*> objects, bool monitor_changes = true);
            //! Sets the object by providing a list of smart pointers.
            /*!
              \param objects A list of objects. When the list contains 1 item, it will be used in this widget, if it contains more than 1 this function does nothing.
              \param monitor_changes When true this function will check if the object implements the Qtilities::Core::Interfaces::IModificationNotifier
                interface, and if so it will monitor it for changes. When it changes the property browser will automatically call refresh().
              */
            void setObject(QList<QPointer<QObject> > objects, bool monitor_changes = true);

        private slots:
            void propertyChangedFromBrowserSide(QtProperty *, const QVariant &);
            void propertyChangedFromObjectSide(bool modified = true);
            void handleObjectDeleted();
            void handleAddProperty();
            void handleRemoveProperty();

        signals:
            void propertyAdded(const QString& property_name);
            void propertyRemoved(const QString& property_name);

        private:
            //! Inspect the dynamic properties of an object and add these properties to the property browser.
            void inspectObject(const QObject* obj);

            ObjectDynamicPropertyBrowserPrivateData* d;
        };
    }
}

#endif // QTILITIES_PROPERTY_BROWSER
#endif // ObjectDynamicPropertyBrowser_H
