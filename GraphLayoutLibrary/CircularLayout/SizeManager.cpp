#include "SizeManager.h"

SizeManager::SizeManager()
{

}

double SizeManager::calculateAreaOfNode(VertexDescriptor vVertex, SubGraph &gSubgraph)
{
    Q_ASSERT_X(&gSubgraph != NULL,
               "Calculate area of node",
               "Trying to calculate area of node without asigning graph to method");

    // Get properties from vertex
    int iHeight = m_boostGraphWrapper.getVertexHeight(vVertex, gSubgraph);
    int iWidth = m_boostGraphWrapper.getVertexWidth(vVertex, gSubgraph);

    // Calculate Area
    double dNodeArea = ((iHeight * iWidth) / 2);
    return dNodeArea;
}

double SizeManager::calculateAreaOfCluster(SubGraph &gSubgraph)
{
    Q_ASSERT_X(&gSubgraph != NULL,
               "Calculate area of cluster",
               "Trying to calculate area of cluster without asigning graph to method");

    /* To calculate the area of rectangle
       area of rectagle = (iHeight * iWidth) / 2.
    */
    int iHeight = m_boostGraphWrapper.getGraphHeight(gSubgraph);
    int iWidth = m_boostGraphWrapper.getGraphWidth(gSubgraph);
    double dClusterArea = ((iHeight * iWidth) / 2);
    return dClusterArea;
}

int SizeManager::calculateNodeLeftXFromCenterX(VertexDescriptor vVertex, SubGraph &gSubgraph)
{
    Q_ASSERT_X(&gSubgraph != NULL,
             "Calculate left x coordinate from center x",
             "Trying to calculate left x coordinate of node without asigning graph to method");

    /* To calculate the node rectangle's left X coordinate
       Caculate center X coordinate and
       subtract the width/2 factor
    */
    int iCenterCoordX = m_boostGraphWrapper.getVertexCenterCoordX(vVertex, gSubgraph);
    int iWidth = m_boostGraphWrapper.getVertexWidth(vVertex, gSubgraph);
    int iWidthShareCoordX = iWidth / 2;
    int iLeftCoordX = iCenterCoordX - iWidthShareCoordX;
    return iLeftCoordX;
}

int SizeManager::calculateNodeTopYFromCenterY(VertexDescriptor vVertex, SubGraph &gSubgraph)
{
    /*  To calculate the node rectangle's top Y coordinate
        Calculate center Y coordinate and
        subtract the height/2 factor
    */
    Q_ASSERT_X(&gSubgraph != NULL,
             "Calculate top y coordinate from center y",
             "Trying to calculate top y coordinate of node without asigning graph to method");

    int iCenterCoordY = m_boostGraphWrapper.getVertexCenterCoordY(vVertex, gSubgraph);
    int iHeight = m_boostGraphWrapper.getVertexHeight(vVertex, gSubgraph);
    int iHeightShareCoordY = iHeight / 2;
    int iTopCoordY = iCenterCoordY - iHeightShareCoordY;
    return iTopCoordY;
}

double SizeManager::calculateDiagonalOfNode(VertexDescriptor& vVertex, SubGraph& gSubgraph)
{
    /**
      This function first gets the vertex height and width and then
      calculates the diagonal as per the pythagoras formula
     */

    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));
    int iHeight;
    int iWidth;
    try
    {
        LayoutEnum::NodeType nodeType = m_boostGraphWrapper.getVertexType(vVertex, gSubgraph);
        // consider height and width for dummy node as per its contents in terms of number of nodes
        if(nodeType == LayoutEnum::DummyNode)
        {
            // get the subgraph vertices count related with this dummy node
            VertexDescriptor vGlobalVertex = (gSubgraph).local_to_global(vVertex);
            int iNumVerticesInDummyNode =  (gSubgraph.root())[vGlobalVertex].iVertexCount;

            iHeight = m_boostGraphWrapper.getVertexHeight(vVertex, gSubgraph);
            iHeight  = iHeight + ((iNumVerticesInDummyNode * (int)(2 * PI * RADIUS_SHARE))) + (2*RADIUS_SHARE);
            iWidth = m_boostGraphWrapper.getVertexWidth(vVertex, gSubgraph);
            iWidth = iWidth + ((iNumVerticesInDummyNode * (int)(2 * PI * RADIUS_SHARE))) +( 2*RADIUS_SHARE);
        }
        else
        {
            iHeight = m_boostGraphWrapper.getVertexHeight(vVertex, gSubgraph);
            iWidth = m_boostGraphWrapper.getVertexWidth(vVertex, gSubgraph);
        }
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    double dNodeDiagonalSquare = (double)((iHeight * iHeight) + (iWidth * iWidth));
    double dNodeDiagonalLength = (double)qSqrt(dNodeDiagonalSquare);
    return dNodeDiagonalLength;
}


