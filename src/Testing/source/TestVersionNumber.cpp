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

#include "TestVersionNumber.h"

#include <QtilitiesCore>
using namespace QtilitiesCore;

// The boolean expressions in this test look a little weird because we're
// ensuring that their overloads actually work as expected.
int Qtilities::Testing::TestVersionNumber::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestVersionNumber::testOperatorEqual() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2(5,5,5);
    VersionNumber ver3;
    QVERIFY(ver1 == ver2);
    QVERIFY(!(ver1 == ver3));
}

void Qtilities::Testing::TestVersionNumber::testOperatorNotEqual() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2(5,5,5);
    VersionNumber ver3;
    QVERIFY(!(ver1 != ver2));
    QVERIFY(ver1 != ver3);
}

void Qtilities::Testing::TestVersionNumber::testOperatorAssign() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2;
    QVERIFY(ver1 != ver2);
    ver2 = ver1;
    QVERIFY(ver1 == ver2);
}

void Qtilities::Testing::TestVersionNumber::testOperatorBigger() {
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

void Qtilities::Testing::TestVersionNumber::testOperatorBiggerEqual() {
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

void Qtilities::Testing::TestVersionNumber::testOperatorSmaller() {
    VersionNumber ver1(5,5,5);
    VersionNumber ver2(5,5,5);
    QVERIFY(!(ver2 < ver1));
    VersionNumber ver3(5,5,4);
    QVERIFY(ver3 < ver1);
    VersionNumber ver4(5,4,5);
    QVERIFY(ver4 < ver1);
    VersionNumber ver5(4,5,5);
    QVERIFY(ver5 < ver1);
    VersionNumber ver6(6,5,4);
    QVERIFY(!(ver6 < ver1));
    VersionNumber ver13(6,4,5);
    QVERIFY(!(ver13 < ver1));

    VersionNumber ver14(11,0,0);
    ver14.setDevelopmentStage(VersionNumber::DevelopmentStageServicePack);
    VersionNumber ver15(13,1,1);
//    ver15.setDevelopmentStage(VersionNumber::DevelopmentStageServicePack);
    QVERIFY(!(ver15 < ver14));
    QVERIFY(!(ver15 < ver15));

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

    VersionNumber ver_dev_stage_1(5,5,5,1,VersionNumber::DevelopmentStageBeta);
    VersionNumber ver_dev_stage_2(5,5,5,2,VersionNumber::DevelopmentStageBeta);
    QVERIFY(ver_dev_stage_1 < ver_dev_stage_2);
    VersionNumber ver_dev_stage_3(4,5,5,1,VersionNumber::DevelopmentStageBeta);
    QVERIFY(!(ver_dev_stage_1 < ver_dev_stage_3));
    VersionNumber ver_dev_stage_4(5,4,5,1,VersionNumber::DevelopmentStageBeta);
    QVERIFY(!(ver_dev_stage_1 < ver_dev_stage_4));
    ver_dev_stage_4.setIsVersionMinorUsed(false);
    QVERIFY(!(ver_dev_stage_1 < ver_dev_stage_4));
    ver_dev_stage_4.setIsVersionMinorUsed(true);
    ver_dev_stage_4.setIsVersionRevisionUsed(false);
    QVERIFY(!(ver_dev_stage_1 < ver_dev_stage_4));
    ver_dev_stage_4.setIsVersionMinorUsed(false);
    ver_dev_stage_4.setIsVersionRevisionUsed(false);
    QVERIFY(!(ver_dev_stage_1 < ver_dev_stage_4));
    VersionNumber ver_dev_stage_5(5,5,4,1,VersionNumber::DevelopmentStageBeta);
    QVERIFY(!(ver_dev_stage_1 < ver_dev_stage_5));
    VersionNumber ver_dev_stage_6(5,5,5,0,VersionNumber::DevelopmentStageBeta);
    QVERIFY(ver_dev_stage_1 < ver_dev_stage_6);

    VersionNumber ver_dev_stage_8(5,5,5,1,VersionNumber::DevelopmentStageAlpha);
    QVERIFY(!(ver_dev_stage_1 < ver_dev_stage_8));
    VersionNumber ver_dev_stage_12(5,5,5,2,VersionNumber::DevelopmentStageBeta);
    QVERIFY(ver_dev_stage_1 < ver_dev_stage_12);
    VersionNumber ver_dev_stage_9(5,5,5,1,VersionNumber::DevelopmentStageReleaseCandidate);
    QVERIFY(ver_dev_stage_1 < ver_dev_stage_9);
    VersionNumber ver_dev_stage_7(5,5,5,1,VersionNumber::DevelopmentStageNone);
    QVERIFY(ver_dev_stage_1 < ver_dev_stage_7);
    VersionNumber ver_dev_stage_10(5,5,5,1,VersionNumber::DevelopmentStageServicePack);
    QVERIFY(ver_dev_stage_1 < ver_dev_stage_10);
    VersionNumber ver_dev_stage_11(5,5,5,0,VersionNumber::DevelopmentStageServicePack);
    QVERIFY(ver_dev_stage_1 < ver_dev_stage_11);
    VersionNumber ver_dev_stage_13(5,5,5,0,VersionNumber::DevelopmentStageServicePack);
    QVERIFY(!(ver_dev_stage_11 < ver_dev_stage_13));
}

void Qtilities::Testing::TestVersionNumber::testOperatorSmallerEqual() {
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

// For some reason, I get a link error when using QCOMPARE with ver.toString()
// and a string literal:
//
//     undefined symbol: _ZN5QTest8qCompareI7QStringA6_cEEbRKT_RKT0_PKcSA_SA_i
//
// But QStringLiteral is apparently the "proper" thing to use anyway.

// I'm reverting this back to QString to keep v1.5 compatible with Qt 4.8.
void Qtilities::Testing::TestVersionNumber::testToString() {
    VersionNumber ver(1,2,3);
    QCOMPARE(ver.toString(), QString("1.2.3"));
    ver.setDevelopmentStage(VersionNumber::DevelopmentStageServicePack);
    ver.setVersionDevelopmentStage(1);
    QCOMPARE(ver.toString(), QString("1.2.3%1%2").arg(VersionNumber::defaultDevelopmentStageIdentifer(VersionNumber::DevelopmentStageServicePack)).arg(ver.versionDevelopmentStage()));
    ver.setDevelopmentStage(VersionNumber::DevelopmentStageReleaseCandidate);
    ver.setDevelopmentStageIdentifier(" Release Candidate ");
    ver.setVersionDevelopmentStage(2);
    QCOMPARE(ver.toString(), QString("1.2.3 Release Candidate 2"));
    ver.setVersionDevelopmentStage(0);
    ver.setFieldWidthMinor(3);
    QCOMPARE(ver.toString(), QString("1.002.3"));
    ver.setFieldWidthRevision(3);
    QCOMPARE(ver.toString(), QString("1.002.003"));

    ver.setIsVersionRevisionUsed(false);
    QCOMPARE(ver.toString(), QString("1.002"));
    ver.setIsVersionMinorUsed(false);
    QCOMPARE(ver.toString(), QString("1"));

    VersionNumber ver1(1,12,103);
    QCOMPARE(ver1.toString(), QString("1.12.103"));
    ver1.setFieldWidthMinor(3);
    QCOMPARE(ver1.toString(), QString("1.012.103"));
    ver1.setFieldWidthRevision(3);
    QCOMPARE(ver1.toString(), QString("1.012.103"));

    ver1.setIsVersionRevisionUsed(false);
    QCOMPARE(ver1.toString(), QString("1.012"));
    ver1.setIsVersionMinorUsed(false);
    QCOMPARE(ver1.toString(), QString("1"));
}

