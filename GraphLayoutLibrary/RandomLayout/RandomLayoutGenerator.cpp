#include "RandomLayoutGenerator.h"

RandomLayoutGenerator::RandomLayoutGenerator()
{
}

void RandomLayoutGenerator::applyRandomLayout(SubGraph &gMainGraph)
{
    //Calculate compartment size according to number of nodes in graph
    int iNumVertices;
    iNumVertices = m_boostGraphWrapper.numVertices(gMainGraph);

    int iSideLength = iNumVertices * 30 ;
//    int iSideLength = 10000;
    if(iSideLength > 10000)
    {
        iSideLength = 10000;
    }

    m_mainCompartment.width = iSideLength;
    m_mainCompartment.height = iSideLength;
    m_mainCompartment.x = iSideLength/-2;
    m_mainCompartment.y = iSideLength/-2;

    //Create topology of compartment size

    boost::minstd_rand randGenerator;
    rectangle_topology<> rectTopology(randGenerator,
                                      m_mainCompartment.x,
                                      m_mainCompartment.y,
                                      m_mainCompartment.x + iSideLength,
                                      m_mainCompartment.x + iSideLength);

    //Apply boost random layout
    random_graph_layout(gMainGraph
                        , get(vertex_position , gMainGraph)
                        , rectTopology);

    /*Set x,y properties in graph iCoordx and iCoordY
    TODO: Later change this if coordinate representation changed */
    int iCoordX , iCoordY;
    BGL_FORALL_VERTICES(vVertex , gMainGraph , SubGraph)
    {
        iCoordX = (int)get(vertex_position, gMainGraph)[vVertex][0];
        iCoordY = (int)get(vertex_position, gMainGraph)[vVertex][1];

        m_boostGraphWrapper.setVertexCenterCoordX(vVertex , gMainGraph , iCoordX);
        m_boostGraphWrapper.setVertexCenterCoordY(vVertex , gMainGraph , iCoordY);
    }

}

