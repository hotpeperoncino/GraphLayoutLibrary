#include "GridLayouter.h"

GridLayouter::GridLayouter()
{
}

void GridLayouter::gridLayouterForClusteredGraph(SubGraph & gCurrentClusteredGraph,
                                                 SubGraph & gRootClusteredGraph)
{
    BoostGraphWrapper boostGraphWrapper;

    //Construct a non-clustered graph newG for given clustered graph--------------------------------------------
    //where, all clusters are replaced by nodes with same size

    //create newG
    SubGraph gNewGraph;

    //define data structures to store mapping between nodes of newG and originalG
    QMap <int, bool> isCluster;
    QMap <int, VertexIterator> originalVertexForNewVertex;
    QMap <int, ChildrenIterator> childrenList;

    //add nodes to newG
    this->addVerticesToNewGraph(gCurrentClusteredGraph, gNewGraph, gRootClusteredGraph,
                                isCluster, originalVertexForNewVertex, childrenList);
    LAYOUT_ASSERT(boost::num_vertices(gNewGraph) == isCluster.size(),
                  LayoutException("gridLayouterForClusteredGraph",
                                  LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                  "isCluster",
                                  "after adding node to NewGraph"));

    //if nothing added to newG, it means that current clustered graph doesnot contain anything
    //(neither vertex nor another cluster)
    //in such case, add a dummy invisible node with some fixed size and location and return
    if(boost::num_vertices(gNewGraph) == 0)
    {
        //add a dummy invisible vertex of some fixed size
        VertexDescriptor vDummyInvisibleVertex = add_vertex(gCurrentClusteredGraph);
        boostGraphWrapper.setVertexCenterCoordX(vDummyInvisibleVertex, gCurrentClusteredGraph, 0);
        boostGraphWrapper.setVertexCenterCoordY(vDummyInvisibleVertex, gCurrentClusteredGraph, 0);
        boostGraphWrapper.setVertexHeight(vDummyInvisibleVertex, gCurrentClusteredGraph, 40);
        boostGraphWrapper.setVertexWidth(vDummyInvisibleVertex, gCurrentClusteredGraph, 40);
        boostGraphWrapper.setVertexIsInvisible(vDummyInvisibleVertex, gCurrentClusteredGraph, true);

        //Update properties of current clustered graph
        Reingold reingold;
        int iMargine = 10;
        reingold.setCompartMentProps(gCurrentClusteredGraph, iMargine);

        return;
    }

    //define data structures to store edges to be added to newG and their corres. original edges
    std::vector<GraphPreProcessor::VertexOrderType> oEdgesToRemove;
    std::vector<GridLayouter::edgeType> oEdgesToAdd;
    std::vector<GraphPreProcessor::VertexOrderType> oInnerEdgesToRemove;

    //find edges to be added and removed
    this->findEdgesToBeAddedRemoved(gCurrentClusteredGraph, gNewGraph,
                                    oEdgesToRemove,
                                    oEdgesToAdd,
                                    oInnerEdgesToRemove,
                                    childrenList);

    LAYOUT_ASSERT(oEdgesToRemove.size() == oEdgesToAdd.size(),
                  LayoutException("gridLayouterForClusteredGraph",
                                  LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                  "oEdgesToRemove-oEdgesToAdd",
                                  "after finding edges to be added/removed"));

    //add edges to newG
    this->addEdgesToNewGraph(gCurrentClusteredGraph, gNewGraph,
                             oEdgesToRemove,
                             oEdgesToAdd,
                             oInnerEdgesToRemove,
                             originalVertexForNewVertex);
    //---------------------------------------------------------------------------------------------------------

    //call gridLayouterForNonClusteredGraph() for newG---------------------------------------------------------
    //Remove parallel edges
    GraphPreProcessor graphPreProcessor;
    GraphPreProcessor::EdgeOrderType oParallelEdges;
    std::vector<QString> oParallelEdgeIDs;
    graphPreProcessor.removeParallelEdges(gNewGraph, oParallelEdges, oParallelEdgeIDs);
    //NEED TO REMOVE PARALLEL EDGES since they may be introdiced when adding edges between nodes-for-clusters

    this->gridLayouterForNonClusteredGraph(gNewGraph);

//    //Add parallel edges back
//    graphPreProcessor.addParallelEdges(gNewGraph, oParallelEdges);
    //NO NEED TO ADD PARALLEL EDGES BACK since we use only coordinates of vertices of newG
    //---------------------------------------------------------------------------------------------------------

    //copy back the coordinates to original graph
    this->copyBackCoordinates(gCurrentClusteredGraph, gNewGraph, gRootClusteredGraph,
                              isCluster,
                              originalVertexForNewVertex,
                              childrenList);

    //Update properties of current clustered graph
    Reingold reingold;
    int iMargine = 10;
    reingold.setCompartMentProps(gCurrentClusteredGraph, iMargine);
}


