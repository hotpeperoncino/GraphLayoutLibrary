#include "SpaceUtilizer.h"

SpaceUtilizer::SpaceUtilizer()
{

}

void SpaceUtilizer::getCircleIntersection(SubGraph &gSubgraph, vector<int>& vectIntersetionPoints)
{

    LAYOUT_ASSERT(&gSubgraph != NULL,LayoutException(__FUNCTION__
                                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                  , "gSubgraph"
                                                  , ""));


    // Get Main graph circle's radius and center coordinates
    m_structMainCircleProperty.iCenterCoordX = m_boostGraphWrapper.getGraphCenterCoordX(gSubgraph);
    m_structMainCircleProperty.iCenterCoordY = m_boostGraphWrapper.getGraphCenterCoordY(gSubgraph);
    m_structMainCircleProperty.dRadius = m_boostGraphWrapper.getGraphRadius(gSubgraph);

    // Get subgraph circle's radius and center coordinates
    for(boost::tie(m_ItrSubgraph, m_ItrSubgraphEnd) = gSubgraph.children();
        m_ItrSubgraph != m_ItrSubgraphEnd;
        m_ItrSubgraph++)
    {
    CircleProperty structSubCircleProperty;
    structSubCircleProperty.iCenterCoordX = m_boostGraphWrapper.getGraphCenterCoordX(*m_ItrSubgraph);
    structSubCircleProperty.iCenterCoordY = m_boostGraphWrapper.getGraphCenterCoordY(*m_ItrSubgraph);
    structSubCircleProperty.dRadius = m_boostGraphWrapper.getGraphRadius(*m_ItrSubgraph);

    // Function call for getting the intersecting coodinates of main and sub graph circles
    getCircleIntersectionCoordinates(structSubCircleProperty, vectIntersetionPoints);
    }

}

