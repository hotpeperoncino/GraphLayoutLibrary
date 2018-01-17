#include "GridBasedLayout.h"

GridBasedLayout::GridBasedLayout()
{

}

void GridBasedLayout::findCanonicalOrder(SubGraph & gInputGraph, std::vector<int> & oVertexIndexCanonicalOrder)
{
//     change graph format:
//     gInputGraph is a bidirectedS graph, whereas the Canonical Ordering algorithm accepts undirectedS graphs.
//     To map our graph to undirected one, we create a local undirected graph and copy the edges and vertices
//     in our graph to this local graph; and pass this to the algorithm.

    VertexDescriptor descVertexSource, descVertexTarget;
    EdgeIterator itrEdge, itrEdgeEnd;
    EdgeDescriptor descEdge;
    int iVertexSource, iVertexTarget, iNumVertices;
    BoostGraphWrapper graphWrapper;
    iNumVertices = boost::num_vertices(gInputGraph);
    GridBasedLayout::graph g(iNumVertices); //create a local undirected graph with same no. of vertices
    VertexOrderType oCanonicalOrder;
    LayoutUtility layoutUtility;

    // copy each edge one by one
    for(boost::tie(itrEdge, itrEdgeEnd) = edges(gInputGraph); itrEdge != itrEdgeEnd; itrEdge++)
    {
        //get its src and tgt
        descEdge = *itrEdge;
        descVertexSource = graphWrapper.getEdgeSource(descEdge, gInputGraph);
        descVertexTarget = graphWrapper.getEdgeTarget(descEdge, gInputGraph);
        //get indices of src and tgt
        iVertexSource = graphWrapper.getVertexIndex(descVertexSource);
        iVertexTarget = graphWrapper.getVertexIndex(descVertexTarget);
        //add edge between corresponding vertices in local graph
        add_edge(iVertexSource, iVertexTarget, g);
    }
    // Initialize the interior edge index
    layoutUtility.reinitializeEdgeIndices(g);

    // Test for planarity - we know it is planar, we just want to
    // compute the planar embedding as a side-effect
    typedef std::vector< graph_traits< graph >::edge_descriptor > vec_t;
    std::vector<vec_t> embedding(num_vertices(g));
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                     boyer_myrvold_params::embedding = make_iterator_property_map(embedding.begin(),
                                                                                                  get(vertex_index,
                                                                                                      g))))
    {
        //std::cout << "Input graph is planar2" << std::endl;
    }
    else
    {
        throw LayoutException("findCanonicalOrder",
                              LayoutExceptionEnum::INVALID_GRAPH_TYPE,
                              "inputGraph",
                              "non-planar");
        //std::cout << "Input graph is not planar2" << std::endl;
    }

    //find the Canonical Order
    planar_canonical_ordering(g, make_iterator_property_map(embedding.begin(), get(vertex_index, g)),
                              std::back_inserter(oCanonicalOrder));

    //copy the canonical order of indices to the supplied parameter
    for(int i = 0; i < oCanonicalOrder.size(); ++i)
        oVertexIndexCanonicalOrder.push_back(get(vertex_index, g, oCanonicalOrder[i]));
}



