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

#ifndef OBSERVERDOTGRAPH_H
#define OBSERVERDOTGRAPH_H

#include "QtilitiesCore_global.h"

#include <QObject>
#include <QString>
#include <QHash>

namespace Qtilities {
    namespace Core {  
        class Observer;

        /*!
        \struct ObserverDotWriterPrivateData
        \brief Structure used by ObserverDotWriter to store private data.
          */
        struct ObserverDotWriterPrivateData;

        /*!
          \class ObserverDotWriter
          \brief The ObserverDotWriter class generate dot scripts to create \p dot graphs for an observer tree.

          The ObserverDotWriter class is used to generate scripts in the \p dot language which can be parsed by
          a number of program to create \p dot graphs. For an overview of the \p dot language, a good place to start
          is <a href="http://www.graphviz.org/Documentation/dotguide.pdf">The Dot Guide</a>. To find software
          which can generate graphs from \p dot scripts, a good place to start is <a href="http://www.graphviz.org/">Graphviz</a>.

          \section observer_dot_graph_creating_a_graph Creating A Graph
          To create a \p dot script for an observer tree is easy. For example:
\code
// Create tree structure:
TreeNode* node = new TreeNode("Root Node");
node->addItem("Item 1");
node->addItem("Item 2");
node->addItem("Item 3");

// Create a dot script for this tree:
ObserverDotWriter dotGraph(node);
dotGraph.generateDotScript();
dotGraph.saveToFile("output_file.gv");
\endcode

        The resulting \p dot script looks like this:
\code
digraph "Root Node" {
    0 [label="Root Node"];
    0 -> 1;
    0 -> 2;
    0 -> 3;
    1 [label="Item 1"];
    2 [label="Item 2"];
    3 [label="Item 3"];
}
\endcode

        Next we can run the script using the \p Gvedit application found in the \p GraphViz distribution to create graph.
        Different layout engines are available for the scripts created by ObserverDotWriter and here we will only show a couple of them.
        First the output of the \p dot layout engine is shown:
        \image html observer_dot_graph_example_tree_dot.jpg "Example Graph (Dot Layout Engine)"

        Next the output of the \p circo layout engine is shown:
        \image html observer_dot_graph_example_tree_circo.jpg "Example Graph (Circo Layout Engine)"

        Since tree items can be attached to multiple nodes, it is possible to create more complex graphs easily as well.
        Lets look at an example:

\code
// Create tree structure:
TreeNode* node = new TreeNode("Root Node");
TreeNode* nodeA = node->addNode("Node A");
TreeNode* nodeB = node->addNode("Node B");
nodeA->addItem("Item 1");
nodeA->addItem("Item 2");
TreeItem* sharedItem = nodeA->addItem("Shared Item");
nodeB->attachSubject(sharedItem);
nodeB->addItem("Item 3");
nodeB->addItem("Item 4");

// Create a dot script for this tree:
ObserverDotWriter dotGraph(node);
dotGraph.generateDotScript();
dotGraph.saveToFile(QtilitiesApplication::applicationSessionPath() + "/output_file.gv");
\endcode

        The resulting \p dot script looks like this:

\code
digraph "Root Node" {
    0 [label="Root Node"];
    0 -> 1;
    0 -> 5;
    1 [label="Node A"];
    1 -> 2;
    1 -> 3;
    1 -> 4;
    2 [label="Item 1"];
    3 [label="Item 2"];
    4 [label="Shared Item"];
    5 [label="Node B"];
    5 -> 4;
    5 -> 6;
    5 -> 7;
    6 [label="Item 3"];
    7 [label="Item 4"];
}
\endcode

        The graph produced by the \p dot layout engine is shown below:

        \image html observer_dot_graph_example_complex_tree_dot.jpg "Example Graph (Dot Layout Engine)"

        The same graph produced by the \p twopi layout engine is shown below:

        \image html observer_dot_graph_example_complex_tree_twopi.jpg "Example Graph (Twopi Layout Engine)"

        \section observer_dot_graph_customizing Customizing A Graph

        The ObserverDotWriter class allows complete customization of the graph it creates. Before looking at the different
        ways to customize a graph, a short overview of the parts of the graph to which properties can be applied must be given.
        - The graph itself
        - Nodes in the graph
        - Connections between nodes (edge attributes)

        The properties that can be applied to each of these are detailed in the <a href="http://www.graphviz.org/Documentation/dotguide.pdf">The Dot Guide</a> and
        will not be repeated here. For all properties the defaults of the \p dot language is used since ObserverDotWriter does
        not add them to the script if they are not specified.

        \subsection observer_dot_graph_attributes Graph Attributes

        It is possible to add any graph attribute specified in the dot language to a your graph. For example:

\code
// Create tree structure:
TreeNode* node = new TreeNode("Root Node");
node->addItem("Item 1");
node->addItem("Item 2");
node->addItem("Item 3");

// Create a dot script for this tree:
ObserverDotWriter dotGraph(node);
dotGraph.addGraphAttribute("label","Graph Title");
dotGraph.generateDotScript();
dotGraph.saveToFile("output_file.gv");
\endcode

        The resulting \p dot script looks like this:
\code
digraph "Root Node" {
    label = "Graph Title";
    0 [label="Root Node"];
    0 -> 1;
    0 -> 2;
    0 -> 3;
    1 [label="Item 1"];
    2 [label="Item 2"];
    3 [label="Item 3"];
}
\endcode

        \subsection observer_dot_graph_node_attributes Node Attributes

        It is possible to add any node attribute specified in the dot language to a node in your graph. For example:

\code
// Create tree structure:
TreeNode* node = new TreeNode("Root Node");
node->addItem("Item 1");
node->addItem("Item 2");
node->addItem("Item 3");

// Create a dot script for this tree:
ObserverDotWriter dotGraph(node);
dotGraph.addNodeAttribute(node,"color","red");
dotGraph.generateDotScript();
dotGraph.saveToFile("output_file.gv");
\endcode

        The resulting \p dot script looks like this:
\code
digraph "Root Node" {
    0 [label="Root Node" color="red"];
    0 -> 1;
    0 -> 2;
    0 -> 3;
    1 [label="Item 1"];
    2 [label="Item 2"];
    3 [label="Item 3"];
}
\endcode

        \subsection observer_dot_graph_edge_attributes Edge Attributes

        It is possible to add any edge attribute specified in the dot language between two nodes in your graph. For example:

\code
// Create tree structure:
TreeNode* node = new TreeNode("Root Node");
TreeItem* item1 = node->addItem("Item 1");
TreeItem* item2 = node->addItem("Item 2");
node->addItem("Item 3");

// Create a dot script for this tree:
ObserverDotWriter dotGraph(node);
dotGraph.addEdgeAttribute(node,item1,"label","\"My label\"");
dotGraph.addEdgeAttribute(node,item1,"style","bold");
dotGraph.addEdgeAttribute(node,item12,"color","red");
dotGraph.generateDotScript();
dotGraph.saveToFile("output_file.gv");
\endcode

        The resulting \p dot script looks like this:
\code
digraph "Root Node" {
    label = "Graph Title";
    0 [label="Root Node" color="red"];
    0 -> 1 [label="My label",style=bold];
    0 -> 2 [color=red];
    0 -> 3;
    1 [label="Item 1"];
    2 [label="Item 2"];
    3 [label="Item 3"];
}
\endcode

        Note the needed extra \p \" characters for the \p label attribute.

        Combining all of the different attributes that we've set above, we get a graph like this:

        \image html observer_dot_graph_example_attributes_dot.jpg "Example Graph With Attributes (Dot Layout Engine)"

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT ObserverDotWriter : public QObject
        {
            Q_OBJECT
            Q_ENUMS(GraphType)

        public:
            //! Default constructor
            /*!
               \param observer The observer for which the script must be generated. The observer will also become the parent of this class.
              */
            ObserverDotWriter(Observer* observer = 0);
            //! Copy constructor.
            ObserverDotWriter(const ObserverDotWriter& other);
            //! Overloaded = operator.
            ObserverDotWriter& operator=(const ObserverDotWriter& other);
            //! Destructor.
            virtual ~ObserverDotWriter();