int SizeManager::calculateGraphLeftXFromCenterX(SubGraph &gSubgraph)
{
    Q_ASSERT_X(&gSubgraph != NULL,
               "Calculate left x coordinate from center x of cluster",
               "Trying to calculate left x coordinate of cluster without asigning graph to method");

    // get the center x coordinate for this cluster
    int iGraphCentroidCoordX = calculateGraphCenterCoordX(gSubgraph);

    // get the width for this cluster
    int iWidth = m_boostGraphWrapper.getGraphWidth(gSubgraph);
    int iWidthShareCoordX = iWidth / 2;

    // deduct half length of width
    int iGraphLeftCoordX = iGraphCentroidCoordX - iWidthShareCoordX;

    return iGraphLeftCoordX;
}

int SizeManager::calculateGraphTopYFromCenterY(SubGraph &gSubgraph)
{
    Q_ASSERT_X(&gSubgraph != NULL,
             "Calculate top y coordinate from center y of cluster",
             "Trying to calculate top y coordinate of cluster without asigning graph to method");

    // get the center y coordinate from this cluster
    int iGraphCentroidCoordY = calculateGraphCenterCoordY(gSubgraph);

    // get the height for this cluster
    int iHeight = m_boostGraphWrapper.getGraphHeight(gSubgraph);
    int iHeightShareCoordY = iHeight / 2;

    // deduct half length of height
    int iGraphTopCoordY = iGraphCentroidCoordY - iHeightShareCoordY;

    return iGraphTopCoordY;
}

int SizeManager::calculateGraphWidth(SubGraph &gSubgraph)
{
    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));
    int iNodeMaximumWidth = 0;
    int iVertexCount = num_vertices(gSubgraph);
    // calculate the maximum node width amoung all the nodes
    BGL_FORALL_VERTICES(vVertex, gSubgraph, SubGraph)
    {
        LayoutEnum::NodeType nodeType = m_boostGraphWrapper.getVertexType(vVertex,gSubgraph);
        if(LayoutEnum::isValidCircularVertexType(nodeType) == true)
        {
            if(nodeType == LayoutEnum::DummyNode)
            {
                // Skip Dummy Nodes
            }
            else
            {
                int iLocalWidth = m_boostGraphWrapper.getVertexWidth(vVertex, gSubgraph);
                if(iLocalWidth > 0)
                {
                    if(iNodeMaximumWidth < iLocalWidth)
                    {
                        iNodeMaximumWidth = iLocalWidth;
                    }
                }
                else
                {
                    //cout<<"Exception : Invalid Height value to vertex in calculateGraphWidth()"<<endl;
                    throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE, WIDTH_VALUE, VERTEX);
                }
            }
        }
        else
        {
            //cout<<"Invalid Node Type in circular layout for calculating graph width"<<endl;
            throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_TYPE, INVALID_NODE_TYPE);
        }
    }
    // calculate the width increased due to radius share
    int iSpaceError = iVertexCount * RADIUS_SHARE;
//    //cout<<"Graph Width : "<<iNodeMaximumWidth<<" ";
//    //cout<<"Graph Space : "<<iSpaceError<<" ";
//    //cout<<"Total Width : "<<(iNodeMaximumWidth + iSpaceError)<<endl;
    int iSpaceWidth = iNodeMaximumWidth + iSpaceError;

    // for cluster inside the main graph
    double dClusterRadius;
    try
    {
        dClusterRadius = calculateGraphMaxRadiusUsingDFS(gSubgraph);
    }
    catch(...)
    {throw;}
    // calculate the total width for the cluster
    int iWidth = iSpaceWidth + ((int)dClusterRadius * 2);
//    //cout<<"Graph Width : "<<iWidth<<endl;

    return iWidth;
}