void GridBasedLayout::findGridBasedLayout(SubGraph & gInputGraph, std::vector<int> & oCanonicalOrder, std::vector< GridBasedLayout::Position > & oVertexPosition)
{
    int iNumVertices = boost::num_vertices( gInputGraph );
    LAYOUT_ASSERT(oCanonicalOrder.size() == iNumVertices,
                  LayoutException("findGridBasedLayout",
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  "canonicalOrder",
                                  "size of")); //size_of(canonical_order) = #nodes
    //initialize set of associated vertices such that: Associated Vertices of V = {V}
    std::vector< std::vector<VertexDescriptor> > AssociatedVertices(iNumVertices);
    for(int iCount=0; iCount < iNumVertices ; iCount++)
    {
        AssociatedVertices[iCount].push_back(vertex(iCount, gInputGraph));
    }

    //INITIALIZE for first 2 vertices-------------------------------------------------------------------------
    BoostGraphWrapper graphWrapper;
    int iTempararyIndex;
    std::vector<VertexDescriptor> oContour;
    std::vector< Position > PositionVector( iNumVertices );

    //set positions of first 2 vertices as (0,0)
    iTempararyIndex = oCanonicalOrder[0];
    PositionVector[iTempararyIndex].iCoordX = 0;
    PositionVector[iTempararyIndex].iCoordY = 0;

    iTempararyIndex = oCanonicalOrder[1];
    PositionVector[iTempararyIndex].iCoordX = 0;
    PositionVector[iTempararyIndex].iCoordY = 0;

    //initial contour contains vertices 0-1
    oContour.push_back(vertex(oCanonicalOrder[0], gInputGraph));
    oContour.push_back(vertex(oCanonicalOrder[1], gInputGraph));

    //Find the initial overlap and shift the node horizontally
    VertexDescriptor v0, v1;
    v0 = vertex(oCanonicalOrder[0], gInputGraph);
    v1 = vertex(oCanonicalOrder[1], gInputGraph);

    int iWidthOfV0 = graphWrapper.getVertexWidth(v0, gInputGraph);
    int iWidthOfV1 = graphWrapper.getVertexWidth(v1, gInputGraph);
    int iInitialOverlapLeft = std::ceil((iWidthOfV0/2) + (iWidthOfV1/2)) + SEPARATION;
    int iInitialOverlapRight = 0;
    int iInitialP = 0; //index of left neighbour of the node to be shifted (in current contour)

    this->secondShiftRight(iInitialOverlapLeft, iInitialOverlapRight, gInputGraph,
                           PositionVector, AssociatedVertices,
                           oContour, iInitialP);
    //--------------------------------------------------------------------------------------------------------

    //REPEATE for 3rd vertex in canonical vertex onward-------------------------------------------------------
    for(int k = 2; k < oCanonicalOrder.size(); ++k)
    {
        int iP, iQ, iOverlapLeft, iOverlapRight;
        iP = 0;
        iQ = 0;

        //FIND EXTREME NEIGHBOURS of kth vertex in contour
        this->findNeighbours(gInputGraph, oContour, oCanonicalOrder[k], iP, iQ);
        LAYOUT_ASSERT((0<=iP) & (iP<=iQ) & (iQ<=oContour.size()),
                      LayoutException("findGridBasedLayout",
                                      LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                      "contour",
                                      "P-Q"));

        //REARRANGE GRAPH by shifting some vertices right by 1 or 2 places
        this->shiftRight(TWO, gInputGraph, PositionVector, AssociatedVertices, oContour, iP, iQ);
        this->shiftRight(ONE, gInputGraph, PositionVector, AssociatedVertices, oContour, iP, iQ);

        //FIND POSITION of kth vertex
        this->findNewPosition(oCanonicalOrder[k], iP, iQ, gInputGraph, PositionVector, oContour);

        //FIND TOPMOST POINT under this new vertex
        int iIndexOfTopmostVertex;
        this->findTopmostVertex(iP, iQ, oContour, gInputGraph, PositionVector, iIndexOfTopmostVertex);

        //UPDATE ASSOCIATED VERTICES for kth vertex
        this->updateAssociaedVertices(AssociatedVertices, gInputGraph, oContour, oCanonicalOrder[k], iP, iQ);

        //UPDATE CONTOUR to reflect change after adding new vertex
        this->updateContour(oContour, gInputGraph, oCanonicalOrder[k], iP, iQ);

        //FIND OVERLAPS for newly added vertex
        this->findOverlaps(iP, oContour, gInputGraph, PositionVector, iOverlapLeft, iOverlapRight);

        //REARRANGE GRAPH by shifting some vertices right if overlapping
        this->secondShiftRight(iOverlapLeft, iOverlapRight, gInputGraph, PositionVector, AssociatedVertices,
                               oContour, iP);

        //Find NEW POSITION
        int iR = iP+2;
        this->findNewPosition(oCanonicalOrder[k], iP, iR, gInputGraph, PositionVector, oContour);

        //SHIFT VERTICALLY if needed
        this->shiftVertically(iIndexOfTopmostVertex, oCanonicalOrder[k], gInputGraph, PositionVector);
    }
    //---------------------------------------------------------------------------------------------------------

    oVertexPosition = PositionVector;
}

