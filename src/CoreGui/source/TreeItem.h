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

#ifndef TREE_ITEM_H
#define TREE_ITEM_H

#include "QtilitiesCoreGui_global.h"
#include "TreeItemBase.h"

#include <IModificationNotifier>
#include <IExportable>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct TreeItemPrivateData
        \brief Structure used by TreeItem to store private data.
          */
        struct TreeItemPrivateData;

        /*!
          \class TreeItem
          \brief The TreeItem class is an item in a tree. It can be attached to a TreeNode.

          <i>This class was added in %Qtilities v0.2.</i>
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TreeItem : public TreeItemBase
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)
            Q_PROPERTY(QFont Font READ getFont WRITE setFont)
            Q_PROPERTY(QColor ForegroundRole READ getForegroundColor WRITE setForegroundColor)
            Q_PROPERTY(QColor BackgroundRole READ getBackgroundColor WRITE setBackgroundColor)
            Q_PROPERTY(QSize Size READ getSizeHint WRITE setSizeHint)
            Q_PROPERTY(QString StatusTip READ getStatusTip WRITE setStatusTip)
            Q_PROPERTY(QString ToolTip READ getToolTip WRITE setToolTip)
            Q_PROPERTY(QString WhatsThis READ getWhatsThis WRITE setWhatsThis)
            Q_PROPERTY(QString Category READ getCategoryString WRITE setCategoryString)

        public:
            TreeItem(const QString& name = QString(), QObject* parent = 0);
            virtual ~TreeItem();

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, TreeItem> factory;

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

        protected:
            TreeItemPrivateData* d;
        };
    }
}

#endif //  TREE_ITEM_H
