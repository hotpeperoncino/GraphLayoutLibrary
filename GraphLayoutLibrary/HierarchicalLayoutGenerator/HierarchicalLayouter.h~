#ifndef HIERARCHICALLAYOUTER_H
#define HIERARCHICALLAYOUTER_H

#include <GraphLayoutLibrary_global.h>


#include <Common/BoostGraphWrapper.h>
#include <common/GraphCycleHandler.h>
#include <Common/CustomDFSVisitors.h>
#include <Common/LayoutUtility.h>
#include <Common/ConstantType.h>
#include <boost/graph/breadth_first_search.hpp>

#include <HierarchicalLayoutGenerator/HierarchicalLayoutTypedefs.h>
#include <HierarchicalLayoutGenerator/SubgraphOrderingGraph.h>
#include <HierarchicalLayoutGenerator/SubgraphOrderingGraphDFSVisitor.h>


#include <QHash>
#include <array>
#include <QQueue>
#include <QStack>
#include <QtAlgorithms>
#include <boost/circular_buffer.hpp>
//Required to assess time taken by Hierarcical Layout Processes
#include <QTime>
#include <QFile>
#include <HierarchicalLayoutGenerator/HierarchicalLayoutTestingConstants.h>
#include <QObject>


#define UNDEFINED_POS INT_MIN
#define INFINITY_INT INT_MAX
#define HORIZONTAL_UNIT_SPACE 1

//This macro allows or denies if the calculated bend points are to be added to respective edges
//if its value is 'true' then the bend points will be added to edges which should have them and  subsequently
//graphml writer will write them in output graphml
//It is introduced to set to avoid bend points in output , because 'ScoMos' application does not
//handle bend points
#define ALLOW_BEND_POINTS false


/**
 * @brief The HierarchicalLayouter class
 *
 * The class generates hierarchical layout of a graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT HierarchicalLayouter
{

public:

/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type HierarchicalLayouter.
 */
//@{
	/**
     Constructs new object of type HierarchicalLayouter.
    @pre none

    @param none

    @return none

    @throw none
    */
    HierarchicalLayouter();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of HierarchicalLayouter.
  */
 //@{
    /**
      This function accepts boost graph and lay it out hierarchical manner.

      @pre
        -# gGraph != NULL

      @param gGraph
        reference to the graph

      @param iNodeSeparation
        constant to decide separation between adjucent nodes

      @param iEdgeSeparation
        constant to decide separation between edge to nodes and other edges

      @param iLayerSeparation
        constant to decide separation between layers in hierarchy

      @param iBorderMargin
        constant to decide margin for cluster (subgraph) border

      @return none

      @throws none


    */
    void applyHierarchicalLayout(SubGraph &gGraph, int iNodeSeparation, int iEdgeSeparation, int iLayerSeparation, int iBorderMargin);

    /**
      This function accepts boost graph and lay it out hierarchically

      @pre
        -# gGraph != NULL

      @param gGraph
        reference to the graph

      @return none

      @throws none
    */

    void applyHierarchicalLayout(SubGraph &gGraph);

    /**
      This function sets execution log csv file name

      @pre
      -# m_sExecutionLogFileName.isEmpty() == false

      @param m_sExecutionLogFileName
      execution log filename string

      @return none

      @throws none
    */
    void setExecutionLogFileName(QString sFileName);
//@}


    QString m_sExecutionLogFileName;  /*!< Name of log file >*/

    SubGraph *m_gMainGraph; /*!< Pointer reference to main graph object >*/

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type HierarchicalLayouter.
 */
//@{
    /**
      This function returns final crossings present in the graph if the value is set
      @pre none

      @param none

      @return none

      @throws none
     */
    int getFinalCrossings();
//@}

    /*! This is map of all layers, key is the integer value of the rank of each layer,
     *  value is pointer to the layer of LayerNodes
     */
