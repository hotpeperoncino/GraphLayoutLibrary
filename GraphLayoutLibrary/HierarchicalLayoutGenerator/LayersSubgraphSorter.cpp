#include "LayersSubgraphSorter.h"

LayersSubgraphSorter::LayersSubgraphSorter()
{
}

LayersSubgraphSorter::LayersSubgraphSorter(NestingTreeSubgraphNode &rootNestingTreeNode, MapLayerIdToLayerRef &layeredGraph)
{
    m_rootNestingTreeNodeRef = &rootNestingTreeNode;
    m_mapLayeredGraphRef = &layeredGraph;
}
