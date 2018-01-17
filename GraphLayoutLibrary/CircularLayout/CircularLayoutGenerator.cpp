#include "CircularLayoutGenerator.h"
#include "Common/LayoutEnum.h"

CircularLayoutGenerator::CircularLayoutGenerator()
{
    m_iCenterX = 0;
    m_iCenterY = 0;            
}

void CircularLayoutGenerator::applyCircularLayout(SubGraph& gInputGraph, LayoutEnum::VertexOrderCriteria enVertexOrder)
{
//    enVertexOrder = LayoutEnum::VertexOrderCriteria::DefaultOrder;
//    qDebug("Circular layout applied");

    bool isSubgraph = true; // root will always be a subgraph

    LAYOUT_ASSERT(&gInputGraph != NULL, LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, NULL_GRAPH_FOUND));

    LAYOUT_ASSERT((LayoutEnum::isValidVertexOrderingCriteria(enVertexOrder)) == true,
                  LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_TYPE, TYPE_ENUM_VERTEXORDER_TYPE, ORDERING_CRITERIA));

    // construct container of subgraph lists in the vector
    m_vecSubgraphContainer.push_back(&gInputGraph);

    try
    {
        // iterate the subgraph hierarchy and get the list of subgraphs in the container
        m_vecBFSOrderedSubgraphs.push_back(&gInputGraph);
        iterateChildrenGraphs(m_vecBFSOrderedSubgraphs);
    }
    catch(LayoutException& eException)
    {
        LayoutException(__FUNCTION__, LayoutExceptionEnum::EMPTY_CONTAINER, eException.getEntityValue());
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }

    // There should be atleast one subgraph, if there is none, throw an exception so that caller functions can take appropriate decision.
    LAYOUT_ASSERT(!(m_vecSubgraphContainer.empty()),
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  NULL_SUBGRAPH_CONTAINER_FOUND));

    // preprocessing for the circular layout
    for(vector<SubGraph*>::iterator itrSubgraph = m_vecSubgraphContainer.begin();
        itrSubgraph != m_vecSubgraphContainer.end();
        ++itrSubgraph)
    {
        // for empty graph add dummy node to that graph
        int iNumVertices = num_vertices(**itrSubgraph);
        if(iNumVertices == 0)
        {
            VertexDescriptor vVertex = m_boostGraphWrapper.addVertex(**itrSubgraph, LayoutEnum::InvisibleNode);
            // This dummy node will set the size of cluster
            // set height = 80 nd width = 80;
            int iDummyNodeHeight = 80;
            int iDummyNodeWidth = 80;
            m_boostGraphWrapper.setVertexHeight(vVertex,**itrSubgraph, iDummyNodeHeight);
            m_boostGraphWrapper.setVertexWidth(vVertex, **itrSubgraph, iDummyNodeWidth);
        }
    }

    // iterate the list of subgraphs from the container and apply circle layout
    for(vector<SubGraph*>::iterator itrSubgraph = m_vecSubgraphContainer.begin();
        itrSubgraph != m_vecSubgraphContainer.end();
        ++itrSubgraph)
    {
        // find center
        VertexIterPair vIterVerticesPair = vertices(**itrSubgraph);
        if(!isSubgraph)
        {
            // calculating middle vertex index for subgraph
            VertexDescriptor vMidVertexIndex;
            if((vIterVerticesPair.second - vIterVerticesPair.first) == 1)
            {
                // if single vertex is present in the subgraph then median should be that vertex only.
                vMidVertexIndex = vIterVerticesPair.first[0];
            }
            else
            {
                // for more than 1 vertex in the subgraph then median should be (diff + 1)/2.
                vMidVertexIndex = (vIterVerticesPair.second - vIterVerticesPair.first +1)/2;
            }

            // get the middle vertex's global index
            VertexDescriptor vGlobalMidVertex = (**itrSubgraph).local_to_global(vMidVertexIndex);

            // get the x and y coordinates of middle vertex as the center cooordinates of the graph
            m_iCenterX = m_boostGraphWrapper.getVertexCenterCoordX(vGlobalMidVertex ,gInputGraph);
            m_iCenterY = m_boostGraphWrapper.getVertexCenterCoordY(vGlobalMidVertex, gInputGraph);
        }
        else
        {
            isSubgraph = false;
        }
        // calculating radius for cluster using radius share factor
        // get total vertex present in the graph
        int iTotalVertex = num_vertices(**itrSubgraph);

        // get number of dummy vertices present in the graph
        int iDummyVertices = (**itrSubgraph).num_children();

        // calculate real vertices present in the graph
        int iVertexCount = iTotalVertex - iDummyVertices;

        // calculate the radius for real vertices considering the radius share factor
        double dRadiusUsingShare = ((iVertexCount) * RADIUS_SHARE);

        // calculate size considered radius from diagonal
        double dRadiusFromNodeDetails;       
        dRadiusFromNodeDetails = calculateRadius(**itrSubgraph);

        // add size considered radius and radius with radius share
        m_dRadius = dRadiusUsingShare + dRadiusFromNodeDetails;

        // Check the order specified by caller function
        MapOrderVertex mapOrderVertex;
        bool bIsValidVertexOrder = LayoutEnum::isValidVertexOrderingCriteria(enVertexOrder);
        if(bIsValidVertexOrder == true)
        {
            if(enVertexOrder == LayoutEnum::TopologicalOrder)
            {
                // topological Ordering Criteria
                VectorEdgeDescriptor vectBackEdges;
                GraphCycleHandler graphCycleHandler;
                graphCycleHandler.detectBackEdges(**itrSubgraph,vectBackEdges);
                graphCycleHandler.reverseEdges(**itrSubgraph,vectBackEdges);

                // Apply vertex ordering methods and get ordered map
                m_boostGraphWrapper.applyTopologicalVertexOrdering(**itrSubgraph,mapOrderVertex);
            }
            else if(enVertexOrder == LayoutEnum::ConnectedComponentOrder)
            {
                // connected Component ordering criteria
                m_boostGraphWrapper.applyConnectedComponent(**itrSubgraph,mapOrderVertex);
            }
            else
            {
                // default Ordering
                PGL_MAP_VERTEX_ORDER(mapVertexOrder,**itrSubgraph);
                mapOrderVertex = m_boostGraphWrapper.getMapOrderedVertices(**itrSubgraph
                                                                           ,mapVertexOrder);
            }
        }
        else
        {
            // if order is not valid then we are keeping it as the default vertex ordering
            PGL_MAP_VERTEX_ORDER(mapVertexOrder,**itrSubgraph);
            mapOrderVertex = m_boostGraphWrapper.getMapOrderedVertices(**itrSubgraph
                                                                       , mapVertexOrder);
        }

        // apply circle layout to this graph
        CircleLayouter circleLayouter;
        try
        {
            circleLayouter.applyCircleGraphLayout(**itrSubgraph, get(vertex_position,**itrSubgraph),
                                              m_dRadius, m_iCenterX, m_iCenterY, mapOrderVertex);
        }
        catch(boost::exception& eBoostException)
        {
            throw *boost::get_error_info<errmsg_info>(eBoostException);
        }

        // set the radius for this graph in its GraphProperty
        m_boostGraphWrapper.setGraphRadius(m_dRadius, (**itrSubgraph));

        // set center coordinates in the GraphProperty of this graph
        m_boostGraphWrapper.setGraphCenterCoordX(m_iCenterX, (**itrSubgraph));
        m_boostGraphWrapper.setGraphCenterCoordY(m_iCenterY, (**itrSubgraph));
    }

    // size Manager Functionality
    // calculate the subgraph size and set respective parameters in its properties
    m_sizeManager.processSizeManager(gInputGraph);

    // space utilizer functionality
    m_spaceUtilizer.addSubgraphDummyVerticesAndMap(gInputGraph, VERTEX_START_INDEX);

    // apply second pass of circle layout on the graph which will provide uniform space on the circumference of circle
    m_spaceUtilizer.processSecondPassCircularLayout(gInputGraph, CENTERCOORDINATEX_START, CENTERCOORDINATEY_START);

    // apply size manager functinality to update propertis with respect to second pass circle layout
    m_sizeManager.processSizeManager(gInputGraph);
}