void SpaceUtilizer::getCircleIntersectionCoordinates(CircleProperty structSubCircleProperty, vector<int>& vectIntersetionPoints)
{
    LAYOUT_ASSERT(structSubCircleProperty.dRadius > 0,LayoutException(__FUNCTION__
                                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                  , "dRadius"
                                                  , "in getting CircleIntersectionCoordinates"));

    // Main Circle Properties
    int iMainCircleCenterCoordX = m_structMainCircleProperty.iCenterCoordX;
    int iMainCircleCenterCoordY = m_structMainCircleProperty.iCenterCoordY;
    double dMainCircleRadius = m_structMainCircleProperty.dRadius;
    std::cout<<"Main Circle : "<<iMainCircleCenterCoordX<<" "<<iMainCircleCenterCoordY<<" "<<dMainCircleRadius<<std::endl;

    // Sub Circle Properties
    int iSubCircleCenetrCoordX = structSubCircleProperty.iCenterCoordX;
    int iSubCircleCenetrCoordY = structSubCircleProperty.iCenterCoordY;
    double dSubCircleRadius = structSubCircleProperty.dRadius;

    // Calculate distance between the centers of two circles
    boost::geometry::model::d2::point_xy<int> MainCircleCenter(iMainCircleCenterCoordX, iMainCircleCenterCoordY);
    boost::geometry::model::d2::point_xy<int> SubCircleCenter(iSubCircleCenetrCoordX, iSubCircleCenetrCoordY);

    double dCircleCenterDistance = boost::geometry::distance(MainCircleCenter, SubCircleCenter);
    double dTotalDistance = dMainCircleRadius + dSubCircleRadius;
    double dRadiusDifference = dMainCircleRadius - dSubCircleRadius;

    if(dRadiusDifference < 0)
    {
        dRadiusDifference = dRadiusDifference * NEGATIVE_FACTOR;
    }

    // No Solution
    if(dCircleCenterDistance > dTotalDistance)
    {
        std::cout<<"No solution : Distance between centers is greater than total distance"<<std::endl;
    }

    else if(dCircleCenterDistance < dRadiusDifference)
    {
        std::cout<<"Circles are contained within each other"<<std::endl;
    }

    else if(dCircleCenterDistance == 0 && dMainCircleRadius == dSubCircleRadius)
    {
        std::cout<<"Circles are same"<<std::endl;
    }

    else
    {
        // Calculate distance between one circle's center and line joining the intersection points
        double dFirstSegment = ( ((dMainCircleRadius * dMainCircleRadius) - (dSubCircleRadius * dSubCircleRadius))
                                 + (dCircleCenterDistance * dCircleCenterDistance) ) / (2 * dCircleCenterDistance);

        // Calculate distance between point of intersection and line joining two centers of circle
        double dSecondSegment = ( (dMainCircleRadius * dMainCircleRadius) - (dFirstSegment * dFirstSegment));
        dSecondSegment = sqrt(dSecondSegment);
        // Calculate point coordinates, where the line through the circle intersection points crosses the line between the circle centers.
        double dCrossPointX = (iMainCircleCenterCoordX +
                               ( (dFirstSegment / dCircleCenterDistance) * (iSubCircleCenetrCoordX - iMainCircleCenterCoordX)));
        double dCrossPointY = (iMainCircleCenterCoordY +
                               ( (dFirstSegment / dCircleCenterDistance) * (iSubCircleCenetrCoordY - iMainCircleCenterCoordY)));


        // Calculate the points of intersection of circles
        int iFirstPointX = (int)(dCrossPointX +
                                 ((dSecondSegment / dCircleCenterDistance) * (iSubCircleCenetrCoordY - iMainCircleCenterCoordY)));
        int iFirstPointY = (int)(dCrossPointY -
                                 ((dSecondSegment / dCircleCenterDistance) * (iSubCircleCenetrCoordX - iMainCircleCenterCoordX)));
        int iSecondPointX = (int)(dCrossPointX -
                                  ((dSecondSegment / dCircleCenterDistance) * (iSubCircleCenetrCoordY - iMainCircleCenterCoordY)));;
        int iSecondPointY = (int)(dCrossPointY +
                                  ((dSecondSegment / dCircleCenterDistance) * (iSubCircleCenetrCoordX - iMainCircleCenterCoordX)));;
        std::cout<<"Intersection Points"<<iFirstPointX<<" "<<iFirstPointY<<" "<<iSecondPointX<<" "<<iSecondPointY<<std::endl;
        vectIntersetionPoints.push_back(iFirstPointX);
        vectIntersetionPoints.push_back(iFirstPointY);
        vectIntersetionPoints.push_back(iSecondPointX);
        vectIntersetionPoints.push_back(iSecondPointY);
    }
}

double SpaceUtilizer::getIntersectionPointAngle(int iCoordX, int iCoordY, SubGraph &gSubgraph)
{
    /**
    This function calculates the angle on the circumference of the circle.
    to calculate angle we require cos inverse functionality and os inverse has valid range
    of (0 to 180) degrees.
    Hence we use (360- angle) for angle values int range (180 to 360)
    */

    LAYOUT_ASSERT(&gSubgraph != NULL,LayoutException(__FUNCTION__
                                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                  , "gSubgraph"
                                                  , ""));

    int iCenterCoordX = m_boostGraphWrapper.getGraphCenterCoordX(gSubgraph);
    double dRadius = m_boostGraphWrapper.getGraphRadius(gSubgraph);
    double dExpressionValue = ((iCoordX - iCenterCoordX) / dRadius);
    double dAngleDegrees;

    // Checking 3rd and 4th quadrant as cos inverse is valid range
    if((iCoordX <= 0 && iCoordY <= 0) || (iCoordX >= 0 && iCoordY <= 0))
    {
        dAngleDegrees = (qAcos(dExpressionValue)) * (PI_VALUE_DEGREE / PI);
    }
    // Checking 1st and second quadrant as cos inverse is invalid range
    else
    {
        dAngleDegrees = TWO_PI_VALUE_DEGREE - ((qAcos(dExpressionValue)) * (PI_VALUE_DEGREE / PI));
    }
    return dAngleDegrees;
}

