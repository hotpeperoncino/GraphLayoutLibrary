#ifndef SPACEUTILIZER_H
#define SPACEUTILIZER_H
#include <GraphLayoutLibrary_global.h>
#include <Common/BoostGraphWrapper.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <qmath.h>
#include <CircularLayout/CircleLayouter.h>
#include <Common/ConstantType.h>

// Constant to define the negative factor
#define NEGATIVE_FACTOR -1;
#define PI_VALUE_DEGREE 180.0
#define TWO_PI_VALUE_DEGREE 360.0


#define PGL_MAP_VERTEX_TOPOLOGICAL_ORDER(mapVertexTopologicalOrder,graph) \
    property_map<SubGraph , std::size_t VertexProperties::*>::type \
    mapVertexTopologicalOrder(get(&VertexProperties::iTopologicalOrder , graph));

/**
 * A structure to store the properties of the Circle; used in circular layout.
 */
typedef struct CircleProperty
{
    int iCenterCoordX; /*!< x coordinate of the center */
    int iCenterCoordY; /*!< y coordinate of the center */
    double dRadius; /*!< Radius */
}CircleProperty;

/**
 * @brief The SpaceUtilizer class
 *
 * The class provides set of functionalities around circular layout of a graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT SpaceUtilizer
{

private:
    /**
     * Refers to an empty object of BoostGraphWrapper
     */
    BoostGraphWrapper m_boostGraphWrapper;
    /**
     * Stores the properties of the outermost circle
     */
    CircleProperty m_structMainCircleProperty;

    /**
     * Iterator for child items of the subgraph.
     */
    ChildrenIterator m_ItrSubgraph;

    /**
     * Iterator for child items of the subgraph. This iterator initially points to the end.
     */
    ChildrenIterator m_ItrSubgraphEnd;

    typedef QMap<VertexDescriptor, SubGraph *> mapDummyVertexToSubgraph;

    /**
     * Map of vertext descriptor and its associated subgraph for all dummy vertices added to the graph.
     */
    mapDummyVertexToSubgraph m_mapDummyVertexToSubgraph;    

public:
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type SpaceUtilizer.
  */
 //@{
    /**
        constructs the object of type SpaceUtilizer.
    */
    SpaceUtilizer();
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type SpaceUtilizer.
 */