void CircularLayoutGenerator::iterateChildrenGraphs(vector<SubGraph *> &subgraphQueue)
{
    /*
        we have used queue because it will contain reference of subgraphs.
        Adding all the subgraphs in queue to iterate one by one in circular way.
    */
    LAYOUT_ASSERT(!subgraphQueue.empty(),
                  LayoutException(__FUNCTION__,LayoutExceptionEnum::EMPTY_CONTAINER,VECTOR_CONTENTS));

    // define local queue which will contain the childrens of main graph
    vector<SubGraph*> subgraphSequence;

    try
    {
        // To iterate input queue which will contain graph reference
        for( vector<SubGraph*>::iterator itrSubgraphQueue = subgraphQueue.begin();
             itrSubgraphQueue != subgraphQueue.end();
             itrSubgraphQueue++)
        {
            // Finding the children upto deep level
            SubGraph::children_iterator itrSubgraph, itrSubgraphEnd;
            for (boost::tie(itrSubgraph, itrSubgraphEnd) = (**itrSubgraphQueue).children();
                 itrSubgraph != itrSubgraphEnd;
                 ++itrSubgraph)
            {
                // Add children in the global queue container
                m_vecSubgraphContainer.push_back(&(*itrSubgraph));

                // Add children in the local queue conatainer
                subgraphSequence.push_back(&(*itrSubgraph));
            }
        }
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {
        throw;
    }
    // To iterarte the local queue again if ant children is present
    if(!subgraphSequence.empty())
    {
        // Recursive call to iterate children
        iterateChildrenGraphs(subgraphSequence);
    }
}

double CircularLayoutGenerator::calculateRadius(SubGraph &gSubgraph)
{
    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));
    CircleLayouter circleLayouter;
    double dCircumference;
    try
    {
    dCircumference = circleLayouter.calculateCircumference(gSubgraph);

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
                              eException.getEntityValue(),eException.getEntityType());
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    LAYOUT_ASSERT(dCircumference > 0,
                  LayoutException(__FUNCTION__,
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  INVALID_CIRCUMFERENCE_VALUE));

    double dRadius = (dCircumference / (2 * PI));

    m_boostGraphWrapper.setGraphRadius(dRadius, gSubgraph);
    return dRadius;
}

