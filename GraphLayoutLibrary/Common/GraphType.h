#ifndef GRAPHTYPE_H
#define GRAPHTYPE_H

#include <QString>
#include <QMap>
#include <vector>
#include <QVector>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/graph_utility.hpp>
#include <Common/LayoutEnum.h>
#include <LayoutException/LayoutExceptionEnum.h>

using namespace std;
using namespace boost;

//enum NodeType {  UpperBorderNode = 999,
//                 LowerBorderNode,
//                 DummyNode,
//                 GraphNode,
//                 ExpandableNode
//              };

//enum EdgeType {  NestingEdge = 1100,
//                 GraphEdge,
//                 LongEdgeSegment,
//                 InvisibleEdge
//              };

typedef QVector<size_t> QVectorUInt; 
typedef QVectorUInt::iterator IteratorQVectorUInt;

typedef QVector<int> QVectorInt;
typedef QVectorInt::iterator IteratorQVectorInt;

/**
 * A structure to store the bend points on an edge.
 */
struct BendPoints
{
    int iCoordinateX; 
    int iCoordinateY; /*!< y coordinate */
	/**
     * Set bend point to origin
     */
    BendPoints()
    {
        iCoordinateX = 0;
        iCoordinateY = 0;
    }
};

typedef QVector<BendPoints *> QVectorBendPoints;
typedef QVectorBendPoints::iterator IteratorQVectorBendPoints;

/**
 * A structure to store vertex properties.
 */
struct VertexProperties
{
    QString sId; /*!< Unique string id */

    int iLeftCoordX; /*!< Left upper point x coordinates */
    int iLeftCoordY; /*!< Left upper point y coordinates */

    int iHeight; /*!< Height of vertex */
    int iWidth; /*!< Width of vertex */

    int iCoord_X; /*!< Center point x coordinate of vertex */
    int iCoord_Y; /*!< Center point y coordinate of vertex */

    int iVertexNumber; // To Do: later remove this, currently used in client as vertex_index

    bool bIsInvisible; /*!< Is vertex invisible */
    bool bIsExpandable; /*!< For dummy graph node */

    size_t iOrder; /*!< For vertex ordering */

    /** @name Hierachical Layout Properties
	 */
	//@{
    int iTreeWidth;
    int iDistanceFromRoot; /*!< Unique string id */
    int iRank; /*!< Unique string id */
    int iHorizontalPosition; /*!< Horizontal position in the Layer */
    double dXCoordinateRelativePosition; /*!< X Coordinate Relative Position */
    double dBarryCenter; /*!< Barrycenter Weight */
    double dXPos; /*!< Horizontal X position */
    double dYPos; /*!< Vertical y position */
    int iTreeLeftX; /*!< Unique string id */
    bool bVisited; /*!< Has it been visited */
    int iVertexCount; /*!< number of vertices in this subgraph */
    LayoutEnum::NodeType enVertexType; /*!< Node type */
    int iTopologicalOrder; /*!< Order for SubgraphOrderingGraph Nodes */
	//@}

    /** @name Force Directed Properties
	 */
	//@{
    double iVertXDisp ; /*!< Unique string id */
    double iVertYDisp ; /*!< Unique string id */
    bool bIsVertexLocked; /*!< Unique string id */
    int iClusterIdentifier; // Identifies cluster of vertex /*!< Unique string id */
    bool bIsVirtualNode; /*!< Unique string id */
	//@}

	/**
     * Assign default property values to the structure variable
     */
    VertexProperties()
    {
        sId = "";
        iLeftCoordX = 0;
        iLeftCoordY= 0;
        iHeight = 30;
        iWidth = 30;
        iCoord_X = 0;
        iCoord_Y = 0;
        iVertexNumber = 0;
        bIsInvisible = false;
        bIsExpandable = false;
        iOrder = 0;
        iTreeWidth = 0;
        iDistanceFromRoot = 0;
        iRank = 0;
        iHorizontalPosition = 0;
        dXCoordinateRelativePosition = 0;
        dBarryCenter = 1; //Asigned 1 because first position in layer start from 1
        iTreeLeftX = 0;
        bVisited = false;
        enVertexType = LayoutEnum::GraphNode;
        dYPos = 0.0;
        dXPos = 0.0;
        iTopologicalOrder = 0;
        iVertexCount = 0;
        iVertXDisp = 0;                  // Force Directed Props
        iVertYDisp = 0;
        bIsVertexLocked = false;
        iClusterIdentifier = 999;   // 999 default indicates default cluster
        bIsVirtualNode = false ;
    }
};

/**
 * A structure to store egde properties.
 */
struct EdgeProperties
{
    QString sId; /*!< Unique string id */
	std::size_t iEdgeIndex; /*!< Global index */
    bool bBidirectional; /*!< Type of edge direction */
    bool bIsInvisible; /*!< Is edge invisible  */
    size_t iOrder; // For edge ordering
    int iWeight; /*!< Weight of edge */
    bool bVisited; /*!< Has edge been visited */
    bool bReversed; /*!< Has edge been reversed */
    bool bIsConflicted; /*!< Conflicted or crossing edge */
    LayoutEnum::EdgeType enEdgeType; /*!< Edge type */
    QVectorBendPoints vecBendPoints; /*!< List of bend Points */

	/**
     * Assign default property values to the structure variable
     */
    EdgeProperties()
    {
        sId = "";
        iEdgeIndex = 0;
        bBidirectional = false;
        bIsInvisible = false;
        bReversed = false;
        iOrder = 0;
        iWeight = 0;
        bVisited = false;
        bReversed = false;
        enEdgeType = LayoutEnum::GraphEdge;
        bIsConflicted = false;
    }
};

