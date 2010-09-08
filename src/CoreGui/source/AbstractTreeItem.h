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
            AbstractTreeItemData() { }

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

            // -------------------------------
            // Getter & Setter functions for object role properties
            // -------------------------------
            //! Sets the category of the tree item.
            void setCategory(const QtilitiesCategory& category, TreeNode* tree_node);
            //! Gets the category of the tree item.
            QtilitiesCategory getCategory(TreeNode* tree_node);
            //! Checks if the item has a category.
            bool hasCategory();
            //! Sets the tool tip of the tree item.
            void setToolTip(const QString& tooltip);
            //! Gets the tool tip of the tree item.
            QString getToolTip();
            //! Checks if the item has a tool tip.
            bool hasToolTip();
            //! Sets the icon of the tree item.
            void setIcon(const QIcon& icon);
            //! Gets the icon of the tree item.
            QIcon getIcon();
            //! Checks if the item has an icon.
            bool hasIcon();
            //! Sets the what's this text of the tree item.
            void setWhatsThis(const QString& whats_this);
            //! Gets the what's this text of the tree item.
            QString getWhatsThis();
            //! Checks if the item has what's this text.
            bool hasWhatsThis();
            //! Sets the status tip of the tree item.
            void setStatusTip(const QString& status_tip);
            //! Gets the status tip of the tree item.
            QString getStatusTip();
            //! Checks if the item has status tip text.
            bool hasStatusTip();
            //! Sets the size hint of the tree item.
            void setSizeHint(const QSize& size);
            //! Gets the size hint of the tree item.
            QSize getSizeHint();
            //! Checks if the item has a size hint.
            bool hasSizeHint();
            //! Sets the font of the tree item.
            void setFont(const QFont& font);
            //! Gets the font of the tree item.
            QFont getFont();
            //! Checks if the item has a font.
            bool hasFont();
            //! Sets the alignment of the tree item.
            void setAlignment(const Qt::AlignmentFlag& alignment);
            //! Gets the alignement of the tree item.
            Qt::AlignmentFlag getAlignment();
            //! Checks if the item has a font.
            bool hasAlignment();
            //! Sets the foreground role of the tree item.
            void setForegroundRole(const QBrush& foreground_role);
            //! Gets the foreground role of the tree item.
            QBrush getForegroundRole();
            //! Checks if the item has a foreground role.
            bool hasForegroundRole();
            //! Sets the background role of the tree item.
            void setBackgroundRole(const QBrush& background_role);
            //! Gets the background role of the tree item.
            QBrush getBackgroundRole();
            //! Checks if the item has a background role.
            bool hasBackgroundRole();

        protected:
            AbstractTreeItemData* baseItemData;
        };
    }
}

#endif //  ABSTRACT_TREE_ITEM_H
