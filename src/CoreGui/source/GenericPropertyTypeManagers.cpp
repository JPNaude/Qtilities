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

#include "GenericPropertyTypeManagers.h"
#include "QtilitiesApplication.h"

#ifdef QTILITIES_PROPERTY_BROWSER

namespace Qtilities {
namespace CoreGui {

// --------------------------------
// PathPropertyManager
// --------------------------------

PathPropertyManager::PathPropertyManager(QObject *parent)
    : QtStringPropertyManager(parent)
{
}

PathPropertyManager::~PathPropertyManager() {

}

QString PathPropertyManager::defaultStartPath(QtProperty *property) const {
    Q_UNUSED(property)
    return QtilitiesApplication::applicationSessionPath();
}

QString PathPropertyManager::fileNameFilter(QtProperty *property) const {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property)
            return generic_property->fileNameFilter();
    }
    return QString();
}

QString PathPropertyManager::propertyName(QtProperty *property) const {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property)
            return generic_property->propertyName();
    }
    return QString();
}

bool PathPropertyManager::contextDependent(QtProperty *property) const {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property)
            return generic_property->contextDependent();
    }
    return false;
}

bool PathPropertyManager::editable(QtProperty *property) const {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property)
            return generic_property->editable();
    }
    return true;
}

QString PathPropertyManager::listSeperatorBackend(QtProperty *property) const {
    return listSeparatorBackend(property);
}

QString PathPropertyManager::listSeparatorBackend(QtProperty *property) const {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property)
            return generic_property->listSeparatorBackend();
    }
    return "|";
}

QString PathPropertyManager::value(QtProperty *property) const {
    return valueText(property);
}

QStringList PathPropertyManager::listValues(QtProperty *property) const {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property) {
            if (generic_property->type() == GenericProperty::TypeFileList || generic_property->type() == GenericProperty::TypePathList)
                return generic_property->fileList();
        }
    }
    return QStringList();
}

GenericProperty::PropertyType PathPropertyManager::propertyType(QtProperty *property) const {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property)
            return generic_property->type();
    }
    return GenericProperty::TypeFile;
}

void PathPropertyManager::addPropertyData(QtProperty *property, GenericProperty *generic_property) {
    if (property && generic_property) {
        propertyToData[property] = generic_property;
    }
}

void PathPropertyManager::notifyPropertyChanged(QtProperty *property) {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property) {
            emit propertyChanged(property);
            emit valueChanged(property,generic_property->valueString());
        }
    }
}

QString PathPropertyManager::valueText(const QtProperty *property) const {
    if (propertyToData.contains(property)) {
        QPointer<GenericProperty> generic_property = propertyToData[property];
        if (generic_property) {
            //qDebug() << Q_FUNC_INFO << generic_property->propertyName() << generic_property->valueString();
            return generic_property->valueString();
        }
    }
    Q_ASSERT(0);
    return "";
}

void PathPropertyManager::initializeProperty(QtProperty *property) {
    Q_UNUSED(property)
}

void PathPropertyManager::uninitializeProperty(QtProperty *property) {
    Q_UNUSED(property)
}

// --------------------------------
// FileEditorFactory
// --------------------------------

FileEditorFactory::FileEditorFactory(QObject *parent)
    : QtAbstractEditorFactory<PathPropertyManager>(parent)
{

}

FileEditorFactory::~FileEditorFactory() {
    QList<QPointer<GenericPropertyPathEditor> > editors = editorToProperty.keys();
    foreach (GenericPropertyPathEditor* editor, editors) {
        if (editor)
            delete editor;
    }
}

void FileEditorFactory::connectPropertyManager(PathPropertyManager *manager) {
    addPropertyManager(manager);
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)), this, SLOT(slotValueChanged(QtProperty *, const QString &)));
}

QWidget *FileEditorFactory::createEditor(PathPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    GenericPropertyPathEditor *editor = new GenericPropertyPathEditor(manager->propertyType(property),parent);
    if (!editor)
        return 0;

    editor->setItemFilter(manager->fileNameFilter(property));
    editor->setEditable(!manager->contextDependent(property) && manager->editable(property));
    editor->setDefaultOpenPath(manager->defaultStartPath(property));
    editor->setPropertyName(manager->propertyName(property));
    if (manager->propertyType(property) == GenericProperty::TypeFile || manager->propertyType(property) == GenericProperty::TypePath)
        editor->setText(manager->value(property));
    else if (manager->propertyType(property) == GenericProperty::TypeFileList || manager->propertyType(property) == GenericProperty::TypePathList) {
        editor->setCurrentValues(manager->listValues(property));
        editor->setListSeparatorBackend(manager->listSeparatorBackend(property));
    }

    connect(editor,SIGNAL(valueChanged(QString)),SLOT(slotSetValue(QString)));
    connect(editor,SIGNAL(destroyed(QObject*)),SLOT(slotEditorDestroyed(QObject*)));

    createdEditors[property].append(editor);
    editorToProperty[editor] = property;

    return editor;
}

void FileEditorFactory::disconnectPropertyManager(PathPropertyManager *manager) {
    removePropertyManager(manager);
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)), this, SLOT(slotValueChanged(QtProperty *, const QString &)));
}

void FileEditorFactory::slotValueChanged(QtProperty *property, const QString &value) {
    if (!createdEditors.contains(property))
        return;

    PathPropertyManager *manager = propertyManager(property);
    if (!manager)
        return;

    QList<QPointer<GenericPropertyPathEditor> > editors = createdEditors[property];
    QListIterator<QPointer<GenericPropertyPathEditor> > itEditor(editors);
    while (itEditor.hasNext()) {
        GenericPropertyPathEditor *editor = itEditor.next();
        if (editor)
            editor->setText(value);
    }
}

void FileEditorFactory::slotEditorDestroyed(QObject *object) {
    QMap<QPointer<GenericPropertyPathEditor>, QtProperty *>::ConstIterator itEditor =
                editorToProperty.constBegin();
    while (itEditor != editorToProperty.constEnd()) {
        if (itEditor.key() == object) {
            GenericPropertyPathEditor *editor = itEditor.key();
            QtProperty *property = itEditor.value();
            editorToProperty.remove(editor);
            createdEditors[property].removeAll(editor);
            // TODO: For some reason this slot is not called, so the list contains wrong values.
            // This is however not really a problem since we use QPointers, thus null values in the list does not matter.
            //qDebug() << "Removing deleted editor";
            if (createdEditors[property].isEmpty())
                createdEditors.remove(property);
            return;
        }
        itEditor++;
    }
}

void FileEditorFactory::slotSetValue(const QString &value) {
    QObject *object = sender();
    const QMap<QPointer<GenericPropertyPathEditor>, QtProperty *>::ConstIterator ecend = editorToProperty.constEnd();
    for (QMap<QPointer<GenericPropertyPathEditor>, QtProperty *>::ConstIterator itEditor = editorToProperty.constBegin(); itEditor != ecend; ++itEditor ) {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            PathPropertyManager *manager = propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
    }
}

}
}

#endif