            //! Sets the observer context.
            bool setObserverContext(Observer* observer);
            //! Gets a pointer to the observer context.
            Observer* observerContext() const;

            //! Saves the dot script to a file.
            /*!
              Function which will write the dot script generated using the generateDotScript() function to the specified file.

              \note If no observer context have been specified, this function will return false.
              */
            virtual bool saveToFile(const QString& fileName) const;
            //! Function which will generate the dot script for the specified observer context.
            /*!
              \note If no observer context have been specified, this function will return QString().
              */
            QString generateDotScript() const;

            //! Adds a node attribute to a node in the graph.
            /*!
              See <a href="http://www.graphviz.org/Documentation/dotguide.pdf">The Dot Guide</a> for a list of possible node attributes. Overwrites existing attribute with the same name if it exists. If the node you specify is not part of the observer tree (see observerContext()) then this function will fail and return false.

              \param node A pointer to the node.
              \param attribute The name of the attribute to be added.
              \param value The value of the attribute.

              \returns True if successfull, false otherwise.
              */
            bool addNodeAttribute(QObject* node, const QString& attribute, const QString& value);
            //! Removes a node attribute from a node in the graph.
            /*!
              \param node A pointer to the node.
              \param attribute The name of the attribute to be removed.

              \returns True if successfull, false otherwise (also when the property did not exist).
              */
            bool removeNodeAttribute(QObject* node, const QString& attribute);
            //! Returns a list of all attributes on a node in the graph.
            /*!
              \returns A QHash with attribute names a keys and their corresponding values as values.
              */
            QHash<QByteArray,QString> nodeAttributes(QObject* node) const;

