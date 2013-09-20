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

#ifndef OBJECTPROPERTYBROWSER_H
#define OBJECTPROPERTYBROWSER_H

#ifdef QTILITIES_PROPERTY_BROWSER
#include <QWidget>

#include "QtilitiesCoreGui_global.h"
#include <Factory>

#include <QPointer>

#include <qtpropertybrowser.h>

class QtProperty;

using namespace Qtilities::Core;

namespace Qtilities {
    namespace CoreGui {
        /*!
          \struct Qtilities::CoreGui::ObjectPropertyBrowserPrivateData
          \brief The ObjectPropertyBrowserPrivateData class contains private data which is used by an ObjectPropertyBrowser widget.

          \sa Qtilities::CoreGui::ObjectPropertyBrowser
          */
        struct ObjectPropertyBrowserPrivateData;

        /*!
          \class Qtilities::CoreGui::ObjectPropertyBrowser
          \brief The ObjectPropertyBrowser class provides an interface to the Qt Property Editor solution.

          To view the properties of an object, set the object using the setObject() function and the current object can be obtained using the object() function. It is possible to filter the inherited classes which are shown by the property browser by calling the setFilterList() function. The current filter
          can be obtained using the filterList() function and it can be cleared using the clearFilter() function. It is also possible to create an inversed filter using the setFilterListInversed() function. This is usefull when you want to display all inherited classes expect the ones in the filter list.

          The properties can be displayed in different ways, defined by the BrowserType enumeration.

          The widget embedded in the debug plugin is shown below for an example object.

          \image html debugging_static_properties.jpg "Static Properties Browser"

          \sa ObjectDynamicPropertyBrowser
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObjectPropertyBrowser : public QWidget
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

            ObjectPropertyBrowser(BrowserType browser_type = TreeBrowser, QWidget *parent = 0);
            ~ObjectPropertyBrowser();
            bool eventFilter(QObject *object, QEvent *event);

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QWidget, ObjectPropertyBrowser> factory;    

            //! Sets the object to inspect and display properties for.
            QObject *object() const;

            //! Sets up a filter list. Classes specified in the filter list will not be editable. The internal data structures will be updated automatically to exclude the previously filtered classes.
            /*!
              \note For optimal performance, set your filter list before calling the setObject() function.
              */
            void setFilterList(QStringList filter_list, bool inversed_list = false);
            //! Gets the current filter list.
            QStringList filterList() const;
            //! Clears the current filter list. The internal data structures will be updated automatically to include the previously filtered classes.
            void clearFilter();
            //! Sets the filter list to inverse mode, thus you can filter all classes except a specific set.
            void setFilterListInversed(bool toggle);
            //! Gets the filter list inverse setting. True if the inverse is enabled, false otherwise.
            bool filterListInversed();

            //! Toggle if read only properties should be disabled in the property editor. If not they will be editable, but changes won't be written back to the object.
            void toggleReadOnlyPropertiesDisabled(bool toggle);

            //! Override size hint in order to resize it according to the number of properties displayed.
            QSize sizeHint() const;

            //! Return a reference to the QtAbstractPropertyBrowser property browser used internally.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            QtAbstractPropertyBrowser* abstractPropertyBrowser() const;

        public slots:
            //! Refresh function which checks all properties on the current object and refreshes the property editor.
            /*!
              \sa setObject()
              */
            void refresh(bool has_changes = true);
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

              \param objects A list of objects. When the list contains 1 item, it will be used in this widget.
              \param monitor_changes When true this function will check if the object implements the Qtilities::Core::Interfaces::IModificationNotifier
                interface, and if so it will monitor it for changes. When it changes the property browser will automatically call refresh().
              */
            void setObject(QList<QObject*> objects, bool monitor_changes = true);
            //! Sets the object by providing a list of smart pointers.
            /*!
              \param objects A list of objects. When the list contains 1 item, it will be used in this widget.
              \param monitor_changes When true this function will check if the object implements the Qtilities::Core::Interfaces::IModificationNotifier
                interface, and if so it will monitor it for changes. When it changes the property browser will automatically call refresh().
              */
            void setObject(QList<QPointer<QObject> > objects, bool monitor_changes = true);          

        private slots:
            void handle_property_changed(QtProperty *, const QVariant &);
            void handleObjectDeleted();

        private:
            //! Inspect the meta object of a class to see which properties must be added, then add these properties
            void inspectClass(const QMetaObject *metaObject);
            void refreshClass(const QMetaObject *metaObject, bool recursive);
            int enumToInt(const QMetaEnum &metaEnum, int enumValue) const;
            int intToEnum(const QMetaEnum &metaEnum, int intValue) const;
            int flagToInt(const QMetaEnum &metaEnum, int flagValue) const;
            int intToFlag(const QMetaEnum &metaEnum, int intValue) const;
            bool isSubValue(int value, int subValue) const;
            bool isPowerOf2(int value) const;

            ObjectPropertyBrowserPrivateData* d;

        };
    }
}

#endif // QTILITIES_PROPERTY_BROWSER
#endif // OBJECTPROPERTYBROWSER_H