void SpaceUtilizer::getSubgraphMinMaxVertices(SubGraph &gGraph, vector<VertexDescriptor> &vectMinMaxVertices)
{
    LAYOUT_ASSERT(&gGraph != NULL,LayoutException(__FUNCTION__
                                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                  , "gGraph"
                                                  , ""));

    VertexDescriptor vMinVertex;
    VertexDescriptor vMaxVertex;
    for(boost::tie(m_ItrSubgraph, m_ItrSubgraphEnd) = gGraph.children();
        m_ItrSubgraph != m_ItrSubgraphEnd;
        m_ItrSubgraph++)
    {
        m_boostGraphWrapper.getMinMaxVertices(*m_ItrSubgraph, vMinVertex, vMaxVertex);
        //cout<<"Min - Max Vertices : "<<vMinVertex<<" "<<vMaxVertex<<endl;
        vectMinMaxVertices.push_back(vMinVertex);
        vectMinMaxVertices.push_back(vMaxVertex);
    }
}

void SpaceUtilizer::getSubgraphMinMaxVerticesOrder(SubGraph &gGraph, vector<int> &vectMinMaxVerticesOrder)
{
    LAYOUT_ASSERT(&gGraph != NULL,LayoutException(__FUNCTION__
                                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                  , "gGraph"
                                                  , ""));

    int iMinOrder;
    int iMaxOrder;

    for(boost::tie(m_ItrSubgraph, m_ItrSubgraphEnd) = gGraph.children();
        m_ItrSubgraph != m_ItrSubgraphEnd;
        m_ItrSubgraph++)
    {
        m_boostGraphWrapper.getMinMaxVertexOrder(*m_ItrSubgraph, iMinOrder, iMaxOrder);

        vectMinMaxVerticesOrder.push_back(iMinOrder);
        vectMinMaxVerticesOrder.push_back(iMaxOrder);
    }
}

void SpaceUtilizer::claculateVerticesBetweenMinMax(SubGraph &gGraph, int iMinOrder, int iMaxOrder,  MapOrderVertex& mapMinMaxOrderVertex )
{
    LAYOUT_ASSERT(&gGraph != NULL,LayoutException(__FUNCTION__
                                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                  , "gGraph"
                                                  , ""));

    // Get start of vertex list Order
    int iStartVertexOrder = iMinOrder + 1;
    // Get Last of vertex list Order
    int iLastVertexOrder = iMaxOrder - 1;

    // Prepare list of vertices order
    PGL_MAP_VERTEX_ORDER(mapVertexOrder,gGraph);
    MapOrderVertex mapOrderVertex;
    mapOrderVertex = m_boostGraphWrapper.getMapOrderedVertices(gGraph, mapVertexOrder);

    cout << "Vertices order between min and max : " << endl;
    IteratorMapOrderVertex mapIter(mapOrderVertex);
    while(mapIter.hasNext())
    {
        mapIter.next();
        std::size_t key = mapIter.key();
        VertexDescriptor vVertex = mapOrderVertex[key];

        if(iStartVertexOrder < iLastVertexOrder)
        {
            if((key >= iStartVertexOrder) && (key <= iLastVertexOrder))
            {
                bool bIsExpandable = m_boostGraphWrapper.getVertexExpandable(vVertex,gGraph);
                if(bIsExpandable == true)
                {
                    // Do not push vertices in the map
                }
                else
                {
                    mapMinMaxOrderVertex.insert(key,vVertex);
                }
            }
        }
        else
        {
            if((key > iMinOrder) && (key < iMaxOrder))
            {
                if(vVertex == gGraph.global_to_local(vVertex))
                {
                    //cout<<"Own Vertices"<<endl;
                    bool bIsExpandable = m_boostGraphWrapper.getVertexExpandable(vVertex,gGraph);
                    if(bIsExpandable == true)
                    {
                        // Do not push vertices in the map
                    }
                    else
                    {
                        mapMinMaxOrderVertex.insert(key,vVertex);
                    }
                }
            }
        }
    }
}

