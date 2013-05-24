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

#include "ObserverDotWriter.h"
#include "ObserverRelationalTable.h"
#include "Observer.h"
#include "QtilitiesProperty.h"

struct Qtilities::Core::ObserverDotWriterPrivateData {
    ObserverDotWriterPrivateData() : observer(0) {}

    Observer*                       observer;
    // Key = attribue, Value = value
    QHash<QString,QString>          graph_attributes;
};

Qtilities::Core::ObserverDotWriter::ObserverDotWriter(Observer* observer) : QObject(observer) {
    d = new ObserverDotWriterPrivateData;
    d->observer = observer;
}

Qtilities::Core::ObserverDotWriter::~ObserverDotWriter() {
    delete d;
}

bool Qtilities::Core::ObserverDotWriter::setObserverContext(Observer* observer) {
    if (observer) {
        d->observer = observer;
        return true;
    } else {
        return false;
    }
}

Qtilities::Core::Observer* Qtilities::Core::ObserverDotWriter::observerContext() const {
    return d->observer;
}

Qtilities::Core::ObserverDotWriter::ObserverDotWriter(const ObserverDotWriter& other) : QObject(other.parent()) {
    d = new ObserverDotWriterPrivateData;
    d->observer = other.observerContext();
}

ObserverDotWriter& Qtilities::Core::ObserverDotWriter::operator=(const ObserverDotWriter& other) {
    if (this==&other) return *this;

    d->observer = other.observerContext();

    return *this;
}

bool Qtilities::Core::ObserverDotWriter::saveToFile(const QString& fileName) const {
    if (!d->observer)
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;

    file.write(generateDotScript().toUtf8());
    file.close();

    return true;
}

QString Qtilities::Core::ObserverDotWriter::generateDotScript() const {
    if (!d->observer)
        return QString();

    // Create the dot string:
    QString dotString;
    dotString.append(QString("digraph \"%1\" {\n").arg(d->observer->observerName()));

    // Add graph attributes:
    for (int i = 0; i < d->graph_attributes.count(); ++i) {
        dotString.append("    ");
        dotString.append(d->graph_attributes.keys().at(i));
        dotString.append(" = \"");
        dotString.append(d->graph_attributes.values().at(i));
        dotString.append("\";\n");
    }

    // Then do the relationships between items:
    ObserverRelationalTable table(d->observer);
    for (int i = 0; i < table.count(); ++i) {
        RelationalTableEntry* entry = table.entryAt(i);

        // Label this entry:
        QString entry_label = QString("    %1 [label=\"%2\"").arg(entry->visitorID()).arg(entry->name());
        // Add properties to it (node attributes):
        if (entry->object()) {
            QList<QByteArray> property_names = entry->object()->dynamicPropertyNames();
            for (int p = 0; p < property_names.count(); p++) {
                QString property_name(property_names.at(p).data());
                if (property_name.startsWith("qti.dot.node")) {
                    // This is a dot node attribute property:
                    QByteArray ba = property_name.remove(0,13).toUtf8();
                    const char* char_name = ba.data();
                    // Get the shared property:
                    SharedProperty shared_property = ObjectManager::getSharedProperty(entry->object(),property_names.at(p).data());
                    if (shared_property.isValid()) {
                        entry_label.append(" " + QString(char_name) + "=" + shared_property.value().toString());
                    }
                }
            }
        }
        entry_label.append("];\n");
        dotString.append(entry_label);

        // Now fill in the relationship data:
        for (int c = 0; c < entry->children().count(); c++) {
            QString relationship_string = QString("    %1").arg(entry->visitorID());
            relationship_string.append(" -> ");
            RelationalTableEntry* child_entry = table.entryWithVisitorID(entry->children().at(c));
            relationship_string.append(QString("%1").arg(child_entry->visitorID()));
            dotString.append(relationship_string);

            QMap<QString,QString> edge_attributes;
            // Get edge attributes for this relationship:
            QList<QByteArray> property_names = child_entry->object()->dynamicPropertyNames();
            for (int p = 0; p < property_names.count(); p++) {
                QString property_name(property_names.at(p).data());
                if (property_name.startsWith("qti.dot.edge")) {
                    // This is a dot note attribute property:
                    QByteArray ba = property_name.remove(0,13).toUtf8();
                    const char* char_name = ba.data();
                    // Get the shared property:
                    MultiContextProperty multi_context_property = ObjectManager::getMultiContextProperty(child_entry->object(),property_names.at(p).data());
                    if (multi_context_property.isValid()) {
                        // Now check if this multi_context_property has a value for our context:
                        if (multi_context_property.hasContext(entry->sessionID())) {
                            edge_attributes[QString(char_name)] = multi_context_property.value(entry->sessionID()).toString();
                        }
                    }
                }
            }

            // Now add all found attributes for this edge:
            for (int e = 0; e < edge_attributes.count(); e++) {
                if (e == 0)
                    dotString.append(" [");
                dotString.append(edge_attributes.keys().at(e) + "=" + edge_attributes.values().at(e));
                if (edge_attributes.count() > 1 && e != edge_attributes.count()-1)
                    dotString.append(",");
                if (e == edge_attributes.count()-1)
                    dotString.append("]");
            }

            // Finally add the new end line character and the new line:
            dotString.append(";\n");
        }
    }

    // Append the closing } character:
    dotString.append("}");
    LOG_INFO(dotString);

    return dotString;
}