void GridLayouter::addVerticesToNewGraph(SubGraph & gCurrentClusteredGraph, SubGraph & gNewGraph,
                                         SubGraph & gRootClusteredGraph,
                                         QMap <int, bool> & isCluster,
                                         QMap <int, VertexIterator> & originalVertexForNewVertex,
                                         QMap <int, ChildrenIterator> & childrenList)
{
    BoostGraphWrapper boostGraphWrapper;
    int iGraphClusterId = boostGraphWrapper.getGraphClusterID(gCurrentClusteredGraph);

    //add normal nodes from current graph to newG-----------------------------------------------------------
    VertexIterator itrVert, itrEndVert;
    for(boost::tie(itrVert, itrEndVert) = vertices(gCurrentClusteredGraph); itrVert != itrEndVert; itrVert++)
    {
        VertexDescriptor vCurrentVertexInClusteredGraph = *itrVert;
        int iVertexClusterId = boostGraphWrapper.getVertexClusterID(*itrVert, gCurrentClusteredGraph);
        if(iVertexClusterId == iGraphClusterId)
        {
            VertexDescriptor vNew = add_vertex(gNewGraph);

            //set size of new node same as the original node
            int iVertexHeight = boostGraphWrapper.getVertexHeight(vCurrentVertexInClusteredGraph, gCurrentClusteredGraph);
            int iVertexWidth = boostGraphWrapper.getVertexWidth(vCurrentVertexInClusteredGraph, gCurrentClusteredGraph);
            boostGraphWrapper.setVertexHeight(vNew, gNewGraph, iVertexHeight);
            boostGraphWrapper.setVertexWidth(vNew, gNewGraph, iVertexWidth);

            int iNewVertexIndex = get(vertex_index, gNewGraph, vNew);
            isCluster.insert(iNewVertexIndex, false);
            originalVertexForNewVertex.insert(iNewVertexIndex, itrVert);
        }
    }
    //--------------------------------------------------------------------------------------------------------

    //for each child, call gridLayouterForClusteredGraph() and add newNode to newG----------------------------
    ChildrenIterator itrChild, itrEndChild;
    for(boost::tie(itrChild, itrEndChild) = gCurrentClusteredGraph.children(); itrChild != itrEndChild; ++itrChild)
    {
        this->gridLayouterForClusteredGraph(*itrChild, gRootClusteredGraph);
        VertexDescriptor vNew = add_vertex(gNewGraph);

        //set size of new node same as the child graph
        int iChildHeight, iChildWidth;
        iChildHeight = boostGraphWrapper.getGraphHeight(*itrChild);
        iChildWidth = boostGraphWrapper.getGraphWidth(*itrChild);
        boostGraphWrapper.setVertexHeight(vNew, gNewGraph, iChildHeight);
        boostGraphWrapper.setVertexWidth(vNew, gNewGraph, iChildWidth);

        int iNewVertexIndex = get(vertex_index, gNewGraph, vNew);
        isCluster.insert(iNewVertexIndex, true);
        childrenList.insert(iNewVertexIndex, itrChild);
    }
    //---------------------------------------------------------------------------------------------------------
}