void SpaceUtilizer::processSpaceUtilizer(SubGraph &gGraph)
{
    LAYOUT_ASSERT(&gGraph != NULL,LayoutException(__FUNCTION__
                                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                  , "gGraph"
                                                  , ""));


    // Get Min - Max Vertices Order.
    vector<int> vecMinMaxVerticesOrder;
    getSubgraphMinMaxVerticesOrder(gGraph, vecMinMaxVerticesOrder);
    int iFirstItem = vecMinMaxVerticesOrder[0];
    vecMinMaxVerticesOrder.push_back(iFirstItem);

    // Process Min - Max for getting vertices list.
    vector<int>::iterator iterMinMaxList = vecMinMaxVerticesOrder.begin();
    iterMinMaxList++;
    int iFirstOrder;
    int iSecondOrder;

    // Get Intersecting coordinates of circles and calculate list of angle.
    vector<int> vecIntersectionPoints;
    getCircleIntersection(gGraph,vecIntersectionPoints);

    // prepare list of intersecting angles of all clusters present at first level in the subgraph
    vector<double> vecIntersectingAngles;
    for(vector<int>::iterator iterPoints = vecIntersectionPoints.begin();
        iterPoints != vecIntersectionPoints.end();
        iterPoints++)
    {
        int iCoordX = *iterPoints;
        iterPoints++;
        int iCoordY = *iterPoints;
        double dAngle = getIntersectionPointAngle(iCoordX, iCoordY, gGraph);
        vecIntersectingAngles.push_back(dAngle);
    }

    double dFistAngle = vecIntersectingAngles[0];
    vecIntersectingAngles.push_back(dFistAngle);
    vector<double>::iterator iterIntersectingAngles = vecIntersectingAngles.begin();
    iterIntersectingAngles++;
    double dStartAngle = 0.0;
    double dEndAngle = 0.0;

    while((iterMinMaxList != vecMinMaxVerticesOrder.end()) && (iterIntersectingAngles != vecIntersectingAngles.end()))
    {
        iFirstOrder = *iterMinMaxList;
        iterMinMaxList++;
        iSecondOrder = *iterMinMaxList;

        // Get Vertex List between Max - Min vertices.
        MapOrderVertex mapVerticesOrderList;
        claculateVerticesBetweenMinMax(gGraph, iFirstOrder, iSecondOrder,mapVerticesOrderList);

        dStartAngle = *iterIntersectingAngles;
        iterIntersectingAngles++;
        dEndAngle = *iterIntersectingAngles;

        // Get Radius for the subgraph.
        double dRadius = m_boostGraphWrapper.getGraphRadius(gGraph);

        // Apply arc layout to vertices between Max - Min of subgraphs.
        CircleLayouter circleLayouter;
        double dDirectionAngle = dStartAngle;
        dStartAngle = dEndAngle;
        dEndAngle = dDirectionAngle;
        circleLayouter.applyArcLayout(gGraph, dStartAngle, dEndAngle, get(vertex_position, gGraph), dRadius, mapVerticesOrderList);

        // Set the arc laid out point coordinates in the vertices
        IteratorMapOrderVertex mapIter(mapVerticesOrderList);
        while(mapIter.hasNext())
        {
            mapIter.next();
            std::size_t key = mapIter.key();
            VertexDescriptor vVertex = mapVerticesOrderList[key];

            int iVertexX = (int)get(vertex_position, gGraph)[vVertex][0];
            int iVertexY = (int)get(vertex_position, gGraph)[vVertex][1];

            int iCenterX = m_boostGraphWrapper.getGraphCenterCoordX(gGraph);
            int iCenterY = m_boostGraphWrapper.getGraphCenterCoordY(gGraph);

            iVertexX += iCenterX;
            iVertexY += iCenterY;

            QString sVertexId = m_boostGraphWrapper.getVertexId(vVertex, gGraph);
            //cout<<"Arc Laid out Coordinates : "<<endl;

            std::cout<<sVertexId.toStdString()<<" : "<<vVertex<<" : "<<iVertexX;
            std::cout<<iVertexY<<std::endl;

            m_boostGraphWrapper.setVertexCenterCoordX(vVertex, gGraph, iVertexX);
            m_boostGraphWrapper.setVertexCenterCoordY(vVertex, gGraph, iVertexY);

        }

        iterMinMaxList++;
        iterIntersectingAngles++;
    }
}

