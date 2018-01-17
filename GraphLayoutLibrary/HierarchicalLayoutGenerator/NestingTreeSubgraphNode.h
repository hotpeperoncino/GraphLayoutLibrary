#ifndef NESTINGTREESUBGRAPHNODE_H
#define NESTINGTREESUBGRAPHNODE_H

#include <Common/BoostGraphWrapper.h>
#include <HierarchicalLayoutGenerator/LayerNode.h>
#include <Common/ConstantType.h>
#include <QVector>
#include <QMultiMap>
#include <QSet>

class LayerNode;

class GRAPHLAYOUTLIBRARYSHARED_EXPORT NestingTreeSubgraphNode
{
public:
    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type NestingTreeSubgraphNode.
     */

    //@{
    NestingTreeSubgraphNode();

    NestingTreeSubgraphNode(SubGraph &gGraph);
    //@}


    typedef QVector< NestingTreeSubgraphNode* > VectorNestingTreeSubgraphNodesRef; /*!< Vector of pointer references to nesting trees in subgraphs as node  */

    typedef QVectorIterator< NestingTreeSubgraphNode* > IteratorVectorNestingTreeSubgraphNodesRef; /*!< Iterator of type NestingTreeSubgraphNode*  */

    typedef QMultiMap< int , LayerNode* > MultiMapLayerIdToLayerNodeRef; /*!< Multimap of Layer Ids to layer node references  */

    typedef QMapIterator<int , LayerNode*> IteratorMultiMapLayerIdToLayerNodeRef;  /*!< Iterator to  Multimap of Layer Ids to layer node references (MultiMapLayerIdToLayerNodeRef)  */


private:
    BoostGraphWrapper m_BoostGraphWrapper;

    /* Boolean which tells if this nesting subgraph node represents
     * the main (root) boost graph or not
     */
    bool m_bIsRoot;

    /**< Vector of nesting tree of subgraphs as node */
    VectorNestingTreeSubgraphNodesRef  m_vecChildNestingTreeSubgraphNode;

    SubGraph *m_gSubgraph; /*!< Pointer to the boost Subgraph  */

    NestingTreeSubgraphNode *m_parentNestingTreeSubraphNode; /*!< Pointer to Parent Nesting Tree Subgraph Node  */

    /* Multimap of vertices sortes on their 'layer id/rank'
     * key: layer rank or id of the layer node which belongs current Subgraph
     * value: corresponding node
     */
    MultiMapLayerIdToLayerNodeRef m_mapLayerIdToLayerNodeRef;

    int m_iMinRank;  /*!< Rank of UpperBorder Vertex */

    int m_iMaxRank; /*!< Rank of LowerBorder Vertex */

    /*! QSet (internally a hash)
     *  Contains the layer ids of vertices
     *  which belongs to this Nesting Tree Subgraph Node
     */
    QSet< int > m_setLayerIds;

    double m_dAverageBarryCenter; /*!< Average Barry Center */

    int m_iSubgraphOrderingGraphVertexIndex; /*!< Unique nesting tree identifier */

    int m_iTopologocalOrder; /*!< iTopological Order according to subgraph ordering graph */

public:

    /**
      This function sets the boost graph for current Nesting Tree Subgraph Node

      @pre none

      @param none

      @return none

      @throws none
    */
    void setGraph(SubGraph &gGraph);

    /**
      This function gets the boost graph for current Nesting Tree Subgraph Node

      @pre none

      @param none

      @return none

      @throws none
    */
    SubGraph& getGraph();

    /**
      This function returns parent node of current Nesting Tree Subgraph Node

      @pre none

      @param none

      @return reference to parent node of current Nesting Tree Subgraph Node

      @throws none
    */
    NestingTreeSubgraphNode& getParent();

    /**
      This function sets parent node of current Nesting Tree Subgraph Node

      @pre
      -# &parentNestingTreeSubraphNode != NULL

      @param none

      @return none

      @throws none
    */
    void setParentNestingTreeSubgraphNode(NestingTreeSubgraphNode &parentNestingTreeSubraphNode);

    /**
      This function returns that if current graph is Root (main) graph or not

      @pre none

      @param none

      @return bool
        graph is root (main) graph or not

      @throws LayoutException
      -# INVA
    */
    bool isRoot();

    /**
      This function adds new child Nesting Tree Subgraph Node

      @pre
        -# m_gSubgraph != NULL

      @param gGraph
        reference to the graph for which the Nesting Tree Subgraph Node is to be created

      @return none

      @throws none
    */
    NestingTreeSubgraphNode& addChildNestingTreeSubgraphNode(SubGraph &gChildGraph);