bool Qtilities::Core::ObserverDotWriter::addNodeAttribute(QObject* node, const QString& attribute, const QString& value) {
    if (!d->observer)
        return false;

    if (!node)
        return false;

    QList<QObject*> tree_items = d->observer->treeChildren();
    if (!tree_items.contains(node))
        return false;

    QString string_name = "qti.dot.node." + attribute;
    QByteArray byte_array_name = string_name.toUtf8();
    const char* char_name = byte_array_name.data();
    SharedProperty shared_property(char_name,value);

    if (ObjectManager::setSharedProperty(node,shared_property)) {
        // Set the modification state of the object:
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (node);
        if (mod_iface)
            mod_iface->setModificationState(true);
        return true;
    } else
        return false;
}

bool Qtilities::Core::ObserverDotWriter::removeNodeAttribute(QObject* node, const QString& attribute) {
    if (!d->observer)
        return false;

    if (!node)
        return false;

    QList<QObject*> tree_items = d->observer->treeChildren();
    if (!tree_items.contains(node))
        return false;

    QString prop_name("qti.dot.node." + attribute);
    QByteArray byteArray = prop_name.toUtf8();
    const char * char_name = byteArray.data();
    if (!ObjectManager::propertyExists(node,char_name))
        return false;

    if (!node->setProperty(char_name,QVariant())) {
        // Set the modification state of the object:
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (node);
        if (mod_iface)
            mod_iface->setModificationState(true,IModificationNotifier::NotifyListeners,true);
        return true;
    } else
        return false;
}

QHash<QByteArray,QString> Qtilities::Core::ObserverDotWriter::nodeAttributes(QObject* node) const {
    if (!d->observer)
        return QHash<QByteArray,QString>();

    if (!node)
        return QHash<QByteArray,QString>();

    QList<QByteArray> property_names = node->dynamicPropertyNames();
    QHash<QByteArray,QString> dot_properties;
    for (int i = 0; i < property_names.count(); ++i) {
        QString prop_qstring_name = property_names.at(i);
        if (prop_qstring_name.startsWith("qti.dot.node")) {
            // Get the property value:
            SharedProperty prop = ObjectManager::getSharedProperty(node,property_names.at(i));
            if (prop.isValid())
                dot_properties[property_names.at(i)] = prop.value().toString();
        }
    }

    return dot_properties;
}