void SpaceUtilizer::prepareGraphOwnVertexList(SubGraph &gSubgraph,
                                              QMap<size_t,
                                              VertexDescriptor>& mapOwnVerticesOrderToVertex)
{
    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__,LayoutExceptionEnum::NULL_POINTER_EXCEPTION,GRAPH));

    IteratorQVectorUInt iterOwnVertex , iterOwnVertexEnd;

    try{
    for(boost::tie(iterOwnVertex , iterOwnVertexEnd) = m_boostGraphWrapper.ownVerticesIter(gSubgraph);
        iterOwnVertex != iterOwnVertexEnd;
        iterOwnVertex++)
    {
        VertexDescriptor vCurrentVertex = *iterOwnVertex;
        LayoutEnum::NodeType nodeType = m_boostGraphWrapper.getVertexType(vCurrentVertex,gSubgraph);
        if(nodeType == LayoutEnum::DummyNode)
        {
            VertexDescriptor vVertex = gSubgraph.local_to_global(vCurrentVertex);
            SubGraph& gDummyGraph = (*(m_mapDummyVertexToSubgraph.value(vVertex)));
            int iOrder;
            try
            {
                iOrder = calculateMedianOfSubgraph(gDummyGraph);
            }
            catch(LayoutMemoryException& eException)
            {
                qDebug()<<"Memory Exception: in Calculate median of subgraph";
                iOrder = m_boostGraphWrapper.getVertexOrder(vVertex,gSubgraph);
            }
            catch(boost::exception& eException)
            {
                qDebug()<<"Boost Exception: in Calculate median of subgraph";
                iOrder = m_boostGraphWrapper.getVertexOrder(vVertex,gSubgraph);
            }
            catch(...)
            {
                qDebug()<<"Boost Exception: in Calculate median of subgraph";
                iOrder = m_boostGraphWrapper.getVertexOrder(vVertex,gSubgraph);
            }
            mapOwnVerticesOrderToVertex.insert(iOrder,vCurrentVertex);
        }
        else
        {
            int iOrder = m_boostGraphWrapper.getVertexOrder(vCurrentVertex, gSubgraph);
//            //cout<<"Own Vertex List : "<<vCurrentVertex<<endl;
            mapOwnVerticesOrderToVertex.insert(iOrder,vCurrentVertex);
        }
    }
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {}
}