void GridBasedLayout::findNeighbours(SubGraph &gInputGraph, std::vector<VertexDescriptor> & oContour,
                    int & iNewVertexIndex, int& p, int& q)
{
    //This method goes through the contour and checks whether the current vertex in the contour
    //is adjecent to vNewVertex in gInputGraph
    bool bHasFoundNeighbour = false; //reflects whether vNewVertex has started encountering neighbours in contour
    EdgeBoolPair pairEdgeBoolVtoC, pairEdgeBoolCtoV, pairEdgeBoolUndirected;
    VertexDescriptor vNewVertex = vertex(iNewVertexIndex, gInputGraph);

    //traverse contour sequentially
    for(int iContourIndex = 0; iContourIndex < oContour.size(); ++iContourIndex)
    {
        //check whether edge exists between vNewVertex and current vertex in contour
        pairEdgeBoolVtoC = lookup_edge(vNewVertex, oContour[iContourIndex], gInputGraph);
        pairEdgeBoolCtoV = lookup_edge(oContour[iContourIndex], vNewVertex, gInputGraph);
        pairEdgeBoolUndirected = edge(vNewVertex, oContour[iContourIndex], gInputGraph);
        bool bIsEdge = false;
        bIsEdge = pairEdgeBoolVtoC.second || pairEdgeBoolCtoV.second || pairEdgeBoolUndirected.second;

        //find first and last neighbour in contour
        if( bIsEdge && !bHasFoundNeighbour) //if found first neighbour
        {
            p = iContourIndex;
            bHasFoundNeighbour = true;
            if(iContourIndex == (oContour.size() - 1)) //if end of contour, it is also the last neighbour
            {
                q = iContourIndex;
                break;
            }
            //else continue;
        }
        else if( bIsEdge && bHasFoundNeighbour) //if found another neighbour
        {
            if(iContourIndex == (oContour.size() - 1)) //if end of contour, it is the last neighbour
            {
                q = iContourIndex;
                break;
            }
            //else continue;
        }
        else if( !bIsEdge && !bHasFoundNeighbour) //if not found a neighbour yet
            continue;
        else if( !bIsEdge && bHasFoundNeighbour) //if found last neighbour
        {
            q = iContourIndex - 1;
            break;
        }
        else //invalid condition
        {
            throw LayoutException("findNeighbours",
                                  LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION,
                                  " ",
                                  " ");
            //break;
        }
    }
}


void GridBasedLayout::shiftRight(int iShiftBy, SubGraph & gInputGraph,
                                 std::vector< Position > & vecPositon,
                                 std::vector< std::vector<VertexDescriptor> > & vecAssociated,
                                 std::vector<VertexDescriptor> & oContour,
                                 int& p, int& q)
{
    //This method rearranges current layout to accomodate new vertex
    //It is called twice; first with iShiftBy = 2 and then with iShiftBy = 1
    //shifts all vertices associated to (Wq.....Wm) by 2 to their right
    //shifts all vertices associated to (Wp+1.....Wq-1) by 1 to their right

    //LAYOUT_ASSERT
    VertexDescriptor vWi;
    int iIndexOfWi, iIndexOfAssociatedVertex;

    switch (iShiftBy)
    {
    case 2:
        //for vertices (Wq.....Wm) in contour
        for(int i = q; i < oContour.size(); i++)
        {
            vWi = oContour[i];
            iIndexOfWi = get(vertex_index, gInputGraph, vWi);
            //for all of its associated vertices
            for(int j = 0; j < vecAssociated[iIndexOfWi].size(); j++)
            {
                iIndexOfAssociatedVertex = get(vertex_index, gInputGraph, vecAssociated[iIndexOfWi][j]);
                //shift by 2
                vecPositon[iIndexOfAssociatedVertex].iCoordX = vecPositon[iIndexOfAssociatedVertex].iCoordX + 2;
            }
        }
        break;
    case 1:
        //for vertices (Wp+1.....Wq-1) in contour
        for(int i = p+1; i < q; i++)
        {
            vWi = oContour[i];
            iIndexOfWi = get(vertex_index, gInputGraph, vWi);
            //for all of its associated vertices
            for(int j = 0; j < vecAssociated[iIndexOfWi].size(); j++)
            {
                iIndexOfAssociatedVertex = get(vertex_index, gInputGraph, vecAssociated[iIndexOfWi][j]);
                //shift by 1
                vecPositon[iIndexOfAssociatedVertex].iCoordX = vecPositon[iIndexOfAssociatedVertex].iCoordX + 1;
            }
        }
        break;
    default:
        //THROW
        std::cout << "Invalid value of shiftBy:" << iShiftBy << std::endl;
        break;
    }
}

