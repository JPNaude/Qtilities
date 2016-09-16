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

#ifndef GENERIC_PROPERTY_MANAGERS_H
#define GENERIC_PROPERTY_MANAGERS_H

#ifdef QTILITIES_PROPERTY_BROWSER
#include "QtilitiesCoreGui_global.h"
#include "GenericProperty.h"
#include "GenericPropertyPathEditor.h"

#include <qtpropertymanager.h>
#include <QtLineEditFactory>

#include <QMap>

namespace Qtilities {
    namespace CoreGui {
        // --------------------------------
        // PathPropertyManager
        // --------------------------------
        class PathPropertyManager : public QtStringPropertyManager
        {
            Q_OBJECT
        public:
            PathPropertyManager(QObject *parent = 0);
            ~PathPropertyManager();

            //! Gets the default start path for a property, if possible.
            /*!
              \section Determining the default start path for file based properties:

              By default, retuns QtilitiesApplication::applicationSessionPath().

              \section Determining the default start path for path based properties:

              By default, retuns QtilitiesApplication::applicationSessionPath().
              */
            virtual QString defaultStartPath(QtProperty* property) const;
            QString fileNameFilter(QtProperty* property) const;
            QString propertyName(QtProperty* property) const;
            bool contextDependent(QtProperty* property) const;
            bool editable(QtProperty* property) const;
            QString listSeparatorBackend(QtProperty* property) const;
            Q_DECL_DEPRECATED QString listSeperatorBackend(QtProperty* property) const;
            //! Returns the current list values of the property. Only applicable to FileList and PathList properties.
            QStringList listValues(QtProperty* property) const;
            GenericProperty::PropertyType propertyType(QtProperty* property) const;
            void addPropertyData(QtProperty* property, GenericProperty* build_step_property);
            void notifyPropertyChanged(QtProperty* property);
            QString value(QtProperty *property) const;

        protected:
            QString valueText(const QtProperty *property) const;
            virtual void initializeProperty(QtProperty *property);
            virtual void uninitializeProperty(QtProperty *property);

        private:
            QMap<const QtProperty *, QPointer<GenericProperty> > propertyToData;
        };

        // --------------------------------
        // FileEditorFactory
        // --------------------------------
        class FileEditorFactory : public QtAbstractEditorFactory<PathPropertyManager>
        {
            Q_OBJECT
        public:
            FileEditorFactory(QObject *parent = 0);
            ~FileEditorFactory();
        protected:
            void connectPropertyManager(PathPropertyManager *manager);
            QWidget *createEditor(PathPropertyManager *manager, QtProperty *property,QWidget *parent);
            void disconnectPropertyManager(PathPropertyManager *manager);
        private slots:
            void slotValueChanged(QtProperty *property, const QString &value);
            void slotEditorDestroyed(QObject *object);
            void slotSetValue(const QString& value);
        private:
            QMap<QtProperty *, QList<QPointer<GenericPropertyPathEditor> > > createdEditors;
            QMap<QPointer<GenericPropertyPathEditor>, QtProperty *> editorToProperty;
        };
    }
}

#endif // QTILITIES_PROPERTY_BROWSER
#endif // GENERIC_PROPERTY_MANAGERS_H
