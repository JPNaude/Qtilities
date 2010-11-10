/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include "ObserverDotGraph.h"
#include "ObserverRelationalTable.h"

struct Qtilities::Core::ObserverDotGraphData {
    ObserverDotGraphData() {}


};

Qtilities::Core::ObserverDotGraph::ObserverDotGraph(Observer* observer) : QObject(observer), ObserverAwareBase() {
    d = new ObserverDotGraphData;

    setObserverContext(observer);

}

Qtilities::Core::ObserverDotGraph::~ObserverDotGraph() {
    delete d;
}

Qtilities::Core::ObserverDotGraph::ObserverDotGraph(const ObserverDotGraph& other) : QObject(other.parent()), ObserverAwareBase() {
    d = new ObserverDotGraphData;

}

void Qtilities::Core::ObserverDotGraph::operator=(const ObserverDotGraph& other) {
    d_observer = other.d_observer;
}

bool Qtilities::Core::ObserverDotGraph::saveToFile(const QString& fileName) const {
    if (!d_observer)
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;

    file.write(generateDotScript().toLocal8Bit());
    file.close();

    return true;
}

QString Qtilities::Core::ObserverDotGraph::generateDotScript() const {
    if (!d_observer)
        return QString();

    // Create the dot string:
    QString dotString = QString("digraph \"%1\" {\n").arg(d_observer->observerName());

    // Then do the relationships between items:
    ObserverRelationalTable table(d_observer);
    for (int i = 0; i < table.count(); i++) {
        RelationalTableEntry* entry = table.entryAt(i);

        // Label this entry:
        QString entry_label = QString("    %1 [label=\"%2\"];\n").arg(entry->d_visitorID).arg(entry->d_name);
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

