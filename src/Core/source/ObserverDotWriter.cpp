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

#include "ObserverDotWriter.h"
#include "ObserverRelationalTable.h"
#include "Observer.h"
#include "ObserverProperty.h"

struct Qtilities::Core::ObserverDotWriterPrivateData {
    ObserverDotWriterPrivateData() : observer(0) {}

    Observer*                       observer;
    ObserverDotWriter::GraphType     graph_type;
    QHash<QString,QString>          graph_attributes;
};

Qtilities::Core::ObserverDotWriter::ObserverDotWriter(Observer* observer, GraphType graph_type) : QObject(observer) {
    d = new ObserverDotWriterPrivateData;
    d->observer = observer;
    d->graph_type = graph_type;
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

Qtilities::Core::ObserverDotWriter::GraphType Qtilities::Core::ObserverDotWriter::type() const {
    return d->graph_type;
}

void Qtilities::Core::ObserverDotWriter::setType(GraphType type) {
    d->graph_type = type;
}

Qtilities::Core::ObserverDotWriter::ObserverDotWriter(const ObserverDotWriter& other) : QObject(other.parent()) {
    d = new ObserverDotWriterPrivateData;
    d->observer = other.observerContext();
    d->graph_type = other.type();
}

void Qtilities::Core::ObserverDotWriter::operator=(const ObserverDotWriter& other) {
    d->observer = other.observerContext();
}

bool Qtilities::Core::ObserverDotWriter::saveToFile(const QString& fileName) const {
    if (!d->observer)
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;

    file.write(generateDotScript().toLocal8Bit());
    file.close();

    return true;
}

QString Qtilities::Core::ObserverDotWriter::generateDotScript() const {
    if (!d->observer)
        return QString();

    // Create the dot string:
    QString dotString = QString("digraph \"%1\" {\n").arg(d->observer->observerName());

    // Add graph attributes:

    // Then do the relationships between items:
    ObserverRelationalTable table(d->observer);
    for (int i = 0; i < table.count(); i++) {
        RelationalTableEntry* entry = table.entryAt(i);

        // Label this entry:
        QString entry_label = QString("    %1 [label=\"%2\"").arg(entry->d_visitorID).arg(entry->d_name);
        // Add properties to it (node attributes):
        if (entry->d_obj) {
            QList<QByteArray> property_names = entry->d_obj->dynamicPropertyNames();
            for (int p = 0; p < property_names.count(); p++) {
                QString property_name(property_names.at(p).data());
                if (property_name.startsWith("qti.dot.node")) {
                    // This is a dot note attribute property:
                    QByteArray ba = property_name.remove(0,13).toAscii();
                    const char* char_name = ba.data();
                    // Get the shared property:
                    SharedObserverProperty shared_property = Observer::getSharedProperty(entry->d_obj,property_names.at(p).data());
                    if (shared_property.isValid()) {
                        entry_label.append(" " + QString(char_name) + "=" + shared_property.value().toString());
                    }
                }
            }
        }
        entry_label.append("];\n");
        dotString.append(entry_label);

        // Now fill in the relationship data:
        for (int c = 0; c < entry->d_children.count(); c++) {
            QString relationship_string = QString("    %1").arg(entry->d_visitorID);
            relationship_string.append(" -> ");
            relationship_string.append(QString("%1").arg(table.entryWithVisitorID(entry->d_children.at(c))->d_visitorID));
            dotString.append(relationship_string);
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
    QByteArray byte_array_name = string_name.toAscii();
    const char* char_name = byte_array_name.data();
    SharedObserverProperty shared_property(value,char_name);
    return Observer::setSharedProperty(node,shared_property);
}

bool Qtilities::Core::ObserverDotWriter::removeNodeAttribute(QObject* node, const QString& attribute) {
    if (!d->observer)
        return false;

    if (!node)
        return false;

    QList<QObject*> tree_items = d->observer->treeChildren();
    if (!tree_items.contains(node))
        return false;

    //return !node->setProperty("qti.dot.node." + attribute,QVariant());
}

QHash<QString,QString> Qtilities::Core::ObserverDotWriter::nodeAttributes(QObject* node) const {
    if (!d->observer)
        return QHash<QString,QString>();

    return QHash<QString,QString>();
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
    QByteArray byte_array_name = string_name.toAscii();
    const char* char_name = byte_array_name.data();

    if (Observer::propertyExists(child,char_name)) {
        ObserverProperty existing_property = Observer::getObserverProperty(child,char_name);
        existing_property.setValue(value,parent->observerID());
        return Observer::setObserverProperty(child,existing_property);
    } else {
        ObserverProperty new_property(char_name);
        new_property.setValue(value,parent->observerID());
        return Observer::setObserverProperty(child,new_property);
    }
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

    return true;
}

QHash<QString,QString> Qtilities::Core::ObserverDotWriter::edgeAttributes(Observer* parent, QObject* child) const {
    if (!d->observer)
        return QHash<QString,QString>();

    return QHash<QString,QString>();
}

bool Qtilities::Core::ObserverDotWriter::addGraphAttribute(const QString& attribute, const QString& value) {
    if (d->graph_attributes.keys().contains("qti.dot.graph." + attribute))
        d->graph_attributes.remove(attribute);

    d->graph_attributes[attribute] = value;
    return true;
}

bool Qtilities::Core::ObserverDotWriter::removeGraphAttribute(const QString& attribute) {
    if (d->graph_attributes.keys().contains("qti.dot.graph." + attribute))
        d->graph_attributes.remove(attribute);
    return true;
}

QHash<QString,QString> Qtilities::Core::ObserverDotWriter::graphAttributes() const {
    return d->graph_attributes;
}

