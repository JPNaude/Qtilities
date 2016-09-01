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

#include "TestExporting.h"

#include <QtilitiesProjectManagement>
using namespace QtilitiesProjectManagement;

#include <QtilitiesExtensionSystem>
using namespace QtilitiesExtensionSystem;

#include <QDomDocument>
#include <QDomElement>

int Qtilities::Testing::TestExporting::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestExporting::testPropertyNameEncoding() {
    QString string_name = QString("Test Name");
    const char* char_name = string_name.toUtf8().constData();
    QString string_name_2 = QString(char_name);
    const char* char_name_2 = string_name_2.toUtf8().constData();
    QCOMPARE(string_name, string_name_2);
    QCOMPARE(strcmp(char_name,char_name_2), 0);
}

void Qtilities::Testing::TestExporting::genericTest(IExportable* obj_source, IExportable* obj_import_binary, IExportable* obj_import_xml, Qtilities::ExportVersion write_version, Qtilities::ExportVersion read_version, const QString& file_name) {
    QDataStream::Version data_stream_write_version;
    if (write_version == Qtilities::Qtilities_1_0 || write_version == Qtilities::Qtilities_1_1 || write_version == Qtilities::Qtilities_1_2)
        data_stream_write_version =  QDataStream::Qt_4_7;
    QDataStream::Version data_stream_read_version;
    if (read_version == Qtilities::Qtilities_1_0 || read_version == Qtilities::Qtilities_1_1 || read_version == Qtilities::Qtilities_1_2)
        data_stream_read_version =  QDataStream::Qt_4_7;

    QList<QPointer<QObject> > import_list;
    // -------------------------------------------------
    // Binary Exporting & Importing
    // -------------------------------------------------
    if (obj_source->supportedFormats() & IExportable::Binary && obj_import_binary) {
        QFile file(QtilitiesApplication::applicationSessionPath() + "/" + file_name + ".binary");
        file.open(QIODevice::WriteOnly);
        QDataStream stream_out(&file);
        stream_out.setVersion(data_stream_write_version);
        obj_source->setExportVersion(write_version);
        obj_source->exportBinary(stream_out);
        file.close();

        file.open(QIODevice::ReadOnly);
        QDataStream stream_in(&file);
        stream_in.setVersion(data_stream_read_version);
        obj_import_binary->setExportVersion(read_version);
        obj_import_binary->importBinary(stream_in,import_list);

        QFile file1(QtilitiesApplication::applicationSessionPath() + "/" + file_name + "_readback.binary");
        file1.open(QIODevice::WriteOnly);
        QDataStream stream_out1(&file1);
        stream_out1.setVersion(data_stream_write_version);
        obj_import_binary->exportBinary(stream_out1);
        file1.close();
    }

    // -------------------------------------------------
    // XML Exporting & Importing
    // -------------------------------------------------
    if (obj_source->supportedFormats() & IExportable::XML && obj_import_xml) {
        QFile file(QtilitiesApplication::applicationSessionPath() + "/" + file_name + ".xml");
        file.open(QIODevice::WriteOnly);
        QDomDocument doc("QtilitiesTesting");
        QDomElement root = doc.createElement("QtilitiesTesting");
        doc.appendChild(root);
        QDomElement rootItem = doc.createElement("object_node");
        root.appendChild(rootItem);

        QCOMPARE(obj_source->exportXml(&doc,&rootItem), IExportable::Complete);
        QString docStr = doc.toString(2);
        file.write(docStr.toUtf8());
        file.close();

        obj_import_xml->setExportVersion(read_version);
        QCOMPARE(obj_import_xml->importXml(&doc,&rootItem,import_list), IExportable::Complete);

        QFile file2(QtilitiesApplication::applicationSessionPath() + "/" + file_name + "_readback.xml");
        file2.open(QIODevice::WriteOnly);
        QDomDocument doc2("QtilitiesTesting");
        QDomElement root2 = doc2.createElement("QtilitiesTesting");
        doc2.appendChild(root2);
        QDomElement rootItem2 = doc2.createElement("object_node");
        root2.appendChild(rootItem2);
        QCOMPARE(obj_import_xml->exportXml(&doc2,&rootItem2), IExportable::Complete);
        QString docStr2 = doc2.toString(2);
        file2.write(docStr2.toUtf8());
        file2.close();
    }  
}

// --------------------------------------------------------------------
// Test Qtilities_1_0 against Qtilities_1_0
// That is, exported with Qtilities_1_0 and imported with Qtilities_1_0
// --------------------------------------------------------------------

