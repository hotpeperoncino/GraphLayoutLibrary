#ifndef BOOSTGRAPHWRAPPER_H
#define BOOSTGRAPHWRAPPER_H

#include <GraphLayoutLibrary_global.h>
#include <Common/GraphType.h>
#include <Common/GeometryHelper.h>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/smallest_last_ordering.hpp>
#include <boost/graph/topological_sort.hpp>
#include <QtCore/qmath.h>
#include <boost/exception/get_error_info.hpp>
#include <LayoutUtilities/CommonLayoutConstants.h>
#include <LayoutException/LayoutExceptionConstants.h>
#include <LayoutException/LayoutMemoryException.h>
#include <QDebug>

using namespace std;

typedef boost::error_info<struct tag_errmsg, std::string> errmsg_info;


//To Do: Later move MACROS to separate header file
#define DELETE_AND_SET_NULL(objectPointer)\
if(objectPointer != NULL)\
{\
    delete objectPointer;\
    objectPointer = NULL;\
}


#define PGL_VERTEX_INDEX(vertex,graph) \
    get(vertex_index,graph,vertex)

#define PGL_PRINT_VERTEX_ORDER( graph , mapVertexOrder , GraphType) \
    cout << "Vertex Order: "; \
    BGL_FORALL_VERTICES(BGL_CAT(v,__LINE__) , graph , GraphType) \
    cout << mapVertexOrder[BGL_CAT(v,__LINE__)] << " "; \
    cout << endl;

//For Vertex Ordering
enum ShiftDirection { LeftDirection = 1000, RightDirection = 1001 };

//Map vertex descriptor to order
#define PGL_MAP_VERTEX_ORDER(mapVertexOrder,graph) \
    property_map<SubGraph , std::size_t VertexProperties::*>::type \
    mapVertexOrder(get(&VertexProperties::iOrder , graph));

//Map vertex descriptor to its bundled property
#define PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexProperty, PropertyType , PropertyName , graphObject) \
    property_map<SubGraph , PropertyType VertexProperties::*>::type \
    mapVertexProperty(get(&VertexProperties::PropertyName , graphObject));

//Swap vertex order values
#define PGL_SWAP_VERTEX_ORDER(v1 , v2 , mapVertexOrder) \
    mapVertexOrder[v1] = mapVertexOrder[v1] ^ mapVertexOrder[v2]; \
    mapVertexOrder[v2] = mapVertexOrder[v1] ^ mapVertexOrder[v2]; \
    mapVertexOrder[v1] = mapVertexOrder[v1] ^ mapVertexOrder[v2];

//Map graph property
#define PGL_MAP_GRAPH_REF_PROPERTY(mapGraphRefPropertyName , gGraph)\
    boost::ref_property_map<SubGraph*, GraphProperties>\
    mapGraphRefPropertyName(boost::get_property(gGraph,graph_custom_prop));

//To Do: later compartment class
/**
 * A structure to store comprtment layout.
 */
struct LayoutCompartment{
    int x,y; /*!< coordinates of location */
    int height; /*!< Height */
    int width; /*!< Width */
};

/**
 * @brief The BoostGraphWrapper class
 *
 * The wrapper class around Boost graphs. Provides methods to access and manipulte properties of the graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT BoostGraphWrapper
{

private:

    SubGraph m_Graph; /*!< Instance of Boost graph */

public:
/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type BoostGraphWrapper.
 */

//@{
    /**
     Constructs new object of type BoostGraphWrapper
    */
    BoostGraphWrapper();

    /**
     Constructs new object of type BoostGraphWrapper
     @param graph
     Object of boost graph type
    */
    BoostGraphWrapper(SubGraph graph);
//@}

/** @name Modifiers
 *  The methods under this section are responsible for modifying
 *  an instance of type BoostGraphWrapper.
 */

