#include "NestingTreeSubgraphNode.h"

NestingTreeSubgraphNode::NestingTreeSubgraphNode()
{
    m_bIsRoot = false;

    m_parentNestingTreeSubraphNode = NULL;

    m_gSubgraph = NULL;

    m_iMinRank = 0;

    m_iMaxRank = 0;

}

NestingTreeSubgraphNode::NestingTreeSubgraphNode(SubGraph &gGraph)
{
    m_gSubgraph = &gGraph;

    m_bIsRoot = gGraph.is_root();

    VertexDescriptor vUpperBorder = m_BoostGraphWrapper.getGraphUpperBorderVertex(gGraph);
    VertexDescriptor vLowerBorder = m_BoostGraphWrapper.getGraphLowerBorderVertex(gGraph);

    m_iMinRank = m_BoostGraphWrapper.getVertexRank(vUpperBorder , gGraph);
    m_iMaxRank = m_BoostGraphWrapper.getVertexRank(vLowerBorder , gGraph);
}

void NestingTreeSubgraphNode::setGraph(SubGraph &gGraph)
{
    m_gSubgraph = &gGraph;

    m_bIsRoot = gGraph.is_root();

    VertexDescriptor vUpperBorder = m_BoostGraphWrapper.getGraphUpperBorderVertex(gGraph);
    VertexDescriptor vLowerBorder = m_BoostGraphWrapper.getGraphLowerBorderVertex(gGraph);

    m_iMinRank = m_BoostGraphWrapper.getVertexRank(vUpperBorder , gGraph);
    m_iMaxRank = m_BoostGraphWrapper.getVertexRank(vLowerBorder , gGraph);
}

SubGraph &NestingTreeSubgraphNode::getGraph()
{
    return *m_gSubgraph;
}

NestingTreeSubgraphNode &NestingTreeSubgraphNode::getParent()
{
    return *m_parentNestingTreeSubraphNode;
}

void NestingTreeSubgraphNode::setParentNestingTreeSubgraphNode(NestingTreeSubgraphNode &parentNestingTreeSubraphNode)
{
    m_parentNestingTreeSubraphNode = &parentNestingTreeSubraphNode;
}

bool NestingTreeSubgraphNode::isRoot()
{
    //Q_ASSERT_X(m_gSubgraph != NULL , "Nesting Tree Subgraph Node" , "Checking graph isRoot property without assigning a graph");
    LAYOUT_ASSERT(m_gSubgraph != NULL , LayoutException(__FUNCTION__
                                                        ,LayoutExceptionEnum::INVALID_OPERATION
                                                        ,"Checking graph isRoot property without assigning a graph"
                                                        , "Nesting Tree Subgraph Node"));
    return m_bIsRoot;
}

NestingTreeSubgraphNode& NestingTreeSubgraphNode::addChildNestingTreeSubgraphNode(SubGraph &gChildGraph)
{
    NestingTreeSubgraphNode *newChildNestingTreeSubgraphNode;

    newChildNestingTreeSubgraphNode = new NestingTreeSubgraphNode(gChildGraph);

    //Add child entry to vec
    m_vecChildNestingTreeSubgraphNode.push_back(newChildNestingTreeSubgraphNode);

    //Set child subGraph Nodes Parent
    newChildNestingTreeSubgraphNode->setParentNestingTreeSubgraphNode(*this);

    return *newChildNestingTreeSubgraphNode;
}

const NestingTreeSubgraphNode::VectorNestingTreeSubgraphNodesRef &NestingTreeSubgraphNode::getChildNestingTreeSubgraphNodes()
{
    return m_vecChildNestingTreeSubgraphNode;
}

NestingTreeSubgraphNode::IteratorVectorNestingTreeSubgraphNodesRef NestingTreeSubgraphNode::getIteratorChildNestingTreeSubgraphNodes()
{
    IteratorVectorNestingTreeSubgraphNodesRef iterVecNestingTreeSubgraphNodesRef(m_vecChildNestingTreeSubgraphNode);

    return iterVecNestingTreeSubgraphNodesRef;
}

int NestingTreeSubgraphNode::getCountOfChildNestingTreeSubgraphNodes()
{
    return m_vecChildNestingTreeSubgraphNode.size();
}

int NestingTreeSubgraphNode::getCountOfLayerNodes()
{
    return m_mapLayerIdToLayerNodeRef.size();
}

NestingTreeSubgraphNode::MultiMapLayerIdToLayerNodeRef& NestingTreeSubgraphNode::getMapLayerIdToLayerNodeRef()
{
    return m_mapLayerIdToLayerNodeRef;
}

NestingTreeSubgraphNode::IteratorMultiMapLayerIdToLayerNodeRef NestingTreeSubgraphNode::getChildLayerNodesIterator()
{
    IteratorMultiMapLayerIdToLayerNodeRef iterLayerNodes(m_mapLayerIdToLayerNodeRef);

    return iterLayerNodes;
}

