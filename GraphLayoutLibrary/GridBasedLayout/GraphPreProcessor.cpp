#include "GraphPreProcessor.h"

GraphPreProcessor::GraphPreProcessor()
{
}

void GraphPreProcessor::toMaximalPlanar(SubGraph & gInputGraph, EdgeOrderType &oDummyEdges)
{
    //CHANGE GRAPH FORMAT--------------------------------------------------------------------------------------
    //gInputGraph is a bidirectedS graph, whereas the algorithm to make it maximal planar accepts undirectedS graphs.
    //To map our graph to undirected one, we create a local undirected graph and copy the edges and vertices
    //in our graph to this local graph; and pass this to the algorithm.

    VertexDescriptor descVertexSource, descVertexTarget;
    EdgeIterator itrEdge, itrEdgeEnd;
    EdgeDescriptor descEdge;
    int iVertexSource, iVertexTarget, iNumVertices;
    BoostGraphWrapper graphWrapper;
    iNumVertices = boost::num_vertices(gInputGraph);
    GraphPreProcessor::graph g(iNumVertices); //create a local undirected graph with same no. of vertices
    LayoutUtility layoutUtility;

    // copy each edge one by one
    for(boost::tie(itrEdge, itrEdgeEnd) = edges(gInputGraph); itrEdge != itrEdgeEnd; itrEdge++)
    {
        descEdge = *itrEdge;
        descVertexSource = graphWrapper.getEdgeSource(descEdge, gInputGraph);
        descVertexTarget = graphWrapper.getEdgeTarget(descEdge, gInputGraph);

        iVertexSource = get(vertex_index, gInputGraph, descVertexSource);
        iVertexTarget = get(vertex_index, gInputGraph, descVertexTarget);

        add_edge(iVertexSource, iVertexTarget, g);
    }
    //Initialize the interior edge index
    layoutUtility.reinitializeEdgeIndices(g);

    //Test for planarity; compute the planar embedding as a side-effect
    typedef std::vector< graph_traits<graph>::edge_descriptor > vec_t;
    std::vector<vec_t> embedding(num_vertices(g));
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                     boyer_myrvold_params::embedding = &embedding[0]))
    {
        //std::cout << "Input graph is planar1" << std::endl;
    }
    else
    {
        throw LayoutException("toMaximalPlanar",
                              LayoutExceptionEnum::INVALID_GRAPH_TYPE,
                              "inputGraph",
                              "non-planar");
        //std::cout << "Input graph is not planar1" << std::endl;
    }
    //END------------------------------------------------------------------------------------------------------

    //MAKE MAXIMAL PLANAR--------------------------------------------------------------------------------------
    make_connected(g);
    // Re-initialize the edge index, since we just added a few edges
    layoutUtility.reinitializeEdgeIndices(g);

    //Test for planarity again; compute the planar embedding as a side-effect
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                     boyer_myrvold_params::embedding = &embedding[0]))
    {
        //std::cout << "After calling make_connected, the graph is still planar" << std::endl;
    }
    else
    {
        throw LayoutException("toMaximalPlanar",
                              LayoutExceptionEnum::INVALID_GRAPH_TYPE,
                              "connectedGraph",
                              "non-planar");
        //std::cout << "After calling make_connected, the graph is not planar" << std::endl;
    }

    make_biconnected_planar(g, &embedding[0]);
    // Re-initialize the edge index, since we just added a few edges
    layoutUtility.reinitializeEdgeIndices(g);

    //Test for planarity again; compute the planar embedding as a side-effect
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                     boyer_myrvold_params::embedding = &embedding[0]))
    {
          //std::cout << "After calling make_biconnected, the graph is still planar" << std::endl;
    }
    else
    {
        throw LayoutException("toMaximalPlanar",
                              LayoutExceptionEnum::INVALID_GRAPH_TYPE,
                              "biconnectedGraph",
                              "non-planar");
        //std::cout << "After calling make_biconnected, the graph is not planar" << std::endl;
    }

    make_maximal_planar(g, &embedding[0]);
    // Re-initialize the edge index, since we just added a few edges
    layoutUtility.reinitializeEdgeIndices(g);

    // Test for planarity one final time; compute the planar embedding as a side-effect
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                     boyer_myrvold_params::embedding = &embedding[0]))
    {
          //std::cout << "After calling make_maximal_planar, the final graph is planar." << std::endl;
    }
    else
    {
        throw LayoutException("toMaximalPlanar",
                              LayoutExceptionEnum::INVALID_GRAPH_TYPE,
                              "maxPlanarGraph",
                              "non-planar");
        //std::cout << "is not planar." << std::endl;
    }
    //END------------------------------------------------------------------------------------------------------

    //FIND EXTRA EDGES ADDED--------------------------------------------------------------------------------
    //add them to gInputGraph as well, and remember them as dummy edges
    graph_traits< graph >::edge_iterator itrUndirEdge, itrUndirEdgeEnd;
    graph_traits< graph >::edge_descriptor descUndirEdge;
    graph_traits< graph >::vertex_descriptor descUndirVertSource, descUndirVertTarget;
    bool bIsEdge;
    EdgeBoolPair pairEdgeBoolStoT, pairNewEdge;
    BoostGraphWrapper boostGraphWrapper;

    //for each edge in g:
    for(boost::tie(itrUndirEdge, itrUndirEdgeEnd) = edges(g); itrUndirEdge != itrUndirEdgeEnd; itrUndirEdge++)
    {
        //get edge descriptor for current edge (in g)
        descUndirEdge = *itrUndirEdge;
        //get its source and target vertices (in g)
        descUndirVertSource = source(descUndirEdge, g);
        descUndirVertTarget = target(descUndirEdge, g);
        //get indices of source and target (in g)
        iVertexSource = get(vertex_index, g, descUndirVertSource);
        iVertexTarget = get(vertex_index, g, descUndirVertTarget);

        //get corresponding vertices in gInputGraph
        descVertexSource = vertex(iVertexSource, gInputGraph);
        descVertexTarget = vertex(iVertexTarget, gInputGraph);

        //check if there's corresponding edge between these vertices in gInputGraph
        pairEdgeBoolStoT = lookup_edge(descVertexSource, descVertexTarget, gInputGraph);
        bIsEdge = pairEdgeBoolStoT.second;

        if(!bIsEdge)//if there isn't any edge, current edge is a dummy edge
        {
            //add an edge in gInputGraph
            pairNewEdge = boostGraphWrapper.addEdge(descVertexSource, descVertexTarget, gInputGraph);
            //remember the added edge
            LAYOUT_ASSERT(pairNewEdge.second,
                          LayoutException("toMaximalPlanar",
                                          LayoutExceptionEnum::INVALID_OPERATION,
                                          "add_edge",
                                          "toMaximalPlanar"));
            oDummyEdges.push_back(pairNewEdge.first);
        }

    }
    //Initialize the interior edge index in input graph
    layoutUtility.reinitializeEdgeIndices(gInputGraph);
    //END------------------------------------------------------------------------------------------------------
}

