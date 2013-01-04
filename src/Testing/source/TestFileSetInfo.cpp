/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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
    fsi.addFile(QApplication::applicationDirPath() + "/test_binary.bin");
    QVERIFY(fsi.fileSetHash() == 210687302);
}