void NestingTreeSubgraphNode::addLayerIdAndLayerNode(int iLayerRank, LayerNode* layerNodeRef)
{
    //Add layer node to LayerIdToLayerNodeRef multimap
    m_mapLayerIdToLayerNodeRef.insertMulti(iLayerRank , layerNodeRef);

    //Record Layer Id //Edit 4-9-14
    recordLayerIdRecurUp(iLayerRank);

}


int NestingTreeSubgraphNode::getMinRank()
{
    LAYOUT_ASSERT(m_gSubgraph != NULL , LayoutException(__FUNCTION__
                                                        ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                        ,"Subgraph and thus MinRank"
                                                        , "Nesting Tree Subgraph Node"));
    return m_iMinRank;
}

int NestingTreeSubgraphNode::getMaxRank()
{
    LAYOUT_ASSERT(m_gSubgraph != NULL , LayoutException(__FUNCTION__
                                                        ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                        ,"Subgraph and thus MaxRank"
                                                        , "Nesting Tree Subgraph Node"));
    return m_iMaxRank;
}

bool NestingTreeSubgraphNode::isBetweenMinMaxRanks(int iRank)
{
    //Q_ASSERT_X(iRank > 0 , "Checking isBetweenMinMaxRanks" , "Invalid rank value. Rank should be greater than 0");
    LAYOUT_ASSERT(iRank > 0 , LayoutException(__FUNCTION__ , LayoutExceptionEnum::INVALID_PARAMETER
                                              , "Rank should be greater than 0"
                                              , "iRank"));

    //Q_ASSERT_X(m_gSubgraph != NULL , "Checking isBetweenMinMaxRanks" , "Boost Subgraph is not assigned to current Nesting Tree Subgaph Node");
    LAYOUT_ASSERT(m_gSubgraph != NULL , LayoutException(__FUNCTION__
                                                        , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                        , "Subgraph for current Nesting Tree Node", "" ));

    bool bIsBetweenMinMaxRanks = false;

    if(m_iMinRank <= iRank && iRank <= m_iMaxRank)
    {
        bIsBetweenMinMaxRanks = true;
    }

    return bIsBetweenMinMaxRanks;
}

void NestingTreeSubgraphNode::recordLayerIdRecurUp(int iLayerId)
{
    if(doesSubgraphNodeContainLayerId(iLayerId) == false)
    {
        m_setLayerIds.insert(iLayerId);

        //Update parent hierararchy for including new LayerId

        if(m_parentNestingTreeSubraphNode != NULL)
        {
            (*m_parentNestingTreeSubraphNode).recordLayerIdRecurUp(iLayerId);
        }
    }


}


bool NestingTreeSubgraphNode::doesSubgraphNodeContainLayerId(int iLayerId)
{
    return m_setLayerIds.contains(iLayerId);
}

double NestingTreeSubgraphNode::updateAverageBarryCenter()
{

    try
    {
        LAYOUT_ASSERT(m_gSubgraph != NULL , LayoutMemoryException(__FUNCTION__
                                                            , LayoutExceptionEnum::NULL_POINTER_EXCEPTION
                                                            , "Subgraph for current nesting tree node"));

        int iTotalVertices = num_vertices(*m_gSubgraph);
        double dTotalBarryCenterWeight = 0.0;

        BGL_FORALL_VERTICES(vVertex, *m_gSubgraph , SubGraph)
        {
            dTotalBarryCenterWeight += m_BoostGraphWrapper.getVertexHorizontalPosition(vVertex , *m_gSubgraph);
        }

        m_dAverageBarryCenter = dTotalBarryCenterWeight / (double)iTotalVertices;

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }

    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
    return m_dAverageBarryCenter;
}

double NestingTreeSubgraphNode::getAverageBarryCenter()
{
    return m_dAverageBarryCenter;
}

int NestingTreeSubgraphNode::setSubgraphOrderingGraphVertexIndex(int iSubgraphOrderingGraphVertexIndex)
{
    m_iSubgraphOrderingGraphVertexIndex = iSubgraphOrderingGraphVertexIndex;
}

int NestingTreeSubgraphNode::getSubgraphOrderingGraphVertexIndex()
{
    return m_iSubgraphOrderingGraphVertexIndex;
}

int NestingTreeSubgraphNode::setTopologicalOrder(int iTopologicalOrder)
{
    m_iTopologocalOrder = iTopologicalOrder;
}

int NestingTreeSubgraphNode::getTopologicalOrder()
{
    return m_iTopologocalOrder;
}

void NestingTreeSubgraphNode::printName()
{
    QString sName = m_BoostGraphWrapper.getGraphId(*m_gSubgraph);
    qDebug() << sName;
}
