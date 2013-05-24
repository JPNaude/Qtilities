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

#ifndef IEXPORTALE_FORMATTING_H
#define IEXPORTALE_FORMATTING_H

#include "QtilitiesCore_global.h"
#include "IExportable.h"

class QDomDocument;
class QDomElement;

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            /*!
            \class IExportableFormatting
            \brief This interface allows object base classes to export formatting information which requires QtGui from within a library which does not use QtGui.

            <i>This class was added in %Qtilities v0.2.</i>
              */
            class QTILIITES_CORE_SHARED_EXPORT IExportableFormatting {
            public:
                IExportableFormatting() {}
                virtual ~IExportableFormatting() {}

                //----------------------------
                // XML Exporting
                //----------------------------
                //! Allows exporting of formatting information to a XML document. A reference to the QDomElement to which the object's information must be added is provided, along with a reference to the QDomDocument.
                virtual IExportable::ExportResultFlags exportFormattingXML(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version) const = 0;
                //! Allows importing of formatting information from a XML document. A reference to the QDomElement which contains the object's information is provided, along with a reference to the QDomDocument.
                virtual IExportable::ExportResultFlags importFormattingXML(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version) = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IExportableFormatting,"com.Qtilities.Core.IExportableFormatting/1.0")

#endif // IEXPORTALE_FORMATTING_H
