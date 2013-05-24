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