void GridLayouter::findEdgesToBeAddedRemoved(SubGraph & gCurrentClusteredGraph, SubGraph & gNewGraph,
                                             std::vector<GraphPreProcessor::VertexOrderType> & oEdgesToRemove,
                                             std::vector<GridLayouter::edgeType> & oEdgesToAdd,
                                             std::vector<GraphPreProcessor::VertexOrderType> & oInnerEdgesToRemove,
                                             QMap <int, ChildrenIterator> & childrenList)
{
    std::vector<VertexDescriptor> oEndPointsOfEdgeToRemove(2);
    GridLayouter::edgeType oEndPointsOfEdgeToAdd(2);
    std::vector<VertexDescriptor> oEndPointsOfInnerEdge(2);

    //find edges in/out of each child and their corresponding newEdges

    //for each child in currentG-----------------------------------------------------------------------------
    ChildrenIterator itrChild, itrEndChild;
    for(boost::tie(itrChild, itrEndChild) = gCurrentClusteredGraph.children(); itrChild != itrEndChild; ++itrChild)
    {
        //find newNode (in newG) corresponding to this child
        VertexDescriptor vCorresVertInNewG;
        bool bFound = false;
        QMapIterator<int, ChildrenIterator> itrChildrenListMap(childrenList);
        while (itrChildrenListMap.hasNext())
        {
            itrChildrenListMap.next();
            if(itrChild == itrChildrenListMap.value())
            {
                vCorresVertInNewG = vertex(itrChildrenListMap.key(), gNewGraph);
                bFound = true;
                break;
            }
        }
        LAYOUT_ASSERT(bFound,
                      LayoutException("findEdgesToBeAddedRemoved",
                                      LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                      "childrenList",
                                      ""));

        //for each vertex in this child----------------------------------------------------------------------
        VertexIterator itrVert, itrEndVert;
        for(boost::tie(itrVert, itrEndVert) = vertices(*itrChild); itrVert != itrEndVert; ++itrVert)
        {
            //get its global descriptor
            VertexDescriptor vGlobalOfVert = (*itrChild).local_to_global(*itrVert);

            //for each edge in currentG----------------------------------------------------------------------
            EdgeIterator itrEdge, itrEndEdge;
            for(boost::tie(itrEdge, itrEndEdge) =  edges(gCurrentClusteredGraph); itrEdge != itrEndEdge; ++itrEdge)
            {
                //Find the type of edge----------------------------------------------------------------------
                GridLayouter::EdgeType enumEdgeStatus;

                //find src and tgt of edge (in currentG)
                VertexDescriptor vSrcOfEdgeInCurrentG, vTgtOfEdgeInCurrentG;
                vSrcOfEdgeInCurrentG = source(*itrEdge, gCurrentClusteredGraph);
                vTgtOfEdgeInCurrentG = target(*itrEdge, gCurrentClusteredGraph);
                //find global of src and tgt
                VertexDescriptor vGlobalOfSrc, vGlobalOfTgt;
                vGlobalOfSrc = gCurrentClusteredGraph.local_to_global(vSrcOfEdgeInCurrentG);
                vGlobalOfTgt = gCurrentClusteredGraph.local_to_global(vTgtOfEdgeInCurrentG);

                //find if the edge is incident on current vertex or not
                if(vGlobalOfVert == vGlobalOfSrc) //if current vertex is src; E(v --> tgt)
                {
                    if(!(*itrChild).find_vertex(vGlobalOfTgt).second) //if tgt lies outside current child
                    {
                        //save the type of edge
                        enumEdgeStatus = GOING_OUT;
                        //remember the egde as 'to be removed'
                        oEndPointsOfEdgeToRemove[0] = vSrcOfEdgeInCurrentG;
                        oEndPointsOfEdgeToRemove[1] = vTgtOfEdgeInCurrentG;
                        //remenber its corresponding edge 'to be added' in newG
                        oEndPointsOfEdgeToAdd[0].vNode = vCorresVertInNewG;
                        oEndPointsOfEdgeToAdd[0].vertexType = NEW_VERTEX;
                        oEndPointsOfEdgeToAdd[1].vNode = vTgtOfEdgeInCurrentG;
                        oEndPointsOfEdgeToAdd[1].vertexType = ORIGINAL_VERTEX;
                    }
                    else //if tgt also lies inside current child
                    {
                        //save the type of edge
                        enumEdgeStatus = LIES_IN;
                        //remember the egde as 'inner edge'; not added in newG
                        oEndPointsOfInnerEdge[0] = vSrcOfEdgeInCurrentG;
                        oEndPointsOfInnerEdge[1] = vTgtOfEdgeInCurrentG;
                    }
                }
                else if(vGlobalOfVert == vGlobalOfTgt) //if current vertex is tgt; E(src --> v)
                {
                    if(!(*itrChild).find_vertex(vGlobalOfSrc).second) //if src lies outside current child
                    {
                        //save the type of edge
                        enumEdgeStatus = COMMING_IN;
                        //remember the egde as 'to be removed'
                        oEndPointsOfEdgeToRemove[0] = vSrcOfEdgeInCurrentG;
                        oEndPointsOfEdgeToRemove[1] = vTgtOfEdgeInCurrentG;
                        //remenber its corresponding edge 'to be added' in newG
                        oEndPointsOfEdgeToAdd[0].vNode = vSrcOfEdgeInCurrentG;
                        oEndPointsOfEdgeToAdd[0].vertexType = ORIGINAL_VERTEX;
                        oEndPointsOfEdgeToAdd[1].vNode = vCorresVertInNewG;
                        oEndPointsOfEdgeToAdd[1].vertexType = NEW_VERTEX;
                    }
                    else //if src also lies inside current child
                    {
                        //save the type of edge
                        enumEdgeStatus = LIES_IN;
                        //remember the egde as 'inner edge'; not added in newG
                        oEndPointsOfInnerEdge[0] = vSrcOfEdgeInCurrentG;
                        oEndPointsOfInnerEdge[1] = vTgtOfEdgeInCurrentG;
                    }
                }
                else //if current vertex is neither src, nor tgt of this edge (edge not incident on current vertex)
                {
                    //save the type of edge
                    enumEdgeStatus = NOT_INCIDENT;
                }
                //---------------------------------------------------------------------------------------------

                //Remember the edges added and/or removed depending on type of current edge--------------------
                //if one end is in child and other out
                if(enumEdgeStatus == GOING_OUT || enumEdgeStatus == COMMING_IN)
                {
                    //search in Removed[]
                    bool bIsInRemoved = false;
                    for(int iIndexOfEdgesToRemove = 0; iIndexOfEdgesToRemove < oEdgesToRemove.size(); ++iIndexOfEdgesToRemove)
                    {
                        //if already in Removed[]
                        if(vSrcOfEdgeInCurrentG == oEdgesToRemove[iIndexOfEdgesToRemove][0] &
                                vTgtOfEdgeInCurrentG == oEdgesToRemove[iIndexOfEdgesToRemove][1])
                        {
                            //compare ToRemove and ToAdd, find common node, replace it in Removed[]
                            //if src is common
                            if(oEdgesToRemove[iIndexOfEdgesToRemove][0] == oEdgesToAdd[iIndexOfEdgesToRemove][0].vNode)
                            {
                                oEdgesToAdd[iIndexOfEdgesToRemove][0].vNode = vCorresVertInNewG;
                                oEdgesToAdd[iIndexOfEdgesToRemove][0].vertexType = NEW_VERTEX;
                                bIsInRemoved = true;
                                break;
                            }
                            //if tgt is common
                            else if(oEdgesToRemove[iIndexOfEdgesToRemove][1] == oEdgesToAdd[iIndexOfEdgesToRemove][1].vNode)
                            {
                                oEdgesToAdd[iIndexOfEdgesToRemove][1].vNode = vCorresVertInNewG;
                                oEdgesToAdd[iIndexOfEdgesToRemove][1].vertexType = NEW_VERTEX;
                                bIsInRemoved = true;
                                break;
                            }
                            else //if no common node found
                            {
                                throw LayoutException("findEdgesToBeAddedRemoved",
                                                       LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                                       "edes to remove/add",
                                                       "common vertex");
                            }
                        }
                    }

                    //if not in Removed[]
                    if(!bIsInRemoved)
                    {
                        //save it in Removed[], and corresponding new edge in Added[]
                        oEdgesToRemove.push_back(oEndPointsOfEdgeToRemove);
                        oEdgesToAdd.push_back(oEndPointsOfEdgeToAdd);
                    }
                }
                else if(enumEdgeStatus == LIES_IN) //if both ends are in child
                {
                    //search in InnerEdges[]
                    bool bIsInInnerEdgesToRemove = false;
                    for(int iInnerEdgeIndex = 0; iInnerEdgeIndex < oInnerEdgesToRemove.size(); ++iInnerEdgeIndex)
                    {
                        //if already in InnerEdges[]
                        if(vSrcOfEdgeInCurrentG == oInnerEdgesToRemove[iInnerEdgeIndex][0] &
                                vTgtOfEdgeInCurrentG == oInnerEdgesToRemove[iInnerEdgeIndex][1])
                        {

                            bIsInInnerEdgesToRemove = true;
                            break;
                        }
                    }
                    if(!bIsInInnerEdgesToRemove)//if not in InnerEdges[]
                    {
                        //save it in InnerEdges[]
                        oInnerEdgesToRemove.push_back(oEndPointsOfInnerEdge);
                    }
                }
                //else NOT_INCIDENT; ignore
                //-------------------------------------------------------------------------------------------
            }
            //--------------------------------------------------------------------------------------EOFL edge
        }
        //--------------------------------------------------------------------------------------EOFL vertices
    }
    //---------------------------------------------------------------------------------------------EOFL child
}