void SpaceUtilizer::addSubgraphDummyVerticesAndMap(SubGraph &gSubgraph, VertexDescriptor vVertex)
{
    Q_ASSERT_X(&gSubgraph != NULL,
               "process Dummy Vertex",
               "Trying to process dummy vertex without passing a graph to method");

    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION,GRAPH));
    try{
        ChildrenIterator itrSubgraph, itrSubgraphEnd;
        for(boost::tie(itrSubgraph, itrSubgraphEnd) = gSubgraph.children();
            itrSubgraph != itrSubgraphEnd;
            itrSubgraph++)
        {
            // add dummy vertex for child subgraph in the parent subgraph
            VertexDescriptor vVertex = m_boostGraphWrapper.addDummyVertexForChildGraph(gSubgraph, *itrSubgraph);

            // add properties of subgraph to its dummy node
            // get properties from the subgraph which is a child subgraph
            int iDummyVertexCenterX = m_boostGraphWrapper.getGraphCenterCoordX(*itrSubgraph);
            int iDummyVertexCenterY = m_boostGraphWrapper.getGraphCenterCoordY(*itrSubgraph);
            int iDummyVertexLeftX = m_boostGraphWrapper.getGraphLeftTopCoordX(*itrSubgraph);
            int iDummyVertexTopY = m_boostGraphWrapper.getGraphLeftTopCoordY(*itrSubgraph);
            int iDummyVertexHeight = m_boostGraphWrapper.getGraphHeight(*itrSubgraph);
            int iDummyVertexWidth = m_boostGraphWrapper.getGraphWidth(*itrSubgraph);

            //set properties of subgraph into its dummy vertex of parent graph
            m_boostGraphWrapper.setVertexCenterCoordX(vVertex, gSubgraph, iDummyVertexCenterX);
            m_boostGraphWrapper.setVertexCenterCoordY(vVertex, gSubgraph, iDummyVertexCenterY);
            m_boostGraphWrapper.setVertexLeftCoordX(vVertex, gSubgraph, iDummyVertexLeftX);
            m_boostGraphWrapper.setVertexLeftCoordY(vVertex, gSubgraph,iDummyVertexTopY);
            m_boostGraphWrapper.setVertexHeight(vVertex, gSubgraph, iDummyVertexHeight);
            m_boostGraphWrapper.setVertexWidth(vVertex, gSubgraph, iDummyVertexWidth);

            // add global vertex to the map
            VertexDescriptor vGlobalVertex = gSubgraph.local_to_global(vVertex);
            m_mapDummyVertexToSubgraph.insert(vGlobalVertex, &(*itrSubgraph));

            int iVertexSubgraphCount = num_vertices(*itrSubgraph);
            gSubgraph.root()[vGlobalVertex].iVertexCount = iVertexSubgraphCount;

            addSubgraphDummyVerticesAndMap(*itrSubgraph, vVertex);
        }
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
}

int SpaceUtilizer::getVertexCountForDummyNode(VertexDescriptor &vDummyVertex)
{
    SubGraph& gSubgraph = (*m_mapDummyVertexToSubgraph.value(vDummyVertex));
    int iNumVertices = num_vertices(gSubgraph);
    return iNumVertices;
}

int SpaceUtilizer::calculateMedianOfSubgraph(SubGraph &gSubgraph)
{
    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));

    int iVertexOrder;
    size_t iOrder;
    try
    {
    // create a vertex order map.
    PGL_MAP_VERTEX_ORDER(mapVertexOrder,gSubgraph);

    // get map of order to vertices of the graph
    MapOrderVertex mapOrderVertex;
    mapOrderVertex = m_boostGraphWrapper.getMapOrderedVertices(gSubgraph, mapVertexOrder);

    // count total number of elements in the order to vertex map
    int iTotalMapElements = mapOrderVertex.count();
    // get the middle element's order from the map.
    //
    int iMedianElementOffset;
    iMedianElementOffset = std::floor((double)(iTotalMapElements + 1) / 2);

    IteratorMapOrderVertex mapIterOrderVertex(mapOrderVertex);

    int iCount = 1;

    VertexDescriptor vVertex;
    while(mapIterOrderVertex.hasNext())
    {
        mapIterOrderVertex.next();
        if(iCount <= iMedianElementOffset)
        {
            iOrder = mapIterOrderVertex.key();
            vVertex = mapIterOrderVertex.value();
            iCount++;
        }
    }

    iVertexOrder= m_boostGraphWrapper.getVertexOrder(vVertex, gSubgraph);
    }catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {
        throw;
    }
//    //cout<<"Median Order : "<<iOrder<<" "<<iVertexOrder<<endl;
    return iVertexOrder;
}

