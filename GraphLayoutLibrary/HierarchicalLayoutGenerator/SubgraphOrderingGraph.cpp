#include "SubgraphOrderingGraph.h"

SubgraphOrderingGraphWrapper::SubgraphOrderingGraphWrapper()
{
}

void SubgraphOrderingGraphWrapper::setVertexNestingTreeSubgraphNodeRef(NestingTreeSubgraphNode *nestingTreeNodeRef, SubgraphOrderingGraphVertexDescriptor vVertex, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph[vVertex].layerNode == NULL,
            LayoutException(__FUNCTION__
                            ,LayoutExceptionEnum::INVALID_OPERATION
                            ,"Setting Nesting Tree Node"
                            ,"SubgraphOrderingGraph"));

    gSubgraphOrderingGraph[vVertex].nestingTreeSubgraphNode = nestingTreeNodeRef;

    gSubgraphOrderingGraph[vVertex].bIsLayerNode = false;
}

NestingTreeSubgraphNode *SubgraphOrderingGraphWrapper::getVertexNestingTreeSubgraphNode(SubgraphOrderingGraphVertexDescriptor vVertex, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph[vVertex].nestingTreeSubgraphNode != NULL,
            LayoutException(__FUNCTION__
                            ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                            ,"nestingTreeSubgraphNode"
                            ,"SubgraphOrderingGraph"));

    return gSubgraphOrderingGraph[vVertex].nestingTreeSubgraphNode;
}

void SubgraphOrderingGraphWrapper::setVertexLayerNodeRef(LayerNode *layerNodeRef, SubgraphOrderingGraphVertexDescriptor vVertex, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph[vVertex].nestingTreeSubgraphNode == NULL,
             LayoutException(__FUNCTION__
                             ,LayoutExceptionEnum::INVALID_OPERATION
                             ,"Setting LayerNodeRef"
                             ,"SubgraphOrderingGraph"));

    gSubgraphOrderingGraph[vVertex].layerNode = layerNodeRef;

    gSubgraphOrderingGraph[vVertex].bIsLayerNode = true;
}

LayerNode *SubgraphOrderingGraphWrapper::getVertexLayerNodeRef(SubgraphOrderingGraphVertexDescriptor vVertex, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph[vVertex].layerNode != NULL,
             LayoutException(__FUNCTION__
                             ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                             ,"layerNode"
                             ,"SubgraphOrderingGraph"));
    return gSubgraphOrderingGraph[vVertex].layerNode;
}


bool SubgraphOrderingGraphWrapper::isSubgraphOrderingVertexIsLayerNode(SubgraphOrderingGraphVertexDescriptor vVertex, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph[vVertex].nestingTreeSubgraphNode == NULL ||
             gSubgraphOrderingGraph[vVertex].layerNode == NULL,
            LayoutException(__FUNCTION__
                            ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                            ,"layerNode and nestingTreeSubgraphNode"
                            ,"SubgraphOrderingGraph"));

    return gSubgraphOrderingGraph[vVertex].bIsLayerNode;
}

bool SubgraphOrderingGraphWrapper::getIsEdgeInSpanningTree(SubgraphOrderingGraphEdgeDescriptor eEdge, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph.find_edge(eEdge).second == true,
             LayoutException(__FUNCTION__
                             ,LayoutExceptionEnum::INVALID_OPERATION
                             ,"Edge does not exist"
                             ,"Getting IsEdgeInSpanningTree"));
    return gSubgraphOrderingGraph[eEdge].bIsSpanningTreeEdge;
}

void SubgraphOrderingGraphWrapper::setIsEdgeInSpanningTree(bool bIsSpannigTreeEdge, SubgraphOrderingGraphEdgeDescriptor eEdge, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph.find_edge(eEdge).second == true,
                  LayoutException(__FUNCTION__
                                  ,LayoutExceptionEnum::INVALID_OPERATION
                                  ,"Edge does not exist"
                                  ,"Getting IsEdgeInSpanningTree"));

    gSubgraphOrderingGraph[eEdge].bIsSpanningTreeEdge = bIsSpannigTreeEdge;
}

void SubgraphOrderingGraphWrapper::printGraph(SubgraphOrderingGraphType &gGraph)
{
    QString sLine = "";
    BGL_FORALL_VERTICES(vVertex , gGraph , SubgraphOrderingGraphType)
    {
        sLine ="";
        sLine.append(QString::number((int)vVertex));
        sLine.append(" --> ");

        BGL_FORALL_ADJ(vVertex , vAdjVertex , gGraph , SubgraphOrderingGraphType)
        {
            sLine.append(QString::number((int)vAdjVertex));
            sLine.append(" ");
        }

        qDebug() << sLine;

    }
}
