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

#ifndef ABSTRACT_TREE_ITEM_H
#define ABSTRACT_TREE_ITEM_H

#include "QtilitiesCoreGui_global.h"

#include <QtilitiesCore>
using namespace QtilitiesCore;

#include <QIcon>
#include <QBrush>
#include <QSize>
#include <QFont>

using namespace Qtilities::Core::Interfaces;

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        class TreeNode;

        /*!
        \struct AbstractTreeItemData
        \brief Structure used by AbstractTreeItem to store private data.
          */
        struct AbstractTreeItemData {
            AbstractTreeItemData() : is_exportable(true) { }

            bool is_exportable;
        };

        /*!
          \class AbstractTreeItem
          \brief The AbstractTreeItem is an abstract base class which can be used to build tree items.

          This class was added in %Qtilities v0.2.
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT AbstractTreeItem : virtual public IObjectBase
        {

        public:
            AbstractTreeItem();
            virtual ~AbstractTreeItem();

            //! Indicates if the formatting information of this tree item must be exported
            bool isFormattingExportable() const;
            //! Sets if the formatting information of this tree item must be exported.
            void setIsFormattingExportable(bool is_exportable);

            /*!
              This function will add a formatting node to \p object_node with the formatting information of this item.
              At present the following information is part of the formatting export:
              - Alignment
              - Background Color
              - Foreground Color
              - Size Hint
              - Status Tip
              - Tool Tip
              - Font

              The following information is not part of the formatting export:
              - Icon

              \note Category: The category is not part of the export. It is part of this item's item node.
              */
            IExportable::Result exportFormattingXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>()) const;
            IExportable::Result importFormattingXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>());

            // -------------------------------
            // Getter & Setter functions for object role properties
            // -------------------------------
            //! Sets the category of the tree item.
            void setCategory(const QtilitiesCategory& category, TreeNode* tree_node);
            //! Gets the category of the tree item.
            QtilitiesCategory getCategory(TreeNode* tree_node) const;
            //! Checks if the item has a category.
            bool hasCategory() const;
            //! Sets the tool tip of the tree item.
            void setToolTip(const QString& tooltip);
            //! Gets the tool tip of the tree item.
            QString getToolTip() const;
            //! Checks if the item has a tool tip.
            bool hasToolTip() const;
            //! Sets the icon of the tree item.
            void setIcon(const QIcon& icon);
            //! Gets the icon of the tree item.
            QIcon getIcon() const;
            //! Checks if the item has an icon.
            bool hasIcon() const;
            //! Sets the what's this text of the tree item.
            void setWhatsThis(const QString& whats_this);
            //! Gets the what's this text of the tree item.
            QString getWhatsThis() const;
            //! Checks if the item has what's this text.
            bool hasWhatsThis() const;
            //! Sets the status tip of the tree item.
            void setStatusTip(const QString& status_tip);
            //! Gets the status tip of the tree item.
            QString getStatusTip() const;
            //! Checks if the item has status tip text.
            bool hasStatusTip() const;
            //! Sets the size hint of the tree item.
            void setSizeHint(const QSize& size);
            //! Gets the size hint of the tree item.
            QSize getSizeHint() const;
            //! Checks if the item has a size hint.
            bool hasSizeHint() const;
            //! Sets the font of the tree item.
            void setFont(const QFont& font);
            //! Gets the font of the tree item.
            QFont getFont() const;
            //! Checks if the item has a font.
            bool hasFont() const;
            //! Sets the alignment of the tree item.
            void setAlignment(const Qt::AlignmentFlag& alignment);
            //! Gets the alignement of the tree item.
            Qt::AlignmentFlag getAlignment() const;
            //! Checks if the item has a font.
            bool hasAlignment() const;
            //! Sets the foreground role of the tree item.
            void setForegroundRole(const QBrush& foreground_role);
            //! Gets the foreground role of the tree item.
            QBrush getForegroundRole() const;
            //! Checks if the item has a foreground role.
            bool hasForegroundRole() const;
            //! Sets the background role of the tree item.
            void setBackgroundRole(const QBrush& background_role);
            //! Gets the background role of the tree item.
            QBrush getBackgroundRole() const;
            //! Checks if the item has a background role.
            bool hasBackgroundRole() const;

        protected:
            AbstractTreeItemData* baseItemData;
        };
    }
}

#endif //  ABSTRACT_TREE_ITEM_H
