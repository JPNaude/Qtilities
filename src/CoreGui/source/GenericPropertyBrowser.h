/****************************************************************************
**
** Copyright 2010-2013, CSIR
** Author: JP Naude, jpnaude@csir.co.za
**
****************************************************************************/

#ifndef GENERIC_PROPERTY_BROWSER_H
#define GENERIC_PROPERTY_BROWSER_H

#ifdef QTILITIES_PROPERTY_BROWSER
#include <QMainWindow>

#include "QtilitiesCoreGui_global.h"
#include "GenericPropertyManager.h"

#include <qtpropertybrowser.h>

#include <QPointer>

class QtProperty;

namespace Qtilities {
    namespace CoreGui {
        /*!
          \struct GenericPropertyBrowserPrivateData
          \brief The GenericPropertyBrowserPrivateData class contains private data which is used by an GenericPropertyBrowser widget.
          */
        struct GenericPropertyBrowserPrivateData;

        /*!
          \class Qtilities::CoreGui::GenericPropertyBrowser
          \brief The GenericPropertyBrowser allows browsing a list of GenericProperty objects.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT GenericPropertyBrowser : public QMainWindow
        {
            Q_OBJECT
            Q_ENUMS(BrowserType)

        public:
            //! This enumeration contains all the possible modes in which the property editor can be used.
            enum BrowserType {
                TreeBrowser,
                GroupBoxBrowser,
                ButtonBrowser
            };

            GenericPropertyBrowser(GenericPropertyManager* property_manager, BrowserType browser_type = TreeBrowser, QWidget *parent = 0);
            ~GenericPropertyBrowser();

            //! Return a reference to the QtAbstractPropertyBrowser property browser used internally.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            QtAbstractPropertyBrowser* abstractPropertyBrowser() const;

            //! Sets the object to inspect and display properties for.
            QObject *object() const;
            //! Reimplement size hint for our case.
            QSize sizeHint() const;
            //! Clears currently displayed properties.
            void clear();

            //! Sets if the property browser is read only.
            virtual void setReadOnly(bool read_only);
            //! Gets if the property browser is read only.
            bool readOnly() const;

        public slots:
            //! Toggles display of advanced settings.
            void toggleAdvancedSettings(bool show);
            //! Toggles display of switch names.
            void toggleSwitchNames(bool show);
            //! Completely refresh the properties in the browser.
            void refresh();

        private slots:
            //! Handles changes to int properties from the display side.
            void handle_intPropertyChanged(QtProperty *property, int value);
            //! Handles changes to double properties from the display side.
            void handle_doublePropertyChanged(QtProperty *property, double value);
            //! Handles changes to string properties from the display side.
            void handle_stringPropertyChanged(QtProperty *property, const QString & value);
            //! Handles changes to enum properties from the display side.
            void handle_enumPropertyChanged(QtProperty *property, int index);
            //! Handles changes to bool properties from the display side.
            void handle_boolPropertyChanged(QtProperty *property, bool value);
            //! Handles changes to file properties from the display side.
            void handle_pathPropertyChanged(QtProperty *property, const QString &value);

            //! Responds to deletion of the build step property manager.
            void handleObjectDeleted();

            //! Function which responds to value changes to displayed properties from the build step side, thus it will update the display.
            void handlePropertyValueChanged(GenericProperty* property);
            //! Function which responds to editable changes to displayed properties from the build step side, thus it will update the display.
            void handlePropertyEditableChanged(GenericProperty *property);
            //! Function which responds to design dependent changes to displayed properties from the build step side, thus it will update the display.
            void handlePropertyContextDependentChanged(GenericProperty *property);
            //! Function which responds to possible value changes to displayed properties from the build step side, thus it will update the display.
            void handlePropertyPossibleValuesChanged(GenericProperty* property);
            //! Function which responds to editable changes to displayed properties from the build step side, thus it will update the display.
            void handlePropertyDefaultValueChanged(GenericProperty *property);
            //! Function which responds to note changes to displayed properties from the build step side, thus it will update the display.
            void handlePropertyNoteChanged(GenericProperty *property);

        protected:
            //! Gets the tooltip text for a property.
            virtual QString getToolTipText(GenericProperty* property);

        private:
            //! Worker function for all property changed slots.
            void updatePropertyValue(QtProperty *property, const QVariant &value);
            //! Inspect the property manager for which properties are displayed.
            void inspectPropertyManager();

            GenericPropertyBrowserPrivateData* d;
        };
    }
}
#endif

#endif // GENERIC_PROPERTY_BROWSER_H