void GraphPreProcessor::removeDummyEdges(SubGraph & gInputGraph, EdgeOrderType &oDummyEdges)
{
    LayoutUtility layoutUtility;

    //for each DummyEdge
    for(int i = 0; i < oDummyEdges.size(); ++i)
    {
        //find corresponding edge in gInputGraph
        EdgeBoolPair dummyEdgeBool = lookup_edge(source(oDummyEdges[i], gInputGraph),
                                                 target(oDummyEdges[i], gInputGraph),
                                                 gInputGraph);
        //remove it from gInputGraph
        if(dummyEdgeBool.second)
        {
            remove_edge(source(dummyEdgeBool.first, gInputGraph),
                        target(dummyEdgeBool.first, gInputGraph),
                        gInputGraph);
        }
        else
        {
            throw LayoutException("removeDummyEdges",
                                  LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                  "graph",
                                  "dummy edge");
        }
    }
    //Initialize the interior edge index in input graph
    layoutUtility.reinitializeEdgeIndices(gInputGraph);
}

void GraphPreProcessor::removeParallelEdges(SubGraph &gInputGraph, EdgeOrderType &oParallelEdgeList, std::vector<QString> & oParrallelEdgeIDs)
{
    //std::vector<QString> oParrallelEdgeIDs;
    LayoutUtility layoutUtility;
    BoostGraphWrapper bGraphWrapper;

    int iNumVertices = boost::num_vertices(gInputGraph);
    GraphPreProcessor::graph gLocalGraph(iNumVertices);

    //for each edge in input graph
    EdgeIterator itrEdge, itrEdgeEnd;
    for(boost::tie(itrEdge, itrEdgeEnd) = edges(gInputGraph); itrEdge != itrEdgeEnd; ++itrEdge)
    {
        //get its endpoints in local graph
        graph_traits<graph>::vertex_descriptor vLocalSource, vLocalTarget;
        vLocalSource = vertex(get(vertex_index,
                                 gInputGraph,
                                 source(*itrEdge, gInputGraph)),
                             gLocalGraph);
        vLocalTarget = vertex(get(vertex_index,
                                 gInputGraph,
                                 target(*itrEdge, gInputGraph)),
                             gLocalGraph);

        //check if the edge already exists in local graph
        bool bIsInLocalG = (lookup_edge(vLocalSource, vLocalTarget, gLocalGraph).second
                            || lookup_edge(vLocalTarget, vLocalSource, gLocalGraph).second);
        if(!bIsInLocalG) //if doesnot exist in local graph
        {
            //add it to local graph
            LAYOUT_ASSERT(add_edge(vLocalSource, vLocalTarget, gLocalGraph).second,
                          LayoutException("removeParallelEdges",
                                          LayoutExceptionEnum::INVALID_OPERATION,
                                          "add_edge",
                                          "removeParallelEdges"));
            //re-initialize edge indices
            layoutUtility.reinitializeEdgeIndices(gLocalGraph);
        }
        else //if already exists in local graph
        {
            //remember it as a parallel edge
            EdgeDescriptor eParallelEdge = *itrEdge;
            oParallelEdgeList.push_back(eParallelEdge);
            QString sEdheID = bGraphWrapper.getEdgeId(eParallelEdge, gInputGraph);
            oParrallelEdgeIDs.push_back(sEdheID);
        }
    }

    //for each parallel edge
    for(int iEdgeIndex = 0; iEdgeIndex < oParallelEdgeList.size(); ++iEdgeIndex)
    {
        //remove it
        remove_edge(oParallelEdgeList[iEdgeIndex], gInputGraph);
    }
    //re-initiate edge indices
    layoutUtility.reinitializeEdgeIndices(gInputGraph);
}

void GraphPreProcessor::toPlanar(SubGraph & gInputGraph, VertexOrderType &oDummyVertexList,
                                 std::vector<GraphPreProcessor::EdgePairType> & correspondingCrossingEdgeList)
{
    int iNumVertices = boost::num_vertices(gInputGraph);
    int iNumEdges = boost::num_edges(gInputGraph);
    GraphPreProcessor::graph gLocalGraph(iNumVertices);
    std::vector< graph_traits< graph >::edge_descriptor > localCrossingEdgeList;

    //separate out the crossing edges from the graph
    this->findCrossingEdges(gInputGraph, gLocalGraph, localCrossingEdgeList);

    int iNumLocalEdges = boost::num_edges(gLocalGraph);
    LAYOUT_ASSERT((localCrossingEdgeList.size() + iNumLocalEdges) == iNumEdges,
                  LayoutException("toPlanar",
                                  LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                  "localCrossingEdgeList",
                                  "gLocalGraph"));

    //replace each crossing by a dummy vertex
    this->replaceCrossingsByDummyVertices(gInputGraph, gLocalGraph, localCrossingEdgeList,
                                          oDummyVertexList, correspondingCrossingEdgeList);

    LAYOUT_ASSERT(oDummyVertexList.size() == correspondingCrossingEdgeList.size(),
                  LayoutException("toPlanar",
                                  LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                  "DummyVertices",
                                  "CorrespondingEdges"));
}

