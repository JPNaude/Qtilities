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

#include "BenchmarkTests.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <QDomDocument>

int Qtilities::UnitTests::BenchmarkTests::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::UnitTests::BenchmarkTests::benchmarkObserverExport_1_0_1_0_data() {
    QTest::addColumn<int>("TreeDepth");
    QTest::newRow("small tree") << 5;
    QTest::newRow("big tree") << 50;
}

void Qtilities::UnitTests::BenchmarkTests::benchmarkObserverExport_1_0_1_0() {
    QFETCH(int, TreeDepth);

    // ---------------------------------------------------
    // Test export only with categories:
    // ---------------------------------------------------
    TreeNode* obj_source = new TreeNode("Root Node");

    // Build a tree:
    for (int i = 0; i < TreeDepth; i++) {
        TreeNode* child_node = obj_source->addNode("TestNode" + QString::number(i));
        for (int r = 0; r < TreeDepth; r++) {
            child_node->addItem("TestChild_" + QString::number(i) + "_" + QString::number(r));
        }
    }

    // Do the export:
    QFile file("testObserverDataOnlyWithCategoriesBenchmark_0_3_0_3.xml");
    file.open(QIODevice::WriteOnly);
    QDomDocument doc("QtilitiesTesting");
    QDomElement root = doc.createElement("QtilitiesTesting");
    doc.appendChild(root);
    QDomElement rootItem = doc.createElement("object_node");
    root.appendChild(rootItem);
    obj_source->setExportVersion(Qtilities::Qtilities_1_0);

    QBENCHMARK {
        QVERIFY(obj_source->exportXml(&doc,&rootItem) == IExportable::Complete);
    }

    QString docStr = doc.toString(2);
    file.write(docStr.toAscii());
    file.close();

    // Don't delete it here since deletion will make the test slower. Thus we don't care about the memory leaks.
    // delete obj_source;
}

void Qtilities::UnitTests::BenchmarkTests::benchmarkObserverImport_1_0_1_0() {
    // ---------------------------------------------------
    // Test export only with categories:
    // ---------------------------------------------------
    TreeNode* obj_import_xml = new TreeNode;

    QFile file("testObserverDataOnlyWithCategoriesBenchmark_0_3_0_3.xml");
    file.open(QIODevice::WriteOnly);
    QDomDocument doc("QtilitiesTesting");
    QDomElement root = doc.createElement("QtilitiesTesting");
    doc.appendChild(root);
    QDomElement rootItem = doc.createElement("object_node");
    root.appendChild(rootItem);

    // Do the import:
    obj_import_xml->setExportVersion(Qtilities::Qtilities_1_0);
    QList<QPointer<QObject> > import_list;

    QVERIFY(obj_import_xml->importXml(&doc,&rootItem,import_list) == IExportable::Complete);

    file.close();
    delete obj_import_xml;
}