int SizeManager::calculateGraphHeight(SubGraph &gSubgraph)
{
    Q_ASSERT_X(&gSubgraph != NULL,
               "Calculate height of cluster",
               "Trying to calculate height of cluster without asigning graph to method");

    int iGraphHeight = 0;
    int iVertexCount = num_vertices(gSubgraph);

    // calculate the maximum node height amoung all the nodes
    BGL_FORALL_VERTICES(vVertex, gSubgraph, SubGraph)
    {
        LayoutEnum::NodeType nodeType = m_boostGraphWrapper.getVertexType(vVertex,gSubgraph);
        if(LayoutEnum::isValidCircularVertexType(nodeType) == true)
        {
            if(nodeType == LayoutEnum::DummyNode)
            {
                // Skip Dummy Nodes
            }
            else
            {
                int iHeight;
                try
                {
                    iHeight = m_boostGraphWrapper.getVertexHeight(vVertex, gSubgraph);
                }
                catch(boost::exception& eBoostException)
                {
                    throw *boost::get_error_info<errmsg_info>(eBoostException);
                }
                if(iHeight > 0)
                {
                    if(iGraphHeight < iHeight)
                    {
                        iGraphHeight = iHeight;
                    }
                }
                else
                {
                    //cout<<"Exception : Invalid Height value to vertex in calculateGraphHeight()"<<endl;
                    throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE, HEIGHT_VALUE, VERTEX);
                }
            }
        }
        else
        {
            //cout<<"Invalid node type for the circular layout in calculating the graph height"<<endl;
            throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_TYPE, INVALID_NODE_TYPE);
        }
    }

    // calculate the height increased due to radius share
    int iSpaceError = iVertexCount * RADIUS_SHARE;
//    //cout<<"Graph Height : "<<iGraphHeight<<" ";
//    //cout<<"Graph Space : "<<iSpaceError<<" ";
//    //cout<<"Total Height : "<<(iGraphHeight + iSpaceError)<<endl;
    int iSpaceHeight = iGraphHeight + iSpaceError;

    //for cluster inside the main graph
    double dClusterRadius;
    try
    {
        dClusterRadius = calculateGraphMaxRadiusUsingDFS(gSubgraph);
    }
    catch(LayoutMemoryException& eException)
    {
        throw LayoutMemoryException(__FUNCTION__,
                              LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                              eException.getObjectName());
    }
    catch(LayoutException& eException)
    {
        throw LayoutException(__FUNCTION__,
                              LayoutExceptionEnum::INVALID_PARAMETER,
                              eException.getEntityValue());
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    { }

    // calculate the total height for the cluster
    int iHeight = iSpaceHeight + ((int)dClusterRadius * 2);
//    //cout<<"Graph Height : "<<iHeight<<endl;

    return iHeight;
}

int SizeManager::calculateGraphCenterCoordX(SubGraph &gSubgraph)
{
    Q_ASSERT_X(&gSubgraph != NULL,
               "Calculate center x of cluster",
               "Trying to calculate center x  of cluster without asigning graph to method");
    int iVertexCount = 0;
    int iTotalCoordX = 0;

    // caculate the total value by adding the values of all x-coordinate of this cluster
    BGL_FORALL_VERTICES(vVertex, gSubgraph, SubGraph)
    {
        LayoutEnum::NodeType nodeType = m_boostGraphWrapper.getVertexType(vVertex,gSubgraph);
        if(LayoutEnum::isValidCircularVertexType(nodeType) == true)
        {
            if(nodeType == LayoutEnum::DummyNode)
            {
                // Skip Dummy Nodes
            }
            else
            {
                int iVertexCenterX = m_boostGraphWrapper.getVertexCenterCoordX(vVertex, gSubgraph);
                iTotalCoordX += iVertexCenterX;
                iVertexCount++;
            }
        }
        else
        {
            //cout<<"Invalid node type in circular layout for calculating the center coord X of graph"<<endl;
        }
    }

    // calculate the centroid of this cluster
    int iGraphCentroidCoordX = iTotalCoordX / iVertexCount;
//    //cout<<"Graph Center X : "<<iGraphCentroidCoordX<<endl;

    /*
     *Calculate displacement between centroidX and center X
      of the graph to be used by the second pass of the circular layout
    */
    int iGraphCenterCoordX = m_boostGraphWrapper.getGraphCenterCoordX(gSubgraph);
    int iDistanceOfCenterFromCentroidInGraphX = iGraphCentroidCoordX - iGraphCenterCoordX;
//    std:://cout<<"Distance X : "<<iDistanceOfCenterFromCentroidInGraphX<<std::endl;
    m_boostGraphWrapper.setGraphDistanceBetweenCentoidAndCenterCoordX(iDistanceOfCenterFromCentroidInGraphX, gSubgraph);

    return iGraphCentroidCoordX;
}