void GraphPreProcessor::findCrossingEdges(SubGraph & gInputGraph, GraphPreProcessor::graph & gLocalGraph,
                                          std::vector< graph_traits< graph >::edge_descriptor > & localCrossingEdgeList)
{
    LayoutUtility layoutUtility;

    //for each edge in input graph
    EdgeIterator itrEdge, itrEdgeEnd;
    for(boost::tie(itrEdge, itrEdgeEnd) = edges(gInputGraph); itrEdge != itrEdgeEnd; ++itrEdge)
    {
        graph_traits<graph>::vertex_descriptor vLocalSource, vLocalTarget;
        vLocalSource = vertex(get(vertex_index, gInputGraph, source(*itrEdge, gInputGraph)),
                             gLocalGraph);
        vLocalTarget = vertex(get(vertex_index, gInputGraph, target(*itrEdge, gInputGraph)),
                             gLocalGraph);

        //add it to local graph
        add_edge(vLocalSource, vLocalTarget, gLocalGraph);
        //re-initialize edge indices
        layoutUtility.reinitializeEdgeIndices(gLocalGraph);

        //check planarity
        typedef std::vector< graph_traits<graph>::edge_descriptor > vec_t;
        std::vector<vec_t> embedding(num_vertices(gLocalGraph));
        bool bIsPlanar = boyer_myrvold_planarity_test(boyer_myrvold_params::graph = gLocalGraph,
                                                      boyer_myrvold_params::embedding = &embedding[0]);

        if(!bIsPlanar) //if graph looses planarity due to added edge
        {
            LAYOUT_ASSERT(lookup_edge(vLocalSource, vLocalTarget, gLocalGraph).second,
                          LayoutException("findCrossingEdges",
                                          LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                          "LocalGraph",
                                          "crossing edge"));
            //remember it as crossing edge
            graph_traits<graph>::edge_descriptor eLocalCrossingEdge;
            eLocalCrossingEdge = lookup_edge(vLocalSource, vLocalTarget, gLocalGraph).first;
            localCrossingEdgeList.push_back(eLocalCrossingEdge);

            //remove it
            remove_edge(vLocalSource, vLocalTarget, gLocalGraph);
            //re-initialize edge indices
            layoutUtility.reinitializeEdgeIndices(gLocalGraph);
        }
        //else if planar, continue adding edges to local graph
    }
}