void CircularLayoutGenerator::addDummyEdgesForTopologicalOrder(SubGraph &gSubgraph)
{
    /*
        This fumction adds the edges which will be treated as a dummy edges in the graph for getting
        the nodes for topological ordering in the consecutive mannar. Hence we will add edges between
        dummy subgraph nodes and nodes of that subgraph.
    */
    LAYOUT_ASSERT(&gSubgraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));

    ChildrenIterator itrSubgraph, itrSubgraphEnd;
    for(boost::tie(itrSubgraph, itrSubgraphEnd) = gSubgraph.children();
        itrSubgraph != itrSubgraphEnd;
        itrSubgraph++)
    {
        size_t iDummyNodeIndex;
        iDummyNodeIndex = m_boostGraphWrapper.getGraphDummyNodeIndex(*itrSubgraph);
        m_boostGraphWrapper.printGraph(*itrSubgraph);

        BGL_FORALL_VERTICES(vVertex, gSubgraph, SubGraph)
        {
            int iVertexIndex = m_boostGraphWrapper.getVertexIndex(vVertex);
            if((iVertexIndex == iDummyNodeIndex))
            {
                VertexDescriptor vGlobalDummyVertex = (*itrSubgraph).local_to_global(vVertex);
                BGL_FORALL_VERTICES(vSubVertex, *itrSubgraph, SubGraph)
                {
                    VertexDescriptor vGlobalSubVertex = (*itrSubgraph).local_to_global(vSubVertex);
                    if(vGlobalDummyVertex != vGlobalSubVertex)
                    {
                        m_boostGraphWrapper.addEdge(vGlobalDummyVertex, vGlobalSubVertex,gSubgraph);
                    }
                }
            }
        }
    }
}