void GridLayouter::addEdgesToNewGraph(SubGraph & gCurrentClusteredGraph, SubGraph & gNewGraph,
                                      std::vector<GraphPreProcessor::VertexOrderType> & oEdgesToRemove,
                                      std::vector<GridLayouter::edgeType> & oEdgesToAdd,
                                      std::vector<GraphPreProcessor::VertexOrderType> & oInnerEdgesToRemove,
                                      QMap <int, VertexIterator> & originalVertexForNewVertex)
{
    //for each edge in currentG------------------------------------------------------------------------------
    EdgeIterator itrEdge, itrEndEdge;
    for(boost::tie(itrEdge, itrEndEdge) = edges(gCurrentClusteredGraph); itrEdge != itrEndEdge; ++itrEdge)
    {
        EdgeDescriptor eCurrentEdge = *itrEdge;
        VertexDescriptor vSourceOfCurrentEdge, vTargetOfCurrentEdge, vSourceOfNewEdge, vTargetOfNewEdge;
        //get its src and tgt
        vSourceOfCurrentEdge = source(eCurrentEdge, gCurrentClusteredGraph);
        vTargetOfCurrentEdge = target(eCurrentEdge, gCurrentClusteredGraph);

        //search the edge in InnerEdges[]
        bool bIsInnerEdge = false;
        int iInnerEdgeIndex;
        for(iInnerEdgeIndex = 0; iInnerEdgeIndex < oInnerEdgesToRemove.size(); ++iInnerEdgeIndex)
        {
            if(vSourceOfCurrentEdge == oInnerEdgesToRemove[iInnerEdgeIndex][0] &
                    vTargetOfCurrentEdge == oInnerEdgesToRemove[iInnerEdgeIndex][1])
            {
                bIsInnerEdge = true;
                break;
            }
        }
        if(bIsInnerEdge) //if edge is in InnerEdges[]
        {
            //don't add it to NewG
        }
        else //if edge is not in InnerEdges[]----------------------------------------------------------------
        {
            //search the edge in Removed[]
            bool bIsToBeRemoved = false;
            int iRemovedEdgeIndex;
            for(iRemovedEdgeIndex = 0; iRemovedEdgeIndex < oEdgesToRemove.size(); ++iRemovedEdgeIndex)
            {
                if(vSourceOfCurrentEdge == oEdgesToRemove[iRemovedEdgeIndex][0] &
                        vTargetOfCurrentEdge == oEdgesToRemove[iRemovedEdgeIndex][1])
                {
                    bIsToBeRemoved = true;
                    break;
                }
            }
            if(bIsToBeRemoved) //edge is in Removed[]
            {
                //get its corresponding edge in Added[]
                if(oEdgesToAdd[iRemovedEdgeIndex][0].vertexType == ORIGINAL_VERTEX) //if source of this edge is from original graph
                {
                    //search src of 'Added' in originalVerticesList
                    bool bHasFound = false;
                    QMapIterator<int, VertexIterator> itroriginalVertexForNewVertexMap(originalVertexForNewVertex);
                    while (itroriginalVertexForNewVertexMap.hasNext())
                    {
                        itroriginalVertexForNewVertexMap.next();
                        if(oEdgesToAdd[iRemovedEdgeIndex][0].vNode
                                == *itroriginalVertexForNewVertexMap.value()) //if found in list
                        {
                            //get its corresponding new vertex
                            vSourceOfNewEdge = vertex(itroriginalVertexForNewVertexMap.key(), gNewGraph);
                            bHasFound = true;
                            break;
                        }
                    }
//                    LAYOUT_ASSERT(bHasFound,
//                                  LayoutException("addEdgesToNewGraph",
//                                                  LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
//                                                  "originalVertexForNewVertex",
//                                                  "source of edge to be replaced"));
                    if(!bHasFound)
                    {
                        goto label_SkipTheEdge;
                    }
                }
                else //if source of this edge is from new graph (NEW_VERTEX)
                {
                    vSourceOfNewEdge = oEdgesToAdd[iRemovedEdgeIndex][0].vNode;
                }

                if(oEdgesToAdd[iRemovedEdgeIndex][1].vertexType == ORIGINAL_VERTEX) //if target of this edge is from original graph
                {
                    //search src of 'Added' in originalVerticesList
                    bool bHasFound = false;
                    QMapIterator<int, VertexIterator> itroriginalVertexForNewVertexMap(originalVertexForNewVertex);
                    while (itroriginalVertexForNewVertexMap.hasNext())
                    {
                        itroriginalVertexForNewVertexMap.next();
                        if(oEdgesToAdd[iRemovedEdgeIndex][1].vNode
                                == *itroriginalVertexForNewVertexMap.value())
                        {
                            //get its corresponding new vertex
                            vTargetOfNewEdge = vertex(itroriginalVertexForNewVertexMap.key(), gNewGraph);
                            bHasFound = true;
                            break;
                        }
                    }
//                    LAYOUT_ASSERT(bHasFound,
//                                  LayoutException("addEdgesToNewGraph",
//                                                  LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
//                                                  "originalVertexForNewVertex",
//                                                  "target of edge to be replaced"));
                    if(!bHasFound)
                    {
                        goto label_SkipTheEdge;
                    }
                }
                else //if target of this edge is from new graph (NEW_VERTEX)
                {
                    vTargetOfNewEdge = oEdgesToAdd[iRemovedEdgeIndex][1].vNode;
                }

                //add edge to newG
                LAYOUT_ASSERT(add_edge(vSourceOfNewEdge, vTargetOfNewEdge, gNewGraph).second,
                              LayoutException("addEdgesToNewGraph",
                                              LayoutExceptionEnum::INVALID_OPERATION,
                                              "add_edge(replace)",
                                              "addEdgesToNewGraph"));
                label_SkipTheEdge:
                continue;
            }
            else //if edge not in Removed[], it is to be added as it is
            {
                //search its src in originalVerticesList
                bool bHasFound = false;
                QMapIterator<int, VertexIterator> itroriginalVertexForNewVertexMap1(originalVertexForNewVertex);
                while (itroriginalVertexForNewVertexMap1.hasNext())
                {
                    itroriginalVertexForNewVertexMap1.next();
                    if(vSourceOfCurrentEdge
                            == *itroriginalVertexForNewVertexMap1.value())
                    {
                        //get its corresponding new vertex
                        vSourceOfNewEdge = vertex(itroriginalVertexForNewVertexMap1.key(), gNewGraph);
                        bHasFound = true;
                        break;
                    }
                }
                LAYOUT_ASSERT(bHasFound,
                              LayoutException("addEdgesToNewGraph",
                                              LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                              "originalVertexForNewVertex",
                                              "source of edge to be add"));

                //search its tgt in originalVerticesList
                bHasFound = false;
                QMapIterator<int, VertexIterator> itroriginalVertexForNewVertexMap2(originalVertexForNewVertex);
                while (itroriginalVertexForNewVertexMap2.hasNext())
                {
                    itroriginalVertexForNewVertexMap2.next();
                    if(vTargetOfCurrentEdge
                            == *itroriginalVertexForNewVertexMap2.value())
                    {
                        //get its corresponding new vertex
                        vTargetOfNewEdge = vertex(itroriginalVertexForNewVertexMap2.key(), gNewGraph);
                        bHasFound = true;
                        break;
                    }
                }
                LAYOUT_ASSERT(bHasFound,
                              LayoutException("addEdgesToNewGraph",
                                              LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                              "originalVertexForNewVertex",
                                              "target of edge to be add"));

                //add edge to newG
                LAYOUT_ASSERT(add_edge(vSourceOfNewEdge, vTargetOfNewEdge, gNewGraph).second,
                              LayoutException("addEdgesToNewGraph",
                                              LayoutExceptionEnum::INVALID_OPERATION,
                                              "add_edge(as it is)",
                                              "addEdgesToNewGraph"));
            }
        }
        //---------------------------------------------------------------------------------------------------
    }
    //-------------------------------------------------------------------------------------------------------
}


