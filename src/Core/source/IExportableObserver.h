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

#ifndef IEXPORTALE_OBSERVER_H
#define IEXPORTALE_OBSERVER_H

#include "QtilitiesCore_global.h"
#include "IExportable.h"
#include "ObserverData.h"

class QDomDocument;
class QDomElement;

namespace Qtilities {
    namespace Core {
        namespace Interfaces {

            /*!
            \class IExportableObserver
            \brief Observers and observer subclasses must implement this extended version of Qtilities::Core::IExportable in order to support the extended exporting on observers.
              */
            class QTILIITES_CORE_SHARED_EXPORT IExportableObserver {
            public:
                IExportableObserver() {}
                virtual ~IExportableObserver() {}

                virtual IExportable::ExportResultFlags exportBinaryExt(QDataStream& stream, ObserverData::ExportItemFlags export_flags = ObserverData::ExportData) const {
                    Q_UNUSED(stream)
                    Q_UNUSED(export_flags)

                    return IExportable::Complete;
                }
                virtual IExportable::ExportResultFlags exportXmlExt(QDomDocument* doc, QDomElement* object_node, ObserverData::ExportItemFlags export_flags = ObserverData::ExportData) const {
                    Q_UNUSED(doc)
                    Q_UNUSED(object_node)
                    Q_UNUSED(export_flags)

                    return IExportable::Complete;
                }
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IExportableObserver,"com.Qtilities.Core.IExportableObserver/1.0")

#endif // IEXPORTALE_OBSERVER_H
