#ifndef LAYOUTUTILITY_H
#define LAYOUTUTILITY_H
#include <GraphLayoutLibrary_global.h>
#include <Common/BoostGraphWrapper.h>
#include <QDebug>


#include <boost/graph/depth_first_search.hpp>
#include <GridBasedLayout/GridBasedLayout.h>

/**
 * @brief The LayoutUtility class
 *
 * The class provides layout related utilities.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayoutUtility
{

public:
    typedef adjacency_list
        < listS,
          vecS,
          undirectedS,
          property<vertex_index_t, int>,
          property<edge_index_t, int>
        >
        graph; /*!< user defined type to represent non-weighted graph based on boost adjecency list */
    typedef adjacency_list
        < listS,
          vecS,
          undirectedS,//derectedS
          property<vertex_index_t, int>,//no_property,
          property <  edge_weight_t, int, property<edge_index_t, int> >
        >
        dijkstra_graph; /*!< user defined type to represent weighted graph based on boost adjecency list; used in Dijkstra's algorithm */
	
	BoostGraphWrapper m_GraphWrapper; /*!< Instance of BoostGraphWrapper class */
    int m_iMargin; /*!< Margin to used in layouts */

public:
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type LayoutUtility.
  */
 //@{
	/**
     Constructs new object of type LayoutUtility.
    @pre none

    @param none

    @return none

    @throw none
    */
    LayoutUtility();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of LayoutUtility.
  */
 //@{
    void setCompartment(SubGraph &gSubgraph , SubGraph& gMainGraph);
    void setCompartmentTest(SubGraph &gSubgraph);

    /**
     * @brief reinitializeEdgeIndices
     *
     * Reinitializes the edge indices of a graph of type 'Subgraph'.
     *
     * @param gInputSubGraph
     *          object of Subgraph.
     *
     * @return none
     *
     * @throw none
     */
    void reinitializeEdgeIndices(SubGraph & gInputSubGraph);

    /**
     * @brief reinitializeEdgeIndices
     *
     * Reinitializes the edge indices of a graph of type 'graph'.
     *
     * @param gInputGraph
     *          object of graph.
     *
     * @return none
     *
     * @throw none
     */
    void reinitializeEdgeIndices(graph & gInputGraph);

    /**
     * @brief reinitializeEdgeIndices
     *
     * Reinitializes the edge indices of a graph of type 'dijkstra_graph'.
     *
     * @param gInputDijkGraph
     *          object of gInputDijkGraph.
     *
     * @return none
     *
     * @throw none
     */
    void reinitializeEdgeIndices(dijkstra_graph & gInputDijkGraph);

//    void copyCompartmentProperties(SubGraph & gCopyGraph,
//                                   QMap<int, SubGraph *> &mapClusterIdToChild);

//@}
};

#endif // LAYOUTUTILITY_H
