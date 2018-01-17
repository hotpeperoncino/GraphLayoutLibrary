#ifndef SUBGRAPHORDERINGGRAPHTYPE_H
#define SUBGRAPHORDERINGGRAPHTYPE_H

#include <boost/graph/adjacency_list.hpp>
#include <HierarchicalLayoutGenerator/SubgraphOrderingGraph.h>
#include <HierarchicalLayoutGenerator/NestingTreeSubgraphNode.h>
#include <QVector>

class SubgraphOrderingGraphWrapper;

/**
 * A structure to store the vertex propertie in ordering graph.
 */
struct OrderingGraphVertexProperties
{
    SubgraphOrderingGraphWrapper *gSubgraphOrderingGraph;

    NestingTreeSubgraphNode *nestingTreeSubgraphNode;

    LayerNode *layerNode;

    double dAverageBarryCenter;

    bool bIsLayerNode;

    OrderingGraphVertexProperties()
    {
        gSubgraphOrderingGraph = NULL;
        nestingTreeSubgraphNode = NULL;
        layerNode = NULL;
        dAverageBarryCenter = 0.0;
        bIsLayerNode = true;
    }
};

/**
 * A structure to store the edge propertie in ordering graph.
 */
struct OrderingGraphEdgeProperties
{
    bool bIsSpanningTreeEdge;

    OrderingGraphEdgeProperties()
    {
        bIsSpanningTreeEdge = false;
    }
};

// Graph Properties
enum graph_custom_prop2_t
{
    graph_custom_prop2
};

namespace boost
{
BOOST_INSTALL_PROPERTY(graph,custom_prop2);
}

struct OrderingGraphProperties
{
};

typedef boost::adjacency_list< boost::listS,
boost::vecS,
boost::bidirectionalS,
boost::property<boost::vertex_index_t, int ,
       OrderingGraphVertexProperties > ,
boost::property<boost::edge_index_t,int , OrderingGraphEdgeProperties> ,
boost::property<graph_custom_prop2_t,OrderingGraphProperties> >
OrderingGraphType;

typedef boost::subgraph< OrderingGraphType > SubgraphOrderingGraphType;

//typedef OrderingGraphType SubgraphOrderingGraphType;

/*!< graph_traits of SubgraphOrderingGraphType defined as SubgraphOrderingGraphTraits */
typedef graph_traits<SubgraphOrderingGraphType> SubgraphOrderingGraphTraits;

/*!< vertex_descriptor of SubgraphOrderingGraphType defined as SubgraphOrderingGraphVertexDescriptor */
typedef SubgraphOrderingGraphTraits::vertex_descriptor SubgraphOrderingGraphVertexDescriptor;
/*!< edge_descriptor of SubgraphOrderingGraphType defined as SubgraphOrderingGraphEdgeDescriptor */
typedef SubgraphOrderingGraphTraits::edge_descriptor SubgraphOrderingGraphEdgeDescriptor;
/*!< adjacency_iterator of SubgraphOrderingGraphType defined as AdjacencyIteratorSubgraphOrdeingGraph */
typedef SubgraphOrderingGraphTraits::adjacency_iterator AdjacencyIteratorSubgraphOrdeingGraph;

/*!< Vector of SubgraphOrderingGraphVertexDescriptor defined as VectorSubgraphOrderingGraphVertexDescriptor*/
typedef QVector<SubgraphOrderingGraphVertexDescriptor> VectorSubgraphOrderingGraphVertexDescriptor;
/*!< Vector Iterator of SubgraphOrderingGraphVertexDescriptor defined as IteratorVectorSubgraphOrderingGraphVertexDescriptor*/
typedef QVectorIterator<SubgraphOrderingGraphVertexDescriptor> IteratorVectorSubgraphOrderingGraphVertexDescriptor;
/*!< Vector of SubgraphOrderingGraphEdgeDescriptor defined as VectorSubgraphOrderingGraphEdgeDescriptor*/
typedef QVector<SubgraphOrderingGraphEdgeDescriptor> VectorSubgraphOrderingGraphEdgeDescriptor;
/*!< Vector iterator of SubgraphOrderingGraphEdgeDescriptor defined as IteratorVectorSubgraphOrderingGraphEdgeDescriptor*/
typedef QVectorIterator<SubgraphOrderingGraphEdgeDescriptor> IteratorVectorSubgraphOrderingGraphEdgeDescriptor;



#endif // SUBGRAPHORDERINGGRAPHTYPE_H
