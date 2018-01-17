#ifndef GRAPHCYCLEHANDLER_H
#define GRAPHCYCLEHANDLER_H

#include <GraphLayoutLibrary_global.h>
#include <Common/BoostGraphWrapper.h>
#include <boost/graph/depth_first_search.hpp>
#include <Common/CustomDFSVisitors.h>
#include <QDebug>
#include <QQueue>

/**
 * @brief The GraphCycleHandler class
 *
 * The class provides helpers to manage edge cycles in graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT GraphCycleHandler
{
private:
    BoostGraphWrapper m_BoostGraphWrapper;  /*!< Local instance of BoostGraphWrapper */

public:
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type GraphCycleHandler.
  */
 //@{
	/**
     Constructs new object of type GraphCycleHandler.
    @pre none

    @param none

    @return none

    @throw none
    */
    GraphCycleHandler();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of GraphCycleHandler.
  */
 //@{
	/** This function reverses the edge from given graph marks reverse edge property to true which might be helpful for the algorithm to know which edges are reversed

    @pre none

    @param gGraph
        reference to graph
    @param vectBackEdges
        reference to vector of back edges which are to be reversed

    @return none

    @throw boost graph exception

    */
    void reverseEdges(SubGraph& gGraph, VectorEdgeDescriptor& vectBackEdges);
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type GraphCycleHandler.
 */
//@{
    /** detectBackEdges function detects edge cycles using boost DFS and gives the list of back edges

    @pre
        -# gGraph != NULL

    @param gGraph
        reference to graph
    @param vectBackEdges
        reference to empty vector of edges

    @return none

    @throw boost graph exception
    */
    void detectBackEdges(SubGraph& gGraph , VectorEdgeDescriptor& vectBackEdges);

    /** This function checks if an edge between the two vertices creates a cycle or not. It assumes that there is no cylce present in the graph.
    @pre
        -# gGraph != NULL
        -# gGraph is acyclic

    @param gGraph
        reference to graph
    @param vectBackEdges
        reference to vector of back edges which are to be reversed

    @return none

    @throw none
     */
    bool doesEdgeCreateCycle(VertexDescriptor vSource , VertexDescriptor vTarget , SubGraph& gGraph);
//@}
};

#endif // GRAPHCYCLEHANDLER_H
