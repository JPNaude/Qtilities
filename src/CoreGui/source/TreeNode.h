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

#ifndef TREE_NODE_H
#define TREE_NODE_H

#include "QtilitiesCoreGui_global.h"
#include "AbstractTreeItem.h"
#include "TreeItem.h"
#include "NamingPolicyFilter.h"
#include "ActivityPolicyFilter.h"

#include <Observer>
#include <ObserverHints>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct TreeNodeData
        \brief Structure used by TreeNode to store private data.
          */
        struct TreeNodeData;

        /*!
          \class TreeNode
          \brief The TreeNode class is a node in a tree which can have items attached to it and can also be attached to other nodes.

          The TreeNode class is a node in a tree. That is, it is an item in the tree and other items or other nodes can be
          attached/added underneath it. The node inherits Observer and therefore provides all the functionality of the
          Observer class such as installation of subject filters etc.

          All items in a TreeNode is attached to the Qtilities::Core::Observer subclass using Qtilities::Core::Observer::ObserverScopeOwnership. Thus
          when a item appears once in a tree it will be deleted when removed from the TreeNode. However if an item
          appears multiple times, it will not be deleted when removed from a single TreeNode. This way trees build using
          TreeNode and TreeItem manages the lifetimes of all items in the tree automatically and cleans up after the
          developer automatically.

          \section tree_node_building_trees Building trees using TreeNode and TreeItem
          The advantage of using the TreeNode class over a normal Qtilities::Core::Observer is that it provides convenience functions to
          easily manipulate the tree. For example, new tree items or nodes can be added to the tree simply by providing the
          name of the item to be created. Lets look at an example:

\code
// Create the tree nodes:
TreeNode* nodeA = new TreeNode("Node A");
TreeNode* nodeB = nodeA->addNode("Node B");
TreeNode* nodeC = nodeA->addNode("Node C");

// Create the tree items:
nodeA->addItem("Item 1");
nodeB->addItem("Item 2");
nodeB->addItem("Item 3");
nodeB->addItem("Item 4");
nodeC->addItem("Item 5");
nodeC->addItem("Item 6");
nodeC->addItem("Item 7");

// Create an observer widget wih the items:
ObserverWidget* tree_widget = new ObserverWidget();
tree_widget->setObserverContext(nodeA);
tree_widget->initialize();
tree_widget->show();
\endcode

          The above example will show the following widget:
          \image html trees_simple_example.jpg "Simple Tree Example"

          \section tree_node_formatting_trees Formatting items in the tree
          The second advantage is that TreeNode inherits AbstractTreeItem, thus we can easily provide role hints for the node,
          in the same whay that we can add role hints for TreeItem instances. Because the TreeNode class is intended to build
          trees very quickly mostly for display purposes, it automatically calls Observer::useDisplayHints() in the constructor.

          Lets look at an example where we set some hints on the items in the tree. The example above can be modified to look like this:
\code
// Create the tree nodes:
TreeNode* nodeA = new TreeNode("Node A");
TreeNode* nodeB = nodeA->addNode("Node B");
TreeNode* nodeC = nodeA->addNode("Node C");
nodeC->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);

// Add some formatting to the nodes:
nodeA->setForegroundRole(QBrush(Qt::darkRed));
nodeA->setFont(QFont("Helvetica [Cronyx]",20));
nodeA->setAlignment(Qt::AlignCenter);
nodeA->setBackgroundRole(QBrush(Qt::gray));
nodeB->setForegroundRole(QBrush(Qt::darkGreen));
nodeB->setFont(QFont("Helvetica [Cronyx]",20));
nodeB->setAlignment(Qt::AlignCenter);
nodeB->setBackgroundRole(QBrush(Qt::gray));
nodeC->setForegroundRole(QBrush(Qt::darkYellow));
nodeC->setFont(QFont("Helvetica [Cronyx]",20));
nodeC->setAlignment(Qt::AlignCenter);
nodeC->setBackgroundRole(QBrush(Qt::gray));

// Create the tree items:
// Be carefull to use it like this. If the addItem() call fails it returns 0.
nodeA->addItem("Item 1")->setStatusTip("Hello, I'm a status tip.");
nodeB->addItem("Item 2")->setIcon(QIcon(ICON_EDIT_COPY_16x16));
nodeB->addItem("Item 3")->setIcon(QIcon(ICON_EDIT_CLEAR_16x16));
nodeB->addItem("Item 4")->setIcon(QIcon(ICON_EDIT_CUT_16x16));
nodeC->addItem("Item 5")->setWhatsThis("Hello, I'm a What's This message.");
nodeC->addItem("Item 6")->setToolTip("Hello, I'm a ToolTip text");
nodeC->addItem("Item 7")->setCategory(QtilitiesCategory("Category 1"),nodeC);

// Create an observer widget wih the items:
ObserverWidget* tree_widget = new ObserverWidget();
QtilitiesApplication::setMainWindow(tree_widget);
tree_widget->setObserverContext(nodeA);
tree_widget->initialize();
tree_widget->show();
\endcode

          The above example will show a formatted tree as shown below:
          \image html trees_simple_example_formatted.jpg "Simple Tree Example With Formatting"

          \section tree_node_subject_filters Easy to control names and activity
          The TreeNode class provides the enableNamingControl() and enableActivityControl() functions which
          allows control over names and activity without having to create, set up and install subject
          filters manually.

          This class was added in %Qtilities v0.2.
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TreeNode : public Observer, public AbstractTreeItem
        {

        public:
            TreeNode(const QString& name = QString());
            virtual ~TreeNode();

            //! Convenience function to set up naming control on this tree node.
            /*!
              This function will create a Qtilities::CoreGui::NamingPolicyFilter with the specified
              parameters and install it on the tree node. It will also set the correct parameters on the
              Observer base class.

              This function must be called only once, after the construction of your tree node. Calling
              it a second time will do nothing.

              \param naming_control The ObserverHints::NamingControl hint to use in this tree node.
              \param uniqueness_policy The NamingPolicyFilter::UniquenessPolicy to use in this tree node.
              \param resolution_policy The NamingPolicyFilter::ResolutionPolicy to use in this tree node. This resolution policy will be used for NamingPolicyFilter::uniquenessResolutionPolicy() and validityResolutionPolicy().
              */
            void enableNamingControl(ObserverHints::NamingControl naming_control,
                                     NamingPolicyFilter::UniquenessPolicy uniqueness_policy,
                                     NamingPolicyFilter::ResolutionPolicy resolution_policy = NamingPolicyFilter::PromptUser);

            //! Convenience function to set up activity control on this tree node.
            /*!
              This function will create a Qtilities::Core::ActivityPolicyFilter with the specified
              parameters and install it on the tree node. It will also set the correct parameters on the
              Observer base class.

              This function must be called only once, after the construction of your tree node. Calling
              it a second time will do nothing.

              \param activity_display The ObserverHints::ActivityDisplay hint to use in this tree node.
              \param activity_control The ObserverHints::ActivityControl hint to use in this tree node.
              \param activity_policy The ActivityPolicyFilter::ActivityPolicy hint to use in this tree node.
              \param minimum_activity_policy The ActivityPolicyFilter::MinimumActivityPolicy hint to use in this tree node.
              \param new_subject_activity_policy The ActivityPolicyFilter::NewSubjectActivityPolicy hint to use in this tree node.
              */
            void enableActivityControl(ObserverHints::ActivityDisplay activity_display,
                                       ObserverHints::ActivityControl activity_control = ObserverHints::NoActivityControlHint,
                                       ActivityPolicyFilter::ActivityPolicy activity_policy = ActivityPolicyFilter::UniqueActivity,
                                       ActivityPolicyFilter::MinimumActivityPolicy minimum_activity_policy = ActivityPolicyFilter::ProhibitNoneActive,
                                       ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy = ActivityPolicyFilter::SetNewActive);

            /*!
              This overloaded function will start a naming validation cycle on the naming policy filter if enableNamingControl() called.
              */
            void startProcessingCycle();
            /*!
              This overloaded function will end a naming validation cycle on the naming policy filter if enableNamingControl() called.
              */
            void endProcessingCycle();

            //! Creates a new tree item and then add it as a tree item under this node.
            /*!
              \param name The name of the item.
              \returns The reference to the TreeItem created.

              \note This tree node will manage the lifetime of the new tree item.
              */
            TreeItem* addItem(const QString& name);
            //! Creates a new tree node and then add it as a tree item under this node.
            /*!
              \param name The name of the node.
              \returns The reference to the TreeNode created.

              \note This tree node will manage the lifetime of the new tree node.
              */
            TreeNode* addNode(const QString& name);
            //! Creates a new tree item and then add it as a tree item under this node.
            /*!
              \param item The reference to the item to be added to this tree.
              \returns True if the item was attached successfully, false otherwise.

              \note This tree node will manage the lifetime of the tree item.
              */
            bool addItem(TreeItem* item);
            //! Creates a new tree node and then add it as a tree item under this node.
            /*!
              \param node The reference to the node to be added to this tree.
              \returns True if the node was attached successfully, false otherwise.

              \note This tree node will manage the lifetime of the node item.
              */
            bool addNode(TreeNode* node);
            //! Removes the tree item or tree node specified by \p name from this tree node.
            /*!
              \param item The name of the tree item or tree node to be removed.
              \returns True if the item was removed successfully, false otherwise.

              \note Only use the name of the item directly if you are sure that items in your tree node has
              unique names. This can be enforced by installing a NamingPolicyFilter on the tree node. If
              you use this function on a TreeNode which does have multiple items with the same name, the
              first item found with the specified name will be removed.
              */
            bool removeItem(const QString& name);
            //! Removes the specified tree item from this node.
            /*!
              \param item The reference to the item to be removed from this tree.
              \returns True if the item was removed successfully, false otherwise.
              */
            bool removeItem(TreeItem* item);
            //! Creates a new tree node and then add it as a tree item under this node.
            /*!
              \param item The reference to the node to be removed from this tree.
              \returns True if the node was removed successfully, false otherwise.
              */
            bool removeNode(TreeNode* node);

            //! Saves the tree under this tree node to an XML file.
            /*!
              \param file_name The file name from to which the file must be saved.
              \param verbose_output When true, verbose output will be printed during saving to the file.
              \returns Result of the operation.
              */
            IExportable::Result saveToFile(const QString& file_name) const;
            //! Loads the tree under this tree node from an XML file.
            /*!
              \param file_name The file name from which the tree must be loaded.
              \param clear_first When true, all items under this tree is deleted first. When false, the items in the input file will be added to the current set of items under this tree node.
              \param verbose_output When true, verbose output will be printed during construction of the tree.
              \returns Result of the operation.
              */
            IExportable::Result loadFromFile(const QString& file_name, bool clear_first = true);

            // --------------------------------
            // IObjectBase Implemenation
            // --------------------------------
            QObject* objectBase() { return this; }

            // --------------------------------
            // Factory Interface Implemenation
            // --------------------------------
            static FactoryItem<QObject, TreeNode> factory;

        protected:
            TreeNodeData* nodeData;
        };
    }
}

#endif //  TREE_NODE_H
