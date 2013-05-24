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
