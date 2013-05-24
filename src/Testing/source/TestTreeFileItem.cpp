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

#include "TestTreeFileItem.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

int Qtilities::Testing::TestTreeFileItem::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestTreeFileItem::testMe() {

}