    /**
      This function returns const reference to the next level child nesting tree subgraph nodes

      @pre none

      @param none

      @return the vector containing next level child nesting tree subgraph nodes of current node

      @throws none
    */
    const VectorNestingTreeSubgraphNodesRef& getChildNestingTreeSubgraphNodes();

    /**
      This function returns iterator for m_vecChildNestingTreeSubgraphNode

      @pre none

      @param none

      @return iterator for vecChildNestingTreeSubgraphNode

      @throws none
    */
    IteratorVectorNestingTreeSubgraphNodesRef getIteratorChildNestingTreeSubgraphNodes();

    /**
      This function returns total number of child NestingTree Subgraph nodes

      @pre none

      @param none

      @return total number of child NestingTree Subgraph nodes

      @throws none
    */
    int getCountOfChildNestingTreeSubgraphNodes();

    /**
      This function returns total count of LayerNode nodes

      @pre none

      @param none

      @return total  count of LayerNode nodes

      @throws none
    */
    int getCountOfLayerNodes();



    /**
      This function returns reference to Multimap containing vertices which belongs to current SubgaphNode mapped by their LayerIds

      @pre none

      @param none

      @return reference multimap containing vertices which belongs to current SubgaphNode mapped by their LayerIds

      @throws none
    */
    MultiMapLayerIdToLayerNodeRef& getMapLayerIdToLayerNodeRef();

    /**
      This function returns java style iterator for multimap of LayerNodes

      @pre none

      @param none

      @return java style iterator for multimap of LayerNodes

      @throws none
    */
    IteratorMultiMapLayerIdToLayerNodeRef getChildLayerNodesIterator();


    /**
      This function adds vertex to current NestingTreeSubgraphNode

      @pre none

      @param none

      @return none

      @throws none
    */
    void addLayerIdAndLayerNode(int iLayerRank , LayerNode* layerNodeRef);


    /**
      This function gives min rank of current Subgaph Node i.e. rank of UpperBorderVertex

      @pre none

      @param none

      @return integer min rank of current Subgaph Node

      @throws none
    */
    int getMinRank();

    /**
      This function gives max rank of current Subgaph Node i.e. rank of LowerBorderVertex

      @pre none

      @param none

      @return integer max rank of current Subgaph Node

      @throws none
    */
    int getMaxRank();


    /**
      This function tells if the provided rank value is in between the min and max rank values of the current subgraph node

      @pre
      -# iRank > 0

      @param iRank
        rank value

      @return the bool value for if the provided rank value lies between min and max ranks of current subgraph node

      @throws none
    */
    bool isBetweenMinMaxRanks(int iRank);

    /**
      This function adds layer id into a set contaning unique layer ids of all nodes belongs to current subgraph

      @pre none

      @param iLayerId
        layer id or rank of node

      @return none

      @throws none
    */
    void recordLayerIdRecurUp(int iLayerId);

    /**
      This function checks if provided LayerId is already present in set contaning unique layer ids of all nodes belongs to current subgraph node

      @pre none

      @param iLayerId
        layer id or rank of node

      @return boolean for if Subgraph node contains the Layer node with provided LayerId

      @throws none
    */
    bool doesSubgraphNodeContainLayerId(int iLayerId);

    /**
      This function calculates average barrycenter position of all nodes contained within subgraph

      @pre none

      @param none

      @return none

      @throws none
    */
    double updateAverageBarryCenter();

    /**
      This function gives average barry center position of subgraph

      @pre none

      @param none

      @return average barry center position

      @throws none
    */
    double getAverageBarryCenter();

    /**
      This function sets unique id of current NestingTreeSubgraphNode

      @pre none

      @param iUniqueNestingTreeNodeId
      integer value for NestingTreeSubgraphNode unique id

      @return none

      @throws none
    */
    int setSubgraphOrderingGraphVertexIndex(int iSubgraphOrderingGraphVertexIndex);

    /**
      This function gets unique id of current NestingTreeSubgraphNode

      @pre none

      @param none

      @return integer unique id of current NestingTreeSubgraphNode

      @throws none
    */
    int getSubgraphOrderingGraphVertexIndex();


    /**
      This function sets Topological Order of current NestingTreeSubgraphNode

      @pre none

      @param iTopologicalOrder
      integer value for NestingTreeSubgraphNode Topological Order

      @return none

      @throws none
    */
    int setTopologicalOrder(int iTopologicalOrder);

    /**
      This function gets Topological Order of current NestingTreeSubgraphNode

      @pre none

      @param none

      @return integer Topological Order of current NestingTreeSubgraphNode

      @throws none
    */
    int getTopologicalOrder();

    /**
      This function prints the subgraph name

      @pre none

      @param none

      @return none

      @throws none
    */
    void printName();


};

#endif // NESTINGTREESUBGRAPHNODE_H