//@{
    //Add Vertex
    //for m_Graph
    /**
     This function adds new vertex in the wrapper member graph.

    @pre
    -# gInputGraph != NULL

    @param none

    @return vertexdescriptor

    @throw none
    */
    VertexDescriptor addVertex();
    //for m_Graph

    /**
     This function adds provided vertex in the wrapper member graph.

    @pre none

    @param none

    @return vertexdescriptor

    @throw none
    */
    VertexDescriptor addVertex(VertexDescriptor& vertex);

    /**
     This function adds new vertex in the provided graph.

    @pre
    -# subgraph != NULL

    @param subgraph
        reference to input graph

    @return vertexdescriptor

    @throw none
    */
    VertexDescriptor addVertex(SubGraph& subgraph);

    /**
     This functions adds provided vertex in the provided graph.

    @pre
    -# subgraph != NULL

    @param subgraph
        reference to input graph

    @param vertex
        reference to vertex

    @return vertexdescriptor

    @throw none
    */
    VertexDescriptor addVertex(VertexDescriptor& vertex , SubGraph& subgraph);

    /**
      This function adds a vertex (node) of specified type

      @pre
      -# subgraph != NULL
      -# enVertexType should be valid

      @param subgraph
        reference to input graph.

      @param enVertexType
        type of vertex from available types.

      @return VertexDescriptor

      @throw none
     */
    VertexDescriptor addVertex(SubGraph &subgraph , LayoutEnum::NodeType enVertexType);

    //Add Edge

    //for m_Graph
    /**
     This function adds edge between provided source and target vertices in the wrapper member graph.

    @pre none

    @param vertex1
        reference of source vertex.

    @param vertex2
        reference of target vertex.

    @return edgedescriptor and bool pair

    @throw none
    */
    EdgeBoolPair addEdge(VertexDescriptor& vertex1,VertexDescriptor& vertex2);

    /**
     This function adds edge between provided source and target vertices in the provided graph.

    @pre none

    @param vertex1
        reference of source vertex.

    @param vertex2
        reference of target vertex.

    @param subgraph
        referecne to graph

    @return edgedescriptor and bool pair

    @throw none
    */
    EdgeBoolPair addEdge(VertexDescriptor& vertex1,VertexDescriptor& vertex2,
                         SubGraph& subgraph);

    /**
     This function adds edge between provided source and target vertices in the wrapper member graph.

    @pre
    -# subgraph != NULL
    -# enEdgeType should be valid type

    @param vertex1
        reference of source vertex.

    @param vertex2
        reference of target vertex.

    @param subgraph
        referecne to graph

    @param enEdgeType
        type of edge from available types

    @return edgedescriptor and bool pair

    @throw none
    */
    EdgeBoolPair addEdge(VertexDescriptor& vertex1,VertexDescriptor& vertex2,
                         SubGraph& subgraph, LayoutEnum::EdgeType enEdgeType);

    /**
     This function deletes provided edge from provided graph.

    @pre
    -# gGraph != NULL

    @param eEdge
        reference of edge

    @param subgraph
        referecne to graph

    @return none

    @throw none
    */
    void removeEdge(EdgeDescriptor& eEdge , SubGraph& gGraph);

    /**
     This function deletes provided edge from wrapper member graph.

    @pre
    -# gGraph != NULL

    @param eEdge
        reference to edge

    @return none

    @throw none
    */
    void removeEdge(EdgeDescriptor& eEdge);


    /**
     This function deletes edge between provided source and target vertices from provided graph.

    @pre
    -# gGraph != NULL

    @param vSource
        reference to vSource

    @param vTarget
        reference to vTarget

    @param gGraph
        reference to gGraph

    @return none

    @throw none
    */
    void removeEdge(VertexDescriptor& vSource , VertexDescriptor& vTarget , SubGraph& gGraph);


    /**
     This function deletes edge between provided source and target vertices from wrapeer member graph.

    @pre none

    @param vSource
        reference to vSource

    @param vTarget
        reference to vTarget

    @return none

    @throw none
    */
    void removeEdge(VertexDescriptor& vSource , VertexDescriptor& vTarget);
	
	//Subgraph
    /**
     This function creates empty subgraph under the hierarchy of wrapper member graph.

    @pre none

    @param none

    @return reference to subgraph

    @throw none
    */
    SubGraph& addSubgraph();


    /**
     This function creates empty subgraph under the hierarchy of provided graph.

    @pre
    -# subgraph != NULL

    @param subgraph
        reference to subgraph

    @return reference to subgraph

    @throw none
    */
    SubGraph& addSubgraph(SubGraph& subgraph);
	
	/**
     This function sets vertex id for provided vertex to vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @pre
    -# sId should not be empty

    @param vertex
        reference to subgraph

    @param vertex
        reference to vertex

    @param sId
        id for the vertex

    @return none

    @throw none
    */
    void setVertexId(VertexDescriptor& vertex, SubGraph& subgraph, QString sId);


    /**
     This function sets vertex id for provided vertex to vertex property of wrapper member graph.

    @pre
    -# subgraph != NULL

    @pre
    -# sId should not be empty

    @param vertex
        reference to vertex

    @param sId
        id for the vertex

    @return none

    @throw none
    */
    void setVertexId(VertexDescriptor& vertex , QString sId);
	
	/**
     This function sets height for provided vertex to vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iHeight
        height of vertex

    @return none

    @throw none
    */
    void setVertexHeight(VertexDescriptor& vertex, SubGraph& subgraph , int iHeight);


    /**
     This function sets height for provided vertex to vertex property of wrapper member graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param iHeight
        height of vertex

    @return none

    @throw none
    */
    void setVertexHeight(VertexDescriptor& vertex , int iHeight);
	
	/**
     This function sets width for provided vertex to vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iWidth
        width of vertex

    @return none

    @throw none
    */
    void setVertexWidth(VertexDescriptor& vertex, SubGraph& subgraph , int iWidth);


    /**
     This function sets width for provided vertex to vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iWidth
        width of vertex

    @return none

    @throw none
    */
    void setVertexWidth(VertexDescriptor& vertex , int iWidth);
	
	/**
     This function writes left x coordinate for provided vertex to vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iLeftCoordX
        value of left coordinate X

    @return none

    @throw none
    */
    void setVertexLeftCoordX(VertexDescriptor& vertex , int iLeftCoordX);


    /**
     This function writes left x coordinate for provided vertex to vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iLeftCoordX
        value of left coordinate X

    @return none

    @throw none
    */
    void setVertexLeftCoordX(VertexDescriptor& vertex, SubGraph& subgraph , int iLeftCoordX);
	
	/**
     This function writes left y coordinate for provided vertex to vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iLeftCoordY
        value of left coordinate Y

    @return none

    @throw none
    */
    void setVertexLeftCoordY(VertexDescriptor& vertex, SubGraph& subgraph , int iLeftCoordY);


    /**
     This function writes left y coordinate for provided vertex to vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iLeftCoordY
        value of left coordinate Y

    @return none

    @throw none
    */
    void setVertexLeftCoordY(VertexDescriptor& vertex, int iLeftCoordY);
	
	/**
     This function writes center x coordinate for provided vertex to vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iCoordX
        value of center coordinate X

    @return none

    @throw none
    */
    void setVertexCenterCoordX(VertexDescriptor& vertex , int iCoordX);


    /**
     This function writes center x coordinate for provided vertex to vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iCoordX
        value of center coordinate X

    @return none

    @throw none
    */
    void setVertexCenterCoordX(VertexDescriptor& vertex, SubGraph& subgraph , int iCoordX);
	
	/**
     This function writes center y coordinate for provided vertex to vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iCoordY
        value of center coordinate Y

    @return none

    @throw none
    */
    void setVertexCenterCoordY(VertexDescriptor& vertex, SubGraph& subgraph , int iCoordY);


    /**
     This function writes center y coordinate for provided vertex to vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iCoordY
        value of center coordinate y

    @return none

    @throw none
    */
    void setVertexCenterCoordY(VertexDescriptor& vertex, int iCoordY);
	
	/**
     This function writes whether provided vertex is invisible or not to vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param bIsInvisible
        boolean whether the vertex is visible type or not

    @return none

    @throw none
    */
    void setVertexIsInvisible(VertexDescriptor& vertex, SubGraph& subgraph , bool bIsInvisible);


    /**
     This function writes whether provided vertex is invisible or not to vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param bIsInvisible
        boolean whether the vertex is visible type or not

    @return none

    @throw none
    */
    void setVertexIsInvisible(VertexDescriptor& vertex , bool bIsInvisible);
	
	/**
     This function writes whether provided vertex is invisible or not to vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param bIsExpandable
        boolean whether the vertex is expandable type or not

    @return none

    @throw none
    */
    void setVertexExpandable(VertexDescriptor& vertex, SubGraph& subgraph , bool bIsExpandable);


    /**
     This function writes whether provided vertex is expandable or not to vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param bIsExpandable
        boolean whether the vertex is epandable type or not

    @return none

    @throw none
    */
    void setVertexExpandable(VertexDescriptor& vertex , bool bIsExpandable);
	
	/**
     This function writes treewidth in the vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iTreeWidth
        tree width in integer

    @return none

    @throw none
    */
    void setVertexTreeWidth(VertexDescriptor& vertex, SubGraph& subgraph , int iTreeWidth);


    /**
     This function writes treewidth in the vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iTreeWidth
        value of the treewidth

    @return none

    @throw none
    */
    void setVertexTreeWidth(VertexDescriptor& vertex,int iTreeWidth);
	
	/**
     This function writes distance from root in the vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iDistanceFromRoot
        distance from root in integer

    @return none

    @throw none
    */
    void setVertexDistanceFromRoot(VertexDescriptor& vertex, SubGraph& subgraph , int iDistanceFromRoot);


    /**
     This function writes distance from root in the vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iDistanceFromRoot
        value of the distance from root.

    @return none

    @throw none
    */
    void setVertexDistanceFromRoot(VertexDescriptor& vertex , int iDistanceFromRoot);

    //int iRank;

    


    /**
     This function writes rank of vertex in the vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iRank
        rank of vertex in integer

    @return none

    @throw none
    */
    void setVertexRank(VertexDescriptor& vertex, SubGraph& subgraph , int iRank);


    /**
     This function writes vertex rank in the vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iRank
        value of the vertex rank.

    @return none

    @throw none
    */
    void setVertexRank(VertexDescriptor& vertex , int iRank);


    //int iPosition;
    


    /**
     This function writes horizontal position of vertex in the vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iHorizontalPosition
        horizontal position of vertex in integer

    @return none

    @throw none
    */
    void setVertexHorizontalPosition(VertexDescriptor& vertex, SubGraph& subgraph , int iHorizontalPosition);


    /**
     This function writes vertex horizontal position in the vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iHorizontalPosition
        value of the horizontal position.

    @return none

    @throw none
    */
    void setVertexHorizontalPosition(VertexDescriptor& vertex , int iHorizontalPosition);

    //Barrycenter Weight
    //double dBarryCenter;
    


    /**
     This function writes barry center of vertex in the vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param dBarryCenter
        barry center of vertex in integer

    @return none

    @throw none
    */
    void setVertexBarryCenter(VertexDescriptor& vertex, SubGraph& subgraph , double dBarryCenter);


    /**
     This function writes vertex barry center value in the vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param dBarryCenter
        value of the barry center.

    @return none

    @throw none
    */
    void setVertexBarryCenter(VertexDescriptor& vertex , double dBarryCenter);
	
	void setVertexTreeLeftX(VertexDescriptor& vertex, SubGraph& subgraph , int iTreeLeftX);

    void setVertexTreeLeftX(VertexDescriptor& vertex , int iTreeLeftX);

    //Node type
    //NodeType enVertexType;

    


    /**
     This function writes type of vertex in the vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param enVertexType
        type of vertex

    @return none

    @throw none
    */
    void  setVertexType(VertexDescriptor& vertex , SubGraph& subgraph , LayoutEnum::NodeType enVertexType);


    /**
     This function writes vertex type in the vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param enVertexType
        type of vertex from available types

    @return none

    @throw none
    */
    void  setVertexType(VertexDescriptor& vertex , LayoutEnum::NodeType enVertexType);
	
	void  setVertexVisited(VertexDescriptor& vertex , SubGraph& subgraph , bool bVisited);

    void  setVertexVisited(VertexDescriptor& vertex , bool bVisited);

    
    /**
     This function writes topological order of vertex in the vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @param iTopologicalOrder
        topological order of vertex in integer

    @return none

    @throw none
    */
    void setVertexTopologicalOrder(int iTopologicalOrder , VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function writes vertex topological order in the vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @param iTopologicalOrder
        topological order of vertex in integer

    @return none

    @throw none
    */
    void setVertexTopologicalOrder(int iTopologicalOrder , VertexDescriptor& vertex);

    
    /**
     This function writes id of edge in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @param sId
        id of edge

    @return none

    @throw none
    */
    void setEdgeId(EdgeDescriptor& edge, SubGraph& subgraph, QString sId);


    /**
     This function writes edge id in the edge property of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @param sId
        id of edge

    @return none

    @throw none
    */
    void setEdgeId(EdgeDescriptor& edge , QString sId);
	
	void setEdgeIndex(EdgeDescriptor& edge, SubGraph& subgraph, std::size_t iEdgeIndex);

    void setEdgeIndex(EdgeDescriptor& edge , std::size_t iEdgeIndex);


    /**
     This function writes type of edge direction in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @param bBidirectional
        type of edge direction

    @return none

    @throw none
    */
    void setEdgeBidirectional(EdgeDescriptor& edge, SubGraph& subgraph , bool bBidirectional);


    /**
     This function writes type of edge direction in the edge property of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @param bBidirectional
        type of edge direction

    @return none

    @throw none
    */
    void setEdgeBidirectional(EdgeDescriptor& edge , bool bBidirectional);

    /**
     This function writes type of edge invisible in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @param bIsInvisible
        type of edge invisible

    @return none

    @throw none
    */
    void setEdgeIsInvisible(EdgeDescriptor& edge, SubGraph& subgraph , bool bIsInvisible);


    /**
     This function writes type of edge invisible in the edge property of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @param bIsInvisible
        type of edge invisible

    @return none

    @throw none
    */
    void setEdgeIsInvisible(EdgeDescriptor& edge , bool bIsInvisible);

    

    void  setEdgeVisited(EdgeDescriptor& edge , SubGraph& subgraph , bool bVisited);

    void  setEdgeVisited(EdgeDescriptor& edge , bool bVisited);


    //Edge Type
    void setEdgeType(EdgeDescriptor eEdge, SubGraph &subgraph, LayoutEnum::EdgeType enEdgeType);

    void setEdgeType(EdgeDescriptor eEdge, LayoutEnum::EdgeType enEdgeType);

    /**
     This function writes type of edge reversed in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @param bReversed
        type of edge reversed

    @return none

    @throw none
    */
    void  setEdgeReversed(EdgeDescriptor& edge , SubGraph& subgraph , bool bReversed);

    /**
     This function writes type of edge reversed in the edge property of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @param bReversed
        type of edge reversed

    @return none

    @throw none
    */
    void  setEdgeReversed(EdgeDescriptor& edge , bool bReversed);

    /**
     This function writes if edge is conflicted/crossing with another edge in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @param bIsConflicted
        writes if edge is conflicted/crossing with another edge or not

    @return none

    @throw none
    */
    void setEdgeIsConflicted(EdgeDescriptor& edge, SubGraph& subgraph , bool bIsConflicted);


    /**
     This function writes if edge is conflicted/crossing with another edge in the edge property of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @param bIsConflicted
        if edge is conflicted/crossing with another edge or not

    @return none

    @throw none
    */
    void setEdgeIsConflicted(EdgeDescriptor& edge , bool bIsConflicted);
	
	//Bend points
    /**
      This function prepares list of bend points objects into edge properties of provided graph.
     *
     * @pre
     *  -# gGraph != NULL
     *
     * @param bendPointRef
     *      reference to bendpoints containing x and y coordinate
     *
     * @return none
     *
     * @throw none
     */
    void addBendPoint(BendPoints* bendPointRef , EdgeDescriptor eEdge , SubGraph& gGraph);

    /**
      This function sets id to provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *  -# sId should not be empty.
     *
     * @param gSubgraph
     *      reference to graph
     *
     * @param sId
     *      id of a graph
     *
     * @return none
     *
     * @throw none
     */
    void setGraphId(QString sId , SubGraph& gSubgraph);


    /**
      This function sets id to wrapper member graph
     *
     * @pre
     *  -# sId should not be empty.
     *
     * @param sId
     *      id of a graph
     *
     * @return none
     *
     * @throw none
     */
    void setGraphId(QString sId);

    /**
      This function sets index of this provided graph.
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to graph
     *
     * @return none
     *
     * @throw none
     */
    void setGraphDummyNodeIndex(std::size_t iDummyNodeIndex , SubGraph& gSubgraph);


    /**
      This function sets index of this wrapper member graph.
     *
     * @pre none
     *
     * @param iDummyNodeIndex
     *      value of index to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphDummyNodeIndex(std::size_t iDummyNodeIndex);

    


    /**
      This function sets left x coordinate to provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param iLeftCoordX
     *      value of left coordinate to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphLeftTopCoordX(int iLeftTopCoordX , SubGraph& gSubgraph);


    /**
      This function sets left x coordinate to wrapper member graph
     *
     * @pre none
     *
     * @param iLeftCoordX
     *      value of left coordinate to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphLeftTopCoordX(int iLeftTopCoordX);
	
	/**
      This function sets top y coordinate to provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param iLeftCoordX
     *      value of left coordinate to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphLeftTopCoordY(int iLeftTopCoordY , SubGraph& gSubgraph);


    /**
      This function sets top y coordinate to wrapper member graph
     *
     * @pre none
     *
     * @param iLeftCoordY
     *      value of left y coordinate to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphLeftTopCoordY(int iLeftTopCoordY);

    
    /**
      This function sets center x coordinate to provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param iCenterCoordX
     *      value of center x coordinate to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphCenterCoordX(int iCenterCoordX , SubGraph& gSubgraph);


    /**
      This function sets center x coordinate to wrapper member graph
     *
     * @pre none
     *
     * @param iCenterCoordX
     *      value of center x coordinate to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphCenterCoordX(int iCenterCoordX);


    /**
      This function sets center y coordinate to provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param iCenterCoordY
     *      value of center y coordinate to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphCenterCoordY(int iCenterCoordY , SubGraph& gSubgraph);


    /**
      This function sets center y coordinate to wrapper member graph
     *
     * @pre none
     *
     * @param iCenterCoordY
     *      value of center y coordinate to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphCenterCoordY(int iCenterCoordY);

    /**
      This function sets radius to provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param dRadius
     *      value of radius to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphRadius(double dRadius, SubGraph& gSubgraph);


    /**
      This function sets radius to wrapper member graph
     *
     * @pre none
     *
     * @param dRadius
     *      value of radius to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphRadius(double dRadius);

    /**
      This function sets height to provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param iHeight
     *      value of height to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphHeight(int iHeight , SubGraph& gSubgraph);


    /**
      This function sets height to wrapper member graph
     *
     * @pre none
     *
     * @param iHeight
     *      value of height to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphHeight(int iHeight);

    


    /**
      This function sets width to provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param iWidth
     *      value of width to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphWidth(int iWidth , SubGraph& gSubgraph);


    /**
      This function sets width to wrapper member graph
     *
     * @pre none
     *
     * @param iWidth
     *      value of width to be set
     *
     * @return none
     *
     * @throw none
     */
    void setGraphWidth(int iWidth);
	
	void setGraphCoordX(int iCoordX , SubGraph& gSubgraph);

    void setGraphCoordX(int iCoordX);

    void setGraphCoordY(int iCoordY , SubGraph& gSubgraph);

    void setGraphCoordY(int iCoordY);

    /**
      This function sets distance between centroid of graph and center x coordinate of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param iDistanceCoordX
     *      distance between centroid of graph and center x coordinate
     *
     * @return none
     *
     * @throw none
     */
    void setGraphDistanceBetweenCentoidAndCenterCoordX(int iDistanceCoordX , SubGraph& gSubgraph);


    /**
      This function sets distance between centroid of graph and center x coordinate of wrapper member graph
     *
     * @pre none
     *
     * @param iDistanceCoordX
     *      distance between centroid of graph and center x coordinate
     *
     * @return none
     *
     * @throw none
     */
    void setGraphDistanceBetweenCentoidAndCenterCoordX(int iDistanceCoordX);

    /**
      This function sets distance between y coordinate centroid of graph and center y coordinate of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @param iDistanceCoordY
     *      distance between centroid of graph and center y coordinate
     *
     * @return none
     *
     * @throw none
     */
    void setGraphDistanceBetweenCentoidAndCenterCoordY(int iDistanceCoordY , SubGraph& gSubgraph);

    /**
      This function sets distance between  y coordinate centroid of graph and center y coordinate of wrapper member graph
     *
     * @pre none
     *
     * @param iDistanceCoordY
     *      distance between centroid of graph and center y coordinate
     *
     * @return none
     *
     * @throw none
     */
    void setGraphDistanceBetweenCentoidAndCenterCoordY(int iDistanceCoordY);


    //Own Vertices
    //QVectorUInt vectOwnVertices;
    /**
      This function sets provided vertex as a own vertex property for the provided graph.
     *
     * @pre none
     *
     * @param vVertex
     *      value of the vertex
     *
     * @return none
     *
     * @throw none
     */
    void setAsOwnVertex(VertexDescriptor vVertex , SubGraph &gGraph);
	
	/**
      This function sets the upper border value of provided graph property.
     *
     * @pre
     *-# gGraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     *@param vUpperBorderVertex
     *      value of upper border vertex
     *
     * @return none
     *
     * @throw none
     */
    void setGraphUpperBorderVertex(std::size_t vUpperBorderVertex , SubGraph& gGraph);


    /**
      This function sets the upper border value of wrapper member graph property.
     *
     * @pre none
     *
     *@param vUpperBorderVertex
     *      value of upper border vertex
     *
     * @return none
     *
     * @throw none
     */
    void setGraphUpperBorderVertex(std::size_t vUpperBorderVertex);

    
    /**
      This function sets the lower border value of provided graph property.
     *
     * @pre
     *-# gGraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     *@param vLowerBorderVertex
     *      value of lower border vertex
     *
     * @return none
     *
     * @throw none
     */
    void setGraphLowerBorderVertex(std::size_t vLowerBorderVertex , SubGraph& gGraph);


    /**
      This function sets the lower border value of wrapper member graph property.
     *
     * @pre none
     *
     *@param vLowerBorderVertex
     *      value of lower border vertex
     *
     * @return none
     *
     * @throw none
     */
    void setGraphLowerBorderVertex(std::size_t vLowerBorderVertex);
	
	//Swap vertex order in 'graph' as well as 'mapOrderVertex'
    /**
      This function swaps vertex's provided orders in 'graph' as well as 'mapOrderVertex'
     *
     * @pre
     *-# subgraph != NULL
     *
     * @param order1
     *      value of first order
     *
     * @param order2
     *      value of second order
     *
     * @param mapOrderVertex
     *      reference to order vertex map
     *
     * @param mapVertexOrder
     *      reference to vertex order map
     *
     * @return none
     *
     * @throw none
     */
    template <typename MapVertexOrder>
    void swapVertexOrderByOrder(std::size_t order1, std::size_t order2,
                                MapOrderVertex& mapOrderVertex,
                                MapVertexOrder& mapVertexOrder)
    {
        //check
        //mapOrderVertex is a valid MapOrderVertex type map
        //order1 and order2 are valid keys with valid values

        LAYOUT_ASSERT(((order1 > 0) && (order2 > 0)),
                      LayoutException(__FUNCTION__,
                                      LayoutExceptionEnum::INVALID_PARAMETER,
                                      NEGATIVE_ORDER,ORDER_VALUE));
        /* Swapping 'order' value in mapOrderVertex by
        exchanging the vertex desciptors among the order1 and order2 */
        VertexDescriptor vertex1 = mapOrderVertex[order1];
        mapOrderVertex.insert(order1, mapOrderVertex[order2]);
        mapOrderVertex.insert(order2, vertex1);

        //Swapping actual 'order' property values
        PGL_SWAP_VERTEX_ORDER(mapOrderVertex[order1],mapOrderVertex[order2],mapVertexOrder);
    }


    /**
      This function swaps vertex's orders provided vertices in 'graph' as well as 'mapOrderVertex'
     *
     * @pre
     *-# subgraph != NULL
     *
     * @param v1
     *      reference to first vertex
     *
     * @param v2
     reference to second vertex
     *
     * @param mapOrderVertex
     *      reference to order vertex map
     *
     * @param mapVertexOrder
     *      reference to vertex order map
     *
     * @return none
     *
     * @throw none
     */
    template <typename MapVertexOrder>
    void swapVertexOrderByVertex(VertexDescriptor& v1 , VertexDescriptor& v2,
                                 MapOrderVertex& mapOrderVertex,
                                 MapVertexOrder& mapVertexOrder)
    {
        //check
        //mapOrderVertex is a valid MapOrderVertex type map
        //v1 and v2 are valid values in mapOrderVertex

        std::size_t order1 = mapOrderVertex.key(v1);
        std::size_t order2 = mapOrderVertex.key(v2);

        swapVertexOrderByOrder(order1,order2,mapOrderVertex,mapVertexOrder);
    }

    //***Left Shift****
    //LeftShift
    /**
      This function shifts the order of vertex provided order of vartex.
     *
     * @pre none
     *
     * @param iOrderSource
     *      value of order to be shifted
     *
     * @param mapOrderVertex
     *      reference to order vertex map
     *
     * @param enDirection
     *      direction of shifting
     *
     * @return none
     *
     * @throw none
     */
    template <typename MapVertexOrder>
    void leftShiftVertexByOrder(std::size_t iOrderSource,MapOrderVertex& mapOrderVertex,
                                MapVertexOrder& mapVertexOrder)
    {
        //ToDo : check iOrderSource is valid for mapVertexOrder

        //Check iOrder1 is valid key in mapOrderVertex
        LAYOUT_ASSERT(mapOrderVertex.contains(iOrderSource),LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                 , "mapOrderVertex"
                                                 , "iOrderSource"));

        shiftVertexByOrder(iOrderSource,mapOrderVertex,mapVertexOrder,
                           1,LeftDirection);
    }


    /**
      This function shifts the order of vertex provided vartex.
     *
     * @pre none
     *
     * @param vSource
     *      value of vertex
     *
     * @param mapOrderVertex
     *      reference to order vertex map
     *
     * @param enDirection
     *      direction of shifting
     *
     * @return none
     *
     * @throw none
     */
    template <typename MapVertexOrder>
    void leftShiftVertexByVertex(VertexDescriptor& vSource,MapOrderVertex& mapOrderVertex,
                                MapVertexOrder& mapVertexOrder)
    {
        //ToDo : check vOrderSource is valid value from mapVertexOrder

        //Get key for vSource
        std::size_t iOrderSource = mapOrderVertex.key(vSource);

        shiftVertexByOrder(iOrderSource,mapOrderVertex,mapVertexOrder,
                           1,LeftDirection);

    }


    /**
     * Function to shift vertex order value to left by using order values
     *
     * @param iOrderSource
     * @param mapOrderVertex
     * @param mapVertexOrder
     * @param iShiftBy
     *
     * @return none
     *
     * @throw none
     */

    //LeftShift by N
    template <typename MapVertexOrder>

    void leftShiftVertexByOrder(std::size_t iOrderSource,
                                MapOrderVertex& mapOrderVertex,
                                MapVertexOrder& mapVertexOrder,
                                int iShiftBy)
    {
        //ToDo : check iOrderSource is valid for mapVertexOrder


        LAYOUT_ASSERT(iShiftBy>0,LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::INVALID_PARAMETER
                                                 , "must be greater than zero"
                                                 , "iShiftBy"));

        //Check iOrder1 is valid key in mapOrderVertex
        LAYOUT_ASSERT(mapOrderVertex.contains(iOrderSource),LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                 , "mapOrderVertex"
                                                 , "iOrderSource"));

        shiftVertexByOrder(iOrderSource,mapOrderVertex,mapVertexOrder,
                           iShiftBy,LeftDirection);
    }

    /**
     * Function to shift vertex order value to left by using vertices
     *
     * @param vSource
     * @param mapOrderVertex
     * @param mapVertexOrder
     * @param iShiftBy
     *
     * @return none
     *
     * @throw none
     */
    template <typename MapVertexOrder>
    void leftShiftVertexByVertex(VertexDescriptor& vSource,
                                MapOrderVertex& mapOrderVertex,
                                MapVertexOrder& mapVertexOrder,
                                int iShiftBy)
    {
        //ToDo : check vOrderSource is valid value from mapVertexOrder
        LAYOUT_ASSERT(iShiftBy>0,LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::INVALID_PARAMETER
                                                 , "must be greater than zero"
                                                 , "iShiftBy"));

        //Get key for vSource
        std::size_t iOrderSource = mapOrderVertex.key(vSource);

        shiftVertexByOrder(iOrderSource,mapOrderVertex,mapVertexOrder,
                           iShiftBy,LeftDirection);

    }

    //***Rigth Shift****

    //RightShiftS
    /**
     * Function to shift vertex order value to right by using order
     *
     * @param iOrderSource
     * @param mapOrderVertex
     * @param mapVertexOrder
     *
     * @return none
     *
     * @throw none
     */
    template <typename MapVertexOrder>
    void rightShiftVertexByOrder(std::size_t iOrderSource,MapOrderVertex& mapOrderVertex,
                                MapVertexOrder& mapVertexOrder)
    {
        //check : iOrderSource is valid for both maps: mapOrderVertex and mapVertexOrder

        //Check iOrder1 is valid key in mapOrderVertex      
        LAYOUT_ASSERT(mapOrderVertex.contains(iOrderSource),LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                 , "mapOrderVertex"
                                                 , "iOrderSource"));

        shiftVertexByOrder(iOrderSource,mapOrderVertex,mapVertexOrder,
                           1,RightDirection);
    }

    /**
     * Function to shift vertex order value to right by using vertex
     * @param vSource
     * @param mapOrderVertex
     * @param mapVertexOrder
     *
     * @return none
     *
     * @throw none
     */
    template <typename MapVertexOrder>    
    void rightShiftVertexByVertex(VertexDescriptor& vSource,
                                  MapOrderVertex& mapOrderVertex,
                                  MapVertexOrder& mapVertexOrder)
    {
        //ToDo : check vOrderSource is valid value from mapVertexOrder

        //Get key for vSource
        std::size_t iOrderSource = mapOrderVertex.key(vSource);

        shiftVertexByOrder(iOrderSource,mapOrderVertex,mapVertexOrder,
                           1,RightDirection);
    }

    //RightShift by N
    /**
     * Function to shift vertex order value to right by using order values
     *
     * @param iOrderSource
     * @param mapOrderVertex
     * @param mapVertexOrder
     * @param iShiftBy
     */
    template <typename MapVertexOrder>    
    void rightShiftVertexByOrder(std::size_t iOrderSource,MapOrderVertex& mapOrderVertex,
                                MapVertexOrder& mapVertexOrder, int iShiftBy)
    {
        //check : iOrderSource is valid for both maps: mapOrderVertex and mapVertexOrder

        //Check iOrder1 is valid key in mapOrderVertex        
        LAYOUT_ASSERT(mapOrderVertex.contains(iOrderSource),LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                 , "mapOrderVertex"
                                                 , "iOrderSource"));

        shiftVertexByOrder(iOrderSource,mapOrderVertex,mapVertexOrder,
                           iShiftBy,RightDirection);
    }


    /**
     * Function to shift vertex order value to right by using vertex
     *
     * @param vSource
     * @param mapOrderVertex
     * @param mapVertexOrder
     * @param iShiftBy
     */
    template <typename MapVertexOrder>
    void rightShiftVertexByVertex(VertexDescriptor& vSource,
                                  MapOrderVertex& mapOrderVertex,
                                  MapVertexOrder& mapVertexOrder,
                                  int iShiftBy)
    {
        //ToDo : check vOrderSource is valid value from mapVertexOrder

        LAYOUT_ASSERT(iShiftBy>0,LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::INVALID_PARAMETER
                                                 , "must be greater than zero"
                                                 , "iShiftBy"));

        //Get key for vSource
        std::size_t iOrderSource = mapOrderVertex.key(vSource);

        shiftVertexByOrder(iOrderSource,mapOrderVertex,mapVertexOrder,
                           iShiftBy,RightDirection);

    }
	
	//************************* Ordering Criteria Components***************
    /**
     This function orders the vertices of the graph ion the basis of connected components.

    @pre
    -# gGraph != NULL
    -# !mapOrderVertex.empty()

    @param gGraph
        reference to input graph.
    @param mapOrderVertex
        map containing order for vertices.

    @return none

    @throw none
    */
    void applyConnectedComponent(SubGraph &gGraph,MapOrderVertex& mapOrderVertex);

    /**
     This function orders the vertices of the graph in topological manner.

    @pre
    -# gGraph != NULL
    -# !mapOrderVertex.empty()

    @param gGraph
        reference to input graph.
    @param mapOrderVertex
        map containing order for vertices.

    @return none

    @throw none
    */
    void applyTopologicalVertexOrdering(SubGraph &gGraph,MapOrderVertex& mapOrderVertex);

    //************************* Ordering Criteria Components END***********


    //*************************Ordering ***************
    /**
     This function sorts the vertices of the graph in orderly manner.

    @pre
    -# gGraph != NULL
    -# !vecVertexOrder.empty()

    @param gGraph
        reference to input graph.
    @param vecVertexOrder
        map containing vertices for order.

    @return none

    @throw none
    */
    void sortVerticesByOrderList(VectorVertexOrder vecVertexOrder, SubGraph& gGraph);
    //*************************Ordering END***********

    //initialize vertex order values equals to the vertex_index
    /**
    This function initializes the all vertex order values to its index from provided graph.

   @pre
   -# gSubgraph != NULL

   @param gSubgraph
       reference to graph.

   @return none

   @throw none
   */
    void initGraphVertexOrderValues(SubGraph& subgraph);
	
	/**
    This function adds dummy nodes coresponding to each subgraph in the graph

    @pre
    -# gParentGraph != NULL
    -# gChildGraph != NULL

    @param gParentGraph
        referecne to parent graph

    @param gChildGraph
        reference to child graph

     @return vertex descriptor for dummy NodeType

     @throw none
    */
    VertexDescriptor addDummyVertexForChildGraph(SubGraph& gParentGraph, SubGraph& gChildGraph);
	
	// Force Directed Props Getter and Setter
    // TO DO - Complete documentation

    void setVertXDisp(double iDispX,VertexDescriptor verDis,SubGraph &l_graph);

    void setVertYDisp(double iDispY,VertexDescriptor verDis,SubGraph &l_graph);

    void setVertexLock(VertexDescriptor verDis, SubGraph &l_graph);

    void setGraphClusterID(int iClusterID, SubGraph& gSubgraph);

    void setVertexClusterID(int iClusterID, VertexDescriptor vDisc, SubGraph& gSubgraph );

    bool setIsVirtualNode(VertexDescriptor vertex, SubGraph& subgraph);

    void setClusterXDisplacement(SubGraph& gSubgraph,int iDisp);

    void setClusterYDisplacement(SubGraph& gSubgraph,int iDisp);
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type BoostGraphWrapper.
 */

