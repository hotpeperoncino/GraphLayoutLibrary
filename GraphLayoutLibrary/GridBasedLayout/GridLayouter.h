#ifndef GRIDLAYOUTER_H
#define GRIDLAYOUTER_H

#include <Common/BoostGraphWrapper.h>
#include <Common/LayoutEnum.h>
#include <LayoutManager/GraphLayoutErrorCodes.h>
#include <QMap>
#include <GraphLayoutLibrary_global.h>
#include <GridBasedLayout/GridBasedLayout.h>
#include <GridBasedLayout/GraphPreProcessor.h>
#include <ForceBasedLayout/Reingold.h>

/**
 * @brief The GridLayouter class
 *
 * A class to handle grid based layout algorithm to be applied on a clustred graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT GridLayouter
{
private:
    /**
     * @brief The enumVertexType enum to denote the type of vertex in list of edges to be added to NewGraph.
     */
    enum enumVertexType {ORIGINAL_VERTEX, NEW_VERTEX};
    /**
     * @brief The EdgeType enum to denote the type of edge in CurrentClusteredGraph with respect to current cluster.
     */
    enum EdgeType {COMMING_IN, GOING_OUT, NOT_INCIDENT, LIES_IN};
    /**
     * @brief The Node_Type_Pair struct to store a node in an edge that will replace edge in CurrentClusteredGraph.
     */
    struct Node_Type_Pair
    {
        VertexDescriptor vNode; /**< a vertex */
        enumVertexType vertexType; /**< type of the vertex */
    };
    /**
     * @brief edgeType type to define edge that will replace edge in CurrentClusteredGraph.
     */
    typedef std::vector<Node_Type_Pair> edgeType;

public:
	/** @name Creators
     *
	 *  The methods under this section are responsible for constructing
     *  an instance of type GridLayouter.
     */

    //@{
	/**
     Constructs new object of type GridLayouter.
    @pre none

    @param none

    @return none

    @throw none
    */
    GridLayouter();
	//@}

 /** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type GridLayouter.
 */
//@{
    /**
      This method reads a clustered graph, applies grid based layout on the clustered graph.
      It works on the clusters recursively in bootom-up way. Innermost clusters will be layouted first
      and the the obtained layout is put in the parent cluster and so on.
      The layout is also reflected in RootClusteredGraph.

     * @brief gridLayouterForClusteredGraphProcess
     *          applies Grid Based Layout to passed graph taking its clusters into consideration.
     *
     * @param gCurrentClusteredGraph
     *          graph to be layed out.
     *
     * @param gRootClusteredGraph
     *          root graph of the CurrentClusteredGraph.
     *
     * @return none
     *
     * @throw LayoutException
     */
    void gridLayouterForClusteredGraph(SubGraph & gCurrentClusteredGraph,
                                       SubGraph & gRootClusteredGraph);

    /**
      This method reads a non-clustered graph, applies grid based layout on the graph.
      It simply ignores the clusters in the graph if any are present and treats the graph as non-clustered.

     * @brief gridLayouterForNonClusteredGraphProcess
     *          applies Grid Based Layout to passed graph without taking its clusters into consideration.
     *
     * @param gCurrentNonClusteredGraph
     *          graph to be layed out.
     *
     * @return none
     *
     * @throw LayoutException
     */
    void gridLayouterForNonClusteredGraph(SubGraph & gCurrentNonClusteredGraph);
//@}