void GridLayouter::gridLayouterForNonClusteredGraph(SubGraph & gCurrentNonClusteredGraph)
{
    LAYOUT_ASSERT(num_vertices(gCurrentNonClusteredGraph) > 0,
                  LayoutException("gridLayouterForNonClusteredGraph",
                                  LayoutExceptionEnum::EMPTY_CONTAINER,
                                  "zero vertices",
                                  "NonClusteredGraph"));

    GraphPreProcessor graphPreProcessor;
    GridBasedLayout gridBasedLayout;
    BoostGraphWrapper boostGraphWrapper;

    GraphPreProcessor::VertexOrderType oDummyVertices;
    std::vector<GraphPreProcessor::EdgePairType> oCrossingEdges;
    GraphPreProcessor::EdgeOrderType oDummyEdges;
    std::vector<int> oCanonicalOrder;
    std::vector< GridBasedLayout::Position > oVertexPossition;

    if(boost::num_vertices(gCurrentNonClusteredGraph) > 2)
    {
        //CONVERT NON-PLANAR GRAPH TO PLANAR
        //In this process, some extra vertices (at edge crossings) and edges may be added to the graph
        //We also store the crossing edges, which a DummyVertex replaces, in m_oCrossingEdges
        graphPreProcessor.toPlanar(gCurrentNonClusteredGraph, oDummyVertices, oCrossingEdges);

        //CONVERT PLANAR GRAPH TO MAXIMAL PLANAR
        //In this process, some extra edges may be added to the graph
        graphPreProcessor.toMaximalPlanar(gCurrentNonClusteredGraph, oDummyEdges);

        //FIND CANONICAL ORDER OF VERTICES
        gridBasedLayout.findCanonicalOrder(gCurrentNonClusteredGraph, oCanonicalOrder);

        //APPLY GRID BASED LAYOUT
        //findGridBasedLayout() requires the graph to be MAXIMAL PLANAR ONLY
        gridBasedLayout.findGridBasedLayout(gCurrentNonClusteredGraph, oCanonicalOrder, oVertexPossition);

        //REMOVE DUMMY EDGES
        //This section removes the extra edges added during conversion from planar graph to maximal planar graph
        graphPreProcessor.removeDummyEdges(gCurrentNonClusteredGraph, oDummyEdges);

        //REMOVE DUMMY VERTICES
        //This section removes the extra vertices and edges added
        //during conversion from non-planar graph to planar graph
        graphPreProcessor.removeDummyVertices(gCurrentNonClusteredGraph, oDummyVertices, oCrossingEdges);
    }
    else //if(boost::num_vertices(gCurrentNonClusteredGraph) <= 2)
    {
        std::vector< GridBasedLayout::Position > PositionVector(boost::num_vertices(gCurrentNonClusteredGraph));
        int iXCoordInSmallGraph = 0;
        int iSeparation = 1;

        //if there are 2 vertices, find separation needed to avoid overlap
        if(boost::num_vertices(gCurrentNonClusteredGraph) == 2)
        {
            VertexDescriptor v0, v1;
            v0 = vertex(0, gCurrentNonClusteredGraph);
            v1 = vertex(1, gCurrentNonClusteredGraph);
            int iWidthOfV0 = boostGraphWrapper.getVertexWidth(v0, gCurrentNonClusteredGraph);
            int iWidthOfV1 = boostGraphWrapper.getVertexWidth(v1, gCurrentNonClusteredGraph);

            iSeparation = std::ceil((iWidthOfV0/2) + (iWidthOfV1/2)) + (2*SEPARATION);
        }

        //set positions of vertices
        VertexIterator itrVertex, itrVertexEnd;
        for(boost::tie(itrVertex, itrVertexEnd) = vertices(gCurrentNonClusteredGraph); itrVertex != itrVertexEnd; ++itrVertex)
        {
            int iVertexIndex = get(vertex_index, gCurrentNonClusteredGraph, *itrVertex);
            PositionVector[iVertexIndex].iCoordX = iXCoordInSmallGraph;
            PositionVector[iVertexIndex].iCoordY = 0;
            iXCoordInSmallGraph += iSeparation; //++iXCoordInSmallGraph;
        }

        oVertexPossition = PositionVector;
    }

    //set coordinates of vertices-------------------------------------------------------------------------------
    VertexIterator iterVertex, iterVertexEnd;
    VertexDescriptor descVertex;
    int iVertexIndex, iXCoordinate, iYCoordinate;

    LAYOUT_ASSERT(oVertexPossition.size() == boost::num_vertices(gCurrentNonClusteredGraph),
                  LayoutException("gridLayouterForNonClusteredGraph",
                                  LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                  "oVertexPosition",
                                  ""));

    //for each vertex
    for(boost::tie(iterVertex, iterVertexEnd) = vertices(gCurrentNonClusteredGraph); iterVertex != iterVertexEnd; ++iterVertex)
    {
        descVertex = *iterVertex;
        iVertexIndex = get(vertex_index, gCurrentNonClusteredGraph, descVertex);
        iXCoordinate = oVertexPossition[iVertexIndex].iCoordX;
        iYCoordinate = oVertexPossition[iVertexIndex].iCoordY;
        //set coordinates
        boostGraphWrapper.setVertexCenterCoordX(descVertex, gCurrentNonClusteredGraph, iXCoordinate);
        boostGraphWrapper.setVertexCenterCoordY(descVertex, gCurrentNonClusteredGraph, iYCoordinate);
    }
    //----------------------------------------------------------------------------------------------------------
}

