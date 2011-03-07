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

#include "TestVersionNumber.h"

#include <QtilitiesCore>
using namespace QtilitiesCore;

void Qtilities::UnitTests::TestVersionNumber::testOperatorEqual() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2(5,5,5);
    VersionNumber ver3;
    QVERIFY(ver1 == ver2);
    QVERIFY(!(ver1 == ver3));
}

void Qtilities::UnitTests::TestVersionNumber::testOperatorNotEqual() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2(5,5,5);
    VersionNumber ver3;
    QVERIFY(!(ver1 != ver2));
    QVERIFY(ver1 != ver3);
}

void Qtilities::UnitTests::TestVersionNumber::testOperatorAssign() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2;
    QVERIFY(ver1 != ver2);
    ver2 = ver1;
    QVERIFY(ver1 == ver2);
}

void Qtilities::UnitTests::TestVersionNumber::testOperatorBigger() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2(5,5,5);
    QVERIFY(!(ver2 > ver1));
    VersionNumber ver3(6,5,5);
    QVERIFY(ver3 > ver1);
    VersionNumber ver4(5,6,5);
    QVERIFY(ver4 > ver1);
    VersionNumber ver5(5,5,6);
    QVERIFY(ver5 > ver1);
    VersionNumber ver6(4,5,5);
    QVERIFY(!(ver6 > ver1));

    VersionNumber ver7(6,5,5);
    ver7.setIsVersionRevisionUsed(false);
    QVERIFY(ver7 > ver1);
    VersionNumber ver8(5,6,5);
    ver8.setIsVersionRevisionUsed(false);
    QVERIFY(ver8 > ver1);
    VersionNumber ver9(5,5,6);
    ver9.setIsVersionRevisionUsed(false);
    QVERIFY(!(ver9 > ver1));

    VersionNumber ver10(6,5,5);
    ver10.setIsVersionRevisionUsed(false);
    ver10.setIsVersionMinorUsed(false);
    QVERIFY(ver10 > ver1);
    VersionNumber ver11(5,6,5);
    ver11.setIsVersionRevisionUsed(false);
    ver11.setIsVersionMinorUsed(false);
    QVERIFY(!(ver11 > ver1));
    VersionNumber ver12(5,5,6);
    ver12.setIsVersionRevisionUsed(false);
    ver12.setIsVersionMinorUsed(false);
    QVERIFY(!(ver12 > ver1));
}

void Qtilities::UnitTests::TestVersionNumber::testOperatorBiggerEqual() {
    VersionNumber ver0(4,4,4);
    VersionNumber ver1(5,5,5);
    QVERIFY(!(ver0 >= ver1));
    VersionNumber ver2(5,5,5);
    QVERIFY(ver2 >= ver1);
    VersionNumber ver3(6,5,5);
    QVERIFY(ver3 >= ver1);
    VersionNumber ver4(5,6,5);
    QVERIFY(ver4 >= ver1);
    VersionNumber ver5(5,5,6);
    QVERIFY(ver5 >= ver1);
    VersionNumber ver6(4,5,5);
    QVERIFY(!(ver6 >= ver1));
}

void Qtilities::UnitTests::TestVersionNumber::testOperatorSmaller() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2(5,5,5);
    QVERIFY(!(ver2 < ver1));
    VersionNumber ver3(5,5,4);
    QVERIFY(ver3 < ver1);
    VersionNumber ver4(5,4,5);
    QVERIFY(ver4 < ver1);
    VersionNumber ver5(4,5,5);
    QVERIFY(ver5 < ver1);

    VersionNumber ver7(5,5,4);
    ver7.setIsVersionRevisionUsed(false);
    QVERIFY(!(ver7 < ver1));
    VersionNumber ver8(5,4,5);
    ver8.setIsVersionRevisionUsed(false);
    QVERIFY(ver8 < ver1);
    VersionNumber ver9(4,5,5);
    ver9.setIsVersionRevisionUsed(false);
    QVERIFY(ver9 < ver1);

    VersionNumber ver10(5,5,4);
    ver10.setIsVersionRevisionUsed(false);
    ver10.setIsVersionMinorUsed(false);
    QVERIFY(!(ver10 < ver1));
    VersionNumber ver11(5,4,5);
    ver11.setIsVersionRevisionUsed(false);
    ver11.setIsVersionMinorUsed(false);
    QVERIFY(!(ver11 < ver1));
    VersionNumber ver12(4,5,5);
    ver12.setIsVersionRevisionUsed(false);
    ver12.setIsVersionMinorUsed(false);
    QVERIFY(ver12 < ver1);
}

void Qtilities::UnitTests::TestVersionNumber::testOperatorSmallerEqual() {
    VersionNumber ver0(6,6,6);
    VersionNumber ver1(5,5,5);
    QVERIFY(!(ver0 <= ver1));
    VersionNumber ver2(5,5,5);
    QVERIFY(ver2 <= ver1);
    VersionNumber ver3(5,5,4);
    QVERIFY(ver3 <= ver1);
    VersionNumber ver4(5,4,5);
    QVERIFY(ver4 <= ver1);
    VersionNumber ver5(4,5,5);
    QVERIFY(ver5 <= ver1);
}

void Qtilities::UnitTests::TestVersionNumber::testToString() {
    VersionNumber ver(1,2,3);
    QVERIFY(ver.toString().compare("1.2.3") == 0);
    ver.setFieldWidthMinor(3);
    qDebug() << ver.toString();
    QVERIFY(ver.toString().compare("1.002.3") == 0);
    ver.setFieldWidthRevision(3);
    QVERIFY(ver.toString().compare("1.002.003") == 0);

    ver.setIsVersionRevisionUsed(false);
    QVERIFY(ver.toString().compare("1.002") == 0);
    ver.setIsVersionMinorUsed(false);
    QVERIFY(ver.toString().compare("1") == 0);

    VersionNumber ver1(1,12,103);
    QVERIFY(ver1.toString().compare("1.12.103") == 0);
    ver1.setFieldWidthMinor(3);
    QVERIFY(ver1.toString().compare("1.012.103") == 0);
    ver1.setFieldWidthRevision(3);
    QVERIFY(ver1.toString().compare("1.012.103") == 0);

    ver1.setIsVersionRevisionUsed(false);
    QVERIFY(ver1.toString().compare("1.012") == 0);
    ver1.setIsVersionMinorUsed(false);
    QVERIFY(ver1.toString().compare("1") == 0);
}

