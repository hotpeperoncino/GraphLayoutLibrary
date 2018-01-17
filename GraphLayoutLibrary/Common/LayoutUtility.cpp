#include "LayoutUtility.h"

LayoutUtility::LayoutUtility()
{

}

void LayoutUtility::setCompartment(SubGraph &gSubgraph, SubGraph &gMainGraph)
{
    int iLowestX, iLowestY, iHighestX, iHighestY;
    int  iHeight, iWidth;
    iLowestX = iLowestY = iHighestX = iHighestY = 0;
    m_iMargin = 15;

    BGL_FORALL_VERTICES(vVertex, gSubgraph, SubGraph)
    {
        VertexDescriptor vGlobalVertex = gSubgraph.local_to_global(vVertex);

        qDebug() << "V: " << PGL_VERTEX_INDEX(vGlobalVertex , gMainGraph) ;
        int iXCoordinate, iYCoordinate;
        if(m_GraphWrapper.getVertexExpandable(vGlobalVertex , gMainGraph) ||
               m_GraphWrapper.getVertexIsInvisible(vGlobalVertex , gMainGraph) )
        {
            //skip the expandable or invisible vertex
            continue;
        }
        ;iXCoordinate = m_GraphWrapper.getVertexCenterCoordX(vGlobalVertex , gMainGraph);
        iYCoordinate = m_GraphWrapper.getVertexCenterCoordY(vGlobalVertex , gMainGraph);

        if (iLowestX > iXCoordinate)
            (iLowestX = iXCoordinate);

        if (iLowestY > iYCoordinate)
            (iLowestY = iYCoordinate);

        if (iHighestX < iXCoordinate)
            (iHighestX = iXCoordinate);

        if (iHighestY < iYCoordinate)
            (iHighestY = iYCoordinate);

    }
    iHeight =  std::abs(std::abs(iLowestY) -  std::abs(iHighestY));
    iWidth =  std::abs(std::abs(iLowestX) -  std::abs(iHighestX));
    qDebug()<<"Low X :"<<iLowestX;
    qDebug()<<"Low Y :"<<iLowestY;
    qDebug()<<"Height :"<<iHeight;
    qDebug()<<"Width :"<<iWidth;
    int iCompartmentLowX = (iLowestX - m_iMargin);
    int iCompartmentLowY = (iLowestY - m_iMargin);
    int iCompartmentHeight = (iHeight + (2 * m_iMargin));
    int iCompartmentWidth = (iWidth + (2 * m_iMargin));
    m_GraphWrapper.setGraphCoordX(iCompartmentLowX , gSubgraph);
    m_GraphWrapper.setGraphCoordY(iCompartmentLowY , gSubgraph);
    m_GraphWrapper.setGraphHeight(iCompartmentHeight , gSubgraph);
    m_GraphWrapper.setGraphWidth(iCompartmentWidth , gSubgraph);

}

void LayoutUtility::setCompartmentTest(SubGraph &gSubgraph)
{
    int iLowestX, iLowestY, iHighestX, iHighestY;
    int  iHeight, iWidth;
    iLowestX = iLowestY = iHighestX = iHighestY = 0;
    m_iMargin = 15;

    BGL_FORALL_VERTICES(vVertex, gSubgraph, SubGraph)
    {

        qDebug() << "V: " << PGL_VERTEX_INDEX(vVertex , gSubgraph) ;
        int iXCoordinate, iYCoordinate;
        if(m_GraphWrapper.getVertexExpandable(vVertex , gSubgraph) ||
               m_GraphWrapper.getVertexIsInvisible(vVertex , gSubgraph) )
        {
            //skip the expandable or invisible vertex
            continue;
        }
        iXCoordinate = m_GraphWrapper.getVertexCenterCoordX(vVertex , gSubgraph);
        iYCoordinate = m_GraphWrapper.getVertexCenterCoordY(vVertex , gSubgraph);

        if (iLowestX > iXCoordinate)
            (iLowestX = iXCoordinate);

        if (iLowestY > iYCoordinate)
            (iLowestY = iYCoordinate);

        if (iHighestX < iXCoordinate)
            (iHighestX = iXCoordinate);

        if (iHighestY < iYCoordinate)
            (iHighestY = iYCoordinate);

    }
    iHeight =  std::abs(std::abs(iLowestY) -  std::abs(iHighestY));
    iWidth =  std::abs(std::abs(iLowestX) -  std::abs(iHighestX));
    qDebug()<<"Low X :"<<iLowestX;
    qDebug()<<"Low Y :"<<iLowestY;
    qDebug()<<"Height :"<<iHeight;
    qDebug()<<"Width :"<<iWidth;
    int iCompartmentLowX = (iLowestX - m_iMargin);
    int iCompartmentLowY = (iLowestY - m_iMargin);
    int iCompartmentHeight = (iHeight + (2 * m_iMargin));
    int iCompartmentWidth = (iWidth + (2 * m_iMargin));
    m_GraphWrapper.setGraphCoordX(iCompartmentLowX , gSubgraph);
    m_GraphWrapper.setGraphCoordY(iCompartmentLowY , gSubgraph);
    m_GraphWrapper.setGraphHeight(iCompartmentHeight , gSubgraph);
    m_GraphWrapper.setGraphWidth(iCompartmentWidth , gSubgraph);

}