void GridBasedLayout::findNewPosition(int & vNewVertexIntex, int& p, int& q, SubGraph & gInputGraph,
                                      std::vector< Position > & vecPositon,
                                      std::vector<VertexDescriptor> & oContour)
{
    //This method calculates coordinates of vNewVertex on the grid layout
    //new vertex is plotted at intersection of lines with slopes +1 and -1,
    //passing through first and last neighbours (Wp,Wq), respectively
    //new position given by formula P(Wp,Wq) = (x1-y1+x2+y2, -x1+y1+x2+y2) / 2

    //read Wp and Wq
    VertexDescriptor vWp, vWq;
    vWp = oContour[p];
    vWq = oContour[q];
    //get their indices
    int iIndexP, iIndexQ;
    iIndexP = get(vertex_index, gInputGraph, vWp);
    iIndexQ = get(vertex_index, gInputGraph, vWq);

    //read coordinates of Wp and Wq
    int iX1, iY1, iX2, iY2;
    iX1 = vecPositon[iIndexP].iCoordX;
    iY1 = vecPositon[iIndexP].iCoordY;
    iX2 = vecPositon[iIndexQ].iCoordX;
    iY2 = vecPositon[iIndexQ].iCoordY;

    //calculate coordinates of vNewVertex and update them
    vecPositon[vNewVertexIntex].iCoordX = (int)((iX1 - iY1 + iX2 + iY2) / 2);
    vecPositon[vNewVertexIntex].iCoordY = (int)((iY1 - iX1 + iX2 + iY2) / 2);
}

void GridBasedLayout::updateAssociaedVertices(std::vector< std::vector<VertexDescriptor> > & vecAssociated,
                                              SubGraph & gInputGraph,
                                              std::vector<VertexDescriptor> & oContour,
                                              int & iNewVertexIndex,
                                              int& p, int& q)
{
    //This method updates Associated Vertices' List for the new vertex
    //This is done as: Associated Vertices's List of Vk = Union of Associated Vertices' Lists of (Wp+1....Wq-1)

    //for each vertex between (Wp+1....Wq-1) in contour
    for(int iIndexInContour = p + 1; iIndexInContour < q; iIndexInContour++)
    {
        VertexDescriptor vWi = oContour[iIndexInContour];
        //get its index
        int iIndexW = get(vertex_index, gInputGraph, vWi);

        //for each vertex in Associated Vertices' List of Wi
        for(int iIndexInAssociatedList = 0; iIndexInAssociatedList < vecAssociated[iIndexW].size(); iIndexInAssociatedList++)
        {
            //copy it to Associated Vertices' List of NewVertex
            vecAssociated[iNewVertexIndex].push_back(vecAssociated[iIndexW][iIndexInAssociatedList]);
        }
    }
}

void GridBasedLayout::updateContour(std::vector<VertexDescriptor> & oContour, SubGraph & gInputGraph,
                                    int & iNewVertexIndex,
                                    int& p, int& q)
{
    //This method updates contour after adding new vertex
    //It removes all the vertices in contour inbetween Wp and Wq
    //and replaces them with vNewVertex; say Vk
    //thus contour (W1-...-Wp-...-Wq-...-Wm) becomes (W1-...-Wp-Vk-Wq-...-Wm)

    //declare temporary contour
    std::vector<VertexDescriptor> oTempararyContour;

    //push (W1-...-Wp) to temporary contour
    for(int i = 0; i <= p; i++)
        oTempararyContour.push_back(oContour[i]);
    //push NewVertex to temporary contour
    oTempararyContour.push_back(vertex(iNewVertexIndex, gInputGraph));
    //push (Wq-...-Wm) to temporary contour
    for(int i = q; i < oContour.size(); i++)
        oTempararyContour.push_back(oContour[i]);

    //copy temporary contour to original contour
    oContour = oTempararyContour;
}