            //! Adds an edge attribute between two nodes in the graph.
            /*!
              See <a href="http://www.graphviz.org/Documentation/dotguide.pdf">The Dot Guide</a> for a list of possible edge attributes. Overwrites existing attribute with the same name if it exists. If either of the nodes you specify are not part of the top level observer tree (see observerContext()) then this function will fail and return false.

              \param parent A pointer to the parent observer.
              \param child The child underneath the parent observer.
              \param attribute The name of the attribute to be added.
              \param value The value of the attribute.

              \returns True if successfull, false otherwise.
              */
            bool addEdgeAttribute(Observer* parent, QObject* child, const QString& attribute, const QString& value);
            //! Removes a node attribute from a node in the graph.
            /*!
              \param node A pointer to the node.
              \param attribute The name of the attribute to be removed.

              \returns True if successfull, false otherwise (also when the property did not exist).
              */
            bool removeEdgeAttribute(Observer* parent, QObject* child, const QString& attribute);
            //! Returns a list of all attributes on a node in the graph.
            /*!
              \returns A QHash with attribute names a keys and their corresponding values as values.
              */
            QHash<QByteArray,QString> edgeAttributes(Observer* parent, QObject* child) const;

            //! Adds a node attribute to a node in the graph.
            /*!
              See <a href="http://www.graphviz.org/Documentation/dotguide.pdf">The Dot Guide</a> for a list of possible graph attributes. Overwrites existing attribute with the same name if it exists.

              \param attribute The name of the attribute to be added.
              \param value The value of the attribute.

              \returns True if successfull, false otherwise.
              */
            bool addGraphAttribute(const QString& attribute, const QString& value);
            //! Removes a node attribute from a node in the graph.
            /*!
              \param attribute The name of the attribute to be removed.

              \returns True if successfull, false otherwise.
              */
            bool removeGraphAttribute(const QString& attribute);
            //! Returns a list of all attributes on a node in the graph.
            /*!
              \returns A QHash with attribute names a keys and their corresponding values as values.
              */
            QHash<QString,QString> graphAttributes() const;

        private:
            ObserverDotWriterPrivateData* d;
        };

    }
}

#endif // OBSERVERDOTGRAPH_H
