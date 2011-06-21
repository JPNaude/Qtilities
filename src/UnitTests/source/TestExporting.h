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

#ifndef TEST_EXPORTING_H
#define TEST_EXPORTING_H

#include "UnitTests_global.h"
#include <IExportable>

#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace UnitTests {
        using namespace Interfaces;
        using namespace Qtilities::Core::Interfaces;

        //! Allows testing of the exporting capabilities of %Qtilities classes.
        class UNIT_TESTS_SHARED_EXPORT TestExporting: public QObject, public ITestable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::UnitTests::Interfaces::ITestable)

        public:
            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // ITestable Implementation
            // --------------------------------
            int execTest(int argc = 0, char ** argv = 0);
            QString testName() const { return tr("Exporting of all exportable classes"); }

        private slots:
            // --------------------------------------------------------------------
            // Test Qtilities_0_3 against Qtilities_0_3
            // That is, exported with Qtilities_0_3 and imported with Qtilities_0_3
            // --------------------------------------------------------------------
            void testInstanceFactoryInfo_0_3_0_3();
            void testActivityPolicyFilter_0_3_0_3();
            void testCategoryLevel_0_3_0_3();
            void testNamingPolicyFilter_0_3_0_3();
            void testQtilitiesCategory_0_3_0_3();
            void testObserverHints_0_3_0_3();
            void testMultiContextProperty_0_3_0_3();
            void testRelationalTableEntry_0_3_0_3();
            void testObserverRelationalTable_0_3_0_3();
            void testSharedProperty_0_3_0_3();
            void testSubjectTypeFilter_0_3_0_3();
            void testTreeFileItem_0_3_0_3();
            void testTreeItem_0_3_0_3();
            void testCodeEditorProjectItemWrapper_0_3_0_3();
            void testObserver_0_3_0_3();
            void testProject_0_3_0_3();
            void testObserverProjectItemWrapper_0_3_0_3();
            void testExtensionSystemConfigurationFiles_0_3_0_3();
            void testShortcutMappingFiles_0_3_0_3();

        private:
            void genericTest(IExportable* obj_source,IExportable* obj_import_binary,IExportable* obj_import_xml,Qtilities::ExportVersion write_version, Qtilities::ExportVersion read_version, const QString& file_name);
        };
    }
}

#endif // TEST_EXPORTING_H