void GraphPreProcessor::replaceCrossingsByDummyVertices(SubGraph & gInputGraph, GraphPreProcessor::graph & gLocalGraph,
                                                        std::vector< graph_traits< graph >::edge_descriptor > & localCrossingEdgeList,
                                                        VertexOrderType &oDummyVertexList,
                                                        std::vector<GraphPreProcessor::EdgePairType> & correspondingCrossingEdgeList)
{
    LayoutUtility layoutUtility;

    //for each crossing edge
    for(int iCrossingEdgeIndex = 0; iCrossingEdgeIndex < localCrossingEdgeList.size(); ++iCrossingEdgeIndex)
    {
        //get its src and tgt and their indices in local graph
        graph_traits<SubGraph>::vertex_descriptor vSourceOfLocalCrossingEdge, vTargetOfLocalCrossingEdge;
        vSourceOfLocalCrossingEdge = source(localCrossingEdgeList[iCrossingEdgeIndex], gLocalGraph);
        vTargetOfLocalCrossingEdge = target(localCrossingEdgeList[iCrossingEdgeIndex], gLocalGraph);
        int iIndexOfSourceOfCrossingEdge = get(vertex_index, gLocalGraph, vSourceOfLocalCrossingEdge);
        int iIndexOfTargetOfCrossingEdge = get(vertex_index, gLocalGraph, vTargetOfLocalCrossingEdge);

        //get corresponding src and tgt in input graph
        VertexDescriptor vSourceOfCrossingEdge, vTargetOfCrossingEdge;
        vSourceOfCrossingEdge = vertex(iIndexOfSourceOfCrossingEdge, gInputGraph);
        vTargetOfCrossingEdge = vertex(iIndexOfTargetOfCrossingEdge, gInputGraph);

        //find augmented dual graph of local graph------------------------------------------------------------
        GraphPreProcessor::graph gDualGraph;
        UndirEdgeOrderType dualEdge(boost::num_edges(gLocalGraph));
        std::vector< graph_traits<GraphPreProcessor::graph>::vertex_descriptor > oAugmentedVertices(2);
        std::vector< int > oIndicesOfEndsOfCrossingEdge(2);

        oIndicesOfEndsOfCrossingEdge[0] = iIndexOfSourceOfCrossingEdge;
        oIndicesOfEndsOfCrossingEdge[1] = iIndexOfTargetOfCrossingEdge;

        this->createAugmentedDualGraph(gLocalGraph, gDualGraph, dualEdge,
                                       oIndicesOfEndsOfCrossingEdge,
                                       oAugmentedVertices);

        graph_traits<GraphPreProcessor::graph>::vertex_descriptor vAugmentedSource, vAugmentedTarget;
        vAugmentedSource = oAugmentedVertices[0];
        vAugmentedTarget = oAugmentedVertices[1];
        //----------------------------------------------------------------------------------------------------

        //find shortest path----------------------------------------------------------------------------------
        GraphPreProcessor::dijkstra_graph gDijkstraGraph(num_vertices(gDualGraph));
        std::vector<GraphPreProcessor::dijkstra_vertex_descriptor> path;

        this->findShortestPath(gDualGraph, gDijkstraGraph, vAugmentedSource, vAugmentedTarget, path);
        //---------------------------------------------------------------------------------------------------

        //Replace edge crossings along the shortest path by dummy vertices-----------------------------------
        graph_traits<graph>::vertex_descriptor vRecentlyAddedLocalVertex = vSourceOfLocalCrossingEdge;
        VertexDescriptor vRecentlyAddedVertex = vSourceOfCrossingEdge;

        LAYOUT_ASSERT(path.size() > 3,
                      LayoutException("replaceCrossingsByDummyVertices",
                                      LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                      "shortest path",
                                      "in dual graph"));
        //for each dual edge in the shortest path
        for(int iPathIndex = 1; iPathIndex <= path.size() - 3; ++iPathIndex)
        {
            //this dual edge ( path[iPathIndex]-path[iPathIndex+1] ) represents a crossing
            //find edge in local graph corresponding to this dual edge (call it 'vertical edge')-------------
            graph_traits<graph>::vertex_descriptor vSourceOfDualEdge, vTargetOfDualEdge;

            //find vertices in (aug)dual draph, corresponding to path[i] and path[i+1] in gDijkstraGraph
            int iSourceIndexOfDualEdgeInPath = get(vertex_index, gDijkstraGraph, path[iPathIndex]);
            vSourceOfDualEdge = vertex(iSourceIndexOfDualEdgeInPath, gDualGraph);
            int iTargetIndexOfDualEdgeInPath = get(vertex_index, gDijkstraGraph, path[iPathIndex + 1]);
            vTargetOfDualEdge = vertex(iTargetIndexOfDualEdgeInPath, gDualGraph);

            //find edge between these vertices in (aug)dual graph
            graph_traits<graph>::edge_descriptor eDualEdge;
            if(lookup_edge(vSourceOfDualEdge, vTargetOfDualEdge, gDualGraph).second)
                eDualEdge = lookup_edge(vSourceOfDualEdge, vTargetOfDualEdge, gDualGraph).first;
            else if(lookup_edge(vTargetOfDualEdge, vSourceOfDualEdge, gDualGraph).second)
                eDualEdge = lookup_edge(vTargetOfDualEdge, vSourceOfDualEdge, gDualGraph).first;

            //find its corresponding edge in local graph
            graph_traits<graph>::edge_descriptor eLocalVerticalEdge;
            int iDualEdgeIndex = get(edge_index, gDualGraph, eDualEdge);
            LAYOUT_ASSERT(iDualEdgeIndex < dualEdge.size(),
                          LayoutException("replaceCrossingsByDummyVertices",
                                          LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                          "dualEdge",
                                          "index out of bound"));
            eLocalVerticalEdge = dualEdge[iDualEdgeIndex];

            //find vertical edge's endpoints in local graph
            graph_traits<graph>::vertex_descriptor vSourceOfLocalVerticalEdge, vTargetOfLocalVerticalEdge;
            vSourceOfLocalVerticalEdge = source(eLocalVerticalEdge, gLocalGraph);
            vTargetOfLocalVerticalEdge = target(eLocalVerticalEdge, gLocalGraph);

            //find vertical edge's endpoints in input graph
            VertexDescriptor vSourceOfVerticalEdge, vTargetOfVerticalEdge;
            int iIndexOfSourceOfLocalVerticalEdge = get(vertex_index, gLocalGraph, vSourceOfLocalVerticalEdge);
            vSourceOfVerticalEdge = vertex(iIndexOfSourceOfLocalVerticalEdge, gInputGraph);
            int iIndexOfTargetOfLocalVerticalEdge = get(vertex_index, gLocalGraph, vTargetOfLocalVerticalEdge);
            vTargetOfVerticalEdge = vertex(iIndexOfTargetOfLocalVerticalEdge, gInputGraph);
            //-----------------------------------------------------------------------------------------------

            //Replace the crossing edges by dummy vertex-----------------------------------------------------
            //IN INPUT GRAPH:
            //add new (dummy) vertex
            VertexDescriptor vNewVertex = add_vertex(gInputGraph);
            oDummyVertexList.push_back(vNewVertex);
            //join new vertex to ends of vertical edge
            add_edge(vSourceOfVerticalEdge, vNewVertex, gInputGraph);
            add_edge(vNewVertex, vTargetOfVerticalEdge, gInputGraph);
            //remove vertical edge
            remove_edge(vSourceOfVerticalEdge, vTargetOfVerticalEdge, gInputGraph);
            remove_edge(vTargetOfVerticalEdge, vSourceOfVerticalEdge, gInputGraph);
            //join recent vertex to new vertex
            add_edge(vRecentlyAddedVertex, vNewVertex, gInputGraph);

            //remember crossing which is being replaced by vertex
            GraphPreProcessor::EdgePairType crossingEdgePair (2, std::vector<VertexDescriptor>(2));
            crossingEdgePair[0][0] = vSourceOfCrossingEdge;
            crossingEdgePair[0][1] = vTargetOfCrossingEdge;
            crossingEdgePair[1][0] = vSourceOfVerticalEdge;
            crossingEdgePair[1][1] = vTargetOfVerticalEdge;
            correspondingCrossingEdgeList.push_back(crossingEdgePair);
            //new vertex becomes recent vertex
            vRecentlyAddedVertex = vNewVertex;

            //IN LOCAL GRAPH:
            //add new vert
            graph_traits<graph>::vertex_descriptor vNewLocalVertex = add_vertex(gLocalGraph);
            //join new vertex to ends of vertical edge
            add_edge(vSourceOfLocalVerticalEdge, vNewLocalVertex, gLocalGraph);
            add_edge(vNewLocalVertex, vTargetOfLocalVerticalEdge, gLocalGraph);
            //remove vertical edge
            remove_edge(vSourceOfLocalVerticalEdge, vTargetOfLocalVerticalEdge, gLocalGraph);
            remove_edge(vTargetOfLocalVerticalEdge, vSourceOfLocalVerticalEdge, gLocalGraph);
            //join recent vertex to new vertex
            add_edge(vRecentlyAddedLocalVertex, vNewLocalVertex, gLocalGraph);
            //new vertex becomes recent vertex
            vRecentlyAddedLocalVertex = vNewLocalVertex;
            //-----------------------------------------------------------------------------------------------
        }

        //connect the last dummy vertex----------------------------------------------------------------------
        //IN INPUT GRAPH:
        //join recent and target of horizontal edge
        add_edge(vRecentlyAddedVertex, vTargetOfCrossingEdge, gInputGraph);
        //remove crossing (horizontal) edge
        remove_edge(vSourceOfCrossingEdge, vTargetOfCrossingEdge, gInputGraph);
        remove_edge(vTargetOfCrossingEdge, vSourceOfCrossingEdge, gInputGraph);
        //re-initiate edge indices
        layoutUtility.reinitializeEdgeIndices(gInputGraph);

        //IN LOCAL GRAPH:
        //join recent and target of horizontal edge
        add_edge(vRecentlyAddedLocalVertex, vTargetOfLocalCrossingEdge, gLocalGraph);
        //re-initialize edge indices
        layoutUtility.reinitializeEdgeIndices(gLocalGraph);
        //---------------------------------------------------------------------------------------------------

        //---------------------------------------------------------------------------------------------------
    }
}