//@{
    /**
     This function provides data required for calculating intersection of circles .

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        referece to graph
    @param vectIntersetionPoints
        out parameter containing the list of intersection points

    @return none

    @throw none
    */
    void getCircleIntersection(SubGraph& gSubgraph, vector<int>& vectIntersetionPoints);


    /**
     This function calculates intersection coordinates of circles.

    @pre
    -# structSubCircleProperty != NULL

    @param structSubCircleProperty
        object of structure containing circle properties
    @param vectIntersetionPoints
        reference to list of integer values

    @return none

    @throw none
    */
    void getCircleIntersectionCoordinates(CircleProperty structSubCircleProperty, vector<int>& vectIntersetionPoints);


    /**
     This function calculates angle of intersection point of circles using x coordinate value.

    @pre
    -# gSubgraph != NULL

    @param iCoordX
        x coordinate integer value
    @param iCoordY
        y coordinate integer value
    @param gSubgraph
        reference to graph

    @return angle value in degrees

    @throw none
    */
    double getIntersectionPointAngle(int iCoordX,int iCoordY, SubGraph& gSubgraph);


    /**
    This function provides the min and max vertices for next level subgraphs in the graph.

    @pre
    -# gGraph != NULL

    @param gGraph
        referece to graph
    @param vectMinMaxVertices
        reference to list of vertex descriptors

    @return none

    @throw none

    */
    void getSubgraphMinMaxVertices(SubGraph& gGraph, vector<VertexDescriptor>& vectMinMaxVertices);


    /**
    This fuction provides the min ans max vertices order for next level subgraphs in the graph.

    @param gGraph
        reference to graph
    @param vectMinMaxVerticesOrder
        out parameter containing the indices of the minimum order vertex and maximum order vertex.

    @return none

    @throw none

    */
    void getSubgraphMinMaxVerticesOrder(SubGraph& gGraph, vector<int>& vectMinMaxVerticesOrder);


    /**
    This function provides the min and max vertices for next level subgraphs in the graph.

    @pre
    -# gGraph != NULL

    @param gGraph
        referece to graph
    @param iMinOrder
        index of vertex with minimum order
    @param iMaxOrder
        index of vertex with maximum order
    @param mapMinMaxOrderVertex
        reference to list of vertex descriptors occurring between minimum and maximum order vertices.

    @return none

    @throw none

    */
    void claculateVerticesBetweenMinMax(SubGraph& gGraph,int iMinOrder, int iMaxOrder, MapOrderVertex& mapMinMaxOrderVertex);
	
	/**
    Returns map of vertices ordered by their 'topological order' property value.
    @pre
    -# gGraph != NULL

    @param gGraph
        referece to graph

    @return MapOrderVertex

    @throw none

    */
    template <typename MapVertexOrder>
    MapOrderVertex getMapTopologicalOrderedVertex(SubGraph& gSubgraph,MapVertexOrder& mapVertexOrder)
    {
        MapOrderVertex mapOrderVertex;

        BGL_FORALL_VERTICES(vVertex, gSubgraph , SubGraph)
        {
            std::size_t iTopologicalOrder;
            iTopologicalOrder = gSubgraph[vVertex].iTopologicalOrder;
            //cout<<"Topological Order---------: "<<iTopologicalOrder <<" - "<<vVertex<<endl;
            //add elements (order,vertex)
            mapOrderVertex.insert(iTopologicalOrder , vVertex);
        }
        return mapOrderVertex;
    }


    /**
    This function prepares list of own vertices of the graph

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @return none

    @throw none
    */
    void prepareGraphOwnVertexList(SubGraph& gSubgraph,QMap<size_t,VertexDescriptor>& mapOwnVerticesOrderToVertex);
	
	/**
    This function returns the number of vertices associated with the provided dummy node.

    @pre
    -# vDummyVertex should be dummy node

    @param vDummyVertex
        reference to dummy vertex

    @return count of vertices

    @throw none
    */
    int getVertexCountForDummyNode(VertexDescriptor& vDummyVertex);

    /**
    This function calculates the median ordered vertex order considering whlole graph.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @return median order of vertex.

    @throw none
    */
    int calculateMedianOfSubgraph(SubGraph& gSubgraph);
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of SpaceUtilizer.
  */
 //@{
    /**
    This function processes the grapth and tries to make it compact to make it utilize the space maximully.
    @pre
    -# gGraph != NULL

    @param gGraph
        referece to graph

    @return none

    @throw none

    */
    void processSpaceUtilizer(SubGraph& gGraph);

    /**
    This function adds dummy vertices into parent graph and creates entry into
    child subgraph as dummy vertex for that child subgraph
    prepares map of vertex to child subgraph.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph
    @param vVertex
        VertexDescriptor of the dummy vertex to be added to gSubgraph.
    @return none

    @throw none
    */
    void addSubgraphDummyVerticesAndMap(SubGraph& gSubgraph, VertexDescriptor vVertex);

    


    /**
    This function processes graph for second pass of circular layout.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @return none

    @throw none
    */

    /**
     * @brief processSecondPassCircularLayout
     * This function processes graph for second pass of circular layout.
     * @pre
       -# gSubgraph != NULL
     * @param gSubgraph
     *     reference to graph
     * @param iCenterCoordX
     *     X coordinate of the center point
     * @param iCenterCoordY
     *     Y coordinate of the center point
     */
    void processSecondPassCircularLayout(SubGraph& gSubgraph, int iCenterCoordX, int iCenterCoordY);
	//@}

};

#endif // SPACEUTILIZER_H