int SizeManager::calculateGraphCenterCoordY(SubGraph &gSubgraph)
{
    Q_ASSERT_X(&gSubgraph != NULL,
               "Calculate center y of cluster",
               "Trying to calculate center y of cluster without asigning graph to method");
    int iVertexCount = 0;
    int iTotalCoordY = 0;

    // caculate the total value by adding the values of all y-coordinate of this cluster
    BGL_FORALL_VERTICES(vVertex, gSubgraph, SubGraph)
    {
        LayoutEnum::NodeType nodeType = m_boostGraphWrapper.getVertexType(vVertex,gSubgraph);
        if(nodeType == LayoutEnum::DummyNode)
        {
            // Skip Dummy Nodes
        }
        else
        {
        int iVertexCenterY = m_boostGraphWrapper.getVertexCenterCoordY(vVertex, gSubgraph);
        iTotalCoordY += iVertexCenterY;
        iVertexCount++;
        }
    }

    // calculate the centroid of this cluster
    int iGraphCentroidCoordY = iTotalCoordY / iVertexCount;
//    //cout<<"Graph Center Y : "<<iGraphCentroidCoordY<<endl;

    /*
     *Calculate displacement between centroidY and center Y
      of the graph to be used by the second pass of the circular layout
    */
    int iGraphCenterCoordY = m_boostGraphWrapper.getGraphCenterCoordY(gSubgraph);
    int iDistanceOfCenterFromCentroidInGraphY = iGraphCentroidCoordY - iGraphCenterCoordY;
//    std:://cout<<"Distance Y : "<<iDistanceOfCenterFromCentroidInGraphY<<std::endl;
    m_boostGraphWrapper.setGraphDistanceBetweenCentoidAndCenterCoordY(iDistanceOfCenterFromCentroidInGraphY, gSubgraph);

    return iGraphCentroidCoordY;
}

void SizeManager::processSizeManager(SubGraph &gSubgraph)
{
//    Q_ASSERT_X(&gSubgraph != NULL,
//               "process size manager in DFS way",
//               "Trying to process subgraphs in dfs without asigning graph to method");

    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));
    // Haldling for negative or invalid height value
    int iHeight;
    try
    {
        //calculate height
        iHeight = calculateGraphHeight(gSubgraph);
        // set the width in the graph property of this cluster
        m_boostGraphWrapper.setGraphHeight(iHeight, gSubgraph);
    }
    catch(...)
    {throw;}

    try
    {
        // calculate width
        int iWidth = calculateGraphWidth(gSubgraph);

        // set the width in the graph property of this cluster
        m_boostGraphWrapper.setGraphWidth(iWidth, gSubgraph);
    }
    catch(...)
    {throw;}

    // calculate graph's left x and top y coordinates
    int iGraphLeftCoordX = calculateGraphLeftXFromCenterX(gSubgraph);
    m_boostGraphWrapper.setGraphLeftTopCoordX(iGraphLeftCoordX,gSubgraph);

    int iGraphTopCoordY = calculateGraphTopYFromCenterY(gSubgraph);
    m_boostGraphWrapper.setGraphLeftTopCoordY(iGraphTopCoordY, gSubgraph);

    // iterate child graphs recursively
    ChildrenIterator itrGraph, itrGraphEnd;
    for(boost::tie(itrGraph, itrGraphEnd) = gSubgraph.children();
        itrGraph != itrGraphEnd;
        ++itrGraph)
    {
        processSizeManager(*itrGraph);
    }
}

double SizeManager::calculateGraphMaxRadiusUsingDFS(SubGraph &gSubgraph)
{
    /*
        This function will calculate the radius required for the graph considering its
        all deep level subgraphs and passes the maximum required radius to upper graph.
    */

    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));
    double dMaxChildRadius = 0;
    // get the own real radius for every subgraph at each recursive call
    double dGrphOwnRadius;
    try{
    dGrphOwnRadius = m_boostGraphWrapper.getGraphRadius(gSubgraph);
    }catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }

    ChildrenIterator itrSubgraph, itrSubgraphEnd;
    for(boost::tie(itrSubgraph, itrSubgraphEnd) = gSubgraph.children();
        itrSubgraph != itrSubgraphEnd;
        ++itrSubgraph)
    {
        // traverse upto deep level in the subgraph tree
        double dCurrentGraphRadius;
        dCurrentGraphRadius = calculateGraphMaxRadiusUsingDFS(*itrSubgraph);
        if(dCurrentGraphRadius > dMaxChildRadius)
        {
            dMaxChildRadius = dCurrentGraphRadius;
        }
    }
    if(dGrphOwnRadius > 0)
    {
        // add lower level childs radius to own radius
        dGrphOwnRadius += dMaxChildRadius;
    }
    else
    {
        throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_PARAMETER, INVALID_RADIUS_VALUE);
    }
    // Update graph's own radius with deep level max radius
    double dRadius;
    try
    {
        dRadius = m_boostGraphWrapper.getGraphRadius(gSubgraph);
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    if(dRadius > 0)
    {
        dGrphOwnRadius = dRadius + dMaxChildRadius;
    }
    else
    {
        throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_PARAMETER, INVALID_RADIUS_VALUE);
    }
//    //cout<<"Total Radius : "<<dGrphOwnRadius<<" "<<dRadius<<endl;
    return dGrphOwnRadius;
}