void GraphPreProcessor::createAugmentedDualGraph(GraphPreProcessor::graph & gLocalGraph,
                                                 GraphPreProcessor::graph & gDualGraph,
                                                 UndirEdgeOrderType & dualEdge,
                                                 std::vector< int > & oIndicesOfEndsOfCrossingEdge,
                                                 std::vector< graph_traits<GraphPreProcessor::graph>::vertex_descriptor > & oAugmentedVertices)
{
    LayoutUtility layoutUtility;

    //find dual graph of local graph-------------------------------------------------------------------
    std::vector< UndirVertexOrderType > adjecentFace(boost::num_vertices(gLocalGraph));

    // Compute the planar embedding - we know the input graph is planar,
    // so we're ignoring the return value of the test
    typedef std::vector< graph_traits<graph>::edge_descriptor > vec_t;
    std::vector<vec_t> embedding(num_vertices(gLocalGraph));
    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = gLocalGraph,
                                 boyer_myrvold_params::embedding = &embedding[0]);

    create_dual_graph(gLocalGraph, gDualGraph, &embedding[0], adjecentFace, dualEdge);
    //-------------------------------------------------------------------------------------------------

    //find augmented dual graph------------------------------------------------------------------------
    //add source and target of the crossing-edge to dual graph
    oAugmentedVertices[0] = add_vertex(gDualGraph);
    oAugmentedVertices[1] = add_vertex(gDualGraph);

    //connect these src and tgt to their adjecent faces
    //for all of source's adjecent faces
    LAYOUT_ASSERT(oIndicesOfEndsOfCrossingEdge[0] < adjecentFace.size(),
                  LayoutException("createAugmentedDualGraph",
                                  LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE,
                                  "adjecentFace",
                                  "index out of Bound")); //adjecentFace[oIndicesOfEndsOfCrossingEdge[0]] not Null
    LAYOUT_ASSERT(adjecentFace[oIndicesOfEndsOfCrossingEdge[0]].size() > 0,
                  LayoutException("createAugmentedDualGraph",
                                  LayoutExceptionEnum::EMPTY_CONTAINER,
                                  "adjecentFace[source]",
                                  "adjecentFace of aug.source")); //adjecentFace[oIndicesOfEndsOfCrossingEdge[0]].size() > 0
    for(int iFaceIndex = 0; iFaceIndex < adjecentFace[oIndicesOfEndsOfCrossingEdge[0]].size(); ++iFaceIndex)
    {
        //if src not already connected to adjecent face
        if(!(lookup_edge(oAugmentedVertices[0], adjecentFace[oIndicesOfEndsOfCrossingEdge[0]][iFaceIndex], gDualGraph).second
             || lookup_edge(adjecentFace[oIndicesOfEndsOfCrossingEdge[0]][iFaceIndex], oAugmentedVertices[0], gDualGraph).second))
        {
            //add edge to dual graph
            add_edge(oAugmentedVertices[0], adjecentFace[oIndicesOfEndsOfCrossingEdge[0]][iFaceIndex], gDualGraph);
        }
        //else continue
    }
    //for all of target's adjecent faces
    LAYOUT_ASSERT(oIndicesOfEndsOfCrossingEdge[1] < adjecentFace.size(),
                  LayoutException("createAugmentedDualGraph",
                                  LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE,
                                  "adjecentFace",
                                  "index out of Bound")); //adjecentFace[oIndicesOfEndsOfCrossingEdge[1]] not Null
    LAYOUT_ASSERT(adjecentFace[oIndicesOfEndsOfCrossingEdge[1]].size() > 0,
                  LayoutException("createAugmentedDualGraph",
                                  LayoutExceptionEnum::EMPTY_CONTAINER,
                                  "adjecentFace[target]",
                                  "adjecentFace of aug.target")); //adjecentFace[oIndicesOfEndsOfCrossingEdge[1]].size() > 0
    for(int iFaceIndex = 0; iFaceIndex < adjecentFace[oIndicesOfEndsOfCrossingEdge[1]].size(); ++iFaceIndex)
    {
        //if tgt not already connected to adjecent face
        if(!(lookup_edge(oAugmentedVertices[1], adjecentFace[oIndicesOfEndsOfCrossingEdge[1]][iFaceIndex], gDualGraph).second
             || lookup_edge(adjecentFace[oIndicesOfEndsOfCrossingEdge[1]][iFaceIndex], oAugmentedVertices[1], gDualGraph).second))
        {
            //add edge to dual graph
            add_edge(oAugmentedVertices[1], adjecentFace[oIndicesOfEndsOfCrossingEdge[1]][iFaceIndex], gDualGraph);
        }
        //else continue
    }

    //re-initialize edge indices
    layoutUtility.reinitializeEdgeIndices(gDualGraph);
    //---------------------------------------------------------------------------------------------------
}

