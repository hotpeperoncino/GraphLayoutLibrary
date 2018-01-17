#include "ReducedNestingTreeNode.h"

ReducedNestingTreeNode::ReducedNestingTreeNode()
{
        m_layerNode = NULL;
        m_nestingTreeNode = NULL;

        m_dAveragePosition = 0.0;

}

bool ReducedNestingTreeNode::isLayerNode()
{
    return m_constBIsLayerNode;
}

void ReducedNestingTreeNode::addChildNode(double dAvgPosition, ReducedNestingTreeNode *reducedNestingTreeNodeRef)
{
    m_mapChildrenRNTNodes.insert(dAvgPosition , reducedNestingTreeNodeRef);
}

void ReducedNestingTreeNode::setLayerNode(LayerNode *layerNode)
{
    LAYOUT_ASSERT(m_nestingTreeNode == NULL && layerNode != NULL,
             LayoutException(__FUNCTION__
                             ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                             ,"m_nestingTreeNode and layerNode"
                             ,"ReducedNestingTreeNode"));

    m_layerNode = layerNode;

    m_constBIsLayerNode = true;
}

LayerNode *ReducedNestingTreeNode::getLayerNode()
{
    Q_ASSERT_X(m_layerNode != NULL , "ReducedNestingTree Get LayerNode" , "Layer Node is not set");

    return m_layerNode;
}

void ReducedNestingTreeNode::setNestingTreeNode(NestingTreeSubgraphNode *nestingTreeNode)
{
    LAYOUT_ASSERT(m_layerNode == NULL && nestingTreeNode != NULL,
             LayoutException(__FUNCTION__
                             ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                             ,"m_nestingTreeNode and layerNode"
                             ,"ReducedNestingTreeNode"));


    m_nestingTreeNode = nestingTreeNode;

    m_constBIsLayerNode = false;
}

NestingTreeSubgraphNode *ReducedNestingTreeNode::getNestingTreeNode()
{
    LAYOUT_ASSERT(m_nestingTreeNode != NULL ,
                    LayoutException(__FUNCTION__
                                    ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                    ,"Nesting Tree Node"
                                    ,"ReducedNestingTreeNode"));

    return m_nestingTreeNode;
}


double ReducedNestingTreeNode::getAveragePosition()
{
    return m_dAveragePosition;
}

void ReducedNestingTreeNode::setAveragePosition(double dAveragePosition)
{
    m_dAveragePosition = dAveragePosition;
}

int ReducedNestingTreeNode::getChildLayerNodeCount()
{
    return m_iLayerNodeCount;
}


void ReducedNestingTreeNode::setChildLayerNodeCount(int iLayerNodeCount)
{
    LAYOUT_ASSERT(iLayerNodeCount >= 0 ,
             LayoutException(__FUNCTION__
                             ,LayoutExceptionEnum::INVALID_PARAMETER
                             ,QString::number(iLayerNodeCount)
                             ,"iLayerNodeCount"));

    m_iLayerNodeCount = iLayerNodeCount;
}

ReducedNestingTreeNode::IteratorMapAvgPosToChildRNTNode ReducedNestingTreeNode::getChildNodesIterator()
{
    IteratorMapAvgPosToChildRNTNode iterChildNodes(m_mapChildrenRNTNodes);
    return iterChildNodes;
}
