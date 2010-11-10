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

#ifndef OBSERVERDOTGRAPH_H
#define OBSERVERHINTS_H

#include "QtilitiesCore_global.h"
#include "Observer.h"

#include <QObject>

namespace Qtilities {
    namespace Core {
        /*!
        \struct ObserverDotGraphData
        \brief Structure used by ObserverDotGraph to store private data.
          */
        struct ObserverDotGraphData;

        /*!
          \class ObserverDotGraph
          \brief The ObserverDotGraph class generate scripts to create \p dot graphs for an observer hierarhcy.

          The ObserverDotGraph class is used to generate scripts in the \p dot language which can be parsed by
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
ObserverDotGraph dotGraph(node);
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
        Different layout engines are available for the scripts created by ObserverDotGraph and here we will only show a couple of them.
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
ObserverDotGraph dotGraph(node);
dotGraph.generateDotScript();
dotGraph.saveToFile(QApplication::applicationDirPath() + "/output_file.gv");
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

        The ObserverDotGraph class allows complete customization of the graph it creates. Before looking at the different
        ways to customize a graph, a short overview of the parts of the graph to which properties can be applided
        language must be given.
        - The graph itself
        - Nodes in the graph
        - Connections between nodes

        The properties that can be applied to each of these are detailed in the <a href="http://www.graphviz.org/Documentation/dotguide.pdf">The Dot Guide</a> and
        will not be repeated here. For all properties the defaults of the \p dot language is used since ObserverDotGraph does
        not add them to the script if they are not specified. The only aspect of the graph that is set explicitly is
        the type of graph: undirected or directed.

          */
        class QTILIITES_CORE_SHARED_EXPORT ObserverDotGraph : public QObject, public ObserverAwareBase
        {
            Q_OBJECT
            Q_ENUMS(GraphType)

        public:
            //! Default constructor
            /*!
               \param parent The parent of the observer hint object. If the parent is an observer, setObserverContext() will automatically
                be called using the parent.
              */
            ObserverDotGraph(Observer* observer = 0);
            //! Copy constructor.
            ObserverDotGraph(const ObserverDotGraph& other);
            //! Overloaded = operator.
            void operator=(const ObserverDotGraph& other);
            //! Destructor.
            virtual ~ObserverDotGraph();

            //! The possible graph types.
            /*!
              Default is Directed.

              \sa setGraphType(), graphType()
              */
            enum GraphType {
                Directed =    0, /*!< Directed graph. */
                Undirected =  1  /*!< Undirected graph. */
            };

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



        private:
            ObserverDotGraphData* d;
        };

    }
}

#endif // OBSERVERHINTS_H