void GraphPreProcessor::findShortestPath(GraphPreProcessor::graph & gDualGraph,
                                         GraphPreProcessor::dijkstra_graph & gDijkstraGraph,
                                         graph_traits<GraphPreProcessor::graph>::vertex_descriptor & vAugmentedSource,
                                         graph_traits<GraphPreProcessor::graph>::vertex_descriptor & vAugmentedTarget,
                                         std::vector<GraphPreProcessor::dijkstra_vertex_descriptor> & path)
{
    LayoutUtility layoutUtility;

    //create a copy of the augmented dual graph with edge weights set to '1'------------------------------------
    //for each edge in (augmented)dual graph
    graph_traits< GraphPreProcessor::graph >::edge_iterator itrDualGraphEdge, itrDualGraphEdgeEnd;
    for(boost::tie(itrDualGraphEdge, itrDualGraphEdgeEnd) = edges(gDualGraph); itrDualGraphEdge != itrDualGraphEdgeEnd; ++itrDualGraphEdge)
    {
        //get src and tgt of the edge
        graph_traits< GraphPreProcessor::graph >::vertex_descriptor vSourceOfDualEdge, vTargetOfDualEdge;
        vSourceOfDualEdge = source(*itrDualGraphEdge, gDualGraph);
        vTargetOfDualEdge = target(*itrDualGraphEdge, gDualGraph);
        int iIndexOfSourceOfDualEdge = get(vertex_index, gDualGraph, vSourceOfDualEdge);
        int iIndexOfTargetOfDualEdge = get(vertex_index, gDualGraph, vTargetOfDualEdge);

        //get corresponding src and tgt in dijkstra graph
        graph_traits< GraphPreProcessor::dijkstra_graph >::vertex_descriptor vSourceOfDualEdgeInDijkstra, vTargetOfDualEdgeInDijkstra;
        vSourceOfDualEdgeInDijkstra = vertex(iIndexOfSourceOfDualEdge, gDijkstraGraph);
        vTargetOfDualEdgeInDijkstra = vertex(iIndexOfTargetOfDualEdge, gDijkstraGraph);

        //add edge in dijkstra graph
        add_edge(vSourceOfDualEdgeInDijkstra, vTargetOfDualEdgeInDijkstra, 1, gDijkstraGraph);
    }
    //Initialize the interior edge index
    layoutUtility.reinitializeEdgeIndices(gDijkstraGraph);
    //----------------------------------------------------------------------------------------------------------

    //find vertices in gDijkstraGraph, corresponding to vAugmentedSource and vAugmentedTarget
    GraphPreProcessor::dijkstra_vertex_descriptor vDijkstraSource, vDijkstraTarget;
    int iIndexOfAugmentedSource = get(vertex_index, gDualGraph, vAugmentedSource);
    vDijkstraSource = vertex(iIndexOfAugmentedSource, gDijkstraGraph);
    int iIndexOfAugmentedTarget = get(vertex_index, gDualGraph, vAugmentedTarget);
    vDijkstraTarget = vertex(iIndexOfAugmentedTarget, gDijkstraGraph);

    //call Dijkstra's single source shortest path algorithm-----------------------------------------------------
    //it will calculate distances and preceding vertex of each vertex, in the shortest path from the source
    std::vector<GraphPreProcessor::dijkstra_vertex_descriptor> p(num_vertices(gDijkstraGraph)); //to maintatin parent of each vertex in Dijkstra traversal
    std::vector<int> d(num_vertices(gDijkstraGraph)); //to maintatin distance of each vertex from source in Dijkstra traversal
    dijkstra_shortest_paths(gDijkstraGraph,
                            vDijkstraSource,
                            predecessor_map(boost::make_iterator_property_map(p.begin(),
                                                                              get(boost::vertex_index,
                                                                                  gDijkstraGraph))).distance_map(boost::make_iterator_property_map(d.begin(),
                                                                                                                                                   get(boost::vertex_index,
                                                                                                                                                       gDijkstraGraph))));
    //find path from src to tgt
    this->findPath(vDijkstraTarget, path, p); //finds path in reverse(target to source) in Dijkstra graph
    std::reverse(path.begin(), path.end());
    LAYOUT_ASSERT(path[0] == vDijkstraSource & path[path.size() - 1] == vDijkstraTarget,
                  LayoutException("findShortestPath",
                                  LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                  "shortest path",
                                  "aug. source/target")); //path is from source to target
    //----------------------------------------------------------------------------------------------------------
}

void GraphPreProcessor::findPath(GraphPreProcessor::dijkstra_vertex_descriptor &vTarget,
                                 std::vector<GraphPreProcessor::dijkstra_vertex_descriptor> &myPath,
                                 std::vector<GraphPreProcessor::dijkstra_vertex_descriptor> &parentVertex)
{
    LAYOUT_ASSERT(vTarget < parentVertex.size(),
                  LayoutException("findPath",
                                  LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE,
                                  "parentVertex",
                                  "index out of bound")); //parentVertex[vTarget] not NULL
    if(parentVertex[vTarget] == vTarget)
    {
        myPath.push_back(vTarget);
        return;
    }
    else
    {
       myPath.push_back(vTarget);
       findPath(parentVertex[vTarget], myPath, parentVertex);
       return;
    }
}

