#ifndef LAYERNODE_H
#define LAYERNODE_H

#include <Common/GraphType.h>
#include <HierarchicalLayoutGenerator/NestingTreeSubgraphNode.h>
#include <QMultiMap>

class NestingTreeSubgraphNode;

/**
 * @brief The LayerNode class
 *
 * The class provides helpers to manage layer node in hierarchical layout.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayerNode
{
private:

    /*! Every layer node should have link to
     *   its parent node; it is useful Splitting Long Edges */
    NestingTreeSubgraphNode *m_parentNestingTreeSubgraphNode;

    /*! All properties are to be accessed using this Global Vertex Descriptor
     *  and main graph */
    VertexDescriptor m_vGlobalVertex;

    /*! iTopological Order according to subgraph ordering graph */
    int m_iTopologocalOrder;

    /*! Vertex descriptor from Subgraph Ordering Graph */
    int m_iSubgraphOrderingGraphVertexIndex;

public:

/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type LayerNode.
 */

//@{
	/**
     Constructs new object of type LayerNode.
    @pre none

    @param none

    @return none

    @throw none
    */
    LayerNode();

	/**
     Constructs new object of type LayerNode.
    @pre none

    @param vGlobalVertex
		instance of VertexDescriptor

    @return none

    @throw none
    */
    LayerNode(VertexDescriptor vGlobalVertex);

    /**
     Constructs new object of type LayerNode.
    @pre none

    @param parentNestingTreeSubgraphNode
		reference of NestingTreeSubgraphNode

    @return none

    @throw none
    */
	LayerNode(NestingTreeSubgraphNode& parentNestingTreeSubgraphNode);

    /**
     Constructs new object of type LayerNode.
    @pre none

    @param parentNestingTreeSubgraphNode
		reference of NestingTreeSubgraphNode
	
	@param vGlobalVertex
		instance of VertexDescriptor

    @return none

    @throw none
    */
	LayerNode(NestingTreeSubgraphNode& parentNestingTreeSubgraphNode ,
              VertexDescriptor vGlobalVertex);
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of LayerNode.
  */
 //@{
	/**
      This function sets parent Nesting Tree Subgraph Node of current LayerNode

      @pre none

      @param parentNestingTreeSubgraphNode
        parent Nesting Tree Subgraph Node of current LayerNode

      @return none

      @throws none
    */
    void setParentNestingTreeSubgraphNode(NestingTreeSubgraphNode& parentNestingTreeSubgraphNode);
	
	/**
      This function sets global vertex which current LayerNode should represent

      @pre none

      @param vGlobalVertex
        global vertex index to which current LayerNode should represent

      @return none

      @throws none
    */
    void setVertex(VertexDescriptor vGlobalVertex);
	
	/**
      This function sets parent Nesting Tree Subgraph Node and global vertex which current LayerNode should represent

      @pre none

      @param vGlobalVertex
        global vertex index to which current LayerNode should represent

      @return none

      @throws none
    */
    void setValues(NestingTreeSubgraphNode& parentNestingTreeSubgraphNode ,
                   VertexDescriptor vGlobalVertex);

    /**
      This function sets Topological Order of current Layer Node

      @pre none

      @param iTopologicalOrder
      integer value for Layer Node Topological Order

      @return none

      @throws none
    */
    int setTopologicalOrder(int iTopologicalOrder);
	
	/**
      This function sets the Vertex Index from its SubgraphOrderingGraph

      @pre none

      @param iSubgraphOrderingGraphVertexIndex
        vertex index of LayerNode from its SubgraphOrderingGraph

      @return none

      @throws none
    */
    void setSubgraphOrderingGraphVertex(int iSubgraphOrderingGraphVertexIndex);
 //@}
 
 /** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type LayerNode.
 */
//@{
	/**
      This function gets parent Nesting Tree Subgraph Node of current LayerNode

      @pre none

      @param none

      @return none

      @throws none
    */
    NestingTreeSubgraphNode &getParentNestingTreeSubgraphNode();
	
	/**
      This function gets global vertex which current LayerNode represents

      @pre none

      @param none

      @return none

      @throws none
    */
    VertexDescriptor getVertex();
	
	/**
      This function gets Topological Order of current Layer Node

      @pre none

      @param none

      @return integer Topological Order of current Layer Node

      @throws none
    */
    int getTopologicalOrder();
	
	/**
      This function gets the Vertex Index of this LayerNode in its SubgraphOrderingGraph

      @pre none

      @param vGlobalVertex
        global vertex index to which current LayerNode should represent

      @return none

      @throws none
    */
    int getSubgraphOrderingGraphVertex();

    /**
      This function prints the vertex name

      @pre none

      @param none

      @return none

      @throws none
    */
    void printName();
 //@}
    
};

#endif // LAYERNODE_H