void LayoutUtility::reinitializeEdgeIndices(graph &gInputGraph)
{
    property_map<graph, edge_index_t>::type e_index = get(edge_index, gInputGraph);
    graph_traits<graph>::edges_size_type edge_count = 0;
    graph_traits<graph>::edge_iterator ei, ei_end;
    for(boost::tie(ei, ei_end) = edges(gInputGraph); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);
}

void LayoutUtility::reinitializeEdgeIndices(SubGraph &gInputSubGraph)
{
    property_map<SubGraph, edge_index_t>::type in_e_index = get(edge_index, gInputSubGraph);
    graph_traits<SubGraph>::edges_size_type in_edge_count = 0;
    graph_traits<SubGraph>::edge_iterator in_ei, in_ei_end;
    for(boost::tie(in_ei, in_ei_end) = edges(gInputSubGraph); in_ei != in_ei_end; ++in_ei)
        put(in_e_index, *in_ei, in_edge_count++);
}

void LayoutUtility::reinitializeEdgeIndices(dijkstra_graph & gInputDijkGraph)
{
    property_map<dijkstra_graph, edge_index_t>::type in_e_index = get(edge_index, gInputDijkGraph);
    graph_traits<dijkstra_graph>::edges_size_type in_edge_count = 0;
    graph_traits<dijkstra_graph>::edge_iterator in_ei, in_ei_end;
    for(boost::tie(in_ei, in_ei_end) = edges(gInputDijkGraph); in_ei != in_ei_end; ++in_ei)
        put(in_e_index, *in_ei, in_edge_count++);
}

//void LayoutUtility::copyCompartmentProperties(SubGraph &gCopyGraph,
//                                              QMap<int, SubGraph*> &mapClusterIdToChild)
//{
//    //for each chld of current graph
//    ChildrenIterator itrChild, itrChildEnd;
//    for(boost::tie(itrChild, itrChildEnd)=gCopyGraph.children(); itrChild!=itrChildEnd; ++itrChild)
//    {
//        //recursive
//        //SubGraph gChild = *itrChild;
//        copyCompartmentProperties(*itrChild, mapClusterIdToChild);
//    }

//    BoostGraphWrapper boostGraphWrapper;
//    //get child from mainGraph corres. to current graph
//    int iCurrentClusterID = boostGraphWrapper.getGraphClusterID(gCopyGraph);
//    LAYOUT_ASSERT(mapClusterIdToChild.contains(iCurrentClusterID),
//                  LayoutException("copyCompartmentProperties",
//                                  LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
//                                  "mapClusterIDToChild",
//                                  "ClusterID"));
//    SubGraph* gCorrespondingChild = mapClusterIdToChild.value(iCurrentClusterID);
//    //get its properties
//    int iChildHeight, iChildWidth, iLeftTopX, iLeftTopY;
//    iChildHeight = boostGraphWrapper.getGraphHeight(*gCorrespondingChild);
//    iChildWidth = boostGraphWrapper.getGraphWidth(*gCorrespondingChild);
//    iLeftTopX = boostGraphWrapper.getGraphLeftTopCoordX(*gCorrespondingChild);
//    iLeftTopY = boostGraphWrapper.getGraphLeftTopCoordY(*gCorrespondingChild);
//    //set properties of current graph
//    boostGraphWrapper.setGraphHeight(iChildHeight, gCopyGraph);
//    boostGraphWrapper.setGraphWidth(iChildWidth, gCopyGraph);
//    boostGraphWrapper.setGraphLeftTopCoordX(iLeftTopX, gCopyGraph);
//    boostGraphWrapper.setGraphLeftTopCoordY(iLeftTopY, gCopyGraph);

//    qDebug()<<"copying C"<<iCurrentClusterID<<"("<<iChildHeight<<","<<iChildWidth<<","<<iLeftTopX<<","<<iLeftTopY<<")\n";
//}