void GridBasedLayout::findOverlaps(int &p, std::vector<VertexDescriptor> &oContour, SubGraph &gInputGraph,
                                   std::vector<Position> &vecPositon, int &iOverlapLeft, int &iOverlapRight)
{
    LAYOUT_ASSERT((p+2) < oContour.size(),
                  LayoutException("findOverlaps",
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  "contour",
                                  "P"));

    //Read new vertex and vertices at its left and right in current contour
    VertexDescriptor vLeftVertex, vNewVertex, vRightVertex;
    vLeftVertex = oContour[p];
    vNewVertex = oContour[p + 1];
    vRightVertex = oContour[p + 2];

    //Find distance between them and their widths--------------------------------------------------------------
    BoostGraphWrapper boostGraphWrapper;
    int iCoordXOfLeft, iCoordXOfRight,iCoordXOfNew;
    int iDistanceAtLeft, iDistanceAtRight, iWidthOfLeftVertex, iWidthOfRightVertex, iWidthOfNewVertex;

    //get centre coordinates
    iCoordXOfLeft = vecPositon[get(vertex_index, gInputGraph, vLeftVertex)].iCoordX;
    iCoordXOfNew = vecPositon[get(vertex_index, gInputGraph, vNewVertex)].iCoordX;
    iCoordXOfRight = vecPositon[get(vertex_index, gInputGraph, vRightVertex)].iCoordX;

    //get distance between centres
    iDistanceAtLeft = std::abs(iCoordXOfNew - iCoordXOfLeft);
    iDistanceAtRight = std::abs(iCoordXOfRight - iCoordXOfNew);

    //get widths
    iWidthOfLeftVertex = boostGraphWrapper.getVertexWidth(vLeftVertex, gInputGraph);
    iWidthOfNewVertex = boostGraphWrapper.getVertexWidth(vNewVertex, gInputGraph);
    iWidthOfRightVertex = boostGraphWrapper.getVertexWidth(vRightVertex, gInputGraph);
    //---------------------------------------------------------------------------------------------------------

    //If they overlap, find the amount by which they overlap---------------------------------------------------
    //get separation between centres needed to avoid overlap
    int iSeparationLeft, iSeparationRight;
    iSeparationLeft = std::ceil((iWidthOfLeftVertex / 2) + (iWidthOfNewVertex / 2));
    iSeparationRight = std::ceil((iWidthOfRightVertex / 2) + (iWidthOfNewVertex / 2));

    if(iSeparationLeft >= iDistanceAtLeft) //if NewNode overlaps with node at left
        iOverlapLeft = (iSeparationLeft - iDistanceAtLeft) + SEPARATION;
    else
        iOverlapLeft = 5;

    if(iSeparationRight >= iDistanceAtRight) //if NewNode overlaps with node at right
        iOverlapRight = (iSeparationRight - iDistanceAtRight) + SEPARATION;
    else
        iOverlapRight = 5;
    //---------------------------------------------------------------------------------------------------------
}

void GridBasedLayout::secondShiftRight(int &iOverlapLeft, int &iOverlapRight, SubGraph &gInputGraph,
                                       std::vector<Position> &vecPositon,
                                       std::vector<std::vector<VertexDescriptor> > &vecAssociated,
                                       std::vector<VertexDescriptor> &oContour, int &p)
{
    LAYOUT_ASSERT((p+2) <= oContour.size(),
                  LayoutException("secondShiftRight",
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  "contour",
                                  "P"));
    //for vertices associated to new node, shift by iOverlapLeft-----------------------------------------------
    VertexDescriptor vNewVertex = oContour[p + 1];
    int iIndexOfNewVertex = get(vertex_index, gInputGraph, vNewVertex);

    //for each vertex associated to NewVertex
    for(int iAssociatedIndex = 0; iAssociatedIndex < vecAssociated[iIndexOfNewVertex].size(); iAssociatedIndex++)
    {
        int iVertIndex = get(vertex_index, gInputGraph, vecAssociated[iIndexOfNewVertex][iAssociatedIndex]);
        //shift by iOverlapLeft
        vecPositon[iVertIndex].iCoordX = vecPositon[iVertIndex].iCoordX + iOverlapLeft;
    }
    //---------------------------------------------------------------------------------------------------------

    //for vertices associated to Wq onwords, shift by iOverlapLeft + iOverlapRight-----------------------------
    //for each vertex (Wq.....) on contour
    for(int iIndexInContour = p+2; iIndexInContour < oContour.size(); iIndexInContour++)
    {
        VertexDescriptor vVertexOnContour = oContour[iIndexInContour];
        int iContourVertIndex = get(vertex_index, gInputGraph, vVertexOnContour);

        //for each of its associated vertex
        for(int iAssociatedIndex = 0; iAssociatedIndex < vecAssociated[iContourVertIndex].size(); iAssociatedIndex++)//for associated vertices
        {
            int iVertIndex = get(vertex_index, gInputGraph, vecAssociated[iContourVertIndex][iAssociatedIndex]);
            //shift by iOverlapLeft + iOverlapRight
            vecPositon[iVertIndex].iCoordX = vecPositon[iVertIndex].iCoordX + iOverlapLeft + iOverlapRight;
        }
    }
    //---------------------------------------------------------------------------------------------------------
}

