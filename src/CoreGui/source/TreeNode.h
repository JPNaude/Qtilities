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

#ifndef TREE_NODE_H
#define TREE_NODE_H

#include "QtilitiesCoreGui_global.h"
#include "AbstractTreeItem.h"
#include "TreeItem.h"
#include "NamingPolicyFilter.h"

#include <Observer>
#include <ObserverHints>
#include <ActivityPolicyFilter>
#include <IExportableFormatting>
#include <QtilitiesCore>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct TreeNodePrivateData
        \brief Structure used by TreeNode to store private data.
          */
        struct TreeNodePrivateData;

        /*!
          \class TreeNode
          \brief The TreeNode class is a node in a tree which can have items attached to it and can also be attached to other nodes.

          The TreeNode class represents a node in a tree. That is, it is an item in the tree to which other items or other nodes can be
          attached/added. The node inherits Observer and therefore provides all the functionality of the
          Observer class such as installation of subject filters etc.

          All items in a TreeNode is attached to the Qtilities::Core::Observer subclass using Qtilities::Core::Observer::ObserverScopeOwnership. Thus
          when an item appears once in a tree it will be deleted when removed from the TreeNode. However if an item
          appears multiple times, it will not be deleted when removed from a single TreeNode. This way trees build using
          TreeNode and TreeItemBase based objects manages the lifetimes of all items in the tree automatically and cleans up after the
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

// Create an observer widget showing the tree:
TreeWidget* tree_widget = new TreeWidget(nodeA);
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
nodeB->addItem("Item 2")->setIcon(QIcon(qti_icon_EDIT_COPY_16x16));
nodeB->addItem("Item 3")->setIcon(QIcon(qti_icon_EDIT_CLEAR_16x16));
nodeB->addItem("Item 4")->setIcon(QIcon(qti_icon_EDIT_CUT_16x16));
nodeC->addItem("Item 5")->setWhatsThis("Hello, I'm a What's This message.");
nodeC->addItem("Item 6")->setToolTip("Hello, I'm a ToolTip text");
nodeC->addItem("Item 7")->setCategory(QtilitiesCategory("Category 1"),nodeC);

// Create an observer widget showing the tree:
TreeWidget* tree_widget = new TreeWidget;
QtilitiesApplication::setMainWindow(tree_widget);
tree_widget->setObserverContext(nodeA);
tree_widget->initialize();
tree_widget->show();
\endcode

          The above example will show a formatted tree as shown below:
          \image html trees_simple_example_formatted.jpg "Simple Tree Example With Formatting"

          \section tree_node_other_features Other features.

          The TreeNode class provides the enableNamingControl() and enableActivityControl() functions which
          allows control over names and activity without having to create, set up and install subject
          filters manually. Even if you did not call enableNamingControl() or enableActivityControl() and you
          installed such filters manually on the Observer base class, you can access these filters using
          namingPolicyFilter() or activityPolicyFilter().

          To get the number of tree items in the complete tree under this node, use treeCount(). The
          treeChildren() function returns a list of all the tree items under this node and we can look for a specific
          child using treeContains() function. A specific child can be accessed from the treeChildren() list using
          treeAt().

          It is possible to iterate through the items in a tree using Qtilities::Core::TreeIterator.

          <i>This class was added in %Qtilities v0.2.</i>
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TreeNode : public Observer, public AbstractTreeItem, public IExportableFormatting
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportableFormatting)
            Q_PROPERTY(QFont Font READ getFont WRITE setFont)
            Q_PROPERTY(QColor ForegroundRole READ getForegroundColor WRITE setForegroundColor)
            Q_PROPERTY(QColor BackgroundRole READ getBackgroundColor WRITE setBackgroundColor)
            Q_PROPERTY(QSize Size READ getSizeHint WRITE setSizeHint)
            Q_PROPERTY(QString StatusTip READ getStatusTip WRITE setStatusTip)
            Q_PROPERTY(QString ToolTip READ getToolTip WRITE setToolTip)
            Q_PROPERTY(QString WhatsThis READ getWhatsThis WRITE setWhatsThis)

        public:
            TreeNode(const QString& name = QString(), QObject* parent = 0);
            virtual ~TreeNode();

            // --------------------------------
            // IExportableFormatting Implementation
            // --------------------------------
            IExportable::ExportResultFlags exportFormattingXML(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version) const;
            IExportable::ExportResultFlags importFormattingXML(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version);

            // --------------------------------
            // AbstractTreeItem virtual overwrites
            // --------------------------------
            bool setCategory(const QtilitiesCategory& category, TreeNode* tree_node);
            bool setCategory(const QtilitiesCategory& category, int observer_id = -1);

            // --------------------------------
            // TreeNode Implementation
            // --------------------------------
            //! Enables categorized display on this node.
            /*!
              This is a convenience function which sets the Qtilities::Core::ObserverHints::CategorizedHierarchy hint on
              this node's observer base class.

              \param category_editing_flags The Qtilities::Core::ObserverHints::CategoryEditingFlags that must be used on category nodes. By default ObserverHints::CategoriesNoEditingFlags which
              will respect the current editing flags set on the displayHints() of the tree node.

              \note This function does not refresh the view and it sets the modification state of the node to modified.

              \sa getCategorizedDisplayEnabled(), disableCategorizedDisplay()
              */
            void enableCategorizedDisplay(ObserverHints::CategoryEditingFlags category_editing_flags = ObserverHints::CategoriesNoEditingFlags);
            //! Disables categorized display on this node.
            /*!
              \sa getCategorizedDisplayEnabled(), enableCategorizedDisplay();
              */
            void disableCategorizedDisplay();
            //! Function indicating if categorized display is used on this tree node.
            /*!
              \sa enableCategorizedDisplay();, enableCategorizedDisplay();
              */
            bool getCategorizedDisplayEnabled() const;    

            //! Convenience function to set up naming control on this tree node.
            /*!
              This function will create a Qtilities::CoreGui::NamingPolicyFilter with the specified
              parameters and install it on the tree node. It will also set the correct parameters on the
              Observer base class's observer hints.

              This function must be called only once, after the construction of your tree node before
              any tree items have been attached to the node. Calling it a second time will just return
              the already constructed filter.

              \param naming_control The ObserverHints::NamingControl hint to use in this tree node.
              \param uniqueness_policy The NamingPolicyFilter::UniquenessPolicy to use in this tree node.
              \param uniqueness_resolution_policy The NamingPolicyFilter::ResolutionPolicy to use in for duplicate items under this tree node.
              \param validity_resolution_policy The NamingPolicyFilter::ResolutionPolicy to use in for invalid items under this tree node.
              \returns The naming policy filter used in this node. If the filter could not be constructed, null is returned.
              */
            NamingPolicyFilter* enableNamingControl(ObserverHints::NamingControl naming_control,
                                     NamingPolicyFilter::UniquenessPolicy uniqueness_policy,
                                     NamingPolicyFilter::ResolutionPolicy uniqueness_resolution_policy = NamingPolicyFilter::PromptUser,
                                     NamingPolicyFilter::ResolutionPolicy validity_resolution_policy = NamingPolicyFilter::PromptUser);
            //! Disables naming control in this node.
            void disableNamingControl();
            //! Function which returns a reference to the naming control filter used by this node, if any.
            NamingPolicyFilter* namingPolicyFilter() const;

            //! Convenience function to set up activity control on this tree node.
            /*!
              This function will create a Qtilities::Core::ActivityPolicyFilter with the specified
              parameters and install it on the tree node. It will also set the correct parameters on the
              Observer base class's observer hints.

              This function must be called only once, after the construction of your tree node before
              any tree items have been attached to the node. Calling it a second time will just return
              the already constructed filter.

              \param activity_display The ObserverHints::ActivityDisplay hint to use in this tree node.
              \param activity_control The ObserverHints::ActivityControl hint to use in this tree node.
              \param activity_policy The ActivityPolicyFilter::ActivityPolicy hint to use in this tree node.
              \param parent_tracking_policy The ActivityPolicyFilter::ParentTrackingPolicy hint to use in this tree node.
              \param minimum_activity_policy The ActivityPolicyFilter::MinimumActivityPolicy hint to use in this tree node.
              \param new_subject_activity_policy The ActivityPolicyFilter::NewSubjectActivityPolicy hint to use in this tree node.
              \param monitor_modification_state When true the modification state of the activity filter will be monitored by the node. Thus if the active subjects changes the TreeNode will indicate that it changed.
              \returns The activity policy filter used in this node. If the filter could not be constructed, null is returned.
              */
            ActivityPolicyFilter* enableActivityControl(ObserverHints::ActivityDisplay activity_display,
                                       ObserverHints::ActivityControl activity_control = ObserverHints::NoActivityControlHint,
                                       ActivityPolicyFilter::ActivityPolicy activity_policy = ActivityPolicyFilter::MultipleActivity,
                                       ActivityPolicyFilter::ParentTrackingPolicy parent_tracking_policy = ActivityPolicyFilter::ParentIgnoreActivity,
                                       ActivityPolicyFilter::MinimumActivityPolicy minimum_activity_policy = ActivityPolicyFilter::AllowNoneActive,
                                       ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy = ActivityPolicyFilter::SetNewInactive,
                                       bool monitor_modification_state = true);
            //! Disables actvity control in this node.
            void disableActivityControl();
            //! Function which returns a reference to the activity control filter used by this node, if any.
            ActivityPolicyFilter* activityPolicyFilter() const;

            //! Convenience function to set up subject type on this tree node.
            /*!
              This function will create a Qtilities::Core::SubjectTypeFilter with the specified
              parameters and install it on the tree node. It is usefull to use when you want to give a name for
              items under a context. Observer widgets use this name in it's header for the name column.

              This function must be called only once, after the construction of your tree node before
              any tree items have been attached to the node. Calling it a second time will just return
              the already constructed filter.

              \note This function will enable inversed filtering on the subject type filter.
              */
            SubjectTypeFilter* setChildType(const QString& child_group_name);
            //! Disables naming control in this node.
            void clearChildType();
            //! Function which returns a reference to the naming control filter used by this node, if any.
            SubjectTypeFilter* subjectTypeFilter() const;

            /*!
              This overloaded function will also start a naming validation cycle on the naming policy filter if this node has one.
              */
            void startProcessingCycle();
            /*!
              This overloaded function will also end a naming validation cycle on the naming policy filter if this node has one.
              */
            void endProcessingCycle(bool broadcast = true);
            /*!
              This overloaded function will also start a naming validation cycle on the naming policy filter for tree nodes which has ones.
              */
            void startTreeProcessingCycle();
            /*!
              This overloaded function will also end a naming validation cycle on the naming policy filter for tree nodes which has ones.
              */
            void endTreeProcessingCycle(bool broadcast = true);

            //! Creates a new tree item and then add it as a tree item under this node.
            /*!
              \param name The name of the item.
              \param category The category of the item. By default QtilitiesCategory(), thus it does not specify a category.
              \returns The reference to the TreeItem created.

              \note This tree node will manage the lifetime of the item and this node can be obtained through the normal QObject parent() function.
              */
            TreeItem* addItem(const QString& name, const QtilitiesCategory& category = QtilitiesCategory());
            //! Creates new tree items and then add them under this node.
            /*!
              \param names The names of the items to add.
              \param category The category of the items. By default QtilitiesCategory(), thus it does not specify a category.

              \note This tree node will manage the lifetime of the items and this node can be obtained through the normal QObject parent() function.
              */
            void addItems(const QStringList& items, const QtilitiesCategory& category = QtilitiesCategory());
            //! Creates a new tree node and then add it as a tree item under this node.
            /*!
              \param name The name of the node.
              \param category The category of the item. By default QtilitiesCategory(), thus it does not specify a category.
              \returns The reference to the TreeNode created.

              \note This tree node will manage the lifetime of the item and this node can be obtained through the normal QObject parent() function.
              */
            TreeNode* addNode(const QString& name, const QtilitiesCategory& category = QtilitiesCategory());
            //! Adds a tree item under this node.
            /*!
              \param item The reference to the item to be added to this tree.
              \param category The category of the item. By default QtilitiesCategory(), thus it does not specify a category.
              \returns True if the item was attached successfully, false otherwise.

              \note This tree node will manage the lifetime of the tree item.
              */
            bool addItem(TreeItemBase* item, const QtilitiesCategory& category = QtilitiesCategory());
            //! Adds a tree mode under this node.
            /*!
              \param node The reference to the node to be added to this tree.
              \param category The category of the item. By default QtilitiesCategory(), thus it does not specify a category.
              \returns True if the node was attached successfully, false otherwise.

              \note This tree node will manage the lifetime of the tree item.
              */
            bool addNode(TreeNode* node, const QtilitiesCategory& category = QtilitiesCategory());
            //! Adds a tree mode under this from a file.
            /*!
              \param file_name The file name to be parsed.
              \param category The category of the item. By default QtilitiesCategory(), thus it does not specify a category.
              \param errorMessage If this function returns false the reason why it failed is available through this parameter if a valid QString reference is provided.
              \returns True if the node was attached successfully, false otherwise.

              \note This tree node will manage the lifetime of the node item.
              */
            bool addNodeFromFile(QString file_name, const QtilitiesCategory& category = QtilitiesCategory(), QString* errorMsg = 0);
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
            bool removeItem(TreeItemBase* item);
            //! Creates a new tree node and then add it as a tree item under this node.
            /*!
              \param item The reference to the node to be removed from this tree.
              \returns True if the node was removed successfully, false otherwise.
              */
            bool removeNode(TreeItemBase* node);

            //! Saves the tree under this tree node to an XML file.
            /*!
              \param file_name The file name from to which the file must be saved.
              \param errorMessage If this function returns IExportable::Failed the reason why it failed is available through this parameter if a valid QString reference is provided.
              \returns Result of the operation.

              \sa loadFromFile()
              */
            IExportable::ExportResultFlags saveToFile(const QString& file_name, QString* errorMsg = 0, ObserverData::ExportItemFlags export_flags = ObserverData::ExportData) const;
            //! Loads the tree under this tree node from an XML file.
            /*!
              \param file_name The file name from which the tree must be loaded.
              \param clear_first When true, all items under this tree is deleted first. When false, the items in the input file will be added to the current set of items under this tree node.
              \param errorMessage If this function returns IExportable::Failed the reason why it failed is available through this parameter if a valid QString reference is provided.
              \returns Result of the operation.

              \sa saveToFile()
              */
            IExportable::ExportResultFlags loadFromFile(const QString& file_name, QString* errorMsg = 0, bool clear_first = true);

