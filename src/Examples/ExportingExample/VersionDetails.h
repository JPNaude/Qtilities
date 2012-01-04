/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#ifndef VERSION_DETAILS_H
#define VERSION_DETAILS_H

#include <QObject>
#include <IExportable>
#include <Factory>

namespace Qtilities {
    namespace Examples {
        //! Namespace containing all the classes which forms part of the Exporting Example.
        namespace ExportingExample {
            using namespace Qtilities::Core;
            using namespace Qtilities::Core::Interfaces;

            struct VersionDetailsPrivateData;

            //! The VersionDetails class is an example implementation of Qtilities::Core::Interfaces::IExportable.
            class VersionDetails : public QObject, public IExportable
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)

            public:
                explicit VersionDetails(const QString& brief = QString(), const QString& detailed = QString(), QObject *parent = 0);
                ~VersionDetails();

                QString descriptionBrief() const;
                void setDescriptionBrief(const QString& description);
                QString descriptionDetailed() const;
                void setDescriptionDetailed(const QString& description);
                int versionMajor() const;
                void setVersionMajor(int major);
                int versionMinor() const;
                void setVersionMinor(int minor);

                // --------------------------------
                // IObjectBase Implemenation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------
                // IExportable Implemenation
                // --------------------------------
                ExportModeFlags supportedFormats() const;
                InstanceFactoryInfo instanceFactoryInfo() const;
                Result exportXml(QDomDocument* doc, QDomElement* object_node) const;
                Result importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

                // --------------------------------
                // Factory Interface Implemenation
                // --------------------------------
                static FactoryItem<QObject, VersionDetails> factory;

            private:
                VersionDetailsPrivateData* d;
            };
        }
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Examples::ExportingExample::VersionDetails& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Examples::ExportingExample::VersionDetails& stream_obj);

#endif // VERSION_DETAILS_H
