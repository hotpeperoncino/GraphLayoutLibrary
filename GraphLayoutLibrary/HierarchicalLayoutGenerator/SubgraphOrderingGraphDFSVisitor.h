#ifndef SUBGRAPHORDERINGGRAPHDFSVISITOR_H
#define SUBGRAPHORDERINGGRAPHDFSVISITOR_H

#include <HierarchicalLayoutGenerator/SubgraphOrderingGraphType.h>
#include <boost/graph/depth_first_search.hpp>

/*! Custom DFS Visitor for back edges recording for cycle detection */
class SubgraphOrderingGraphBackEdgeRecorder : public default_dfs_visitor
{
public:
    SubgraphOrderingGraphBackEdgeRecorder(VectorSubgraphOrderingGraphEdgeDescriptor& vecBackEdge)
                                    : vecRecordBackEdge(&vecBackEdge)
    {}

    void back_edge(SubgraphOrderingGraphEdgeDescriptor eBackEdge , SubgraphOrderingGraphType gGraph)
    {
        Q_UNUSED(gGraph);
        vecRecordBackEdge->push_back(eBackEdge);
    }

private:
    VectorSubgraphOrderingGraphEdgeDescriptor *vecRecordBackEdge; /*!< vector of back edges recording for cycle detection */
};


#endif // SUBGRAPHORDERINGGRAPHDFSVISITOR_H