private:

    /** Map of layer ids (integer) to layer references **/
    MapLayerIdToLayerRef m_mapLayeredGraph;

    /** Boost graph wrapper object **/
    BoostGraphWrapper m_BoostGraphWrapper;

    ConstantType<int> m_iNestingDepth; /*!< Nesting depth of the graph >*/
    ConstantType<int> m_iRankDifferenceInLayers; /*!< Rank difference between two layers  >*/
    ConstantType<int> m_iTotalLayers; /*!< Total number of layers in graph >*/

    /*! 'VertexPair' type is defined to use to store the source and target vertices of edge
    Because once the edge is deleted from graph the EdgeDescriptor becomes invalid */
    typedef std::pair<VertexDescriptor , VertexDescriptor > VertexPair;




    //Map of deleted long edges with their dummy vertices
    QMap<VertexPair , QVector<VertexDescriptor> * > m_mapDeletedLongEdgesToDummyVertex;

    typedef QMapIterator<VertexPair , QVector<VertexDescriptor> * > IteratorMapDeletedLongEdgeToVectorDummyVertex;

    //Map of reversed edges
    QMultiMap<VertexDescriptor , VertexDescriptor> m_mapReversedEdges;

    QMultiMap<VertexPair , EdgeProperties*> m_mapDeletedEdgeProperties; /*!< Map of vertex pairs to deleted edge properties >*/

    QMultiMap<VertexPair , EdgeProperties*> m_mapReversedEdgeProperties; /*!< Map of vertex pairs to reversed edge properties >*/

    QMultiMap<VertexPair, EdgeDescriptor> m_mapVertexPairToReversedEdgeDescriptor; /*!< Map of vertex pairs to edge  >*/






    /*! This is global hash map of layer nodes, to which the Layered
     *  Graph and Nesting Tree will point so the changes will be
     *  consistent throughout because the use of these common
     *  data objects
     */
    QHash<VertexDescriptor , LayerNode*> hashVertexToLayerNode;
    typedef QHashIterator<VertexDescriptor , LayerNode*> IteratorHashVertexToLayerNode;




    MapLayerIdtoReducedNestedTreeRef m_mapLayerReducedNestedTree;

    /*! Nesting tree stores the ownership relation of vertices to
     *  their direct parent graph and from subgraphs to their parent graphs
     */
    NestingTreeSubgraphNode m_rootNestingTreeSubgraphNode;




    QVector<int> m_vecSortedRanks; /*!< sorted vector of ranks >*/

    //*************************************************************************

    int m_iFinalCrossings; /*!< Final crossing counter >*/


    //******************************POSITIONING *******************************

    typedef QVector<double> QVectorDouble; /*!< vector of double >*/

    typedef QVector<QMap<int , double> > VecMapShiftClass; /*!< Vector for holding shift value of classes of vertices from BK algorithm >*/

    double dDelta;

    ConstantType<int> m_iReductionParameterHorizontal; /*!< This parameter decides the total number of pixels per unit horizontal position >*/

    ConstantType<int> m_iNodeSeparation;    /*!< Node separation >*/
    ConstantType<int> m_iEdgeSeparation;    /*!< Edge separation >*/
    ConstantType<int> m_iRankSeparation;    /*!< Rank separation >*/
    ConstantType<int> m_iBorderMargin;      /*!< Edge separation >*/

    //*************************************************************************

    /**
    This function counts the graphs nesting depth

        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the graph

        @return none

        @throws none
    */
    int getGraphNestingDepth(SubGraph& gGraph);

    /**
    This function creates a nested tree by adding border nodes and nesting edges

        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the graph

        @return none

        @throws BoostException

        @throws LayoutException

        @throws MemoryException
    */
    void createNestingGraph(SubGraph &gGraph);

    /**
    This function adds upper and lower border vertices and nesting edges in the child graph and sets the references to the vChildUpperBorder and vChildLowerBorder vertices.

        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the child graph

        @param vUpperChildNode
            reference to the upper border vertex of child graph

        @param vLowerChildNode
            reference to the lower border vertex of child graph

        @return none

        @throws none
    */
    void addNestingToChildByRecur(SubGraph& gGraph , VertexDescriptor& vUpperBorder , VertexDescriptor& vLowerBorder);

    /**
      This function assigns ranks to every vertex in the tree graph
        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the graph

        @return none

        @throws LayoutException
        -# REQUIRED_PARAMETER_NOT_SET if nesting depth is not set
        -# INVALID_OPERATION if upper border vertex index is not less by 1 than lower border vertex

        @throws BoostException

        @throws MemoryException

      */
    void assignVertexRanks(SubGraph &gGraph , VertexDescriptor vRootVertex);




    /**
      This function adjusts vertices rank
        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the graph

        @return none

        @throws LayoutException
        -# REQUIRED_PARAMETER_NOT_SET if nesting depth is not set

      */
    void adjustVertexRanks(SubGraph &gGraph);

    /**
      This function pulls assigned ranks of every vertex if it can be pulled up. This reduces some long edges and therefor the dummy nodes
        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the graph

        @return none

        @throws LayoutException
        -# REQUIRED_PARAMETER_NOT_SET if nesting depth is not set
        -# INAVALID_OPERATION if upper border vertex index is not less by 1 than lower border vertex


      */
    void pullUpVertexRanks(SubGraph &gGraph , VertexDescriptor vRootVertex);

    /**
      This function pulls assigned ranks of every vertex if it can be pulled up. This reduces some long edges and therefor the dummy nodes
        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the graph

        @return none

        @throws none

      */
    void pullAndSpreadUpVertexRanks(SubGraph &gGraph , VertexDescriptor vRootVertex);


    /**
      This function records unique rank values and sort them
        @pre none

        @param none

        @return none

        @throws none

      */
    void recordUniqueRankAndSort();

    /**
      This function removes nesting long edges
        @pre none

        @param none

        @return none

        @throws none

      */
    void removeNestingEdges();

    /**
      This function splits the long edges using dummy nodes and add  Dummy node LayerNode, adds it to Nesting Tree also
        @pre none

        @param none

        @return none

        @throws LayoutException
        -# INCONSISTENT_DATASTRUCTURE if boost graph contains duplicate long edges
      */
    int splitLongEdgesAndUpdateNestingTree();


    /**
      This function return edge span. It is the difference between source and destinition vertex rank values.

        @pre
            -# eEdge != NULL

        @param eEdge
            edge

        @return int
            edge span value

        @throws none

      */
    int edgeSpan(EdgeDescriptor eEdge, SubGraph &gGraph);

    /**
      This function removes the cycles from the graph by removing back edges. Records the back edges into provided vector of edges.

        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the graph

        @param vectBackEdges
            vector of edges to record the back edges which creates cycle

        @return none

        @throws Boost Exception

        @throws LayoutException

      */
    void removeCycle(SubGraph& gGraph ,  VectorEdgeDescriptor& vectBackEdges);


    /**
      This function initiates hashVertexToLayerNode

        @pre none

        @param none

        @return none

        @throws none

      */
    void initHashVertexToLayerNode(SubGraph &gGraph);

    /**
      This function is for creating root node of nesting tree, it generates nesting tree by internally calling its recursive version

        @pre none

        @param none

        @return none

        @throws BoostException

      */
    void generateNestingTree();

    /**
      This function creates nesting tree recursively

        @pre none

        @param none

        @return none

        @throws none

      */
    void generateNestingTreeByRecur(NestingTreeSubgraphNode &nestingTreeSubgraphNodes , SubGraph &gRootGraph);

    /**
      This function returns path between provided nesting tree nodes

        @pre
        -# &sourceSubgraphNode != NULL
        -# &targetSubgraphNode != NULL

        @param none

        @return path between source and target subgraph nodes in nesting tree in a queue of nesting subgraph node

        @throws none

      */
    void getNestingTreeSubgraphNodesPath(NestingTreeSubgraphNode& sourceSubgraphNode , NestingTreeSubgraphNode& targetSubgraphNode , QueueNestingTreeSubgraphNodesRef &qNestingTreeSubgraphNodesRef);

    /**
      This function generates LayeredGraph, creates Layers of LayerNodes which have same rank as the LayerId of the Layer, and creates Map of Layers thus provides access to the graph Horizontally (Layer wise).

        @pre
        -# gRankedRootGraph.is_root() == true

        @param none

        @return none

        @throws none

      */
    void generateLayeredGraph();


    /**
      This function reversed the layer node sequence and updates the new positions into the vertex horizontalPosition property

        @pre none

        @param none

        @return none

        @throws none

      */
    void reverseLayerNodesPositions(int iLayerId);

    /**
      This function reverses the layered graph layers horizontally

        @pre none

        @param none

        @return none

        @throws LayoutException
        -# INCONSISTENT_DATASTRUCTURE if layered graph becomes inconsistent while reversing layers horizontally

        @throws BoostException

        @throws MemoryException

      */
    void reverseLayeredGraphHorizontaly();
    /**
      This function reverses the layered graph layers vertically. But it does not update the rank values of vertices

        @pre none

        @param none

        @return none

        @throws MemoryException

      */
    void reverseLayeredGraphVertically();

    /**
      This function restores the BackEdges and Long edges. Adds bend points to the long edges

        @pre none

        @param none

        @return none

        @throws none

      */
    void restoreReversedAndLongEdgesWithBendPoints();

    /**
      This function stores edge properties and returns a pointer to the structure

        @pre none

        @param none

        @return a pointer to the new EdgeProperties storing structure

        @throws none

      */
    EdgeProperties* recordEdgeProperties(EdgeDescriptor& eGlobalEdge);

    /**
      This function sets edge's stored properties

        @pre none

        @param none

        @return none

        @throws none

      */
    void setEdgeProperties(EdgeDescriptor& eGlobalEdge , EdgeProperties* edgeProperties);


    //****************************CROSSING REDUCTION*******************************************************

    /**
      This function reduces edge crossings without considering the subgraph using BarryCenter Method

        @pre
        -# gMainGraph.is_root() == true

        @param gMainGraph
        reference to main graph

        @return none

        @throws none

      */
    void globalCrossingReducion(SubGraph& gMainGraph);

    /**
      This function reduces edge crossings without considering the subgraph using BarryCenter Method

        @pre
        -# m_mapLayeredGraph.contains(iLayerId) == true

        @param iLayerId
        layer id (rank) of Layer to be sorted by barrycenters

        @param enDirection
        direction of barry center weights calculation

        @return none

        @throws LayoutException
        -# INVALIS_OPERATION if first layer id in the provided direction is given for sorting by barycenters
        -# NOT_FOUND_IN_CONTAINER if given layer id is not fond in layered graph

        @throws BoostException

      */
    void sortLayerByBarryCenters(int iLayerId , ProcessDirection enDirection , NeighborNodes enNeighborNodes);


    /**
      This function reduces edge crossings by considering the subgraphs

        @pre
        -# gMainGraph.is_root() == true

        @param gMainGraph
        reference to main graph

        @param iCrossingReductionDegree
        parameter to control crossing reduction process iterations

        @return none

        @throws LayoutException

        @throws BoostException

        @throws MemoryException

      */
    void reduceEdgeCrossings(int iCrossingReductionDegree);

    /**
      This function reduces edge crossings using BarryCenter method and ReducedNestingTree method. It places nodes of each subgraph in unbroken sequence on each layer while reducing the crossings.

        @pre none

        @param bIsUpwardDirection
        boolean to decide the direction

        @return none

        @throws LayoutException

        @throws BoostException

        @throws MemoryException

      */
    void crossingReductionByReducedNestingTree(bool bIsUpwardDirection);

    /**
      This function reduces edge crossings using SubgraphOrderingGraph method. It places nodes of each subgraph in unbroken sequence on all layers.

        @pre none

        @param bIsUpwardDirection
        boolean to decide the direction

        @return none

        @throws LayoutException
        -# INCONSISTENT_DATASTRUCTURE if subgraph ordering graph is inconsistent

        @throws BoostException

        @throws MemoryException

      */
    void crossingReductionBySubgraphOrderingGraph(bool bIsUpwardDirection);

    /**
      This function sorts all nodes by only single subgraph ordering graph.

        @pre none

        @param none

        @return none

        @throws none

      */
    void crosingReductionFinalTopologicalSorting();

    /**
      This function sorts the layerNodes according to their vertex iTopological Order Property

        @pre none

        @param iLayerId
        layer id of the layer to be sorted

        @return none

        @throws LayoutException
        -# NOT_FOUND_IN_CONTAINER if layer id is not found in layered graph
        -# INCONSISTENT_DATASTRUCTURE if layer is not properly sorted by topological order or node positions are inconsistent with their horizontal positions in graph

      */
    void sortLayerByTopologicalOrder(int iLayerId);


    /**
      This function reduces edge crossings without considering the subgraph using BarryCenter Method

        @pre
        -# m_mapLayeredGraph.contains(iLayerId) == true
        -# nestingTreeRootNode != NULL

        @param iLayerId
        layer id (rank) of Layer

        @param nestingTreeRootNode
        root nesting tree node

        @param reducedNestingTreeNode
        outparameter reduced nesting tree node

        @return none

        @throws LayoutException
        -# INVALID_PARAMETER if invalid layer id is passed
        -# INCONSISTENT_DATASTRUCTURE if incorrect ReducedNestingTree or Layer is generated

        @throws BoostException

      */
    void doSortedTraversalByReducedNestingTree(int iLayerId );

    /**
      This function creates reduced nested tree for given iLayerId

        @pre
        -# m_mapLayeredGraph.contains(iLayerId) == true
        -# nestingTreeRootNode != NULL

        @param iLayerId
        layer id (rank) of Layer

        @param nestingTreeRootNode
        root nesting tree node

        @param reducedNestingTreeNode
        outparameter reduced nesting tree node

        @return none

        @throws none

      */
    void createReducedNestedTree(int iLayerId , NestingTreeSubgraphNode &nestingTreeRootNode , ReducedNestingTreeNode &reducedNestingTreeNode);

    /**
      This function creates Layer sorted according to the provided ReducedNestingTree

        @pre
        -# newLayer.isEmpty() == true

        @param newLayer
        out parameter layer

        @param reducedNestingTreeRootNode
        root node of reduced nesting tree

        @return none

        @throws none

      */

    void generateLayerByReducedNestingTree(MapPositionToLayerNode &newLayer , ReducedNestingTreeNode &reducedNestingTreeRootNode);

    /**
      This is recursive function to add LayerNodes sorted according to the provided ReducedNestingTree in provided Layer

        @pre
        -# iHorizontalPosition > 0

        @param iHorizontalPosition
        integer to provide horizontal position to LayerNodes in new Layer

        @param newLayer
        out parameter layer

        @param reducedNestingTreeRootNode
        root node of reduced nesting tree

        @return none

        @throws none

      */

    void generateLayerByReducedNestingTreeRecur(int &iHorizontalPosition , MapPositionToLayerNode &newLayer , ReducedNestingTreeNode &reducedNestingTreeRootNode);

    //Function to get upper/lower layer id from current layer id
    /**
      This function gives next layer in the provided direction of the provided layer type

        @pre
        -# m_mapLayeredGraph.contains(iLayerId) == true

        @param iLayerId
        layer id

        @param enDirection
        direction enum

        @param enLayerType
        layer type enum for specifying type of layer

        @return none

        @throws none

      */
    int getNextLayerId(int iLayerId , ProcessDirection enDirection , LayerType enLayerType = AnyTypeLayer);