void GridBasedLayout::findTopmostVertex(int &p, int &q, std::vector<VertexDescriptor> &oContour,
                                        SubGraph &gInputGraph, std::vector<Position> &vecPositon,
                                        int &iIndexOfTopmostVertex)
{
    BoostGraphWrapper boostGrapgWrapper;
    int iHighestPoint = -2147483648; // initialize to smallest int value

    //initially, set contour[p] as topmost vertex
    iIndexOfTopmostVertex = get(vertex_index, gInputGraph, oContour[p]);

    //linearly search through contour in range p...q
    for(int iContourIndex = p; iContourIndex <= q; ++iContourIndex)
    {
        //calculate topmost point of current vertex
        int iHeightOfCurrentVertex, iYCoordOfCentreOfCurrentVertex, iTopmostPointOfCurrentVertex;
        iHeightOfCurrentVertex = boostGrapgWrapper.getVertexHeight(oContour[iContourIndex], gInputGraph);
        iYCoordOfCentreOfCurrentVertex = vecPositon[get(vertex_index, gInputGraph, oContour[iContourIndex])].iCoordY;
        iTopmostPointOfCurrentVertex = iYCoordOfCentreOfCurrentVertex + (iHeightOfCurrentVertex / 2);

        //update the topmost vertex
        if(iTopmostPointOfCurrentVertex > iHighestPoint)
        {
            iHighestPoint = iTopmostPointOfCurrentVertex;
            iIndexOfTopmostVertex = get(vertex_index, gInputGraph, oContour[iContourIndex]);
        }
        //else continue
    }
}

void GridBasedLayout::shiftVertically(int &iIndexOfTopmostVertex, int &iNewVertexIndex,
                                      SubGraph &gInputGraph, std::vector<Position> &vecPositon)
{
    BoostGraphWrapper boostGraphWrapper;

    //Find whether topmost vertex and new vertex overlap
    int iDistanceBetweenCentres, iSeparation;
    int iYCoordOfCentreOfTopmoostVertex, iYCoordOfCentreOfNewVertex;
    int iHeightOfTopmostVertex, iHeightOfNewVertex;
    VertexDescriptor vTopmostVertex = vertex(iIndexOfTopmostVertex, gInputGraph);
    VertexDescriptor vNewVertex = vertex(iNewVertexIndex, gInputGraph);

    //get coordinates of centres
    iYCoordOfCentreOfTopmoostVertex = vecPositon[iIndexOfTopmostVertex].iCoordY;
    iYCoordOfCentreOfNewVertex = vecPositon[iNewVertexIndex].iCoordY;
    //get distance between centres
    iDistanceBetweenCentres = iYCoordOfCentreOfNewVertex - iYCoordOfCentreOfTopmoostVertex;

    //get heights
    iHeightOfTopmostVertex = boostGraphWrapper.getVertexHeight(vTopmostVertex, gInputGraph);
    iHeightOfNewVertex = boostGraphWrapper.getVertexHeight(vNewVertex, gInputGraph);
    //get separation between centres needed to avoid overlap
    iSeparation = std::ceil((iHeightOfNewVertex / 2) + (iHeightOfTopmostVertex / 2));

    int iAmountOfVerticalOverlap = 5;
    if(iSeparation >= iDistanceBetweenCentres) //if there is overlap
        iAmountOfVerticalOverlap = std::abs(iSeparation - iDistanceBetweenCentres) + SEPARATION;

    //shift new vertex vertically
    vecPositon[iNewVertexIndex].iCoordY += iAmountOfVerticalOverlap;
}