//            //! Saves the tree under this tree node to an XML file.
//            /*!
//              \param file_name The file name from to which the file must be saved.
//              \param errorMessage If this function returns IExportable::Failed the reason why it failed is available through this parameter if a valid QString reference is provided.
//              \returns Result of the operation.

//              \sa loadFromString()
//              */
//            IExportable::ExportResultFlags saveToString(QString* target_string, QString* errorMsg = 0, ObserverData::ExportItemFlags export_flags = ObserverData::ExportData) const;
//            //! Loads the tree under this tree node from an XML file.
//            /*!
//              \param file_name The file name from which the tree must be loaded.
//              \param clear_first When true, all items under this tree is deleted first. When false, the items in the input file will be added to the current set of items under this tree node.
//              \param errorMessage If this function returns IExportable::Failed the reason why it failed is available through this parameter if a valid QString reference is provided.
//              \returns Result of the operation.

//              \sa saveToString()
//              */
//            IExportable::ExportResultFlags loadFromString(QString* target_string, QString* errorMsg = 0, bool clear_first = true);

            // --------------------------------
            // AbstractTreeItem Implementation
            // --------------------------------
            QObject* getTreeItemObjectBase() { return this; }
            const QObject* getTreeItemObjectBase() const { return this; }

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, TreeNode> factory;

        protected:
            TreeNodePrivateData* nodeData;
        };
    }
}

#endif //  TREE_NODE_H
