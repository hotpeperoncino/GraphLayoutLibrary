#include "HierarchicalLayoutGenerator.h"

HierarchicalLayoutGenerator::HierarchicalLayoutGenerator()
{
    m_iTreeWidth = 0;
    m_iTreeDepth = 0;
}

void HierarchicalLayoutGenerator::test()
{
    qDebug ("Testing Hierarchical Layout Generator");
}

void HierarchicalLayoutGenerator::applyHierarchicalLayout(SubGraph &gMainGraph)
{
    boostGraphWrapper = new BoostGraphWrapper(gMainGraph);

    //create invisible root vertex
    VertexDescriptor vRootVertex = boostGraphWrapper->addVertex(gMainGraph);
    boostGraphWrapper->setVertexIsInvisible(vRootVertex , gMainGraph , true);
    boostGraphWrapper->setVertexExpandable(vRootVertex , gMainGraph , false);

    //Create list of vertex without in_edges- firstLevelVertices

    SubGraph& gRootGraph = boostGraphWrapper->getGraph();

    print_graph(gMainGraph);

    BGL_FORALL_VERTICES(vVertex , gMainGraph , SubGraph)
    {
        if(vVertex == vRootVertex
                || boostGraphWrapper->getVertexExpandable(vVertex , gMainGraph))
        {
            if(boostGraphWrapper->getVertexExpandable(vVertex , gMainGraph))
            {
                //cout << "Expandable: " << PGL_VERTEX_INDEX(vVertex , gMainGraph)
                        //<< endl;
            }
            continue;
        }

        int iInDegree = in_degree(vVertex , gMainGraph);

        if(iInDegree == 0)
        {
            //Add edges from root vertex to firstLevelVertices
            EdgeDescriptor eEdge = (boostGraphWrapper->addEdge(vRootVertex , vVertex , gMainGraph)).first;
            boostGraphWrapper->setEdgeIsInvisible(eEdge , gMainGraph , true);


            //cout << "Adding Edge " << PGL_VERTEX_INDEX(vRootVertex , gMainGraph)
                 //<< " - " << PGL_VERTEX_INDEX(vVertex , gMainGraph) << endl;
        }
    }

    //boostGraphWrapper->printGraph();
    print_graph(gMainGraph);

    BGL_FORALL_VERTICES(vertex , gMainGraph , SubGraph)
    {

        boostGraphWrapper->setVertexVisited(vertex , gMainGraph , false);
        //cout << boostGraphWrapper->getVertexVisited(vertex , gMainGraph);

    }

    //set iTreeWidth, and distanceFromRoot for each vertex and iTreeDepth
    m_iTreeWidth = setTreeWidth(vRootVertex,0,gMainGraph);

    //setting compartment size
    int iCompartmentWidth = m_iTreeWidth * (m_iTreeDepth * 3);
    if(iCompartmentWidth < (m_iTreeWidth * 50))
    {
        iCompartmentWidth = m_iTreeWidth * 50;
    }

    int iCompartmentHeight = m_iTreeDepth * (m_iTreeWidth * 3);
    if(iCompartmentHeight < (m_iTreeDepth * 50))
    {
        iCompartmentHeight = m_iTreeDepth * 50;
    }

    if(iCompartmentWidth > 10000)
    {
        iCompartmentWidth = 10000;
    }

    if(iCompartmentHeight > 10000)
    {
        iCompartmentHeight = 10000;
    }

    //cout <<" H : " <<iCompartmentHeight << " W: " << iCompartmentWidth << endl;
    m_mainCompartment.x = iCompartmentWidth / -2;
    m_mainCompartment.y = iCompartmentHeight / -2;
    m_mainCompartment.width = iCompartmentWidth;
    m_mainCompartment.height = iCompartmentHeight;


    BGL_FORALL_VERTICES(vertex , gRootGraph , SubGraph)
    {

        boostGraphWrapper->setVertexVisited(vertex , gMainGraph , false);
         //cout << boostGraphWrapper->getVertexVisited(vertex , gMainGraph);
    }

    m_iTreeWidth = boostGraphWrapper->getVertexTreeWidth(vRootVertex,gMainGraph);

    m_iHorizontalStep = m_mainCompartment.width / m_iTreeWidth;
    m_iVerticalStep = m_mainCompartment.height / m_iTreeDepth / 2;

    //Initialize iTreeLeftX of root vertex to the left x value of compartment
    boostGraphWrapper->setVertexTreeLeftX(vRootVertex , gMainGraph ,m_mainCompartment.x);

    //set x,y coordinates for every vertex
    setCoordinates(vRootVertex , gMainGraph);

    //Set compartment
    LayoutUtility layoutUtility;
    //layoutUtility.setSubgraphsCompartment(gMainGraph);

}

void HierarchicalLayoutGenerator::applyCoordinatesFromPositionAndRank(SubGraph &gMainGraph , int iVerticleStep , int iHorizontalStep)
{
    PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexHorizontalPosition , int , iHorizontalPosition , gMainGraph);
    PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexRank , int , iRank , gMainGraph);

    int iHorizontalPosition = 0;
    int iRank = 0;
    BGL_FORALL_VERTICES(vVertex , gMainGraph , SubGraph)
    {
        iHorizontalPosition = mapVertexHorizontalPosition[vVertex];
        iRank = mapVertexRank[vVertex];

        boostGraphWrapper->setVertexCenterCoordX(vVertex , gMainGraph , iHorizontalPosition * iHorizontalStep);
        boostGraphWrapper->setVertexCenterCoordY(vVertex , gMainGraph , iRank * iVerticleStep);
    }

}