private:
    /**
      This method adds vertices to the NewGraph. Vertices are added for all vertices and clusteres
      at current level of CurrentClustered graph. NewGraph will not contain any cluster.
      It also saves Maps from vertices of NewGraph to their corresponding vertices or clusters
      in CurrentClusteredGraph.

     * @brief addVerticesToNewGraphProcess
     *          adds vertices to NewGraph corresponding to vertices and clusters of CurrentClusteredGraph.
     *
     * @param gCurrentNonClusteredGraph
     *          input graph from which the vertices are to be added to NewGraph.
     *
     * @param gNewGraph
     *          graph to which the vertices are to be added.
     *
     * @param gRootClusteredGraph
     *          root graph of the CurrentClusteredGraph.
     *
     * @param isCluster
     *          map from vertex of NewGraph to a flag that specifies whether that vertex corresponds to
     *          a vertex or a cluster in CurrentClusteredGraph.
     *
     * @param originalVertexForNewVertex
     *          map from vertex of NewGraph to its corresponding vertex in CurrentClusteredGraph.
     *
     * @param childrenList
     *          map from vertex of NewGraph to its corresponding cluster in CurrentClusteredGraph.
     *
     * @return none
     *
     * @throw LayoutException
     */
    void addVerticesToNewGraph(SubGraph & gCurrentClusteredGraph, SubGraph & gNewGraph,
                               SubGraph & gRootClusteredGraph,
                               QMap <int, bool> & isCluster,
                               QMap <int, VertexIterator> & originalVertexForNewVertex,
                               QMap <int, ChildrenIterator> & childrenList);

    /**
      This method finds the edges from CurrentClusteredGraph that should not be added to NewGraph
      and the edges that will replace these edges if any. It saves the list of edges that are to be removed
      while transfering to NewGraph and corresponding edges to be added to NewGraph if any.

     * @brief findEdgesToBeAddedRemovedProcess
     *          finds edges to be removed from CurrentClusteredGraph and to be added to NewGraph.
     *
     * @param gCurrentNonClusteredGraph
     *          input graph from which the vertices are to be added to NewGraph.
     *
     * @param gNewGraph
     *          graph to which the vertices are to be added.
     *
     * @param oEdgesToRemove
     *          vector that will store edges that should be replaced by new edges in NewGraph.
     *
     * @param oEdgesToAdd
     *          vector that will store edges that should replace edges from CurrentClusteredGraph.
     *
     * @param oInnerEdgesToRemove
     *          vector that will store edges that should not be added in NewGraph.
     *
     * @param childrenList
     *          map from vertex of NewGraph to its corresponding cluster in CurrentClusteredGraph.
     *
     * @return none
     *
     * @throw LayoutException
     */
    void findEdgesToBeAddedRemoved(SubGraph & gCurrentClusteredGraph, SubGraph & gNewGraph,
                                   std::vector<GraphPreProcessor::VertexOrderType> & oEdgesToRemove,
                                   std::vector<GridLayouter::edgeType> & oEdgesToAdd,
                                   std::vector<GraphPreProcessor::VertexOrderType> & oInnerEdgesToRemove,
                                   QMap <int, ChildrenIterator> & childrenList);

    /**
      This method reads the edges from CurrentClusteredGraph that should not be added to NewGraph
      and the edges that will replace these edges if any. It removes or replaces these removed edges by new edges.

     * @brief addEdgesToNewGraphProcess
     *          adds edges to NewGraph corresponding to edges in CurrentClusteredGraph.
     *
     * @param gCurrentNonClusteredGraph
     *          input graph from which the vertices are to be added to NewGraph.
     *
     * @param gNewGraph
     *          graph to which the vertices are to be added.
     *
     * @param oEdgesToRemove
     *          vector that stores edges that should be replaced by new edges in NewGraph.
     *
     * @param oEdgesToAdd
     *          vector that stores edges that should replace edges from CurrentClusteredGraph.
     *
     * @param oInnerEdgesToRemove
     *          vector that stores edges that should not be added in NewGraph.
     *
     * @param originalVertexForNewVertex
     *          map from vertex of NewGraph to its corresponding vertex in CurrentClusteredGraph.
     *
     * @return none
     *
     * @throw LayoutException
     */
    void addEdgesToNewGraph(SubGraph & gCurrentClusteredGraph, SubGraph & gNewGraph,
                            std::vector<GraphPreProcessor::VertexOrderType> & oEdgesToRemove,
                            std::vector<GridLayouter::edgeType> & oEdgesToAdd,
                            std::vector<GraphPreProcessor::VertexOrderType> & oInnerEdgesToRemove,
                            QMap <int, VertexIterator> & originalVertexForNewVertex);

    /**
      This method reads clustered CurrentClusteredGraph and its corresponding non-clustered NewGraph
      and transfers the layout of NewGraph to CurrentClusteredGraph. It puts the clusters at locations of
      theis corresponding vertices in NewGraph so that the layout is maintained.

     * @brief copyBackCoordinatesProcess
     *          copies the layout of NewGraph to CurrentClusteredGraph.
     *
     * @param gCurrentNonClusteredGraph
     *          input graph from which the vertices are to be added to NewGraph.
     *
     * @param gNewGraph
     *          graph to which the vertices are to be added.
     *
     * @param gRootClusteredGraph
     *          root graph of the CurrentClusteredGraph.
     *
     * @param isCluster
     *          map from vertex of NewGraph to a flag that specifies whether that vertex corresponds to
     *          a vertex or a cluster in CurrentClusteredGraph.
     *
     * @param originalVertexForNewVertex
     *          map from vertex of NewGraph to its corresponding vertex in CurrentClusteredGraph.
     *
     * @param childrenList
     *          map from vertex of NewGraph to its corresponding cluster in CurrentClusteredGraph.
     *
     * @return none
     *
     * @throw LayoutException
     */
    void copyBackCoordinates(SubGraph &gCurrentClusteredGraph, SubGraph &gNewGraph,
                             SubGraph &gRootClusteredGraph,
                             QMap<int, bool> &isCluster,
                             QMap<int, VertexIterator> &originalVertexForNewVertex,
                             QMap<int, ChildrenIterator> &childrenList);

};

#endif // GRIDLAYOUTER_H