public:
    /**
      This function gives number of crossings in a layered graph

        @pre none

        @param none

        @return none

        @throws LayoutException
        -# INVALID_OPERATION if any two layer upper and lower layer edge sources queue and targets queue respectively, size mismatches

        @throws BoostException

        @throws MemoryException

      */
    int getTotalCrossings();

private:

    //***************************CROSSING REDUCTION END************************************************************
    //***************************SUBGRAPH ORDERING GRAPH************************************************************
    typedef QMap<NestingTreeSubgraphNode* , SubgraphOrderingGraphType*> MapNestingTreeRefToSubgraphOrderingGraphRef;
    typedef QMapIterator<NestingTreeSubgraphNode* , SubgraphOrderingGraphType*> IteratorMapNestingTreeRefToSubgraphOrderingGraphRef;

    MapNestingTreeRefToSubgraphOrderingGraphRef m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef;

    SubgraphOrderingGraphWrapper m_subgraphOrderingGraphWrapper;

    /**
      This function creates empty subgraphOrderingGraphs for each NestingTreeSubgraphNode and stores its entry in global map m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef

        @pre none

        @param none

        @return none

        @throws LayoutException
        -# INCONSISTENT_DATASTRUCTURE if invalid Subgraph Ordering Graph is created

        @throws BoostException


      */
    void createEmptySubgraphOrderingGraphsForEachNestingTreeSubgraphNode();

    //Generate SubgraphOrdering Graph
    /**
      This function generates nodes and edges into empty subgraphs created for each Nesting Tree Subgraph Node

        @pre none

        @param none

        @return none

        @throws LayoutException

        @throws BoostException

      */
    void generateSubgraphOrderingGraph();

    /**
      This function adds layer nodes and edges accoring to the 'left of' relations into Subgraph Ordering Graphs

        @pre none

        @param none

        @return none

        @throws LayoutException
        -# NOT_FOUND_IN_CONTAINER if Left node nesting tree parent tree does not contain Root nesting tree node or if any vertex is not found in Subgraph Ordering Graph
        -# INVALID_OPERATION if finding common parent for any two neighbor nodes process fails
        -# INVALID_PARAMETER if left or right nesting tree node index in generated list of parent nesting tree nodes found to be a negative value

      */
    void populateNodesAndEdgesInSubgraphOrderingGraph();

    /**
      This function adds layer node to its own parent NestingTreeSubgraphNode's  SubgraphOrderingGraph

        @pre
        -# layerNode->getSubgra

        @param layerNode
        layer node

        @return none

        @throws none

      */
    void addLayerNodeToOwnParentSubgraphOrderingGraph(LayerNode* layerNode);

    /**
      This function gives parent SubgraphOrderingGraph pointer from nesting tree node

        @pre none

        @param nestingTreeNode
        nesting tree node

        @return pointer to SubgraphOrderingGraph pointer for nesting tree node

        @throws none

      */
    SubgraphOrderingGraphType* getParentSubgraphOrderingGraph(NestingTreeSubgraphNode* nestingTreeNode);

    //Give topological order to vertices according to Subgraph Ordering Graph

    //Sort Layered Graph According to Subgraph Ordering Graph

    //Remove cycles in Subgraph Ordering Graphs
    /**
      This function removes cycles from SubgraphOrderingGraphs

        @pre none

        @param none

        @return none

        @throws LayoutException
        -# EMPTY_CONTAINER if map nesting tree nodes to their Subgraph ordering graph is empty

        @throws BoostException

        @throws MemoryException

      */
    void removeCycleFromSubgraphOrderingGraph();

    /**
      This function break cycle created by provided node nodes at node among the all participating nodes in cycle which has least most average barrycenter

        @pre
        -# gSubgraphOrderingGraph.find_edge(eBackEdge).second == true

        @param eBackEdge
        a back edge detected in SubgraphOrderingGraph

        @param gSubgraphOrderingGraph
        SubgraphOrderingGraph

        @return none

        @throws LayoutException
        -# NOT_FOUND_IN_CONTAINER if back edge is not found in Subgraph Ordering Graph
        -# INVALID_PARAMETER if the back edge is actually a self loop i.e. an edge from a vertex to itself

        @throws BoostException

        @throws MemoryException

      */
    void breakCycleAtSmallestAverageBarryCenterNode(SubgraphOrderingGraphEdgeDescriptor eBackEdge , SubgraphOrderingGraphType& gSubgraphOrderingGraph);


    /**
      This function assigns topological order according to SubgraphOrderingGraph to all vertices of original graph

        @pre
        -# iStartOrder > 0
        -# m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.contains(nestingTreeNode)

        @param iStartOrder
        the order value to give to first vertex in the SubgraphOrderingGraph pointed by provided NestingTreeSubgraphNode

        @param nestingTreeNode
        nesting tree node which points to the SubgraphOrderingGraph

        @return the next value of last order value assigned in SubgraphOrderingGraph

        @throws LayoutException
        -# INVALID_PARAMETER if Start Order for topological order of subgraph ordering graph is zero or negative
        -# NOT_FOUND_IN_CONTAINER if nesting tree node entry not found in map of nesting tree nodes to their subrgaph ordering graphs
        -# INVALID_OPERATION if operation of assigning topological order to nodes of leaf subgaph ordering graph fails

      */
    int assignTopologicalOrderAccordingToSubgraphOrderingGraph(int iStartOrder , NestingTreeSubgraphNode* nestingTreeNode);


    /**
      This function calculate and update AverageBarryCenter weights of NestingTreeNodes in SubgraphOrderingGraph

        @pre none

        @param none

        @return none

        @throws LayoutException
        -# EMPTY_CONTAINER if map of nesting tree nodes to their subgraph ordering graph object references is empty

      */
    void updateAverageBarryCenterForNestingTreeNodes();

    /**
      This function gives average BarryCenter weight of vertex in SubgraphOrderingGraph efficiently

        @pre
        -# gSubgraphOrderingGraph.find_vertex(vVertex).second == true

        @param vVertex
        vertex of SubgraphOrderingGraph

        @param gSubgraphOrderingGraph
        SubgraphOrderingGraph

        @return average barrycenter position of provided vertex

        @throws none

      */
    double getSubgraphOrderingGraphVertexAverageBarryCenter(SubgraphOrderingGraphVertexDescriptor vVertex , SubgraphOrderingGraphType &gSubgraphOrderingGraph);

    //***************************SUBGRAPH ORDERING GRAPH END******************************************************
    //***************************VERTICAL BORDER DRAWING**********************************************************
    /**
      This function add VerticalBorder nodes to define subgraph vertical boundaries

        @pre
        -# m_iRankDifferenceInLayers.isSet() == true

        @param none

        @return none

        @throws LayoutException
        -# REQUIRED_PARAMETER_NOT_SET if rank difference between layers is not set

        @throws BoostException

        @throws MemoryException

      */
    void addVerticalBorderNodesForSubgraphs();

    /**
      This function add VerticalBorder nodes for given NestingTreeSubgraphNode recursively starting from innermost NestingTreeSubgraphNode

        @pre none

        @param nestingTreeNode
        NestingTreeSubgraphNode to which vertical border nodes are to be added

        @return none

        @throws LayoutException
        -# INVALID_OPERATION if vertical border node is added at horizontal position which is already taken by another node at same layer

        @throws BoostException

        @throws MemoryException

      */
    void addVerticalBorderNodesNestingTreeRecur(NestingTreeSubgraphNode& nestingTreeNode);

    /**
      This function adds dummy nodes in empty layers of each subgraphs. This is neccessary in order to give vertical border vertex at every layer of every subgraph.

        @pre none

        @param none

        @return none

        @throws LayoutException

        @throws BoostException

      */
    void addDummyNodeToEmptyLayersRecur(NestingTreeSubgraphNode& rootNestingTreeNode);

    /**
      This function updates horizontal positions of LayerNodes on each layer, space them to accomodate Vertical Border Nodes

        @pre
        m_iRankDifferenceInLayers.isSet() == true

        @param none

        @return none

        @throws LayoutException
        -# REQUIRED_PARAMETER_NOT_SET if rank difference between layers is not set

        @throws BoostException

        @throws MemoryException

      */
    void setHorizontalPositionsForVerticalBorderNodes();

    //***************************VERTICAL BORDER DRAWING END******************************************************

    //***************************POSITIONING***********************************************************************

    /**
      This function generates center coordinates of all vertices according to their rank and horizontal position values

        @pre
        -# gMainGraph.is_root() == true

        @param none

        @return none

        @throws LayoutException
        -# INVALID_PARAMETER if provided graph is not a root graph
        -# REQUIRED_PARAMETER_NOT_SET if y coordinate or next layer's height is not set

        @throws BoostException

        @throws MemoryException

      */
    void assignYCoordinates(SubGraph& gMainGraph);

    /**
      This function calculates and writes compartment (cluster) border left top coordinates and height - width to Graph properties

        @pre none

        @param none

        @return none

        @throws BoostException

      */
    void setSubgraphCompartmentProperties();

    /**
      This function generates x coordinates of all vertices according to their horizontal position values

        @pre
        -# iHorizontalStep > 0

        @param iHorizontalStep
        space between two nodes 1 distance apart

        @return none

        @throws none

      */
    void applyXCoordinatesFromHorizontalPosition(int iHorizontalStep);



    /**
      This function gives coordinates that the space below leaf vertices can also be
      utilised and graph width compresses to some extent. It generates center
      coordinates of all vertices according to their rank, horizontal position
      and out degree values

        @pre none

        @param none

        @return none

        @throws LayoutException
        -# INCONSISTENT_DATASTRUCTURE if layered graph is inconsistent

        @throws MemoryException

        @throws BoostException
      */
    void setHorizontalPosition2();
    /**
      This function creates average median vertical alignment

      @pre
      -# vecUpLeftAlign.size() == num_vertices(*m_gMainGraph)
      -# vecUpRightAlign.size() == num_vertices(*m_gMainGraph)
      -# vecDownLeftAlign.size() == num_vertices(*m_gMainGraph)
      -# vecDownRightAlign.size() == num_vertices(*m_gMainGraph)

      @param vecUpLeftAlign
      out parameter vector of upward left alignment roots of vertices

      @param vecUpRightAlign
      vector of upward right alignment

      @return none

      @throws LayoutException
      -# INVALID_PARAMETER if any provided alignement vector has size not equal to total number of nodes

      @throws MemoryException

      @throws BoostException
     */
    void alignToSmallestAlignment(QVectorDouble &vecUpLeftAlign, QVectorDouble &vecUpRightAlign, QVectorDouble &vecDownLeftAlign, QVectorDouble &vecDownRightAlign);

    /**
      This function merges the four vertical alignments- UpLeft, UpRight , DownLeft and DownRight

      @pre
      -# vecUpLeftAlign.size() == num_vertices(*m_gMainGraph)
      -# vecUpRightAlign.size() == num_vertices(*m_gMainGraph)
      -# vecDownLeftAlign.size() == num_vertices(*m_gMainGraph)
      -# vecDownRightAlign.size() == num_vertices(*m_gMainGraph)

      @param vecUpLeftAlign
      out parameter vector of upward left alignment roots of vertices

      @param vecUpRightAlign
      vector of upward right alignment

      @param vecDownLeftAlign
      vector of downward left alignment

      @param vecDownRightAlign
      vector of downward right alignment

      @return a vector of final horizontal positioning by merging the four vertical alignments

      @throws LayoutException
      -# INVALID_PARAMETER if any provided alignement vector has size not equal to total number of nodes

      @throws MemoryException

      @throws BoostException
     */
    QVectorDouble mergeAlignments(QVectorDouble &vecUpLeftAlign, QVectorDouble &vecUpRightAlign, QVectorDouble &vecDownLeftAlign, QVectorDouble &vecDownRightAlign);

    /**
      This function gives minimum and maximaum positions in given vector of positions

      @pre
      -# vecPositions.size() == num_vertices(*m_gMainGraph)

      @param vecPositions
      out parameter vector of upward left alignment roots of vertices

      @return none

      @throws none
     */
    void getMinMaxPositions(double &iMinPosition , double &iMaxPosition , QVectorDouble &vecPositions);

    /**
      This function initialises LeftOf Relation vector of vertices which tells the immediete left neighbor vertex of the vertex on same layer

      @pre none

      @param none

      @return none

      @throws LayoutException
      -# INVALID_PARAMETER if vector vecLeftNeighborVertex does not have size equal to total number of vertices

      @throws BoostException

      @throws MemoryException
     */

    void initLeftNeighborVertexVector(QVectorInt &vecLeftNeighborVertex);

    /**
      This function marks edge crossings between LongEdgeSegment and GraphEdge and sets it into edge property

      @pre none

      @param vecIsMarkedEdge
      it is an empty vector of bool values for specifying edge is marked as conflicted true or false

      @return none

      @throws LayoutException

      @throws BoostException

      @throws MemoryException
     */
    void markConflictedEdges();

    /**
      This function creates upward left vertical alignment

      @pre
      -# vecUpLeftAlignRoot.size() == num_vertices(*m_gMainGraph)
      -# vecUpLeftAlign.size() == num_vertices(*m_gMainGraph)

      @param vecLeftAlignRoot
      out parameter vector of upward left alignment roots of vertices

      @param vecLeftAlign
      out parameter vector of upward left alignment

      @return none

      @throws LayoutException
      -# INCONSISTENT_DATASTRUCTURE if a vertical border node has out degree or in degree greater than 1
      -# INVALID_PARAMETER if vector LeftAlignRoot or UpLeftAlign does not have size equal to total number of vertices

      @throws BoostException

      @throws MemoryException
     */
    QVectorInt createUpwardLeftAlignment(QVectorInt &vecUpLeftAlignRoot , QVectorInt &vecUpLeftAlign);

    /**
      This function creates downward left vertical alignment

      @pre
      -# vecDownLeftAlignRoot.size() == num_vertices(*m_gMainGraph)
      -# vecDownLeftAlign.size() == num_vertices(*m_gMainGraph)

      @param vecDownLeftAlignRoot
      out parameter vector of downward left alignment roots of vertices

      @param vecDownLeftAlign
      out parameter vector of downward left alignment

      @return none

      @throws LayoutException
      -# INVALID_OPERATION if the layered graph is not reversed
      -# INCONSISTENT_DATASTRUCTURE if a vertical border node has out degree or in degree greater than 1
      -# INVALID_PARAMETER if vector DownLeftAlignRoot or DownLeftAlign does not have size equal to total number of vertices

      @throws BoostException

      @throws MemoryException
     */
    QVectorInt createDownwardLeftAlignment(QVectorInt &vecDownLeftAlignRoot , QVectorInt &vecDownLeftAlign);

    /**
      This function creates upward right vertical alignment

      @pre
      -# vecUpRightAlignRoot.size() == num_vertices(*m_gMainGraph)
      -# vecUpRightAlign.size() == num_vertices(*m_gMainGraph)

      @param vecUpRightAlignRoot
      out parameter vector of upward right alignment roots of vertices

      @param vecUpRightAlign
      out parameter vector of upward right alignment

      @return none

      @throws none
     */
    QVectorInt createUpwardRightAlignment(QVectorInt &vecUpRightAlignRoot , QVectorInt &vecUpRightAlign);

    /**
      This function creates downward right vertical alignment

      @pre
      -# vecDownRightAlignRoot.size() == num_vertices(*m_gMainGraph)
      -# vecDownRightAlign.size() == num_vertices(*m_gMainGraph)

      @param vecDownRightAlignRoot
      out parameter vector of upward right alignment roots of vertices

      @param vecDownRightAlign
      out parameter vector of upward right alignment

      @return none

      @throws none
     */
    QVectorInt createDownwardRightAlignment(QVectorInt &vecDownRightAlignRoot , QVectorInt &vecDownRightAlign);

    /**
      This function gives QMap of median upper neighbors of provided vertex

      @pre
        -# vVertex >= 0

      @param none

      @return none

      @throws none
     */
    MapPositionToVertexDescriptor getMedianUpperNeighborsExcludeConflictedEdges(VertexDescriptor &vGlobalVertex);

    /**
      This function gives QMap of median lower or down neighbors of provided vertex

      @pre
        -# vVertex >= 0

      @param none

      @return none

      @throws none
     */
    MapPositionToVertexDescriptor getMedianLowerNeighborsExcludeConflictedEdges(VertexDescriptor &vGlobalVertex);


    /**
      This function gives horizontal compaction of given vertical alignment considering vertex sizes

      @pre
        -# vecPositions.size() = num_vertices(mainGraph)
        -# vecAlignVertex.size() = num_vertices(mainGraph)
        -# vecRootVertex.size() = num_vertices(mainGraph)
        -# vecLeftNeighborVertex.size() = num_vertices(mainGraph)

      @param vecPositions
      vector as out parameter which stores the each vertex horizontal position

      @param vecAlignVertex
      vector of vertical alignment

      @param vecRootVertex
      vector of root vertices according to the vertical alignment

      @param vecLeftNeighborVertex
      vector of left neighbor nodes of vertices on ssame layer

      @return none

      @throws LayoutException
      -# INCONSISTENT_DATASTRUCTURE if horizontal compaction shuffles the node positions or creates inconsistent positions with respect to the alignments of nodes
      -# INVALID_PARAMETER if any of vector vecPositions , vecAlignVertex, vecRootVertex, vecLeftNeighborVertex has size not equal to total nodes in the graph

      @throws BoostException

      @throws MemoryException
     */
    void horizontalCompaction2(QVectorDouble &vecPositions , const QVectorInt &vecAlignVertex , const QVectorInt &vecRootVertex , const QVectorInt vecLeftNeighborVertex);


    /**
      This function places the block of vertices according to their vertical alignment using space considerations

      @pre
        -# vVertex >= 0
        -# vecPositions.size() = num_vertices(mainGraph)
        -# vecShift.size() = num_vertices(mainGraph)
        -# vecSink.size() = num_vertices(mainGraph)
        -# vecAlignVertex.size() = num_vertices(mainGraph)
        -# vecRootVertex.size() = num_vertices(mainGraph)
        -# vecLeftNeighborVertex.size() = num_vertices(mainGraph)


      @param none

      @return none

      @throws LayoutException
      -# INVALID_PARAMETER if any provided vector has size not equal to total number of nodes

      @throws MemoryException

      @throws BoostException
     */
    void placeBlock2(int iLevel
                     , VertexDescriptor vVertex
                     , QVectorDouble &vecPositions
                     , QVectorDouble &vecShift
                     , QVectorInt &vecSink
                     , const QVectorInt &vecAlignVertex
                     , const QVectorInt &vecRootVertex
                     , const QVectorInt &vecLeftNeighborVertex
                     , VecMapShiftClass &mapShiftClass);


    /**
      This function updates shift for class of vertices having same sink

      @pre
      -# iToShift >= 0
      -# iNeighbor >= 0

      @param iToShift
      vertex to shift

      @param iNeighbor
      vertex neighbor to iToShift vertex

      @param mapShiftClass
      map to store shifts of classes of vertices

      @return none

      @throws none
     */
    void updateShift(int iToShift , int iNeighbor , double dDelta, VecMapShiftClass &mapShiftClas);

    /**
      This function provides separation value in pixels

      @pre
      -# vVertex >= 0

      @param vVertex
      vertex index

      @return number of pixels separation required

      @throws none
     */
    int separation(VertexDescriptor vVertex);


    /**
      This function adjusts x and y coordinates of vertices to shift to left top point according to height and width of them

      @pre none

      @param none

      @return none

      @throws BoostException
     */
    int shiftVertexCoordinateToLeftTop();

    //*****************************Miscelleneous Function*********************************************************
    /**
      This function prints  indent space according to level
      @pre
        -# iLevel >= 0

      @param none

      @return none

      @throws none
     */
    QString printIndent(int iLevel);


    //************************************************************************************************************
    /*Testing Functions:
     */
    /**
      This function tests if proper ranking is assigned and there are no upward edges exist
        @pre
            -# gGraph != NULL

        @param gGraph
            reference to the graph

        @return none

        @throws none

      */
    bool testUpwardEdgesAndRanks(SubGraph &gGraph);

    /**
      This function tests if proper layered graph is created: 1. checks if all vertices are present in layered graph
        @pre none

        @param gGraph
            reference to the graph

        @return none

        @throws none

      */
    bool testLayeredGraph();


    /**
      This function tests consistancy of LayerNode position in Layer and the actual Vertex property iHorizontalPosition, they should be same

        @pre none

        @param mapPositionToLayerNode
        map of LayerNodes representing the layer

        @param gMainGraph
        reference to main (root) graph

        @return bool value true if all LayerNode position are consistent with corrensponding Vertex's property iHorizopntalPosition otherwise returns false

        @throws none

      */
    int testGetLayerKeysAndVertexPositionNotConsistentCount(MapPositionToLayerNode& mapPositionToLayerNode , SubGraph& gMainGraph);


    /**
      This function tests the horizontal compaction aligned horizontalPositions of vertices changes the original order of nodes on layer or not

        @pre
            -# veAlignedPosition.size() == num_vertices(*m_gMainGraph)

        @param veAlignedPosition
        vector of vertices horizontal position

        @return bool value true horizontal compaction aligned horizontalPositions of vertices changes the original order of nodes on layer otherwise false
        @throws none

      */
    bool testHorizontalCompaction(QVectorInt &vecAlignedPosition);

    /**
      This function tests the horizontal compaction aligned horizontalPositions of vertices changes the original order of nodes on layer or not

        @pre
            -# veAlignedPosition.size() == num_vertices(*m_gMainGraph)

        @param veAlignedPosition
        vector of vertices horizontal position

        @return bool value true horizontal compaction aligned horizontalPositions of vertices changes the original order of nodes on layer otherwise false
        @throws none

      */
    bool testHorizontalCompaction(QVectorDouble &vecAlignedPosition);

    /**
      This function tests the aligned horizontalPositions of vertices with their alignment

        @pre
            -# veAlignedPosition.size() == num_vertices(*m_gMainGraph)
            -# vecAlign.size() == num_vertices(*m_gMainGraph)

        @param veAlignedPosition
        vector of vertices horizontal position

        @return bool value true horizontal compaction aligned horizontalPositions of vertices changes the original order of nodes on layer otherwise false
        @throws none

      */
    bool testPositionWithAlignment(const QVectorInt &vecAlignedPosition , const QVectorInt &vecAlign);

    /**
      This function tests the aligned horizontalPositions of vertices with their alignment

        @pre
            -# veAlignedPosition.size() == num_vertices(*m_gMainGraph)
            -# vecAlign.size() == num_vertices(*m_gMainGraph)

        @param veAlignedPosition
        vector of vertices horizontal position

        @return bool value true horizontal compaction aligned horizontalPositions of vertices changes the original order of nodes on layer otherwise false
        @throws none

      */
    bool testPositionWithAlignment(const QVectorDouble &vecAlignedPosition , const QVectorInt &vecAlign);

    /**
      This function writes current hierarchical layout execution time details and number off nodes, edes, ling edges to CSV file: FILENAME_EXECUTION_TIME_DETAILS from HIERARCHICALLAYOUTTESTINGCONSTANTS_H

        @pre none

        @param sExecutionDetails
        string containing details in comma separated form

        @return none

        @throws none

      */
    void writeLog(QString sExecutionDetails);

    /**
      This function tests the number of nodes and subgraph count in the generated nesting tree

        @pre none

        @param none

        @return bool value true if the nesting tree is correct otherwise false

        @throws none

      */
    bool testNestingTree();

    /**
      This function tests the number of nodes and subgraph count in the generated Reduced Nesting tree

        @pre
        -# reducedNestingTreeRoot.isLayerNode() == false

        @param reducedNestingTreeRoot
        root reduced nesting tree node

        @return bool value true if the Reduced Nesting tree is correct otherwise false

        @throws none

      */
    bool testReducedNestingTree(ReducedNestingTreeNode &reducedNestingTreeRoot);

    /**
      This function tests SubgraphOrderingGraphs are consistent with NestingTreeSubgraphNode structure

        @pre none

        @param none

        @return bool value true if SubgraphOrderingGraphs are consistent with NestingTreeSubgraphNode structure otherwise false

        @throws none

      */
    bool testSubgraphOrderingGraph();

    /**
      This function tests if layerNodes are sorted according their iTopologicalOrder value

        @pre
        -# m_mapLayeredGraph.contains(iLayerId)==true

        @param none

        @return bool value true if layerNodes are sorted according their iTopologicalOrder value otherwise false

        @throws none

      */
    bool testIsLayerTopologicallySorted(int iLayerId);


    //************************************************************************************************************

};

#endif // HIERARCHICALLAYOUTER_H