void GraphPreProcessor::addParallelEdges(SubGraph &gInputGraph, EdgeOrderType &oParallelEdgeList, std::vector<QString> & oParrallelEdgeIDs)
{
    BoostGraphWrapper boostGraphWrapper;
    //for each parallel edge, add it to graph
    for(int iParallelEdgeIndex = 0; iParallelEdgeIndex < oParallelEdgeList.size(); ++iParallelEdgeIndex)
    {
        VertexDescriptor vSourceOfParallelEdge, vTargetOfParallelEdge;
        vSourceOfParallelEdge = source(oParallelEdgeList[iParallelEdgeIndex], gInputGraph);
        vTargetOfParallelEdge = target(oParallelEdgeList[iParallelEdgeIndex], gInputGraph);
        QString sEdgeID = oParrallelEdgeIDs[iParallelEdgeIndex];

        EdgeBoolPair pEdgeBool = add_edge(vSourceOfParallelEdge, vTargetOfParallelEdge, gInputGraph);
        LAYOUT_ASSERT(pEdgeBool.second,
                      LayoutException("addParallelEdges",
                                      LayoutExceptionEnum::INVALID_OPERATION,
                                      "add_edge",
                                      "addParallelEdges"));
        boostGraphWrapper.setEdgeId(pEdgeBool.first, gInputGraph, sEdgeID);
    }

    //reinitialize edge index
    LayoutUtility layoutUtility;
    layoutUtility.reinitializeEdgeIndices(gInputGraph);
}

void GraphPreProcessor::removeDummyVertices(SubGraph & gInputGraph, VertexOrderType &oDummyVertices,
                                            std::vector<GraphPreProcessor::EdgePairType> & correspondingCrossingEdge)
{
    //for each dummy vertex (in reverse order of insertion)
    for(int iDummyVertexIndex = oDummyVertices.size() - 1; iDummyVertexIndex >= 0; --iDummyVertexIndex)
    {
        std::vector<EdgeDescriptor> oEdgesToBeRemoved;

        //remove all edges incident on current dummy vertex-----------------------------------------------------
        //for each edge in InputGraph
        EdgeIterator itrEdge, itrEdgeEnd;
        for(boost::tie(itrEdge, itrEdgeEnd)=edges(gInputGraph); itrEdge != itrEdgeEnd; ++itrEdge)
        {
            //if it has dummy vertex as end-point
            if(source(*itrEdge, gInputGraph) == oDummyVertices[iDummyVertexIndex]
                    || target(*itrEdge, gInputGraph) == oDummyVertices[iDummyVertexIndex])
            {
                //remember it as ToBeRemoved
                oEdgesToBeRemoved.push_back(*itrEdge);
            }
            //else continue
        }

        //for each edge ToBeRemoved
        for(int iEdgeIndex = 0; iEdgeIndex < oEdgesToBeRemoved.size(); ++iEdgeIndex)
        {
            VertexDescriptor vSourceOfEdgeToBeRemoved, vTargetOfEdgeToBeRemoved;
            vSourceOfEdgeToBeRemoved = source(oEdgesToBeRemoved[iEdgeIndex], gInputGraph);
            vTargetOfEdgeToBeRemoved = target(oEdgesToBeRemoved[iEdgeIndex], gInputGraph);
            //remove edge
            remove_edge(vSourceOfEdgeToBeRemoved, vTargetOfEdgeToBeRemoved, gInputGraph);
            remove_edge(vTargetOfEdgeToBeRemoved, vSourceOfEdgeToBeRemoved, gInputGraph);
        }
        //------------------------------------------------------------------------------------------------------

        //add the corresponding crossing edges back-------------------------------------------------------------
        //for each crossing-edge corresponding to current dummy vrtex
        LAYOUT_ASSERT(iDummyVertexIndex < correspondingCrossingEdge.size(),
                      LayoutException("removeDummyVertices",
                                      LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                      "correspondingCrossingEdge",
                                      "index out of bound"));
        LAYOUT_ASSERT(correspondingCrossingEdge[iDummyVertexIndex].size() == 2,
                      LayoutException("removeDummyVertices",
                                      LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                      "correspondingCrossingEdges for dummyVertex",
                                      "index out of bound"));
        for(int iCrossingEdgeIndex = 0; iCrossingEdgeIndex < correspondingCrossingEdge[iDummyVertexIndex].size(); ++iCrossingEdgeIndex)
        {
            LAYOUT_ASSERT(correspondingCrossingEdge[iDummyVertexIndex][iCrossingEdgeIndex].size() == 2,
                          LayoutException("removeDummyVertices",
                                          LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                          "endPoints of correspondingCrossingEdge for dummyVertex",
                                          "index out of bound"));
            //get its src and tgt
            VertexDescriptor vSourceOfCrossingEdge, vTargetOfCrossingEdge;
            vSourceOfCrossingEdge = correspondingCrossingEdge[iDummyVertexIndex][iCrossingEdgeIndex][0];
            vTargetOfCrossingEdge = correspondingCrossingEdge[iDummyVertexIndex][iCrossingEdgeIndex][1];
            //if not already added to InputGraph
            if(!(lookup_edge(vSourceOfCrossingEdge, vTargetOfCrossingEdge, gInputGraph).second
                 || lookup_edge(vTargetOfCrossingEdge, vSourceOfCrossingEdge, gInputGraph).second))
            {
                //add edge
                LAYOUT_ASSERT(add_edge(vSourceOfCrossingEdge, vTargetOfCrossingEdge, gInputGraph).second,
                              LayoutException("removeDummyVertices",
                                              LayoutExceptionEnum::INVALID_OPERATION,
                                              "add_edge",
                                              "removeDummyVertices"));
            }
            //else continue
        }
        //-----------------------------------------------------------------------------------------------------

        //reinitialize edge index
        LayoutUtility layoutUtility;
        layoutUtility.reinitializeEdgeIndices(gInputGraph);

        //set dummy vertex invisible
        BoostGraphWrapper boostGraphWrapper;
        boostGraphWrapper.setVertexIsInvisible(oDummyVertices[iDummyVertexIndex], gInputGraph, true);
    }
}