void SpaceUtilizer::processSecondPassCircularLayout(SubGraph &gSubgraph, int iCenterCoordX, int iCenterCoordY)
{

    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, GRAPH));

    // get the list of own vertices of graph
    MapOrderVertex mapKeyToOwnVertices;

    // prepare own vertex list
    try
    {
        prepareGraphOwnVertexList(gSubgraph, mapKeyToOwnVertices);
    }
    catch(LayoutMemoryException& eException)
    {
        throw LayoutMemoryException(__FUNCTION__,
                              LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                              eException.getObjectName());
    }
    catch(...)
    {
        throw;
    }


    // Calculate radius for this graph
    double dRadius = m_boostGraphWrapper.getGraphRadius(gSubgraph);

    // call second pass circular and expand dummy vertices and apply circular to it
    CircleLayouter circleLayouter;
    try
    {
        circleLayouter.applyCircleGraphLayout(gSubgraph, (get(vertex_position,gSubgraph)), dRadius,
                                              iCenterCoordX, iCenterCoordY, mapKeyToOwnVertices);
    }
    catch(LayoutMemoryException& eException)
    {
        throw LayoutMemoryException(__FUNCTION__,
                              LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                              eException.getObjectName());
    }
    catch(LayoutException& eException)
    {
        if(eException.getExceptionSubType() == LayoutExceptionEnum::INVALID_PARAMETER)
        {
            throw LayoutException(__FUNCTION__,
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  eException.getEntityValue(),
                                  eException.getEntityType());
        }
        else if(eException.getExceptionSubType() == LayoutExceptionEnum::EMPTY_CONTAINER)
        {
            throw LayoutException(__FUNCTION__,
                                  LayoutExceptionEnum::EMPTY_CONTAINER,
                                  eException.getEntityValue(),
                                  eException.getEntityType());
        }
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {

    }
    //add nesting edges
    IteratorQVectorUInt iterOwnVertex ,iterOwnVertexEnd;

    for(boost::tie(iterOwnVertex , iterOwnVertexEnd) = m_boostGraphWrapper.ownVerticesIter(gSubgraph);
        iterOwnVertex != iterOwnVertexEnd;
        iterOwnVertex++)
    {
        VertexDescriptor vCurrentVertex = *iterOwnVertex;
        LayoutEnum::NodeType nodeType = m_boostGraphWrapper.getVertexType(vCurrentVertex,gSubgraph);
        if(nodeType == LayoutEnum::DummyNode)
        {
            VertexDescriptor vVertex;
            try
            {
                vVertex = gSubgraph.local_to_global(vCurrentVertex);
            }
            catch(boost::exception& eBoostException)
            {
                throw *boost::get_error_info<errmsg_info>(eBoostException);
            }
            try
            {
            SubGraph& gDummyGraph = (*(m_mapDummyVertexToSubgraph.value(vVertex)));

//            //cout<<"Dummy Graph In 2nd Pass"<<endl;
//            m_boostGraphWrapper.printGraph(gDummyGraph);

            // calculate the cenroid of this graph and calculate its ceneter
            int iCentroidX = m_boostGraphWrapper.getVertexCenterCoordX(vCurrentVertex, gDummyGraph.parent());
            int iCentroidY = m_boostGraphWrapper.getVertexCenterCoordY(vCurrentVertex, gDummyGraph.parent());

            int iDistanceX = m_boostGraphWrapper.getGraphDistanceBetweenCentoidAndCenterCoordX(gDummyGraph);
            int iDistanceY = m_boostGraphWrapper.getGraphDistanceBetweenCentoidAndCenterCoordY(gDummyGraph);
            int iCenterDummyGraphVertexX = iCentroidX - iDistanceX;
            int iCenterDummyGraphVertexY = iCentroidY - iDistanceY;

            //set center of graph
            m_boostGraphWrapper.setGraphCenterCoordX(iCenterDummyGraphVertexX,gDummyGraph);
            m_boostGraphWrapper.setGraphCenterCoordY(iCenterDummyGraphVertexY,gDummyGraph);

            // Recursive call to second circle layout pass upto deep level
            processSecondPassCircularLayout(gDummyGraph, iCenterDummyGraphVertexX, iCenterDummyGraphVertexY);
            }
            catch(LayoutMemoryException& eException)
            {
                throw LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION,eException.getObjectName());
            }
            catch(boost::exception& eBoostException)
            {
                throw *boost::get_error_info<errmsg_info>(eBoostException);
            }
            catch(...)
            {}
        }
    }
}