// Graph Properties
enum graph_custom_prop_t
{
    graph_custom_prop
};

namespace boost
{
BOOST_INSTALL_PROPERTY(graph,custom_prop);
}

/**
 * A structure to store graph properties.
 */
struct GraphProperties
{
    QString sId; /*!< Unique string id */
    size_t iDummyNodeIndex; /*!< Index of vertex reperesenting this subgraph - Dummy Node */
    int iLeftTopCoordX; /*!< Left upper point x coordinate */
    int iLeftTopCoordY; /*!< Left upper point y coordinate */
    int iHeight; /*!< Height of graph */
    int iWidth; /*!< Weight of graph */
    double dRadius; /*!< Radius for graph */
    int iCenterCoordX; /*!< Center x coordinate for graph */
    int iCenterCoordY; /*!< Center y coordinate for graph */
    int iCoordX; /*!< Center point x of graph */
    int iCoordY; /*!< Center point y of graph */
    int iXDistanceFromDummyNodeCentroid; /*!< Horizontal displacement between centroid of graph and center of graph */
    int iYDistanceFromDummyNodeCentroid; /*!< Vertical displacement between centroid of graph and center of graph */
    QVectorUInt vectOwnVertices; /*!< Own Vertices */
    std::size_t vUpperBorderVertex; /*!< Upper Border Vertex */
    std::size_t vLowerBorderVertex; /*!< Lower Border Vertex */

	/** @name Force Directed Properties
	 */
	//@{
    int iClusterID; /*!< Cluster ID */
    int iClusterXDisplacement; /*!< Horizontal cluster displacemrnt */
    int iClusterYDisplacement; /*!< Horizontal cluster displacemrnt */
	//@}

	/**
     * Assign default property values to the structure variable
     */
    GraphProperties()
    {
        sId = "";
        iDummyNodeIndex = 0;
        iLeftTopCoordX = 0;
        iLeftTopCoordY = 0;
        iCenterCoordX = 0;
        iCenterCoordY = 0;
        iHeight = 0;
        iWidth = 0;
        dRadius = 0.0;
        iCoordX = 0;
        iCoordY = 0;
        vUpperBorderVertex = 0;
        vLowerBorderVertex= 0;
        iXDistanceFromDummyNodeCentroid = 0;
        iYDistanceFromDummyNodeCentroid = 0;
        iClusterID =999;      // for global graph
        iClusterXDisplacement = 0;
        iClusterYDisplacement = 0;
    }
};


//To Do: Update point type to Geometry Point
enum vertex_position_t {vertex_position};
namespace boost {BOOST_INSTALL_PROPERTY(vertex , position);}
typedef circle_topology<>::point_type point;

typedef boost::adjacency_list< boost::listS,
boost::vecS,
boost::bidirectionalS,
boost::property<boost::vertex_index_t, int ,
       property<vertex_position_t, point, VertexProperties> > ,
boost::property<boost::edge_index_t,int , EdgeProperties>,
boost::property<graph_custom_prop_t,GraphProperties> >
Graph;

typedef boost::subgraph< Graph > SubGraph; //PGL Graph Type

//*****************************************************************

typedef graph_traits<SubGraph> SubGraphTraits;

//Vertex and Edge Descriptor --------------------------------------
typedef SubGraphTraits::vertex_descriptor VertexDescriptor;
typedef SubGraphTraits::edge_descriptor EdgeDescriptor;

//Iterators -------------------------------------------------------
typedef SubGraphTraits::vertex_iterator VertexIterator;
typedef SubGraphTraits::edge_iterator EdgeIterator;
typedef SubGraphTraits::in_edge_iterator InEdgeIterator;
typedef SubGraphTraits::out_edge_iterator OutEdgeIterator;
typedef SubGraphTraits::adjacency_iterator AdjacencyIterator;

typedef SubGraph::children_iterator ChildrenIterator;

//Pair types -----------------------------------------------------
//Vertex Iterator pair, return type of: vertices()
typedef std::pair<VertexIterator , VertexIterator> VertexIterPair;
//Edge pair, return type of: add_edge(u,v,graph)
typedef std::pair<EdgeDescriptor,bool> EdgeBoolPair;
//Children Iterator pair, return type of: subgraphObj.children()
typedef std::pair<ChildrenIterator,ChildrenIterator> ChildrenIterPair;
//In_Edge Iterator pair, return type of: in_edges(vertex,graph)
typedef std::pair<InEdgeIterator,InEdgeIterator> InEdgeIterPair;
//Out_Edge Iterator pair, return type of: out_edges(vertex,graph)
typedef std::pair<OutEdgeIterator,OutEdgeIterator> OutEdgeIterPair;


//Maps --------------------------------------------------------------
//Vertex Ordering Map
typedef QMap<std::size_t , VertexDescriptor> MapOrderVertex;
typedef QMapIterator<std::size_t , VertexDescriptor> IteratorMapOrderVertex;

// Vectors --------------------------------------------------------
typedef vector<SubGraph *> VectorSubgraphRefs;
//Edges Vector
typedef vector<EdgeDescriptor> VectorEdgeDescriptor;
//Vertices Vector
typedef vector<VertexDescriptor> VectorVertexDescriptor;
//Order Vector
typedef vector<std::size_t> VectorVertexOrder;
//*****************************************************************

//MACRO
typedef graph_traits<SubGraph>::vertices_size_type
VertexSizeType;





#endif // GRAPHTYPE_H
