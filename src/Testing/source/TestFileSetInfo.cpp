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

#include "TestFileSetInfo.h"

#include <QtilitiesCore>
using namespace QtilitiesCore;

int Qtilities::Testing::TestFileSetInfo::execTest(int argc, char ** argv) {
    // Before executing the test, we write some example files to disk:
    // TEXT FILE:
    QFile file_text("test_text.txt");
    if (file_text.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file_text);
        out << "The magic number is: " << 49 << "\n";
    }
    file_text.close();

    // BINARY FILE:
    QFile file_binary("test_binary.bin");
    if (file_binary.open(QIODevice::WriteOnly)) {
        QDataStream out(&file_binary);
        out << "The magic number is: " << 49 << "\n";
    }
    file_binary.close();

    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestFileSetInfo::testGetHash() {
    FileSetInfo fsi;
    fsi.addFile(QApplication::applicationDirPath() + "/test_text.txt");

    int hash1 = fsi.fileSetHash();
    fsi.addFile(QApplication::applicationDirPath() + "/test_binary.bin");

    int hash2 = fsi.fileSetHash();
    QVERIFY(hash1 != hash2);
}
