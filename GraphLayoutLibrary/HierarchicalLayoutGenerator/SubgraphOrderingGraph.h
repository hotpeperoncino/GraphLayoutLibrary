#ifndef SUBGRAPHORDERINGGRAPH_H
#define SUBGRAPHORDERINGGRAPH_H

#include <GraphLayoutLibrary_global.h>
#include <HierarchicalLayoutGenerator/SubgraphOrderingGraphType.h>

/**
 * @brief The SubgraphOrderingGraphWrapper class
 *
 * The class provides helpers to manage subgraph ordering in hierarchical layout.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT SubgraphOrderingGraphWrapper
{
public:

	SubgraphOrderingGraphType *m_gSubgraphOrderingGraph; /*!< Pointer reference to ordered subgraph  */

/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type SubgraphOrderingGraphWrapper.
 */

//@{
	/**
     Constructs new object of type SubgraphOrderingGraphWrapper.
    @pre none

    @param none

    @return none

    @throw none
    */
    SubgraphOrderingGraphWrapper();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of SubgraphOrderingGraphWrapper.
  */
 //@{
    /**
      Sets a vertex of graph as nesting tree subgraph node and sets layer node as false
      @pre none

      @param NestingTreeSubgraphNode
      reference to nesting tree subgraph node

      @param SubgraphOrderingGraphVertexDescriptor
      vertex descriptor to be set

      @param SubgraphOrderingGraphType
      reference to ordered subgraph

      @return none

      @throws LayoutException
    */
    void setVertexNestingTreeSubgraphNodeRef(NestingTreeSubgraphNode* nestingTreeNodeRef , SubgraphOrderingGraphVertexDescriptor vVertex , SubgraphOrderingGraphType &gSubgraphOrderingGraph);
	
	/**
      Assigns the layer reference to a vertex descriptor in the subgraph
      @pre none

      @param LayerNode*
      pointer to layer node reference

      @param SubgraphOrderingGraphVertexDescriptor
      vertex descriptor to be set

      @param SubgraphOrderingGraphType
      reference to ordered subgraph

      @return none

      @throws LayoutException
    */
    void setVertexLayerNodeRef(LayerNode* layerNodeRef , SubgraphOrderingGraphVertexDescriptor vVertex , SubgraphOrderingGraphType &gSubgraphOrderingGraph);
	
//    void setVertexAverageBarryCenter(double dAverageBarryCenter , SubgraphOrderingGraphVertexDescriptor vVertex , SubgraphOrderingGraphType &gSubgraphOrderingGraph);
	
	/**
      This function set the property that 'IsEdge' is in spanning tree. This helps to detect edges in spanning tree

      @pre
      -# gSubgraphOrderingGraph.find_edge(eEdge) == true

      @param bIsSpannigTreeEdge
      boolean value to set to edge's bIsSpanningTreeEdge property

      @param eEdge
      subgraph ordering graph edge

      @param gSubgraphOrderingGraph
      subgraph ordering graph

      @return none

      @throws none
    */
    void setIsEdgeInSpanningTree(bool bIsSpannigTreeEdge , SubgraphOrderingGraphEdgeDescriptor eEdge , SubgraphOrderingGraphType& gSubgraphOrderingGraph);
//@}
	
/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type SubgraphOrderingGraphWrapper.
 */
//@{
    /**
      Returns nesting tree subgraph node of a vertex descriptor in SubgraphOrderingGraphType
      @pre none

      @param SubgraphOrderingGraphVertexDescriptor
      vertex descriptor to be set

      @param SubgraphOrderingGraphType
      reference to ordered subgraph

      @return none

      @throws LayoutException
    */
    NestingTreeSubgraphNode* getVertexNestingTreeSubgraphNode(SubgraphOrderingGraphVertexDescriptor vVertex , SubgraphOrderingGraphType &gSubgraphOrderingGraph);
	
	/**
      Returns the layer reference corresponding to a vertex descriptor in the subgraph
      @pre none

      @param SubgraphOrderingGraphVertexDescriptor
      vertex descriptor to be set

      @param SubgraphOrderingGraphType
      reference to ordered subgraph

      @return LayerNode*

      @throws LayoutException
    */
    LayerNode* getVertexLayerNodeRef(SubgraphOrderingGraphVertexDescriptor vVertex , SubgraphOrderingGraphType &gSubgraphOrderingGraph);
	
//    double getVertexAverageBarryCenter(SubgraphOrderingGraphVertexDescriptor vVertex , SubgraphOrderingGraphType &gSubgraphOrderingGraph );
	
	/**
      Returns whether a vertex is layer node or not

      @pre none

      @param SubgraphOrderingGraphVertexDescriptor
      Vertex Descriptors

      @param SubgraphOrderingGraphType
      reference to ordered subgraph

      @return bool

      @throws none
    */
    bool isSubgraphOrderingVertexIsLayerNode(SubgraphOrderingGraphVertexDescriptor vVertex , SubgraphOrderingGraphType &gSubgraphOrderingGraph);

    /**
      This function gets if the edge is spanning tree edge property is true or false

      @pre
      -# gSubgraphOrderingGraph.find_edge(eEdge) == true

      @param eEdge
      subgraph ordering graph edge

      @param gSubgraphOrderingGraph
      subgraph ordering graph

      @return boolean true - if the edge is spanning tree edge property is true otherwise false

      @throws none
    */
    bool getIsEdgeInSpanningTree(SubgraphOrderingGraphEdgeDescriptor eEdge , SubgraphOrderingGraphType& gSubgraphOrderingGraph);
	
	/**
      Prints a textual representation of ordered subgraph. Used for debugging purposes only.

      @pre none

      @param SubgraphOrderingGraphType
      reference to ordered subgraph

      @return none

      @throws none
    */
    void printGraph(SubgraphOrderingGraphType& gGraph);
//@}

  
};

#endif // SUBGRAPHORDERINGGRAPH_H