//@{
    //Print Graph with SubGraph
    //for m_Graph
    /**
     This function prints wrapper member graph.

    @pre none

    @param none

    @return none

    @throw none
    */
    void printGraph();
    //for any graph


    /**
     This function prints provided graph.

    @pre none

    @param subgraph
        reference to subgraph

    @return none

    @throw none
    */
    void printGraph(SubGraph& subgraph);


    //Total Vertex Count
    //for m_Graph
    /**
     This function returns total number of vertices present in wrapper member graph.

    @pre none

    @param none

    @return number of vertices

    @throw none
    */
    int numVertices();


    /**
     This function returns total number of vertices present in provided graph.

    @pre none

    @param gSubgraph
        reference to gSubgraph

    @return number of vertices

    @throw none
    */
    int numVertices(SubGraph &gSubgraph);


    /**
     This function returns index for provided vertex present in wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return number of vertices

    @throw none
    */
    int getVertexIndex(VertexDescriptor& vertex);
	
	/**
     This function returns graph from wrapper member graph.

    @pre
    -# subgraph != NULL

    @param none

    @return reference to subgraph

    @throw none
    */
    SubGraph& getGraph();
	
	//vertex order
    //QString sId;
    /**
     This function returns vertex id of provided vertex from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return id of provided vertex

    @throw none
    */
    QString getVertexId(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns vertex id of provided vertex from vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return id of provided vertex

    @throw none
    */
    QString getVertexId(VertexDescriptor& vertex);
	
	/**
     This function returns height for provided vertex from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return height of vertex

    @throw none
    */
    int getVertexHeight(VertexDescriptor& vertex, SubGraph& subgraph);

    /**
     This function returns height for provided vertex from vertex property of wrapper member graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @return height of vertex

    @throw none
    */
    int getVertexHeight(VertexDescriptor& vertex);
	
	//int iWidth;
    /**
     This function returns width for provided vertex from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return width of vertex

    @throw none
    */
    int getVertexWidth(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns width for provided vertex from vertex property of wrapper member graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @return width of vertex

    @throw none
    */
    int getVertexWidth(VertexDescriptor& vertex);
	
	/**
     This function returns left x coordinate for provided vertex from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return left coordinate X of vertex

    @throw none
    */
    int getVertexLeftCoordX(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns left x coordinate for provided vertex from vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return left coordinate X of vertex

    @throw none
    */
    int getVertexLeftCoordX(VertexDescriptor& vertex);
	
	//int iCoord_Y;
    /**
     This function returns left y coordinate for provided vertex from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return left coordinate Y of vertex

    @throw none
    */
    int getVertexLeftCoordY(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns left y coordinate for provided vertex from vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return left coordinate of vertex

    @throw none
    */
    int getVertexLeftCoordY(VertexDescriptor& vertex);
	
	/**
     This function returns center x coordinate for provided vertex from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return center coordinate x of vertex

    @throw none
    */
    int getVertexCenterCoordX(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns center x coordinate for provided vertex from vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return center x coordinate of vertex

    @throw none
    */
    int getVertexCenterCoordX(VertexDescriptor& vertex);
	
	/**
     This function returns center y coordinate for provided vertex from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return center coordinate y of vertex

    @throw none
    */
    int getVertexCenterCoordY(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns center y coordinate for provided vertex from vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return center y coordinate of vertex

    @throw none
    */
    int getVertexCenterCoordY(VertexDescriptor& vertex);
	
	/**
     This function returns whether provided vertex is invisible or not from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return true or false whether the vertex is visible or not

    @throw none
    */
    bool getVertexIsInvisible(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns whether provided vertex is invisible or not from vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return true or false whether the vertex is visible or not

    @throw none
    */
    bool getVertexIsInvisible(VertexDescriptor& vertex);
	
	/**
     This function returns whether provided vertex is expandable or not from vertex property of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return true or false whether the vertex is expandable or not

    @throw none
    */
    bool getVertexExpandable(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns whether provided vertex is expandable or not from vertex property of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return true or false whether the vertex is visible or not

    @throw none
    */
    bool getVertexExpandable(VertexDescriptor& vertex);
	
	/**
     This function returns treewidth of vertex.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return treewidth in integer

    @throw none
    */
    int getVertexTreeWidth(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns treewidth of vertex of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return treewidth in interger

    @throw none
    */
    int getVertexTreeWidth(VertexDescriptor& vertex);
	
	/**
     This function returns distance of vertex from root saved in the vertexproperty of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return distance from root in integer

    @throw none
    */
    int getVertexDistanceFromRoot(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns distance from root of vertex of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return distance from root in interger

    @throw none
    */
    int getVertexDistanceFromRoot(VertexDescriptor& vertex);
	
	/**
     This function returns rank of vertex saved in the vertexproperty of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return rank of vertex in integer

    @throw none
    */
    int getVertexRank(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns rank of vertex of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return rank of vertex in interger

    @throw none
    */
    int getVertexRank(VertexDescriptor& vertex);
	
	/**
     This function returns horizontal position of vertex saved in the vertexproperty of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return horizontal position of vertex in integer

    @throw none
    */
    int getVertexHorizontalPosition(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns horizontal position of vertex of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return horizontal position of vertex in interger

    @throw none
    */
    int getVertexHorizontalPosition(VertexDescriptor &vertex);
	
	/**
     This function returns barry center of vertex saved in the vertexproperty of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return barry center of vertex in integer

    @throw none
    */
    double getVertexBarryCenter(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns barry center of vertex of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return barry center of vertex in interger

    @throw none
    */
    double getVertexBarryCenter(VertexDescriptor &vertex);
	
	//int iTreeLeftX;
    int getVertexTreeLeftX(VertexDescriptor& vertex, SubGraph& subgraph);

    int getVertexTreeLeftX(VertexDescriptor& vertex);
	
	/**
     This function returns type of vertex saved in the vertexproperty of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return type of vertex

    @throw none
    */
    LayoutEnum::NodeType getVertexType(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns type of vertex of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return type of vertex

    @throw none
    */
    LayoutEnum::NodeType getVertexType(VertexDescriptor& vertex);
	
	bool getVertexVisited(VertexDescriptor& vertex, SubGraph& subgraph);

    bool getVertexVisited(VertexDescriptor& vertex);
	
	/**
     This function returns topological order of vertex saved in the vertexproperty of provided graph.

    @pre
    -# subgraph != NULL

    @param vertex
        reference to vertex

    @param subgraph
        reference to subgraph

    @return topological order of vertex

    @throw none
    */
    int getVertexTopologicalOrder(VertexDescriptor& vertex, SubGraph& subgraph);


    /**
     This function returns topological order of vertex of wrapper member graph.

    @pre none

    @param vertex
        reference to vertex

    @return topological order of vertex

    @throw none
    */
    int getVertexTopologicalOrder(VertexDescriptor& vertex);
	
	/**
     This function returns id of edge saved in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @return id of edge

    @throw none
    */
    QString getEdgeId(EdgeDescriptor& edge, SubGraph& subgraph);


    /**
     This function returns id of edge of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @return id of edge

    @throw none
    */
    QString getEdgeId(EdgeDescriptor& edge);
	
	std::size_t getEdgeIndex(EdgeDescriptor& edge, SubGraph& subgraph);

    std::size_t getEdgeIndex(EdgeDescriptor& edge);
	
	/**
     This function returns type of edge direction saved in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @return type of edge direction

    @throw none
    */
    bool getEdgeBidirectional(EdgeDescriptor& edge, SubGraph& subgraph);


    /**
     This function returns type of edge direction of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @return type of edge direction

    @throw none
    */
    bool getEdgeBidirectional(EdgeDescriptor& edge);
	
	/**
     This function returns type of edge invisible saved in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @return type of edge invisible

    @throw none
    */
    bool getEdgeIsInvisible(EdgeDescriptor& edge, SubGraph& subgraph);


    /**
     This function returns type of edge invisible of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @return type of edge invisible

    @throw none
    */
    bool getEdgeIsInvisible(EdgeDescriptor& edge);
	
	bool getEdgeVisited(EdgeDescriptor& edge, SubGraph& subgraph);

    bool getEdgeVisited(EdgeDescriptor& edge);
	
	/**
     This function returns type of edge reversed saved in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @return type of edge reversed

    @throw none
    */
    bool getEdgeReversed(EdgeDescriptor& edge, SubGraph& subgraph);


    /**
     This function returns type of edge reversed of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @return type of edge reversed

    @throw none
    */
    bool getEdgeReversed(EdgeDescriptor& edge);
	
	/**
     This function returns if edge is conflicted/crossing with another edge saved in the edge property of provided graph.

    @pre
    -# subgraph != NULL

    @param edge
        reference to edge

    @param subgraph
        reference to subgraph

    @return if edge is conflicted/crossing with another edge or not

    @throw none
    */
    bool getEdgeIsConflicted(EdgeDescriptor& edge, SubGraph& subgraph);


    /**
     This function returns if edge is conflicted/crossing with another edge of wrapper member graph.

    @pre none

    @param edge
        reference to edge

    @return if edge is conflicted/crossing with another edge or not

    @throw none
    */
    bool getEdgeIsConflicted(EdgeDescriptor& edge);
	
	LayoutEnum::EdgeType getEdgeType(EdgeDescriptor eEdge , SubGraph &gGraph);

    LayoutEnum::EdgeType getEdgeType(EdgeDescriptor eEdge);
	
	/**
      This function iterates the bend points list of the edge.
     * @brief edgeBendPointsIter
     *
     * @param eEdge
     *  graph should contain this edge.
     *
     * @param gSubgraph
     *  -# gSubgraph != NULL
     *
     * @return none
     *
     * @throw none
     */
    std::pair<IteratorQVectorBendPoints, IteratorQVectorBendPoints> edgeBendPointsIter(EdgeDescriptor& eEdge, SubGraph& gSubgraph);
	
	/**
      This function returns id of a provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to graph
     *
     * @return none
     *
     * @throw none
     */
    QString getGraphId(SubGraph& gSubgraph);


    /**
      This function returns id of a wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return id of graph
     *
     * @throw none
     */
    QString getGraphId();
	
	//Index of vertex reperesenting this subgraph - Dummy Node
    //size_t iDummyNodeIndex;
    /**
      This function returns index of this provided graph.
     *
     * @pre
     *  -# gSubgraph != NULL
     *  -# sId should not be empty
     *
     * @param gSubgraph
     *      reference to graph
     *
     * @return index for this graph's node
     *
     * @throw none
     */
    std::size_t getGraphDummyNodeIndex(SubGraph& gSubgraph);


    /**
      This function returns index of this wrapper member graph.
     *
     * @pre none
     *
     * @param none
     *
     * @return index for this graph's node
     *
     * @throw none
     */
    std::size_t getGraphDummyNodeIndex();
	
	/**
      This function returns left x coordinate of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return graph's x left coordinate
     *
     * @throw none
     */
    int getGraphLeftTopCoordX(SubGraph& gSubgraph);

    /**
      This function returns left x coordinate of wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return graph's left x coordinate
     *
     * @throw none
     */
    int getGraphLeftTopCoordX();
	
	/**
      This function returns top y coordinate of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return graph's y top coordinate
     *
     * @throw none
     */
    int getGraphLeftTopCoordY(SubGraph& gSubgraph);


    /**
      This function returns top y coordinate of wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return graph's top y coordinate
     *
     * @throw none
     */
    int getGraphLeftTopCoordY();
	
	/**
      This function returns center coordinate of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return graph's center x coordinate
     *
     * @throw none
     */
    int getGraphCenterCoordX(SubGraph& gSubgraph);


    /**
      This function returns center x coordinate of wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return graph's center x coordinate
     *
     * @throw none
     */
    int getGraphCenterCoordX();
	
	/**
      This function returns center coordinate of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return graph's center y coordinate
     *
     * @throw none
     */
    int getGraphCenterCoordY(SubGraph& gSubgraph);


    /**
      This function returns center y coordinate of wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return graph's center y coordinate
     *
     * @throw none
     */
    int getGraphCenterCoordY();
	
	/**
      This function returns radius of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return radius of graph
     *
     * @throw none
     */
    double getGraphRadius(SubGraph& gSubgraph);


    /**
      This function returns radius of wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return radius of graph
     *
     * @throw none
     */
    double getGraphRadius();
	
	/**
      This function returns height of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return height of graph
     *
     * @throw none
     */
    int getGraphHeight(SubGraph& gSubgraph);


    /**
      This function returns height from wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return height of graph
     *
     * @throw none
     */
    int getGraphHeight();
	
	/**
      This function returns width of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return width of graph
     *
     * @throw none
     */
    int getGraphWidth(SubGraph& gSubgraph);


    /**
      This function returns width from wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return width of graph
     *
     * @throw none
     */
    int getGraphWidth();
	
	int getGraphCoordX(SubGraph& gSubgraph);

    int getGraphCoordX();
	
	int getGraphCoordY(SubGraph& gSubgraph);

    int getGraphCoordY();
	
	/**
      This function returns distance between centroid of graph and center x coordinate of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return distance between centroid of graph and center x coordinate
     *
     * @throw none
     */
    int getGraphDistanceBetweenCentoidAndCenterCoordX(SubGraph& gSubgraph);


    /**
      This function returns distance between centroid of graph and center x coordinate of wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return distance between centroid of graph and center x coordinate
     *
     * @throw none
     */
    int getGraphDistanceBetweenCentoidAndCenterCoordX();
	
	/**
      This function returns distance between y coordinate centroid of graph and center y coordinate of provided graph
     *
     * @pre
     *  -# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return distance between y coorcinate centroid of graph and center y coordinate
     *
     * @throw none
     */
    int getGraphDistanceBetweenCentoidAndCenterCoordY(SubGraph& gSubgraph);


    /**
      This function returns distance between y coordinate centroid of graph and center y coordinate of wrapper member graph
     *
     * @pre none
     *
     * @param none
     *
     * @return distance between centroid of graph and center y coordinate
     *
     * @throw none
     */
    int getGraphDistanceBetweenCentoidAndCenterCoordY();
	
	/**
      This function checks whether provided vertex is own vertex of provided graph.
     *
     * @pre none
     *
     * @param vVertex
     *      value of the vertex
     *
     * @return none
     *
     * @throw none
     */
    bool isOwnVertex(VertexDescriptor vVertex , SubGraph &gGraph);

    /**
      This function iterates the ownvertex list of a provided graph
     *
     * @pre none
     *
     * @param gGraph
     *      reference to graph
     *
     * @return pair of iterators of vector of int type
     *
     * @throw none
     */
    std::pair<IteratorQVectorUInt , IteratorQVectorUInt> ownVerticesIter(SubGraph &gGraph);


    /**
      This function returns the total number of own vertices of the provided graph.
     *
     * @pre
     *-# gGraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     * @return number of own vertices
     *
     * @throw none
     */
    int getCountOfOwnVertices(SubGraph &gGraph);

    //Upper Border Vertex
    //std::size_t vUpperBorderVertex;
    //int iCoordX;
    /**
      This function returns the value of the upper border vertex with respect to provided graph.
     *
     * @pre
     *-# gGraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     * @return value of the upper border vertex
     *
     * @throw none
     */
    std::size_t getGraphUpperBorderVertex(SubGraph& gGraph);


    /**
      This function returns the value of the upper border vertex with respect to wrapper member graph.
     *
     * @pre
     *-# gGraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     * @return value of the upper border vertex
     *
     * @throw none
     */
    std::size_t getGraphUpperBorderVertex();
	
	/**
      This function returns the value of the lower border vertex with respect to provided graph.
     *
     * @pre
     * -# gGraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     * @return value of the lower border vertex
     *
     * @throw none
     */
    std::size_t getGraphLowerBorderVertex(SubGraph& gGraph);


    /**
      This function returns the value of the lower border vertex with respect to wrapper member graph.
     *
     * @pre
     *-# gGraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     * @return value of the lower border vertex
     *
     * @throw none
     */
    std::size_t getGraphLowerBorderVertex();
	
	/**
      This function returns the source vertex of provided edge
     *
     * @pre
     * -# subgraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     * @param edge
     *      edge from the graph
     *
     * @return none
     *
     * @throw none
     */
    VertexDescriptor getEdgeTarget(EdgeDescriptor edge , SubGraph& subgraph);


    /**
      This function returns the target vertex of provided egde from wrapper member graph.
     *
     * @pre none
     *
     *@param edge
     *      value of edge from wrapper member graph
     *
     * @return none
     *
     * @throw none
     */
    VertexDescriptor getEdgeTarget(EdgeDescriptor edge);


    /**
      This function returns the target vertex of provided edge
     *
     * @pre
     *-# subgraph !- NULL
     *
     * @param gGraph
     *      reference to graph
     *
     * @param edge
     *      edge from the graph
     *
     * @return none
     *
     * @throw none
     */
    VertexDescriptor getEdgeSource(EdgeDescriptor edge , SubGraph& subgraph);

    /**
      This function returns the source vertex of provided egde.
     *
     * @pre none
     *
     *@param edge
     *      value of edge from wrapper member graph
     *
     * @return none
     *
     * @throw none
     */
    VertexDescriptor getEdgeSource(EdgeDescriptor edge);
	
	/**
      This function returns the vertices in the pair of vertex iterators form of wrapper member graph.
     *
     * @pre none
     *
     * @return none
     *
     * @throw none
     */
    VertexIterPair getVertices();

    /**
      This function returns the vertices in the pair of vertex iterators form of provided graph.
     *
     * @pre
     * -# subgraph != NULL
     *
     * @param subgraph
     *  reference to subgraph
     *
     * @return pair of vertex iterators
     *
     * @throw none
     */
    VertexIterPair getVertices(SubGraph& subgraph);


    //Children Iterator

    //for m_Graph
    /**
      This function returns pair of children iteartors for getting children of wrapper member graph.
     *
     * @pre none
     *
     * @param none
     *
     * @return pair of children iterators
     *
     * @throw none
     */
    ChildrenIterPair getChildren();


    /**
      This function returns pair of children iteartors for getting children of provided graph.
     *
     * @pre none
     *
     * @param subgraph
     *      reference to graph
     *
     * @return pair of children iterators
     *
     * @throw none
     */
    ChildrenIterPair getChildren(SubGraph& subgraph);

    //in_edges iterator

    /**
      This function returns inedges coming at the vertex provided from wrapper member graph.
     *
     * @pre none
     *
     * @param vertex
     *      reference to vertex
     *
     * @return pair of inedge iterators
     *
     * @throw none
     */
    InEdgeIterPair getInEdges(VertexDescriptor &vertex);


    /**
      This function returns inedges coming at the vertex provided from provided graph.
     *
     * @pre none
     *
     * @param vertex
     *      reference to vertex
     *
     * @param subgraph
     *      reference tosubgraph
     *
     * @return pair of inedge iterators
     *
     * @throw none
     */
    InEdgeIterPair getInEdges(VertexDescriptor &vertex,SubGraph& subgraph);

    //out_edges iterator
    /**
      This function returns outedges coming at the vertex provided from wrapper member graph.
     *
     * @pre none
     *
     * @param vertex
     *      reference to vertex
     *
     * @return pair of outedges iterators
     *
     * @throw none
     */
    OutEdgeIterPair getOutEdges(VertexDescriptor &vertex);


    /**
      This function returns outedges coming at the vertex provided from provided graph.
     *
     * @pre none
     *
     * @param vertex
     *      reference to vertex
     *
     * @param subgraph
     *      reference tosubgraph
     *
     * @return pair of outedges iterators
     *
     * @throw none
     */
    OutEdgeIterPair getOutEdges(VertexDescriptor &vertex,SubGraph& subgraph);

    /**
      This function returns children subgraphs of provided graph.
     *
     * @pre
     *-# gSubgraph != NULL
     *
     * @param gSubgraph
     *      reference to subgraph
     *
     * @return none
     *
     * @throw none
     */
    void getChildrenVector(SubGraph &gSubgraph, VectorSubgraphRefs &vecChildSubgraphs);
	
	/**
      This function returns map of vertices ordered by their 'order' property value.
     *
     * @pre
     *-# subgraph != NULL
     *
     * @param subgraph
     *      reference to subgraph
     * @param mapVertexOrder
     *      reference to vertex order map.
     *
     * @return order vertex map
     *
     * @throw none
     */
    template <typename MapVertexOrder>
    MapOrderVertex getMapOrderedVertices(SubGraph& subgraph,MapVertexOrder& mapVertexOrder)
    {
        //To Do: assert for mapVertexOrder

        MapOrderVertex mapOrderVertex;

        //iterate all vertices
        std::size_t order;

        BGL_FORALL_VERTICES(vertex, subgraph , SubGraph)
        {
            order = mapVertexOrder[vertex];
//            //cout<<"order: "<<order <<" - "<<PGL_VERTEX_INDEX(vertex , subgraph)<<endl;

            //add elements (order,vertex)
            mapOrderVertex.insert(order , vertex);
        }

        return mapOrderVertex;
    }

    /**
      This function checks the provided order is valid or not.
     *
     * @pre
     *-# subgraph != NULL
     *
     * @param order
     *      order value
     *
     * @return valid order or not
     *
     * @throw none
     */
    bool isValidOrder(std::size_t order);


    /**
      This function returns the order of provided vertex.
     *
     * @pre
     *-# subgraph != NULL
     *
     * @param vertex
     *      reference to vertex
     *
     * @return order value
     *
     * @throw none
     */
    int getVertexOrder(VertexDescriptor& vertex , SubGraph& subgraph);
	
	/**
      This function checks whether the shifting of orders of vertices is valid or not.
     *
     * @pre
     *-# subgraph != NULL
     *
     * @param iOrderSource
     *      value of order to be shifted
     *
     * @param iShiftBy
     *      shift by value
     *
     * @param mapOrderVertex
     *      reference to order vertex map
     *
     * @param enDirection
     *      direction of shifting
     *
     * @return whether valid shift or not
     *
     * @throw none
     */
    bool isValidShift(std::size_t iOrderSource,MapOrderVertex& mapOrderVertex,
                      int iShiftBy, ShiftDirection enDirection);
					  
	/**
     This function returns souce vertex x coordinate of provided edge of provided graph.

    @pre
    -# subGraph != NULL

    @param eEdge
        edge value

    @param subGraph
        reference to input graph.


    @return x coordinate of source vertex of edge

    @throw none
    */
    int getEdgeSourceX(EdgeDescriptor eEdge, SubGraph &subGraph);


    /**
     This function returns souce vertex  y coordinate of provided edge of provided graph.

    @pre
    -# subGraph != NULL

    @param eEdge
        edge value

    @param subGraph
        reference to input graph.


    @return y coordinate of source vertex of edge

    @throw none
    */
    int getEdgeSourceY(EdgeDescriptor eEdge, SubGraph &subGraph);


    /**
     This function returns target vertex  x coordinate of provided edge of provided graph.

    @pre
    -# subGraph != NULL

    @param eEdge
        edge value

    @param subGraph
        reference to input graph.


    @return x coordinate of source vertex of edge

    @throw none
    */
    int getEdgeTargetX(EdgeDescriptor eEdge, SubGraph &subGraph);


    /**
     This function returns target vertex  y coordinate of provided edge of provided graph.

    @pre
    -# subGraph != NULL

    @param eEdge
        edge value

    @param subGraph
        reference to input graph.


    @return y coordinate of source vertex of edge

    @throw none
    */
    int getEdgeTargetY(EdgeDescriptor eEdge, SubGraph &subGraph);


    /**
     This function returns count of crossings f edges.

    @pre
    -# subGraph != NULL

    @param gGraph
        reference to input graph.

    @return count of crossings of edges from graph

    @throw none
    */
    int getEdgeCrossingCount(SubGraph & gGraph);
	
	/**
     This function calculates the minimum and maximun numbered vertices from the graph.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph.
    @param iMaxVertex
        value for maximum ordered vertex.
    @param iMinVertex
        value for minimum ordered vertex.

    @return none

    @throw none
    */
    void getMinMaxVertexOrder(SubGraph& gSubgraph, int& iMinVertexOrder, int& iMaxVertexOrder);

    /**
    This function provides the lower index vertex and higher index vertex.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph
    @return none

    @throw none
    */
    void getMinMaxVertices(SubGraph& gSubgraph, VertexDescriptor& vMinVertex, VertexDescriptor& vMaxVertex);
	
	//Convert Boost Edge to QString
    QString edgeToQString(EdgeDescriptor eEdge, SubGraph gGraph);
	
	int getVertXDisp(VertexDescriptor verDis,SubGraph &l_graph );

    int getVertYDisp(VertexDescriptor verDis,SubGraph &l_graph);
	
	bool getIsVertexLocked(VertexDescriptor verDis, SubGraph &l_graph);
	
	int getGraphClusterID(SubGraph& gSubgraph);

    int getVertexClusterID(VertexDescriptor vertex, SubGraph& subgraph);

    bool getIsVirtualNode(VertexDescriptor vertex, SubGraph& subgraph);
	
	int getClusterXDisplacement(SubGraph& gSubgraph);
	
	int getClusterYDisplacement(SubGraph& gSubgraph);
//@}


private:

    //initialize vertex order for newly inserted vertex
    /**
    This function initializes the new inserted vertex order values to its index from provided graph.

   @pre
   -# gSubgraph != NULL

   @param vertex
    reference to vertex

   @param gSubgraph
       reference to graph.

   @return none

   @throw none
   */
    void initVertexOrderValue(VertexDescriptor& vertex,SubGraph& subgraph);


    //***shift vertex order***
    /**
    This function shifts the vertices by its orderin the order vertex map

   @pre
    -# iShiftBy != 0
    -# mapOrderVertex.contains(iOrderSource)

   @param iOrderSource
        value of order in the map

    @param mapOrerVertex
        order to vertex map

    @param mapVertexOrder
        vertex to order map

   @param iShiftBy
        integer value to shift position in map

   @param enDirection
        value for direction left or right

   @return none

   @throw none
   */
    template <typename MapVertexOrder>
    void shiftVertexByOrder(std::size_t iOrderSource, MapOrderVertex& mapOrderVertex,
                            MapVertexOrder& mapVertexOrder, int iShiftBy,
                            ShiftDirection enDirection)
    {
        //Check iShift>0
        LAYOUT_ASSERT(iShiftBy>0,LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::INVALID_PARAMETER
                                                 , "must be greater than zero"
                                                 , "iShiftBy"));

        LAYOUT_ASSERT(mapOrderVertex.contains(iOrderSource),LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                 , "mapOrderVertex"
                                                 , "iOrderSource"));

        //Check iOrder1 is valid key in mapOrderVertex
        if(mapOrderVertex.contains(iOrderSource))
        {
            //Check shifting is valid or not
            bool isValidShiftValue = isValidShift(iOrderSource,mapOrderVertex
                                             ,iShiftBy,enDirection);
            if(isValidShiftValue)
            {
                //shift in enDirection by 1
                MapOrderVertex::iterator iterMapOrderVertex;
                iterMapOrderVertex = mapOrderVertex.find(iOrderSource);

                if(enDirection==LeftDirection)
                {
                    iterMapOrderVertex--;
                }
                else if(enDirection==RightDirection)
                {
                    iterMapOrderVertex++;
                }

                std::size_t iOrderDestination = iterMapOrderVertex.key();

                //Swapping the source with destinition
                swapVertexOrderByOrder(iOrderSource,iOrderDestination,
                                       mapOrderVertex,mapVertexOrder);

                //call shifting again pass: iShiftBy reduced by 1
                iShiftBy--;
                if(iShiftBy>0)
                {
                //Now iOrderDestinition is the iOrderSource
                shiftVertexByOrder(iOrderDestination,mapOrderVertex,mapVertexOrder,
                                   iShiftBy,enDirection);
                }

            }
            else
            {
                cout<<directionEnumString(enDirection).toStdString();
                //cout<<" shift not possible. Invalid shift value:"<<iShiftBy<<"."<<endl;
            }
        }
        else
        {
            //Warning iOrderSource is not a valid order value in map 'mapOrderVertex'
            cout << directionEnumString(enDirection).toStdString();
            //cout<< " shift not possible. Invalid order: "<<iOrderSource<<"."<<endl;
        }

    }

    //Return string for enum ShiftDirection
    /**
    This function returns the direction for the shifting of order.

   @pre
   -# enDirection should be valid direction

   @param enDirection
       value for the direction.

   @return direction in string

   @throw none
   */
    QString directionEnumString(ShiftDirection enDirection);
};




#endif // BOOSTGRAPHWRAPPER_H