void RandomLayoutGenerator::applyRandomLayoutForClusteredGraph(SubGraph & gInputGraph, SubGraph & gRootGraph)
{
    BoostGraphWrapper boostGraphWrapper;

    std::vector<int> oHeightOfChild;
    std::vector<int> oWidthOfChild;
    int iChildCount = 0;
    int iBlockCount = 0;

    //Get the partitions of  input graph-------------------------------------------------------------------------
    //partitions are made up of children and remaining vertices
    //apply random layout on these partitions recursively

    //for each child
    ChildrenIterator itrChild, itrChildEnd;
    for(boost::tie(itrChild, itrChildEnd)=gInputGraph.children(); itrChild != itrChildEnd; ++itrChild)
    {
        //recursively apply random layout to child
        this->applyRandomLayoutForClusteredGraph(*itrChild, gRootGraph);
        //read and save height and width of child
        oHeightOfChild.push_back(boostGraphWrapper.getGraphHeight(*itrChild));
        oWidthOfChild.push_back(boostGraphWrapper.getGraphWidth(*itrChild));
        //maintain count of children (say N)
        iChildCount++;
    }
    iBlockCount = iChildCount;

    //for remaining vertices
    //create a dummy graph
    std::vector<VertexIterator> oRemainingVertices;
    SubGraph gDummyGraph;
    int iInputGraphID = boostGraphWrapper.getGraphClusterID(gInputGraph);
    //for each vertex in input graph
    VertexIterator itrVert, itrVertEnd;
    for(boost::tie(itrVert, itrVertEnd)=vertices(gInputGraph); itrVert != itrVertEnd; ++itrVert)
    {
        int iVertexID = boostGraphWrapper.getVertexClusterID(*itrVert, gInputGraph);
        if(iInputGraphID == iVertexID) //if remaining vertex
        {
            add_vertex(gDummyGraph);
            oRemainingVertices.push_back(itrVert);
        }
    }
    //applyRandomLayout on these remaining vertices
    if(num_vertices(gDummyGraph) > 0)
    {
        this->applyRandomLayout(gDummyGraph);
        Reingold reingold;
        reingold.setCompartMentProps(gDummyGraph, 10);
        iBlockCount++;
    }
    //-----------------------------------------------------------------------------------------------------------

    if(iBlockCount == 0) //if input graph is empty
    {
        //add dummy vertex to input graph
        VertexDescriptor vDummyVertex = add_vertex(gInputGraph);
        boostGraphWrapper.setVertexCenterCoordX(vDummyVertex, gInputGraph, 0);
        boostGraphWrapper.setVertexCenterCoordY(vDummyVertex, gInputGraph, 0);
        boostGraphWrapper.setVertexHeight(vDummyVertex, gInputGraph, 25);
        boostGraphWrapper.setVertexWidth(vDummyVertex, gInputGraph, 25);
        boostGraphWrapper.setVertexIsInvisible(vDummyVertex, gInputGraph, true);
        //goto setArea;
        Reingold reingold;
        reingold.setCompartMentProps(gInputGraph, 10);
        return;
    }

    //Divide area of input graph into blocks---------------------------------------------------------------------
    //find max height and width among these areas
    int iMaxHeight = 0, iMaxWidth = 0;
    //max height and width among children
    for(int iChildIndex = 0; iChildIndex < iChildCount; iChildIndex++)
    {
        if(iMaxHeight < oHeightOfChild[iChildIndex])
        {
            iMaxHeight = oHeightOfChild[iChildIndex];
        }
        if(iMaxWidth < oWidthOfChild[iChildIndex])
        {
            iMaxWidth = oWidthOfChild[iChildIndex];
        }
    }
    //max height and width among all blocks
    if(num_vertices(gDummyGraph) > 0)
    {
        if(iMaxHeight < boostGraphWrapper.getGraphHeight(gDummyGraph))
        {
            iMaxHeight = boostGraphWrapper.getGraphHeight(gDummyGraph);
        }
        if(iMaxWidth < boostGraphWrapper.getGraphWidth(gDummyGraph))
        {
            iMaxWidth = boostGraphWrapper.getGraphWidth(gDummyGraph);
        }
    }

    //define area consisting of minimum (N+1) blocks, find offset of each block
    std::vector<Point> oOffsetOfBlock;
    this->calculateBlockOffsets(iBlockCount, iMaxHeight, iMaxWidth, oOffsetOfBlock);
    //-----------------------------------------------------------------------------------------------------------

    //Assign random block to each partition----------------------------------------------------------------------
    bool oIsBlockUsed[iBlockCount]; //flag to check if the block is used already
    //int iUsedBlockCount = 0;

    for(int iBlockIndex = 0; iBlockIndex < iBlockCount; ++iBlockIndex)
    {
        oIsBlockUsed[iBlockIndex] = false;
    }

    //for each child graph
    for(boost::tie(itrChild, itrChildEnd)=gInputGraph.children(); itrChild != itrChildEnd; ++itrChild)
    {
        //generate unique-random block number
        int iRandomBlockNumber;
        do
        {
            iRandomBlockNumber = 0 + (std::rand() * (iBlockCount-1))/RAND_MAX;

        }while(oIsBlockUsed[iRandomBlockNumber]);

//        //calculate its location(row, column) in the area
//        int iBlockRowIndex = iRandomBlockNumber / (int)std::sqrt(iBlockCount);
//        int iBlockColIndex = iRandomBlockNumber % (int)std::sqrt(iBlockCount);

        //for each vertex in the child graph
        for(boost::tie(itrVert, itrVertEnd)=vertices(*itrChild); itrVert != itrVertEnd; ++itrVert)
        {
            //get corres global vertex
            VertexDescriptor vGlobalOfCurrentVertex = (*itrChild).local_to_global(*itrVert);

            //read current coords
            int iCurrentX = boostGraphWrapper.getVertexCenterCoordX(vGlobalOfCurrentVertex, gRootGraph);
            int iCurrentY = boostGraphWrapper.getVertexCenterCoordY(vGlobalOfCurrentVertex, gRootGraph);


            //update coords by putting them at offset of current block
            boostGraphWrapper.setVertexCenterCoordX(vGlobalOfCurrentVertex, gRootGraph,
                                                    oOffsetOfBlock[iRandomBlockNumber].iXCoord + iCurrentX + 25);
            boostGraphWrapper.setVertexCenterCoordY(vGlobalOfCurrentVertex, gRootGraph,
                                                    oOffsetOfBlock[iRandomBlockNumber].iYCoord + iCurrentY + 25);
        }
        oIsBlockUsed[iRandomBlockNumber] = true; //mark as used
        //iUsedBlockCount++;
    }

    //for remaining vertices
    if(num_vertices(gDummyGraph) > 0)
    {
        //generate unique-random block number
        int iRandomBlockNumber;
        do
        {
            iRandomBlockNumber = 0 + (std::rand() * (iBlockCount-1))/RAND_MAX;

        }while(oIsBlockUsed[iRandomBlockNumber]);

//        //calculate its location(row, column) in the area
//        int iBlockRowIndex = iRandomBlockNumber / (int)std::sqrt(iBlockCount);
//        int iBlockColIndex = iRandomBlockNumber % (int)std::sqrt(iBlockCount);

        //for each vertex in the dummy graph
        for(boost::tie(itrVert, itrVertEnd)=vertices(gDummyGraph); itrVert != itrVertEnd; ++itrVert)
        {
            //get corresponding remaining vertex
            int iDummyVertIndex = get(vertex_index, gDummyGraph, *itrVert);
            *oRemainingVertices[iDummyVertIndex];

            //read current coords
            VertexDescriptor vCurrentVertexInDummuGraph = *itrVert;
            int iCurrentX = boostGraphWrapper.getVertexCenterCoordX(vCurrentVertexInDummuGraph, gDummyGraph);
            int iCurrentY = boostGraphWrapper.getVertexCenterCoordY(vCurrentVertexInDummuGraph, gDummyGraph);

            //get corres global vertex
            VertexDescriptor vGlobalOfCurrentVertex = (gInputGraph).local_to_global(*oRemainingVertices[iDummyVertIndex]);

            //update coords of the remaining vertex by putting it at offset of current block
            boostGraphWrapper.setVertexCenterCoordX(vGlobalOfCurrentVertex, gRootGraph,
                                                    oOffsetOfBlock[iRandomBlockNumber].iXCoord + iCurrentX + 25);
            boostGraphWrapper.setVertexCenterCoordY(vGlobalOfCurrentVertex, gRootGraph,
                                                    oOffsetOfBlock[iRandomBlockNumber].iYCoord + iCurrentY + 25);
        }
        oIsBlockUsed[iRandomBlockNumber] = true; //mark as used
    }
    //-----------------------------------------------------------------------------------------------------------

    //set area of graph------------------------------------------------------------------------------------------
    //setArea:
    Reingold reingold;
    reingold.setCompartMentProps(gInputGraph, 10);
    //-----------------------------------------------------------------------------------------------------------
}


void RandomLayoutGenerator::calculateBlockOffsets(int & iBlockCount, int iMaxHeight, int iMaxWidth,
                                                  std::vector<Point> & oOffsetOfBlock)
{
    int iRowsCols = ceil(sqrt(iBlockCount));
    for(int iRowIndex = 0; iRowIndex < iRowsCols; ++iRowIndex)
    {
        for(int iColIndex = 0; iColIndex < iRowsCols; ++iColIndex)
        {
//            oOffsetOfBlock[iRowIndex][iColIndex].iXCoord = iColIndex*iMaxWidth;
//            oOffsetOfBlock[iRowIndex][iColIndex].iYCoord = iRowIndex*iMaxHeight;
            Point offsetPoint;
            offsetPoint.iXCoord = iColIndex*iMaxWidth;
            offsetPoint.iYCoord = iRowIndex*iMaxHeight;
            oOffsetOfBlock.push_back(offsetPoint);
        }
    }
    iBlockCount = iRowsCols*iRowsCols;
}