void GraphPreProcessor::toPlanarAlternate(SubGraph &gInputGraph, EdgeOrderType &oNonPlanarEdges)
{
    VertexDescriptor descVertexSource, descVertexTarget, vNewInputGraphVertex;
    EdgeIterator itrEdge, itrEdgeEnd;
    EdgeDescriptor descEdge;
    int iVertexSource, iVertexTarget, iNumVertices;
    BoostGraphWrapper graphWrapper;
    iNumVertices = boost::num_vertices(gInputGraph);
    GraphPreProcessor::graph gLacalGraph(iNumVertices); //create a local undirected graph with same no. of vertices
    std::vector< graph_traits< graph >::edge_descriptor > nonPlanarUndirEdges;
    graph_traits< graph >::edge_descriptor eCurrentEdge, eTemporaryEdge1, eTemporaryEdge2, eTemporaryEdge3;
    bool bAdded;

    //PLANARIZATION------------------------------------------------------------------------------------------
    // add each edge from gInputGraph to gLacalGraph one by one, until gLacalGraph is planar
    // if an edge makes gLacalGraph non-planar, remove and store it aside
    property_map<graph, edge_index_t>::type e_index = get(edge_index, gLacalGraph);
    graph_traits<graph>::edges_size_type edge_count;
    graph_traits<graph>::edge_iterator ei, ei_end;

    for(boost::tie(itrEdge, itrEdgeEnd) = edges(gInputGraph); itrEdge != itrEdgeEnd; itrEdge++)
    {
        descEdge = *itrEdge;
        descVertexSource = graphWrapper.getEdgeSource(descEdge, gInputGraph);
        descVertexTarget = graphWrapper.getEdgeTarget(descEdge, gInputGraph);

        iVertexSource = get(vertex_index, gInputGraph, descVertexSource);
        iVertexTarget = get(vertex_index, gInputGraph, descVertexTarget);
        boost::tie(eCurrentEdge, bAdded) = add_edge(iVertexSource, iVertexTarget, gLacalGraph);

        if(bAdded)
        {
        //Initialize the interior edge index
        edge_count = 0;
        for(boost::tie(ei, ei_end) = edges(gLacalGraph); ei != ei_end; ++ei)
            put(e_index, *ei, edge_count++);

        //Test for planarity; compute the planar embedding as a side-effect
        typedef std::vector< graph_traits<graph>::edge_descriptor > vec_t;
        std::vector<vec_t> embedding(num_vertices(gLacalGraph));
        if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = gLacalGraph,
                                         boyer_myrvold_params::embedding = &embedding[0]))
        {
            //std::cout << "Input graph is planar3" << std::endl;
        }
        else
        {
            //std::cout << "Input graph is not planar3" << std::endl;
            nonPlanarUndirEdges.push_back(eCurrentEdge);
            remove_edge(eCurrentEdge, gLacalGraph);

            //Re-initialize the interior edge index
            edge_count = 0;
            for(boost::tie(ei, ei_end) = edges(gLacalGraph); ei != ei_end; ++ei)
                put(e_index, *ei, edge_count++);
        }

        }
    }
    //END----------------------------------------------------------------------------------------------------

    //REMOVE NON-PLANAR EDGES FROM INPUT GRAPH---------------------------------------------------------------
    for(int iEdgeIndex = 0; iEdgeIndex < nonPlanarUndirEdges.size(); ++iEdgeIndex)
    {
        //find corresponding non-planar edge in gInputGraph
        VertexDescriptor vSourceInInputGraph, vTargetInInputGraph;
        vSourceInInputGraph = vertex(get(vertex_index,
                                         gInputGraph,
                                         source(nonPlanarUndirEdges[iEdgeIndex], gLacalGraph)),
                                     gInputGraph);
        vTargetInInputGraph = vertex(get(vertex_index,
                                         gInputGraph,
                                         target(nonPlanarUndirEdges[iEdgeIndex], gLacalGraph)),
                                     gInputGraph);

        EdgeDescriptor eNonPlanarEdgeInInputGraph;
        if(lookup_edge(vSourceInInputGraph, vTargetInInputGraph, gInputGraph).second)
            eNonPlanarEdgeInInputGraph = lookup_edge(vSourceInInputGraph, vTargetInInputGraph, gInputGraph).first;
        else if(lookup_edge(vTargetInInputGraph, vSourceInInputGraph, gInputGraph).second)
            eNonPlanarEdgeInInputGraph = lookup_edge(vTargetInInputGraph, vSourceInInputGraph, gInputGraph).first;
        else
            std::cout << "Error in finding non-planar edge" << std::endl;

        //remove that edge from gInputGraph and store it
        oNonPlanarEdges.push_back(eNonPlanarEdgeInInputGraph);
        remove_edge(eNonPlanarEdgeInInputGraph, gInputGraph);
    }
    //END----------------------------------------------------------------------------------------------------
}

void GraphPreProcessor::addNonPlanarEdges(SubGraph &gInputGraph, EdgeOrderType &oNonPlanarEdges)
{
    for(int i = 0; i < oNonPlanarEdges.size(); ++i)
    {
        add_edge(source(oNonPlanarEdges[i], gInputGraph),
                 target(oNonPlanarEdges[i], gInputGraph),
                 gInputGraph);
    }
    //Initialize the interior edge index in input graph
    property_map<SubGraph, edge_index_t>::type in_e_index = get(edge_index, gInputGraph);
    graph_traits<SubGraph>::edges_size_type in_edge_count = 0;
    graph_traits<SubGraph>::edge_iterator in_ei, in_ei_end;
    for(boost::tie(in_ei, in_ei_end) = edges(gInputGraph); in_ei != in_ei_end; ++in_ei)
        put(in_e_index, *in_ei, in_edge_count++);
}