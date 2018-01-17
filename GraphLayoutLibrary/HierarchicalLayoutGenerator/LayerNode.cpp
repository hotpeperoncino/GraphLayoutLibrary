#include "LayerNode.h"

LayerNode::LayerNode()
{
    m_parentNestingTreeSubgraphNode = NULL;
    m_vGlobalVertex = 0;
    m_iSubgraphOrderingGraphVertexIndex = -1;
}

LayerNode::LayerNode(VertexDescriptor vGlobalVertex)
{
    m_parentNestingTreeSubgraphNode = NULL;
    m_vGlobalVertex = vGlobalVertex;
    m_iSubgraphOrderingGraphVertexIndex = -1;

}

LayerNode::LayerNode(NestingTreeSubgraphNode &parentNestingTreeSubgraphNode)
{
    m_parentNestingTreeSubgraphNode = &parentNestingTreeSubgraphNode;
    m_vGlobalVertex = 0;
    m_iSubgraphOrderingGraphVertexIndex = -1;

}

LayerNode::LayerNode(NestingTreeSubgraphNode &parentNestingTreeSubgraphNode, VertexDescriptor vGlobalVertex)
{
    setValues(parentNestingTreeSubgraphNode , vGlobalVertex);
    m_iSubgraphOrderingGraphVertexIndex = -1;

}

void LayerNode::setParentNestingTreeSubgraphNode(NestingTreeSubgraphNode& parentNestingTreeSubgraphNode)
{
    m_parentNestingTreeSubgraphNode = &parentNestingTreeSubgraphNode;
}

NestingTreeSubgraphNode &LayerNode::getParentNestingTreeSubgraphNode()
{
    Q_ASSERT_X(m_parentNestingTreeSubgraphNode != NULL , "Layer Node" , "Parent nesting tree subgraph node is not set");

    return *m_parentNestingTreeSubgraphNode;
}

void LayerNode::setVertex(VertexDescriptor vGlobalVertex)
{
    m_vGlobalVertex = vGlobalVertex;
}

VertexDescriptor LayerNode::getVertex()
{
    return m_vGlobalVertex;
}

void LayerNode::setValues(NestingTreeSubgraphNode &parentNestingTreeSubgraphNode, VertexDescriptor vGlobalVertex)
{
    setParentNestingTreeSubgraphNode(parentNestingTreeSubgraphNode);
    setVertex(vGlobalVertex);
}

int LayerNode::setTopologicalOrder(int iTopologicalOrder)
{
    m_iTopologocalOrder = iTopologicalOrder;
}

int LayerNode::getTopologicalOrder()
{
    return m_iTopologocalOrder;
}

void LayerNode::setSubgraphOrderingGraphVertex(int iSubgraphOrderingGraphVertexIndex)
{
    m_iSubgraphOrderingGraphVertexIndex = iSubgraphOrderingGraphVertexIndex;
}

int LayerNode::getSubgraphOrderingGraphVertex()
{
    return m_iSubgraphOrderingGraphVertexIndex;
}

void LayerNode::printName()
{
    SubGraph& gGraph = m_parentNestingTreeSubgraphNode->getGraph();

    SubGraph& gRootGraph = gGraph.root();

    BoostGraphWrapper boostGraphWrapper;

    QString sName = boostGraphWrapper.getVertexId(m_vGlobalVertex , gRootGraph);

    sName.append(" V: ");

    sName.append((QString::number((int)m_vGlobalVertex)));

    qDebug() << sName;

}
