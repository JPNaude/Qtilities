/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#ifndef IEXPORTALE_FORMATTING_H
#define IEXPORTALE_FORMATTING_H

#include "QtilitiesCore_global.h"
#include "IExportable.h"
#include "IObjectBase.h"

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
            class QTILIITES_CORE_SHARED_EXPORT IExportableFormatting : virtual public IObjectBase {
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
