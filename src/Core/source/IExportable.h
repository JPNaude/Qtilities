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

#ifndef IEXPORTALE_H
#define IEXPORTALE_H

#include "QtilitiesCore_global.h"
#include "IObjectBase.h"
#include "Qtilities.h"
#include "InstanceFactoryInfo.h"

#include <Logger>

#include <QList>
#include <QPointer>
#include <QString>

class QDomDocument;
class QDomElement;

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            using namespace Qtilities::Core;
            using namespace Qtilities::Logging;

            /*!
            \class IExportable
            \brief Objects can implement this interface if they are able to export and reconstruct themselves.

            IExportable is an interface used throughout %Qtilities by classes in order to stream their data. At present two export options are supported:
            - Serialized binary streaming
            - QDomDocument construction

            Any object that implements this interface can specify which of the above export formats it supports through the supportedFormats() function. The interface also allows you to provide the needed information about reconstructing your object through the instanceFactoryInfo() function. In short, this allows your object to specify the factory that should be used to reconstruct it as well as the factory tag to use in that factory. For a detailed overview of the factory architecture used in %Qtilities, please refer to \ref page_factories.

            In your own applications you can easily implement this interface in order to make your objects reconstructable and exportable throughout %Qtilities. For example if you implement this interface in an object and attach that object to an Qtilities::Core::Observer, the observer's export function will automatically make your object part of their exports. All export and import functions returns the result of their operation in the form of Qtilities::Core::Interfaces::IExportable::Result.

            Lets look at the two export formats in detail.

            \section iexportable_binary Binary Exporting

            Binary exports are done through the exportBinary() and importBinary() functions. This works exactly in the same way as normal << and >> stream operator, you get a QDataStream object to which your data is streamed. On the import side, there is an additional parameter which acts as a cleanup list. This is used in cases where you construct objects which are not parented through QObject::parent() and you have a long list of objects that is streamed. If an object close to then end of the list indicates that it failed, the import operation must be stopped and all unparented objects must be deleted in order to prevent memory leaks. In this case the list of objects which must be deleted will be available in this list.

            See the \ref iexportable_comparison section of this page for a comparison between Binary and XML exports.

            \section iexportable_xml XML Exporting

            XML exports allow you to build up an XML QDomDocument with information about a set of objects and is performed through the exportXml() and importXml() functions. These both provides you with a reference to the QDomDocument which allows you to create a new QDomElements. A QDomElement which represents your object is also provided. This allows you to easily construct new QDomElements and attach them to your objects node.

            See the \ref iexportable_comparison section of this page for a comparison between Binary and XML exports.

            \section iexportable_comparison Binary vs. XML Exports

            Both binary and XML imports have their advantages and disadvantages and when using %Qtilities projects, observers or export functions on the object manager, additional advantages and disadvantages applies.

            Lets provide an overview of the advantages and disadvantages of both approaches:

            <b>Binary Exports</b><br>
            Advantages:
            - Allows you to stream binary data which won't always be allowed in a XML document format.
            - Very usefull when you need to serialize complex data structures which is not viewed by a user.
            - When streaming tree structures to binary it is possible to add the complete relational table (see Qtilities::Core::ObserverRelationalTable) of your tree to the output and it can be reconstructed during an import.

            Disadvantages:
            - Difficult to debug.
            - Not user friendly. If you for example use projects in your application, users would much rather prefer an XML file that they can edit than a stream of binary data.
            - Backward compatibility of exported files becomes difficult to maintain and changing classes requires frequency export format version steps.
            - All object's properties are not exported.

            <b>XML Exports</b><br>
            Advantages:
            - User friendly.
            - Easy to debug and maintain.
            - Backward compatibility is easy to do and stepping of export format versions happens way less frequently than in binray exports.
            - Perfect for simple data, and for more advanced data CDATA elements are available.
            - When streaming tree structures to XML it is possible to add the complete relational table (see Qtilities::Core::ObserverRelationalTable) of your tree to the output and it can be reconstructed during an import.

            Disadvantages:
            - All object's properties are not exported.

            Clearly there are advantages in both export methods, and if more usefull standards arive in the future the interface can be expanded and your objects can easily adopt to new standards (JSON for example).

            \section iexportable_versioning Versioning

            Proper versioning of the format used during import and export operations is important if you want to support files that was exported using previous versions of your applications, and previous version of %Qtilities. Much like the way QDataStream::version() functionality allows easy management of the format that internal Qt classes use during data streaming operations, the interface provides the exportVersion() and setExportVersion() functions in order to determine the formats used during export and import operations on %Qtilities classes. In addition to that, the interface offers applicationExportVersion() and setApplicationExportVersion() which allows you keep track of what formatting to use for your own classes.

            You can specify your application's export version as follows:
\code
int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("YourOrganization");
    QtilitiesApplication::setOrganizationDomain("YourDomain");
    QtilitiesApplication::setApplicationName("My Application");
    QtilitiesApplication::setApplicationVersion("1.0");

    // Set the application export version:
    QtilitiesApplication::setApplicationExportVersion(0);

    // lots of application code...
}
\endcode

            The same can be done in non-GUI applications through Qtilities::Core::QtilitiesCoreApplication. When using the approach above, all your classes which implement IExportable will automatically return the application export version you set, and you can specify a different version to use explicitly if you want to using the setter function. The exportVersion() function will automatically return the latest version of %Qtilities, specified through Qtilities::Qtilities_Latest.

            An important note is that in tree type hierarchical structure, the export and application export version must be traversed down the tree. This is automatically done by Qtilities::Core::Observer, thus if you attach your objects to an Observer and use it to do your exports, you don't need to worry about it. Also when using for example Qtilities::ProjectManagement::Project you don't need to care about this as it is automatically done for you. Or if you use Qtilities::CoreGui::TreeNode::saveToFile(), the same will apply, you don't need to worry about it.

            For an overview of the data formats used by different versions of %Qtilities, see \ref page_serializing_overview.

            \section iexportable_example Example Implementation

            This section provides an example implementation of IExportable and shows some output results. We will only implement the XML import and export functions here for simplicity, but the same principles applies to Binary exports.

            The example class is called VersionInformation and it basically stores information about a version number. It implements IExportable and implements itself as a FactoryItem so that it can be reconstructed during imports. The complete class along with its source code is part of the \p ExportingExample example in the \p QtilitiesExamples project and documented under Qtilities::Examples::ExportingExample::VersionDetails.

            Here only the export and import functions will be shown. First, the export function which shows how the function handles different application versions.

\code
IExportable::Result VersionDetails::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    if (applicationExportVersion() < 0)
        return IExportable::FailedTooOld;
    if (applicationExportVersion() > 2)
        return IExportable::FailedTooNew;

    // Create a simple node and add our information to it:
    QDomElement revision_data = doc->createElement("RevisionInfo");
    object_node->appendChild(revision_data);

    // Information in both version 0 and version 1 of our class:
    revision_data.setAttribute("DescriptionBrief",d->description_brief);
    revision_data.setAttribute("DescriptionDetailed",d->description_detailed);
    revision_data.setAttribute("Minor",d->version_minor);
    revision_data.setAttribute("Major",d->version_major);

    // Lets say we add a new parameter in the next version of the class:
    if (applicationExportVersion() == 1)
        revision_data.setAttribute("NewAttribute",d->new_attribute_storage);

    return IExportable::Complete;
}
\endcode

            And next the import function which checks the application export version again. In this case the version will be set to the version of the file that is parsed.

\code
IExportable::Result VersionDetails::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    if (applicationExportVersion() < 0)
        return IExportable::FailedTooOld;
    if (applicationExportVersion() > 2)
        return IExportable::FailedTooNew;

    // Find our RevisionInfo element:
    IExportable::Result result = IExportable::Complete;
    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); i++) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "RevisionInfo") {
            if (child.hasAttribute("DescriptionBrief"))
                d->description_brief = child.attribute("DescriptionBrief");
            if (child.hasAttribute("DescriptionDetailed"))
                d->description_detailed = child.attribute("DescriptionDetailed");
            if (child.hasAttribute("Minor"))
                d->version_minor = child.attribute("Minor").toInt();
            if (child.hasAttribute("Major"))
                d->version_major = child.attribute("Major").toInt();
            if (child.hasAttribute("NewAttribute") && applicationExportVersion() == 1)
                d->new_attribute_storage = child.attribute("NewAttribute");
            continue;
        }
    }

    return result;
}
\endcode

        The \p ExportingExample basically creates a couple of classes, attaches them to an Qtilities::CoreGui::TreeNode and exports them for us to see the effect of the applicationExportVersion().

\code
int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("YourOrganization");
    QtilitiesApplication::setOrganizationDomain("YourDomain");
    QtilitiesApplication::setApplicationName("My Application");
    QtilitiesApplication::setApplicationVersion("1.0");

    // Set the application export version:
    QtilitiesApplication::setApplicationExportVersion(0);

    // Register our VersionDetails class in the Qtilities factory:
    FactoryItemID version_info_id("Version Details");
    OBJECT_MANAGER->registerFactoryInterface(&VersionDetails::factory,version_info_id);

    // Next create a TreeNode with a couple of our classes attached to it:
    TreeNode* node = new TreeNode("TestNode");
    VersionDetails* ver1 = new VersionDetails;
    ver1->setDescriptionBrief("Version 1 Brief");
    ver1->setDescriptionDetailed("Version 1 Brief");
    ver1->setVersionMajor(0);
    ver1->setVersionMinor(0);
    VersionDetails* ver2 = new VersionDetails;
    ver2->setDescriptionBrief("Version 2 Brief");
    ver2->setDescriptionDetailed("Version 2 Brief");
    ver2->setVersionMajor(1);
    ver2->setVersionMinor(2);
    node->attachSubject(ver1);
    node->attachSubject(ver2);
    node->addNode("NewNode");

    // Next export the node to a file:
    node->saveToFile("Output_Version_0.xml");
    node->setApplicationExportVersion(1);
    node->saveToFile("Output_Version_1.xml");

    ObserverWidget* view = new ObserverWidget(node);
    view->show();
    return a.exec();
}
\endcode

            Finally we look at the section of the two output files for this example where the VersionDetails class was exported. First the contents of \p Output_Version_0.xml is shown:

\code
<TreeItem Ownership="ManualOwnership" InstanceFactoryInfo="Version Details" Name="Version 1 Brief">
    <RevisionInfo Minor="0" Major="0" DescriptionDetailed="Version 1 Brief" DescriptionBrief="Version 1 Brief"/>
</TreeItem>
<TreeItem Ownership="ManualOwnership" InstanceFactoryInfo="Version Details" Name="Version 2 Brief">
    <RevisionInfo Minor="2" Major="1" DescriptionDetailed="Version 2 Brief" DescriptionBrief="Version 2 Brief"/>
\endcode


            Next the contents of \p Output_Version_1.xml is shown, note the added \p NewAttribute parameter:
\code
<TreeItem Ownership="ManualOwnership" InstanceFactoryInfo="Version Details" Name="Version 1 Brief">
    <RevisionInfo Minor="0" Major="0" DescriptionDetailed="Version 1 Brief" DescriptionBrief="Version 1 Brief" NewAttribute=""/>
</TreeItem>
<TreeItem Ownership="ManualOwnership" InstanceFactoryInfo="Version Details" Name="Version 2 Brief">
    <RevisionInfo Minor="2" Major="1" DescriptionDetailed="Version 2 Brief" DescriptionBrief="Version 2 Brief" NewAttribute=""/>
\endcode

            If the above is not clear enough, running and playing around with the example should clear any confusion.

              */
            class QTILIITES_CORE_SHARED_EXPORT IExportable : virtual public IObjectBase {
            public:
                IExportable();
                virtual ~IExportable() {}

                //! Possible export modes that an implementation of IExportable can support.
                /*!
                  \sa supportedFormats()
                  */
                enum ExportMode {
                    None = 0,      /*!< Does not support any export modes. */
                    Binary = 1,    /*!< Binary exporting using QDataStream. \sa exportBinary(), importBinary() */
                    XML = 2        /*!< XML exporting using QDomDocument. \sa exportXml(), importXml() */
                };
                Q_DECLARE_FLAGS(ExportModeFlags, ExportMode);
                Q_FLAGS(ExportModeFlags);

                //! The possible results of an export/import operation.
                enum Result {
                    Complete,     /*!< Complete when all the information was successfully exported/imported. */
                    Incomplete,   /*!< Incomplete when some information could not be exported/imported. An example of this is when an Observer exports itself. When only a subset of the subjets observed by the observer implements the IExportable interface the Observer will return Partial because it was only exported partially. */
                    Failed,       /*!< Failed when an error occured. The operation must be aborted in this case. */
                    FailedTooNew, /*!< Failed because the import format is too new. The operation must be aborted in this case. */
                    FailedTooOld  /*!< Failed because the import format is too old. The operation must be aborted in this case. */
                };
                Q_ENUMS(Result)

                //! Provides information about the export format(s) supported by your implementation of IExportable.
                /*!
                  \note It is important to note that you need to check the exportVersion() when you return the supported formats.
                  */
                virtual ExportModeFlags supportedFormats() const;
                //! The instance factory information which must be used when the exported object is reconstructed during an import.
                /*!
                  For more information see \ref factory_iexportable_relationship.
                  */
                virtual InstanceFactoryInfo instanceFactoryInfo() const { return InstanceFactoryInfo(); }

                //----------------------------
                // Versioning
                //----------------------------
                //! Returns the export version currently used by all %Qtilities classes.
                /*!
                  By default the latest %Qtilities version is selected. For an overview of the data formats used by %Qtilities, see \ref page_serializing_overview.

                  \sa setExportVersion()
                  */
                inline Qtilities::ExportVersion exportVersion() const { return d_export_version; }
                //! Returns the export version currently used by all %Qtilities classes.
                /*!
                  By default the latest %Qtilities version is selected.

                  \sa exportVersion()
                  */
                virtual void setExportVersion(Qtilities::ExportVersion version) { d_export_version = version; }
                //! Returns the application export version currently used by all your application's classes.
                /*!
                  The default is version 0.

                  \sa setApplicationExportVersion()
                  */
                quint32 applicationExportVersion() const;
                //! Returns the application export version currently used by all your application's classes.
                /*!
                  \sa applicationExportVersion()
                  */
                virtual void setApplicationExportVersion(quint32 version);

                //----------------------------
                // Is Exportable
                //----------------------------
                //! Sets if this object must be part of it's parents' exports.
                virtual void setIsExportable(bool new_is_exportable) {
                    d_is_exportable = new_is_exportable;
                }
                //! Gets if this object must be part of it's parents' exports.
                /*!
                  True by default.
                  */
                bool isExportable() const {
                    return d_is_exportable;
                }

                //----------------------------
                // Binary Exporting
                //----------------------------
                //! Allows exporting to a QDataStream.
                /*!
                    See \ref page_serializing_overview for more information about the expected output format.

                    \param stream A reference to the QDataStream to which the object's information must be appended is provided.
                  */
                virtual Result exportBinary(QDataStream& stream) const;
                //! Allows importing and reconstruction of the object state from information provided in a QDataStream.
                /*!
                    See \ref page_serializing_overview for more information about the expected output format.

                    \param stream The QDataStream which contains the object's information.
                    \param import_list All objects constructed during the import operation must be added to the import list. When the operation fails, all objects in this list will be deleted.
                    */
                virtual Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);

                //----------------------------
                // XML Exporting
                //----------------------------
                //! Allows exporting to an XML document. A reference to the QDomElement to which the object's information must be added is provided, along with a reference to the QDomDocument.
                /*!
                    See \ref page_serializing_overview for more information about the expected output format.
                  */
                virtual Result exportXml(QDomDocument* doc, QDomElement* object_node) const;
                //! Allows importing and reconstruction of data from information provided in a XML document. A reference to the QDomElement which contains the object's information is provided, along with a reference to the QDomDocument.
                /*!
                    See \ref page_serializing_overview for more information about the expected output format.
                  */
                virtual Result importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

                //----------------------------
                // Enum <-> String Functions
                //----------------------------
                //! Function which returns a string associated with a specific ExportMode.
                static QString exportModeToString(ExportMode export_mode);
                //! Function which returns the ExportMode associated with a string.
                static ExportMode stringToExportMode(const QString& export_mode_string);

            private:
                Qtilities::ExportVersion d_export_version;
                quint32 d_export_application_version;
                bool d_is_exportable;
                bool d_application_export_version_set;
            };

            Q_DECLARE_OPERATORS_FOR_FLAGS(IExportable::ExportModeFlags)
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IExportable,"com.Qtilities.Core.IExportable/1.0")

#endif // IEXPORTALE_H