bool Qtilities::Core::ObserverDotWriter::addEdgeAttribute(Observer* parent, QObject* child, const QString& attribute, const QString& value) {
    if (!d->observer)
        return false;

    if (!parent || !child)
        return false;

    QList<QObject*> tree_items = d->observer->treeChildren();
    if (parent == d->observer) {
        if (!tree_items.contains(child))
            return false;
    } else {
        if (!tree_items.contains(parent) || !tree_items.contains(child))
            return false;
    }

    QString string_name = "qti.dot.edge." + attribute;
    QByteArray byte_array_name = string_name.toUtf8();
    const char* char_name = byte_array_name.data();

    bool success = false;
    if (ObjectManager::propertyExists(child,char_name)) {
        MultiContextProperty existing_property = ObjectManager::getMultiContextProperty(child,char_name);
        if (existing_property.isValid()) {
            existing_property.setValue(value,parent->observerID());
            success = ObjectManager::setMultiContextProperty(child,existing_property);
        } else
            return false;
    } else {
        MultiContextProperty new_property(char_name);
        new_property.setValue(value,parent->observerID());
        success = ObjectManager::setMultiContextProperty(child,new_property);
    }

    if (success) {
        // Set the modification state of the object:
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (child);
        if (mod_iface)
            mod_iface->setModificationState(true,IModificationNotifier::NotifyListeners,true);
    }

    return success;
}

bool Qtilities::Core::ObserverDotWriter::removeEdgeAttribute(Observer* parent, QObject* child, const QString& attribute) {
    if (!d->observer)
        return false;

    if (!parent || !child)
        return false;


    QList<QObject*> tree_items = d->observer->treeChildren();
    if (parent == d->observer) {
        if (!tree_items.contains(child))
            return false;
    } else {
        if (!tree_items.contains(parent) || !tree_items.contains(child))
            return false;
    }

    // Get the specified MultiContext property:
    QString prop_name("qti.dot.edge." + attribute);
    QByteArray byteArray = prop_name.toUtf8();
    const char * char_name = byteArray.data();
    MultiContextProperty property = ObjectManager::getMultiContextProperty(child,char_name);
    if (property.isValid()) {
        // Check if the property exists:
        if (!property.hasContext(parent->observerID()))
            return false;

        // Now remove the property related to the parent:
        property.removeContext(parent->observerID());

        // Set the property again:
        if (property.contextMap().keys().count() == 0)
            child->setProperty(char_name,QVariant());
        else
            ObjectManager::setMultiContextProperty(child,property);

        // Set the modification state of the object:
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (child);
        if (mod_iface)
            mod_iface->setModificationState(true,IModificationNotifier::NotifyListeners,true);

        return true;
    }

    return false;
}

QHash<QByteArray,QString> Qtilities::Core::ObserverDotWriter::edgeAttributes(Observer* parent, QObject* child) const {
    if (!d->observer)
        return QHash<QByteArray,QString>();

    if (!parent || !child)
        return QHash<QByteArray,QString>();

    QList<QByteArray> property_names = child->dynamicPropertyNames();
    QHash<QByteArray,QString> dot_properties;
    for (int i = 0; i < property_names.count(); ++i) {
        QString prop_qstring_name = property_names.at(i);
        if (prop_qstring_name.startsWith("qti.dot.edge")) {
            // Get the property value:
            MultiContextProperty prop = ObjectManager::getMultiContextProperty(child,property_names.at(i));
            if (prop.isValid()) {
                if (prop.hasContext(parent->observerID()))
                    dot_properties[property_names.at(i)] = prop.value(parent->observerID()).toString();
            }
        }
    }

    return dot_properties;
}

bool Qtilities::Core::ObserverDotWriter::addGraphAttribute(const QString& attribute, const QString& value) {
    if (d->graph_attributes.contains("qti.dot.graph." + attribute))
        d->graph_attributes.remove(attribute);

    d->graph_attributes[attribute] = value;
    return true;
}

bool Qtilities::Core::ObserverDotWriter::removeGraphAttribute(const QString& attribute) {
    if (d->graph_attributes.contains("qti.dot.graph." + attribute))
        d->graph_attributes.remove(attribute);
    return true;
}

QHash<QString,QString> Qtilities::Core::ObserverDotWriter::graphAttributes() const {
    return d->graph_attributes;
}

