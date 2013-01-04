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

#ifndef TEST_EXPORTING_H
#define TEST_EXPORTING_H

#include "Testing_global.h"
#include <IExportable>

#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;
        using namespace Qtilities::Core::Interfaces;

        //! Allows testing of the exporting capabilities of %Qtilities classes.
        /*!
        We always test against the latest version of Qtilities. The goal is to support
        writing out and reading back of all previous export versions.
        Thus is acheived by setting the read and write versions to the same previous version.
          */
        class TESTING_SHARED_EXPORT TestExporting: public QObject, public ITestable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Testing::Interfaces::ITestable)

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
            // Test Qtilities_1_0 against Qtilities_1_0
            // That is, exported with Qtilities_1_0 and imported with Qtilities_1_0
            // --------------------------------------------------------------------
            void testPropertyNameEncoding();
            void testSharedProperty_w1_0_r1_0();
            void testMultiContextProperty_w1_0_r1_0();
            void testExportDynamicProperties_w1_0_r1_0();
            void testInstanceFactoryInfo_w1_0_r1_0();
            void testActivityPolicyFilter_w1_0_r1_0();
            void testCategoryLevel_w1_0_r1_0();
            void testNamingPolicyFilter_w1_0_r1_0();
            void testQtilitiesCategory_w1_0_r1_0();
            void testObserverHints_w1_0_r1_0();
            void testRelationalTableEntry_w1_0_r1_0();
            void testObserverRelationalTable_w1_0_r1_0();
            void testSubjectTypeFilter_w1_0_r1_0();
            void testTreeFileItem_w1_0_r1_0();
            void testTreeItem_w1_0_r1_0();
            void testCodeEditorProjectItemWrapper_w1_0_r1_0();
            void testObserver_w1_0_r1_0();
            void testProject_w1_0_r1_0();
            void testObserverProjectItemWrapper_w1_0_r1_0();
            void testExtensionSystemConfigurationFiles_w1_0_r1_0();
            void testShortcutMappingFiles_w1_0_r1_0();

            // --------------------------------------------------------------------
            // Test Qtilities_1_1 against Qtilities_1_1
            // That is, exported with Qtilities_1_1 and imported with Qtilities_1_1
            //
            // We only test the classes for which the exporting changed.
            // --------------------------------------------------------------------
            void testObserverHints_w1_1_r1_1();

        private:
            void genericTest(IExportable* obj_source,IExportable* obj_import_binary,IExportable* obj_import_xml,Qtilities::ExportVersion write_version, Qtilities::ExportVersion read_version, const QString& file_name);
        };
    }
}

#endif // TEST_EXPORTING_H