void GridLayouter::copyBackCoordinates(SubGraph & gCurrentClusteredGraph, SubGraph & gNewGraph,
                                       SubGraph & gRootClusteredGraph,
                                       QMap <int, bool> & isCluster,
                                       QMap <int, VertexIterator> & originalVertexForNewVertex,
                                       QMap <int, ChildrenIterator> & childrenList)
{
    LAYOUT_ASSERT(isCluster.count() <= boost::num_vertices(gNewGraph),
                  LayoutException("copyBackCoordinates",
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  "isCluster",
                                  " "));
    BoostGraphWrapper boostGraphWrapper;

    //for each vertex in newG
    VertexIterator itrNewVertex, itrEndNewVertex;
    for(boost::tie(itrNewVertex, itrEndNewVertex) = vertices(gNewGraph); itrNewVertex != itrEndNewVertex; ++itrNewVertex)
    {
        int iIndexOfNewVertex = get(vertex_index, gNewGraph, *itrNewVertex);
        VertexDescriptor vCurrentVertex = *itrNewVertex;
        bool bIsInvisible = boostGraphWrapper.getVertexIsInvisible(vCurrentVertex, gNewGraph);
        if(!bIsInvisible)
        {
            //get cooridinates of current NewVertex
            int iXCoordOfNewVertex = boostGraphWrapper.getVertexCenterCoordX(vCurrentVertex, gNewGraph);
            int iYCoordOfNewVertex = boostGraphWrapper.getVertexCenterCoordY(vCurrentVertex, gNewGraph);

            LAYOUT_ASSERT(isCluster.contains(iIndexOfNewVertex),
                          LayoutException("copyBackCoordinates",
                                          LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                          "isCluster",
                                          "NewVertex"));
            if(isCluster.value(iIndexOfNewVertex)) //if current NewVertex is a cluster
            {
                //calculate offset coordinates
                int iHeightOfNewVertex = boostGraphWrapper.getVertexHeight(vCurrentVertex, gNewGraph);
                int iWidthOfNewVertex = boostGraphWrapper.getVertexWidth(vCurrentVertex, gNewGraph);
                int iOffsetXCoord = iXCoordOfNewVertex - (iWidthOfNewVertex / 2) + 35;
                int iOffsetYCoord = iYCoordOfNewVertex - (iHeightOfNewVertex / 2) + 35;

                //get corres child
                LAYOUT_ASSERT(childrenList.contains(iIndexOfNewVertex),
                              LayoutException("copyBackCoordinates",
                                              LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                              "childrenList",
                                              "NewVertex"));
                ChildrenIterator itrCurrentChild = childrenList.value(iIndexOfNewVertex);

                //for each vertex in child
                VertexIterator itrVertexInChild, itrEndVertexInChild;
                for(boost::tie(itrVertexInChild, itrEndVertexInChild) = vertices(*itrCurrentChild); itrVertexInChild != itrEndVertexInChild; ++itrVertexInChild)
                {
                    //get its global vertex
                    VertexDescriptor vGlobalOfVertexInChild = (*itrCurrentChild).local_to_global(*itrVertexInChild);
                    //update X coord of global vertex
                    int iXCoordOfGlobalVertex = boostGraphWrapper.getVertexCenterCoordX(vGlobalOfVertexInChild,  gRootClusteredGraph);
                    boostGraphWrapper.setVertexCenterCoordX(vGlobalOfVertexInChild,  gRootClusteredGraph, (iOffsetXCoord + iXCoordOfGlobalVertex));
                    //update Y coord of global vertex
                    int iYCoordOfGlobalVertex = boostGraphWrapper.getVertexCenterCoordY(vGlobalOfVertexInChild, gRootClusteredGraph);
                    boostGraphWrapper.setVertexCenterCoordY(vGlobalOfVertexInChild, gRootClusteredGraph, (iOffsetYCoord + iYCoordOfGlobalVertex));
                }

            }
            else //if current NewVertex is a simple node
            {
                //get corres original vertex, get its global vertex
                LAYOUT_ASSERT(originalVertexForNewVertex.contains(iIndexOfNewVertex),
                              LayoutException("copyBackCoordinates",
                                              LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER,
                                              "originalVertexForNewVertex",
                                              "NewVertex"));
                VertexDescriptor vOriginalVertex = *originalVertexForNewVertex.value(iIndexOfNewVertex);
                VertexDescriptor vGlobalOfCurrentVertex = gCurrentClusteredGraph.local_to_global(vOriginalVertex);

                //update X coord of global vertex
                boostGraphWrapper.setVertexCenterCoordX(vGlobalOfCurrentVertex, gRootClusteredGraph, iXCoordOfNewVertex);
                //update Y coord of global vertex
                boostGraphWrapper.setVertexCenterCoordY(vGlobalOfCurrentVertex, gRootClusteredGraph, iYCoordOfNewVertex);
            }
        }
    }
}