int HierarchicalLayoutGenerator::setTreeWidth(VertexDescriptor &rootVertex, int iDistanceFromRoot , SubGraph& gSubgraph)
{
    int iTreeWidth = 0;

    //Check not dummy node
    if(boostGraphWrapper->getVertexExpandable(rootVertex,gSubgraph))
    {
        return iTreeWidth;
    }

        //Set iDistanceFromRoot
        boostGraphWrapper->setVertexDistanceFromRoot(rootVertex , gSubgraph ,iDistanceFromRoot);

        //Find out vertices, call setTreeWidth
        OutEdgeIterator iterOutEdge , iterOutEdgeEnd;

        for(boost::tie(iterOutEdge,iterOutEdgeEnd) = boostGraphWrapper->getOutEdges(rootVertex, gSubgraph);
            iterOutEdge != iterOutEdgeEnd;
            ++iterOutEdge)
        {
            EdgeDescriptor eOutEdge = *iterOutEdge;
            VertexDescriptor vOutVertex = target(eOutEdge , gSubgraph);

            if(boostGraphWrapper->getVertexVisited(vOutVertex , gSubgraph))
            {
                //Skip the outvertex
                continue;
            }
            else
            {
                //Make vertex visited
                boostGraphWrapper->setVertexVisited(vOutVertex,
                                                  gSubgraph,
                                                  true);
            }

            //Recursive call
            iTreeWidth += setTreeWidth(vOutVertex ,
                                       (iDistanceFromRoot + 1) ,
                                       gSubgraph);

        }

        if(iTreeWidth == 0) //leaf vertex
        {
            iTreeWidth = 1;

            if(m_iTreeDepth < iDistanceFromRoot)
            {
                m_iTreeDepth = iDistanceFromRoot;
            }

        }

        //Set TreeWidth

        boostGraphWrapper->setVertexTreeWidth(rootVertex , gSubgraph ,iTreeWidth);

        //cout << "V: " << PGL_VERTEX_INDEX(rootVertex , gSubgraph)
         //    << " Width: " << iTreeWidth
          //   << " DistanceFrmRoot: " << iDistanceFromRoot << endl;


    return iTreeWidth;
}

void HierarchicalLayoutGenerator::setCoordinates(VertexDescriptor &vRootVertex, SubGraph &gSubgraph)
{

    int iCoordX , iCoordY;

    // x = levelSpecificFlowLeft X + (center of unitSpaceNeeded * horizontalStep)

    int iTreeLeftX = boostGraphWrapper->getVertexTreeLeftX(vRootVertex , gSubgraph);
    int iHorizontalSpace = boostGraphWrapper->getVertexTreeWidth(vRootVertex , gSubgraph);

    iCoordX = iTreeLeftX + ((iHorizontalSpace / 2.0) * m_iHorizontalStep);

    // y = distanceFromRoot * verticalStep

    //int iDistanceFromRoot = boostGraphWrapper->getVertexDistanceFromRoot(vRootVertex , gSubgraph);
    //iCoordY = iDistanceFromRoot * iVerticalStep + mainCompartment.y;

    //Modification in Y coordinate calculation : for testing new hierarchical layouter
    int iDistanceFromRoot = boostGraphWrapper->getVertexRank(vRootVertex ,gSubgraph);
    iCoordY = iDistanceFromRoot * m_iVerticalStep + m_mainCompartment.y;

    //Assign iTreeLeftX to next level vertices

    int iHorizontalTreeWidthCounter = 0;

   // cout << "V: " << PGL_VERTEX_INDEX(vRootVertex , gSubgraph)
     //    << " LeftX: " << iTreeLeftX << " TreeWidth: " << iHorizontalSpace << " RootDist: "
       //     << iDistanceFromRoot << endl;

    OutEdgeIterator iterOutEdge , iterOutEdgeEnd;

    for(boost::tie(iterOutEdge,iterOutEdgeEnd) = boostGraphWrapper->getOutEdges(vRootVertex , gSubgraph);
        iterOutEdge != iterOutEdgeEnd;
        ++iterOutEdge)
    {
        EdgeDescriptor eOutEdge = *iterOutEdge;
        VertexDescriptor vOutVertex = boostGraphWrapper->getEdgeTarget(eOutEdge , gSubgraph);

        if(boostGraphWrapper->getVertexVisited(vOutVertex , gSubgraph))
        {
            //Skip the outvertex
            continue;
        }
        else
        {
            //Make vertex visited
            boostGraphWrapper->setVertexVisited(vOutVertex,
                                                gSubgraph,
                                                true);
        }

        //Calcularte iTreeLeftX for vOutVertex
        int iOutVertexTreeLeftX = iTreeLeftX + (iHorizontalTreeWidthCounter * m_iHorizontalStep);

        boostGraphWrapper->setVertexTreeLeftX(vOutVertex , gSubgraph ,iOutVertexTreeLeftX);

        //Update iHorizontalTreeWidthCounter
        iHorizontalTreeWidthCounter += boostGraphWrapper->getVertexTreeWidth(vOutVertex , gSubgraph);

        //Recursive call
        setCoordinates(vOutVertex , gSubgraph);
    }

    boostGraphWrapper->setVertexCenterCoordX(vRootVertex , gSubgraph ,iCoordX);
    boostGraphWrapper->setVertexCenterCoordY(vRootVertex , gSubgraph , iCoordY);
}
