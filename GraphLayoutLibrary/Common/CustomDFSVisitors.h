#ifndef CUSTOMDFSVISITORS_H
#define CUSTOMDFSVISITORS_H

#include <Common/BoostGraphWrapper.h>
#include <boost/graph/depth_first_search.hpp>

/**
 * @brief The back_edge_recorder class
 *
 * Custom DFS Visitor that records back-edges for cycle detection. Inherited from 'default_dfs_visitor' in 'depth_first_search' from Boost.
 */
class back_edge_recorder : public default_dfs_visitor
{
public:
/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type back_edge_recorder.
 */

//@{
    /**
     * Construct object of back_edge_recorder
     *
     * @param vectEdges
     *  vector of edges as out parameter
     */
    back_edge_recorder(VectorEdgeDescriptor& vectEdges) : vectBackEdges(&vectEdges) {}
//@}

/** @name Modifiers
 *  The methods under this section are responsible for modifying
 *  an instance of type back_edge_recorder.
 */

//@{
    /**
     *  It is a visitor function which adds back-edge to the list of visited edges
     *
     * @param e
     *  back-edge
     *
     * @param gGraph
     *  graph
     */
    void back_edge(EdgeDescriptor e, SubGraph gGraph)
    {
        Q_UNUSED(gGraph);
        vectBackEdges->push_back(e);
    }
//@}

private:
    VectorEdgeDescriptor *vectBackEdges; /*!< Vector of edge desctriptors to store back-edges. */
};


#endif // CUSTOMDFSVISITORS_H