void Qtilities::Testing::TestExporting::testExportDynamicProperties_w1_0_r1_0() {
    QObject* obj_source = new QObject;
    QObject* obj_import_binary = new QObject;
    QObject* obj_import_xml = new QObject;

    // Add some properties:
    // 1. Test a shared property:
    SharedProperty test_shared_property("Shared Property",QVariant(5));
    ObjectManager::setSharedProperty(obj_source,test_shared_property);
    QVERIFY(ObjectManager::propertyExists(obj_source,"Shared Property"));
    // 2. Test a multi context property:
    MultiContextProperty test_multi_context_property("Multi Context Property");
    test_multi_context_property.setValue(QVariant(1),1);
    test_multi_context_property.setValue(QVariant(2),2);
    ObjectManager::setMultiContextProperty(obj_source,test_multi_context_property);
    QVERIFY(ObjectManager::propertyExists(obj_source,"Multi Context Property"));
    // 3. Test a normal QVariant property
    QVariant test_normal_variant(10);
    obj_source->setProperty("Normal Property",test_normal_variant);
    QVERIFY(ObjectManager::propertyExists(obj_source,"Normal Property"));

    // -------------------------------------------------
    // Binary Exporting & Importing
    // -------------------------------------------------
    QTemporaryFile file;
    file.open();
    QDataStream stream_out(&file);
    stream_out.setVersion(QDataStream::Qt_4_7);
    QCOMPARE(ObjectManager::exportObjectPropertiesBinary(obj_source,stream_out), IExportable::Complete);
    file.close();

    file.open();
    QDataStream stream_in(&file);
    stream_in.setVersion(QDataStream::Qt_4_7);

    QVERIFY(!ObjectManager::compareDynamicProperties(obj_source,obj_import_binary));
    QCOMPARE(ObjectManager::importObjectPropertiesBinary(obj_import_binary,stream_in), IExportable::Complete);
    QVERIFY(ObjectManager::compareDynamicProperties(obj_source,obj_import_binary));

    // -------------------------------------------------
    // XML Exporting & Importing
    // -------------------------------------------------
    QDomDocument doc("QtilitiesTesting");
    QDomElement root = doc.createElement("QtilitiesTesting");
    doc.appendChild(root);
    QDomElement rootItem = doc.createElement("Root");
    root.appendChild(rootItem);
    QFile xml_file(QtilitiesApplication::applicationSessionPath() + "/testExportDynamicProperties_w1_0_r1_0.xml");
    xml_file.open(QIODevice::WriteOnly);

    QCOMPARE(ObjectManager::exportObjectPropertiesXml(obj_source,&doc,&rootItem), IExportable::Complete);
    QString docStr = doc.toString(2);
    xml_file.write(docStr.toUtf8());
    xml_file.close();

    QVERIFY(!ObjectManager::compareDynamicProperties(obj_source,obj_import_xml));
    QCOMPARE(ObjectManager::importObjectPropertiesXml(obj_import_xml,&doc,&rootItem), IExportable::Complete);
    //QVERIFY(ObjectManager::compareDynamicProperties(obj_source,obj_import_xml));

    // Create a readback file:
    QDomDocument doc2("QtilitiesTesting");
    QDomElement root2 = doc2.createElement("QtilitiesTesting");
    doc2.appendChild(root2);
    QDomElement rootItem2 = doc2.createElement("Root");
    root2.appendChild(rootItem2);
    QFile xml_file2(QtilitiesApplication::applicationSessionPath() + "/testExportDynamicProperties_w1_0_r1_0_readback.xml");
    xml_file2.open(QIODevice::WriteOnly);

    QCOMPARE(ObjectManager::exportObjectPropertiesXml(obj_import_xml,&doc2,&rootItem2), IExportable::Complete);
    QString docStr2 = doc2.toString(2);
    xml_file2.write(docStr2.toUtf8());
    xml_file2.close();

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testInstanceFactoryInfo_w1_0_r1_0() {
    Qtilities::ExportVersion write_version = Qtilities::Qtilities_1_0;
    Qtilities::ExportVersion read_version = Qtilities::Qtilities_1_0;

    InstanceFactoryInfo obj_source;
    obj_source.d_factory_tag = "TestFactoryTag";
    obj_source.d_instance_name = "TestInstanceName";
    obj_source.d_instance_tag = "TestInstanceTag";

    // -------------------------------------------------
    // Binary Exporting & Importing
    // -------------------------------------------------
    QTemporaryFile file;
    file.open();
    QDataStream stream_out(&file);
    stream_out.setVersion(QDataStream::Qt_4_7);
    obj_source.exportBinary(stream_out,write_version);
    file.close();

    file.open();
    QDataStream stream_in(&file);
    stream_in.setVersion(QDataStream::Qt_4_7);

    InstanceFactoryInfo obj_import_binary;
    QVERIFY(obj_source != obj_import_binary);
    obj_import_binary.importBinary(stream_in,read_version);
    QCOMPARE(obj_source, obj_import_binary);

    // -------------------------------------------------
    // XML Exporting & Importing
    // -------------------------------------------------
    QDomDocument doc("QtilitiesTesting");
    QDomElement root = doc.createElement("QtilitiesTesting");
    doc.appendChild(root);
    QDomElement rootItem = doc.createElement("Root");

    QVERIFY(obj_source.exportXml(&doc,&rootItem,write_version));

    InstanceFactoryInfo obj_import_xml;
    QVERIFY(obj_source != obj_import_xml);
    QVERIFY(obj_import_xml.importXml(&doc,&rootItem,read_version));
    QCOMPARE(obj_source, obj_import_xml);
}

void Qtilities::Testing::TestExporting::testActivityPolicyFilter_w1_0_r1_0() {
    ActivityPolicyFilter* obj_source = new ActivityPolicyFilter;
    ActivityPolicyFilter* obj_import_binary = new ActivityPolicyFilter;
    ActivityPolicyFilter* obj_import_xml = new ActivityPolicyFilter;

    obj_source->setObjectName("TestObjectName");
    obj_source->setActivityPolicy(ActivityPolicyFilter::UniqueActivity);
    obj_source->setMinimumActivityPolicy(ActivityPolicyFilter::ProhibitNoneActive );
    obj_source->setNewSubjectActivityPolicy(ActivityPolicyFilter::SetNewActive);
    obj_source->setParentTrackingPolicy(ActivityPolicyFilter::ParentFollowActivity);
    obj_source->setIsModificationStateMonitored(false);

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testActivityPolicyFilter_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testActivityPolicyFilter_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testActivityPolicyFilter_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testActivityPolicyFilter_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testActivityPolicyFilter_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testCategoryLevel_w1_0_r1_0() {
    CategoryLevel* obj_source = new CategoryLevel;
    CategoryLevel* obj_import_binary = new CategoryLevel;
    CategoryLevel* obj_import_xml = new CategoryLevel;

    obj_source->d_name = "TestTest";

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testCategoryLevel_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testCategoryLevel_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testCategoryLevel_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testCategoryLevel_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testCategoryLevel_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testNamingPolicyFilter_w1_0_r1_0() {
    NamingPolicyFilter* obj_source = new NamingPolicyFilter;
    NamingPolicyFilter* obj_import_binary = new NamingPolicyFilter;
    NamingPolicyFilter* obj_import_xml = new NamingPolicyFilter;

    obj_source->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    obj_source->setUniquenessResolutionPolicy(NamingPolicyFilter::Reject);
    obj_source->setValidityResolutionPolicy(NamingPolicyFilter::Reject);

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testNamingPolicyFilter_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testNamingPolicyFilter_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testNamingPolicyFilter_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testNamingPolicyFilter_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testNamingPolicyFilter_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testQtilitiesCategory_w1_0_r1_0() {
    QtilitiesCategory* obj_source = new QtilitiesCategory;
    QtilitiesCategory* obj_import_binary = new QtilitiesCategory;
    QtilitiesCategory* obj_import_xml = new QtilitiesCategory;

    obj_source->setAccessMode(Observer::ReadOnlyAccess);
    (*obj_source) << "Top Level Category" << "Middle Level Category" << "Lowest Level Category";

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testQtilitiesCategory_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testQtilitiesCategory_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testQtilitiesCategory_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testQtilitiesCategory_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testQtilitiesCategory_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testObserverHints_w1_0_r1_0() {
    ObserverHints* obj_source = new ObserverHints;
    ObserverHints* obj_import_binary = new ObserverHints;
    ObserverHints* obj_import_xml = new ObserverHints;

    obj_source->setObserverSelectionContextHint(ObserverHints::SelectionUseSelectedContext);
    obj_source->setNamingControlHint(ObserverHints::ReadOnlyNames);
    obj_source->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
    obj_source->setActivityControlHint(ObserverHints::CheckboxTriggered);
    obj_source->setItemSelectionControlHint(ObserverHints::NonSelectableItems);
    obj_source->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    obj_source->setDisplayFlagsHint(ObserverHints::PropertyBrowser);
    obj_source->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    obj_source->setDragDropHint(ObserverHints::AllDragDrop);
    obj_source->setActionHints(ObserverHints::ActionAllHints);
    obj_source->setModificationStateDisplayHint(ObserverHints::CharacterModificationStateDisplay);
    obj_source->setCategoryFilterEnabled(true);

    QtilitiesCategory category1("Test1::Test1::Test1","::");
    QtilitiesCategory category2("Test2::Test2::Test2","::");
    QList<QtilitiesCategory> category_list;
    category_list << category1;
    category_list << category2;

    obj_source->setDisplayedCategories(category_list,true);

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testObserverHints_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverHints_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverHints_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverHints_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverHints_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testMultiContextProperty_w1_0_r1_0() {
    MultiContextProperty* obj_source = new MultiContextProperty("TestProperty");
    MultiContextProperty* obj_import_binary = new MultiContextProperty("");
    MultiContextProperty* obj_import_xml = new MultiContextProperty("");

    // ---------------------------------------------
    // We must test all QVariant types we support:
    // ---------------------------------------------
    // variant.type() == QVariant::String:
    obj_source->addContext("Test1",1);

    // variant.type() == QVariant::Bool:
    bool test_bool = true;
    obj_source->addContext(test_bool,2);

    // variant.type() == QVariant::ByteArray
    QByteArray ba("Hello");
    obj_source->addContext(ba,3);

    // variant.type() == QVariant::Char
    // TODO
//    obj_source->addContext(QChar(QChar::Mark_Enclosing),4);

    // variant.type() == QVariant::Date
    obj_source->addContext(QDate::currentDate(),5);

    // variant.type() == QVariant::DateTime
    QDateTime test_date_time(QDate::currentDate());
    obj_source->addContext(test_date_time,6);

    // variant.type() == QVariant::Double
    double test_double = 1.345;
    obj_source->addContext(test_double,7);

    // variant.type() == QVariant::Int
    int test_int = 10;
    obj_source->addContext(test_int,8);

    // variant.type() == QVariant::StringList
    QStringList test_string_list;
    test_string_list << "ListItem1" << "ListItem2" << "ListItem3";
    obj_source->addContext(test_string_list,9);

    // variant.type() == QVariant::Time
    QTime test_time(1,1,1);
    obj_source->addContext(test_time,10);

    // variant.type() == QVariant::UInt
    quint32 test_uint = 5;
    obj_source->addContext(test_uint,11);

    // variant.type() == QVariant::LongLong
    qlonglong test_long_log = 5999;
    obj_source->addContext(test_long_log,12);

    // variant.type() == QVariant::ULongLong
    qulonglong test_ulong_log = 10999;
    obj_source->addContext(test_ulong_log,13);

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testMultiContextProperty_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testMultiContextProperty_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testMultiContextProperty_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testMultiContextProperty_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testMultiContextProperty_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
}

void Qtilities::Testing::TestExporting::testRelationalTableEntry_w1_0_r1_0() {
    RelationalTableEntry* obj_source = new RelationalTableEntry;
    RelationalTableEntry* obj_import_binary = new RelationalTableEntry;
    RelationalTableEntry* obj_import_xml = new RelationalTableEntry;

    obj_source->addChild(1);
    obj_source->addChild(2);
    obj_source->addChild(3);
    obj_source->addParent(1);
    obj_source->addParent(1);
    obj_source->addParent(1);
    obj_source->setName("TestName");
    obj_source->setParentVisitorID(1);
    obj_source->setSessionID(10);
    obj_source->setOwnership(3);
    obj_source->setVisitorID(20);

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testRelationalTableEntry_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testRelationalTableEntry_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testRelationalTableEntry_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testRelationalTableEntry_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testRelationalTableEntry_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testObserverRelationalTable_w1_0_r1_0() {
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    parentNode2->addItem("Child 5");

    ObserverRelationalTable* obj_source = new ObserverRelationalTable(rootNode);
    ObserverRelationalTable* obj_import_binary = new ObserverRelationalTable;
    ObserverRelationalTable* obj_import_xml = new ObserverRelationalTable;

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testObserverRelationalTable_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverRelationalTable_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverRelationalTable_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverRelationalTable_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverRelationalTable_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testSharedProperty_w1_0_r1_0() {
    SharedProperty* obj_source = new SharedProperty("SharedProperty");
    SharedProperty* obj_import_binary = new SharedProperty;
    SharedProperty* obj_import_xml = new SharedProperty;

    obj_source->setValue("TestValue");

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testSharedProperty_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testSharedProperty_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testSharedProperty_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testSharedProperty_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testSharedProperty_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
}

void Qtilities::Testing::TestExporting::testSubjectTypeFilter_w1_0_r1_0() {
    SubjectTypeFilter* obj_source = new SubjectTypeFilter;
    SubjectTypeFilter* obj_import_binary = new SubjectTypeFilter;
    SubjectTypeFilter* obj_import_xml = new SubjectTypeFilter;

    obj_source->addSubjectType(SubjectTypeInfo("TestMetaType1","TestTypeName1"));
    obj_source->addSubjectType(SubjectTypeInfo("TestMetaType2","TestTypeName2"));
    obj_source->addSubjectType(SubjectTypeInfo("TestMetaType3","TestTypeName3"));
    obj_source->addSubjectType(SubjectTypeInfo("TestMetaType4","TestTypeName4"));
    obj_source->setGroupName("TestGroupName");
    obj_source->enableInverseFiltering(true);

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testSubjectTypeFilter_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testSubjectTypeFilter_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testSubjectTypeFilter_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testSubjectTypeFilter_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testSubjectTypeFilter_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testTreeFileItem_w1_0_r1_0() {
    TreeFileItem* obj_source = new TreeFileItem;
    TreeFileItem* obj_import_binary = new TreeFileItem;
    TreeFileItem* obj_import_xml = new TreeFileItem;

    obj_source->setFileForce("TestFileName.txt","/RelativeToPath");
    obj_source->setForegroundRole(QBrush(Qt::darkRed));
    obj_source->setFont(QFont("Helvetica [Cronyx]",20));
    obj_source->setAlignment(Qt::AlignCenter);
    obj_source->setBackgroundRole(QBrush(Qt::gray));
    obj_source->setSizeHint(QSize(10,20));
    obj_source->setStatusTip("Example Status Tip");
    obj_source->setToolTip("Example Tool Tip");
    obj_source->setWhatsThis("Example Whats This");

    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testTreeFileItem_w1_0_r1_0");

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testTreeFileItem_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testTreeFileItem_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testTreeFileItem_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testTreeFileItem_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testTreeItem_w1_0_r1_0() {
    TreeItem* obj_source = new TreeItem;
    TreeItem* obj_import_binary = new TreeItem;
    TreeItem* obj_import_xml = new TreeItem;

    obj_source->setForegroundRole(QBrush(Qt::darkRed));
    obj_source->setFont(QFont("Helvetica [Cronyx]",20));
    obj_source->setAlignment(Qt::AlignCenter);
    obj_source->setBackgroundRole(QBrush(Qt::gray));
    obj_source->setSizeHint(QSize(10,20));
    obj_source->setStatusTip("Example Status Tip");
    obj_source->setToolTip("Example Tool Tip");
    obj_source->setWhatsThis("Example Whats This");

    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testTreeItem_w1_0_r1_0");

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testTreeItem_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testTreeItem_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testTreeItem_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testTreeItem_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testCodeEditorProjectItemWrapper_w1_0_r1_0() {
    CodeEditorWidget code_editor_widget_source;
    CodeEditorProjectItemWrapper* obj_source = new CodeEditorProjectItemWrapper(&code_editor_widget_source);
    CodeEditorWidget code_editor_widget_binary;
    CodeEditorProjectItemWrapper* obj_import_binary = new CodeEditorProjectItemWrapper(&code_editor_widget_binary);
    CodeEditorWidget code_editor_widget_xml;
    CodeEditorProjectItemWrapper* obj_import_xml = new CodeEditorProjectItemWrapper(&code_editor_widget_xml);

    code_editor_widget_source.codeEditor()->setPlainText("Testing Plain Text... Hooray!");

    QVERIFY(code_editor_widget_source.codeEditor()->toPlainText() != code_editor_widget_binary.codeEditor()->toPlainText());
    QVERIFY(code_editor_widget_source.codeEditor()->toPlainText() != code_editor_widget_xml.codeEditor()->toPlainText());
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testCodeEditorProjectItemWrapper_w1_0_r1_0");
    QCOMPARE(code_editor_widget_source.codeEditor()->toPlainText(), code_editor_widget_binary.codeEditor()->toPlainText());
    QCOMPARE(code_editor_widget_source.codeEditor()->toPlainText(), code_editor_widget_xml.codeEditor()->toPlainText());

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testCodeEditorProjectItemWrapper_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testCodeEditorProjectItemWrapper_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testCodeEditorProjectItemWrapper_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testCodeEditorProjectItemWrapper_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testObserver_w1_0_r1_0() {
    Qtilities::ExportVersion write_version = Qtilities::Qtilities_1_0;
    Qtilities::ExportVersion read_version = Qtilities::Qtilities_1_0;

    // ---------------------------------------------------
    // Test ExportData only export:
    // ---------------------------------------------------
    {
        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() ExportData only start:");
        TreeNode* obj_source = new TreeNode("Root Node");
        TreeNode* obj_import_binary = new TreeNode;
        TreeNode* obj_import_xml = new TreeNode;

        // Add all known filters:
        obj_source->enableActivityControl(ObserverHints::CheckboxActivityDisplay);
        obj_source->enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames);
        obj_source->setChildType("Example child type");

        // Build a tree:
        obj_source->addItem("Item 1");
        obj_source->addItem("Item 2");
        obj_source->addItem("Item 3");
        TreeNode* child_node = obj_source->addNode("TestNode1");
        child_node->addItem("TestChild1");
        child_node->addItem("TestChild2");
        child_node->addItem("TestChild3");

        genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testObserverDataOnly_w1_0_r1_0");

        // Compare output files:
        QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverDataOnly_w1_0_r1_0");
        QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverDataOnly_w1_0_r1_0");
        QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverDataOnly_w1_0_r1_0");
        QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverDataOnly_w1_0_r1_0");
        if (obj_source->supportedFormats() & IExportable::Binary)
            QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
        if (obj_source->supportedFormats() & IExportable::XML)
            QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

        delete obj_source;
        delete obj_import_binary;
        delete obj_import_xml;
        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() ExportData only end:");
    }
    // ---------------------------------------------------
    // Test ExportData only with categories:
    // ---------------------------------------------------
    {
        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() ExportData only with categories start:");
        TreeNode* obj_source = new TreeNode("Root Node");
        TreeNode* obj_import_binary = new TreeNode;
        TreeNode* obj_import_xml = new TreeNode;

        // Add all known filters:
        obj_source->enableCategorizedDisplay();

        // Build a tree:
        obj_source->addItem("Item 1",QtilitiesCategory("TestCategory1"));
        obj_source->addItem("Item 2",QtilitiesCategory("TestCategory1::LowerTestLevel","::"));
        obj_source->addItem("Item 3",QtilitiesCategory("TestCategory2"));
        TreeNode* child_node = obj_source->addNode("TestNode1");
        child_node->addItem("TestChild1");
        child_node->addItem("TestChild2");
        child_node->addItem("TestChild3");

        genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testObserverDataOnlyWithCategories_w1_0_r1_0");

        // Compare output files:
        QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverDataOnlyWithCategories_w1_0_r1_0");
        QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverDataOnlyWithCategories_w1_0_r1_0");
        QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverDataOnlyWithCategories_w1_0_r1_0");
        QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverDataOnlyWithCategories_w1_0_r1_0");
        if (obj_source->supportedFormats() & IExportable::Binary)
            QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
        if (obj_source->supportedFormats() & IExportable::XML)
            QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

        delete obj_source;
        delete obj_import_binary;
        delete obj_import_xml;
        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() ExportData only with categories end:");
    }
    // ---------------------------------------------------
    // Test ExportData only containment tree building approach:
    // When doing containment, the containment class should reconstruct the contained observer: Otherwise
    // reconstruction will just skip the containment class and attach the contained observer directly
    // to the parent of the containment class.
    // ---------------------------------------------------
    //    {
    //        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() Containment start:");
    //        TreeNode* obj_source = new TreeNode("Root Node");
    //        TreeNode* obj_import_binary = new TreeNode;
    //        TreeNode* obj_import_xml = new TreeNode;

    //        // Add all known filters:
    //        obj_source->enableActivityControl(ObserverHints::CheckboxActivityDisplay);
    //        obj_source->enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames);
    //        obj_source->setChildType("Example child type");

    //        // Build a tree:
    //        obj_source->addItem("Item 1");
    //        obj_source->addItem("Item 2");
    //        QObject* obj = new QObject;
    //        obj->setObjectName("ContainedParent");
    //        obj_source->attachSubject(obj);
    //        TreeNode* child_node = new TreeNode("ContainedNode");
    //        child_node->addItem("TestChild1");
    //        child_node->addItem("TestChild2");
    //        child_node->addItem("TestChild3");
    //        child_node->setParent(obj);

    //        genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testObserverContainment_w1_0_r1_0");

    //        int original_xml = FileUtils::textFileHashCode(QString("%1.xml").arg("testObserverContainment_w1_0_r1_0"));
    //        int readback_xml = FileUtils::textFileHashCode(QString("%1_readback.xml").arg("testObserverContainment_w1_0_r1_0"));
    //        // We expect a fail here since containment works, but it should not really be used like this in exports.
    //        // The parent class must implement IExportable and manually export/import on the contained object.
    //        QEXPECT_FAIL("","Testing containment in a way that it should not really be used.",Continue);
    //        QVERIFY(original_xml == readback_xml);

    //        //delete obj_source;
    //        delete obj_import_binary;
    //        //delete obj_import_xml;
    //        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() Containment end:");
    //    }
    // ---------------------------------------------------
    // Test ExportRelationalData binary:
    // ---------------------------------------------------
    {
        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() Extended Binary start:");
        TreeNode* obj_source = new TreeNode("Root Node");
        // Give it the same name since we don't construct it using a factory:
        TreeNode* obj_import_binary = new TreeNode("Root Node");

        // Add all known filters:
        obj_source->enableActivityControl(ObserverHints::CheckboxActivityDisplay);
        obj_source->enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames);
        obj_source->setChildType("Example child type");

        // Build a tree with an item with multiple parents:
        obj_source->addItem("Item 1");
        obj_source->addItem("Item 2");
        TreeItem* shared_item = obj_source->addItem("Shared Item");
        TreeNode* child_nodeA = obj_source->addNode("TestNodeA");
        child_nodeA->addItem("TestChild1");
        child_nodeA->addItem("TestChild2");
        child_nodeA->addItem("TestChild3");
        TreeNode* child_nodeB = obj_source->addNode("TestNodeB");
        child_nodeB->addItem("TestChild1");
        child_nodeB->addItem("TestChild2");
        child_nodeB->addItem("TestChild3");
        child_nodeB->addItem(shared_item);

        QFile file("testObserverExtendedAll_w1_0_r1_0.binary");
        file.open(QIODevice::WriteOnly);
        QDataStream stream_out(&file);
        stream_out.setVersion(QDataStream::Qt_4_7);
        obj_source->setExportVersion(write_version);
        IExportable::ExportResultFlags result_flags = obj_source->exportBinaryExt(stream_out,ObserverData::ExportAllItems);
        QCOMPARE(result_flags, IExportable::Complete);
        file.close();

        file.open(QIODevice::ReadOnly);
        QList<QPointer<QObject> > import_list;
        QDataStream stream_in(&file);
        stream_in.setVersion(QDataStream::Qt_4_7);
        obj_import_binary->setExportVersion(read_version);
        QCOMPARE(obj_import_binary->importBinary(stream_in,import_list), IExportable::Complete);

        QFile file1("testObserverExtendedAll_w1_0_r1_0_readback.binary");
        file1.open(QIODevice::WriteOnly);
        QDataStream stream_out1(&file1);
        stream_out1.setVersion(read_version);
        QCOMPARE(obj_import_binary->exportBinaryExt(stream_out1,ObserverData::ExportAllItems), IExportable::Complete);
        file1.close();

        // Compare output files:
        QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverExtendedAll_w1_0_r1_0");
        QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverExtendedAll_w1_0_r1_0");
        QEXPECT_FAIL("", "Session IDs for observers are different, therefore the output files won't match exactly.", Continue);
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));

        delete obj_source;
        delete obj_import_binary;
        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() Extended Binary end:");
    }
    // ---------------------------------------------------
    // Test ExportRelationalData xml:
    // ---------------------------------------------------
    {
        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() Extended XML start:");
        TreeNode* obj_source = new TreeNode("Root Node");
        // Give it the same name since we don't construct it using a factory:
        TreeNode* obj_import_xml = new TreeNode("Root Node");

        // Add all known filters:
        obj_source->enableActivityControl(ObserverHints::CheckboxActivityDisplay);
        obj_source->enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames);
        obj_source->setChildType("Example child type");

        // Build a tree with an item with multiple parents:
        obj_source->addItem("Item 1");
        obj_source->addItem("Item 2");
        TreeItem* shared_item = obj_source->addItem("Shared Item");
        TreeNode* child_nodeA = obj_source->addNode("TestNodeA");
        child_nodeA->addItem("TestChild1");
        child_nodeA->addItem("TestChild2");
        child_nodeA->addItem("TestChild3");
        TreeNode* child_nodeB = obj_source->addNode("TestNodeB");
        child_nodeB->addItem("TestChild1");
        child_nodeB->addItem("TestChild2");
        child_nodeB->addItem("TestChild3");
        child_nodeB->addItem(shared_item);

        QFile file("testObserverExtendedAll_w1_0_r1_0.xml");
        file.open(QIODevice::WriteOnly);
        QDomDocument doc("QtilitiesTesting");
        QDomElement root = doc.createElement("QtilitiesTesting");
        doc.appendChild(root);
        QDomElement rootItem = doc.createElement("object_node");
        root.appendChild(rootItem);
        QCOMPARE(obj_source->exportXmlExt(&doc,&rootItem,ObserverData::ExportAllItems), IExportable::Complete);
        QString docStr = doc.toString(2);
        file.write(docStr.toUtf8());
        file.close();

        obj_import_xml->setExportVersion(read_version);
        QList<QPointer<QObject> > import_list;
        QCOMPARE(obj_import_xml->importXml(&doc,&rootItem,import_list), IExportable::Complete);

        QFile file2("testObserverExtendedAll_w1_0_r1_0_readback.xml");
        file2.open(QIODevice::WriteOnly);
        QDomDocument doc2("QtilitiesTesting");
        QDomElement root2 = doc2.createElement("QtilitiesTesting");
        doc2.appendChild(root2);
        QDomElement rootItem2 = doc2.createElement("object_node");
        root2.appendChild(rootItem2);
        QCOMPARE(obj_import_xml->exportXmlExt(&doc2,&rootItem2,ObserverData::ExportAllItems), IExportable::Complete);
        QString docStr2 = doc2.toString(2);
        file2.write(docStr2.toUtf8());
        file2.close();

        QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverExtendedAll_w1_0_r1_0");
        QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverExtendedAll_w1_0_r1_0");
        QEXPECT_FAIL("", "Session IDs for observers are different, therefore the output files won't match exactly.", Continue);
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));
        LOG_INFO("TestExporting::testObserver_w1_0_r1_0() Extended XML end:");
    }
}

void Qtilities::Testing::TestExporting::testProject_w1_0_r1_0() {
    CodeEditorWidget code_editor_widget_source;
    code_editor_widget_source.setObjectName("Code Editor");
    CodeEditorProjectItemWrapper* wrapper_source = new CodeEditorProjectItemWrapper(&code_editor_widget_source);
    Project* obj_source = new Project;
    obj_source->addProjectItem(wrapper_source);
    CodeEditorWidget code_editor_widget_binary;
    code_editor_widget_binary.setObjectName("Code Editor");
    CodeEditorProjectItemWrapper* wrapper_binary = new CodeEditorProjectItemWrapper(&code_editor_widget_binary);
    Project* obj_import_binary = new Project;
    obj_import_binary->addProjectItem(wrapper_binary);
    CodeEditorWidget code_editor_widget_xml;
    code_editor_widget_xml.setObjectName("Code Editor");
    CodeEditorProjectItemWrapper* warpper_xml = new CodeEditorProjectItemWrapper(&code_editor_widget_xml);
    Project* obj_import_xml = new Project;
    obj_import_xml->addProjectItem(warpper_xml);

    code_editor_widget_source.codeEditor()->setPlainText("Testing Plain Text... Hooray!");

    obj_source->saveProject(QtilitiesApplication::applicationSessionPath() + "/testProject_w1_0_r1_0.prj");
    obj_source->saveProject(QtilitiesApplication::applicationSessionPath() + "/testProject_w1_0_r1_0.xml");
    obj_import_binary->loadProject(QtilitiesApplication::applicationSessionPath() + "/testProject_w1_0_r1_0.prj");
    obj_import_xml->loadProject(QtilitiesApplication::applicationSessionPath() + "/testProject_w1_0_r1_0.xml");
    obj_import_binary->saveProject(QtilitiesApplication::applicationSessionPath() + "/testProject_w1_0_r1_0_readback.prj");
    obj_import_xml->saveProject(QtilitiesApplication::applicationSessionPath() + "/testProject_w1_0_r1_0_readback.xml");

    // Compare output files:
    QString file_original_binary = QString("%1/%2.prj").arg(QtilitiesApplication::applicationSessionPath()).arg("testProject_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.prj").arg(QtilitiesApplication::applicationSessionPath()).arg("testProject_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testProject_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testProject_w1_0_r1_0");
    QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testObserverProjectItemWrapper_w1_0_r1_0() {
    TreeNode* rootNode = new TreeNode("Root Node");
    TreeNode* binaryNode = new TreeNode("Root Node");
    TreeNode* xmlNode = new TreeNode("Root Node");
    rootNode->addItem("Item 1");
    rootNode->addItem("Item 2");
    rootNode->addItem("Item 3");
    TreeNode* child_node = rootNode->addNode("TestNode1");
    child_node->addItem("TestChild1");
    child_node->addItem("TestChild2");
    child_node->addItem("TestChild3");

    ObserverProjectItemWrapper* obj_source = new ObserverProjectItemWrapper(rootNode);
    ObserverProjectItemWrapper* obj_import_binary = new ObserverProjectItemWrapper(binaryNode);
    ObserverProjectItemWrapper* obj_import_xml = new ObserverProjectItemWrapper(xmlNode);

    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_0,Qtilities::Qtilities_1_0,"testObserverProjectItemWrapper_w1_0_r1_0");

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testCodeEditorProjectItemWrapper_w1_0_r1_0");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testCodeEditorProjectItemWrapper_w1_0_r1_0");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testCodeEditorProjectItemWrapper_w1_0_r1_0");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testCodeEditorProjectItemWrapper_w1_0_r1_0");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}

void Qtilities::Testing::TestExporting::testExtensionSystemConfigurationFiles_w1_0_r1_0() {
    QStringList inactive_plugins;
    inactive_plugins << "Test Plugin 1";
    inactive_plugins << "Test Plugin 2";
    inactive_plugins << "Test Plugin 3";
    QStringList filtered_plugins;
    filtered_plugins << "Filter1*";
    filtered_plugins << "Filter2*";
    filtered_plugins << "Filter3*";
    QVERIFY(EXTENSION_SYSTEM->savePluginConfiguration("testExtensionSystemConfigurationFiles_w1_0_r1_0.xml",&inactive_plugins,&filtered_plugins,Qtilities::Qtilities_1_0));
}

void Qtilities::Testing::TestExporting::testShortcutMappingFiles_w1_0_r1_0() {
    QVERIFY(ACTION_MANAGER->saveShortcutMapping("testShortcutMappingFiles_w1_0_r1_0.xml"));
}

// --------------------------------------------------------------------
// Test Qtilities_1_1 against Qtilities_1_1
// That is, exported with Qtilities_1_1 and imported with Qtilities_1_1
//
// We only test the classes for which the exporting changed.
// --------------------------------------------------------------------
void Testing::TestExporting::testObserverHints_w1_1_r1_1() {
    ObserverHints* obj_source = new ObserverHints;
    ObserverHints* obj_import_binary = new ObserverHints;
    ObserverHints* obj_import_xml = new ObserverHints;

    obj_source->setObserverSelectionContextHint(ObserverHints::SelectionUseSelectedContext);
    obj_source->setNamingControlHint(ObserverHints::ReadOnlyNames);
    obj_source->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
    obj_source->setActivityControlHint(ObserverHints::CheckboxTriggered);
    obj_source->setItemSelectionControlHint(ObserverHints::NonSelectableItems);
    obj_source->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    obj_source->setDisplayFlagsHint(ObserverHints::PropertyBrowser);
    obj_source->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    obj_source->setDragDropHint(ObserverHints::AllDragDrop);
    obj_source->setActionHints(ObserverHints::ActionAllHints);
    obj_source->setModificationStateDisplayHint(ObserverHints::CharacterModificationStateDisplay);
    obj_source->setCategoryFilterEnabled(true);

    // This is different in v1.1:
    obj_source->setCategoryEditingFlags(ObserverHints::CategoriesEditableAllLevels);

    QtilitiesCategory category1("Test1::Test1::Test1","::");
    QtilitiesCategory category2("Test2::Test2::Test2","::");
    QList<QtilitiesCategory> category_list;
    category_list << category1;
    category_list << category2;

    obj_source->setDisplayedCategories(category_list,true);

    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(*obj_source != *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(*obj_source != *obj_import_xml);
    genericTest(obj_source,obj_import_binary,obj_import_xml,Qtilities::Qtilities_1_1,Qtilities::Qtilities_1_1,"testObserverHints_w1_1_r1_1");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QCOMPARE(*obj_source, *obj_import_binary);
    if (obj_source->supportedFormats() & IExportable::XML)
        QCOMPARE(*obj_source, *obj_import_xml);

    // Compare output files:
    QString file_original_binary = QString("%1/%2.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverHints_w1_1_r1_1");
    QString file_readback_binary = QString("%1/%2_readback.binary").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverHints_w1_1_r1_1");
    QString file_original_xml = QString("%1/%2.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverHints_w1_1_r1_1");
    QString file_readback_xml = QString("%1/%2_readback.xml").arg(QtilitiesApplication::applicationSessionPath()).arg("testObserverHints_w1_1_r1_1");
    if (obj_source->supportedFormats() & IExportable::Binary)
        QVERIFY(FileUtils::compareFiles(file_original_binary,file_readback_binary));
    if (obj_source->supportedFormats() & IExportable::XML)
        QVERIFY(FileUtils::compareFiles(file_original_xml,file_readback_xml));

    delete obj_source;
    delete obj_import_binary;
    delete obj_import_xml;
}
