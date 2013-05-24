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

#include "TestActivityPolicyFilter.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <TreeIterator>

int Qtilities::Testing::TestActivityPolicyFilter::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}
