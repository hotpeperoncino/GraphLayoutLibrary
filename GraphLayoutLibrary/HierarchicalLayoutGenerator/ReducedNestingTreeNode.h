#ifndef REDUCEDNESTINGTREENODE_H
#define REDUCEDNESTINGTREENODE_H

#include <HierarchicalLayoutGenerator/LayerNode.h>

#include <QMultiMap>

#include <HierarchicalLayoutGenerator/NestingTreeSubgraphNode.h>

#include <Common/ConstantType.h>

/**
 * @brief The ReducedNestingTreeNode class
 *
 * 
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT ReducedNestingTreeNode
{
private:
    //Boolean for is Layer Node
    ConstantType<bool> m_constBIsLayerNode;

    /*!< Layer node counter */
    ConstantType<int> m_iLayerNodeCount;

public:

    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type ReducedNestingTreeNode.
     */

    //@{
    ReducedNestingTreeNode();
    //@}

    typedef QMultiMap<double , ReducedNestingTreeNode*> MapAvgPosToChildRNTNode; /*!< Multimap average position to reduced nesting trees */

    typedef QMapIterator<double , ReducedNestingTreeNode*> IteratorMapAvgPosToChildRNTNode; /*!< Iterator to Multimap of average position to reduced nesting trees */

    MapAvgPosToChildRNTNode m_mapChildrenRNTNodes;  /*!< Child Reduced Nesting Tree Nodes  */

    double m_dAveragePosition;  /*!< Average Position  */

    NestingTreeSubgraphNode *m_nestingTreeNode;     /*!< Pointer Reference to nesting tree node object  */

    LayerNode *m_layerNode;     /*!< Pointer Reference to layer node object  */

    /**
      This function gives the type of reduced nesting node is LayerNode - true or false

      @pre none

      @param none

      @return the type of reduced nesting node is LayerNode - true or false

      @throws none
    */
    bool isLayerNode();

    /**
      This function adds new child ReducedNestingTreeNode and its average position

      @pre
      -# reducedNestingTreeNodeRef != NULL

      @param dAvgPosition
      average position of child ReducedNestingTreeNode

      @param reducedNestingTreeNodeRef
      pointer to child reduced nested tree node

      @return none

      @throws none
    */
    void addChildNode(double dAvgPosition , ReducedNestingTreeNode *reducedNestingTreeNodeRef);

    /**
      This function sets Layer Node to the current ReducedNestingTreeNode, sets the type of current node as LayerNode

      @pre
      -#    layerNode != NULL;

      @param layerNode
      pointer to layer node

      @return none

      @throws none
    */
    void setLayerNode(LayerNode *layerNode);

    /**
      This function gets Layer Node value of the current ReducedNestingTreeNode

      @pre none

      @param none

      @return pointer to the LayerNode

      @throws none
    */
    LayerNode* getLayerNode();

    /**
      This function sets Nesting Tree Node to the current ReducedNestingTreeNode, sets the type of current node as NestingTreeNode

      @pre
      -#    nestingTreeNode != NULL;

      @param nestingTreeNode
      pointer to nestingTreeNode

      @return none

      @throws none
    */
    void setNestingTreeNode(NestingTreeSubgraphNode *nestingTreeNode);

    /**
      This function gets NestingTreeNode value of the current ReducedNestingTreeNode

      @pre none

      @param none

      @return pointer to the NestingTreeNode

      @throws none
    */
    NestingTreeSubgraphNode* getNestingTreeNode();

    /**
      This function gets average position of current NestingTreeSubgraphNode

      @pre none

      @param none

      @return value of current NestingTreeSubgraphNode average position

      @throws none
    */
    double getAveragePosition();

    /**
      This function sets average position of current NestingTreeSubgraphNode

      @pre none

      @param dAveragePosition
      average position of current NestingTreeSubgraphNode

      @return none

      @throws none
    */
    void setAveragePosition(double dAveragePosition);

    /**
      This function gets child node count of current NestingTreeSubgraphNode

      @pre none

      @param none

      @return value of current NestingTreeSubgraphNode child node count

      @throws none
    */
    int getChildLayerNodeCount();

    /**
      This function sets child layer node count of current NestingTreeSubgraphNode

      @pre
      -#    iLayerNodeCount >= 0

      @param none

      @return value of current NestingTreeSubgraphNode child layer node count

      @throws none
    */
    void setChildLayerNodeCount(int iLayerNodeCount);


    /**
      This function gives iterator for child nodes

      @pre none

      @param none

      @return java style iterator for child nodes

      @throws none
    */
    IteratorMapAvgPosToChildRNTNode getChildNodesIterator();

};

#endif // REDUCEDNESTINGTREENODE_H
