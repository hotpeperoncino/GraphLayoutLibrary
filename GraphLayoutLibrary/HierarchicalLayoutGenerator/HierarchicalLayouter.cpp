#include "HierarchicalLayouter.h"

// File contains starting of actual logic
// Understand this File ***IMP***

HierarchicalLayouter::HierarchicalLayouter()
{
    m_sExecutionLogFileName = FILENAME_EXECUTION_TIME_DETAILS;
    m_iFinalCrossings = 0;
}

void HierarchicalLayouter::applyHierarchicalLayout(SubGraph &gGraph, int iNodeSeparation, int iEdgeSeparation, int iLayerSeparation, int iBorderMargin)
{
    m_iNodeSeparation = iNodeSeparation;
    m_iEdgeSeparation = iEdgeSeparation;
    m_iRankSeparation = iLayerSeparation;
    m_iBorderMargin = iBorderMargin;

    applyHierarchicalLayout(gGraph);
}

void HierarchicalLayouter::applyHierarchicalLayout(SubGraph &gGraph)
{
    QString sTimeDetails = "";

    QTime tGlobalTimer;                  // Main function
    QTime tProcessTimer;


    //sTimeDetails.append("ApplyHierarchicalLayout Started");
    tGlobalTimer.start();

    //Validate gGraph

    m_gMainGraph = &gGraph;

    ////qDebug()<<"Test input00000000000000000";
    BGL_FORALL_EDGES(eEdge , *m_gMainGraph , SubGraph)
    {
        //Get source vertex of long edge to connect to the first dummy node
        VertexDescriptor vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , *m_gMainGraph);
        VertexDescriptor vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , *m_gMainGraph);

        //qDebug()<<"Edge: "<<(int)vSource <<" , "<<vTarget;
    }
    //qDebug()<<"Test input00000000000000000";

    //Set default margins if not already set

    if(m_iNodeSeparation.isSet() == false)
    {
        m_iNodeSeparation = 80;             // setting node separation = 80
    }

    if(m_iEdgeSeparation.isSet() == false)
    {
        m_iEdgeSeparation = 70;             // setting edge separation =70
    }

    if(m_iRankSeparation.isSet() == false)
    {
        m_iRankSeparation = 40;
    }

    if(m_iBorderMargin.isSet() == false)
    {
        m_iBorderMargin = 30;
    }

    //Reset counters. Initializations
    m_iNestingDepth.reset();

    int iTotalTime = 0;

    try
    {
        //Remove previous bend point entries if there are any
        BGL_FORALL_EDGES(eEdge , *m_gMainGraph , SubGraph)
        {
            (*m_gMainGraph)[eEdge].vecBendPoints.clear();
        }

        //sTimeDetails.append("\nRemoving Cycles: ");//1
        tProcessTimer.start();
        //Initial Cycle Removal
        //-Record Back Edges
        //-Recheck for cycles
        //This vector preserves the edges which are removed from graph because they creates cycles.
        VectorEdgeDescriptor vecBackEdges;
        removeCycle(gGraph , vecBackEdges);


        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));
        sTimeDetails.append(",");//Add comma for CSV

        ////qDebug()<<"Graph after cycle removal:\n";
        //print_graph(gGraph);

        //Get graphs nesting depth
        m_iNestingDepth.reset();



        //qDebug()<<"Test after cycle removal 00000000000000000";
        BGL_FORALL_EDGES(eEdge , *m_gMainGraph , SubGraph)
        {
            //Get source vertex of long edge to connect to the first dummy node
            VertexDescriptor vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , *m_gMainGraph);
            VertexDescriptor vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , *m_gMainGraph);

            //qDebug()<<"Edge: "<<(int)vSource <<" , "<<vTarget;
        }
        //qDebug()<<"Test after cycle removal 00000000000000000";


 // *************************************************************
        int iNestingDepth = getGraphNestingDepth(gGraph);   // Checking for number
        //Check if graph has only single cluster          // of clusters
        if(iNestingDepth == 0)
        {
            /*For single cluster graph change Depth to 1 because then
            *later it becomes simpler to identify the LayerRanks.
            */
            iNestingDepth = 1;
        }
        m_iNestingDepth = iNestingDepth;

        int iTotalClusters = 0;

        QQueue<SubGraph*> qSubgraphs;   // a queue of subgraphs
        qSubgraphs.enqueue(m_gMainGraph);
        SubGraph* gSubgraph1 = NULL;
        while(qSubgraphs.isEmpty() == false)
        {
            gSubgraph1 = qSubgraphs.dequeue();
            iTotalClusters++;
            ChildrenIterator iterChild , iterChildEnd;
            for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
                iterChild != iterChildEnd;
                iterChild++)
            {
                SubGraph* gChildGraph = &(*iterChild);
                qSubgraphs.enqueue(gChildGraph);
            }
        }
//***************************************************************************
        sTimeDetails.append(QString::number(iTotalClusters));
        sTimeDetails.append(",");//Add comma for CSV

        m_iRankDifferenceInLayers = m_iNestingDepth * 2 + 1;

        //sTimeDetails.append("\nCreate Nesting Graph: ");//2
        tProcessTimer.restart();

        //Nested Graph Creation
        createNestingGraph(gGraph);

        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));
        sTimeDetails.append(",");//Add comma for CSV

        ////qDebug()<<"Nesting graph:\n";
        //print_graph(gGraph);

        ////qDebug()<<"Rank Asignment:\n";

        //sTimeDetails.append("\nAssign Ranks: ");//3
        tProcessTimer.restart();

        //Initial rank assignment  // returns root vertex
        VertexDescriptor vRootVertex = m_BoostGraphWrapper.getGraphUpperBorderVertex(gGraph);

        //assign rank to nodes may be :0
        assignVertexRanks(gGraph , vRootVertex);

        //pullDownUpperBorderVertices();
        //Final Rank Assignment
        adjustVertexRanks(gGraph);

        //Pull up vertex ranks
        pullUpVertexRanks(gGraph , vRootVertex);//Commented for 42314-4
        //pullAndSpreadUpVertexRanks(gGraph , vRootVertex);

        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));
        sTimeDetails.append(",");//Add comma for CSV

        //Test ranking and upward edges
        bool bIsUpwardEdgeFound = testUpwardEdgesAndRanks(gGraph);

        LAYOUT_ASSERT(bIsUpwardEdgeFound == false , LayoutException(__FUNCTION__,LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION));

//        if(bIsUpwardEdgeFound == true)
//        {
//            ////qDebug() << "CAUTION : Upward Edge(s) found in nesting graph\n";
//            ////qDebug() << "***************************************************\n";
//        }
//        else
//        {
//            ////qDebug() << "Vertex Ranking is proper and No upward edge found. ";
//        }

        //Record unique ranks needed in splitting edges
        recordUniqueRankAndSort();

        //Production of Proper Hierarchy
        //- remove nested edges
        removeNestingEdges();//Approach 42014-1 //Change algorithm

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw;
    }

    bool bIsLayeredGraphCorrect = false;
    try
    {
        //sTimeDetails.append("\nInitialise ");
        //sTimeDetails.append(QString::number(num_vertices(*m_gMainGraph)));
        //sTimeDetails.append(" vertices LayerNode Hashset: ");//4
        tProcessTimer.restart();

        //Initialize hash of vertex mapped to corresponding LayerNode
        initHashVertexToLayerNode(gGraph);

        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));
        sTimeDetails.append(",");//Add comma for CSV

        //sTimeDetails.append("\nGenerate Nesting Tree: ");//5
        tProcessTimer.restart();

        //Create Nesting tree
        generateNestingTree();

        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));
        sTimeDetails.append(",");//Add comma for CSV

        //sTimeDetails.append("\nVertices Before Splitting: ");//6
        sTimeDetails.append(QString::number(num_vertices(*m_gMainGraph)));
        sTimeDetails.append(",");//Add comma for CSV

        //sTimeDetails.append(" , Edges Before Splitting: ");//7
        sTimeDetails.append(QString::number(num_edges(*m_gMainGraph)));
        sTimeDetails.append(",");//Add comma for CSV.

        bool bIsNestingTreeCorrect = testNestingTree();

        //Q_ASSERT_X(bIsNestingTreeCorrect , "Nesting Tree Generation" , "Incorrect Nesting Tree");
        LAYOUT_ASSERT(bIsNestingTreeCorrect , LayoutException(__FUNCTION__
                                                              , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                              , "Nesting Tree"
                                                              , "due to incorrect nesting tree generation"));


        //sTimeDetails.append("\nSplit Long Edges: ");//8
        tProcessTimer.restart();

        //Nesting tree is neccessary to Split Long Edges
        //Splitting long edges
        int iTotalLongEdges = 0;
        iTotalLongEdges = splitLongEdgesAndUpdateNestingTree();
        //iTotalLongEdges = splitLongEdgesAtAllLayerAndUpdateNestingTree();




        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));
        sTimeDetails.append(",");//Add comma for CSV

        //Total Long Edges //9
        sTimeDetails.append(QString::number(iTotalLongEdges));
        sTimeDetails.append(",");//Add comma for CSV

        //sTimeDetails.append("\nVertices After Splitting: ");//10
        sTimeDetails.append(QString::number(num_vertices(*m_gMainGraph)));
        sTimeDetails.append(",");//Add comma for CSV

        //sTimeDetails.append(" , Edges After Splitting: ");//11
        sTimeDetails.append(QString::number(num_edges(*m_gMainGraph)));
        sTimeDetails.append(",");//Add comma for CSV.

        //Check if the Nesting tree is consistent with added dummy node
        bIsNestingTreeCorrect = testNestingTree();
        //Q_ASSERT_X(bIsNestingTreeCorrect , "After splitting long edges" , "Incorrect Nesting Tree");
        LAYOUT_ASSERT(bIsNestingTreeCorrect == true , LayoutException(__FUNCTION__
                                                                      , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                                      , "Nesting Tree"
                                                                      , "after splitting of long edges"));



        //sTimeDetails.append("\nGenerate Layered Graph: ");//12
        tProcessTimer.restart();

        //Generate Layered Graph
        generateLayeredGraph();

        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));
        sTimeDetails.append(",");//Add comma for CSV.

        //Testing Layered Graph
        bIsLayeredGraphCorrect = testLayeredGraph();
        LAYOUT_ASSERT(bIsLayeredGraphCorrect == true , LayoutException(__FUNCTION__
                                                                       , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                                       , "Layered Graph"
                                                                       , "incorrect generation of layered graph"));


        //    if(bIsLayeredGraphCorrect == false)
        //    {
        //        ////qDebug() << "Layered Graph is Incorrect";
        //    }
        //    else
        //    {
        //        ////qDebug() << "Layered Graph is Correct";
        //    }

        //Set total number of layers
        m_iTotalLayers = m_mapLayeredGraph.size();

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw;
    }

    //Edge Crossing Minimisation

    //sTimeDetails.append("\nGlobal Crossing Reduction: ");//13




    //1. Global crossing reduction using BarryCenter Method
    int iIterCrossingReduction =  5;//m_iTotalLayers;

    int iCrossingCount = 0;

    int iSumPrevCrossings = 0;
    double dMeanPrevCrossings = 0.0;

    double dDeviation = INT_MAX;
    double dPrevDeviation = INT_MAX - 1;

    boost::circular_buffer<int> cbPrevCrossingCounts(4);

    iCrossingCount = getTotalCrossings();

    sTimeDetails.append(QString::number(iCrossingCount));



    try
    {
        tProcessTimer.restart();
        while(iIterCrossingReduction--)
        {
            ////qDebug() <<"-------------Crossing Reduction------------";
            globalCrossingReducion(gGraph);

            //Calculate crossings
            iCrossingCount = getTotalCrossings();


            //Calculate total of previous crossing counts
            iSumPrevCrossings = std::accumulate(cbPrevCrossingCounts.begin() , cbPrevCrossingCounts.end(), 0);


            //Calculate Previous crossings mean
            if(cbPrevCrossingCounts.size() != 0)
            {
                dMeanPrevCrossings = ((double)iSumPrevCrossings / cbPrevCrossingCounts.size());
            }
            else
            {
                dMeanPrevCrossings = 0;
            }

            //Calculate deviation of current crossing count with mean of previous crossings
            dDeviation = dMeanPrevCrossings - iCrossingCount;



            ////qDebug() << "____________________________________________";
            //If current deviation is zero means the
            //saturation state is achieved then stop
            if(dDeviation == 0 && dPrevDeviation == 0 )
            {
                ////qDebug() << ">>> Stopping Global Crossing at saturation <<<";
                ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings << " Deviation: " << dDeviation;
                break;
            }


            //store crossing
            cbPrevCrossingCounts.push_back(iCrossingCount);


            dPrevDeviation = dDeviation;


            ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings<< " Deviation: " << dDeviation ;


        }

        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(",");//Add comma for CSV.
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw;
    }


    try
    {
        //***Add dummy nodes to empty layers in leaf NestingTree nodes
        addDummyNodeToEmptyLayersRecur(m_rootNestingTreeSubgraphNode);


        //2. Local + Global crossing reduction


        bIsLayeredGraphCorrect = testLayeredGraph();
        LAYOUT_ASSERT(bIsLayeredGraphCorrect, LayoutException(__FUNCTION__
                                                         , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                         , "Layered Graph"
                                                         , "in Layouting"));

        iCrossingCount = getTotalCrossings();

        sTimeDetails.append(",");//Add comma for CSV.
        sTimeDetails.append(QString::number(iCrossingCount));



        int iIterCrossingReduction =  2;//m_iTotalLayers;

        int iCrossingCount = 0;

        int iSumPrevCrossings = 0;
        double dMeanPrevCrossings = 0.0;

        double dDeviation = 99999;
        double dPrevDeviation = 10000;

        boost::circular_buffer<int> cbPrevCrossingCounts(20);




        tProcessTimer.restart();
        while(iIterCrossingReduction--)
        {
            ////qDebug() <<"-------------LOCAL + GLOBAL Crossing Reduction------------";
            reduceEdgeCrossings(1);

            //Calculate crossings
            iCrossingCount = getTotalCrossings();


            //Calculate total of previous crossing counts
            iSumPrevCrossings = std::accumulate(cbPrevCrossingCounts.begin() , cbPrevCrossingCounts.end(), 0);


            //Calculate Previous crossings mean
            if(cbPrevCrossingCounts.size() != 0)
            {
                if(cbPrevCrossingCounts.size() > (m_iTotalLayers/2))
                {
                dMeanPrevCrossings = ((double)iSumPrevCrossings / cbPrevCrossingCounts.size());
                }
            }
            else
            {
                dMeanPrevCrossings = 0;
            }

            //Calculate deviation of current crossing count with mean of previous crossings
            dDeviation = dMeanPrevCrossings - iCrossingCount;



            ////qDebug() << "__________________LOCAL + GLOBAL__________________________";
            //If current deviation is zero means the
            //saturation state is achieved then stop
            if(dDeviation == 0 && dPrevDeviation == 0 )
            {
                        ////qDebug() << ">>> Stopping Global Crossing at saturation <<<";
                        ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings<< " Deviation: " << dDeviation;
                        break;
            }

            if(dDeviation == dPrevDeviation)
            {
                ////qDebug() << ">>> Stopping Global Crossing at saturation <<<";
                ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings<< " Deviation: " << dDeviation;
                break;
            }


            //store crossing
            cbPrevCrossingCounts.push_back(iCrossingCount);


            dPrevDeviation = dDeviation;


            ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings<< " Deviation: " << dDeviation ;


        }
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw;
    }


    try
    {
        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(",");//Add comma for CSV.
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));

        bIsLayeredGraphCorrect = testLayeredGraph();
        LAYOUT_ASSERT(bIsLayeredGraphCorrect == true
                      , LayoutException(__FUNCTION__
                                        ,LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                        ,"after crossing reduction"
                                        ,"Layered graph"));


        //Add vertical border nodes for every subgraph
        addVerticalBorderNodesForSubgraphs();

        bIsLayeredGraphCorrect = testNestingTree();
        LAYOUT_ASSERT(bIsLayeredGraphCorrect, LayoutException(__FUNCTION__
                                                         , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                         , "NestingTree"
                                                         , "in Layouting"));

        bIsLayeredGraphCorrect = testLayeredGraph();
        LAYOUT_ASSERT(bIsLayeredGraphCorrect, LayoutException(__FUNCTION__
                                                         , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                         , "Layered Graph"
                                                         , "in Layouting"));

        //Posititioning of nodes and edges- straighten out long edges as far as possible
        tProcessTimer.restart();

        setHorizontalPosition2();
        //applyXCoordinatesFromHorizontalPosition(80);

        iTotalTime += tProcessTimer.elapsed();
        sTimeDetails.append(",");//Add comma for CSV.
        sTimeDetails.append(QString::number(tProcessTimer.elapsed()));


        //Set Temporary Coordinates for lay outing of intermidiete stages of Layout Processs
        assignYCoordinates(gGraph);


        //Set compartment properties
        setSubgraphCompartmentProperties();

        //Find total crossings
        int iTotalCrossings = 0;
        iTotalCrossings = getTotalCrossings();
        //////qDebug() << "Total Crossings: " << QString::number(iTotalCrossings);

        m_iFinalCrossings = iTotalCrossings;

        sTimeDetails.append(",");//Add comma for CSV.
        sTimeDetails.append(QString::number(iTotalCrossings));

        sTimeDetails.append(",");//Add comma for CSV.
        sTimeDetails.append(QString::number(iTotalTime));

        ////qDebug() << sTimeDetails;

        if(WRITE_TIME_DETAILS_LOG)
        {
            writeLog(sTimeDetails);
        }

        //Shift to vertices center coordinate to left top
        shiftVertexCoordinateToLeftTop();

        //Restor revrsed and Long edges with bends
        restoreReversedAndLongEdgesWithBendPoints();

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::setExecutionLogFileName(QString sFileName)
{
    LAYOUT_ASSERT(sFileName.isEmpty() == false , LayoutException(__FUNCTION__
                                                                 , LayoutExceptionEnum::EMPTY_CONTAINER
                                                                 , "Setting Execution Log file name"
                                                                 , "Execution Log file name is empty"));
    m_sExecutionLogFileName = sFileName;
}

int HierarchicalLayouter::getGraphNestingDepth(SubGraph &gGraph)
{
    int iDepth = 0;
    int iMaxChildrenDepth = 0;                          // returns number of clusters (may be :P)
    int iChildDepth = 0;
    int iChildCount = 0;

    ChildrenIterator iterChild , iterChildEnd;



    for(boost::tie(iterChild , iterChildEnd) = gGraph.children();
        iterChild != iterChildEnd;
        iterChild++)
    {
        ++iChildCount;

        iChildDepth = getGraphNestingDepth(*iterChild);

        if(iMaxChildrenDepth < iChildDepth)
        {
            iMaxChildrenDepth = iChildDepth;
        }
    }


    if(iChildCount > 0)
    {
        iDepth += iMaxChildrenDepth + 1;
    }


    return iDepth;
}

void HierarchicalLayouter::createNestingGraph(SubGraph &gGraph)
{
    //First create UpperBorder Vertex then LowerBorder Vertex
    //so the index difference is 1 between border vertices of same graph
    //This will help to find UpperBorderVertex from LowerBorderVertex of same graph
    //UpperBorderVertex = LowerBorderVertex - 1 and vice versa
    VertexDescriptor vUpperBorder;
    VertexDescriptor vLowerBorder;

    try
    {
        addNestingToChildByRecur(gGraph , vUpperBorder , vLowerBorder);
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

void HierarchicalLayouter::addNestingToChildByRecur(SubGraph &gGraph, VertexDescriptor &vUpperBorder, VertexDescriptor &vLowerBorder)
{
    BoostGraphWrapper graphWrapper;
    GraphCycleHandler cycleHandler;
    bool bIsEdgeCreateCycle;

    SubGraph& gRootGraph = gGraph.root();

    //Create upper lower border nodes
    //First create UpperBorder Vertex then LowerBorder Vertex
    //so the index difference is 1 between border vertices of same graph
    //This will help to find UpperBorderVertex from LowerBorderVertex of same graph
    //UpperBorderVertex = LowerBorderVertex - 1 and vice versa

    VertexDescriptor vCurrentUpperBorderVertex = graphWrapper.addVertex(gGraph , LayoutEnum::UpperBorderNode);
    VertexDescriptor vCurrentLowerBorderVertex = graphWrapper.addVertex(gGraph , LayoutEnum::LowerBorderNode);

    vUpperBorder = vCurrentUpperBorderVertex;
    vLowerBorder = vCurrentLowerBorderVertex;

    VertexDescriptor vGlobalCurrentUpperBorder = gGraph.local_to_global(vCurrentUpperBorderVertex);
    VertexDescriptor vGlobalCurrentLowerBorder = gGraph.local_to_global(vCurrentLowerBorderVertex);


    //Testing 42314-1 //For empty graph
    //Add edge between upper to lower border vertex
    int iTotalVertices = num_vertices(gGraph);
    if(iTotalVertices == 2)
    {
        graphWrapper.addEdge(vGlobalCurrentUpperBorder , vGlobalCurrentLowerBorder , gRootGraph , LayoutEnum::NestingEdge);

    }


    VertexDescriptor vChildGraphUpperBorder , vChildGraphLowerBorder;
    //Recursive call
    ChildrenIterator iterChildGraph , iterEndChildGraph;
    for(boost::tie(iterChildGraph , iterEndChildGraph) = gGraph.children();
        iterChildGraph != iterEndChildGraph;
        iterChildGraph++)
    {
        try
        {
            addNestingToChildByRecur(*iterChildGraph , vChildGraphUpperBorder , vChildGraphLowerBorder);
        }
        catch(boost::exception& eBoostException)
        {
            throw *boost::get_error_info<errmsg_info>(eBoostException);
        }
        catch(...)
        {
            //throw LayoutException( __FUNCTION__ , ExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
            throw;
        }


        VertexDescriptor vGlobalChildUpperBorder = (*iterChildGraph).local_to_global(vChildGraphUpperBorder);
        VertexDescriptor vGlobalChildLowerBorder = (*iterChildGraph).local_to_global(vChildGraphLowerBorder);



        try
        {
            bIsEdgeCreateCycle = cycleHandler.doesEdgeCreateCycle(vGlobalCurrentUpperBorder ,
                                                                  vGlobalChildUpperBorder ,
                                                                  gRootGraph);
        }
        catch(boost::exception& eBoostException)
        {
            throw *boost::get_error_info<errmsg_info>(eBoostException);
        }
        catch(...)
        {
            //throw LayoutException( __FUNCTION__ , ExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
            throw;
        }

        if(bIsEdgeCreateCycle == false)
        {
            //Add nesting edge from current graphs upper border to its child graph's upper border
            graphWrapper.addEdge(vGlobalCurrentUpperBorder , vGlobalChildUpperBorder , gRootGraph , LayoutEnum::NestingEdge);
        }


        try
        {
            bIsEdgeCreateCycle = cycleHandler.doesEdgeCreateCycle(vGlobalChildLowerBorder ,
                                                                  vGlobalCurrentLowerBorder ,
                                                                  gRootGraph);
        }
        catch(boost::exception& eBoostException)
        {
            throw *boost::get_error_info<errmsg_info>(eBoostException);
        }
        catch(...)
        {
            //throw LayoutException( __FUNCTION__ , ExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
            throw;
        }


        if(bIsEdgeCreateCycle == false)
        {
            try
            {
                //Add nesting edge from child graph's lower border to current graph's lower border
                graphWrapper.addEdge(vGlobalChildLowerBorder , vGlobalCurrentLowerBorder ,gRootGraph , LayoutEnum::NestingEdge);

            }
            catch(boost::exception& eBoostException)
            {
                throw *boost::get_error_info<errmsg_info>(eBoostException);
            }
            catch(...)
            {
                //throw LayoutException( __FUNCTION__ , ExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
                throw;
            }

        }

    }

    //add nesting edges
    IteratorQVectorUInt iterOwnVertex , iterOwnVertexEnd;

    //cout<<endl<<(graphWrapper.getGraphId(gGraph)).toStdString()<<endl;
    for(boost::tie(iterOwnVertex , iterOwnVertexEnd) = graphWrapper.ownVerticesIter(gGraph);
        iterOwnVertex != iterOwnVertexEnd;
        iterOwnVertex++)
    {
        VertexDescriptor vCurrentVertex = *iterOwnVertex;
        try
        {
            if(graphWrapper.getVertexType( vCurrentVertex , gGraph) == LayoutEnum::GraphNode)
            {
                //cout<<*iterOwnVertex<<"--";
                //cout<<gGraph.local_to_global(vCurrentVertex)<<endl;

                //check if any new nesting edge should not create cycle
                //Add nesting edges from upper and lower border nodes to own vertices
                //Nesting edge from gGraphs UpperBorder vertex to own vertex
                bIsEdgeCreateCycle = cycleHandler.doesEdgeCreateCycle(vCurrentUpperBorderVertex ,
                                                                      vCurrentVertex ,
                                                                      gGraph);
                if(bIsEdgeCreateCycle == false)
                {

                    try
                    {
                        //Add nesting edge from current graphs upper border to its child graph's upper border
                        graphWrapper.addEdge(vCurrentUpperBorderVertex , vCurrentVertex , gGraph , LayoutEnum::NestingEdge);

                    }
                    //TODO: catch enum exception
                    catch(boost::exception& eBoostException)
                    {
                        throw *boost::get_error_info<errmsg_info>(eBoostException);
                    }
                    catch(...)
                    {
                        //throw LayoutException( __FUNCTION__ , ExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
                        throw;
                    }
                }

                try
                {
                    //Nesting edge from gGraphs LowerBorder vertex to own vertex
                    bIsEdgeCreateCycle = cycleHandler.doesEdgeCreateCycle(vCurrentVertex ,
                                                                          vCurrentLowerBorderVertex ,
                                                                          gGraph);

                }
                catch(boost::exception& eBoostException)
                {
                    throw *boost::get_error_info<errmsg_info>(eBoostException);
                }
                catch(...)
                {
                    //throw LayoutException( __FUNCTION__ , ExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
                    throw;
                }

                if(bIsEdgeCreateCycle == false)
                {
                    try
                    {
                        //Add nesting edge from own vertex to its lower border node
                        graphWrapper.addEdge(vCurrentVertex , vCurrentLowerBorderVertex, gGraph , LayoutEnum::NestingEdge);

                    }
                    catch(boost::exception& eBoostException)
                    {
                        throw *boost::get_error_info<errmsg_info>(eBoostException);
                    }
                    catch(...)
                    {
                        //throw LayoutException( __FUNCTION__ , ExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
                        throw;
                    }
                }
            }
        }
        catch(boost::exception& eBoostException)
        {
            throw *boost::get_error_info<errmsg_info>(eBoostException);
        }
        catch(...)
        {

            throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
        }



    }



}

void HierarchicalLayouter::assignVertexRanks(SubGraph &gGraph, VertexDescriptor vRootVertex)
{
    //Validate gGraph
    //Assert Nesting Depth is calculated
    //Q_ASSERT_X(m_iNestingDepth.isSet() , "Assigning Ranks" , "Graphs Nesting Depth is not set");

    LAYOUT_ASSERT(m_iNestingDepth.isSet(),LayoutException(__FUNCTION__ , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET , "NestingGraphDepth" ));

    //Traverse graph in topological order
    //Rank = max(Rank(predecessors)} + 1

    try
    {

        int iRankLevelDifference = 2 * m_iNestingDepth + 1;
        QQueue<VertexDescriptor> qRankedVertices;

        //Create Map of Vertex to Rank property - int iRank
        PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexRank , int , iRank , gGraph);

        //Assign rank to root node
        //m_BoostGraphWrapper.setVertexRank(vRootVertex , gGraph , 1);
        mapVertexRank[vRootVertex] = 1;

        //Add root vertex to RankedVertices Queue
        qRankedVertices.enqueue(vRootVertex);

        //Create external map for keeping each vertex in edges visit count
        std::size_t iTotalVertices = num_vertices(gGraph);
        std::vector<int> vecVertexInEdgeVisitedCount(iTotalVertices);

        std::fill(vecVertexInEdgeVisitedCount.begin() , vecVertexInEdgeVisitedCount.end() , 0);

        VertexDescriptor vCurrentVertex;
        VertexDescriptor vCurrentOutVertex;

        EdgeDescriptor eCurrentEdge;
        std::size_t iInEdgeCount;

        while(qRankedVertices.empty() == false)
        {
            //Deque the ranked vertex
            vCurrentVertex = qRankedVertices.dequeue();

            //Iterate out(child) vertices
            OutEdgeIterator iterOutEdge , iterOutEdgeEnd;
            for(boost::tie(iterOutEdge , iterOutEdgeEnd) = out_edges(vCurrentVertex , gGraph);
                iterOutEdge != iterOutEdgeEnd;
                iterOutEdge++)
            {
                eCurrentEdge = *iterOutEdge;
                //Get OutVertex
                vCurrentOutVertex = m_BoostGraphWrapper.getEdgeTarget(eCurrentEdge,
                                                                      gGraph);
                //Increase OutVertex's inEdgeVisited Count
                vecVertexInEdgeVisitedCount[vCurrentOutVertex] += 1;

                iInEdgeCount = vecVertexInEdgeVisitedCount[vCurrentOutVertex];

                //check if all InEdges of CurrentOutVertex are visited
                if(iInEdgeCount == in_degree(vCurrentOutVertex , gGraph))
                {
                    //Assign Rank to current OutVertex
                    InEdgeIterator iterOutVertexInEdge , iterOutVertexInEdgeEnd;

                    int iMaxPredecessorRank = 0;
                    int iPredecessorVertexRank = 0;
                    int iRank = 0;

                    VertexDescriptor vPredecessor;
                    //Checking ranks of predecesor vertices
                    for(boost::tie(iterOutVertexInEdge , iterOutVertexInEdgeEnd) = in_edges(vCurrentOutVertex , gGraph);
                        iterOutVertexInEdge != iterOutVertexInEdgeEnd;
                        iterOutVertexInEdge++)
                    {
                        vPredecessor = m_BoostGraphWrapper.getEdgeSource(*iterOutVertexInEdge , gGraph);
                        //iPredecessorVertexRank = m_BoostGraphWrapper.getVertexRank(vPredecessor , gGraph);
                        iPredecessorVertexRank = mapVertexRank[vPredecessor];

                        if(iMaxPredecessorRank < iPredecessorVertexRank)
                        {
                            iMaxPredecessorRank = iPredecessorVertexRank;
                        }
                    }

                    //Rank of vertex is greater than maximum rank of its predecessors
                    //iRank += 1;
                    LayoutEnum::NodeType enCurrentVertexType;
                    enCurrentVertexType = m_BoostGraphWrapper.getVertexType(vCurrentOutVertex , gGraph);
                    if(enCurrentVertexType == LayoutEnum::GraphNode)
                    {
                        //For GraphNode
                        //Assign Rank in multiple of 2K+1 (where K- Nesting Depth of Graph)
                        //iRank = (iMaxPredecessorRank + iRankLevelDifference); //Commented for 42314-3
                        //iRank = (iMaxPredecessorRank + (iRankLevelDifference * 2));//Added for 42314-3
                        iRank = (iMaxPredecessorRank + iRankLevelDifference); // 42414-4

                        //42414-4
                        int iMaxOutDegreeOfPredecessors = INT_MIN;
                        BGL_FORALL_INEDGES(vCurrentOutVertex , eInEdge , *m_gMainGraph , SubGraph)
                        {
                            VertexDescriptor vVertex = source(eInEdge , *m_gMainGraph);
                            int iPredOutDegree = out_degree(vVertex , *m_gMainGraph);

                            LayoutEnum::NodeType enPredecessorVertexType;
                            enPredecessorVertexType = m_BoostGraphWrapper.getVertexType(vVertex , *m_gMainGraph);

                            if(enPredecessorVertexType == LayoutEnum::UpperBorderNode)
                            {
                                iPredOutDegree = 0;
                            }

                            iMaxOutDegreeOfPredecessors = (iMaxOutDegreeOfPredecessors < iPredOutDegree) ? iPredOutDegree : iMaxOutDegreeOfPredecessors;
                        }

                        //Add rank two level below actual for vertices having parent with more than 5 out degree
                        //that will make space for dummy vertices to be added
                        //*This threshold we add makes a lot of difference in crossing reduction and straightness of edges
                        //number 5 is found to work well for many graphs (inflamation etc.)
                        if( iMaxOutDegreeOfPredecessors > 5)
                        {
                            iRank += (iRankLevelDifference*2);
                        }


                        iRank -= (iRank % iRankLevelDifference);
                    }
                    else
                    {
                        //For Upper and Lower border vertex
                        iRank = iMaxPredecessorRank + 1;
                    }

                    //cout<<"Rank Vertex: "<<vCurrentOutVertex<<" = "<<iRank<<endl;
                    //m_BoostGraphWrapper.setVertexRank(vCurrentOutVertex , gGraph , iRank);
                    mapVertexRank[vCurrentOutVertex] = iRank;

                    //Adjust UpperBorder Vertices rank value
                    //Rank(UpperBorderVertex) = MinRank{Successors(UpperBorderVertex)} - 1
                    if(enCurrentVertexType == LayoutEnum::LowerBorderNode) //51514-1 commented for - little ranking change
                    {
                        int iSuccessorRank = 0;
                        int iMinSuccessorRank = INT_MAX;

                        //UpperBorderVertex and LowerBorderVertex are added in sequence do if
                        //their index are contiguous, ther for index of UpperBorderVertex = LowerBorderVertex -1
                        VertexDescriptor vUpperBorderVertex = vCurrentOutVertex - 1;

                        //                    Q_ASSERT_X((m_BoostGraphWrapper.getVertexType(vUpperBorderVertex , gGraph) == Enum::UpperBorderNode),
                        //                               "Adjusting Upper Border Vertex Rank",
                        //                               "Wrong UpperBorder Vertex found from lower border vertex");
                        LAYOUT_ASSERT((m_BoostGraphWrapper.getVertexType(vUpperBorderVertex , gGraph) == LayoutEnum::UpperBorderNode),
                                      LayoutException(__FUNCTION__ , LayoutExceptionEnum::INVALID_OPERATION
                                                      , "Wrong UpperBorder Vertex found from lower border vertex"
                                                      , "Adjusting Upper Border Vertex Rank"));

                        //Rank(UpperBorderVertex) = MinRank{Successors(UpperBorderVertex)} - 1
                        AdjacencyIterator iterOutVertices , iterOutVerticesEnd;
                        for(boost::tie(iterOutVertices , iterOutVerticesEnd)
                            = adjacent_vertices(vUpperBorderVertex , gGraph);
                            iterOutVertices != iterOutVerticesEnd;
                            iterOutVertices++)
                        {
                            VertexDescriptor vUpperBorderSucessor = *iterOutVertices;
                            //iSuccessorRank = m_BoostGraphWrapper.getVertexRank(vUpperBorderSucessor , gGraph);
                            iSuccessorRank = mapVertexRank[vUpperBorderSucessor];

                            if(iMinSuccessorRank > iSuccessorRank)
                            {
                                //Min{Successors(UpperBorderVertex)}
                                iMinSuccessorRank = iSuccessorRank;
                            }
                        }

                        //MinRank{Successors(UpperBorderVertex)} - 1
                        --iMinSuccessorRank;


                        //m_BoostGraphWrapper.setVertexRank(vUpperBorderVertex , gGraph , iMinSuccessorRank);

                        mapVertexRank[vUpperBorderVertex] = iMinSuccessorRank;

                    }


                    //Add current OutVertex to RankedVertices
                    qRankedVertices.enqueue(vCurrentOutVertex);

                }

            }


        }
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}


void HierarchicalLayouter::adjustVertexRanks(SubGraph &gGraph)
{    
    //Validate gGraph
    //Assert Nesting Depth is calculated
    //Q_ASSERT_X(m_iNestingDepth.isSet() , "Adjusting Ranks" , "Graphs Nesting Depth is not set");
    LAYOUT_ASSERT(m_iNestingDepth.isSet() == true , LayoutException(__FUNCTION__ , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                                    , "Graph Nesting Depth" , ""));

    ////////////////
    //Traverse graph in topological order
    //Rank = max(Rank(predecessors)} + 1

    ConstantType<int> iRankLevelDifference;
    iRankLevelDifference = 2 * m_iNestingDepth + 1;

    QQueue<VertexDescriptor> qRankedVertices;

    //Create Map of Vertex to Rank property - int iRank
    PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexRank , int , iRank , gGraph);

    VertexDescriptor vConverseRoot = m_BoostGraphWrapper.getGraphLowerBorderVertex(gGraph);

    //Add root vertex to RankedVertices Queue
    qRankedVertices.enqueue(vConverseRoot);

    //Create external map for keeping each vertex in edges visit count
    std::size_t iTotalVertices = num_vertices(gGraph);
    std::vector<int> vecVertexOutEdgeVisitedCount(iTotalVertices);

    std::fill(vecVertexOutEdgeVisitedCount.begin() , vecVertexOutEdgeVisitedCount.end() , 0);

    VertexDescriptor vCurrentVertex;
    VertexDescriptor vCurrentInVertex;

    EdgeDescriptor eCurrentEdge;
    std::size_t iOutEdgeCount;

    while(qRankedVertices.empty() == false)
    {
        //Deque the ranked vertex
        vCurrentVertex = qRankedVertices.dequeue();

        //Iterate in(parent) vertices
        InEdgeIterator iterInEdge , iterInEdgeEnd;
        for(boost::tie(iterInEdge , iterInEdgeEnd) = in_edges(vCurrentVertex , gGraph);
            iterInEdge != iterInEdgeEnd;
            iterInEdge++)
        {
            eCurrentEdge = *iterInEdge;
            //Get InVertex
            vCurrentInVertex = m_BoostGraphWrapper.getEdgeSource(eCurrentEdge,
                                                                  gGraph);
            //Increase InVertex's OutEdgeVisited Count
            vecVertexOutEdgeVisitedCount[vCurrentInVertex] += 1;

            iOutEdgeCount = vecVertexOutEdgeVisitedCount[vCurrentInVertex];

            //check if all OutEdges of CurrentInVertex are visited
            if(iOutEdgeCount == out_degree(vCurrentInVertex , gGraph))
            {
                //Assign Rank to current InVertex
                OutEdgeIterator iterInVertexOutEdge , iterInVertexOutEdgeEnd;

                int iMinSuccessorRank = INT_MAX;
                int iSuccessorVertexRank = 0;
                int iRank = 0;

                VertexDescriptor vSuccessor;
                //Checking ranks of successor vertices
                for(boost::tie(iterInVertexOutEdge , iterInVertexOutEdgeEnd) = out_edges(vCurrentInVertex , gGraph);
                    iterInVertexOutEdge != iterInVertexOutEdgeEnd;
                    iterInVertexOutEdge++)
                {
                    vSuccessor = m_BoostGraphWrapper.getEdgeTarget(*iterInVertexOutEdge , gGraph);
                    //iSuccessorVertexRank = m_BoostGraphWrapper.getVertexRank(vSuccessor , gGraph);
                    iSuccessorVertexRank =mapVertexRank[vSuccessor];

                    if(iMinSuccessorRank > iSuccessorVertexRank)
                    {
                        iMinSuccessorRank = iSuccessorVertexRank;
                    }
                }

                //Rank of vertex is lesser than minimum rank of its successors
                //iRank -= 1;
                LayoutEnum::NodeType enCurrentVertexType;
                enCurrentVertexType = m_BoostGraphWrapper.getVertexType(vCurrentInVertex , gGraph);
                if(enCurrentVertexType == LayoutEnum::GraphNode)
                {
                    //For GraphNode
                    //Assign Rank in multiple of 2K+1 (where K- Nesting Depth of Graph)
                    iRank = (iMinSuccessorRank - iRankLevelDifference);
                    iRank -= (iRank % iRankLevelDifference);



                }
                else if(enCurrentVertexType == LayoutEnum::UpperBorderNode)
                {
                    //For Upper border vertex
                    iRank = iMinSuccessorRank - 1;
                }

                //int iVertexRank = m_BoostGraphWrapper.getVertexRank(vCurrentInVertex , gGraph);
                int iVertexRank = mapVertexRank[vCurrentInVertex];

                if(enCurrentVertexType  == LayoutEnum::GraphNode || enCurrentVertexType  ==  LayoutEnum::UpperBorderNode)
                {
                    //Check if vertex's current rank is lesser than new rank
                    //otherwise this process pushes the lowest node in subgraph
                    //upwards from its lower Border vertex and that again pushes
                    //subsequent vertices upwards thus creates gap between
                    //LowerBorder Vertex and the highest rank vertex in the subgraph
                    if(iRank > iVertexRank)
                    {
                        //cout<<"Rank Vertex: "<<vCurrentInVertex<<" = "<<iRank<<endl;
                        //m_BoostGraphWrapper.setVertexRank(vCurrentInVertex , gGraph , iRank);
                        mapVertexRank[vCurrentInVertex] = iRank;
                    }
                }


                //Add current OutVertex to RankedVertices
                qRankedVertices.enqueue(vCurrentInVertex);



            }

        }


    }

    //Printing ranks:
    //cout<<"Ranks:\n";
    //BGL_FORALL_VERTICES(vertex , gGraph , SubGraph)
    //{
        //print
        //cout<<"V: "<<vertex<<" -- "<<m_BoostGraphWrapper.getVertexRank(vertex , gGraph)<<endl;
    //}

}

void HierarchicalLayouter::pullUpVertexRanks(SubGraph &gGraph, VertexDescriptor vRootVertex)
{
    //Validate gGraph
    //Assert Nesting Depth is calculated
    //Q_ASSERT_X(m_iNestingDepth.isSet() , "Assigning Ranks" , "Graphs Nesting Depth is not set");
    LAYOUT_ASSERT(m_iNestingDepth.isSet() == true , LayoutException(__FUNCTION__ ,
                                                                    LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                                    , "Nesting Depth of Graph" , ""));


    //Traverse graph in topological order
    //Rank = max(Rank(predecessors)} + 1

    int iRankLevelDifference = 2 * m_iNestingDepth + 1;
    QQueue<VertexDescriptor> qRankedVertices;

    //Create Map of Vertex to Rank property - int iRank
    PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexRank , int , iRank , gGraph);

    //Add root vertex to RankedVertices Queue
    qRankedVertices.enqueue(vRootVertex);

    //Create external map for keeping each vertex in edges visit count
    std::size_t iTotalVertices = num_vertices(gGraph);
    std::vector<int> vecVertexInEdgeVisitedCount(iTotalVertices);

    std::fill(vecVertexInEdgeVisitedCount.begin() , vecVertexInEdgeVisitedCount.end() , 0);

    //Vector to correct the pull of vertex
    std::vector<int> vecVertexOldRank(iTotalVertices);

    std::fill(vecVertexOldRank.begin() , vecVertexOldRank.end() , 0);

    VertexDescriptor vCurrentVertex;
    VertexDescriptor vCurrentOutVertex;

    EdgeDescriptor eCurrentEdge;
    std::size_t iInEdgeCount;

    while(qRankedVertices.empty() == false)
    {
        //Deque the ranked vertex
        vCurrentVertex = qRankedVertices.dequeue();

        //Iterate out(child) vertices
        OutEdgeIterator iterOutEdge , iterOutEdgeEnd;
        for(boost::tie(iterOutEdge , iterOutEdgeEnd) = out_edges(vCurrentVertex , gGraph);
            iterOutEdge != iterOutEdgeEnd;
            iterOutEdge++)
        {
            eCurrentEdge = *iterOutEdge;
            //Get OutVertex
            vCurrentOutVertex = m_BoostGraphWrapper.getEdgeTarget(eCurrentEdge,
                                                                  gGraph);
            //Increase OutVertex's inEdgeVisited Count
            vecVertexInEdgeVisitedCount[vCurrentOutVertex] += 1;

            iInEdgeCount = vecVertexInEdgeVisitedCount[vCurrentOutVertex];

            //check if all InEdges of CurrentOutVertex are visited
            if(iInEdgeCount == in_degree(vCurrentOutVertex , gGraph))
            {
                //Assign Rank to current OutVertex
                InEdgeIterator iterOutVertexInEdge , iterOutVertexInEdgeEnd;

                int iMaxPredecessorRank = 0;
                int iPredecessorVertexRank = 0;
                int iRank = 0;

                VertexDescriptor vPredecessor;
                //Checking ranks of predecesor vertices
                for(boost::tie(iterOutVertexInEdge , iterOutVertexInEdgeEnd) = in_edges(vCurrentOutVertex , gGraph);
                    iterOutVertexInEdge != iterOutVertexInEdgeEnd;
                    iterOutVertexInEdge++)
                {
                    vPredecessor = m_BoostGraphWrapper.getEdgeSource(*iterOutVertexInEdge , gGraph);
                    //iPredecessorVertexRank = m_BoostGraphWrapper.getVertexRank(vPredecessor , gGraph);
                    iPredecessorVertexRank = mapVertexRank[vPredecessor];

                    if(iMaxPredecessorRank < iPredecessorVertexRank)
                    {
                        iMaxPredecessorRank = iPredecessorVertexRank;
                    }
                }

                //Rank of vertex is greater than maximum rank of its predecessors
                //iRank += 1;
                LayoutEnum::NodeType enCurrentVertexType;
                enCurrentVertexType = m_BoostGraphWrapper.getVertexType(vCurrentOutVertex , gGraph);
                if(enCurrentVertexType == LayoutEnum::GraphNode)
                {
                    //For GraphNode
                    //Assign Rank in multiple of 2K+1 (where K- Nesting Depth of Graph)
                    iRank = (iMaxPredecessorRank + iRankLevelDifference);
                    iRank -= (iRank % iRankLevelDifference);
                }
                else
                {
                    //For Upper and Lower border vertex
                    iRank = iMaxPredecessorRank + 1;
                }


                //Get previous rank value
                int iOldRank = mapVertexRank[vCurrentOutVertex];

                vecVertexOldRank[vCurrentOutVertex] = iOldRank;

                //Update rank to new value only if previous rank value is greater than
                //calculated value, thus it pulls up the position of current node
                if(iRank < iOldRank)
                {
                    //Check if the vertex is a leaf vertex, because we need to pull
                    //leaf vertex rank as up as possible for non-leaf vertices we chose
                    //a rank which is mid point of its old and new rank value
                    if(out_degree(vCurrentOutVertex , gGraph) > 1)
                    {
                        //Current vertex is a non-leaf vertex
                        if(enCurrentVertexType == LayoutEnum::GraphNode)
                        {
                            //Add the half of distance in old and new rank
                            iRank += (iOldRank - iRank)/2;

                            //Adjust rank
                            iRank -= (iRank % iRankLevelDifference);
                        }
                    }

                    mapVertexRank[vCurrentOutVertex] = iRank;
                }


                //Adjust UpperBorder Vertices rank value
                //Rank(UpperBorderVertex) = MinRank{Successors(UpperBorderVertex)} - 1
                if(enCurrentVertexType == LayoutEnum::LowerBorderNode)
                {
                    int iSuccessorRank = 0;
                    int iMinSuccessorRank = INT_MAX;

                    VertexDescriptor vUpperBorderVertex = vCurrentOutVertex - 1;

//                    Q_ASSERT_X((m_BoostGraphWrapper.getVertexType(vUpperBorderVertex , gGraph) == Enum::UpperBorderNode),
//                               "Adjusting Upper Border Vertex Rank",
//                               "Wrong UpperBorder Vertex found from lower border vertex");
                    LAYOUT_ASSERT((m_BoostGraphWrapper.getVertexType(vUpperBorderVertex , gGraph) == LayoutEnum::UpperBorderNode),
                                  LayoutException(__FUNCTION__ , LayoutExceptionEnum::INVALID_OPERATION
                                                  , "Wrong UpperBorder Vertex found from lower border vertex"
                                                  , "Adjusting Upper Border Vertex Rank"));

                    //Rank(UpperBorderVertex) = MinRank{Successors(UpperBorderVertex)} - 1
                    AdjacencyIterator iterOutVertices , iterOutVerticesEnd;
                    for(boost::tie(iterOutVertices , iterOutVerticesEnd)
                        = adjacent_vertices(vUpperBorderVertex , gGraph);
                        iterOutVertices != iterOutVerticesEnd;
                        iterOutVertices++)
                    {
                        VertexDescriptor vUpperBorderSucessor = *iterOutVertices;
                        //iSuccessorRank = m_BoostGraphWrapper.getVertexRank(vUpperBorderSucessor , gGraph);
                        iSuccessorRank = mapVertexRank[vUpperBorderSucessor];

                        if(iMinSuccessorRank > iSuccessorRank)
                        {
                            //Min{Successors(UpperBorderVertex)}
                            iMinSuccessorRank = iSuccessorRank;
                        }
                    }

                    //MinRank{Successors(UpperBorderVertex)} - 1
                    --iMinSuccessorRank;


                    //m_BoostGraphWrapper.setVertexRank(vUpperBorderVertex , gGraph , iMinSuccessorRank);

                    mapVertexRank[vUpperBorderVertex] = iMinSuccessorRank;

                }


                //Add current OutVertex to RankedVertices
                qRankedVertices.enqueue(vCurrentOutVertex);

            }

        }


    }

}

void HierarchicalLayouter::pullAndSpreadUpVertexRanks(SubGraph &gGraph, VertexDescriptor vRootVertex)
{
    //Testing 42314-4

    //Validate gGraph
    //Assert Nesting Depth is calculated
    Q_ASSERT_X(m_iNestingDepth.isSet() , "Assigning Ranks" , "Graphs Nesting Depth is not set");

    //Traverse graph in topological order
    //Rank = max(Rank(predecessors)} + 1

    int iRankLevelDifference = 2 * m_iNestingDepth + 1;
    QQueue<VertexDescriptor> qRankedVertices;

    //Create Map of Vertex to Rank property - int iRank
    PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexRank , int , iRank , gGraph);

    //Add root vertex to RankedVertices Queue
    qRankedVertices.enqueue(vRootVertex);

    //Create external map for keeping each vertex in edges visit count
    std::size_t iTotalVertices = num_vertices(gGraph);
    std::vector<int> vecVertexInEdgeVisitedCount(iTotalVertices);

    std::fill(vecVertexInEdgeVisitedCount.begin() , vecVertexInEdgeVisitedCount.end() , 0);

    //Vector to correct the pull of vertex
    std::vector<int> vecVertexOldRank(iTotalVertices);

    std::fill(vecVertexOldRank.begin() , vecVertexOldRank.end() , 0);

    VertexDescriptor vCurrentVertex;
    VertexDescriptor vCurrentOutVertex;

    EdgeDescriptor eCurrentEdge;
    std::size_t iInEdgeCount;

    while(qRankedVertices.empty() == false)
    {
        //Deque the ranked vertex
        vCurrentVertex = qRankedVertices.dequeue();

        //Iterate out(child) vertices
        OutEdgeIterator iterOutEdge , iterOutEdgeEnd;
        for(boost::tie(iterOutEdge , iterOutEdgeEnd) = out_edges(vCurrentVertex , gGraph);
            iterOutEdge != iterOutEdgeEnd;
            iterOutEdge++)
        {
            eCurrentEdge = *iterOutEdge;
            //Get OutVertex
            vCurrentOutVertex = m_BoostGraphWrapper.getEdgeTarget(eCurrentEdge,
                                                                  gGraph);
            //Increase OutVertex's inEdgeVisited Count
            vecVertexInEdgeVisitedCount[vCurrentOutVertex] += 1;

            iInEdgeCount = vecVertexInEdgeVisitedCount[vCurrentOutVertex];

            //check if all InEdges of CurrentOutVertex are visited
            if(iInEdgeCount == in_degree(vCurrentOutVertex , gGraph))
            {
                //Assign Rank to current OutVertex
                InEdgeIterator iterOutVertexInEdge , iterOutVertexInEdgeEnd;

                int iMaxPredecessorRank = 0;
                int iPredecessorVertexRank = 0;
                int iRank = 0;

                VertexDescriptor vPredecessor;
                //Checking ranks of predecesor vertices
                for(boost::tie(iterOutVertexInEdge , iterOutVertexInEdgeEnd) = in_edges(vCurrentOutVertex , gGraph);
                    iterOutVertexInEdge != iterOutVertexInEdgeEnd;
                    iterOutVertexInEdge++)
                {
                    vPredecessor = m_BoostGraphWrapper.getEdgeSource(*iterOutVertexInEdge , gGraph);
                    //iPredecessorVertexRank = m_BoostGraphWrapper.getVertexRank(vPredecessor , gGraph);
                    iPredecessorVertexRank = mapVertexRank[vPredecessor];

                    if(iMaxPredecessorRank < iPredecessorVertexRank)
                    {
                        iMaxPredecessorRank = iPredecessorVertexRank;
                    }
                }

                //Rank of vertex is greater than maximum rank of its predecessors
                //iRank += 1;



                LayoutEnum::NodeType enCurrentVertexType;
                enCurrentVertexType = m_BoostGraphWrapper.getVertexType(vCurrentOutVertex , gGraph);
                if(enCurrentVertexType == LayoutEnum::GraphNode)
                {
                    //For GraphNode
                    //Assign Rank in multiple of 2K+1 (where K- Nesting Depth of Graph)
                    //iRank = (iMaxPredecessorRank + iRankLevelDifference);
                    iRank = (iMaxPredecessorRank + iRankLevelDifference*2);
                    iRank -= (iRank % iRankLevelDifference);
                }
                else
                {
                    //For Upper and Lower border vertex
                    iRank = iMaxPredecessorRank + 1;
                }


                //Get previous rank value
                int iOldRank = mapVertexRank[vCurrentOutVertex];

                vecVertexOldRank[vCurrentOutVertex] = iOldRank;

                //Update rank to new value only if previous rank value is greater than
                //calculated value, thus it pulls up the position of current node
//                if(iRank < iOldRank)
//                {
//                    //Check if the vertex is a leaf vertex, because we need to pull
//                    //leaf vertex rank as up as possible for non-leaf vertices we chose
//                    //a rank which is mid point of its old and new rank value
//                    if(out_degree(vCurrentOutVertex , gGraph) > 1)
//                    {
//                        //Current vertex is a non-leaf vertex
//                        if(enCurrentVertexType == Enum::GraphNode)
//                        {
//                            //Add the half of distance in old and new rank
//                            iRank += (iOldRank - iRank)/2;

//                            //Adjust rank
//                            iRank -= (iRank % iRankLevelDifference);
//                        }
//                    }

//                    mapVertexRank[vCurrentOutVertex] = iRank;
//                }
                mapVertexRank[vCurrentOutVertex] = iRank;

                //Adjust UpperBorder Vertices rank value
                //Rank(UpperBorderVertex) = MinRank{Successors(UpperBorderVertex)} - 1
                if(enCurrentVertexType == LayoutEnum::LowerBorderNode)
                {
                    int iSuccessorRank = 0;
                    int iMinSuccessorRank = INT_MAX;

                    VertexDescriptor vUpperBorderVertex = vCurrentOutVertex - 1;

                    Q_ASSERT_X((m_BoostGraphWrapper.getVertexType(vUpperBorderVertex , gGraph) == LayoutEnum::UpperBorderNode),
                               "Adjusting Upper Border Vertex Rank",
                               "Wrong UpperBorder Vertex found from lower border vertex");

                    //Rank(UpperBorderVertex) = MinRank{Successors(UpperBorderVertex)} - 1
                    AdjacencyIterator iterOutVertices , iterOutVerticesEnd;
                    for(boost::tie(iterOutVertices , iterOutVerticesEnd)
                        = adjacent_vertices(vUpperBorderVertex , gGraph);
                        iterOutVertices != iterOutVerticesEnd;
                        iterOutVertices++)
                    {
                        VertexDescriptor vUpperBorderSucessor = *iterOutVertices;
                        //iSuccessorRank = m_BoostGraphWrapper.getVertexRank(vUpperBorderSucessor , gGraph);
                        iSuccessorRank = mapVertexRank[vUpperBorderSucessor];

                        if(iMinSuccessorRank > iSuccessorRank)
                        {
                            //Min{Successors(UpperBorderVertex)}
                            iMinSuccessorRank = iSuccessorRank;
                        }
                    }

                    //MinRank{Successors(UpperBorderVertex)} - 1
                    --iMinSuccessorRank;


                    //m_BoostGraphWrapper.setVertexRank(vUpperBorderVertex , gGraph , iMinSuccessorRank);

                    mapVertexRank[vUpperBorderVertex] = iMinSuccessorRank;

                }


                //Add current OutVertex to RankedVertices
                qRankedVertices.enqueue(vCurrentOutVertex);

            }

        }


    }

    //Printing ranks:
    //cout<<"Ranks:\n";
    BGL_FORALL_VERTICES(vertex , gGraph , SubGraph)
    {
        //Adjust vertex rank pull
        int iOldRank = vecVertexOldRank[vertex];
        LayoutEnum::NodeType enVertexType = m_BoostGraphWrapper.getVertexType(vertex , gGraph);
        int iRank = mapVertexRank[vertex];
        //if(iRank < iOldRank)
        {
            if(enVertexType == LayoutEnum::GraphNode)
            {
                //Check if the vertex is a leaf vertex, because we need to pull
                //leaf vertex rank as up as possible for non-leaf vertices we chose
                //a rank which is mid point of its old and new rank value
                //if(out_degree(vertex , gGraph) > 1)
                {
                    //Current vertex is a non-leaf vertex
                    //if(enVertexType == Enum::GraphNode)
                    {
                        //Add the half of distance between old and new rank into new rank
                        iRank = (iOldRank + iRank)/2;

                        //Adjust rank
                        iRank -= (iRank % iRankLevelDifference);
                    }
                }

                mapVertexRank[vertex] = iRank;
            }
        }
    }

}

void HierarchicalLayouter::recordUniqueRankAndSort()
{
    QSet<int> uniqueRanks;

    BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
    {
        int iRank = m_BoostGraphWrapper.getVertexRank(vVertex , *m_gMainGraph);

        if(uniqueRanks.contains(iRank) == false)
        {
            uniqueRanks.insert(iRank);
            m_vecSortedRanks.push_back(iRank);
        }
    }

    qSort(m_vecSortedRanks.begin() , m_vecSortedRanks.end());
}



void HierarchicalLayouter::removeNestingEdges()
{
    /* This function assumes that basic ranking is assigned to the vertices.
     * that means in a directed graph with two vertices the
     * rank value of two vertices connected must have rank
     * difference equal to 1.
     */

    //For all graph edges
    //remove Nesting Long edges having span > 1

    QQueue<EdgeDescriptor> qEdgesToRemoved;
    int iEdgeSpan =0;
    LayoutEnum::EdgeType enEdgeType;
    LayoutEnum::NodeType enSourceVertexType;
    LayoutEnum::NodeType enTargetVertexTyep;
    VertexDescriptor vSource;
    VertexDescriptor vTarget;
    BGL_FORALL_EDGES(eEdge , *m_gMainGraph , SubGraph)
    {
        //Check if edge is Long Edge i.e. edge span >1
        //iEdgeSpan = edgeSpan(eEdge , gGraph);
        //if(iEdgeSpan > 1) //Comment it for Removing all nesting edges
        {
            enEdgeType = m_BoostGraphWrapper.getEdgeType(eEdge , *m_gMainGraph);

            //If edge is nesting edge - Remove it
            if(enEdgeType == LayoutEnum::NestingEdge)
            {
                //commented for testing 42314-1
//                vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , *m_gMainGraph);
//                vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , *m_gMainGraph);

//                enSourceVertexType = m_BoostGraphWrapper.getVertexType(vSource , *m_gMainGraph);
//                enTargetVertexTyep = m_BoostGraphWrapper.getVertexType(vTarget , *m_gMainGraph);

//                if(enSourceVertexType == Enum::UpperBorderNode)
//                {
//                    if(enTargetVertexTyep != Enum::LowerBorderNode)
//                    {
//                        //Check vertex in_degree
//                        if(in_degree(vTarget , *m_gMainGraph) > 0)
//                        {
//                            //Delete nesting edge
//                            qEdgesToRemoved.enqueue(eEdge);
//                        }
//                    }
//                }
//                else if(enTargetVertexTyep == Enum::LowerBorderNode)
//                {
//                    //Check vertex outdegree
//                    if(out_degree(vSource , *m_gMainGraph) > 0)
//                    {
//                        //Delete nesting tree node
//                        qEdgesToRemoved.enqueue(eEdge);
//                    }
//                }

                //Uncommented for testing 42314-1
                //Delete nesting tree node
                qEdgesToRemoved.enqueue(eEdge);

            }

        }
    }

    //Remove edges
    EdgeDescriptor eEdge;
    while(qEdgesToRemoved.empty() == false)
    {
        eEdge = qEdgesToRemoved.dequeue();
        m_BoostGraphWrapper.removeEdge(eEdge , *m_gMainGraph);
    }
}

int HierarchicalLayouter::splitLongEdgesAndUpdateNestingTree()
{
    /* This procedure produce correct splitting of edges
     * only when basic ranking is assigned to the vertices.
     * that means in a directed graph with two vertices the
     * rank value of two vertices connected must have rank
     * difference equal to 1.
     * Assumption 2:  There are no upward edges in graph
     */


    //For all graph edges
    //Split edges having span > 1
    int iTotalLongEdges = 0;
    int iEdgeSpan = 0;
    int iDummyNodesRequired = 0;
    int iRank = 0;
    VertexDescriptor vSource = 0;
    VertexDescriptor vTarget = 0;

    VertexDescriptor vOriginalSourceVertex = 0;
    VertexDescriptor vOriginalTargetVertex = 0;

    //For storing long edge dummy node information to later to convert them into bend points
    //and restore the long edge
    QVector<VertexDescriptor>* vecDummyVertices = NULL;
    EdgeProperties* edgePropertiesRef = NULL;

    //qDebug()<<"Test splitting00000000000000000";
    BGL_FORALL_EDGES(eEdge , *m_gMainGraph , SubGraph)
    {
        vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , *m_gMainGraph);
        vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , *m_gMainGraph);

        //qDebug()<<"Edge: "<<(int)vSource <<" , "<<vTarget;
    }
    //qDebug()<<"Test splitting00000000000000000";

    LayoutEnum::EdgeType enEdgeType;
    QQueue<EdgeDescriptor> qEdgesToRemove;
    BGL_FORALL_EDGES(eEdge , *m_gMainGraph , SubGraph)
    {
        //Check if edge is Long Edge i.e. edge span >1
        iEdgeSpan = edgeSpan(eEdge , *m_gMainGraph);

        //Get source vertex of long edge to connect to the first dummy node
        vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , *m_gMainGraph);
        vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , *m_gMainGraph);
        //qDebug()<<"Current Edge: "<<(int)vSource <<" , "<<vTarget;

        //qDebug() << "Edge span: " << QString::number(iEdgeSpan);
        if(iEdgeSpan > 1)
        {
            iTotalLongEdges++;

            enEdgeType = m_BoostGraphWrapper.getEdgeType(eEdge , *m_gMainGraph);

            //If edge is graph edge - Split the edge
            if(enEdgeType == LayoutEnum::GraphEdge)
            {
                //Queue the long edge to be removed after splitting it
                qEdgesToRemove.enqueue(eEdge);

                //Split and assign ranks to the vertices
                //Calculate dummy nodes required for splitting
                iDummyNodesRequired = iEdgeSpan - 1;

                //Get source vertex of long edge to connect to the first dummy node
                vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , *m_gMainGraph);
                vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , *m_gMainGraph);

                vOriginalSourceVertex = vSource;
                vOriginalTargetVertex = vTarget;

                //qDebug()<<"Long edge: "<<(int)vOriginalSourceVertex <<" , "<<vOriginalTargetVertex;
                vecDummyVertices = new QVector<VertexDescriptor>();
                //Record edge properties to restore them later
                edgePropertiesRef = recordEdgeProperties(eEdge);


                NestingTreeSubgraphNode& subgraphNodeOfSourceVertex
                        = (hashVertexToLayerNode.value(vSource))->getParentNestingTreeSubgraphNode();
                NestingTreeSubgraphNode& subgraphNodeOfTargetVertex
                        = (hashVertexToLayerNode.value(vTarget))->getParentNestingTreeSubgraphNode();

                //Create subgraph tree nodes queue
                QQueue<NestingTreeSubgraphNode*> qNestingSubgraphNodes;
                getNestingTreeSubgraphNodesPath(subgraphNodeOfSourceVertex
                                                          , subgraphNodeOfTargetVertex
                                                          , qNestingSubgraphNodes);


                iRank = m_BoostGraphWrapper.getVertexRank(vSource , *m_gMainGraph);
                NestingTreeSubgraphNode* properSubgraphNodeRef;
                bool bRootSubgraphEncountered = false;

                /*Deque first subgraph node into properSubgraphNode, to be
                 *used as parent graph for new Dummy Vertex
                 */
                properSubgraphNodeRef = qNestingSubgraphNodes.dequeue();
                if(properSubgraphNodeRef->isRoot())
                {
                    bRootSubgraphEncountered = true;
                }

                while(iDummyNodesRequired > 0)
                {
                    //Insert dummy nodes to innermost subgraph which
                    //also contains long edge source or
                    //target vertices and satifies the following condition
                    //Rank(UpperBorderVertex) <= Rank(dummy node) <= Rank(LowerBorderVertex)
                    //Create dummy node

                    //Calculate rank for the dummy node
                    iRank += m_iRankDifferenceInLayers;

                    /*Find a nearest subgraph node of which min and max ranks
                     *can contain the Dummy Nodes rank value in their range
                     */
                    if(bRootSubgraphEncountered == false)
                    {
                        if(qNestingSubgraphNodes.isEmpty() == false)
                        {
                            while(properSubgraphNodeRef->isBetweenMinMaxRanks(iRank) == false)
                            {
                                properSubgraphNodeRef = qNestingSubgraphNodes.dequeue();
                                if(properSubgraphNodeRef->isRoot())
                                {
                                    bRootSubgraphEncountered = true;
                                }

                                if(qNestingSubgraphNodes.isEmpty())
                                {
                                    break;
                                }
                            }
                        }
                    }
                    if(bRootSubgraphEncountered == true)
                    {
                        if(qNestingSubgraphNodes.isEmpty() == false)
                        {
                            while(qNestingSubgraphNodes.first()->isBetweenMinMaxRanks(iRank))
                            {
                                properSubgraphNodeRef = qNestingSubgraphNodes.dequeue();

                                if(qNestingSubgraphNodes.isEmpty())
                                {
                                    break;
                                }
                            }
                        }
                    }

                    SubGraph &properSubgraph = properSubgraphNodeRef->getGraph();

                    vTarget = m_BoostGraphWrapper.addVertex(properSubgraph , LayoutEnum::DummyNode);


                    //Convert dummy node index from local to global index
                    vTarget = properSubgraph.local_to_global(vTarget);


                    //Record Dummy Nodes in vector
                    vecDummyVertices->push_back(vTarget);
                    ////qDebug() << "Add dummy vertex: "<<(int)vTarget;


                    //Add Long Edge Segment
                    m_BoostGraphWrapper.addEdge(vSource , vTarget ,
                                                *m_gMainGraph , LayoutEnum::LongEdgeSegment);

                    m_BoostGraphWrapper.setVertexRank(vTarget , *m_gMainGraph , iRank);


                    //Add Layer Node for new Dummy vertex
                    LayerNode * dummyVertexLayerNode = new LayerNode(*properSubgraphNodeRef , vTarget);

                    //Add new Layer Node Entry to hashVertexToLayerNode
                    hashVertexToLayerNode.insert(vTarget , dummyVertexLayerNode);

                    //Add Layer Node for new Dummy vertex's entry to the Nesting Graph
                    properSubgraphNodeRef->addLayerIdAndLayerNode(iRank ,dummyVertexLayerNode);

                    //Set current Target Dummy Vertex to be Source vertex for next dummy vertex
                    vSource = vTarget;
                    iDummyNodesRequired--;

                }
                //Connect last dummy node to the original long edge target node
                vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , *m_gMainGraph);
                m_BoostGraphWrapper.addEdge(vSource , vTarget ,
                                            *m_gMainGraph , LayoutEnum::LongEdgeSegment);
            }

            //qDebug() << "Delete edge later: " <<(int)vOriginalSourceVertex<<" , "<<(int)vOriginalTargetVertex;
            bool bLongEdgeAlreadyAdded = m_mapDeletedLongEdgesToDummyVertex.contains(VertexPair(vOriginalSourceVertex , vOriginalTargetVertex));
            if(bLongEdgeAlreadyAdded)
            {
                //qDebug() << "Duplicate Edge $$$";
            }

            //Store long edge dummy node information
            m_mapDeletedLongEdgesToDummyVertex.insertMulti(VertexPair(vOriginalSourceVertex , vOriginalTargetVertex) , vecDummyVertices);

            //6314-1 Do not add property of reversed edge to long edge deleted edge because they are already taken in map of reversed edge to their properties
            bool bIsReverseEdge = m_BoostGraphWrapper.getEdgeReversed(eEdge , *m_gMainGraph);
            if(bIsReverseEdge == false)
            {
                //qDebug() << "Add to long edge property : "<<(int)vOriginalSourceVertex<<" , "<<(int)vOriginalTargetVertex;
                m_mapDeletedEdgeProperties.insertMulti(VertexPair(vOriginalSourceVertex , vOriginalTargetVertex) , edgePropertiesRef);
            }
            else
            {
                //qDebug() << "Reversed Long Edge Found";
            }
            //qDebug()<<"---------------";
            vecDummyVertices = NULL;

        }
    }



    //Remove Long Edges from graph and record them to restore later
    while(qEdgesToRemove.isEmpty() == false)
    {
        EdgeDescriptor eEdge = qEdgesToRemove.dequeue();

        vSource = source(eEdge , *m_gMainGraph);
        vTarget = target(eEdge , *m_gMainGraph);


        m_BoostGraphWrapper.removeEdge(eEdge , *m_gMainGraph);
    }

    ////qDebug() << "Splitting done.";

    return iTotalLongEdges;
}



int HierarchicalLayouter::edgeSpan(EdgeDescriptor eEdge , SubGraph& gGraph)
{
    int iEdgeSpan = 0;

    int iSourceRank;
    int iTargetRank;

    VertexDescriptor vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , gGraph);
    VertexDescriptor vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , gGraph);

    iSourceRank = m_BoostGraphWrapper.getVertexRank(vSource , gGraph);
    iTargetRank = m_BoostGraphWrapper.getVertexRank(vTarget , gGraph);

    //Assert: source and target vertices of edge must have a valid rank value
    LAYOUT_ASSERT(iSourceRank > 0 && iTargetRank > 0, LayoutException(__FUNCTION__
                                                                      , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                                      , "Edge Vertices Rank Values"
                                                                      , "for Edge Span"));



    //Difference between Rank(source) and Rank(target)
    iEdgeSpan = std::abs(iTargetRank - iSourceRank);

    int iDifferenceInLayer;
    iDifferenceInLayer = m_iNestingDepth * 2 + 1;

    iEdgeSpan /= iDifferenceInLayer;

    return iEdgeSpan;
}

void HierarchicalLayouter::removeCycle(SubGraph &gGraph, VectorEdgeDescriptor &vectBackEdges)
{

    try
    {
        GraphCycleHandler graphCycleHandler;

        //Record Back edges
        graphCycleHandler.detectBackEdges( gGraph , vectBackEdges );

        EdgeDescriptor eBackEdge;
        VertexDescriptor vSource = 0;
        VertexDescriptor vTarget = 0;
        EdgeProperties* edgeProperties = NULL;
        //Record reversed edges, in vector of EdgeVerticesPair
        VectorEdgeDescriptor::iterator iterVecBackEdges = vectBackEdges.begin();

        //qDebug() << "Recording Back Edges: 1111111111111";
        while(iterVecBackEdges != vectBackEdges.end())
        {
            eBackEdge = *iterVecBackEdges;

            edgeProperties = recordEdgeProperties(eBackEdge);

            vSource = source(eBackEdge , *m_gMainGraph);
            vTarget = target(eBackEdge , *m_gMainGraph);

            //qDebug() << "Record: "<<(int)vSource<<" , "<<(int)vTarget;

            //m_mapReversedEdges.insertMulti(vSource , vTarget);
            m_mapReversedEdgeProperties.insertMulti(VertexPair(vSource , vTarget) , edgeProperties);

            iterVecBackEdges++;
        }

        //qDebug() << "----------------------------";
        //Reverse Back Edges
        //graphCycleHandler.reverseEdges(gGraph,vectBackEdges);
        {
            //Exchange the source and destinition vertices of edge
            VectorEdgeDescriptor::iterator iterEdges;

             //qDebug() << "Reversing Edges 11111111111111111111111";
            for(iterEdges = vectBackEdges.begin();
                iterEdges != vectBackEdges.end();
                iterEdges++)
            {
                EdgeDescriptor eEdge = *iterEdges;
                VertexDescriptor vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , *m_gMainGraph);
                VertexDescriptor vDest = m_BoostGraphWrapper.getEdgeTarget(eEdge , *m_gMainGraph);

                try{
                    //Delete edge
                    m_BoostGraphWrapper.removeEdge(vSource , vDest , *m_gMainGraph);
                    //qDebug() << "Delete: "<<(int)vSource<<" , "<<(int)vDest;
                }
                catch(boost::exception& eBoostException)
                {
                    throw *boost::get_error_info<errmsg_info>(eBoostException);
                }
                catch(...)
                {
                    throw LayoutException( __FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
                }

                //Check if edge is a self loop
                if(vSource == vDest)
                {
                    //qDebug() << "Skip: "<<(int)vSource<<" , "<<(int)vDest;
                    //Skip edge : handle these edges while restoring reversed edges ,  by adding calculated
                    //bend points
                    continue;
                }

                //Add edge
                EdgeDescriptor reversedEdge = (m_BoostGraphWrapper.addEdge(vDest , vSource , *m_gMainGraph)).first;

                //qDebug() << "Reverse add: "<<(int)vSource<<" , "<<(int)vDest;

                try{
                    //Setting reversed edge property
                    m_BoostGraphWrapper.setEdgeReversed(reversedEdge , *m_gMainGraph , true);

                    /*Record reversed edge descriptor  for each back edge. To delete the proper reversed edge
                      added for corresponding back edge
                    */
                    m_mapVertexPairToReversedEdgeDescriptor.insert(VertexPair(vSource, vDest),reversedEdge);
                }
                catch(boost::exception& eBoostException)
                {
                    throw *boost::get_error_info<errmsg_info>(eBoostException);
                }
                catch(...)
                {
                    throw LayoutException( __FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
                }

                //qDebug() << "---------";
            }
            //qDebug() << "=================================";
        }


        //Check for new cycles in graph
        VectorEdgeDescriptor vectNewBackEdges;
        graphCycleHandler.detectBackEdges(gGraph , vectNewBackEdges);
        if(vectNewBackEdges.empty() == false)
        {
            //qDebug()<<"Back edge removal has created new back edges\n";
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw;
    }

}

void HierarchicalLayouter::initHashVertexToLayerNode(SubGraph &gGraph)
{
    ////qDebug() << "Initialising Hash Vertex to Layer Node ";
    LayerNode* newLayerNode;
    BGL_FORALL_VERTICES(vVertex , gGraph , SubGraph)
    {
        newLayerNode = new LayerNode(vVertex);
        hashVertexToLayerNode.insert(vVertex , newLayerNode);
        ////qDebug() << QString::number(vVertex);

    }
    ////qDebug() << "Initialising Hash Vertex to Layer Node DONE!";
}

void HierarchicalLayouter::generateNestingTree()
{
    try
    {
        //set graph for current Nesting Tree Subgraph Node
        m_rootNestingTreeSubgraphNode.setGraph(*m_gMainGraph);

        generateNestingTreeByRecur(m_rootNestingTreeSubgraphNode , *m_gMainGraph);
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {
        throw;
    }

}

void HierarchicalLayouter::generateNestingTreeByRecur(NestingTreeSubgraphNode &nestingTreeSubgraphNodes, SubGraph &gRootGraph)
{
//    ////qDebug() << "Graph: " << (m_BoostGraphWrapper.getGraphId(nestingTreeSubgraphNodes.getGraph()));

//    if(nestingTreeSubgraphNodes.isRoot() == false)
//    {
//        ////qDebug() << " Parent: " << (m_BoostGraphWrapper.getGraphId((nestingTreeSubgraphNodes.getParent()).getGraph()));
//    }
//    //cout << endl;
//    ////qDebug() << "Vertices : ";

    IteratorQVectorUInt iterOwnVertices , iterOwnVerticesEnd;

    //Add own vertices to Nesting Tree Subgraph Node's multimap
    int iLayerRank = 0;
    VertexDescriptor vOwnVertex , vGlobalOwnVertex;
    LayerNode *layerNode;
    for(boost::tie(iterOwnVertices , iterOwnVerticesEnd)
        = m_BoostGraphWrapper.ownVerticesIter(gRootGraph);
        iterOwnVertices != iterOwnVerticesEnd;
        iterOwnVertices++)
    {
        //Get graphs own vertex
        vOwnVertex = *iterOwnVertices;

         ////qDebug() << " local: " << vOwnVertex;
        //Convert vOwnVertex's local index to global
        vGlobalOwnVertex = gRootGraph.local_to_global(vOwnVertex);

        ////qDebug() << " global: " << vGlobalOwnVertex;
        //Get layer rank of current own vertex
        iLayerRank = m_BoostGraphWrapper.getVertexRank(vOwnVertex , gRootGraph);

        //Get corresponding LayerNode for current own vertex
        layerNode = hashVertexToLayerNode.value(vGlobalOwnVertex);

        //Add curent vertex layer rank and layer node
        //to current Nesting Tree Subgaph Node
        nestingTreeSubgraphNodes.addLayerIdAndLayerNode(iLayerRank , layerNode);

        //Add parent Nesting Tree Subgaph Node entry into layer node
        layerNode->setParentNestingTreeSubgraphNode(nestingTreeSubgraphNodes);

        //Record Layer Id into Nesting Tree Subgaph Node and its parent branch
        nestingTreeSubgraphNodes.recordLayerIdRecurUp(iLayerRank);

    }

    ////qDebug() << endl << "---------------" << endl;
    //cout << "\nChild Subgraphs : ";
    //Add Child Subgraphs to nesting tree
    ChildrenIterator iterChildGraph , iterChildGraphEnd;
    for(boost::tie(iterChildGraph , iterChildGraphEnd)
        = gRootGraph.children();
        iterChildGraph != iterChildGraphEnd;
        iterChildGraph++)
    {
        NestingTreeSubgraphNode& childNestingTreeSubgraphNode
                = nestingTreeSubgraphNodes.addChildNestingTreeSubgraphNode(*iterChildGraph);

        generateNestingTreeByRecur(childNestingTreeSubgraphNode , *iterChildGraph);
    }

}

void HierarchicalLayouter::getNestingTreeSubgraphNodesPath(NestingTreeSubgraphNode& sourceSubgraphNode , NestingTreeSubgraphNode& targetSubgraphNode , QueueNestingTreeSubgraphNodesRef &qNestingTreeSubgraphNodesRef)
{
    //////qDebug() << "Generating Subgraph Path\n";

    /*Add nesting tree subgraph nodes from source subgraph node
     *till the root subgraph node in the queue
     */
    QueueNestingTreeSubgraphNodesRef qSourceSubgraphNodeToRoot;

    qSourceSubgraphNodeToRoot.enqueue(&sourceSubgraphNode);



    QSet<NestingTreeSubgraphNode*> visitedSubgraphNodes;
    visitedSubgraphNodes.insert(&sourceSubgraphNode);

    //Traverse path from source subgrph Node till the root of Nesting Tree
    while(&(qSourceSubgraphNodeToRoot.last()->getParent()) != NULL)
    {
        NestingTreeSubgraphNode& parentSubgraphNode = qSourceSubgraphNodeToRoot.last()->getParent();
        qSourceSubgraphNodeToRoot.enqueue(&parentSubgraphNode);

        //Record traversed subgraph nodes in visited nodes set
        visitedSubgraphNodes.insert(&parentSubgraphNode);
    }


    QStack<NestingTreeSubgraphNode*> stackTargetVertexSubgraphNodesToRootNode;

    //Fill stack till it finds a visited subgraph node OR the nesting tree root
    stackTargetVertexSubgraphNodesToRootNode.push(&targetSubgraphNode);
    while(&(stackTargetVertexSubgraphNodesToRootNode.top()->getParent()) != NULL)
    {
        //Check if the previously stacked Subgraph node is in visited node set or not
        if(visitedSubgraphNodes.contains(stackTargetVertexSubgraphNodesToRootNode.top())== true)
        {
            break;
        }
        NestingTreeSubgraphNode& parentSubgraphNode = stackTargetVertexSubgraphNodesToRootNode.top()->getParent();
        stackTargetVertexSubgraphNodesToRootNode.push(&parentSubgraphNode);
    }

    /*The top of the stack contains common Subgraph node from
     *Queue -qSourceSubgraphNodeToRoot and  Stack-stackTargetVertexSubgraphNodesToRootNode
     */

    //Filling final path queue - qNestingTreeSubgraphNodesRef
    while(qSourceSubgraphNodeToRoot.first() != stackTargetVertexSubgraphNodesToRootNode.top())
    {
        //////qDebug() << m_BoostGraphWrapper.getGraphId(qSourceSubgraphNodeToRoot.first()->getGraph());
        qNestingTreeSubgraphNodesRef.enqueue(qSourceSubgraphNodeToRoot.dequeue());
    }
    while(stackTargetVertexSubgraphNodesToRootNode.isEmpty() == false)
    {
        //////qDebug() << m_BoostGraphWrapper.getGraphId(stackTargetVertexSubgraphNodesToRootNode.top()->getGraph());
        qNestingTreeSubgraphNodesRef.enqueue(stackTargetVertexSubgraphNodesToRootNode.pop());
    }

    //////qDebug() << "Path calculation done.";
}

void HierarchicalLayouter::generateLayeredGraph()
{

    /*Add vertex's LayerNode to the Layer which has same LayerId as the Rank of current vertex
     */
    int iRank = 0;
    MapPositionToLayerNode *mapVertexPositionToLayerNode;

    QMap< int , int* > mapLayerIdToLayerNodeCount;
    int iHorizontalPosition;
    int * iLayerNodeCounter;
    BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
    {
        iRank = m_BoostGraphWrapper.getVertexRank(vVertex , *m_gMainGraph);

        //Check if Layer with Rank exists or not
        if(m_mapLayeredGraph.contains(iRank) == false)
        {
            //Create Layer if new Rank is encountered
            mapVertexPositionToLayerNode = new MapPositionToLayerNode();

            //Add new Layer to layeredGraph
            m_mapLayeredGraph.insert(iRank , mapVertexPositionToLayerNode);

            //Create LayerNode count map entry for new Layer
            iLayerNodeCounter = new int();
            *iLayerNodeCounter = 1;
            mapLayerIdToLayerNodeCount.insert(iRank , iLayerNodeCounter);

            iHorizontalPosition = *iLayerNodeCounter;
        }
        else
        {
            mapVertexPositionToLayerNode = m_mapLayeredGraph.value(iRank);
            iHorizontalPosition = *(mapLayerIdToLayerNodeCount.value(iRank));
        }

        //Find LayerNode for current vertex
        LayerNode *layerNodeOfCurrentVertex = hashVertexToLayerNode.value(vVertex);

        //Add LayerNode to the corresponding Layer
        mapVertexPositionToLayerNode->insert(iHorizontalPosition , layerNodeOfCurrentVertex);

        //Increment Layer Node counter for current layer
        (*(mapLayerIdToLayerNodeCount.value(iRank)))++;

        //Update LayerNode position into the Vertex Property
        m_BoostGraphWrapper.setVertexHorizontalPosition(vVertex , *m_gMainGraph , iHorizontalPosition);
    }


}

void HierarchicalLayouter::reverseLayerNodesPositions(int iLayerId)
{
    MapPositionToLayerNode *mapReversedLayer = new MapPositionToLayerNode();

    int iPositionCounter = 1;

    IteratorMapPositionToLayerNode iterLayerNode(*m_mapLayeredGraph[iLayerId]);
    iterLayerNode.toBack();


    LayerNode * layerNode = NULL;

    while(iterLayerNode.hasPrevious())
    {
        iterLayerNode.previous();

        //Add nodes to reversed layer
        mapReversedLayer->insert(iPositionCounter , iterLayerNode.value());

        //Update horizontal position
        layerNode = iterLayerNode.value();

        VertexDescriptor vVertex = layerNode->getVertex();
        m_BoostGraphWrapper.setVertexHorizontalPosition( vVertex,
                                                        *m_gMainGraph ,
                                                        iPositionCounter);


        iPositionCounter++;
    }

    //Delete old layer
    DELETE_AND_SET_NULL(m_mapLayeredGraph[iLayerId]);

    //Set new reversed layer
    m_mapLayeredGraph[iLayerId] = mapReversedLayer;
}

void HierarchicalLayouter::reverseLayeredGraphHorizontaly()
{
    try
    {
        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);
        while(iterLayer.hasNext())
        {
            iterLayer.next();
            int iKey = iterLayer.key();
            reverseLayerNodesPositions(iKey);
        }

        bool bIsProperLayeredGraph = testLayeredGraph();
        LAYOUT_ASSERT(bIsProperLayeredGraph == true
                      , LayoutException(__FUNCTION__
                                        , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                        , "in reverse leyered gaph horizontally"
                                        , "Layer"
                                        ));

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::reverseLayeredGraphVertically()
{
    try
    {
        //vector of keys
        QVectorUInt vecLayerIds;
        int iLayerId = 0;
        int iNegativeLayerId = 0;
        MapLayerIdToLayerRef reverseLayeredGraph;

        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);
        while(iterLayer.hasNext())
        {
            iterLayer.next();

            iLayerId = iterLayer.key();

            iNegativeLayerId = iLayerId * -1;

            //record layer ids to delete later
            vecLayerIds.push_back(iLayerId);

            //reversed layered graph
            reverseLayeredGraph.insert(iNegativeLayerId , iterLayer.value());

        }


        //remove items
        int iVecLayerIdSize = vecLayerIds.size();

        for(int iIndex = 0 ; iIndex < iVecLayerIdSize ; iIndex++)
        {
            //remove layer
            m_mapLayeredGraph.remove(vecLayerIds[iIndex]);
        }

        //Add reversed layers
        IteratorMapLayerIdToLayerRef iterReversedLayer(reverseLayeredGraph);
        while(iterReversedLayer.hasNext())
        {
            iterReversedLayer.next();

            //Add to layered graph
            m_mapLayeredGraph.insert(iterReversedLayer.key() ,
                                     iterReversedLayer.value());
        }

    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::restoreReversedAndLongEdgesWithBendPoints()
{
    VertexDescriptor vLongEdgeSource = 0;
    VertexDescriptor vLongEdgeTarget = 0;

    VertexDescriptor vDummyVertex = 0;

    VertexDescriptor vSource = 0;
    VertexDescriptor vTarget = 0;

    EdgeProperties* edgeProperties = NULL;

    bool bIsOriginalEdgeExist;

    LayoutEnum::NodeType enVertexType;
    LayoutEnum::EdgeType enEdgeType;

    BendPoints* newBendPoint = NULL;

    BGL_FORALL_EDGES(eEdge , *m_gMainGraph , SubGraph)
    {
        m_BoostGraphWrapper.setEdgeVisited(eEdge , *m_gMainGraph , false);
    }

    //check
//    LAYOUT_ASSERT((m_mapDeletedLongEdgesToDummyVertex.size() == (m_mapReversedEdgeProperties.size() +
//                                                                 m_mapDeletedEdgeProperties.size()))
//                  , LayoutException(__FUNCTION__
//                                    , ExceptionEnum::INCONSISTENT_DATASTRUCTURE
//                                    , "Contains duplicate entries of same edge and edge property"
//                                    , "Map of Reversed Edge to Edge Property"));



    //qDebug()<<"--------------------------------------------------**";

    //Iterat deleted long edges
    IteratorMapDeletedLongEdgeToVectorDummyVertex iterDeletedEdgeVertexPair(m_mapDeletedLongEdgesToDummyVertex);
    while(iterDeletedEdgeVertexPair.hasNext())
    {
        iterDeletedEdgeVertexPair.next();
        VertexPair pairLongEdgeVertices = iterDeletedEdgeVertexPair.key();

        vLongEdgeSource = pairLongEdgeVertices.first;
        vLongEdgeTarget = pairLongEdgeVertices.second;

        //qDebug() << "Restore long edge: "<<(int)vLongEdgeSource<<" , "<<(int)vLongEdgeTarget;

        QVector<VertexDescriptor> vecDummyVertices = *(iterDeletedEdgeVertexPair.value());

        //Check if the long edge is a reversed edge or not, by checking its
        //target and source vertex into map of Reversed Vertices

        //if(m_mapReversedEdges.contains(vLongEdgeTarget) == true)
        if(m_mapReversedEdgeProperties.contains(VertexPair(vLongEdgeTarget , vLongEdgeSource)))
        {
            //if(m_mapReversedEdges.values(vLongEdgeTarget).contains(vLongEdgeSource) == true)

            //qDebug() << "Add Reversed Edge : "<<(int)vLongEdgeTarget<<" , "<<(int)vLongEdgeSource;
            //Restore reversed edge
            EdgeDescriptor eReversedLongEdge = m_BoostGraphWrapper.addEdge(  vLongEdgeTarget
                                                                             , vLongEdgeSource
                                                                             , *m_gMainGraph).first;
            //Restore reversed edge properties
            //edgeProperties = m_mapReversedEdgeProperties[VertexPair(vLongEdgeTarget , vLongEdgeSource)];
            QList<EdgeProperties*> listProperty = m_mapReversedEdgeProperties.values(VertexPair(vLongEdgeTarget , vLongEdgeSource));

            LAYOUT_ASSERT(listProperty.isEmpty() == false, LayoutException(__FUNCTION__
                                                                           , LayoutExceptionEnum::EMPTY_CONTAINER
                                                                           , "property"
                                                                           , "Property List of Deleted Reversed Edge"));

            edgeProperties = listProperty.takeFirst();

            setEdgeProperties(eReversedLongEdge , edgeProperties);

            int iRemoved = 0;
            //Remove entry of reverse edge, as it is restored
            //m_mapReversedEdges.remove(vLongEdgeTarget , vLongEdgeSource); // Commented for checking

            //Remove properties as well //6214-1
            iRemoved = m_mapReversedEdgeProperties.remove(VertexPair(vLongEdgeTarget , vLongEdgeSource) , edgeProperties);

            LAYOUT_ASSERT(iRemoved == 1, LayoutException(__FUNCTION__
                                                         , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                         , "Contains duplicate entries of same edge and edge property"
                                                         , "Map of Reversed Edge to Edge Property"));

            if(ALLOW_BEND_POINTS == true)
            {
                //Add bend points in their reversed sequence
                QVectorIterator<VertexDescriptor> reverseIterVecDummyVertex(vecDummyVertices);
                reverseIterVecDummyVertex.toBack();
                while(reverseIterVecDummyVertex.hasPrevious())
                {
                    vDummyVertex = reverseIterVecDummyVertex.previous();

                    //The dummy vertex added for breaking the long edge makes a bend point
                    //Therefor add dummy vertex x,y coordinates as bend point
                    newBendPoint = new BendPoints();

                    newBendPoint->iCoordinateX = m_BoostGraphWrapper.getVertexCenterCoordX(vDummyVertex , *m_gMainGraph);
                    newBendPoint->iCoordinateY = m_BoostGraphWrapper.getVertexCenterCoordY(vDummyVertex , *m_gMainGraph);

                    ////qDebug()<<"add bend: "<<newBendPoint->iCoordinateX<<" , "<<newBendPoint->iCoordinateY;
                    m_BoostGraphWrapper.addBendPoint( newBendPoint , eReversedLongEdge , *m_gMainGraph);
                }
            }

        }
        else
        {

            //Restore long edge
            EdgeDescriptor eLongEdge = m_BoostGraphWrapper.addEdge(  vLongEdgeSource
                                                                     , vLongEdgeTarget
                                                                     , *m_gMainGraph).first;

            //Restore long edge properties
            //edgeProperties = m_mapDeletedEdgeProperties[VertexPair(vLongEdgeSource , vLongEdgeTarget)];

            //******((
            QList<EdgeProperties*> listProperty = m_mapDeletedEdgeProperties.values(VertexPair(vLongEdgeSource , vLongEdgeTarget));

            LAYOUT_ASSERT(listProperty.isEmpty() == false, LayoutException(__FUNCTION__
                                                                           , LayoutExceptionEnum::EMPTY_CONTAINER
                                                                           , "property"
                                                                           , "Property List of Deleted Long Edge"));

            edgeProperties = listProperty.takeFirst();

            setEdgeProperties(eLongEdge , edgeProperties);

            int iRemoved = 0;

            //Remove properties as well //6214-1
            iRemoved = m_mapDeletedEdgeProperties.remove(VertexPair(vLongEdgeSource , vLongEdgeTarget) , edgeProperties);

            LAYOUT_ASSERT(iRemoved == 1, LayoutException(__FUNCTION__
                                                         , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                         , "Contains duplicate entries of same edge and edge property"
                                                         , "Map of Long Edge to Edge Property"));
            //))********


            if(ALLOW_BEND_POINTS == true)
            {
                //Add bend points in their sequence
                QVectorIterator<VertexDescriptor> iterVecDummyVertex(vecDummyVertices);
                while(iterVecDummyVertex.hasNext())
                {
                    vDummyVertex = iterVecDummyVertex.next();

                    //The dummy vertex added for breaking the long edge makes a bend point
                    //Therefor add dummy vertex x,y coordinates as bend point
                    newBendPoint = new BendPoints();

                    int iX = m_BoostGraphWrapper.getVertexCenterCoordX(vDummyVertex , *m_gMainGraph);
                    int iY = m_BoostGraphWrapper.getVertexCenterCoordY(vDummyVertex , *m_gMainGraph);
                    newBendPoint->iCoordinateX = iX;
                    newBendPoint->iCoordinateY = iY;


                    ////qDebug()<<"v: "<<(int)vDummyVertex<<" add bend: "<<newBendPoint->iCoordinateX<<" , "<<newBendPoint->iCoordinateY;

                    m_BoostGraphWrapper.addBendPoint(newBendPoint , eLongEdge , *m_gMainGraph);
                }
            }
        }


    }

    //Till here, the reveresed edges which are long edges are restored
    //Now restore reversed edges which are remaining, if any

    //qDebug() << "Restore reverse edges which are not long edges";
    int iRemainingReverseEdges = m_mapReversedEdgeProperties.size();
    //qDebug()<<"Remainig reverse edges: "<<iRemainingReverseEdges;
    //if(m_mapReversedEdges.size() > 0)
    if(iRemainingReverseEdges > 0)
    {
        //QMapIterator<VertexDescriptor , VertexDescriptor> iterEdge(m_mapReversedEdges);

        QMapIterator<VertexPair , EdgeProperties*> iterEdge(m_mapReversedEdgeProperties);
        while(iterEdge.hasNext())
        {
            iterEdge.next();

            VertexPair pairEdge = iterEdge.key();

            vSource = pairEdge.first;
            vTarget = pairEdge.second;

            //6214-1

            if(vSource != vTarget)
            {
               //Delete reversed edge from graph
                QList<EdgeDescriptor> listEdgesToDelete = m_mapVertexPairToReversedEdgeDescriptor.values(
                            VertexPair(vSource , vTarget));

                LAYOUT_ASSERT(listEdgesToDelete.isEmpty() == false, LayoutException(__FUNCTION__
                                                                               , LayoutExceptionEnum::EMPTY_CONTAINER
                                                                               , "reversed edge"
                                                                               , "List of Reversed Edges does not contain the required edge descriptor"));


                EdgeDescriptor eEdgeToDelete = listEdgesToDelete.takeFirst();

                m_BoostGraphWrapper.removeEdge(eEdgeToDelete, *m_gMainGraph);

                //Remove edge from the m_mapVertexPairToReversedEdgeDescriptor
                m_mapVertexPairToReversedEdgeDescriptor.remove(VertexPair(vSource , vTarget) , eEdgeToDelete);

                //qDebug() << "Remove edge: "<<(int)vTarget<<" , "<<(int)vSource;
            }

            //Restore the original edge in graph
            EdgeDescriptor eReversedEdge = m_BoostGraphWrapper.addEdge(vSource , vTarget , *m_gMainGraph).first;
            //qDebug() << "Add edge: "<<(int)vSource<<" , "<<(int)vTarget;

            edgeProperties = iterEdge.value();


            setEdgeProperties(eReversedEdge , edgeProperties);

            if(vSource == vTarget)
            {
                if(ALLOW_BEND_POINTS == true)
                {
                    //Restore self loop edge with bend points
                    int iWidth = m_BoostGraphWrapper.getVertexWidth(vSource , *m_gMainGraph);
                    int iHeight = m_BoostGraphWrapper.getVertexHeight(vSource , *m_gMainGraph);

                    int iBendX = 0;
                    int iBendY = 0;

                    int iCenterCoordX = m_BoostGraphWrapper.getVertexCenterCoordX(vSource , *m_gMainGraph);
                    int iCenterCoordY = m_BoostGraphWrapper.getVertexCenterCoordY(vSource , *m_gMainGraph);

                    //Add two bends for self loop edge

                    //First bend
                    iBendX = iCenterCoordX + (iWidth / 2) + m_iBorderMargin - 2;
                    iBendY = iCenterCoordY - (iHeight / 2);

                    newBendPoint = new BendPoints();

                    newBendPoint->iCoordinateX = iBendX;
                    newBendPoint->iCoordinateY = iBendY;


                    m_BoostGraphWrapper.addBendPoint(newBendPoint , eReversedEdge , *m_gMainGraph);

                    //Second bend
                    iBendX = iCenterCoordX + (iWidth / 2);
                    iBendY = iCenterCoordY - (iHeight / 2) - (m_iBorderMargin - 2);

                    newBendPoint = new BendPoints();

                    newBendPoint->iCoordinateX = iBendX;
                    newBendPoint->iCoordinateY = iBendY;


                    m_BoostGraphWrapper.addBendPoint(newBendPoint , eReversedEdge , *m_gMainGraph);

                }

            }


        }
    }

    //qDebug()<<"--------------------------------------------------==";
}

EdgeProperties *HierarchicalLayouter::recordEdgeProperties(EdgeDescriptor &eGlobalEdge)
{
    EdgeProperties* newEdgeProperties = new EdgeProperties();

    newEdgeProperties->sId = m_BoostGraphWrapper.getEdgeId(eGlobalEdge , *m_gMainGraph);
    newEdgeProperties->bBidirectional = m_BoostGraphWrapper.getEdgeBidirectional(eGlobalEdge , *m_gMainGraph);

    return newEdgeProperties;
}

void HierarchicalLayouter::setEdgeProperties(EdgeDescriptor &eGlobalEdge, EdgeProperties *edgeProperties)
{
    m_BoostGraphWrapper.setEdgeId(eGlobalEdge , *m_gMainGraph , edgeProperties->sId);
    m_BoostGraphWrapper.setEdgeBidirectional(eGlobalEdge , *m_gMainGraph , edgeProperties->bBidirectional);
}

void HierarchicalLayouter::globalCrossingReducion(SubGraph &gMainGraph)
{
    /*BarryCenter Crossing Reduction requires at least two layers for
     *crossing reduction
     *
     *This process is done top-down and bottom-up for better results
     */

    //Top-Down Crossing Reduction using BarryCenter

    /*Assumption :
     *1. Graph has no Nesting Edges present
     *2. Graph have at least two layers
     */

    //For Top-Down pass
    int iFirstLayerId = 0;
    int iFirstGraphLayerId = 0;
    int iSecondGraphLayerId = 0;

    //For Bottom-Up pass
    int iLastLayerId = 0;
    int iLastGraphLayerId = 0;
    int iSecondLastGraphLayerId = 0;

    int iCurrentLayerId = 0;

    /*To have minimum two layers a graph must have at least one edge
     */
    if(num_edges(*m_gMainGraph) > 0)
    {
        //***********************Top-Down***********************
        //Start frm second layer to Down
        iFirstLayerId = m_mapLayeredGraph.constBegin().key();

        //find first GraphVerticesLayer type Layer Id
        iFirstGraphLayerId = getNextLayerId(iFirstLayerId ,
                                            DownDirection ,
                                            GraphVerticesLayer);

        iSecondGraphLayerId = getNextLayerId(iFirstGraphLayerId ,
                                             DownDirection ,
                                             GraphVerticesLayer);

        Q_ASSERT_X(iSecondGraphLayerId > 0 , "Global Crossing Reduction",
                   "Invalid second GraphVerticesLayer type layer id for Top-Down crossing reduction");

        int iCrossingCount = 0;

        int iSumPrevCrossings = 0;
        double dMeanPrevCrossings = 0.0;

        double dDeviation = 99999;
        double dPrevDeviation = 10000;

        boost::circular_buffer<int> cbPrevCrossingCounts(6);

        int iReapeatTopDown = m_iTotalLayers * 2;
        ////qDebug() << "Top-Down Crossing Counts:";
        while(iReapeatTopDown--)
        {
            /*Sort Layers according to BarryCenter Method, staring
         *from second GraphVerticesLayer type Layer
         */
            MapLayerIdToLayerRef::iterator iterLayerTopToDown
                    = m_mapLayeredGraph.find(iSecondGraphLayerId);

            for(;iterLayerTopToDown != m_mapLayeredGraph.end();
                iterLayerTopToDown++)
            {
                //Skip last layer
                if(iterLayerTopToDown+1 != m_mapLayeredGraph.end())
                {
                    /*Apply crossing reduction on only GraphVerticesLayers*/

                    iCurrentLayerId = iterLayerTopToDown.key();

                    if(iCurrentLayerId % m_iRankDifferenceInLayers == 0)
                    {
                        sortLayerByBarryCenters(iCurrentLayerId , DownDirection , Predecessor);
                    }
                }
            }

            //Calculate crossings
            iCrossingCount = getTotalCrossings();


            //Calculate total of previous crossing counts
            iSumPrevCrossings = std::accumulate(cbPrevCrossingCounts.begin() , cbPrevCrossingCounts.end(), 0);


            //Calculate Previous crossings mean
            if(cbPrevCrossingCounts.size() != 0)
            {
                dMeanPrevCrossings = ((double)iSumPrevCrossings / cbPrevCrossingCounts.size());
            }
            else
            {
                dMeanPrevCrossings = 0;
            }

            //Calculate deviation of current crossing count with mean of previous crossings
            dDeviation = dMeanPrevCrossings - iCrossingCount;



            //If current deviation is positive and equal to negative of previous deviation means the
            //saturation state is achieved then stop
            if(dDeviation >= 0)
            {
                if(dPrevDeviation <= 0)
                {
                    if((dDeviation + dPrevDeviation) < 0.03 &&
                           (dDeviation + dPrevDeviation) > -0.03 )
                    {
                        ////qDebug() << "*** Stopping at saturation ***";
                        ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings<< " Deviation: " << dDeviation;
                        break;
                    }
                }

            }

            //store crossing
            cbPrevCrossingCounts.push_back(iCrossingCount);


            dPrevDeviation = dDeviation;


            ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings<< " Deviation: " << dDeviation ;


        }
        //***********************Bottom-Up***********************
        //Start frm second last layer to Up
        IteratorMapLayerIdToLayerRef iterLayerReverse(m_mapLayeredGraph);

        iterLayerReverse.toBack();
        iterLayerReverse.previous();

        iLastLayerId = iterLayerReverse.key();

        //find last GraphVerticesLayer type Layer Id
        iLastGraphLayerId = getNextLayerId(iLastLayerId , UpDirection , GraphVerticesLayer);

        iSecondLastGraphLayerId = getNextLayerId(iLastGraphLayerId , UpDirection , GraphVerticesLayer);

        Q_ASSERT_X(iSecondLastGraphLayerId > 0 , "Global Crossing Reduction",
                   "Invalid second last GraphVerticesLayer type layer id for Bottom-Up crossing reduction");


        int iRepeatBottomUp = m_iTotalLayers *  2;

        ////qDebug() << "Bottom-Up Crossing Counts:";
        while(iRepeatBottomUp--)
        {
         /*Sort Layers according to BarryCenter Method, staring
         *from second last GraphVerticesLayer type Layer towards first layer
         */
            MapLayerIdToLayerRef::iterator iterLayerBottomToUp
                    = m_mapLayeredGraph.find(iSecondLastGraphLayerId);

            for(;(iterLayerBottomToUp) != m_mapLayeredGraph.begin();
                iterLayerBottomToUp--)
            {
                /*Apply crossing reduction on only GraphVerticesLayers
             */
                iCurrentLayerId = iterLayerBottomToUp.key();

                if(iCurrentLayerId % m_iRankDifferenceInLayers == 0)
                {
                    sortLayerByBarryCenters(iCurrentLayerId , UpDirection , Succesor);
                }
            }

//            for(;true;
//                iterLayerBottomToUp--)
//            {
//                /*Apply crossing reduction on only GraphVerticesLayers
//             */
//                iCurrentLayerId = iterLayerBottomToUp.key();

//                if(iCurrentLayerId % m_iRankDifferenceInLayers == 0)
//                {
//                    sortLayerByBarryCenters(iCurrentLayerId , UpDirection , Succesor);
//                }

//                if((iterLayerBottomToUp) != m_mapLayeredGraph.begin())
//                {
//                    break;
//                }
//            }

            //Calculate crossings
            iCrossingCount = getTotalCrossings();


            //Calculate mean or averageof previous crossing counts
            iSumPrevCrossings = std::accumulate(cbPrevCrossingCounts.begin() , cbPrevCrossingCounts.end(), 0);

            if(cbPrevCrossingCounts.size() != 0)
            {
                dMeanPrevCrossings = ((double)iSumPrevCrossings / cbPrevCrossingCounts.size());
            }
            else
            {
                dMeanPrevCrossings = 0;
            }

            dDeviation = dMeanPrevCrossings - iCrossingCount;

            //If current deviation is positive and equal to negative of previous deviation means the
            //saturation state is achieved then stop
            if(dDeviation >= 0)
            {
                //Only if previous deviation is negative
                if(dPrevDeviation <= 0)
                {
                    //Current deviation is closer to negative previous deviation
                    if((dDeviation + dPrevDeviation) < 0.09 &&
                            (dDeviation + dPrevDeviation) > -0.09 )
                    {
                        ////qDebug() << "*** Stopping at saturation ***";

                        ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings<< " Deviation: " << dDeviation;
                        break;
                    }
                }

            }



            //store crossing
            cbPrevCrossingCounts.push_back(iCrossingCount);
            dPrevDeviation = dDeviation;

            ////qDebug() << iCrossingCount <<" Mean: "<< dMeanPrevCrossings<< " Deviation: " << dDeviation;

        }

    }



}

void HierarchicalLayouter::sortLayerByBarryCenters(int iLayerId, ProcessDirection enDirection, NeighborNodes enNeighborNodes)
{
//    Q_ASSERT_X(m_mapLayeredGraph.contains(iLayerId) ,
//               "Sort Layer by Barry Center Method" ,
//               "Invalid Layer Id provided");
    LAYOUT_ASSERT(m_mapLayeredGraph.contains(iLayerId), LayoutException(__FUNCTION__,
                                                                        LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                                        ,"Layered Graph"
                                                                        ,"LayerId"));

    /*Prerequisite:
     *1. Next Layer in the Direction have consistent
     *iHorizontalPositions in Graph and at LayerNode
     */

    //////qDebug() << "Layer Id" << QString::number(iLayerId);//for testing
    //Check if the provided iLayerId is not the first Layer in the provided direction
    if(enNeighborNodes == Predecessor)
    {
//        Q_ASSERT_X(iLayerId != (m_mapLayeredGraph.constBegin()).key()
//                   , "Sort Layer by Barry Center Method" ,
//                   "Provided Layer is the first layer in the direction");
        LAYOUT_ASSERT(iLayerId != (m_mapLayeredGraph.constBegin()).key()
                                    , LayoutException(__FUNCTION__
                                    , LayoutExceptionEnum::INVALID_OPERATION
                                                      , "Sort first layer in provided direction"
                                                      , "SortByBarryCenter"));

    }
    else if(enNeighborNodes == Succesor)//Up Direction
    {
//        Q_ASSERT_X(iLayerId != (m_mapLayeredGraph.constEnd()).key() , "Sort Layer by Barry Center Method" ,
//                   "Provided Layer is the first layer in the direction");
        LAYOUT_ASSERT(iLayerId != (m_mapLayeredGraph.constEnd()).key()
                                    , LayoutException(__FUNCTION__
                                    , LayoutExceptionEnum::INVALID_OPERATION
                                                      , "Sort first layer in provided direction"
                                                      , "SortByBarryCenter"));
    }


    //Multimap BarryCenter to LayerNode, to have LayerNodes sorted on BarryCenter Weights
    MultiMapBarryCenterToLayerNode mapBarryCenterToLayerNode;

    /*Layer Id of next layer can be checked when we have successor or
     *predecessors of current layerNode vertex at layers farther than
     *adjacent layers, so we can control to choose the layer nodes for
     *calculating BarryCenter using only adjacent( Upper or down) layers
     *or the layers which are not adjacent and still contain a
     *Layer vertex which is a predecessor or successor of current Layer vertex
     *
     *It is required if 'Sparse Long Edge Splitting' is done
     */
    //Find next graph-layer id in specified direction

    //**commented for Testing42014
    //    int iNextGraphLayerId = 0;
//    iNextGraphLayerId = getNextLayerId(iLayerId , enDirection , GraphVerticesLayer);

//    //1:
    int iBadLayerNodeCount = 0;
//     iBadLayerNodeCount = testGetLayerKeysAndVertexPositionNotConsistentCount(*(m_mapLayeredGraph[iNextGraphLayerId]), *m_gMainGraph);
    //////qDebug() << "Next Layer- Inconsistent HorizontalPosition LayerNodes Count :" << QString::number(iBadLayerNodeCount);

    //Q_ASSERT_X(iBadLayerNodeCount == 0 , "Sort Layer by Barry Center Method" , "Next Layer contains LayerNodes with inconsistent HorizontalPosition");


    try
    {
    LayerNode *currentLayerNode = NULL;
    double dBarryCenterWeight = 0;
    double dNextNodeCount = 0;
    int iHorizontalPosition = 0;

    VertexDescriptor vCurrentVertex = 0;
    VertexDescriptor vPredecessor = 0;
    VertexDescriptor vSuccessor = 0;
    InEdgeIterator iterInEdge , iterInEdgeEnd;
    AdjacencyIterator iterAdjacentVertex , iterAdjacentVertexEnd;

    //Calculate BarryCenter Weights for current Layer nodes
    IteratorMapPositionToLayerNode iterLayerNode(*(m_mapLayeredGraph[iLayerId]));

    while(iterLayerNode.hasNext())
    {
        iterLayerNode.next();

        //Get Layer Node
        currentLayerNode = iterLayerNode.value();
        vCurrentVertex = currentLayerNode->getVertex();

        //Reset variables
        dBarryCenterWeight = 0.0;
        dNextNodeCount = 0.0;
        iHorizontalPosition = 0;

        //Iterate next nodes in the Direction
        if(enNeighborNodes == Predecessor)
        {
            //Iterate predecessors
            for(boost::tie(iterInEdge , iterInEdgeEnd) = in_edges(vCurrentVertex , *m_gMainGraph);
                iterInEdge != iterInEdgeEnd;
                iterInEdge++)
            {
                //Get predecessor
                vPredecessor = m_BoostGraphWrapper.getEdgeSource(*iterInEdge , *m_gMainGraph);

                //Get iHorizontal Position
                iHorizontalPosition = m_BoostGraphWrapper.getVertexHorizontalPosition(vPredecessor, *m_gMainGraph);
                dBarryCenterWeight += iHorizontalPosition;

                dNextNodeCount++;
            }
        }
        else if(enNeighborNodes == Succesor)
        {
            //Iterate successors
            for(boost::tie(iterAdjacentVertex , iterAdjacentVertexEnd) = adjacent_vertices(vCurrentVertex , *m_gMainGraph);
                iterAdjacentVertex != iterAdjacentVertexEnd;
                iterAdjacentVertex++)
            {
                //Get successor
                vSuccessor = *iterAdjacentVertex;

                //Get iHorizontal Position
                iHorizontalPosition = m_BoostGraphWrapper.getVertexHorizontalPosition(vSuccessor, *m_gMainGraph);
                dBarryCenterWeight += iHorizontalPosition;

                dNextNodeCount++;
            }
        }


        //Calculate BarryCenter if there are previous layer neighbor nodes
        if(dNextNodeCount > 0)
        {
            dBarryCenterWeight /= dNextNodeCount;

            m_BoostGraphWrapper.setVertexBarryCenter(vCurrentVertex , *m_gMainGraph , dBarryCenterWeight);
        }

        dBarryCenterWeight = m_BoostGraphWrapper.getVertexBarryCenter(vCurrentVertex , *m_gMainGraph);


        m_BoostGraphWrapper.setVertexBarryCenter(vCurrentVertex , *m_gMainGraph , dBarryCenterWeight);


        //Add BarryCenter to LayerNode entry to QMultiMap
        mapBarryCenterToLayerNode.insertMulti(dBarryCenterWeight , currentLayerNode);

    }

    //Delete and set Null old Layer
    DELETE_AND_SET_NULL(m_mapLayeredGraph[iLayerId]);

    //Create layer sorted according to BarryCenter Weights
    MapPositionToLayerNode *newLayer = new MapPositionToLayerNode();

    //Insert new Layer into layered graph
    //m_mapLayeredGraph.insert(iLayerId , newLayer); //OR
    m_mapLayeredGraph[iLayerId] = newLayer;


    //Reset
    iHorizontalPosition = 1;
    vCurrentVertex = 0;

    //Copy LayerNodes sorted on their BarryCenter Weight to new Layer
    IteratorMultiMapBarryCenterToLayerNode iterBarryCenterLayerNode(mapBarryCenterToLayerNode);
    LayerNode* layerNodeFromBarryCenter = NULL;
    while(iterBarryCenterLayerNode.hasNext())
    {
        iterBarryCenterLayerNode.next();

        //Get LayerNode
        layerNodeFromBarryCenter = iterBarryCenterLayerNode.value();
        vCurrentVertex = layerNodeFromBarryCenter->getVertex();

        //insert LayerNode to Layer
        newLayer->insert(iHorizontalPosition , layerNodeFromBarryCenter);

        //Update iHorizotalPosition
        m_BoostGraphWrapper.setVertexHorizontalPosition(vCurrentVertex , *m_gMainGraph , iHorizontalPosition);

        iHorizontalPosition++;
    }

    iBadLayerNodeCount = testGetLayerKeysAndVertexPositionNotConsistentCount(*newLayer , *m_gMainGraph);
    //Q_ASSERT_X(iBadLayerNodeCount == 0 , "Sort Layer by Barry Center Method" , "NEW Layer contains LayerNodes with inconsistent HorizontalPosition");

    LAYOUT_ASSERT(iBadLayerNodeCount == 0, LayoutException(__FUNCTION__
                                                           , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                           , "Layer"
                                                           ,""));
    //////qDebug() << "Inconsistent HorizontalPosition LayerNodes Count :" << QString::number(iBadLayerNodeCount);
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}



void HierarchicalLayouter::reduceEdgeCrossings(int iCrossingReductionDegree)
{
    /*BarryCenter Crossing Reduction requires at least two layers for
     *crossing reduction
     *
     *This process is done top-down and bottom-up for better results
     */


    /*Prerequesits:
     *1. Graph has proper hierarchy
     *2. Nesting Tree is created
     *3. Layered Graph is created
     */

    try
    {
        int iLayerId = 0;

        int iTotalCrossings = 0;

        ConstantType<int> iTotalVertices;
        iTotalVertices = num_vertices(*m_gMainGraph);


        //TODO: user input iCrossingReduction Degree
        iCrossingReductionDegree = 200;

        int iCrossingIter = 20;//m_mapLayeredGraph.size() *  2;

        int iCurrentCrossing = 0;
        int iPrevCrossing = 0;
        int iCrossingStabilityMargin = 0;


        bool bUpwardDirection = false;

        QString sCrossingsCount = "";

        //TODO: Accept 'Crossing Degree' from user to decide number of iterations
        //Iterate still Number of Crossings is unsatisfactory
        //while(iTotalCrossings <= iCrossingReductionDegree)

        //Check if graph has atleast 1 vertex i.e. 1 vertex + 2 Border vertices -upper and lower
        //therefor total 3 vertices
        if(iTotalVertices >= 3)
        {
            while(iCrossingIter--)
            {

                crossingReductionByReducedNestingTree(bUpwardDirection);

                crossingReductionBySubgraphOrderingGraph(bUpwardDirection);


                //*****************************************

                //iTotalCrossings = getTotalCrossings();

                if(bUpwardDirection)
                {
                    ////qDebug() << "Crossing (BottomUp): "<<iTotalCrossings;
                    sCrossingsCount.append("Crossing (BottomUp):");
                    sCrossingsCount.append(QString::number(iTotalCrossings));
                    sCrossingsCount.append("\n");
                }
                else
                {
                    ////qDebug() << "Crossing (TopDown): "<<iTotalCrossings;
                    sCrossingsCount.append("Crossing (TopDown):");
                    sCrossingsCount.append(QString::number(iTotalCrossings));
                    sCrossingsCount.append("\n");
                    sCrossingsCount.append("---------------\n");
                }


                //if(iTotalCrossings == 0)
                //{
                //break;
                //}
                //Reverse process
                bUpwardDirection = !bUpwardDirection;
            }

        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

void HierarchicalLayouter::crossingReductionByReducedNestingTree(bool bIsUpwardDirection)
{
    int  iLayerId = 0;

    try
    {

        MapLayerIdToLayerRef::iterator iterLayer;

        if(bIsUpwardDirection == false)
        {
            /*Rank difference in layers always the rank of first layer with
         *graph nodes if the graph has atleast 1 graph_vertex
         */
            //iterLayer = m_mapLayeredGraph.find(m_iRankDifferenceInLayers);
            iterLayer = m_mapLayeredGraph.begin(); //Testing 42114-1

            iLayerId = iterLayer.key();
        }
        else
        {
            iterLayer = m_mapLayeredGraph.end();

            //Last layer
            iterLayer--;

            //Layer id of last layer
            iLayerId = iterLayer.key();

            //        Commented for Testing 42114-1
            //        //Coming to last graph layer LayerId
            //        iLayerId -= iLayerId % m_iRankDifferenceInLayers;

            //        iterLayer = m_mapLayeredGraph.find(iLayerId);

        }

        ////qDebug() <<"Iterate Layer from: ";

        ////qDebug() << iLayerId;

        //sort first layer according to Reduced Nested Tree
        doSortedTraversalByReducedNestingTree(iLayerId);

        if(bIsUpwardDirection == true)
        {
            iterLayer--;
        }
        else
        {
            iterLayer++;
        }

        //For layer 2 to n
        while(iterLayer != m_mapLayeredGraph.end())
        {

            //Sort current layer by barrycenter weights
            //Sort current layer by sorted traversal of Reduced Nesting Tree
            //This  produces a placement with unbroken sequences of subgraph nodes in current layer

            iLayerId = iterLayer.key();

            ////qDebug() << iLayerId;


            //if(iLayerId % m_iRankDifferenceInLayers == 0) // Commented for Testing42014-1
            {
                //Sort current layer by barrycenter weights
                if(bIsUpwardDirection == false)
                {
                    sortLayerByBarryCenters(iLayerId , DownDirection , Predecessor);
                }
                else
                {
                    sortLayerByBarryCenters(iLayerId , UpDirection , Succesor);
                }

                //Sort current layer by sorted traversal of Reduced Nesting Tree
                doSortedTraversalByReducedNestingTree(iLayerId);

            }


            if(bIsUpwardDirection == true)
            {
                if(iterLayer == m_mapLayeredGraph.begin())
                {
                    break;
                }

                iterLayer--;
            }
            else
            {
                iterLayer++;
            }
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::crossingReductionBySubgraphOrderingGraph(bool bIsUpwardDirection)
{
    try
    {
        int  iLayerId = 0;

        MapLayerIdToLayerRef::iterator iterLayer;

        if(bIsUpwardDirection == false)
        {
            /*Rank difference in layers always the rank of first layer with
         *graph nodes if the graph has atleast 1 graph_vertex
         */
            //iterLayer = m_mapLayeredGraph.find(m_iRankDifferenceInLayers);
            iterLayer = m_mapLayeredGraph.begin();//Testing 42114-1

            iLayerId = iterLayer.key();
        }
        else
        {
            iterLayer = m_mapLayeredGraph.end();

            //Last layer
            iterLayer--;

            //Layer id of last layer
            iLayerId = iterLayer.key();

            //        Commented for Testing 42114-1
            //        //Coming to last graph layer LayerId
            //        iLayerId -= iLayerId % m_iRankDifferenceInLayers;

            //        iterLayer = m_mapLayeredGraph.find(iLayerId);

        }

        ////qDebug() << "-----------------------------";

        //**Calculate Subgraph Ordering Graph OG
        //**Sort OG topologically, break cycles if necessary
        //LayersSubgraphSorter sorter();

        //**
        //Create SubgraphOrderingGraph
        generateSubgraphOrderingGraph();

        bool bIsSubgraphOrderingGraphCorrect = testSubgraphOrderingGraph();
        LAYOUT_ASSERT(bIsSubgraphOrderingGraphCorrect == true,
                      LayoutException(__FUNCTION__
                                      , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                      , "Subgraph ordering graph"
                                      , ""));

        //Update average BarryCenter positions of NestingTreeNode
        updateAverageBarryCenterForNestingTreeNodes();

        //Remove cycles in SubgraphOrderingGraph
        removeCycleFromSubgraphOrderingGraph();

        ////qDebug() << "--Removing Cycle From SOG--";
        //Assign topological order to original graph vertices according to SubgraphOrderingGraphs
        assignTopologicalOrderAccordingToSubgraphOrderingGraph(1,&m_rootNestingTreeSubgraphNode);

        ////qDebug() <<"Iterate Layer from: ";

        ////qDebug() <<"SOG Sort "<< iLayerId;

        //Sort first layer according iTopologicalOrder
        sortLayerByTopologicalOrder(iLayerId);

        if(bIsUpwardDirection == true)
        {
            iterLayer--;
        }
        else
        {
            iterLayer++;
        }

        //For layer 2 to n
        while(iterLayer != m_mapLayeredGraph.end())
        {



            iLayerId = iterLayer.key();

            ////qDebug() <<"SOG Sort "<< iLayerId;


            //if(iLayerId % m_iRankDifferenceInLayers == 0)//42114-1
            {
                //Calculate AverageBarryCenter weigths
                updateAverageBarryCenterForNestingTreeNodes();

                //Assign new topological ordering
                assignTopologicalOrderAccordingToSubgraphOrderingGraph( 1 , &m_rootNestingTreeSubgraphNode);

                //Sort current layer by SubgraphOrderingGraph topological order
                sortLayerByTopologicalOrder(iLayerId);

            }

            ////qDebug() << "-----------------------------";

            if(bIsUpwardDirection == true)
            {
                if(iterLayer == m_mapLayeredGraph.begin())
                {
                    break;
                }

                iterLayer--;
            }
            else
            {
                iterLayer++;
            }
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

void HierarchicalLayouter::crosingReductionFinalTopologicalSorting()
{
    int  iLayerId = 0;

    int iIteration = 1; // 1 for downward and 1 for upward

    MapLayerIdToLayerRef::iterator iterLayer;

    bool bIsUpwardDirection = false;

    ////qDebug() << "SOG Sort with Single SubgraphOrderingGraph";
    while(iIteration--)
    {
        if(bIsUpwardDirection == false)
        {
            /*Rank difference in layers always the rank of first layer with
         *graph nodes if the graph has atleast 1 graph_vertex
         */
            //iterLayer = m_mapLayeredGraph.find(m_iRankDifferenceInLayers);
            iterLayer = m_mapLayeredGraph.begin();//Testing 42114-1

            iLayerId = iterLayer.key();
        }
        else
        {
            iterLayer = m_mapLayeredGraph.end();

            //Last layer
            iterLayer--;

            //Layer id of last layer
            iLayerId = iterLayer.key();

            //        Commented for Testing 42114-1
            //        //Coming to last graph layer LayerId
            //        iLayerId -= iLayerId % m_iRankDifferenceInLayers;

            //        iterLayer = m_mapLayeredGraph.find(iLayerId);

        }



        //**Calculate Subgraph Ordering Graph OG
        //**Sort OG topologically, break cycles if necessary
        //LayersSubgraphSorter sorter();

        //**
        //Create SubgraphOrderingGraph
        generateSubgraphOrderingGraph();

        bool bIsSubgraphOrderingGraphCorrect = testSubgraphOrderingGraph();
        LAYOUT_ASSERT(bIsSubgraphOrderingGraphCorrect == true, LayoutException(__FUNCTION__
                                                                          , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                                          , "SubgraphOrderingGraphCorrect"
                                                                          , "in Crossing Reduction"));


        //Update average BarryCenter positions of NestingTreeNode
        updateAverageBarryCenterForNestingTreeNodes();

        //Remove cycles in SubgraphOrderingGraph
        removeCycleFromSubgraphOrderingGraph();

        ////qDebug() << "--Removing Cycle From SOG--";
        //Assign topological order to original graph vertices according to SubgraphOrderingGraphs
        assignTopologicalOrderAccordingToSubgraphOrderingGraph(1,&m_rootNestingTreeSubgraphNode);

        ////qDebug() <<"Iterate Layer from: ";

        ////qDebug() <<"SOG Sort "<< iLayerId;

        //Sort first layer according iTopologicalOrder
        sortLayerByTopologicalOrder(iLayerId);

        if(bIsUpwardDirection == true)
        {
            iterLayer--;
        }
        else
        {
            iterLayer++;
        }

        //For layer 2 to n
        while(iterLayer != m_mapLayeredGraph.end())
        {



            iLayerId = iterLayer.key();

            ////qDebug() <<"SOG Sort "<< iLayerId;


            //if(iLayerId % m_iRankDifferenceInLayers == 0)//Only GraphLayerNodes
            {

                //Sort current layer by SubgraphOrderingGraph topological order
                sortLayerByTopologicalOrder(iLayerId);

            }


            if(bIsUpwardDirection == true)
            {
                if(iterLayer == m_mapLayeredGraph.begin())
                {
                    break;
                }

                iterLayer--;
            }
            else
            {
                iterLayer++;
            }
        }

        bIsUpwardDirection = !bIsUpwardDirection;
    }
}

void HierarchicalLayouter::sortLayerByTopologicalOrder(int iLayerId)
{
    try
    {
        LAYOUT_ASSERT(m_mapLayeredGraph.contains(iLayerId)
                      , LayoutException(__FUNCTION__
                                        , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                        , "Layered Graph"
                                        , "Layer Id"));

        int iInconsistentLayerNodeCount = testGetLayerKeysAndVertexPositionNotConsistentCount(*(m_mapLayeredGraph[iLayerId]) , *m_gMainGraph);
        LAYOUT_ASSERT(iInconsistentLayerNodeCount == 0,
                      LayoutException(__FUNCTION__
                                      , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                      , "Layer"
                                      , ""));

        ////qDebug() << "Sorting Topologically , Layer: "<<iLayerId;

        QMap<int , QVector<LayerNode*> > mapTopologicalOrderToVectorLayerNodeRef;

        IteratorMapPositionToLayerNode iterLayerNode(*(m_mapLayeredGraph[iLayerId]));

        int iTopologicalOrder = 0;
        LayerNode* currentLayerNode = NULL;
        VertexDescriptor vCurrentVertex = 0;

        //Add LayerNodes to vectors which will preserve their sequence of insertion. For each topological order
        //value there is separate vector

        while(iterLayerNode.hasNext())
        {
            iterLayerNode.next();
            //Get layerNode
            currentLayerNode = iterLayerNode.value();

            //Get TopologicalOrder
            vCurrentVertex = currentLayerNode->getVertex();

            iTopologicalOrder = m_BoostGraphWrapper.getVertexTopologicalOrder(vCurrentVertex , *m_gMainGraph);

            //Add to map
            mapTopologicalOrderToVectorLayerNodeRef[iTopologicalOrder].push_back(currentLayerNode);

        }

        int iOldLayerNodeCount = m_mapLayeredGraph[iLayerId]->size();

        //Delete and set Null old Layer
        DELETE_AND_SET_NULL(m_mapLayeredGraph[iLayerId]);

        //Create layer sorted according to BarryCenter Weights
        MapPositionToLayerNode *newLayer = new MapPositionToLayerNode();

        //Insert new Layer into layered graph
        m_mapLayeredGraph[iLayerId] = newLayer;


        int iHorizontalPosition = 1;

        //Add layerNodes to new layer
        QMapIterator<int , QVector<LayerNode*> > iterMapTopologicalOrderToVectorLayerNodeRef(
                    mapTopologicalOrderToVectorLayerNodeRef);

        //Iterate according to iTopologicalOrder value
        while(iterMapTopologicalOrderToVectorLayerNodeRef.hasNext())
        {
            iterMapTopologicalOrderToVectorLayerNodeRef.next();

            iTopologicalOrder = iterMapTopologicalOrderToVectorLayerNodeRef.key();

            //////qDebug() <<"Topo Order: "<<iTopologicalOrder << " adding nodes";

            QVectorIterator<LayerNode*> iterVectorLayerNodeRef(
                        iterMapTopologicalOrderToVectorLayerNodeRef.value());

            //Add nodes with same topological order value while preserving their insertion sequence
            while(iterVectorLayerNodeRef.hasNext())
            {
                currentLayerNode = iterVectorLayerNodeRef.next();

                //Add layerNode to newLayer
                newLayer->insert(iHorizontalPosition , currentLayerNode);

                vCurrentVertex = currentLayerNode->getVertex();


                //Update new iHorizontalPosition of LayerNode in vertex property iHorizontalPosition
                m_BoostGraphWrapper.setVertexHorizontalPosition(vCurrentVertex
                                                                , *m_gMainGraph
                                                                , iHorizontalPosition);

                iHorizontalPosition++;

            }

        }

        LAYOUT_ASSERT(iOldLayerNodeCount == newLayer->size()
                      , LayoutException(__FUNCTION__
                                        , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                        , "after sorting it by topological order"
                                        , "Layer"));


        bool bIsLayerSortedTopologically = testIsLayerTopologicallySorted(iLayerId);
        LAYOUT_ASSERT(bIsLayerSortedTopologically == true
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                       , "not properly sorted according to topological order"
                                       , "Layer"));

        iInconsistentLayerNodeCount = testGetLayerKeysAndVertexPositionNotConsistentCount(*(m_mapLayeredGraph[iLayerId]) , *m_gMainGraph);

        LAYOUT_ASSERT(iInconsistentLayerNodeCount == 0
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                       , "with inconsistent node positions"
                                       , "Layer"));

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::doSortedTraversalByReducedNestingTree(int iLayerId)
{
    //Q_ASSERT_X(m_mapLayeredGraph.contains(iLayerId) == true , "doSortedTraversalByReducedNestingTree","Invalid layer id passed");

    try
    {

    LAYOUT_ASSERT(m_mapLayeredGraph.contains(iLayerId) == true , LayoutException(__FUNCTION__
                                                                                 ,LayoutExceptionEnum::INVALID_PARAMETER
                                                                                 ,QString::number(iLayerId)
                                                                                 ,"Layer Id"));

    //Create Reduced Nested Tree for iLayerId layer
    ReducedNestingTreeNode *reducedNestingTree = NULL;

    reducedNestingTree = new ReducedNestingTreeNode();

    createReducedNestedTree(iLayerId
                            , m_rootNestingTreeSubgraphNode
                            , *reducedNestingTree);

    int iTotalLayerNodes = m_mapLayeredGraph[iLayerId]->size();
    int iTotalReducedTreeLayerNodes = reducedNestingTree->getChildLayerNodeCount();

    ////qDebug() << "LNodes: " << iTotalLayerNodes << " RNT LNodes: " << iTotalReducedTreeLayerNodes;
    //Q_ASSERT_X(iTotalLayerNodes == iTotalReducedTreeLayerNodes , "Do Sorted Traversal By Reduced Nesting Tree" , "Total layer nodes in layer and in Reduced Nested Tree are not equal");

    LAYOUT_ASSERT(iTotalLayerNodes == iTotalReducedTreeLayerNodes , LayoutException(__FUNCTION__
                                                                                    , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                                                    , "Mismatched layer node count"
                                                                                    , "Reduced Nesting Tree"));
//    if(iTotalLayerNodes != iTotalReducedTreeLayerNodes)
//    {
//        ////qDebug() << "Mismatched layer node count above";
//    }

    //Sorted traversal according to Reduced Nesting Tree
    MapPositionToLayerNode *newLayer = new MapPositionToLayerNode();

    generateLayerByReducedNestingTree(*newLayer , *reducedNestingTree);


    //Test the size of old layer and new layer are same
    int iSizeOfOldLayer = m_mapLayeredGraph[iLayerId]->size();
    int iSizeOfNewLayer = newLayer->size();

    //Q_ASSERT_X(iSizeOfOldLayer == iSizeOfNewLayer , "Do Sorted Traversal By Reduced Nesting Tree" , "Total layer nodes in old layer and in new Layer which is sorted by RNT are not equal");

    LAYOUT_ASSERT(iSizeOfOldLayer == iSizeOfNewLayer , LayoutException(__FUNCTION__
                                                                                    , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                                                    , "Total layer nodes in old layer and in new Layer which is sorted by RNT are not equal"
                                                                                    , "Layer"));

    //Delete old layer
    DELETE_AND_SET_NULL(m_mapLayeredGraph[iLayerId]);

    //Set new layer
    m_mapLayeredGraph[iLayerId] = newLayer;

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::createReducedNestedTree(int iLayerId, NestingTreeSubgraphNode &nestingTreeRootNode, ReducedNestingTreeNode &reducedNestingTreeNode)
{

    //    Q_ASSERT_X(m_mapLayeredGraph.contains(iLayerId) == true , "doSortedTraversalByReducedNestingTree",
    //               "Invalid layer id passed");


    try
    {

        LAYOUT_ASSERT(m_mapLayeredGraph.contains(iLayerId) == true , LayoutException(__FUNCTION__
                                                                                     , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                                                     , "Layered Graph"
                                                                                     , "Layer Id"));


        //Set Nesting tree node to current Reduced Nesting Tree Node
        reducedNestingTreeNode.setNestingTreeNode(&nestingTreeRootNode);

        //RNT - Reduced Nesting Tree

        int iCountTotalLayerNodes = 0;

        double dTotalAveragePosition = 0.0;

        ReducedNestingTreeNode* newReducedNestingTreeNode = NULL;

        //Iterate child nesting tree nodes
        NestingTreeSubgraphNode::VectorNestingTreeSubgraphNodesRef vecChildNestingTreeNodes = nestingTreeRootNode.getChildNestingTreeSubgraphNodes();

        NestingTreeSubgraphNode::VectorNestingTreeSubgraphNodesRef::iterator iterNestingTreeNode
                = vecChildNestingTreeNodes.begin();

        NestingTreeSubgraphNode::VectorNestingTreeSubgraphNodesRef::iterator iterEndNestingTreeNode
                = vecChildNestingTreeNodes.end();


        while(iterNestingTreeNode != iterEndNestingTreeNode)
        {
            NestingTreeSubgraphNode* childNestingTreeNode = *iterNestingTreeNode;

            if(childNestingTreeNode->doesSubgraphNodeContainLayerId(iLayerId))
            {

                //Create new RNT
                newReducedNestingTreeNode = new ReducedNestingTreeNode();

                //Recursive call
                createReducedNestedTree(iLayerId , *childNestingTreeNode , *newReducedNestingTreeNode);

                //Add new reduced nesting tree node to child list of current RNT Node

                double dChildAveragePosition = newReducedNestingTreeNode->getAveragePosition();
                int iChildLayerNodeCount = newReducedNestingTreeNode->getChildLayerNodeCount();

                reducedNestingTreeNode.addChildNode( dChildAveragePosition,
                                                     newReducedNestingTreeNode);

                //Increament child Layer Node counter
                iCountTotalLayerNodes += iChildLayerNodeCount;

                //Accumulate new reduced nesting tree node average position
                dTotalAveragePosition += dChildAveragePosition * iChildLayerNodeCount;


            }


            iterNestingTreeNode++;
        }

        //Iterate layer nodes (vertices) of current nesting Tree Node
        NestingTreeSubgraphNode::MultiMapLayerIdToLayerNodeRef mapLayerNodes
                = nestingTreeRootNode.getMapLayerIdToLayerNodeRef();

        //Get list of layer nodes with rank equal to iLayerId
        QList<LayerNode*> listLayerNodes = mapLayerNodes.values(iLayerId);

        QListIterator<LayerNode*> iterListLayerNodes(listLayerNodes);


        while(iterListLayerNodes.hasNext())
        {
            LayerNode* currentLayerNode = iterListLayerNodes.next();

            VertexDescriptor vCurrentVertex = currentLayerNode->getVertex();

            //Assert the rank of vertex be equal to iLayerId
            int iVertexRank = m_BoostGraphWrapper.getVertexRank(vCurrentVertex , *m_gMainGraph);
            Q_ASSERT_X(iLayerId == iVertexRank , "CreateReducedNestedTree" , "LayerId and vertex rank mismatches");

            int iHorizontalPosition = m_BoostGraphWrapper.getVertexHorizontalPosition(vCurrentVertex , *m_gMainGraph);

            newReducedNestingTreeNode = new ReducedNestingTreeNode();

            newReducedNestingTreeNode->setLayerNode(currentLayerNode);
            newReducedNestingTreeNode->setAveragePosition((double)iHorizontalPosition);

            reducedNestingTreeNode.addChildNode((double)iHorizontalPosition ,
                                                newReducedNestingTreeNode);

            //Increament child RNT counter
            iCountTotalLayerNodes++;

            //Accumulate new reduced netsting tree node average position
            dTotalAveragePosition += (double)iHorizontalPosition;
        }

        //Set average position for current RNT node
        double dCurrentRNTAveragePosition = dTotalAveragePosition / iCountTotalLayerNodes;
        reducedNestingTreeNode.setAveragePosition(dCurrentRNTAveragePosition);

        //Set total child LayerNode count for current RNT node
        reducedNestingTreeNode.setChildLayerNodeCount(iCountTotalLayerNodes);

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::generateLayerByReducedNestingTree(MapPositionToLayerNode &newLayer, ReducedNestingTreeNode &reducedNestingTreeRootNode)
{
    LAYOUT_ASSERT(newLayer.isEmpty() == true, LayoutException(__FUNCTION__
                                                              , LayoutExceptionEnum::INVALID_PARAMETER
                                                              , "it must be empty"
                                                              , "Layer"));


    //Pass first parameter 1 because the horizontal positions should start from 1
    int iHorizontalPositionStart = 1;
    generateLayerByReducedNestingTreeRecur(iHorizontalPositionStart , newLayer , reducedNestingTreeRootNode);
}

void HierarchicalLayouter::generateLayerByReducedNestingTreeRecur(int &iHorizontalPosition, MapPositionToLayerNode &newLayer, ReducedNestingTreeNode &reducedNestingTreeRootNode)
{
    LAYOUT_ASSERT(reducedNestingTreeRootNode.isLayerNode() == false,
             LayoutException(__FUNCTION__
                             ,LayoutExceptionEnum::INVALID_PARAMETER
                             ,"it must not be a layer node"
                             ,"reducedNestingTreeRootNode"));

    ReducedNestingTreeNode::IteratorMapAvgPosToChildRNTNode iterChildRNTNodes
            = reducedNestingTreeRootNode.getChildNodesIterator();

    ReducedNestingTreeNode *currentChildRNTNode = NULL;
    LayerNode *currentLayerNode = NULL;

    VertexDescriptor vCurrentVertex = 0;

    //Testing
    double dAvgPosition = reducedNestingTreeRootNode.getAveragePosition();
    ////qDebug() << "RNT NODE Avg Position: "<< dAvgPosition;


    while(iterChildRNTNodes.hasNext())
    {
        iterChildRNTNodes.next();


        currentChildRNTNode = iterChildRNTNodes.value();

        //Check layer node
        if(currentChildRNTNode->isLayerNode() == true)
        {
            currentLayerNode = currentChildRNTNode->getLayerNode();

            vCurrentVertex = currentLayerNode->getVertex();

            //Update Layer Node Horizontal Position property
            m_BoostGraphWrapper.setVertexHorizontalPosition( vCurrentVertex , *m_gMainGraph ,iHorizontalPosition);

            //Add vertex node with given horizontal position
            newLayer.insert(iHorizontalPosition , currentLayerNode);

            //Increment horizontal position
            ++iHorizontalPosition;
        }
        else
        {
            //Recursive Call
            generateLayerByReducedNestingTreeRecur(iHorizontalPosition , newLayer , *currentChildRNTNode);
        }
    }
}

int HierarchicalLayouter::getNextLayerId(int iLayerId, ProcessDirection enDirection, LayerType enLayerType)
{
        Q_ASSERT_X(m_mapLayeredGraph.contains(iLayerId) , "Get Next LayerId Method" , "Invalid Layer Id provided");

        int iNextLayerId = 0;

        MapLayerIdToLayerRef::iterator iterLayer = m_mapLayeredGraph.find(iLayerId);


        if(enDirection == UpDirection)
        {
            //Check if the current Layer Id is the first Layer
            Q_ASSERT_X(iterLayer != m_mapLayeredGraph.begin() , "Get Next LayerId Method" , "There is no layer in Upward Direction");
        }else if(enDirection == DownDirection)
        {
            //Check if the current Layer Id is the last Layer
            Q_ASSERT_X((iterLayer+1) != m_mapLayeredGraph.end() , "Get Next LayerId Method" , "There is no layer in Downwards Direction");
        }

        bool bKeepTraversing = true;
        while(bKeepTraversing == true){

            //Iterate to next layer in the Direction
            if(enDirection == UpDirection)
            {
                if(iterLayer == m_mapLayeredGraph.begin())
                {
                    bKeepTraversing = false;
                    break;
                }

                iterLayer--;
            }
            else if(enDirection == DownDirection)
            {
                iterLayer++;

                if(iterLayer == m_mapLayeredGraph.end())
                {
                    bKeepTraversing = false;
                    break;
                }

            }

            iNextLayerId = iterLayer.key();

            if(enLayerType == GraphVerticesLayer)
            {
                //Check Graph Vertices Layer
                if((iNextLayerId % m_iRankDifferenceInLayers) == 0)
                {
                    bKeepTraversing = false;
                }

            }
            else if(enLayerType == BorderVerticesLayer)
            {
                //Check Border Vertices Layer
                if((iNextLayerId % m_iRankDifferenceInLayers) > 0)
                {
                    bKeepTraversing = false;
                }
            }
            else
            {
                //For any layer
                bKeepTraversing = false;
            }


        }


        return iNextLayerId;
}

int HierarchicalLayouter::getTotalCrossings()
{
    int iTotalCrossings = 0;

    try
    {

        //Iterate layer from 1 to second last

        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);

        int iUpperPosCurrentEdge = 0;
        int iLowerPosCurrentEdge = 0;

        int iUpperPos = 0;
        int iLowerPos = 0;
        int iRemainingLowerPos = 0;

        QQueue<int> *qUpperLayerVertexPosition = NULL;
        QQueue<int> *qLowerLayerVertexPosition = NULL;

        VertexDescriptor vCurrentVertex;

        LayerNode* currentNode;

        VertexDescriptor vCurrentEdgeSource;
        VertexDescriptor vCurrentEdgeTarget;

        PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexHorizontalPos , int , iHorizontalPosition , *m_gMainGraph);

        while(iterLayer.hasNext())
        {
            iterLayer.next();
            //Check if current layer is not last
            if(iterLayer.hasNext())
            {
                qUpperLayerVertexPosition = new QQueue<int>();
                qLowerLayerVertexPosition = new QQueue<int>();


                //Iterate edges from current layer nodes
                IteratorMapPositionToLayerNode iterNode(*iterLayer.value());
                while(iterNode.hasNext())
                {
                    iterNode.next();

                    currentNode = iterNode.value();
                    vCurrentVertex = currentNode->getVertex();

                    //Iterate edges of current vertex
                    OutEdgeIterator iterEdge , iterEdgeEnd;
                    for(boost::tie(iterEdge , iterEdgeEnd)
                        = out_edges(vCurrentVertex , *m_gMainGraph);
                        iterEdge != iterEdgeEnd;
                        iterEdge++)
                    {
                        EdgeDescriptor eCurrentEdge = *iterEdge;

                        LayoutEnum::EdgeType currentEdgeType = m_BoostGraphWrapper.getEdgeType(eCurrentEdge , *m_gMainGraph);

                        vCurrentEdgeSource = m_BoostGraphWrapper.getEdgeSource(eCurrentEdge , *m_gMainGraph);
                        vCurrentEdgeTarget = m_BoostGraphWrapper.getEdgeTarget(eCurrentEdge , *m_gMainGraph);

                        iUpperPosCurrentEdge = mapVertexHorizontalPos[vCurrentEdgeSource];
                        iLowerPosCurrentEdge = mapVertexHorizontalPos[vCurrentEdgeTarget];


                        if(currentEdgeType == LayoutEnum::GraphEdge ||
                                currentEdgeType == LayoutEnum::LongEdgeSegment)
                        {
                            //Enque source vertex horizontal position in upper vertex position queue
                            qUpperLayerVertexPosition->enqueue(iUpperPosCurrentEdge);

                            //Enque target vertex horizontal position in lower vertex position queue
                            qLowerLayerVertexPosition->enqueue(iLowerPosCurrentEdge);
                        }

                    }
                }

                //Assert if both queue Upper and Lower Vertex Positions has same number of elements
                //Q_ASSERT_X(qUpperLayerVertexPosition->size() == qLowerLayerVertexPosition->size(),
                           //"Edge Crossing Counting" , "Queue upper and lower vertex position are not of same size");

                LAYOUT_ASSERT(qUpperLayerVertexPosition->size() == qLowerLayerVertexPosition->size(),
                              LayoutException(__FUNCTION__
                                              ,LayoutExceptionEnum::INVALID_OPERATION
                                              ,"Queue upper and lower vertex position are not of same size"
                                              , "Edge Crossing Counting"));

                //The target vertices horizontal positions must be in ascending order for
                //their source vertex,For example: if there are horizontal positions of upper and lower nodes
                //of edges originating from an upper vertex are (1,3) and (1,1) then
                //the lower positions 3 and 1 must be in ascending order i.e. it should be (1,1) - (1,3)
                //So we need to sort the lower vertex positions queue

                QListIterator<int> iterUpperVertexPos(*qUpperLayerVertexPosition);

                QQueue<int>::iterator iterQLowerVertexPosBegin;

                int iSequenceStart = 0;
                int iSequenceEnd = 0;
                int iCurrentVal = 0;
                int iPrevVal = 0;
                int iIndex = 0;

                if(iterUpperVertexPos.hasNext())
                {
                    iPrevVal = iterUpperVertexPos.next();
                }

                while(iterUpperVertexPos.hasNext())
                {
                    iCurrentVal = iterUpperVertexPos.next();
                    if(iCurrentVal == iPrevVal)
                    {
                        //Increase Sequence End
                        iSequenceEnd++;
                    }
                    if(iCurrentVal != iPrevVal ||
                            iterUpperVertexPos.hasNext() == false)
                    {

                        //Sort sequence
                        if(iSequenceStart != iSequenceEnd)
                        {
                            //Iterator to the beginning of LowerVertexPosition queue
                            iterQLowerVertexPosBegin = qLowerLayerVertexPosition->begin();

                            qSort(iterQLowerVertexPosBegin + iSequenceStart,
                                  iterQLowerVertexPosBegin + iSequenceEnd + 1,
                                  qLess<int>());
                        }

                        //Set new Sequence Start
                        iSequenceStart = iIndex+1;

                        //Reset Sequence end for new sequence
                        iSequenceEnd = iSequenceStart;

                    }

                    iPrevVal = iCurrentVal;

                    iIndex++;
                }

                //Find crossings

                while(qUpperLayerVertexPosition->isEmpty() == false &&
                      qLowerLayerVertexPosition->isEmpty() == false)
                {
                    iUpperPos = qUpperLayerVertexPosition->dequeue();
                    iLowerPos = qLowerLayerVertexPosition->dequeue();

                    //if(iUpperPos <= iLowerPos)
                    {
                        //Find number of positions lesser than iLowerPos
                        QListIterator<int> iterLowerPos(*qLowerLayerVertexPosition);
                        while(iterLowerPos.hasNext())
                        {
                            iRemainingLowerPos = iterLowerPos.next();

                            if(iRemainingLowerPos < iLowerPos)
                            {
                                //Increase crossing count
                                ++iTotalCrossings;
                            }
                        }

                    }
                }

                //Clean up queues
                DELETE_AND_SET_NULL(qUpperLayerVertexPosition);
                DELETE_AND_SET_NULL(qLowerLayerVertexPosition);

            }
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw;
    }

    return iTotalCrossings;
}

void HierarchicalLayouter::createEmptySubgraphOrderingGraphsForEachNestingTreeSubgraphNode()
{
    try
    {
        //Iterate every Nesting Tree Subgraph node
        QueueNestingTreeSubgraphNodesRef qNestingTreeNodes;

        qNestingTreeNodes.enqueue(&m_rootNestingTreeSubgraphNode);

        NestingTreeSubgraphNode *currentNestingTreeNode = NULL;

        SubgraphOrderingGraphType *gNewSubgraphOrderingGraph = NULL;

        SubgraphOrderingGraphWrapper subgraphOrderingGraphWrapper;

        while(qNestingTreeNodes.isEmpty() == false)
        {
            currentNestingTreeNode = qNestingTreeNodes.dequeue();

            //create Subgraph Ordering Graph
            gNewSubgraphOrderingGraph = new SubgraphOrderingGraphType();

            //Add entry to the global map m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef
            m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.insert(currentNestingTreeNode , gNewSubgraphOrderingGraph);



            //Add node for newSubgraphOrderingGraph in its parent Subgraph Ordering Graph except for root graph
            if(currentNestingTreeNode->isRoot() == false)
            {
                //Find parent nesting tree node
                NestingTreeSubgraphNode& parentNestingTreeNode = currentNestingTreeNode->getParent();

                //Find parent SubgraphOrdering graph from parent nesting tree node
                SubgraphOrderingGraphType* gParentSubgraphOrderingGraph
                        = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(&parentNestingTreeNode);

                //Add vertex for current nesting tree node into its parent subgraph Ordering Graph
                SubgraphOrderingGraphVertexDescriptor vVertex = add_vertex(*gParentSubgraphOrderingGraph);

                //Set nesting tree subgraph node link into its representative subgraph Ordering Graph vertex
                subgraphOrderingGraphWrapper.setVertexNestingTreeSubgraphNodeRef(currentNestingTreeNode , vVertex , *gParentSubgraphOrderingGraph);

                //Add entry of vertex descriptor to currentNestingTreeNode
                currentNestingTreeNode->setSubgraphOrderingGraphVertexIndex((int)vVertex);
            }

            //Enque child Nesting Tree Nodes
            NestingTreeSubgraphNode::IteratorVectorNestingTreeSubgraphNodesRef iterChildNestingTreeNodes
                    = currentNestingTreeNode->getIteratorChildNestingTreeSubgraphNodes();
            while(iterChildNestingTreeNodes.hasNext())
            {
                qNestingTreeNodes.enqueue(iterChildNestingTreeNodes.next());
            }

        }

        //Test subgraph ordering graph creation
        bool bCorrectSOGCreation = true;
        SubgraphOrderingGraphType* rootSubgraphOrderingGraph = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(&m_rootNestingTreeSubgraphNode);

        QQueue<SubgraphOrderingGraphType*> qSubgraphOrderingGraphs;
        qSubgraphOrderingGraphs.enqueue(rootSubgraphOrderingGraph);

        int iSOGCount = 0;

        //Check root subgraph ordering graph can reach to every subgraph
        while(qSubgraphOrderingGraphs.isEmpty() == false)
        {
            SubgraphOrderingGraphType * gCurrentSOG = qSubgraphOrderingGraphs.dequeue();

            iSOGCount++;

            BGL_FORALL_VERTICES(vVertex, *gCurrentSOG , SubgraphOrderingGraphType)
            {
                //check vertex index and the NestingTreeNode it is pointing to has stored same vertex index
                NestingTreeSubgraphNode* nestingTreeNode = subgraphOrderingGraphWrapper.getVertexNestingTreeSubgraphNode(vVertex , *gCurrentSOG);
                if(nestingTreeNode->getSubgraphOrderingGraphVertexIndex() != (int)vVertex)
                {
                    bCorrectSOGCreation = false;
                    ////qDebug() << "Incorrect subgraphOrderingGraph vertex index stored by its NetingTreeSubgraphNode, v: "<<(int)vVertex;
                }
                //check the NestingTree Node points to the correct parent Sbgraph Ordering Graph
                NestingTreeSubgraphNode& parentNestingTreeNode = nestingTreeNode->getParent();
                SubgraphOrderingGraphType *gParentSOG
                        = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(&parentNestingTreeNode);

                if(gParentSOG != gCurrentSOG)
                {
                    bCorrectSOGCreation = false;
                    ////qDebug() << "Incorrect parent SOG pointed by Nesting Tree Subgraph Node";
                }

                //Fill queue by next level SOGs
                qSubgraphOrderingGraphs.enqueue(m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(nestingTreeNode));

            }
        }

        int iTotalNestingTreeNodes = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.size();

        //    if(iSOGCount != iTotalNestingTreeNodes)
        //    {
        //        bCorrectSOGCreation = false;
        //        ////qDebug() << "Total Subgraph Ordering Graph count mismatches with total number of NestingTreeNodes";
        //    }

        //    Q_ASSERT_X(bCorrectSOGCreation , "Create Subgraph Ordering Graphs" , "Incorrect Subgraph Ordering Graph created");

        LAYOUT_ASSERT(iSOGCount == iTotalNestingTreeNodes , LayoutException(__FUNCTION__
                                                                            , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                                            , "Total Subgraph Ordering Graph count mismatches with total number of NestingTreeNodes"
                                                                            , "Subgraph Ordering Graph"));

        ////qDebug()<<"Subgraph Ordering Graphs created correctly";
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::generateSubgraphOrderingGraph()
{

    try
    {
        //Empty up previous subgraph ordering graph
        if(m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.isEmpty() == false)
        {
            //Delete all subgraph ordering graphs
            IteratorMapNestingTreeRefToSubgraphOrderingGraphRef iterMapNestingTreeNodeSubgraphOrderingGraph(
                        m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef);
            while(iterMapNestingTreeNodeSubgraphOrderingGraph.hasNext())
            {
                iterMapNestingTreeNodeSubgraphOrderingGraph.next();
                NestingTreeSubgraphNode* keyNestingTreeNode = iterMapNestingTreeNodeSubgraphOrderingGraph.key();

                SubgraphOrderingGraphType* gSubgraphOrderingGraph = iterMapNestingTreeNodeSubgraphOrderingGraph.value();

                //We can not clear graph because boost does not support this method for subgraph type graph
                //gSubgraphOrderingGraph->clear();

                DELETE_AND_SET_NULL(m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef[keyNestingTreeNode]);
            }

            m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.clear();

            //Clear all vertex index entries in the nesting tree
            QueueNestingTreeSubgraphNodesRef qNestingTreeNodes;
            qNestingTreeNodes.enqueue(&m_rootNestingTreeSubgraphNode);
            while(qNestingTreeNodes.isEmpty() == false)
            {
                NestingTreeSubgraphNode* currentNestingTreeNode = qNestingTreeNodes.dequeue();

                //Clear vertex descriptor entry
                currentNestingTreeNode->setSubgraphOrderingGraphVertexIndex(-1);

                //Enque child nesting tree nodes
                NestingTreeSubgraphNode::IteratorVectorNestingTreeSubgraphNodesRef iterChildNestingTreeNode
                        = currentNestingTreeNode->getIteratorChildNestingTreeSubgraphNodes();
                while(iterChildNestingTreeNode.hasNext())
                {
                    qNestingTreeNodes.enqueue(iterChildNestingTreeNode.next());
                }
            }

            //Reset back all layer node SubgraphOrderingGraphVertex entries to -1
            IteratorHashVertexToLayerNode iterLayerNodes(hashVertexToLayerNode);
            while(iterLayerNodes.hasNext())
            {
                iterLayerNodes.next();
                LayerNode* currentLayerNode = iterLayerNodes.value();

                currentLayerNode->setSubgraphOrderingGraphVertex(-1);
            }
        }
        //Create subgraph ordering graph for every Nesting Tree node
        createEmptySubgraphOrderingGraphsForEachNestingTreeSubgraphNode();

        populateNodesAndEdgesInSubgraphOrderingGraph();

        //Testinfg print
        //    IteratorMapNestingTreeRefToSubgraphOrderingGraphRef
        //            iterNestingTreeSubgraphOrderingGraph(m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef);

        //    ////qDebug()<<"==------start-graph----==\n";
        //    SubgraphOrderingGraphType* gSOG = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(
        //                &m_rootNestingTreeSubgraphNode);
        //    m_subgraphOrderingGraphWrapper.printGraph(*gSOG);
        //    ////qDebug()<<"==------end-graph------==\n";

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::populateNodesAndEdgesInSubgraphOrderingGraph()
{
    try
    {
        //Add layer nodes and edges according to 'left of' relationships

        LayerNode* leftLayerNode = NULL;
        LayerNode* rightLayerNode = NULL;

        NestingTreeSubgraphNode* leftNestingTreeNode = NULL;
        NestingTreeSubgraphNode* rightNestingTreeNode = NULL;

        SubgraphOrderingGraphVertexDescriptor vLeftVertex = 0;
        SubgraphOrderingGraphVertexDescriptor vRightVertex = 0;

        SubgraphOrderingGraphType* gCommonParentSubgraphOrderingGraph = NULL;

        //Iterate layered graph to found neighbor vertices pairs
        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);
        while(iterLayer.hasNext())
        {

            iterLayer.next();
            IteratorMapPositionToLayerNode iterLayerNode(*(iterLayer.value()));
            while(iterLayerNode.hasNext())
            {
                iterLayerNode.next();

                //Get left side layer node
                leftLayerNode = iterLayerNode.value();

                if(leftLayerNode->getSubgraphOrderingGraphVertex() < 0)
                {
                    //Add vertex in Subgraph Ordering Graph for left layer node
                    addLayerNodeToOwnParentSubgraphOrderingGraph(leftLayerNode);

                    //////qDebug() << "add left node to SOG";
                }


                //check if current left layer node has right side neighbor
                if(iterLayerNode.hasNext())
                {
                    rightLayerNode = iterLayerNode.peekNext().value();

                    //////qDebug()<<"Layer node pair: ";
                    leftLayerNode->printName();
                    rightLayerNode->printName();

                    //find common parent NestingTreeSubgraphNode

                    /*If the direct parent NestingTreenonde is not same then
                 *one or both (left and right) layer nodes will be their NestingTree parent nodes
                 *In this case add edge between the parent Nesting Tree Nodes
                 */

                    NestingTreeSubgraphNode* leftLayerNodeParentNestingTreeNode
                            = &(leftLayerNode->getParentNestingTreeSubgraphNode());

                    NestingTreeSubgraphNode* rightLayerNodeParentNestingTreeNode
                            = &(rightLayerNode->getParentNestingTreeSubgraphNode());

                    if(leftLayerNodeParentNestingTreeNode != rightLayerNodeParentNestingTreeNode)
                    {
                        //Left and right layer nodes are not direct siblings i.e.
                        //not direct child of same subgraph

                        //Traverse left side layerNode Nesting Tree Parents till root
                        QVector<NestingTreeSubgraphNode*> vecLeftLayerNodeNestingTreeParents;

                        //Add first element
                        vecLeftLayerNodeNestingTreeParents.push_back(leftLayerNodeParentNestingTreeNode);

                        while(vecLeftLayerNodeNestingTreeParents.last()->isRoot() == false)
                        {
                            NestingTreeSubgraphNode* parentNestingTreeNode
                                    = &(vecLeftLayerNodeNestingTreeParents.last()->getParent());

                            vecLeftLayerNodeNestingTreeParents.push_back(parentNestingTreeNode);
                        }

                        //Traverse right side layerNode Nesting Tree Parents till
                        //finding common parent Nesting tree node
                        QVector<NestingTreeSubgraphNode*> vecRightLayerNodeNestingTreeParents;

                        //Add first element
                        vecRightLayerNodeNestingTreeParents.push_back(rightLayerNodeParentNestingTreeNode);

                        //Pointer to last Nesting Tree node in vecRightLayerNodeNestingTreeParents
                        NestingTreeSubgraphNode* currentRightNestingTreeParent = vecRightLayerNodeNestingTreeParents.last();

                        while(vecLeftLayerNodeNestingTreeParents.contains(currentRightNestingTreeParent) == false)
                        {
                            //Sentinal condition to make sure loop does not go in infinity
                            //This condition will never occur if everything is correct
                            //This checks if currentRightNestingTreeParent is root nesting tree node and still its not found in
                            //vecLeftLayerNodeNestingTreeParents then there must be something wrong because root nesting tree node
                            //should be present in vecLeftLayerNodeNestingTreeParents
                            //Q_ASSERT_X(currentRightNestingTreeParent->isRoot() == false , "populateNodesAndEdgesInSubgraphOrderingGraph"
                            //, "Error while finding common parent nesting node, Left parent list does not contain root nesting tree node");
                            LAYOUT_ASSERT(currentRightNestingTreeParent->isRoot() == false
                                          ,LayoutException(__FUNCTION__
                                                           ,LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                           ,"Left node nesting tree parent tree. Something wrong with left node nesting tree parents list generation"
                                                           , "Root nesting tree node"));

                            NestingTreeSubgraphNode* parentNestingTreeNode
                                    = &(vecRightLayerNodeNestingTreeParents.last()->getParent());

                            vecRightLayerNodeNestingTreeParents.push_back(parentNestingTreeNode);

                            currentRightNestingTreeParent = vecRightLayerNodeNestingTreeParents.last();
                        }

                        //Now currentRightNestingTreeParent contains common parent
                        gCommonParentSubgraphOrderingGraph
                                = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(currentRightNestingTreeParent);

                        //////qDebug() <<"Common parent: ";
                        currentRightNestingTreeParent->printName();

                        int iLeftCommonParentFountAtIndex
                                = vecLeftLayerNodeNestingTreeParents.indexOf(currentRightNestingTreeParent);

                        int iRightCommonParentFoundAtIndex
                                = vecRightLayerNodeNestingTreeParents.indexOf(currentRightNestingTreeParent);


                        LAYOUT_ASSERT(false == (iRightCommonParentFoundAtIndex == iLeftCommonParentFountAtIndex &&
                                                iLeftCommonParentFountAtIndex == 0)
                                      ,LayoutException(__FUNCTION__
                                                       , LayoutExceptionEnum::INVALID_OPERATION
                                                       , "Finding Common parent failed"
                                                       , "Populating Nodes and edges for Subgraph Ordering Graph"));

                        if(iLeftCommonParentFountAtIndex == 0)
                        {
                            //Common parent is found at first location in left vector means
                            //It is direct parent of leftLayerNode, hence we have to consider
                            //leftLayerNodes's vertex descriptor

                            vLeftVertex = leftLayerNode->getSubgraphOrderingGraphVertex();

                        }
                        else
                        {
                            //Select nesting tree node child to common parent nesting tree node
                            leftNestingTreeNode = vecLeftLayerNodeNestingTreeParents.at(iLeftCommonParentFountAtIndex - 1);


                            LAYOUT_ASSERT(leftNestingTreeNode->getSubgraphOrderingGraphVertexIndex() >= 0,
                                          LayoutException(__FUNCTION__
                                                          , LayoutExceptionEnum::INVALID_PARAMETER
                                                          , QString::number(leftNestingTreeNode->getSubgraphOrderingGraphVertexIndex())
                                                          , "LeftNestingTreeNode's Subgraph Ordering Graph Vertex Index"));

                            vLeftVertex = leftNestingTreeNode->getSubgraphOrderingGraphVertexIndex();

                            //qDebug() << "LeftNTNode: ";
                            leftNestingTreeNode->printName();

                        }

                        if(iRightCommonParentFoundAtIndex == 0)
                        {
                            //Common parent is found at first location in right vector means
                            //It is direct parent of rightLayerNode, hence we have to consider
                            //rightLayerNodes's vertex descriptor

                            //Check if right layer node is already added to Subgraph Ordering Graph
                            if(rightLayerNode->getSubgraphOrderingGraphVertex() < 0)
                            {
                                //Add vertex in Subgraph Ordering Graph for right layer node
                                addLayerNodeToOwnParentSubgraphOrderingGraph(rightLayerNode);

                            }

                            vRightVertex = rightLayerNode->getSubgraphOrderingGraphVertex();
                        }
                        else
                        {
                            //Select nesting tree node child to common parent nesting tree node
                            rightNestingTreeNode = vecRightLayerNodeNestingTreeParents.at(iRightCommonParentFoundAtIndex - 1);


                            LAYOUT_ASSERT(rightNestingTreeNode->getSubgraphOrderingGraphVertexIndex() >= 0,
                                          LayoutException(__FUNCTION__
                                                          , LayoutExceptionEnum::INVALID_PARAMETER
                                                          , QString::number(leftNestingTreeNode->getSubgraphOrderingGraphVertexIndex())
                                                          , "RightNestingTreeNode's Subgraph Ordering Graph Vertex Index"));


                            vRightVertex = rightNestingTreeNode->getSubgraphOrderingGraphVertexIndex();

                            //////qDebug() << "RightNTNode";
                            rightNestingTreeNode->printName();
                        }

                    }
                    else
                    {
                        //left and right layer nodes are direct siblings under same subgraph nesting tree node
                        //so get SubgraphOrderingGraph vertices of both layer nodes and add edge

                        //leftLayer node vertex in SubgraphOrdering Graph is added already at the start of this loop
                        vLeftVertex = leftLayerNode->getSubgraphOrderingGraphVertex();


                        //Check if right layer node is already added to Subgraph Ordering Graph
                        if(rightLayerNode->getSubgraphOrderingGraphVertex() < 0)
                        {
                            //Add vertex in Subgraph Ordering Graph for right layer node
                            addLayerNodeToOwnParentSubgraphOrderingGraph(rightLayerNode);


                        }

                        vRightVertex = rightLayerNode->getSubgraphOrderingGraphVertex();

                        //////qDebug() << "Direct parent: ";
                        leftLayerNodeParentNestingTreeNode->printName();

                        //Parent nesting tree node of left and right layer node are same
                        gCommonParentSubgraphOrderingGraph
                                = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(leftLayerNodeParentNestingTreeNode);

                    }

                    //Add edge between - vLeft to vRight in SOG


                    LAYOUT_ASSERT(gCommonParentSubgraphOrderingGraph->find_vertex(vLeftVertex).second
                                  , LayoutException(__FUNCTION__
                                                    , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                    , "Common Parent SubgraphOrderingGraph"
                                                    , "Left Vertex representing left neighbor node on layer"));
                    LAYOUT_ASSERT(gCommonParentSubgraphOrderingGraph->find_vertex(vRightVertex).second
                                  , LayoutException(__FUNCTION__
                                                    , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                    , "Common Parent SubgraphOrderingGraph"
                                                    , "Right Vertex representing right side node on layer"));


                    //////qDebug() <<"Edge: "<<(int)vLeftVertex<<" , "<<(int)vRightVertex;

                    bool bEdgeAddedAlready = false;



                    AdjacencyIteratorSubgraphOrdeingGraph iterAdjacentVertex , iterAdjacentVertexEnd;

                    //check if already there is an edge

                    for(boost::tie(iterAdjacentVertex , iterAdjacentVertexEnd)
                        = adjacent_vertices(vLeftVertex , *gCommonParentSubgraphOrderingGraph);
                        iterAdjacentVertex != iterAdjacentVertexEnd;
                        iterAdjacentVertex++)
                    {
                        SubgraphOrderingGraphVertexDescriptor vAdjacentVertex
                                = *iterAdjacentVertex;

                        //////qDebug()<<(int)vAdjacentVertex;
                        if(vAdjacentVertex == vRightVertex)
                        {
                            bEdgeAddedAlready = true;
                        }
                    }

                    if(bEdgeAddedAlready == false)
                    {
                        //////qDebug()<<"Adding edge";
                        add_edge(vLeftVertex , vRightVertex , *gCommonParentSubgraphOrderingGraph);
                    }
                    else
                    {
                        //////qDebug() << "Edge exist";
                    }






                }//End of IF Right neighbor
                else
                {
                    //////qDebug() <<"No right side node";
                }
            }//Iterate layer layer node loop end
        }//Iterate layer loop end

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

void HierarchicalLayouter::addLayerNodeToOwnParentSubgraphOrderingGraph(LayerNode *layerNode)
{
    LAYOUT_ASSERT(layerNode->getSubgraphOrderingGraphVertex() < 0,
                  LayoutException(__FUNCTION__
                                  , LayoutExceptionEnum::INVALID_PARAMETER
                                  , "it must be uninitialised"
                                  , "Layer Nodes SubgraphOrderingGraphVertex"));

    //Add vertex in Subgraph Ordering Graph for layer node

    //Get parent Nesting Tree node
    NestingTreeSubgraphNode& nestingTreeNodeForLayerNode
            = layerNode->getParentNestingTreeSubgraphNode();

    //Get parent Subgraph Ordering Graph
    SubgraphOrderingGraphType *gLayerNodeParentSubgraphOrderingGraph
            = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(&nestingTreeNodeForLayerNode);

    //Add vertex for layer node
    SubgraphOrderingGraphVertexDescriptor vNewVertex = add_vertex(*gLayerNodeParentSubgraphOrderingGraph);

    //Add link for layer node into newly addded vertex
    m_subgraphOrderingGraphWrapper.setVertexLayerNodeRef(layerNode
                                                       , vNewVertex
                                                       , *gLayerNodeParentSubgraphOrderingGraph);

    //Set newly added subgraph ordering graph vertex index into leftLayerNode
    layerNode->setSubgraphOrderingGraphVertex(vNewVertex);

}

SubgraphOrderingGraphType *HierarchicalLayouter::getParentSubgraphOrderingGraph(NestingTreeSubgraphNode *nestingTreeNode)
{
    LAYOUT_ASSERT(nestingTreeNode != NULL,
                  LayoutException(__FUNCTION__
                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                  , "nestingTreeNode"
                                  , "in SubgraphOrderingGraphType"));

    NestingTreeSubgraphNode& parentNestingTreeNode = nestingTreeNode->getParent();
    return m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(&parentNestingTreeNode);
}

void HierarchicalLayouter::removeCycleFromSubgraphOrderingGraph()
{    
    LAYOUT_ASSERT(m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.isEmpty() == false,
                  LayoutException(__FUNCTION__
                                  , LayoutExceptionEnum::EMPTY_CONTAINER
                                  , "Map must not be empty"
                                  , "mapNestingTreeNodeRefToSubgraphOrderingGraphRef"));

    try
    {
        //For each subgraph ordering graph remove cycles, except the subgraph of a
        //Nesting Tree Node which does not contain any child nesting tree node

        IteratorMapNestingTreeRefToSubgraphOrderingGraphRef iterNestingTreeNodeToSubgraphOrderingGraph(
                    m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef);

        NestingTreeSubgraphNode* currentNestingTreeNode = NULL;
        SubgraphOrderingGraphType* currentSubgraphOrderingGraph = NULL;
        int iChildNestingTreeNodeCount = 0;

        bool bIsEdgeExist = false;
        while(iterNestingTreeNodeToSubgraphOrderingGraph.hasNext())
        {
            iterNestingTreeNodeToSubgraphOrderingGraph.next();

            currentNestingTreeNode = iterNestingTreeNodeToSubgraphOrderingGraph.key();
            currentSubgraphOrderingGraph = iterNestingTreeNodeToSubgraphOrderingGraph.value();

            iChildNestingTreeNodeCount = currentNestingTreeNode->getCountOfChildNestingTreeSubgraphNodes();

            //Skip cycle removal of SubgraphOrderingGraph of nesting tree subgaph node which
            //have zero child nesting tree subgraph nodes
            if(iChildNestingTreeNodeCount > 0)
            {
                //Find list of back edges

                VectorSubgraphOrderingGraphEdgeDescriptor vecBackEdges;

                //create DFS visitor
                SubgraphOrderingGraphBackEdgeRecorder backEdgeRecorder(vecBackEdges);

                //Boost DFS
                depth_first_search(*currentSubgraphOrderingGraph , visitor(backEdgeRecorder));

                while(vecBackEdges.isEmpty() == false)
                {
                    //For every backEdge(vSource , vTarget)
                    //Find path from vTarget to vSource
                    //Break cycle path by deleting edge with the least most Average BarryCenter
                    //Find backEdges again if backedges found then repeat the above procedure

                    IteratorVectorSubgraphOrderingGraphEdgeDescriptor iterBackEdge(vecBackEdges);

                    while(iterBackEdge.hasNext())
                    {
                        SubgraphOrderingGraphEdgeDescriptor eBackEdge
                                = iterBackEdge.next();

                        //Sometimes cycle removal of previous backEdge removes another backEdge
                        //Therefor check that if gSubgaphOrderingGraph contains the backEdge or not
                        bIsEdgeExist = currentSubgraphOrderingGraph->find_edge(eBackEdge).second;

                        if(bIsEdgeExist == true)
                        {
                            //Break cycle created by eBackEdge at node which has least most average barrycenter
                            breakCycleAtSmallestAverageBarryCenterNode(eBackEdge , *currentSubgraphOrderingGraph);
                        }
                    }


                    //Clear back edge list
                    vecBackEdges.clear();

                    //Find if there are more cycles remained in graph
                    depth_first_search(*currentSubgraphOrderingGraph , visitor(backEdgeRecorder));
                }

            }

        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

void HierarchicalLayouter::breakCycleAtSmallestAverageBarryCenterNode(SubgraphOrderingGraphEdgeDescriptor eBackEdge, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph.find_edge(eBackEdge).second == true
                  , LayoutException(__FUNCTION__
                                    ,LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                    , "Subgraph Ordering Graph"
                                    , "Back Edge"));

    LAYOUT_ASSERT(source(eBackEdge , gSubgraphOrderingGraph) != target(eBackEdge , gSubgraphOrderingGraph)
            , LayoutException(__FUNCTION__
                              , LayoutExceptionEnum::INVALID_PARAMETER
                              , "edge with self loop"
                              , "Back Edge"));

    try
    {
        SubgraphOrderingGraphVertexDescriptor vSource = source(eBackEdge , gSubgraphOrderingGraph);
        SubgraphOrderingGraphVertexDescriptor vTarget = target(eBackEdge , gSubgraphOrderingGraph);

        //Find path from vTarget to vSource
        //Break cycle path by deleting edge with the least most Average BarryCenter


        //m_subgraphOrderingGraphWrapper.printGraph(gSubgraphOrderingGraph);

        //Iterate SubgraphOrderingGraph nodes in bfs manner

        //Reset all edges spanning tree boolean value to false
        BGL_FORALL_EDGES(eEdge , gSubgraphOrderingGraph , SubgraphOrderingGraphType)
        {
            m_subgraphOrderingGraphWrapper.setIsEdgeInSpanningTree(false , eEdge , gSubgraphOrderingGraph);
        }

        typedef QQueue<SubgraphOrderingGraphVertexDescriptor> QueueSubgraphOrderingGraphVertices;

        //Queue to contain upper layer vertices
        QueueSubgraphOrderingGraphVertices* qUpperLayerVertex = NULL;

        //Queue to contain direct child of upper level vertices which thus forms a lower level
        QueueSubgraphOrderingGraphVertices* qLowerLayerVertex = NULL;

        qUpperLayerVertex = new QueueSubgraphOrderingGraphVertices();

        //Add vTarget as the root vertex to start the breadth first search
        qUpperLayerVertex->enqueue(vTarget);

        SubgraphOrderingGraphVertexDescriptor vCurrentVertex = 0;
        SubgraphOrderingGraphVertexDescriptor vChildVertex = 0;

        int iTotalVertices = 0;
        iTotalVertices = num_vertices(gSubgraphOrderingGraph);

        //Vertex visited list
        QVector<bool> vecVertexVisited(iTotalVertices);
        std::fill(vecVertexVisited.begin() , vecVertexVisited.end() , false);

        //Mark first vertex visited
        vecVertexVisited[vTarget] = true;

        ////qDebug() << "Creating spanning tree";
        bool bIsSourceVertexReached = false;
        do
        {
            qLowerLayerVertex = new QueueSubgraphOrderingGraphVertices();

            while(qUpperLayerVertex->isEmpty() == false)
            {
                vCurrentVertex = qUpperLayerVertex->dequeue();

                BGL_FORALL_OUTEDGES(vCurrentVertex , eOutEdge , gSubgraphOrderingGraph , SubgraphOrderingGraphType)
                {
                    vChildVertex = target(eOutEdge , gSubgraphOrderingGraph);

                    if(vecVertexVisited[vChildVertex] == true)
                    {
                        //Skip visited vertex to avoid infinite loop
                        continue;
                    }
                    else
                    {
                        //Mark vertex visited
                        vecVertexVisited[vChildVertex] = true;
                    }

                    //Mark edge as spanning tree edge
                    m_subgraphOrderingGraphWrapper.setIsEdgeInSpanningTree(true , eOutEdge , gSubgraphOrderingGraph);

                    //Enque child vertices in queue of LowerLayerVertices
                    qLowerLayerVertex->enqueue(vChildVertex);

                    ////qDebug()<<vChildVertex;
                    if(vChildVertex == vSource)
                    {
                        bIsSourceVertexReached = true;
                        //Clear both queues
                        qUpperLayerVertex->clear();
                        qLowerLayerVertex->clear();


                        break;
                    }
                }

            }

            DELETE_AND_SET_NULL(qUpperLayerVertex);

            //Set queue of child vertices to be the queue of next upper layer vertices
            qUpperLayerVertex = qLowerLayerVertex;

            qLowerLayerVertex = NULL;

        }while(qUpperLayerVertex->isEmpty() == false && bIsSourceVertexReached == false);

        DELETE_AND_SET_NULL(qUpperLayerVertex);



        //Sometimes cycle removal of previous backEdges breaks one or more cycles
        //So in such case we just stop this cycle removal process
        //This can be detected if the source vertex is not reached through
        //the bfs traversal in above loop which starts from target vertex

        if(bIsSourceVertexReached == true)
        {
            //Now, the edges from root vTarget vertex to vSource vertex are marked as SpanningTreeEdges
            //So traverse edges in reverse direction from vSource till vTarget to find the path between them
            //Then meanwhile find vertex with minimum AverageBarryCenter Value -lets call it vSmallestBaryCenterVertex
            //Delete edge from path which has target as vSmallestBaryCenterVertex


            SubgraphOrderingGraphVertexDescriptor vParentVertex;

            //Together vSourceOfEdgeToDelete and vTargetOfEdgeToDelete forms edge to delete to break cycle

            //This vertex should contain the vertex with smallest average barrycenter
            SubgraphOrderingGraphVertexDescriptor vTargetOfEdgeToDelete;

            //This vertex should contain vertex which serves as source vertex for the vTargetOfEdgeToDelete
            SubgraphOrderingGraphVertexDescriptor vSourceOfEdgeToDelete;

            double dMinAverageBarryCenter = INT_MAX;
            double dCurrentVertexAverageBarryCenter = 0.0;
            bool bIsSpanningTreeEdge = false;
            int iCountSpanningTreeInEdges = 0;

            //Set current vertex to be the start and end of cycle i.e. vTarget vertex
            vCurrentVertex = vTarget;
            //Mark the backedge as in spanning tree = true
            m_subgraphOrderingGraphWrapper.setIsEdgeInSpanningTree(true , eBackEdge , gSubgraphOrderingGraph);

            ////qDebug()<<"BackEdge: "<<(int)vSource<<" , "<<(int)vTarget;
            ////qDebug()<<"#Path:";
            do
            {
                dCurrentVertexAverageBarryCenter =
                        getSubgraphOrderingGraphVertexAverageBarryCenter(vCurrentVertex , gSubgraphOrderingGraph);

                //Find in vParentVertex of current vertex by iterating inEdges and
                //finding inEdge with spanning tree property marked as 'true'

                iCountSpanningTreeInEdges = 0;

                ////qDebug() << vCurrentVertex;

                BGL_FORALL_INEDGES(vCurrentVertex , eInEdge , gSubgraphOrderingGraph , SubgraphOrderingGraphType)
                {
                    //eInEdge must have be spanning tree edge property value - true
                    bIsSpanningTreeEdge
                            = m_subgraphOrderingGraphWrapper.getIsEdgeInSpanningTree(eInEdge
                                                                                     , gSubgraphOrderingGraph);
                    if(bIsSpanningTreeEdge == true)
                    {
                        iCountSpanningTreeInEdges++;
                        vParentVertex = source(eInEdge , gSubgraphOrderingGraph);
                    }
                }

                //Every vertex must have only one inEdge with isSpanningTree property value as 'true'


                if(iCountSpanningTreeInEdges == 0)
                {
                    ////qDebug() << "Invalid in edge count: "<<iCountSpanningTreeInEdges;

                    return;
                }

                if(dCurrentVertexAverageBarryCenter < dMinAverageBarryCenter)
                {
                    //Update min average barry center
                    dMinAverageBarryCenter = dCurrentVertexAverageBarryCenter;

                    vTargetOfEdgeToDelete = vCurrentVertex;
                    vSourceOfEdgeToDelete = vParentVertex;

                }

                vCurrentVertex = vParentVertex;

            }while(vCurrentVertex != vTarget);



            //Delete edge to break cycle
            remove_edge(vSourceOfEdgeToDelete , vTargetOfEdgeToDelete , gSubgraphOrderingGraph);

            ////qDebug() << "Remove cycle edge: "<<(int)vSourceOfEdgeToDelete<<" , "<<
                        (int)vTargetOfEdgeToDelete;
        }//END_IF Source vertex is reached or not
        else
        {
            //source vertex is not reached
            ////qDebug() << "Cycle is removed already ";
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

int HierarchicalLayouter::assignTopologicalOrderAccordingToSubgraphOrderingGraph(int iStartOrder, NestingTreeSubgraphNode *nestingTreeNode)
{
    int iTopologicalOrder = 0;
    try
    {
        LAYOUT_ASSERT(iStartOrder > 0 , LayoutException(__FUNCTION__
                                                        , LayoutExceptionEnum::INVALID_PARAMETER
                                                        , QString::number(iStartOrder)
                                                        , "Start Order for topological order of subgraph ordering graph"));

        LAYOUT_ASSERT(m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.contains(nestingTreeNode)
                      , LayoutException(__FUNCTION__
                                        , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                        , "Nesting Tree Node"
                                        , "Map of Nesting Tree Nodes to Subgraph Ordering Graph"));

        //If nesting tree node does not have any child nesting tree node then assign
        //same order to all vertices the corresponding SubgraphOrderingGraph it contain.
        //Otherwise assign order values in bfs order, while assigning 'order' if we have
        //choice for several nodes then assign 'order' in ascending order of AverageBarryCenterValues

        //Map type 'AverageBArryCenter' value to 'SubgraphOrderingGraphVertex (SOG Vertex)'
        typedef QMap<double , SubgraphOrderingGraphVertexDescriptor> MapAverageBarryCenterToSOGVertex;
        typedef QMapIterator<double , SubgraphOrderingGraphVertexDescriptor> IteratorMapAverageBarryCenterToSOGVertex;
        typedef QQueue<SubgraphOrderingGraphVertexDescriptor> QueueSubgraphOrderingGraphVertex;

        int iCountOfNestingTreeChildNode = nestingTreeNode->getCountOfChildNestingTreeSubgraphNodes();

        SubgraphOrderingGraphType* gSubgraphOrderingGraph = NULL;
        LayerNode* currentLayerNode = NULL;
        VertexDescriptor vGlobalGraphVertex = 0;
        SubgraphOrderingGraphVertexDescriptor vSubgraphOrderingGraphVertex = 0;

        bool bIsVertexLayerNode = false;

        iTopologicalOrder = iStartOrder;

        int iNextTopologicalOrderValue = 0;

        //Get SubgraphOrderingGraph nestingTreeNode pointing to.
        gSubgraphOrderingGraph
                = m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.value(nestingTreeNode);

        //Testing
        //////qDebug() << "Assigning topological order";
        //m_subgraphOrderingGraphWrapper.printGraph(*gSubgraphOrderingGraph);

        if(iCountOfNestingTreeChildNode == 0)
        {
            //////qDebug() << "Leaf Nesting Tree Node";
            //If nesting tree node does not have any child nesting tree node then assign
            //same order to all LayerNodes vertices it contain.
            BGL_FORALL_VERTICES(vVertex , *gSubgraphOrderingGraph , SubgraphOrderingGraphType)
            {
                bIsVertexLayerNode = m_subgraphOrderingGraphWrapper.isSubgraphOrderingVertexIsLayerNode(
                            vVertex , *gSubgraphOrderingGraph);
                LAYOUT_ASSERT(bIsVertexLayerNode == true, LayoutException(__FUNCTION__
                                                                          , LayoutExceptionEnum::INVALID_PARAMETER
                                                                          , "it must be LayerNode"
                                                                          , "vVertex from SubgraphOrderingGraphType"));


                currentLayerNode = m_subgraphOrderingGraphWrapper.getVertexLayerNodeRef(vVertex , *gSubgraphOrderingGraph);

                vGlobalGraphVertex = currentLayerNode->getVertex();


                //Assign Topological order to vertex
                m_BoostGraphWrapper.setVertexTopologicalOrder(iTopologicalOrder ,  vGlobalGraphVertex , *m_gMainGraph);

                //Later update this change in layered graph
                //Do not increament iTopological Order
            }
            //Increase the topological order to make it StartOrder value of another node to which it might be passed
            iTopologicalOrder++;

            LAYOUT_ASSERT(iTopologicalOrder == (iStartOrder + 1)
                          , LayoutException(__FUNCTION__
                                            , LayoutExceptionEnum::INVALID_OPERATION
                                            , "assigning topological order to leaf subgraph ordering graph failed"
                                            , "Assigning Topological Order to whole tree of subgraph ordering graphs"));
        }
        else
        {
            //////qDebug() << "Nonleaf Nestnig Tree Node";
            //assign order values in BFS Order, while assigning 'order' if we have
            //choice for several nodes then assign 'order' in ascending order of AverageBarryCenterValues
            QueueSubgraphOrderingGraphVertex* qUpperLayerVertex = NULL;
            QueueSubgraphOrderingGraphVertex* qLowerLayerVertex = NULL;

            MapAverageBarryCenterToSOGVertex mapBarrycenterToAdjacentVertices;

            NestingTreeSubgraphNode* currentNestingTreeNode = NULL;

            int iTotalVertices = 0;
            iTotalVertices = num_vertices(*gSubgraphOrderingGraph);

            QVector<bool> vecQueuedSubgraphOrderingGraphVertex(iTotalVertices);
            std::fill(vecQueuedSubgraphOrderingGraphVertex.begin() , vecQueuedSubgraphOrderingGraphVertex.end() , false);

            double dAverageBarryCenter = 0.0;


            //Find the vertices with 0 in_degree to be in first layer
            BGL_FORALL_VERTICES(vVertex , *gSubgraphOrderingGraph , SubgraphOrderingGraphType)
            {
                if(in_degree(vVertex , *gSubgraphOrderingGraph) == 0)
                {
                    dAverageBarryCenter = getSubgraphOrderingGraphVertexAverageBarryCenter(
                                vVertex , *gSubgraphOrderingGraph);

                    //Add to first layer
                    mapBarrycenterToAdjacentVertices.insertMulti(dAverageBarryCenter , vVertex);

                    //Mark vertex as added to queue - mapUpperLayerVertex in this case
                    vecQueuedSubgraphOrderingGraphVertex[vVertex] = true;
                }
            }

            qUpperLayerVertex = new QueueSubgraphOrderingGraphVertex();

            //Fill the qUpperLayerVertex to start breadth first search
            IteratorMapAverageBarryCenterToSOGVertex iterFirstLayerBarryCenterToVertex(mapBarrycenterToAdjacentVertices);
            while(iterFirstLayerBarryCenterToVertex.hasNext())
            {
                iterFirstLayerBarryCenterToVertex.next();

                vSubgraphOrderingGraphVertex = iterFirstLayerBarryCenterToVertex.value();

                qUpperLayerVertex->enqueue(vSubgraphOrderingGraphVertex);
            }

            //Breadth First Search traversal of SubgraphOrderingGraph pointed by provided nestingTreeNode
            do
            {
                qLowerLayerVertex = new QueueSubgraphOrderingGraphVertex();

                while(qUpperLayerVertex->isEmpty() == false)
                {
                    vSubgraphOrderingGraphVertex = qUpperLayerVertex->dequeue();

                    bIsVertexLayerNode = m_subgraphOrderingGraphWrapper.isSubgraphOrderingVertexIsLayerNode(
                                vSubgraphOrderingGraphVertex , *gSubgraphOrderingGraph);

                    if(bIsVertexLayerNode == true)
                    {
                        //Get layerNode for current SubgraphOrderingGraph vertex
                        currentLayerNode = m_subgraphOrderingGraphWrapper.getVertexLayerNodeRef(vSubgraphOrderingGraphVertex , *gSubgraphOrderingGraph);

                        //Get global graph vertex descriptor from layerNode
                        vGlobalGraphVertex = currentLayerNode->getVertex();

                        //Assign Topological order to graph vertex
                        m_BoostGraphWrapper.setVertexTopologicalOrder(iTopologicalOrder ,  vGlobalGraphVertex , *m_gMainGraph);


                        //Increase iTopologicalOrder
                        iTopologicalOrder++;

                    }
                    else
                    {
                        //The subgraph ordering graph vertex is a NestingTreeNode
                        currentNestingTreeNode = m_subgraphOrderingGraphWrapper.getVertexNestingTreeSubgraphNode(
                                    vSubgraphOrderingGraphVertex , *gSubgraphOrderingGraph);

                        //Recursive call
                        iNextTopologicalOrderValue = assignTopologicalOrderAccordingToSubgraphOrderingGraph(
                                    iTopologicalOrder , currentNestingTreeNode);

                        iTopologicalOrder = iNextTopologicalOrderValue;
                    }


                    //Fill mapLowerLayer with child (next) vertices of  current vSubgraphOrderingGraphVertex
                    //To get them sorted on theeir average barrycenter position
                    mapBarrycenterToAdjacentVertices.clear();

                    BGL_FORALL_ADJ(vSubgraphOrderingGraphVertex , vAdjacentVertex , *gSubgraphOrderingGraph , SubgraphOrderingGraphType)
                    {
                        //Skip vertices which are already added to queue
                        if(vecQueuedSubgraphOrderingGraphVertex[vAdjacentVertex] == true)
                        {
                            //Skip
                            continue;
                        }
                        else
                        {
                            //Mark the vertex as added in queue
                            vecQueuedSubgraphOrderingGraphVertex[vAdjacentVertex] = true;
                        }

                        //Get average barry center
                        dAverageBarryCenter = getSubgraphOrderingGraphVertexAverageBarryCenter(
                                    vAdjacentVertex , *gSubgraphOrderingGraph);

                        //Insert adjacent cartex into Lower layer vertices map
                        mapBarrycenterToAdjacentVertices.insertMulti(dAverageBarryCenter , vAdjacentVertex);
                    }

                    //Put vertices from map to qLowerLayerVertices
                    IteratorMapAverageBarryCenterToSOGVertex iterAdjacentVertices(mapBarrycenterToAdjacentVertices);
                    while(iterAdjacentVertices.hasNext())
                    {
                        iterAdjacentVertices.next();

                        SubgraphOrderingGraphVertexDescriptor vAdjacentVertex = iterAdjacentVertices.value();
                        //Fill qLowerLayer
                        qLowerLayerVertex->enqueue(vAdjacentVertex);

                    }


                }//Iterate upper layer vertices End


                //Make LowerLayer queue of adjacent(child) vertices to be the queue of next upperLayer vertices
                qUpperLayerVertex = qLowerLayerVertex;

                qLowerLayerVertex = NULL;

            }while(qUpperLayerVertex->isEmpty() == false);


        }
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

    return iTopologicalOrder;
}

void HierarchicalLayouter::updateAverageBarryCenterForNestingTreeNodes()
{
    try
    {
        LAYOUT_ASSERT(m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef.isEmpty() == false,
                      LayoutException(__FUNCTION__
                                      , LayoutExceptionEnum::EMPTY_CONTAINER
                                      , "Map must not be empty"
                                      , "mapNestingTreeNodeRefToSubgraphOrderingGraphRef"));

        double dAverageBarryCenter = 0.0;

        IteratorMapNestingTreeRefToSubgraphOrderingGraphRef iterNestingTreeNodeToSubgraphOrderingGraph(
                    m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef);

        NestingTreeSubgraphNode* currentNestingTreeNode = NULL;

        while(iterNestingTreeNodeToSubgraphOrderingGraph.hasNext())
        {
            iterNestingTreeNodeToSubgraphOrderingGraph.next();

            currentNestingTreeNode = iterNestingTreeNodeToSubgraphOrderingGraph.key();

            //Skip root nesting tree Subgraph Node
            if(currentNestingTreeNode->isRoot() == false)
            {

                //Recalculate and update averageBarryCenter of nesting tree node
                dAverageBarryCenter = currentNestingTreeNode->updateAverageBarryCenter();
            }

        }
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

double HierarchicalLayouter::getSubgraphOrderingGraphVertexAverageBarryCenter(SubgraphOrderingGraphVertexDescriptor vVertex, SubgraphOrderingGraphType &gSubgraphOrderingGraph)
{
    LAYOUT_ASSERT(gSubgraphOrderingGraph.find_vertex(vVertex).second == true, LayoutException(__FUNCTION__
                                                                                              , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                                                              , "gSubgraphOrderingGraph"
                                                                                              , "vVertex"));


    bool bIsLayerNodeVertex = m_subgraphOrderingGraphWrapper.isSubgraphOrderingVertexIsLayerNode(
                vVertex , gSubgraphOrderingGraph);

    double dAverageBarryCenter = 0.0;

    if(bIsLayerNodeVertex == true)
    {
        //The SubgraphOrderingGraphType vVertex is a LayerNode
        LayerNode* layerNode = m_subgraphOrderingGraphWrapper.getVertexLayerNodeRef(
                    vVertex , gSubgraphOrderingGraph);
        VertexDescriptor vGlobalVertex = layerNode->getVertex();

        dAverageBarryCenter = m_BoostGraphWrapper.getVertexHorizontalPosition(vGlobalVertex , *m_gMainGraph);
    }
    else
    {
        //The SubgraphOrderingGraphType vVertex is a NestingTreeSubgraph node
        NestingTreeSubgraphNode* nestingTreeNode
                = m_subgraphOrderingGraphWrapper.getVertexNestingTreeSubgraphNode(
                    vVertex , gSubgraphOrderingGraph);

        dAverageBarryCenter = nestingTreeNode->getAverageBarryCenter();

    }

    return dAverageBarryCenter;
}

void HierarchicalLayouter::addVerticalBorderNodesForSubgraphs()
{
    LAYOUT_ASSERT(m_iRankDifferenceInLayers.isSet() == true,
                  LayoutException(__FUNCTION__
                                  ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                  ,"Rank Difference in layers"
                                  ,""));
    try
    {

        //First update horizontal LayerNode positions to accomodate verticalBorderNodes
        setHorizontalPositionsForVerticalBorderNodes();

        //Recursively add Vertical Border nodes from innermost leaf NestingTreeNodes
        //to Root Nesting tree node
        addVerticalBorderNodesNestingTreeRecur(m_rootNestingTreeSubgraphNode);
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::addVerticalBorderNodesNestingTreeRecur(NestingTreeSubgraphNode &nestingTreeNode)
{
    try
    {
        //Map to contain all LayerNodes of current NestingTreeNode. This includes all LayerNodes of
        //its child NestingTree nodes hierarchy
        NestingTreeSubgraphNode::MultiMapLayerIdToLayerNodeRef mapLayerIdToLayerNode;

        int iLayerId = 0;
        int iHorizontalPosition = 0;
        int iMinHorizontalPosition = 0;
        int iMaxHorizontalPosition = 0;

        int iUpperBorderVertexLayerId = 0;
        int iLowerBorderVertexLayerId = 0;

        LayerNode* layerNode = NULL;
        VertexDescriptor vVertex = 0;

        VertexDescriptor vLeftVerticalBorder = 0;
        VertexDescriptor vRightVerticalBorder = 0;

        VertexDescriptor vPreviousLeftVerticalBorder = 0;
        VertexDescriptor vPreviousRightVerticalBorder = 0;

        VertexDescriptor vGlobalLeftVerticalBorder = 0;
        VertexDescriptor vGlobalRightVerticalBorder = 0;

        LayerNode* leftLayerNode = NULL;
        LayerNode* rightLayerNode = NULL;



        NestingTreeSubgraphNode::IteratorVectorNestingTreeSubgraphNodesRef iterChildNestingTreenodes
                = nestingTreeNode.getIteratorChildNestingTreeSubgraphNodes();

        NestingTreeSubgraphNode* childNestingTreeNode = NULL;
        while(iterChildNestingTreenodes.hasNext())
        {
            childNestingTreeNode = iterChildNestingTreenodes.next();
            //Recursive call
            addVerticalBorderNodesNestingTreeRecur(*childNestingTreeNode);

            //Accumulate all Layernode of childNestingTreeNode to mapLayerIdToLayerNode
            NestingTreeSubgraphNode::IteratorMultiMapLayerIdToLayerNodeRef iterChildLayerNode
                    = childNestingTreeNode->getChildLayerNodesIterator();
            while(iterChildLayerNode.hasNext())
            {
                iterChildLayerNode.next();

                //Add layer node
                mapLayerIdToLayerNode.insertMulti(iterChildLayerNode.key(),iterChildLayerNode.value());

            }

        }//Iterating child Nesting Tree End

        //Add own direct layer nodes to mapLayerIdToLayerNode
        NestingTreeSubgraphNode::MultiMapLayerIdToLayerNodeRef& mapOwnLayerNodes
                = nestingTreeNode.getMapLayerIdToLayerNodeRef();

        mapLayerIdToLayerNode += mapOwnLayerNodes;

        //Get upper and Lower border vertex ranks - LayerIds, because
        //to add VerticalBorderVertices for only own Border Vertices
        iUpperBorderVertexLayerId = nestingTreeNode.getMinRank();
        iLowerBorderVertexLayerId = nestingTreeNode.getMaxRank();


        //get unique layerIds in ascending order
        QList<int> listLayerIds = mapLayerIdToLayerNode.uniqueKeys();
        QListIterator<int> iterLayerId(listLayerIds);

        bool bIsFirstVerticalNode = true;

        while(iterLayerId.hasNext())
        {
            iLayerId = iterLayerId.next();

            //testing 42214-1 - consider only GraphNodeLayers
            if(iLayerId % m_iRankDifferenceInLayers != 0)//commented for 5914-1
            {
                //Inside- means iLayerId is a Upper or Lower Border Vertex Layer
                //Skip other subgraphs BorderLayers. Allow only current subgraph
                //border layer ids

                //testing 42214-2
                if(iLayerId != iUpperBorderVertexLayerId &&
                        iLayerId != iLowerBorderVertexLayerId)
                {
                    //Skip other subgraphs BorderLayers.
                    continue;//5914-1 commented for.
                }
            }

            iMinHorizontalPosition = INT_MAX;
            iMaxHorizontalPosition = INT_MIN;

            //Get LayerNodes with same LayerId in a list
            QList<LayerNode*> listLayer = mapLayerIdToLayerNode.values(iLayerId);

            QListIterator<LayerNode*> iterListLayerNode(listLayer);
            while(iterListLayerNode.hasNext())
            {
                layerNode = iterListLayerNode.next();

                vVertex = layerNode->getVertex();

                //Get horizontal position
                iHorizontalPosition = m_BoostGraphWrapper.getVertexHorizontalPosition(
                            vVertex , *m_gMainGraph);

                if(iHorizontalPosition < iMinHorizontalPosition)
                {
                    iMinHorizontalPosition = iHorizontalPosition;
                }

                if(iHorizontalPosition > iMaxHorizontalPosition)
                {
                    iMaxHorizontalPosition = iHorizontalPosition;
                }
            }

            SubGraph &gProperSubgraph = nestingTreeNode.getGraph();

            //*********Add VerticalBorder Vertex at position one minus the min horizontal position

            //Create vertex with VerticalBorderNode type
            vLeftVerticalBorder = m_BoostGraphWrapper.addVertex(gProperSubgraph , LayoutEnum::VerticalBorderNode);

            //Convert vLeftVerticalBorder index from local to global index to add to Layernode
            vGlobalLeftVerticalBorder = gProperSubgraph.local_to_global(vLeftVerticalBorder);

            //Set vertex rank
            m_BoostGraphWrapper.setVertexRank(vGlobalLeftVerticalBorder , *m_gMainGraph , iLayerId);


            //Add Layer Node for new VerticalBorderVertex
            leftLayerNode = new LayerNode(nestingTreeNode , vGlobalLeftVerticalBorder);

            //Add new Layer Node Entry to hashVertexToLayerNode
            hashVertexToLayerNode.insert(vGlobalLeftVerticalBorder , leftLayerNode);

            //Add Layer Node for new VerticalBorderVertex's entry to the Nesting Graph
            nestingTreeNode.addLayerIdAndLayerNode(iLayerId , leftLayerNode);

            //Set vertx Horizontal position
            m_BoostGraphWrapper.setVertexHorizontalPosition(vGlobalLeftVerticalBorder , *m_gMainGraph , (iMinHorizontalPosition - 1));

            //insert Layernode into layer
            LAYOUT_ASSERT((m_mapLayeredGraph[iLayerId]->contains((iMinHorizontalPosition - 1))) == false
                          ,LayoutException(__FUNCTION__
                                           ,LayoutExceptionEnum::INVALID_OPERATION
                                           ,"adding node at horizontal position which is already taken by another node at same layer"
                                           ,"Adding Vertical Border Node"));



            m_mapLayeredGraph[iLayerId]->insert(iMinHorizontalPosition - 1 , leftLayerNode);




            //*********Add VerticalBorder Vertex at position one plus the max horizontal position

            //Create vertex with VerticalBorderNode type
            vRightVerticalBorder = m_BoostGraphWrapper.addVertex(gProperSubgraph , LayoutEnum::VerticalBorderNode);

            //Convert vRightVerticalBorder index from local to global index to add to Layernode
            vGlobalRightVerticalBorder = gProperSubgraph.local_to_global(vRightVerticalBorder);

            //Set vertex rank
            m_BoostGraphWrapper.setVertexRank(vGlobalRightVerticalBorder , *m_gMainGraph , iLayerId);


            //Add Layer Node for new VerticalBorderVertex
            rightLayerNode = new LayerNode(nestingTreeNode , vGlobalRightVerticalBorder);

            //Add new Layer Node Entry to hashVertexToLayerNode
            hashVertexToLayerNode.insert(vGlobalRightVerticalBorder , rightLayerNode);

            //Add Layer Node for new VerticalBorderVertex's entry to the Nesting Graph
            nestingTreeNode.addLayerIdAndLayerNode(iLayerId , rightLayerNode);

            //Set vertx Horizontal position
            m_BoostGraphWrapper.setVertexHorizontalPosition(vGlobalRightVerticalBorder , *m_gMainGraph , (iMaxHorizontalPosition + 1));

            //insert Layernode into layer
            LAYOUT_ASSERT((m_mapLayeredGraph[iLayerId]->contains(iMaxHorizontalPosition + 1)) == false
                          ,LayoutException(__FUNCTION__
                                           ,LayoutExceptionEnum::INVALID_OPERATION
                                           ,"adding node at horizontal position which is already taken by another node at same layer"
                                           ,"Adding Vertical Border Node"));


            m_mapLayeredGraph[iLayerId]->insert(iMaxHorizontalPosition + 1 , rightLayerNode);




            //Add edges
            if(bIsFirstVerticalNode == false)
            {

                //Add Long Edge Segment
                m_BoostGraphWrapper.addEdge(vPreviousLeftVerticalBorder , vLeftVerticalBorder ,
                                            gProperSubgraph , LayoutEnum::VerticalBorderEdgeSegment);

                //Add Long Edge Segment
                m_BoostGraphWrapper.addEdge(vPreviousRightVerticalBorder , vRightVerticalBorder ,
                                            gProperSubgraph , LayoutEnum::VerticalBorderEdgeSegment);
            }

            if(bIsFirstVerticalNode == true)
            {
                bIsFirstVerticalNode = false;
            }

            //Set current Vertical Border Vertices as
            //Previous Vertices to add edge between them later

            vPreviousLeftVerticalBorder = vLeftVerticalBorder;

            vPreviousRightVerticalBorder = vRightVerticalBorder;
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }


}

void HierarchicalLayouter::addDummyNodeToEmptyLayersRecur(NestingTreeSubgraphNode &rootNestingTreeNode)
{
    //Map to contain all LayerNodes of current NestingTreeNode. This includes all LayerNodes of
    //its child NestingTree nodes hierarchy
//    NestingTreeSubgraphNode::MultiMapLayerIdToLayerNodeRef mapLayerIdToLayerNode;

    int iLayerId = 0;
    int iDummyNodeHorizontalPosition = 0;


    VertexDescriptor vDummyVertex = 0;

    VertexDescriptor vGlobalDummyVertex = 0;

    LayerNode* dummyLayerNode = NULL;

    try
    {

        NestingTreeSubgraphNode::IteratorVectorNestingTreeSubgraphNodesRef iterChildNestingTreenodes
                = rootNestingTreeNode.getIteratorChildNestingTreeSubgraphNodes();

        NestingTreeSubgraphNode* childNestingTreeNode = NULL;
        while(iterChildNestingTreenodes.hasNext())
        {
            childNestingTreeNode = iterChildNestingTreenodes.next();
            //Recursive call
            addDummyNodeToEmptyLayersRecur(*childNestingTreeNode);

        }//Iterating child Nesting Tree End

        /*Find list of empty layer ids
         *
         *A graphs child layer nodes are not above its UpperBorderVertex and not below its
         *LowerBorderVertex. Therefor number of layers a graph covers are layers between
         *Upper and Lower Border Vertices
         */

        VertexDescriptor vUpperBorderVertex = 0;
        VertexDescriptor vLowerBorderVertex = 0;

        int iUpperLayerId = 0;
        int iLowerLayerId = 0;

        int iLastHorizontalPosition = 0;

        bool bIsContainLayerId = true;
        SubGraph& gSubgraph = rootNestingTreeNode.getGraph();


        iUpperLayerId = rootNestingTreeNode.getMinRank();

        iLowerLayerId = rootNestingTreeNode.getMaxRank();


        /*Add dummy node in empty layers
         *
         *Every nestingTreeNode records which LayerId layerNodes its hierarchy of nesting tree contains
         *So if any LayerId is not present in that hash of LayerIds the layer is empty
         */

        MapPositionToLayerNode::iterator iterLayerNode;

        MapLayerIdToLayerRef::iterator iterLayer , iterLayerEnd;

        iterLayer = m_mapLayeredGraph.find(iUpperLayerId);
        iterLayerEnd = m_mapLayeredGraph.find(iLowerLayerId);

        for(;iterLayer != iterLayerEnd ; iterLayer++)
        {
            iLayerId = iterLayer.key();

            //Testing 42214-1 Consider only GraphNodeLayers
            if(iLayerId % m_iRankDifferenceInLayers != 0)
            {
                continue;
            }

            bIsContainLayerId = rootNestingTreeNode.doesSubgraphNodeContainLayerId(iLayerId);

            ////qDebug() << iLayerId;

            if(bIsContainLayerId == false)
            {
                ////qDebug()<<"Empty Layer: "<<iLayerId;

                //Add dummy node to the empty layer
                //Create vertex with VerticalBorderNode type
                vDummyVertex = m_BoostGraphWrapper.addVertex(gSubgraph , LayoutEnum::DummyNode);

                //Convert vLeftVerticalBorder index from local to global index to add to Layernode
                vGlobalDummyVertex = gSubgraph.local_to_global(vDummyVertex);

                //Set vertex rank
                m_BoostGraphWrapper.setVertexRank(vGlobalDummyVertex , *m_gMainGraph , iLayerId);


                //Add Layer Node for new VerticalBorderVertex
                dummyLayerNode = new LayerNode(rootNestingTreeNode , vGlobalDummyVertex);

                //Add new Layer Node Entry to hashVertexToLayerNode
                hashVertexToLayerNode.insert(vGlobalDummyVertex , dummyLayerNode);

                //Add Layer Node for new VerticalBorderVertex's entry to the Nesting Graph
                rootNestingTreeNode.addLayerIdAndLayerNode(iLayerId , dummyLayerNode);

                //Get last horizontal position in the layer
                iterLayerNode = m_mapLayeredGraph[iLayerId]->end();
                iterLayerNode--;
                iLastHorizontalPosition = iterLayerNode.key();

                //Set vertex next position to the last horizontal position as it will be
                //changed while SubgraphOrderingGraph Crossing Reduction

                iDummyNodeHorizontalPosition = iLastHorizontalPosition + 1;

                m_BoostGraphWrapper.setVertexHorizontalPosition(vGlobalDummyVertex , *m_gMainGraph ,iDummyNodeHorizontalPosition);

                //insert Layernode into layer
                LAYOUT_ASSERT((m_mapLayeredGraph[iLayerId]->contains(iDummyNodeHorizontalPosition)) == false
                              ,LayoutException(__FUNCTION__
                                               ,LayoutExceptionEnum::INVALID_OPERATION
                                               ,"inserting Layernode into layer at position which is already taken by other node"
                                               ,"Adding Dummy Node to Empty Layers"));

                m_mapLayeredGraph[iLayerId]->insertMulti(iDummyNodeHorizontalPosition , dummyLayerNode);


            }

        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}



void HierarchicalLayouter::assignYCoordinates(SubGraph &gMainGraph)
{
    //"Applying Center Coordinates from rank and position" , "gMainGraph is not main root graph"
    LAYOUT_ASSERT(gMainGraph.is_root() == true , LayoutException(__FUNCTION__
                                                                 , LayoutExceptionEnum::INVALID_PARAMETER
                                                                 , "Main Graph"
                                                                 , "MainGraph is not main root graph"));
    try
    {
        PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexHorizontalPosition , int , iHorizontalPosition , gMainGraph);
        PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexRank , int , iRank , gMainGraph);

        PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexBarryCenter , double , dBarryCenter , gMainGraph);

        int iMaxHeight = 0;
        int iLayerNodeHeight = 0;
        int dBarryCenter = 0;

        int iYCoordinate = 0;
        int iVerticalStep = 0;

        int iHalfHeight = 0;
        int iPreviousHalfHeight = 0;

        int iMaxOutDegree = 0;
        int iMaxInDegree = 0;

        int iOutDegree = 0;
        int iInDegree = 0;

        int iInDegreeSpace  = 0;
        int iOutDegreeSpace = 0;
        int iPreviousOutDegreeSpace = 0;

        bool bIsBorderLayer = false;
        bool bIsPrevBorderLayer = true;

        LayerNode* layerNode = NULL;
        VertexDescriptor vLayerVertex = 0;

        QMap<int,int> mapLayerIdToHalfHeight;

        QMap<int,int> mapLayerIdToYCoord;

        Q_UNUSED(dBarryCenter);

        //Set y coordinates according to node heights
        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);



        while(iterLayer.hasNext())
        {
            iterLayer.next();

            //Skip border layers
            if(iterLayer.key() % m_iRankDifferenceInLayers != 0)
            {
                //continue;
            }

            //42314-2
            //iMaxLayerOutDegree = 0;

            //42414-1
            iMaxOutDegree = INT_MIN;

            iMaxInDegree = INT_MIN;

            iInDegreeSpace = 0;
            iOutDegreeSpace = 0;

            iMaxHeight = INT_MIN;
            IteratorMapPositionToLayerNode iterLayerNode(*(iterLayer.value()));
            while(iterLayerNode.hasNext())
            {
                iterLayerNode.next();

                layerNode = iterLayerNode.value();

                vLayerVertex = layerNode->getVertex();

                iLayerNodeHeight = m_BoostGraphWrapper.getVertexHeight(vLayerVertex , *m_gMainGraph);

                //Testing 42414-1 - Maximum out degree
                iOutDegree = out_degree(vLayerVertex , *m_gMainGraph);//42314-2
                iMaxOutDegree = (iMaxOutDegree < iOutDegree) ? iOutDegree : iMaxOutDegree;
                //            iMaxOutDegree += iOutDegree;

                //Maximum In degree
                iInDegree = in_degree(vLayerVertex , *m_gMainGraph);//42414-1
                iMaxInDegree = (iMaxInDegree < iInDegree) ? iInDegree : iMaxInDegree;
                //            iMaxInDegree += iInDegree;

                if(iLayerNodeHeight > iMaxHeight)
                {
                    iMaxHeight = iLayerNodeHeight;
                }
            }


            if(iterLayer.key() % m_iRankDifferenceInLayers != 0)
            {
                bIsBorderLayer = true;
                //            iMaxOutDegree = 2;
            }
            else
            {
                bIsBorderLayer = false;
            }


            iHalfHeight = iMaxHeight / 2;

            //Store half heights of each graph layer for later use
            mapLayerIdToHalfHeight[iterLayer.key()] = iHalfHeight;


            //Increament y coordinate by half height because every layer must have atleast
            //its max half height space to draw every node on it half above and half below
            //the ycoordinate
            iYCoordinate += iHalfHeight + iPreviousHalfHeight;

            //calculate current vertical step
            iVerticalStep = iHalfHeight + iPreviousHalfHeight;


            //Add previous outdegree space
            iYCoordinate += iPreviousOutDegreeSpace;



            //42414-2
            iInDegreeSpace = iMaxInDegree * (m_iEdgeSeparation);

            //Update y coordinate from iInDegreeSpace by comparing iInDegreeSpace and
            //iPreviousOutDegreeSpace value. If iPreviousOutDegreeSpace is larger than
            //iInDegreeSpace then no need to increase y coordinate by iInDegreeSpace.
            //Otherwise increase y by iInDegreeSpace - iPreviousOutDegreeSpace.
            if(iPreviousOutDegreeSpace < iInDegreeSpace )
            {
                iYCoordinate += (iInDegreeSpace - iPreviousOutDegreeSpace);

                //Update vertical step
                iVerticalStep += (iInDegreeSpace - iPreviousOutDegreeSpace);

            }


            if(iVerticalStep < m_iRankSeparation)
            {
                //if(bIsBorderLayer == false && bIsPrevBorderLayer == false)
                {
                    //Make minimum rank separation
                    iYCoordinate += m_iRankSeparation;
                }

            }

            //Store y coord for later use
            mapLayerIdToYCoord[iterLayer.key()] = iYCoordinate;

            //Set y coordinate
            IteratorMapPositionToLayerNode iterLayerNodeToAssignYcoord(*(iterLayer.value()));

            while(iterLayerNodeToAssignYcoord.hasNext())
            {
                iterLayerNodeToAssignYcoord.next();

                layerNode = iterLayerNodeToAssignYcoord.value();

                vLayerVertex = layerNode->getVertex();

                //            //if(out_degree(vLayerVertex , *m_gMainGraph) > in_degree(vLayerVertex , *m_gMainGraph))
                //            {
                //                iSpecificYCoordinate += (iMaxOutDegree - out_degree(vLayerVertex , *m_gMainGraph))*m_iEdgeSeparation;
                //            }
                //            iSpecificYCoordinate -= (iMaxInDegree - in_degree(vLayerVertex , *m_gMainGraph))*m_iEdgeSeparation;


                m_BoostGraphWrapper.setVertexCenterCoordY(vLayerVertex , *m_gMainGraph , iYCoordinate);
            }





            //42314-2
            iOutDegreeSpace = (iMaxOutDegree)* (m_iEdgeSeparation);


            //if(!bIsBorderLayer)
            iPreviousOutDegreeSpace = iOutDegreeSpace;

            iPreviousHalfHeight = iHalfHeight;

            bIsPrevBorderLayer = bIsBorderLayer;
        }




        //Assign y coordinates to border layers of each graph separately
        //give y coords from innermost graph
        QQueue<SubGraph*> qSubgraphs;
        qSubgraphs.enqueue(m_gMainGraph);
        QStack<SubGraph*> stackSubgraph;
        SubGraph* gGraph = NULL;
        while(qSubgraphs.isEmpty() == false)
        {
            gGraph = qSubgraphs.dequeue();
            stackSubgraph.push(gGraph);

            ChildrenIterator iterChild , iterChildEnd;
            for(boost::tie(iterChild , iterChildEnd) = gGraph->children();
                iterChild != iterChildEnd;
                iterChild++)
            {
                SubGraph* gChildGraph = &(*iterChild);
                qSubgraphs.enqueue(gChildGraph);
            }
        }

        VertexDescriptor vUpperBorder = 0;
        VertexDescriptor vLowerBorder = 0;
        VertexDescriptor vLeftVerticalBorder = 0;
        VertexDescriptor vRightVerticalBorder = 0;
        int iBorderLayerRank = 0;
        int iNextLayerId = 0;
        int iPrevLayerId = 0;
        int iYCoordNextLayer = 0;
        int iYCoordPrevLayer = 0;
        VertexDescriptor vVertex = 0;
        Q_UNUSED(vVertex);
        int iHorizontalPos = 0;

        MapLayerIdToLayerRef::iterator iterGraphLayer;
        MapPositionToLayerNode::iterator iterLayerNode;

        ////qDebug()<<"Setting border layer y coordinate";
        while(stackSubgraph.isEmpty() == false)
        {
            gGraph = stackSubgraph.pop();

            //Get upperborder and lower border vertex
            vUpperBorder = m_BoostGraphWrapper.getGraphUpperBorderVertex(*gGraph);
            vLowerBorder = m_BoostGraphWrapper.getGraphLowerBorderVertex(*gGraph);

            vUpperBorder = gGraph->local_to_global(vUpperBorder);
            vLowerBorder = gGraph->local_to_global(vLowerBorder);

            //find layer of upper border vertex
            iBorderLayerRank = m_BoostGraphWrapper.getVertexRank(vUpperBorder , *m_gMainGraph);
            ////qDebug()<<"Upper border: "<<iBorderLayerRank;

            //Get next layer id
            iterGraphLayer = m_mapLayeredGraph.find(iBorderLayerRank);
            iterGraphLayer++;

            iNextLayerId = iterGraphLayer.key();

            //Get next layer y coordinate
            ////qDebug()<<"Next layer id: "<<iNextLayerId;
            LAYOUT_ASSERT(mapLayerIdToYCoord.contains(iNextLayerId) , LayoutException(__FUNCTION__
                                                                                      , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                                                      , "Y Coordinate of next layer"
                                                                                      , ""));
            iYCoordNextLayer = mapLayerIdToYCoord[iNextLayerId];


            //Get next layer max vertex height
            LAYOUT_ASSERT(mapLayerIdToHalfHeight.contains(iNextLayerId), LayoutException(__FUNCTION__
                                                                                         , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                                                         , "Half height of next layer"
                                                                                         , ""));
            iHalfHeight = mapLayerIdToHalfHeight[iNextLayerId];

            //Get neighbor vertical border vertices
            iHorizontalPos = m_BoostGraphWrapper.getVertexHorizontalPosition(vUpperBorder , *m_gMainGraph);
            iterLayerNode = m_mapLayeredGraph[iBorderLayerRank]->find(iHorizontalPos);

            iterLayerNode--;
            vLeftVerticalBorder = (iterLayerNode.value())->getVertex();

            iterLayerNode++;
            iterLayerNode++;
            vRightVerticalBorder = (iterLayerNode.value())->getVertex();

            //Calculate y coord for upper border for gGraph
            iYCoordinate = iYCoordNextLayer - iHalfHeight - 40;

            //assign y coord to upper border vertex and neighbor vertical border nodes
            m_BoostGraphWrapper.setVertexCenterCoordY(vUpperBorder , *m_gMainGraph , iYCoordinate);
            m_BoostGraphWrapper.setVertexCenterCoordY(vLeftVerticalBorder , *m_gMainGraph , iYCoordinate);
            m_BoostGraphWrapper.setVertexCenterCoordY(vRightVerticalBorder , *m_gMainGraph , iYCoordinate);

            mapLayerIdToHalfHeight[iBorderLayerRank] = 0;
            mapLayerIdToYCoord[iBorderLayerRank] = iYCoordinate;


            //find layer of lower border vertex
            //assign y coord to lower border vertex and neighbor vertical border nodes

            //find layer of Lower border vertex
            iBorderLayerRank = m_BoostGraphWrapper.getVertexRank(vLowerBorder , *m_gMainGraph);

            //Get prev layer id
            iterGraphLayer = m_mapLayeredGraph.find(iBorderLayerRank);
            iterGraphLayer--;

            iPrevLayerId = iterGraphLayer.key();

            //Get prev layer y coordinate
            LAYOUT_ASSERT(mapLayerIdToYCoord.contains(iPrevLayerId) , LayoutException(__FUNCTION__
                                                                                      , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                                                      , "Y Coordinate of next layer"
                                                                                      , ""));
            iYCoordPrevLayer = mapLayerIdToYCoord[iPrevLayerId];

            //Get prev layer max vertex height
            LAYOUT_ASSERT(mapLayerIdToHalfHeight.contains(iPrevLayerId) , LayoutException(__FUNCTION__
                                                                                          , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                                                          , "Half height of next layer"
                                                                                          , ""));
            iHalfHeight = mapLayerIdToHalfHeight[iPrevLayerId];

            //Get neighbor vertical border vertices
            iHorizontalPos = m_BoostGraphWrapper.getVertexHorizontalPosition(vLowerBorder , *m_gMainGraph);
            iterLayerNode = m_mapLayeredGraph[iBorderLayerRank]->find(iHorizontalPos);

            iterLayerNode--;
            vLeftVerticalBorder = (iterLayerNode.value())->getVertex();

            iterLayerNode++;
            iterLayerNode++;
            vRightVerticalBorder = (iterLayerNode.value())->getVertex();

            //Calculate y coord for Lower border for gGraph
            iYCoordinate = iYCoordPrevLayer + iHalfHeight + 40;

            //assign y coord to Lower border vertex and neighbor vertical border nodes
            m_BoostGraphWrapper.setVertexCenterCoordY(vLowerBorder , *m_gMainGraph , iYCoordinate);
            m_BoostGraphWrapper.setVertexCenterCoordY(vLeftVerticalBorder , *m_gMainGraph , iYCoordinate);
            m_BoostGraphWrapper.setVertexCenterCoordY(vRightVerticalBorder , *m_gMainGraph , iYCoordinate);

            mapLayerIdToHalfHeight[iBorderLayerRank] = 0;
            mapLayerIdToYCoord[iBorderLayerRank] = iYCoordinate;


        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }



}

void HierarchicalLayouter::setSubgraphCompartmentProperties()
{
    //Get coordinates of left and right - uppermost and lower most vertical border vertices of current
    //subgraph using its NestingTreeNode - we get four vertical border nodes
    //Find out min X and min Y as the LeftTop coordinate of compartment
    //Find out height and width using the dx and dy of four border vertices coordinates


    try
    {
        int iMinX = INT_MAX;
        int iMaxX = INT_MIN;
        int iMinY = INT_MAX;
        int iMaxY = INT_MIN;

        int iX = 0;
        int iY = 0;

        int iHeight = 0;
        int iWidth = 0;

        int iCountBorderVertex = 0;

        int iUpperBorderLayerId = 0;
        int iLowerBorderLayerId = 0;

        int iBorderLayerId = 0;



        //    //Array to store four coordinates, first subscript denotes - the point And
        //    //second subscript denotes XCoordinate at index '0' and YCoordinate at index '1'
        //    int iBorderVertexCoordinate[4][2];

        NestingTreeSubgraphNode* currentNestingTreeNode = NULL;

        //Traverse ALL nesting tree subgraph nodes
        QueueNestingTreeSubgraphNodesRef qNestingTreeNode;

        //Add root nesting tree node into queue
        qNestingTreeNode.enqueue(&m_rootNestingTreeSubgraphNode);

        LayoutEnum::NodeType enVertexType;
        VertexDescriptor vVertex = 0;

        QQueue<int> qBorderLayerIds;

        while(qNestingTreeNode.isEmpty() == false)
        {
            currentNestingTreeNode = qNestingTreeNode.dequeue();
            SubGraph& gSubgraph = currentNestingTreeNode->getGraph();


            //Add child nesting tree nodes to queue
            NestingTreeSubgraphNode::IteratorVectorNestingTreeSubgraphNodesRef iterChildNestingTreeNode
                    = currentNestingTreeNode->getIteratorChildNestingTreeSubgraphNodes() ;

            while(iterChildNestingTreeNode.hasNext())
            {
                qNestingTreeNode.enqueue(iterChildNestingTreeNode.next());
            }

            NestingTreeSubgraphNode::MultiMapLayerIdToLayerNodeRef& mapOwnLayerNodes
                    = currentNestingTreeNode->getMapLayerIdToLayerNodeRef();


            //Get upper border layer id
            iUpperBorderLayerId = currentNestingTreeNode->getMinRank();
            iLowerBorderLayerId = currentNestingTreeNode->getMaxRank();

            //Enque
            qBorderLayerIds.enqueue(iUpperBorderLayerId);
            qBorderLayerIds.enqueue(iLowerBorderLayerId);

            //Reset variables
            iCountBorderVertex = 0;
            iMinX = INT_MAX;
            iMaxX = INT_MIN;
            iMinY = INT_MAX;
            iMaxY = INT_MIN;

            while(qBorderLayerIds.isEmpty() == false)
            {
                iBorderLayerId = qBorderLayerIds.dequeue();

                //Get LayerNodes residing at Border Layer Id
                QList<LayerNode*> listLayerNodes = mapOwnLayerNodes.values(iBorderLayerId);
                QListIterator<LayerNode*> iterListLayerNode(listLayerNodes);

                while(iterListLayerNode.hasNext())
                {
                    //Get vertex from LayerNode
                    vVertex = (iterListLayerNode.next())->getVertex();

                    enVertexType = m_BoostGraphWrapper.getVertexType(vVertex , *m_gMainGraph);

                    //Check for Vertical Border Vertex
                    if(enVertexType == LayoutEnum::VerticalBorderNode)
                    {
                        iX = m_BoostGraphWrapper.getVertexCenterCoordX(vVertex , *m_gMainGraph);
                        iY = m_BoostGraphWrapper.getVertexCenterCoordY(vVertex , *m_gMainGraph);

                        //                    //Add coordinates to array
                        //                    iBorderVertexCoordinate[iCountBorderVertex][0] = iX;
                        //                    iBorderVertexCoordinate[iCountBorderVertex][1] = iY;

                        //Update min and max X, Y
                        iMinX = (iMinX > iX) ? iX : iMinX;
                        iMaxX = (iMaxX < iX) ? iX : iMaxX;

                        iMinY = (iMinY > iY) ? iY : iMinY;
                        iMaxY = (iMaxY < iY) ? iY : iMaxY;
                    }

                }//Iterating border layerNodes end

                //Calculate Height and Width of compartment-subgraph
                iWidth = std::abs(iMinX - iMaxX);
                iHeight= std::abs(iMinY - iMaxY);

                //The iMinX and iMinY are the left top coordinate of compartment

                //set Lefttop coordinate to the Graph
                m_BoostGraphWrapper.setGraphLeftTopCoordX(iMinX , gSubgraph);
                m_BoostGraphWrapper.setGraphLeftTopCoordY(iMinY , gSubgraph);

                //set height - width
                m_BoostGraphWrapper.setGraphHeight(iHeight , gSubgraph);
                m_BoostGraphWrapper.setGraphWidth(iWidth , gSubgraph);

            }

        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::applyXCoordinatesFromHorizontalPosition(int iHorizontalStep)
{
    int iPos = 0;
    BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
    {
        iPos = m_BoostGraphWrapper.getVertexHorizontalPosition(vVertex , *m_gMainGraph);
        m_BoostGraphWrapper.setVertexCenterCoordX(vVertex , *m_gMainGraph , iHorizontalStep * iPos);
    }
}


void HierarchicalLayouter::setHorizontalPosition2()
{
    try
    {
        int iTotalVertices = 0;
        iTotalVertices = num_vertices(*m_gMainGraph);

        int iTotalEdges = 0;
        iTotalEdges = num_edges(*m_gMainGraph);

        //Set m_iReductionParameterHorizontal value as smallest width vertex
        PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexWidth , int , iWidth , *m_gMainGraph);

        int iMinVertexWidth = INT_MIN;

        BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
        {
            if(m_BoostGraphWrapper.getVertexType(vVertex , *m_gMainGraph)== LayoutEnum::GraphNode)
            {
                if(iMinVertexWidth > mapVertexWidth[vVertex])
                {
                    iMinVertexWidth = mapVertexWidth[vVertex];
                }
            }
        }

        if(iMinVertexWidth < 1)
        {
            //TODO::set to some default value later
            iMinVertexWidth = 30;
        }
        else
        {
            iMinVertexWidth /= 2;
        }

        m_iReductionParameterHorizontal = iMinVertexWidth;

        bool bLayeredGraphCorrect = testLayeredGraph();

        LAYOUT_ASSERT(bLayeredGraphCorrect == true , LayoutException(__FUNCTION__
                                                                     , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                                     , "Layered Graph"
                                                                     , "incorrect layered graph"));




        /*Create position vector:
         *This vector contains information about "Is Left Of" relationship
         *It tells which vertex is positioned immediete left to the current vertex on same layer
         */
        QVectorInt vecLeftNeighborVertex(iTotalVertices);
        QVectorInt vecRightNeighborVertex(iTotalVertices);


        //Mark conflicted edges
        markConflictedEdges();

        //Vertical alignments

        //1. Upward Left Alignment

        //Initialise Left Neighbor Vertex vector, to be used for Horizontal Compaction
        initLeftNeighborVertexVector(vecLeftNeighborVertex);

        QVectorInt vecLeftAlignRoot(iTotalVertices);
        QVectorInt vecLeftAlign(iTotalVertices);

        //Create upward left alignment
        createUpwardLeftAlignment(vecLeftAlignRoot , vecLeftAlign);

        QVectorDouble vecUpwardLeftPosition(iTotalVertices);
        ////qDebug() <<"Upward Left Alignment Horizontal Compaction";
        horizontalCompaction2(vecUpwardLeftPosition , vecLeftAlign , vecLeftAlignRoot , vecLeftNeighborVertex );


        //2.Upward Right alignment

        //Reverse layered graph horizontally
        reverseLayeredGraphHorizontaly();

        /*Initialise Right Neighbor Vertex vector, here we get
         *right neighbors because we have flipped the layered
         *graph horizontally
         */
        initLeftNeighborVertexVector(vecRightNeighborVertex);

        QVectorInt vecRightAlignRoot(iTotalVertices);
        QVectorInt vecRightAlign(iTotalVertices);

        createUpwardLeftAlignment(vecRightAlignRoot , vecRightAlign);
        //createUpwardRightAlignment(vecRightAlignRoot , vecRightAlign);

        QVectorDouble vecUpwardRightPosition(iTotalVertices);

        ////qDebug() <<"Upward Right Alignment Horizontal Compaction";
        horizontalCompaction2(vecUpwardRightPosition , vecRightAlign ,vecRightAlignRoot , vecRightNeighborVertex );

        //Flip vertex positions
        for(int i = 0; i < iTotalVertices ; i++)
        {
            vecUpwardRightPosition[i] *= -1;
        }



        //3. Downward Right Alignment

        //Reverse layered graph vertically
        reverseLayeredGraphVertically();

        QVectorInt vecDownRightAlignRoot(iTotalVertices);
        QVectorInt vecDownRightAlign(iTotalVertices);

        createDownwardLeftAlignment(vecDownRightAlignRoot , vecDownRightAlign);

        ////qDebug() <<"Downward Right Alignment Horizontal Compaction";
        QVectorDouble vecDownRightPositions(iTotalVertices);
        horizontalCompaction2(vecDownRightPositions , vecDownRightAlign ,
                              vecDownRightAlignRoot , vecRightNeighborVertex);

        //Flip vertex positions
        for(int i = 0; i < iTotalVertices ; i++)
        {
            vecDownRightPositions[i] *= -1;
        }


        //4. Downward Left Alignment

        //Reverse layered graph horizontally
        reverseLayeredGraphHorizontaly();

        QVectorInt vecDownLeftAlignRoot(iTotalVertices);
        QVectorInt vecDownLeftAlign(iTotalVertices);

        createDownwardLeftAlignment(vecDownLeftAlignRoot , vecDownLeftAlign);

        ////qDebug() <<"Downward Left Alignment Horizontal Compaction";
        QVectorDouble vecDownLeftPositions(iTotalVertices);
        horizontalCompaction2(vecDownLeftPositions , vecDownLeftAlign ,
                              vecDownLeftAlignRoot , vecLeftNeighborVertex);

        //Restore layered graph
        reverseLayeredGraphVertically();


        //align to assignment of smallest width
        alignToSmallestAlignment(vecUpwardLeftPosition , vecUpwardRightPosition ,
                                 vecDownLeftPositions , vecDownRightPositions);

        //set horizontal position to average median aligned coordinates.
        QVectorDouble vecMergedPosition = mergeAlignments(vecUpwardLeftPosition , vecUpwardRightPosition ,
                                                          vecDownLeftPositions , vecDownRightPositions);

        ////qDebug()<<"Setting Horizontal Coord X:";
        //Writing arrangement
        BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
        {
            int iXCoord = (vecMergedPosition[vVertex] * m_iReductionParameterHorizontal );
            m_BoostGraphWrapper.setVertexCenterCoordX(vVertex , *m_gMainGraph , iXCoord);
            ////qDebug() << "v: "<<(int)vVertex<<" x: "<<iXCoord;
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}


void HierarchicalLayouter::alignToSmallestAlignment(QVectorDouble &vecUpLeftAlign, QVectorDouble &vecUpRightAlign, QVectorDouble &vecDownLeftAlign, QVectorDouble &vecDownRightAlign)
{
    int iTotalVertices = 0;

    try
    {
        iTotalVertices = num_vertices(*m_gMainGraph);
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }

    LAYOUT_ASSERT(vecUpLeftAlign.size() == iTotalVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector UpLeftAlign must have size = total numebr of vertices"
                                   ,"Vector UpLeftAlign"));
    LAYOUT_ASSERT(vecUpRightAlign.size() == iTotalVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector UpRightAlign must have size = total numebr of vertices"
                                   ,"Vector UpRightAlign"));
    LAYOUT_ASSERT(vecDownLeftAlign.size() == iTotalVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector DownLeftAlign must have size = total numebr of vertices"
                                   ,"Vector DownLeftAlign"));
    LAYOUT_ASSERT(vecDownRightAlign.size() == iTotalVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector DownRightAlign must have size = total numebr of vertices"
                                   ,"Vector DownRightAlign"));

    try
    {

        //Pointer to smallest width alilgnment vector
        QVectorDouble *vecSmallestAlignment = NULL;

        //Store all 4 alignments in a vector
        QVector<QVectorDouble*> vecAlignments;

        //Store min max positions of all 4 alignments in a vector
        QVector<QPair<double,double> > vecMinMaxPos(4);

        //Push left alignments first
        vecAlignments.push_back(&vecUpLeftAlign);
        vecAlignments.push_back(&vecDownLeftAlign);

        //Push right alignments after left alignments
        vecAlignments.push_back(&vecUpRightAlign);
        vecAlignments.push_back(&vecDownRightAlign);

        double iMinPosition = 0;
        double iMaxPosition = 0;

        double iCurrentAlignmentWidth = INT_MAX;
        double iSmallestAlignmentWidth = INT_MAX;

        double iMinSmallestAlign = 0;
        double iMaxSmallestAlign = 0;

        //Select the smallest width alignment
        for(int iAlignment = 0; iAlignment < vecAlignments.size() ; iAlignment++)
        {
            //Get min max positions of current alignment
            getMinMaxPositions(iMinPosition , iMaxPosition , *(vecAlignments[iAlignment]));

            //store min max positions in vector
            vecMinMaxPos[iAlignment].first = iMinPosition;
            vecMinMaxPos[iAlignment].second = iMaxPosition;

            //Calculate width of current alignment
            iCurrentAlignmentWidth = iMaxPosition - iMinPosition;

            //Update smallest alignment
            if(iSmallestAlignmentWidth > iCurrentAlignmentWidth)
            {
                iMinSmallestAlign = iMinPosition;
                iMaxSmallestAlign = iMaxPosition;
                vecSmallestAlignment = vecAlignments[iAlignment];
            }
        }

        double iMinPosLeftAlignment = 0;
        double iMaxPosRightAlignment = 0;

        /*Align alignments with the smallest width alignment
         *Align the left alignment min position to match min position with
         *min position of smallest alignment and
         *Align right alignment to match max position with max position of
         *smallest alignment
         */

        double iShift = 0;
        for(int iAlignment = 0; iAlignment < vecAlignments.size() ; iAlignment++)
        {
            //Skip smallest alignment as it is aligned to itself!
            if(vecAlignments[iAlignment] == vecSmallestAlignment)
            {
                continue;
            }

            //First two alignments are left and third - fourth are right as per the sequence
            if(iAlignment < 2)
            {
                //Left Alignment

                //Align left alignement min
                iMinPosLeftAlignment = vecMinMaxPos[iAlignment].first;

                //Shift current left alignment to match min position with min position of smallest alignment
                iShift = iMinSmallestAlign - iMinPosLeftAlignment;

            }
            else
            {
                //Right Alignment

                //Align right alignment max
                iMaxPosRightAlignment = vecMinMaxPos[iAlignment].second;

                //Shift current right alignment to match max position with max position of smallest alignment
                iShift = iMaxSmallestAlign - iMaxPosRightAlignment;
            }


            for(int iVertex = 0; iVertex < iTotalVertices ; iVertex++ )
            {
                //Shift each vertex of the current alignment according to iShift
                (*(vecAlignments[iAlignment]))[iVertex] += iShift;
            }

        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

HierarchicalLayouter::QVectorDouble HierarchicalLayouter::mergeAlignments(HierarchicalLayouter::QVectorDouble &vecUpLeftAlign, HierarchicalLayouter::QVectorDouble &vecUpRightAlign, HierarchicalLayouter::QVectorDouble &vecDownLeftAlign, HierarchicalLayouter::QVectorDouble &vecDownRightAlign)
{


    int iNumVertices = 0;
    try
    {
        iNumVertices = num_vertices(*m_gMainGraph);
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }

    LAYOUT_ASSERT(vecUpLeftAlign.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector UpLeftAlign must have size = total numebr of vertices"
                                   ,"Vector UpLeftAlign"));
    LAYOUT_ASSERT(vecUpRightAlign.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector UpRightAlign must have size = total numebr of vertices"
                                   ,"Vector UpRightAlign"));
    LAYOUT_ASSERT(vecDownLeftAlign.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector DownLeftAlign must have size = total numebr of vertices"
                                   ,"Vector DownLeftAlign"));
    LAYOUT_ASSERT(vecDownRightAlign.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector DownRightAlign must have size = total numebr of vertices"
                                   ,"Vector DownRightAlign"));





    //Pointer to smallest width alilgnment vector
    QVectorDouble vecMergedAlignment(iNumVertices);

    try
    {

        //Size = 4 for 4 alignments
        QVectorDouble vecPositions(4);

        BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
        {
            //get all four positions
            vecPositions[0] = vecUpLeftAlign[vVertex];
            vecPositions[1] = vecUpRightAlign[vVertex];
            vecPositions[2] = vecDownLeftAlign[vVertex];
            vecPositions[3] = vecDownRightAlign[vVertex];

            qSort(vecPositions.begin() , vecPositions.end() , qLess<double>());

            //Mean of median i.e. 1 and 2
            vecMergedAlignment[vVertex] = (double)(vecPositions[1] + vecPositions[2])/2.0;
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }


    return vecMergedAlignment;
}

void HierarchicalLayouter::getMinMaxPositions(double &iMinPosition, double &iMaxPosition, QVectorDouble &vecPositions)
{
    int iNumVertices = 0;
    iNumVertices = num_vertices(*m_gMainGraph);

    Q_ASSERT_X(vecPositions.size() == iNumVertices , "GetMinMaxPositions" , "Invalid length of vecPositions");

    iMinPosition = INT_MAX;
    iMaxPosition = INT_MIN;
    for(int iVertex = 0 ; iVertex < iNumVertices ; iVertex++)
    {
        if(iMinPosition > vecPositions[iVertex])
        {
            iMinPosition = vecPositions[iVertex];
        }

        if(iMaxPosition < vecPositions[iVertex])
        {
            iMaxPosition = vecPositions[iVertex];
        }
    }

}

void HierarchicalLayouter::initLeftNeighborVertexVector(QVectorInt &vecLeftNeighborVertex)
{
    try
    {
        int iNumVertices = 0;
        iNumVertices = num_vertices(*m_gMainGraph);

        LAYOUT_ASSERT(vecLeftNeighborVertex.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector vecLeftNeighborVertex must have size = total number of vertices"
                                       ,"LeftNeighborVertexVector"));

        ////qDebug() << "Initialising Left Neighbors Vector";
        VertexDescriptor vCurrentVertex = 0;
        VertexDescriptor vLeftNeighbor = 0;

        bool bFirstNodeOfLayer = false;

        //Iterate layered graph
        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);
        while(iterLayer.hasNext())
        {
            iterLayer.next();

            bFirstNodeOfLayer = true;

            //Iterate layerNodes
            IteratorMapPositionToLayerNode iterNode(*(iterLayer.value()));

            while(iterNode.hasNext())
            {
                iterNode.next();

                vCurrentVertex = iterNode.value()->getVertex();

                if(bFirstNodeOfLayer == true)
                {
                    vLeftNeighbor = vCurrentVertex;

                    bFirstNodeOfLayer = false;
                }

                //Add left neighbor
                vecLeftNeighborVertex[vCurrentVertex] = vLeftNeighbor;

                vLeftNeighbor = vCurrentVertex;
            }

        }

        //For testing
        //    for(int iter = 0;iter < vecLeftNeighborVertex.size() ; iter++)
        //    {
        //        ////qDebug() << "V: " <<iter << " <- " << vecLeftNeighborVertex[iter];
        //    }


    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

void HierarchicalLayouter::markConflictedEdges()
{
    try
    {
        //reset bIsConflicted value for all edges
        BGL_FORALL_EDGES(eEdge , *m_gMainGraph , SubGraph)
        {
            m_BoostGraphWrapper.setEdgeIsConflicted(eEdge ,*m_gMainGraph , false);
        }


        //Iterate layer from 1 to second last
        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);

        double dUpperPosLongEdge = 0;
        double dUpperPosGraphEdge = 0;
        double dUpperPosBorderEdge = 0;
        double dLowerPosLongEdge = 0;
        double dLowerPosGraphEdge = 0;
        double dLowerPosBorderEdge = 0;

        bool bEdgeConflict = false;


        QVector<EdgeDescriptor> *vecLongEdgeOrVerticalBorderSegment = NULL;
        QVector<EdgeDescriptor> *vecGraphEdge = NULL;
        QVector<EdgeDescriptor> *vecVerticalBorderSegment = NULL;
        QVector<EdgeDescriptor>* vecLongEdgeSegment = NULL;

        VertexDescriptor vCurrentVertex;

        LayerNode* currentNode;

        int iSourceLayerId = 0;
        int iTargetLayerId = 0;
        VertexDescriptor vSource = 0;
        VertexDescriptor vTarget = 0;

        VertexDescriptor vGraphEdgeSource;
        VertexDescriptor vGraphEdgeTarget;

        VertexDescriptor vLongEdgeSource;
        VertexDescriptor vLongEdgeTarget;

        VertexDescriptor vBorderEdgeSource;
        VertexDescriptor vBorderEdgeTarget;


        EdgeDescriptor eGraphEdge;
        EdgeDescriptor eLongEdge;
        EdgeDescriptor eBorderEdge;
        EdgeDescriptor eBorderOrLongEdge;

        int iSizeLongEdgeSegments;
        int iSizeBorderOrLongEdges;
        int iSizeGraphEdges;
        int iSizeVerticalEdgeSegments;

        PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexHorizontalPos , int , iHorizontalPosition , *m_gMainGraph);

        while(iterLayer.hasNext())
        {
            iterLayer.next();
            //Check if current layer is not last
            if(iterLayer.hasNext())
            {
                //Create new vectors to hold graph edges and long edge or Vertical Border segments separately
                vecLongEdgeOrVerticalBorderSegment = new QVector<EdgeDescriptor>();
                vecGraphEdge = new QVector<EdgeDescriptor>();
                vecVerticalBorderSegment = new QVector<EdgeDescriptor>;
                vecLongEdgeSegment = new QVector<EdgeDescriptor>;

                //Iterate edges from current layer nodes and fill the vectors
                IteratorMapPositionToLayerNode iterNode(*iterLayer.value());
                while(iterNode.hasNext())
                {
                    iterNode.next();

                    currentNode = iterNode.value();
                    vCurrentVertex = currentNode->getVertex();

                    //Iterate edges of current vertex
                    OutEdgeIterator iterEdge , iterEdgeEnd;
                    for(boost::tie(iterEdge , iterEdgeEnd) = out_edges(vCurrentVertex , *m_gMainGraph); iterEdge != iterEdgeEnd; iterEdge++)
                    {
                        EdgeDescriptor eCurrentEdge = *iterEdge;

                        //Testing 42214-3 Skip edges with atleast one vertex on non GraphNodeLayerId
                        vSource = m_BoostGraphWrapper.getEdgeSource(eCurrentEdge , *m_gMainGraph);
                        vTarget = m_BoostGraphWrapper.getEdgeTarget(eCurrentEdge , *m_gMainGraph);

                        iSourceLayerId = m_BoostGraphWrapper.getVertexRank(vSource , *m_gMainGraph);

                        iTargetLayerId = m_BoostGraphWrapper.getVertexRank(vTarget , *m_gMainGraph);

                        if(iSourceLayerId % m_iRankDifferenceInLayers != 0 ||
                                iTargetLayerId % m_iRankDifferenceInLayers != 0)
                        {
                            //Either vSource or vTarget vertex is at Layer which is not a GraphNodeLayer
                            //This edge will create falls conflict - crossing, so we skip it
                            //Skipping
                            continue;
                        }
                        //end 42214-3


                        LayoutEnum::EdgeType currentEdgeType = m_BoostGraphWrapper.getEdgeType(eCurrentEdge , *m_gMainGraph);
                        if(currentEdgeType == LayoutEnum::LongEdgeSegment)
                        {
                            //Record in long edge segments
                            vecLongEdgeSegment->push_back(eCurrentEdge);

                            //Record in long edge and vertical border segments
                            vecLongEdgeOrVerticalBorderSegment->push_back(eCurrentEdge);
                        }

                        if(currentEdgeType == LayoutEnum::VerticalBorderEdgeSegment)
                        {
                            //Record in vertical border edge
                            vecVerticalBorderSegment->push_back(eCurrentEdge);

                            //Record in long edge and vertical border segments
                            vecLongEdgeOrVerticalBorderSegment->push_back(eCurrentEdge);
                        }

                        if(currentEdgeType == LayoutEnum::GraphEdge)
                        {
                            //Record in graph edges vector
                            vecGraphEdge->push_back(eCurrentEdge);
                        }
                    }
                }

                //Find crossings between LongEdgeSegments + VerticalBorderSegments and GraphEdges
                iSizeBorderOrLongEdges = vecLongEdgeOrVerticalBorderSegment->size();
                iSizeGraphEdges = vecGraphEdge->size();

                for(int iBorderOrLongEdgeIndex = 0; iBorderOrLongEdgeIndex < iSizeBorderOrLongEdges; iBorderOrLongEdgeIndex++)
                {
                    eBorderOrLongEdge = (*vecLongEdgeOrVerticalBorderSegment)[iBorderOrLongEdgeIndex];

                    vLongEdgeSource = m_BoostGraphWrapper.getEdgeSource(eBorderOrLongEdge , *m_gMainGraph);
                    vLongEdgeTarget = m_BoostGraphWrapper.getEdgeTarget(eBorderOrLongEdge , *m_gMainGraph);

                    dUpperPosLongEdge = mapVertexHorizontalPos[vLongEdgeSource];
                    dLowerPosLongEdge = mapVertexHorizontalPos[vLongEdgeTarget];

                    for(int iGraphEdgeIndex = 0 ; iGraphEdgeIndex < iSizeGraphEdges ; iGraphEdgeIndex++)
                    {
                        eGraphEdge = (*vecGraphEdge)[iGraphEdgeIndex];

                        vGraphEdgeSource = m_BoostGraphWrapper.getEdgeSource(eGraphEdge , *m_gMainGraph);
                        vGraphEdgeTarget = m_BoostGraphWrapper.getEdgeTarget(eGraphEdge , *m_gMainGraph);

                        dUpperPosGraphEdge = mapVertexHorizontalPos[vGraphEdgeSource];
                        dLowerPosGraphEdge = mapVertexHorizontalPos[vGraphEdgeTarget];

                        //Reset for new edges
                        bEdgeConflict = false;

                        //Check crossing
                        if(dUpperPosLongEdge < dUpperPosGraphEdge)
                        {
                            if(dLowerPosLongEdge > dLowerPosGraphEdge)
                            {
                                bEdgeConflict = true;
                            }
                        }
                        else if(dUpperPosLongEdge > dUpperPosGraphEdge)
                        {
                            if(dLowerPosLongEdge < dLowerPosGraphEdge)
                            {
                                bEdgeConflict = true;
                            }
                        }

                        if(bEdgeConflict == true)
                        {
                            //Mark GraphEdge as conflicted
                            m_BoostGraphWrapper.setEdgeIsConflicted(eGraphEdge , *m_gMainGraph , bEdgeConflict);

                        }

                    }

                }



                //Find crossings between LongEdgeSegments and VerticalBorderSegments
                iSizeLongEdgeSegments = vecLongEdgeSegment->size();
                iSizeVerticalEdgeSegments = vecVerticalBorderSegment->size();

                for(int iLongEdgeIndex = 0; iLongEdgeIndex < iSizeLongEdgeSegments; iLongEdgeIndex++)
                {
                    eLongEdge = (*vecLongEdgeSegment)[iLongEdgeIndex];

                    vLongEdgeSource = m_BoostGraphWrapper.getEdgeSource(eLongEdge , *m_gMainGraph);
                    vLongEdgeTarget = m_BoostGraphWrapper.getEdgeTarget(eLongEdge , *m_gMainGraph);

                    dUpperPosLongEdge = mapVertexHorizontalPos[vLongEdgeSource];
                    dLowerPosLongEdge = mapVertexHorizontalPos[vLongEdgeTarget];

                    for(int iBorderEdgeIndex = 0 ; iBorderEdgeIndex < iSizeVerticalEdgeSegments ; iBorderEdgeIndex++)
                    {
                        eBorderEdge = (*vecVerticalBorderSegment)[iBorderEdgeIndex];

                        vBorderEdgeSource = m_BoostGraphWrapper.getEdgeSource(eBorderEdge , *m_gMainGraph);
                        vBorderEdgeTarget = m_BoostGraphWrapper.getEdgeTarget(eBorderEdge , *m_gMainGraph);

                        dUpperPosBorderEdge = mapVertexHorizontalPos[vBorderEdgeSource];
                        dLowerPosBorderEdge = mapVertexHorizontalPos[vBorderEdgeTarget];

                        //Reset for new edges
                        bEdgeConflict = false;

                        //Check crossing
                        if(dUpperPosLongEdge < dUpperPosBorderEdge)
                        {
                            if(dLowerPosLongEdge > dLowerPosBorderEdge)
                            {
                                bEdgeConflict = true;
                            }
                        }
                        else if(dUpperPosLongEdge > dUpperPosBorderEdge)
                        {
                            if(dLowerPosLongEdge < dLowerPosBorderEdge)
                            {
                                bEdgeConflict = true;
                            }
                        }

                        if(bEdgeConflict == true)
                        {
                            //Mark LongEdge as conflicted
                            m_BoostGraphWrapper.setEdgeIsConflicted(eLongEdge , *m_gMainGraph , bEdgeConflict);

                        }

                    }

                }

                //Clean up vectors holding graph edges, long edge and vertical border segments
                vecLongEdgeOrVerticalBorderSegment->clear();
                DELETE_AND_SET_NULL(vecLongEdgeOrVerticalBorderSegment);

                vecGraphEdge->clear();
                DELETE_AND_SET_NULL(vecGraphEdge);

                vecLongEdgeSegment->clear();
                DELETE_AND_SET_NULL(vecLongEdgeSegment);

                vecVerticalBorderSegment->clear();
                DELETE_AND_SET_NULL(vecVerticalBorderSegment);

            }
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

QVectorInt HierarchicalLayouter::createUpwardLeftAlignment(QVectorInt &vecUpLeftAlignRoot , QVectorInt &vecUpLeftAlign)
{
    try
    {
        int iNumVertices = 0;

        iNumVertices = num_vertices(*m_gMainGraph);

        LAYOUT_ASSERT(vecUpLeftAlignRoot.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector LeftAlignRoot must have size = total numebr of vertices"
                                       ,"Vector LeftAlignRoot"));

        LAYOUT_ASSERT(vecUpLeftAlign.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector LeftAlign must have size = total numebr of vertices"
                                       ,"Vector LeftAlign"));

        for(int iVertexId = 0; iVertexId < iNumVertices ; iVertexId++)
        {
            //Initialise root[v] to v
            vecUpLeftAlignRoot[iVertexId] = iVertexId;

            //Initialise align[v] to v
            vecUpLeftAlign[iVertexId] = iVertexId;
        }


        int iHorizontalPos = 0;
        VertexDescriptor vUpperMedianVertex = 0;
        //Iterate all layers
        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);

        while(iterLayer.hasNext())
        {
            iterLayer.next();
            MapPositionToLayerNode *currentLayer = iterLayer.value();

            //Reset iAlignedPosition
            int iAlignedPosition = INT_MIN;

            //Iterate  LayerNodes
            IteratorMapPositionToLayerNode iterNode(*currentLayer);
            while(iterNode.hasNext())
            {
                iterNode.next();

                LayerNode *currentLayerNode = iterNode.value();

                VertexDescriptor vCurrentVertex = currentLayerNode->getVertex();

                //if current node has upper neighbors : d and d > 0
                int iTotalUpperNeighbors = 0;
                iTotalUpperNeighbors = in_degree(vCurrentVertex , *m_gMainGraph);

                if(iTotalUpperNeighbors > 0)
                {
                    //Iterate Median Upper Vertices
                    MapPositionToVertexDescriptor mapPosToUpperMedianVertex = getMedianUpperNeighborsExcludeConflictedEdges(vCurrentVertex);
                    IteratorMapPositionToVertexDescriptor iterUpperNeighbor(mapPosToUpperMedianVertex);

                    //Iterating left to right sorted on (horizontal) position
                    while(iterUpperNeighbor.hasNext())
                    {
                        iterUpperNeighbor.next();
                        iHorizontalPos = iterUpperNeighbor.key();
                        vUpperMedianVertex = iterUpperNeighbor.value();

                        //if align[v] = v
                        if(vecUpLeftAlign[vCurrentVertex] == vCurrentVertex)
                        {
                            //r < position[upperNeighbor]
                            if(iAlignedPosition < iHorizontalPos)
                            {
                                vecUpLeftAlign[vUpperMedianVertex] = vCurrentVertex;
                                vecUpLeftAlignRoot[vCurrentVertex] = vecUpLeftAlignRoot[vUpperMedianVertex];
                                vecUpLeftAlign[vCurrentVertex] = vecUpLeftAlignRoot[vCurrentVertex];

                                //Update iAlignedPosition
                                iAlignedPosition = iHorizontalPos;
                            }
                        }

                    }


                }

            }



        }



        int iTotalVertices = 0;
        iTotalVertices = num_vertices(*m_gMainGraph);

        //Align Vertical Border nodes strictly
        QVector<bool> vecVisitedVerticalBorderVertex(iTotalVertices);
        std::fill(vecVisitedVerticalBorderVertex.begin() , vecVisitedVerticalBorderVertex.end() , false);

        LayoutEnum::NodeType enVertexType;

        VertexDescriptor vRootVerticalBorderVertex = 0;
        VertexDescriptor vCurrentVerticalBorderVertex = 0;

        BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
        {
            enVertexType = m_BoostGraphWrapper.getVertexType(vVertex , *m_gMainGraph);

            if(enVertexType == LayoutEnum::VerticalBorderNode)
            {
                //check if VerticalBorderVertex is visited / aligned already
                if(vecVisitedVerticalBorderVertex[vVertex] == true)
                {
                    //skipping visited-aligned vertex
                    continue;
                }

                vCurrentVerticalBorderVertex = vVertex;

                //Find the root VerticalBorderVertex - it is the reachable vertex which has in_degree = 0
                while(in_degree(vCurrentVerticalBorderVertex , *m_gMainGraph) != 0)
                {
                    //Mark Vertical border vertex visited
                    vecVisitedVerticalBorderVertex[vCurrentVerticalBorderVertex] = true;

                    //Every VerticalBorderVertex must have at max 1 in edge
                    LAYOUT_ASSERT(in_degree(vCurrentVerticalBorderVertex , *m_gMainGraph) == 1
                                  ,LayoutException(__FUNCTION__
                                                   , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                   , "every VerticalBorderVertex must have at max 1 in edge"
                                                   , "Vertical Border Vertex Trail"));



                    BGL_FORALL_INEDGES(vCurrentVerticalBorderVertex , eInEdge , *m_gMainGraph , SubGraph)
                    {
                        VertexDescriptor vInVertex = m_BoostGraphWrapper.getEdgeSource(eInEdge , *m_gMainGraph);

                        vCurrentVerticalBorderVertex = vInVertex;
                    }
                }
                //vCurrentVerticalBorderVertex contains root vertex
                vRootVerticalBorderVertex = vCurrentVerticalBorderVertex;


                //Traverse from root vertex till bottom vertex and align every vertical border vertex
                //with its predecessor and last vertex with root
                while(out_degree(vCurrentVerticalBorderVertex , *m_gMainGraph) != 0)
                {
                    //Every VerticalBorderVertex must have at max 1 out edge
                    LAYOUT_ASSERT(out_degree(vCurrentVerticalBorderVertex , *m_gMainGraph) == 1
                                  ,LayoutException(__FUNCTION__
                                                   , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                   , "every VerticalBorderVertex must have at max 1 out edge"
                                                   , "Vertical Border Vertex Trail"));

                    BGL_FORALL_ADJ(vCurrentVerticalBorderVertex , vNextVertex , *m_gMainGraph , SubGraph)
                    {
                        //Align current vertex with next vertex
                        vecUpLeftAlign[vCurrentVerticalBorderVertex] = vNextVertex;
                        //Set root
                        vecUpLeftAlignRoot[vCurrentVerticalBorderVertex] = vRootVerticalBorderVertex;

                        //update current vertex
                        vCurrentVerticalBorderVertex = vNextVertex;
                    }

                }
                //vCurrentVerticalBorderVertex contains the last/bottom vertex so align it with the root vertex
                vecUpLeftAlign[vCurrentVerticalBorderVertex] = vRootVerticalBorderVertex;
                //Set root
                vecUpLeftAlignRoot[vCurrentVerticalBorderVertex] = vRootVerticalBorderVertex;
            }
        }


        ////qDebug() << "UpwardLeft Alignment";
        QVectorInt::iterator it = vecUpLeftAlign.begin();
        for(int i = 0;it != vecUpLeftAlign.end() ; it++ , i++)
        {
            ////qDebug() << i <<" " <<*it;
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

    return vecUpLeftAlign;
}

QVectorInt HierarchicalLayouter::createDownwardLeftAlignment(QVectorInt &vecDownLeftAlignRoot, QVectorInt &vecDownLeftAlign)
{
    try
    {
        int iNumVertices = 0;

        iNumVertices = num_vertices(*m_gMainGraph);

        //Assert layered graph is reversed
        LAYOUT_ASSERT((m_mapLayeredGraph.begin().key() <= 0)
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_OPERATION
                                       , "Layered graph is not reversed"
                                       , "Create Downward Left Vertical Alignment"));

        LAYOUT_ASSERT(vecDownLeftAlign.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector DownLeftAlign must have size = total numebr of vertices"
                                       ,"Vector DownLeftAlign"));
        LAYOUT_ASSERT(vecDownLeftAlignRoot.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector DownLeftAlignRoot must have size = total numebr of vertices"
                                       ,"Vector DownLeftAlignRoot"));

        for(int iVertexId = 0; iVertexId < iNumVertices ; iVertexId++)
        {
            //Initialise root[v] to v
            vecDownLeftAlignRoot[iVertexId] = iVertexId;

            //Initialise align[v] to v
            vecDownLeftAlign[iVertexId] = iVertexId;
        }


        int iHorizontalPos = 0;
        VertexDescriptor vLowerMedianVertex = 0;
        //Iterate all layers
        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);

        //Reverse iter
        //iterLayer.toBack(); // testing
        while(iterLayer.hasNext())
        {
            iterLayer.next();
            MapPositionToLayerNode *currentLayer = iterLayer.value();

            //Reset iAlignedPosition
            int iAlignedPosition = INT_MIN;

            //Iterate  LayerNodes
            IteratorMapPositionToLayerNode iterNode(*currentLayer);
            while(iterNode.hasNext())
            {
                iterNode.next();

                LayerNode *currentLayerNode = iterNode.value();

                VertexDescriptor vCurrentVertex = currentLayerNode->getVertex();

                //if current node has upper neighbors : d and d > 0
                int iTotalDownNeighbors = 0;
                iTotalDownNeighbors = out_degree(vCurrentVertex , *m_gMainGraph);

                if(iTotalDownNeighbors > 0)
                {
                    //Iterate Median Downper Vertices
                    MapPositionToVertexDescriptor mapPosToLowerMedianVertex = getMedianLowerNeighborsExcludeConflictedEdges(vCurrentVertex);
                    IteratorMapPositionToVertexDescriptor iterLowerNeighbor(mapPosToLowerMedianVertex);

                    //Iterating left to right sorted on (horizontal) position
                    while(iterLowerNeighbor.hasNext())
                    {
                        iterLowerNeighbor.next();
                        iHorizontalPos = iterLowerNeighbor.key();
                        vLowerMedianVertex = iterLowerNeighbor.value();

                        //if align[v] = v
                        if(vecDownLeftAlign[vCurrentVertex] == vCurrentVertex)
                        {
                            //r < position[DownperNeighbor]
                            if(iAlignedPosition < iHorizontalPos)
                            {
                                vecDownLeftAlign[vLowerMedianVertex] = vCurrentVertex;
                                vecDownLeftAlignRoot[vCurrentVertex] = vecDownLeftAlignRoot[vLowerMedianVertex];
                                vecDownLeftAlign[vCurrentVertex] = vecDownLeftAlignRoot[vCurrentVertex];

                                //Update iAlignedPosition
                                iAlignedPosition = iHorizontalPos;
                            }
                        }

                    }


                }

            }



        }

        ////qDebug() << "DownwardLeft Alignment";
        QVectorInt::iterator it = vecDownLeftAlign.begin();
        for(int i = 0;it != vecDownLeftAlign.end() ; it++ , i++)
        {
            ////qDebug() << i <<" " <<*it;
        }

        int iTotalVertices = 0;
        iTotalVertices = num_vertices(*m_gMainGraph);

        //Align Vertical Border nodes strictly
        QVector<bool> vecVisitedVerticalBorderVertex(iTotalVertices);
        std::fill(vecVisitedVerticalBorderVertex.begin() , vecVisitedVerticalBorderVertex.end() , false);

        LayoutEnum::NodeType enVertexType;

        VertexDescriptor vRootVerticalBorderVertex = 0;
        VertexDescriptor vCurrentVerticalBorderVertex = 0;


        BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
        {
            enVertexType = m_BoostGraphWrapper.getVertexType(vVertex , *m_gMainGraph);

            if(enVertexType == LayoutEnum::VerticalBorderNode)
            {
                //check if VerticalBorderVertex is visited / aligned already
                if(vecVisitedVerticalBorderVertex[vVertex] == true)
                {
                    //skipping visited-aligned vertex
                    continue;
                }

                vCurrentVerticalBorderVertex = vVertex;

                //Find the root VerticalBorderVertex - it is the reachable vertex which has in_degree = 0
                while(in_degree(vCurrentVerticalBorderVertex , *m_gMainGraph) != 0)
                {
                    //Mark Vertical border vertex visited
                    vecVisitedVerticalBorderVertex[vCurrentVerticalBorderVertex] = true;

                    //Every VerticalBorderVertex must have at max 1 in edge
                    LAYOUT_ASSERT(in_degree(vCurrentVerticalBorderVertex , *m_gMainGraph) == 1
                                  ,LayoutException(__FUNCTION__
                                                   , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                   , "every VerticalBorderVertex must have at max 1 in edge"
                                                   , "Vertical Border Vertex Trail"));

                    BGL_FORALL_INEDGES(vCurrentVerticalBorderVertex , eInEdge , *m_gMainGraph , SubGraph)
                    {
                        VertexDescriptor vInVertex = m_BoostGraphWrapper.getEdgeSource(eInEdge , *m_gMainGraph);

                        vCurrentVerticalBorderVertex = vInVertex;
                    }
                }
                //vCurrentVerticalBorderVertex contains root vertex
                vRootVerticalBorderVertex = vCurrentVerticalBorderVertex;


                //Traverse from root vertex till bottom vertex and align every vertical border vertex
                //with its predecessor and last vertex with root
                while(out_degree(vCurrentVerticalBorderVertex , *m_gMainGraph) != 0)
                {
                    //Every VerticalBorderVertex must have at max 1 out edge
                    LAYOUT_ASSERT(out_degree(vCurrentVerticalBorderVertex , *m_gMainGraph) == 1
                                  ,LayoutException(__FUNCTION__
                                                   , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                                   , "every VerticalBorderVertex must have at max 1 out edge"
                                                   , "Vertical Border Vertex Trail"));

                    BGL_FORALL_ADJ(vCurrentVerticalBorderVertex , vNextVertex , *m_gMainGraph , SubGraph)
                    {
                        //Align current vertex with next vertex
                        vecDownLeftAlign[vCurrentVerticalBorderVertex] = vNextVertex;
                        //Set Root
                        vecDownLeftAlignRoot[vCurrentVerticalBorderVertex] = vRootVerticalBorderVertex;

                        //update current vertex
                        vCurrentVerticalBorderVertex = vNextVertex;
                    }

                }
                //vCurrentVerticalBorderVertex contains the last/bottom vertex so align it with the root vertex
                vecDownLeftAlign[vCurrentVerticalBorderVertex] = vRootVerticalBorderVertex;
                //Set Root
                vecDownLeftAlignRoot[vCurrentVerticalBorderVertex] = vRootVerticalBorderVertex;
            }
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

    return vecDownLeftAlign;
}

QVectorInt HierarchicalLayouter::createUpwardRightAlignment(QVectorInt &vecUpRightAlignRoot, QVectorInt &vecUpRightAlign)
{
    int iNumVertices = 0;

    iNumVertices = num_vertices(*m_gMainGraph);


    Q_ASSERT_X(vecUpRightAlignRoot.size() == iNumVertices , "Create Upward Right Vertical Alignment" , "Vector LeftAlignRoot must have size = total numebr of vertices");
    Q_ASSERT_X(vecUpRightAlign.size() == iNumVertices , "Create Upward Right Vertical Alignment" , "Vector LeftAlign must have size = total numebr of vertices");


    for(int iVertexId = 0; iVertexId < iNumVertices ; iVertexId++)
    {
        //Initialise root[v] to v
        vecUpRightAlignRoot[iVertexId] = iVertexId;

        //Initialise align[v] to v
        vecUpRightAlign[iVertexId] = iVertexId;
    }


    int iHorizontalPos = 0;
    VertexDescriptor vUpperMedianVertex = 0;
    //Iterate all layers
    IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);

    while(iterLayer.hasNext())
    {
        iterLayer.next();
        MapPositionToLayerNode *currentLayer = iterLayer.value();


        //Iterate  LayerNodes
        IteratorMapPositionToLayerNode iterNode(*currentLayer);
        iterNode.toBack();

        //Reset iAlignedPosition
        int iAlignedPosition = INT_MAX;


        while(iterNode.hasPrevious())
        {
            iterNode.previous();

            LayerNode *currentLayerNode = iterNode.value();

            VertexDescriptor vCurrentVertex = currentLayerNode->getVertex();

            //if current node has upper neighbors : d and d > 0
            int iTotalUpperNeighbors = 0;
            iTotalUpperNeighbors = in_degree(vCurrentVertex , *m_gMainGraph);

            if(iTotalUpperNeighbors > 0)
            {
                //Iterate Median Upper Vertices
                MapPositionToVertexDescriptor mapPosToUpperMedianVertex = getMedianUpperNeighborsExcludeConflictedEdges(vCurrentVertex);
                IteratorMapPositionToVertexDescriptor iterUpperNeighbor(mapPosToUpperMedianVertex);

                iterUpperNeighbor.toBack();

                //Iterating left to right sorted on (horizontal) position
                while(iterUpperNeighbor.hasPrevious())
                {
                    iterUpperNeighbor.previous();

                    iHorizontalPos = iterUpperNeighbor.key();
                    vUpperMedianVertex = iterUpperNeighbor.value();

                    //if align[v] = v
                    if(vecUpRightAlign[vCurrentVertex] == vCurrentVertex)
                    {
                        //r < position[upperNeighbor]
                        if(iAlignedPosition > iHorizontalPos)
                        {
                            vecUpRightAlign[vUpperMedianVertex] = vCurrentVertex;
                            vecUpRightAlignRoot[vCurrentVertex] = vecUpRightAlignRoot[vUpperMedianVertex];
                            vecUpRightAlign[vCurrentVertex] = vecUpRightAlignRoot[vCurrentVertex];

                            //Update iAlignedPosition
                            iAlignedPosition = iHorizontalPos;
                        }
                    }

                }


            }

        }



    }

    //Print for testig
//    ////qDebug() << "Up Right Alignement:";
//    int iSize = vecUpRightAlign.size();
//    while(iSize--)
//    {
//        ////qDebug() <<iSize <<" -- "<<vecUpRightAlign[iSize];
//    }

    return vecUpRightAlign;

}

QVectorInt HierarchicalLayouter::createDownwardRightAlignment(QVectorInt &vecDownRightAlignRoot, QVectorInt &vecDownRightAlign)
{
    int iNumVertices = 0;

    iNumVertices = num_vertices(*m_gMainGraph);


    Q_ASSERT_X(vecDownRightAlignRoot.size() == iNumVertices , "Create Downward Left Vertical Alignment" , "Vector DownRightAlignRoot must have size = total numebr of vertices");
    Q_ASSERT_X(vecDownRightAlign.size() == iNumVertices , "Create Downward Left Vertical Alignment" , "Vector DownRightAlign must have size = total numebr of vertices");


    for(int iVertexId = 0; iVertexId < iNumVertices ; iVertexId++)
    {
        //Initialise root[v] to v
        vecDownRightAlignRoot[iVertexId] = iVertexId;

        //Initialise align[v] to v
        vecDownRightAlign[iVertexId] = iVertexId;
    }


    int iHorizontalPos = 0;
    VertexDescriptor vLowerMedianVertex = 0;
    //Iterate all layers
    IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);

    //Reverse iter
    iterLayer.toBack();

    while(iterLayer.hasPrevious())
    {
        iterLayer.previous();
        MapPositionToLayerNode *currentLayer = iterLayer.value();


        //Iterate  LayerNodes
        IteratorMapPositionToLayerNode iterNode(*currentLayer);
        iterNode.toBack();

        //Reset iAlignedPosition
        int iAlignedPosition = INT_MAX;


        while(iterNode.hasPrevious())
        {
            iterNode.previous();

            LayerNode *currentLayerNode = iterNode.value();

            VertexDescriptor vCurrentVertex = currentLayerNode->getVertex();

            //if current node has Lower neighbors : d and d > 0
            int iTotalLowerNeighbors = 0;
            iTotalLowerNeighbors = out_degree(vCurrentVertex , *m_gMainGraph);

            if(iTotalLowerNeighbors > 0)
            {
                //Iterate Median Lower Vertices
                MapPositionToVertexDescriptor mapPosToLowerMedianVertex = getMedianLowerNeighborsExcludeConflictedEdges(vCurrentVertex);
                IteratorMapPositionToVertexDescriptor iterLowerNeighbor(mapPosToLowerMedianVertex);

                iterLowerNeighbor.toBack();

                //Iterating left to right sorted on (horizontal) position
                while(iterLowerNeighbor.hasPrevious())
                {
                    iterLowerNeighbor.previous();

                    iHorizontalPos = iterLowerNeighbor.key();
                    vLowerMedianVertex = iterLowerNeighbor.value();

                    //if align[v] = v
                    if(vecDownRightAlign[vCurrentVertex] == vCurrentVertex)
                    {
                        //r > position[LowerNeighbor]
                        if(iAlignedPosition > iHorizontalPos)
                        {
                            vecDownRightAlign[vLowerMedianVertex] = vCurrentVertex;
                            vecDownRightAlignRoot[vCurrentVertex] = vecDownRightAlignRoot[vLowerMedianVertex];
                            vecDownRightAlign[vCurrentVertex] = vecDownRightAlignRoot[vCurrentVertex];

                            //Update iAlignedPosition
                            iAlignedPosition = iHorizontalPos;
                        }
                    }

                }


            }

        }



    }

    //Print for testig
//    ////qDebug() << "Down Right Alignement:";
//    int iSize = vecDownRightAlign.size();
//    while(iSize--)
//    {
//        ////qDebug() <<iSize <<" -- "<<vecDownRightAlign[iSize];
//    }

    return vecDownRightAlign;

}

MapPositionToVertexDescriptor HierarchicalLayouter::getMedianUpperNeighborsExcludeConflictedEdges(VertexDescriptor &vGlobalVertex)
{
    MapPositionToVertexDescriptor mapPosToUpperVertex;
    MapPositionToVertexDescriptor mapPosToUpperMedianVertex;;

    VertexDescriptor vInVertex = 0;
    int iHorizontalPos = 0;
    int iInVerticesCounter = 0;
    bool bIsEdgeConflicted = false;

    //Iterate in edges to get upper level vertices
    BGL_FORALL_INEDGES(vGlobalVertex , eInEdge , *m_gMainGraph , SubGraph)
    {
        //Exclude conflicted edges
        bIsEdgeConflicted = m_BoostGraphWrapper.getEdgeIsConflicted(eInEdge , *m_gMainGraph);

        if( bIsEdgeConflicted == true)
        {
            //Skipping conflicted edge
            continue;
        }

        //Upper vertex
        vInVertex = m_BoostGraphWrapper.getEdgeSource(eInEdge , *m_gMainGraph);

        //Horizontal position of upper vertex
        iHorizontalPos = m_BoostGraphWrapper.getVertexHorizontalPosition(vInVertex , *m_gMainGraph);

        mapPosToUpperVertex.insert(iHorizontalPos , vInVertex);

        iInVerticesCounter++;
    }

    double dMean = (iInVerticesCounter + 1 ) / 2.0;
    int iMedianFloor = std::floor(dMean);
    int iMedianCeil = std::ceil(dMean);

    int iCounter = 0;

    IteratorMapPositionToVertexDescriptor iterPosVertex(mapPosToUpperVertex);
    while(iterPosVertex.hasNext())
    {
        iterPosVertex.next();

        iHorizontalPos = iterPosVertex.key();
        vInVertex = iterPosVertex.value();

        iCounter++;

        if(iCounter == iMedianFloor || iCounter == iMedianCeil)
        {
            //Add item to median upper map
            mapPosToUpperMedianVertex.insert(iHorizontalPos , vInVertex);
        }

        if(iCounter == iMedianCeil)
        {
            //break after median ceil encountered to save unnecessary
            //iterating till the end of mapPosToUpperVertex
            break;
        }
    }

    return mapPosToUpperMedianVertex;
}

MapPositionToVertexDescriptor HierarchicalLayouter::getMedianLowerNeighborsExcludeConflictedEdges(VertexDescriptor &vGlobalVertex)
{
    MapPositionToVertexDescriptor mapPosToLowerVertex;
    MapPositionToVertexDescriptor mapPosToLowerMedianVertex;;

    VertexDescriptor vOutVertex = 0;
    int iHorizontalPos = 0;
    int iOutVerticesCounter = 0;
    bool bIsEdgeConflicted = false;

    //Iterate in edges to get Lower level vertices
    BGL_FORALL_OUTEDGES(vGlobalVertex , eOutEdge , *m_gMainGraph , SubGraph)
    {
        //Exclude conflicted edges
        bIsEdgeConflicted = m_BoostGraphWrapper.getEdgeIsConflicted(eOutEdge , *m_gMainGraph);

        if( bIsEdgeConflicted == true)
        {
            //Skipping conflicted edge
            continue;
        }

        //Lower vertex
        vOutVertex = m_BoostGraphWrapper.getEdgeTarget(eOutEdge , *m_gMainGraph);

        //Horizontal position of Lower vertex
        iHorizontalPos = m_BoostGraphWrapper.getVertexHorizontalPosition(vOutVertex , *m_gMainGraph);

        mapPosToLowerVertex.insert(iHorizontalPos , vOutVertex);

        iOutVerticesCounter++;

    }

    double dMean = (iOutVerticesCounter + 1 ) / 2.0;
    int iMedianFloor = std::floor(dMean);
    int iMedianCeil = std::ceil(dMean);

    int iCounter = 0;

    IteratorMapPositionToVertexDescriptor iterPosVertex(mapPosToLowerVertex);
    while(iterPosVertex.hasNext())
    {
        iterPosVertex.next();

        iHorizontalPos = iterPosVertex.key();
        vOutVertex = iterPosVertex.value();

        iCounter++;

        if(iCounter == iMedianFloor || iCounter == iMedianCeil)
        {
            //Add item to median Lower map
            mapPosToLowerMedianVertex.insert(iHorizontalPos , vOutVertex);
        }

        if(iCounter == iMedianCeil)
        {
            //break after median ceil encountered to save unnecessary
            //iterating till the end of mapPosToLowerVertex
            break;
        }
    }

    return mapPosToLowerMedianVertex;
}


void HierarchicalLayouter::horizontalCompaction2(QVectorDouble &vecPositions, const QVectorInt &vecAlignVertex, const QVectorInt &vecRootVertex, const QVectorInt vecLeftNeighborVertex )
{

    try
    {
        unsigned int iNumVertices = 0;

        iNumVertices = num_vertices(*m_gMainGraph);

        LAYOUT_ASSERT(vecPositions.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector Positions must have size = total numebr of vertices"
                                       ,"Vector Positions"));

        LAYOUT_ASSERT(vecAlignVertex.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector AlignVertex must have size = total numebr of vertices"
                                       ,"Vector AlignVertex"));

        LAYOUT_ASSERT(vecAlignVertex.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector RootVertex must have size = total numebr of vertices"
                                       ,"Vector RootVertex"));
        LAYOUT_ASSERT(vecAlignVertex.size() == iNumVertices
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INVALID_PARAMETER
                                       , "Vector LeftNeighborVertex must have size = total numebr of vertices"
                                       ,"Vector LeftNeighborVertex"));





        QVectorInt vecSink(iNumVertices);
        QVectorDouble vecShift(iNumVertices);

        VecMapShiftClass mapShiftClass(iNumVertices);

        //initialise sink[v] = v
        //initialise shift[v] = INFINITY
        for(int iVertex = 0; iVertex < iNumVertices ; iVertex++)
        {
            vecSink[iVertex] = iVertex;
            vecShift[iVertex] = INFINITY_INT;
            vecPositions[iVertex] = UNDEFINED_POS;
        }


        //Root coordinate relative to sink
        for(VertexDescriptor vVertex = 0; vVertex < iNumVertices ; vVertex++)
        {
            //Root vertex
            if(vecRootVertex[vVertex] == vVertex)
            {
                //place_block
                placeBlock2(0
                            , vVertex
                            , vecPositions
                            , vecShift
                            , vecSink
                            , vecAlignVertex
                            , vecRootVertex
                            , vecLeftNeighborVertex
                            , mapShiftClass);

            }
        }


        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);
        while(iterLayer.hasNext())
        {
            iterLayer.next();

            IteratorMapPositionToLayerNode iterNode(*(iterLayer.value()));

            while(iterNode.hasNext())
            {
                iterNode.next();
                VertexDescriptor vVertex = 0;
                vVertex = (iterNode.value())->getVertex();

                //x[v] = x[root[v]]
                vecPositions[vVertex] = vecPositions[vecRootVertex[vVertex]];


                //if (v === root[v] && v === sink[v])
                if(vVertex == vecRootVertex[vVertex] && vVertex == vecSink[vVertex])
                {
                    double dMinShift = INT_MAX;

                    if(mapShiftClass[vVertex].size() > 0)
                    {
                        //Calculate minShift value

                        //Sum up class shift value and vertex shift value
                        QMap<int , double>::iterator iterShiftClass = mapShiftClass[vVertex].begin();
                        while(iterShiftClass != mapShiftClass[vVertex].end())
                        {
                            double dSum = iterShiftClass.value();
                            double dVertexShift = 0;

                            VertexDescriptor vNeighborVertex = iterShiftClass.key();

                            if(vecShift[vNeighborVertex] != INFINITY_INT)
                            {
                                dVertexShift = vecShift[vNeighborVertex];
                            }

                            dSum += dVertexShift;

                            //Update min shift with minimum value
                            if(dMinShift > dSum)
                            {
                                dMinShift = dSum;
                            }

                            iterShiftClass++;
                        }


                    }

                    //shift[v] = minShift;
                    vecShift[vVertex] = dMinShift;

                }

            }
        }

        iterLayer.toFront();
        while(iterLayer.hasNext())
        {
            iterLayer.next();

            IteratorMapPositionToLayerNode iterNode(*(iterLayer.value()));

            while(iterNode.hasNext())
            {
                iterNode.next();
                VertexDescriptor vVertex = 0;
                vVertex = (iterNode.value())->getVertex();

                if(vecShift[vecSink[ vecRootVertex[vVertex]]] < INFINITY_INT)
                {
                    vecPositions[vVertex] += vecShift[vecSink[vecRootVertex[vVertex]]];
                }
            }
        }



        //******************************************************

        bool bHorizontalCompactionSuccess = testHorizontalCompaction(vecPositions);
        LAYOUT_ASSERT(bHorizontalCompactionSuccess == true
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                       , "horizontal compaction has shuffled the node positions"
                                       ,"Node Positions"));



        bool bIsConsistentPositionAlignment = testPositionWithAlignment(vecPositions , vecAlignVertex);
        LAYOUT_ASSERT(bIsConsistentPositionAlignment == true
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                       , "Inconsistent positions with alignment of nodes"
                                       ,"Alignment"));

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}

void HierarchicalLayouter::placeBlock2(int iLevel, VertexDescriptor vVertex, QVectorDouble &vecPositions, QVectorDouble &vecShift, QVectorInt &vecSink, const QVectorInt &vecAlignVertex, const QVectorInt &vecRootVertex, const QVectorInt &vecLeftNeighborVertex, VecMapShiftClass &mapShiftClass)
{
    int iNumVertices = 0;

    try
    {
        iNumVertices = num_vertices(*m_gMainGraph);
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }

    LAYOUT_ASSERT(vecPositions.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector Positions must have size = total numebr of vertices"
                                   ,"Vector Positions"));
    LAYOUT_ASSERT(vecShift.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector Shift must have size = total numebr of vertices"
                                   ,"Vector Shift"));
    LAYOUT_ASSERT(vecSink.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector Sink must have size = total numebr of vertices"
                                   ,"Vector Sink"));
    LAYOUT_ASSERT(vecAlignVertex.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector AlignVertex must have size = total numebr of vertices"
                                   ,"Vector AlignVertex"));
    LAYOUT_ASSERT(vecRootVertex.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector RootVertex must have size = total numebr of vertices"
                                   ,"Vector RootVertex"));
    LAYOUT_ASSERT(vecLeftNeighborVertex.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "Vector LeftNeighborVertex must have size = total numebr of vertices"
                                   ,"Vector LeftNeighborVertex"));

    try
    {


        PGL_MAP_VERTEX_BUNDLED_PROPERTY(mapVertexHorizontalPosition , int , iHorizontalPosition , *m_gMainGraph);

        //----------------------------------------------
        if(vecPositions[vVertex] == UNDEFINED_POS)
        {

            //////qDebug() << printIndent(iLevel)<< "place_block: " << vVertex ;

            //////qDebug()<< printIndent(iLevel)<<"----------------------------------------------------\n";
            vecPositions[vVertex] = 0; //x[v] = 0
            int vNextVertex = vVertex; //w = v

            do
            {

                //////qDebug()<< printIndent(iLevel)<<"w: "<<vNextVertex<<endl;
                //Check if v is first vertex in layer
                if(mapVertexHorizontalPosition[vNextVertex] > 1)
                {
                    //u = root[pred[w]]
                    int vRootVertex = vecRootVertex[vecLeftNeighborVertex[vNextVertex]];

                    //////qDebug()<< printIndent(iLevel)<<"u: "<<vRootVertex<<endl << "calling place_block";

                    //place_block(u)
                    placeBlock2(iLevel+1 ,
                                vRootVertex ,
                                vecPositions ,
                                vecShift,
                                vecSink,
                                vecAlignVertex,
                                vecRootVertex,
                                vecLeftNeighborVertex,
                                mapShiftClass);

                    if(vecSink[vVertex] == vVertex)
                    {

                        //////qDebug()<< printIndent(iLevel)<<"vecSink[v] == v"<<endl;

                        //////qDebug()<< printIndent(iLevel)<<"vecSink[v: "<<vVertex<<"] = "<<"vecSink[u: "<<vRootVertex<<"] = "<<vecSink[vRootVertex]<<endl;
                        //because u is left neighbor of v therefore sinks are same
                        vecSink[vVertex] = vecSink[vRootVertex];
                    }

                    double dDelta = (double)(separation(vecLeftNeighborVertex[vNextVertex]) +
                                             separation(vNextVertex)) / (double)m_iReductionParameterHorizontal;

                    //Check if sinks are not same then:
                    if(vecSink[vVertex] != vecSink[vRootVertex])
                    {
                        //This means a left neighbor has different sink node hence
                        //it pulls the left neighbor class closer, to find out this shift value:
                        //the minimum value between original value i.e. vecShift[vecSink[u]]
                        //and (vecPositions[v] - vecPositions[u] - HORIZONTAL_UNIT_SPACE) is chosen

                        //updateShift(sink[u], sink[v], xs[v] - xs[u] - delta);
                        updateShift(vecSink[vRootVertex] , vecSink[vVertex] , vecPositions[vVertex] - vecPositions[vRootVertex] - dDelta , mapShiftClass);
                    }
                    else
                    {
                        //////qDebug()<< printIndent(iLevel)<<"vecPositions[v: "<<vVertex<<"] = ";

                        vecPositions[vVertex] = std::max(vecPositions[vVertex] , (vecPositions[vRootVertex] + dDelta));

                    }
                }

                vNextVertex = vecAlignVertex[vNextVertex];


            }while(vNextVertex != vVertex);

            //printIndent(iLevel);
            //////qDebug()<<printIndent(iLevel)<<"---------------------------------close block\n";
        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}

void HierarchicalLayouter::updateShift(int iToShift, int iNeighbor, double dDelta , VecMapShiftClass &mapShiftClass)
{
    if(mapShiftClass[iToShift].contains(iNeighbor))
    {
        mapShiftClass[iToShift][iNeighbor] = std::min(mapShiftClass[iToShift][iNeighbor] , dDelta);
    }
    else
    {
        mapShiftClass[iToShift][iNeighbor] = dDelta;
    }


}

int HierarchicalLayouter::separation(VertexDescriptor vVertex)
{
    Q_ASSERT_X(vVertex >= 0 , "Separation" , "invalid vertex");

    int iSeparation = 0;

    int  iWidth = m_BoostGraphWrapper.getVertexWidth(vVertex , *m_gMainGraph);

    //Half of the width of node
    iSeparation += iWidth;

    LayoutEnum::NodeType enVertexType = m_BoostGraphWrapper.getVertexType(vVertex , *m_gMainGraph);

    //Add edge or node separation contant
    if(enVertexType == LayoutEnum::DummyNode)
    {
        iSeparation += m_iEdgeSeparation;
    }
    else if(enVertexType == LayoutEnum::VerticalBorderNode)
    {
        iSeparation += m_iBorderMargin;
    }
    else
    {
        iSeparation += m_iNodeSeparation;
    }

    iSeparation /= 2;

    return iSeparation;
}

int HierarchicalLayouter::shiftVertexCoordinateToLeftTop()
{
    int iXCoordinate = 0;
    int iYCoordinate = 0;
    int iHeight = 0;
    int iWidth = 0;

    try
    {
        BGL_FORALL_VERTICES(vVertex , *m_gMainGraph , SubGraph)
        {
            LayoutEnum::NodeType enVertexType =  m_BoostGraphWrapper.getVertexType(vVertex , *m_gMainGraph);

            if(enVertexType != LayoutEnum::GraphNode)
            {
                //continue;
            }

            iXCoordinate = m_BoostGraphWrapper.getVertexCenterCoordX(vVertex , *m_gMainGraph);
            iYCoordinate = m_BoostGraphWrapper.getVertexCenterCoordY(vVertex , *m_gMainGraph);

            iHeight = m_BoostGraphWrapper.getVertexHeight(vVertex , *m_gMainGraph);
            iWidth = m_BoostGraphWrapper.getVertexWidth(vVertex , *m_gMainGraph);

            if(iHeight > 0 && iWidth > 0)
            {
                iXCoordinate -= (iWidth / 2);
                iYCoordinate -= (iHeight / 2);
            }

            m_BoostGraphWrapper.setVertexLeftCoordX(vVertex , *m_gMainGraph , iXCoordinate);
            m_BoostGraphWrapper.setVertexLeftCoordY(vVertex , *m_gMainGraph , iYCoordinate);


        }

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }
}


QString HierarchicalLayouter::printIndent(int iLevel)
{

    QString sIndent = "";
    int iSpaces = 2;
    for(int i = 0; i <= (iLevel * iSpaces) ;i++)
    {
        if(i % iSpaces == 0)
        {
            sIndent.append("|");
        }
        else
        {
            sIndent.append(" ");
        }
    }

    return sIndent;
}

int HierarchicalLayouter::getFinalCrossings()
{
    return m_iFinalCrossings;
}


//************************************************************************************************************
/*Testing Functions:
 */
bool HierarchicalLayouter::testUpwardEdgesAndRanks(SubGraph &gGraph)
{
    VertexDescriptor vSource , vTarget;
    int iSourceRank , iTargetRank;
    bool bUpwardEdgeFound = false;
    int iUpwardEdges = 0;
    BGL_FORALL_EDGES(eEdge , gGraph , SubGraph)
    {
        vSource = m_BoostGraphWrapper.getEdgeSource(eEdge , gGraph);
        vTarget = m_BoostGraphWrapper.getEdgeTarget(eEdge , gGraph);

        iSourceRank = m_BoostGraphWrapper.getVertexRank(vSource , gGraph);
        iTargetRank = m_BoostGraphWrapper.getVertexRank(vTarget , gGraph);

        if(iSourceRank > iTargetRank)
        {
            ++iUpwardEdges;
        }

    }

    if(iUpwardEdges > 0)
    {
        bUpwardEdgeFound = true;
    }

    return bUpwardEdgeFound;
}

bool HierarchicalLayouter::testLayeredGraph()
{
    ////qDebug() << "Testing Layered Graph:";

    int  bIsLayeredGraphCorrect = true;

    //Check if all nodes are present in Layered Graph
    std::size_t iTotalVertices = num_vertices(*m_gMainGraph);

    std::vector<bool> vertexVisited(iTotalVertices);

    std::fill(vertexVisited.begin() , vertexVisited.end() , false);

    //Iterate Layers
    IteratorMapLayerIdToLayerRef iterLayers(m_mapLayeredGraph);

    int iTotalBadPositionNodeCount = 0;

    while(iterLayers.hasNext())
    {
        iterLayers.next();
        MapPositionToLayerNode * currentLayer = iterLayers.value();

        ////qDebug() << "Layer Rank  : " << QString::number(iterLayers.key());
        //Test Layer: layer node keys and corresponding Vertex iHorizontalPosition are same
        int iBadPositionNodeCount = testGetLayerKeysAndVertexPositionNotConsistentCount(*currentLayer , *m_gMainGraph);

        iTotalBadPositionNodeCount += iBadPositionNodeCount;

    }

    ////qDebug() << "Total Inconsistent Nodes: " << QString::number(iTotalBadPositionNodeCount);

    if(iTotalBadPositionNodeCount > 0)
    {
        bIsLayeredGraphCorrect =  false;
    }
    return bIsLayeredGraphCorrect;
}

int HierarchicalLayouter::testGetLayerKeysAndVertexPositionNotConsistentCount(MapPositionToLayerNode &mapPositionToLayerNode, SubGraph &gMainGraph)
{
    //////qDebug() << "Testing inconsistent LayerNode positions";

    QString sInconsistentNodeList = "";

    int iLayerKeysAndVertexPositionNotConsistentCount = 0;

    IteratorMapPositionToLayerNode iterLayerNodes(mapPositionToLayerNode);

    while(iterLayerNodes.hasNext())
    {
        iterLayerNodes.next();
        //Copy Layer Node key and pointer
        int icurrentLayerNodeKey = iterLayerNodes.key();
        LayerNode *currentLayerNode = iterLayerNodes.value();

        //Compare layer node key with Vertex horizontal position
        VertexDescriptor vCurrentVertex = currentLayerNode->getVertex();
        if(m_BoostGraphWrapper.getVertexHorizontalPosition( vCurrentVertex,
                                                           gMainGraph)
            != icurrentLayerNodeKey)
        {
            iLayerKeysAndVertexPositionNotConsistentCount++;
            sInconsistentNodeList.append(" -");
            sInconsistentNodeList.append(QString::number(icurrentLayerNodeKey));

        }

    }

    //////qDebug() << "Incosistent Node: " << sInconsistentNodeList;

    //////qDebug() << "Testing inconsistent LayerNode positions END";

    return iLayerKeysAndVertexPositionNotConsistentCount;
}

bool HierarchicalLayouter::testHorizontalCompaction(QVectorInt &vecAlignedPosition)
{
    unsigned int iNumVertices = 0;

    iNumVertices = num_vertices(*m_gMainGraph);

    LAYOUT_ASSERT(vecAlignedPosition.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                   , "vecAlignedPosition"
                                   , "size mis matched with number of vertices"));


    bool bHorizontalCompactionCorrect = true;
    VertexDescriptor vCurrent = 0;
    VertexDescriptor vNext = 0;

    LayerNode* currentNode = NULL;
    LayerNode* nextNode = NULL;

    int iPosCurrentVertex = 0;
    int iPosNextVertex = 0;

    //Iterate nodes layer by layer
    IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);
    while(iterLayer.hasNext())
    {
        iterLayer.next();

        int iLayer = iterLayer.key();
        ////qDebug() << "Layer: "<<iLayer;

        IteratorMapPositionToLayerNode iterNode(*(iterLayer.value()));
        while(iterNode.hasNext())
        {
            iterNode.next();

            if(iterNode.hasNext())
            {
                LayoutEnum::NodeType nodeType = m_BoostGraphWrapper.getVertexType(vCurrent
                                                                            , *m_gMainGraph);
                if(nodeType == LayoutEnum::UpperBorderNode ||
                        nodeType == LayoutEnum::LowerBorderNode)
                {
                    //skip border nodes
                    continue;
                }

                //get current node
                currentNode = iterNode.value();
                //get current vertex
                vCurrent = currentNode->getVertex();

                //get next node
                nextNode = (iterNode.peekNext()).value();
                //get next vertex
                vNext = nextNode->getVertex();

                //get horizontal positions from aligned position vector
                iPosCurrentVertex = vecAlignedPosition[vCurrent];
                iPosNextVertex = vecAlignedPosition[vNext];

                // iPosCurrentVertex < iPosNextVertex must be true
                if(iPosCurrentVertex == iPosNextVertex)
                {
                    bHorizontalCompactionCorrect = false;
                    ////qDebug() << "Vertex position overlap, Pos: " << iPosCurrentVertex <<"Vertex: "<<vCurrent <<" , "<<vNext;
                }
                else if(iPosCurrentVertex > iPosNextVertex)
                {
                    bHorizontalCompactionCorrect = false;
                    ////qDebug() << "Vertex position reversed, v- "<<vCurrent<<" , "<<vNext<<" , Pos: "<< iPosCurrentVertex << " , " << iPosNextVertex;
                }
            }
        }


    }

    return bHorizontalCompactionCorrect;
}

bool HierarchicalLayouter::testHorizontalCompaction(QVectorDouble &vecAlignedPosition)
{
    unsigned int iNumVertices = 0;

    iNumVertices = num_vertices(*m_gMainGraph);

    LAYOUT_ASSERT(vecAlignedPosition.size() == iNumVertices
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                   , "vecAlignedPosition"
                                   , "size mis matched with number of vertices"));


    bool bHorizontalCompactionCorrect = true;
    VertexDescriptor vCurrent = 0;
    VertexDescriptor vNext = 0;

    LayerNode* currentNode = NULL;
    LayerNode* nextNode = NULL;

    int iPosCurrentVertex = 0;
    int iPosNextVertex = 0;

    //Iterate nodes layer by layer
    IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);
    while(iterLayer.hasNext())
    {
        iterLayer.next();

        //int iLayer = iterLayer.key();
        ////qDebug() << "Layer: "<<iLayer;

        IteratorMapPositionToLayerNode iterNode(*(iterLayer.value()));
        while(iterNode.hasNext())
        {
            iterNode.next();

            if(iterNode.hasNext())
            {
                LayoutEnum::NodeType nodeType = m_BoostGraphWrapper.getVertexType(vCurrent
                                                                            , *m_gMainGraph);
                if(nodeType == LayoutEnum::UpperBorderNode ||
                        nodeType == LayoutEnum::LowerBorderNode)
                {
                    //skip border nodes
                    continue;
                }

                //get current node
                currentNode = iterNode.value();
                //get current vertex
                vCurrent = currentNode->getVertex();

                //get next node
                nextNode = (iterNode.peekNext()).value();
                //get next vertex
                vNext = nextNode->getVertex();

                //get horizontal positions from aligned position vector
                iPosCurrentVertex = vecAlignedPosition[vCurrent];
                iPosNextVertex = vecAlignedPosition[vNext];

                // iPosCurrentVertex < iPosNextVertex must be true
                if(iPosCurrentVertex == iPosNextVertex)
                {
                    //bHorizontalCompactionCorrect = false;
                    ////qDebug() << "Vertex position overlap, Pos: " << iPosCurrentVertex <<"Vertex: "<<vCurrent <<" , "<<vNext;
                }
                else if(iPosCurrentVertex > iPosNextVertex)
                {
                    bHorizontalCompactionCorrect = false;
                    ////qDebug() << "Vertex position reversed, v- "<<vCurrent<<" , "<<vNext<<" , Pos: "<< iPosCurrentVertex << " , " << iPosNextVertex;
                }
            }
        }


    }

    return bHorizontalCompactionCorrect;
}

bool HierarchicalLayouter::testPositionWithAlignment(const QVectorInt &vecAlignedPosition, const QVectorInt &vecAlign)
{
    bool bIsConsistentPositionAlignment = true;

    int iTotalVertices = 0;
    iTotalVertices = num_vertices(*m_gMainGraph);

    for(int iVertex = 0 ; iVertex < iTotalVertices ; iVertex++)
    {
        //check position of current vertex with position of aligned vertex
        if(vecAlignedPosition[iVertex] != vecAlignedPosition[vecAlign[iVertex]])
        {
            bIsConsistentPositionAlignment = false;
            break;
        }
    }

    return bIsConsistentPositionAlignment;
}

bool HierarchicalLayouter::testPositionWithAlignment(const HierarchicalLayouter::QVectorDouble &vecAlignedPosition, const QVectorInt &vecAlign)
{
    bool bIsConsistentPositionAlignment = true;

    int iTotalVertices = 0;
    iTotalVertices = num_vertices(*m_gMainGraph);

    for(int iVertex = 0 ; iVertex < iTotalVertices ; iVertex++)
    {
        //check position of current vertex with position of aligned vertex
        if(std::abs(vecAlignedPosition[iVertex] - vecAlignedPosition[vecAlign[iVertex]]) > 0.5)
        {
            bIsConsistentPositionAlignment = false;
            break;
        }
    }

    return bIsConsistentPositionAlignment;
}

void HierarchicalLayouter::writeLog(QString sExecutionDetails)
{
    QString sFileName;
    sFileName = FILENAME_EXECUTION_TIME_DETAILS;
    sFileName = m_sExecutionLogFileName;

    if (!sFileName.isEmpty())
    {
        QString sLogFilePath = sFileName;
        QFile logFile(sLogFilePath);
        if (!logFile.open(QFile::Append)) {
            cout<<"File could not be open. Exiting.";
            exit(1);
        }

        QTextStream txtStreamOut;

        txtStreamOut.setDevice(&logFile);
        txtStreamOut.setCodec("UTF-8");

        txtStreamOut << sExecutionDetails.toUtf8();
        txtStreamOut << "\n";
    }
    else
    {
        return;
    }



}

bool HierarchicalLayouter::testNestingTree()
{
    bool bIsNestingTreeCorrect = true;

    QQueue<NestingTreeSubgraphNode*> qNestingTreeNodes;

    qNestingTreeNodes.enqueue(&m_rootNestingTreeSubgraphNode);

    NestingTreeSubgraphNode * currentNestingTreeNode = NULL;

    while(qNestingTreeNodes.isEmpty() == false)
    {
        currentNestingTreeNode = qNestingTreeNodes.dequeue();

        SubGraph & currentSubgraph = (currentNestingTreeNode->getGraph());

        QString sGraphName = m_BoostGraphWrapper.getGraphId(currentSubgraph);

        ////qDebug() << "Graph: "<<sGraphName;

        //check if root or not
        if(currentNestingTreeNode->isRoot())
        {

        }
        else
        {
            //check parent node is not null
            if(&(currentNestingTreeNode->getParent()) == NULL)
            {
                 bIsNestingTreeCorrect = false;
                ////qDebug()<<"Missing Parent: Non root graph must have Parent.";
            }
        }


        //check no of subgraphs
        int iTotalSubgraphs = currentSubgraph.num_children();

        int iTotalChildNestingTreeNodes = currentNestingTreeNode->getChildNestingTreeSubgraphNodes().size();

        if(iTotalSubgraphs != iTotalChildNestingTreeNodes)
        {
            bIsNestingTreeCorrect = false;

            ////qDebug() << "Incorrect number of child subgraphs : "<<iTotalChildNestingTreeNodes << " actual: "<< iTotalSubgraphs;
        }


        //check no of nodes with own nodes count
        int iTotalNodes = currentNestingTreeNode->getMapLayerIdToLayerNodeRef().size();

        int iTotalOwnVertices = m_BoostGraphWrapper.getCountOfOwnVertices(currentSubgraph);

        if(iTotalNodes != iTotalOwnVertices)
        {
            bIsNestingTreeCorrect = false;

            ////qDebug() << "Incorrect number of own vertices: "<< iTotalNodes <<" actual: "<<iTotalOwnVertices;
        }

        //Check the layer id with vertex actual rank property, it should be same
        NestingTreeSubgraphNode::IteratorMultiMapLayerIdToLayerNodeRef iterLayerNodes
                = currentNestingTreeNode->getChildLayerNodesIterator();
        while(iterLayerNodes.hasNext())
        {
            iterLayerNodes.next();

            int iLayerId = iterLayerNodes.key();
            LayerNode *layerNode = iterLayerNodes.value();

            VertexDescriptor vCurrentVertex = layerNode->getVertex();

            int iRank = m_BoostGraphWrapper.getVertexRank(vCurrentVertex , *m_gMainGraph);

            if(iRank != iLayerId)
            {
                bIsNestingTreeCorrect = false;

                ////qDebug() << "Incorrect layer id assigned to vertex, layer id: " << iLayerId<<" Rank: " << iRank;
            }

        }


        //enque next level Nesting Tree Nodes in queue
        NestingTreeSubgraphNode::IteratorVectorNestingTreeSubgraphNodesRef iterChildNestingTreeNodes
                = currentNestingTreeNode->getIteratorChildNestingTreeSubgraphNodes();
        while (iterChildNestingTreeNodes.hasNext())
        {
            qNestingTreeNodes.enqueue(iterChildNestingTreeNodes.next());
        }

    }

    return bIsNestingTreeCorrect;
}

bool HierarchicalLayouter::testReducedNestingTree(ReducedNestingTreeNode &reducedNestingTreeRoot)
{
    //*THIS TEST IS NOT CREATED COMPLETELY*

    Q_ASSERT_X(reducedNestingTreeRoot.isLayerNode() == false , "testReducedNestingTree" ,
               "Invalid reduced nesting tree root node provided, It must be NestingTreeNode type RNT Node");
    bool bIsReducedNestingTreeCorrect = true;

    QQueue<ReducedNestingTreeNode*> qRNTNodes;

    //Add root
    qRNTNodes.enqueue(&reducedNestingTreeRoot);

    NestingTreeSubgraphNode *currentNestingTreeNode = NULL;
    LayerNode *currentLayerNode = NULL;

    ReducedNestingTreeNode * currentReducedNestingTreeNode = NULL;

    while(qRNTNodes.isEmpty() == false)
    {
        currentReducedNestingTreeNode = qRNTNodes.dequeue();

        if(currentReducedNestingTreeNode->isLayerNode())
        {
            currentLayerNode = currentReducedNestingTreeNode->getLayerNode();
            currentNestingTreeNode = NULL;
        }
        else
        {
            currentLayerNode = NULL;
            currentNestingTreeNode = currentReducedNestingTreeNode->getNestingTreeNode();
        }

        //TODO:
        //Check number of child nodes equal to num_child graphs + no. of vertices
    }


    return bIsReducedNestingTreeCorrect;
}

bool HierarchicalLayouter::testSubgraphOrderingGraph()
{
    bool bIsSOGCorrect = true;

    int iTotalVertices = 0;
    iTotalVertices = num_vertices(*m_gMainGraph);

    QSet<std::size_t> setVerticesAddedToSOG;

    //check all layer nodes are placed in SubgraphOrderingGraph
    IteratorHashVertexToLayerNode iterLayerNode(hashVertexToLayerNode);
    while(iterLayerNode.hasNext())
    {
        iterLayerNode.next();

        LayerNode* layerNode = iterLayerNode.value();
        VertexDescriptor vVertex = layerNode->getVertex();

        if(layerNode->getSubgraphOrderingGraphVertex() < 0)
        {


            ////qDebug() << "LayerNode not added to SOG, V: "<<(int)vVertex;
            layerNode->printName();

            bIsSOGCorrect = false;
        }

        //Check if layernode is added more than once
        if(setVerticesAddedToSOG.contains(vVertex))
        {
            ////qDebug() << "LayerNode already added to SOG, V: "<<(int)vVertex;
            layerNode->printName();
        }
        else
        {
            setVerticesAddedToSOG.insert(vVertex);
        }

    }

    IteratorMapNestingTreeRefToSubgraphOrderingGraphRef iterNestingTreeSOGRef(
                m_mapNestingTreeNodeRefToSubgraphOrderingGraphRef)    ;
    while(iterNestingTreeSOGRef.hasNext())
    {
        iterNestingTreeSOGRef.next();

        NestingTreeSubgraphNode* nestingTreeNode = iterNestingTreeSOGRef.key();
        SubgraphOrderingGraphType* gSOG = iterNestingTreeSOGRef.value();

        //check if total vertices in SOG are equal to =
        //layernodes + nestingtree nodes
        int iNumVerticesInSOG = num_vertices(*gSOG);

        int iTotalLayerNodes = nestingTreeNode->getCountOfLayerNodes();
        int iTotalChildNTNodes = nestingTreeNode->getCountOfChildNestingTreeSubgraphNodes();

        if(iNumVerticesInSOG != (iTotalLayerNodes + iTotalChildNTNodes))
        {
            ////qDebug() << "Incorrect number of nodes in SOG: "<<iNumVerticesInSOG<<" It should be: "<<(iTotalLayerNodes + iTotalChildNTNodes);

            bIsSOGCorrect = false;
        }
    }


    return bIsSOGCorrect;

}

bool HierarchicalLayouter::testIsLayerTopologicallySorted(int iLayerId)
{
    /*This function is to be used only when just sorted a layer topologically
     *according to the current topological order of SugraphOrderingGraph.
     *Do not use it after calculating new topological ordering of SubgraphOrderngGraph
     *Because a graph can have many Topological orders so every layer is sorted
     *topologically but with different topological orders of SubgraphOrderingGraph
     *
     */
    LAYOUT_ASSERT(m_mapLayeredGraph.contains(iLayerId)==true, LayoutException(__FUNCTION__
                                                                              , LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER
                                                                              , "Layered Graph"
                                                                              , "iLayerId"));
    bool bIsTopologicallysorted = true;

    int iTopologicalOrder = 0;
    int iNextTopologicalOrder = 0;
    LayerNode *layerNode = NULL;
    VertexDescriptor vVertex = 0;
    VertexDescriptor vNextVertex = 0;


    IteratorMapPositionToLayerNode iterLayerNodes(*(m_mapLayeredGraph[iLayerId]));
    while(iterLayerNodes.hasNext())
    {
        iterLayerNodes.next();

        layerNode = iterLayerNodes.value();

        vVertex = layerNode->getVertex();

        if(iterLayerNodes.hasNext())
        {


            iTopologicalOrder = m_BoostGraphWrapper.getVertexTopologicalOrder(vVertex , *m_gMainGraph);

            layerNode = (iterLayerNodes.peekNext()).value();

            vNextVertex = layerNode->getVertex();

            iNextTopologicalOrder = m_BoostGraphWrapper.getVertexTopologicalOrder(vNextVertex , *m_gMainGraph);

            if(iTopologicalOrder > iNextTopologicalOrder)
            {
                ////qDebug() << "Reversed Topo Order: v: "<<vVertex<<" , "<<vNextVertex<<"order: "<<iTopologicalOrder<<" , "<<iNextTopologicalOrder;

                bIsTopologicallysorted = false;
            }

        }
    }

    return bIsTopologicallysorted;

}



//************************************************************************************************************


void HierarchicalLayouter::setHorizontalPositionsForVerticalBorderNodes()
{
    LAYOUT_ASSERT(m_iRankDifferenceInLayers.isSet() == true,
                  LayoutException(__FUNCTION__
                                  ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                  ,"Rank Difference in layers"
                                  ,""));

    try
    {

        //Space horizontal positions of Layer nodes by m_iRankDifferenceInLayers
        //It will create space for VerticalBorderNodes
        IteratorMapLayerIdToLayerRef iterLayer(m_mapLayeredGraph);

        MapPositionToLayerNode* newLayer = NULL;
        int iHorizontalPosition = 0;
        LayerNode* currentLayerNode = NULL;
        VertexDescriptor vCurrentVertex = 0;
        int iLayerId = 0;

        while(iterLayer.hasNext())
        {
            iterLayer.next();

            //create new layer
            newLayer = new MapPositionToLayerNode();

            //Create old layer copy in newLayer with new iHorizontalPosition
            IteratorMapPositionToLayerNode iterLayerNode(*(iterLayer.value()));
            while(iterLayerNode.hasNext())
            {
                iterLayerNode.next();

                currentLayerNode = iterLayerNode.value();
                vCurrentVertex = currentLayerNode->getVertex();

                iHorizontalPosition = m_BoostGraphWrapper.getVertexHorizontalPosition(
                            vCurrentVertex , *m_gMainGraph);

                iHorizontalPosition *= m_iRankDifferenceInLayers;

                //Update iHorizontalPosition in graph
                m_BoostGraphWrapper.setVertexHorizontalPosition(vCurrentVertex ,*m_gMainGraph ,iHorizontalPosition);

                //Add to newLayer
                newLayer->insert(iHorizontalPosition , currentLayerNode);

            }

            //Delete old layer and replace with new layer
            iLayerId = iterLayer.key();

            m_mapLayeredGraph[iLayerId]->clear();
            DELETE_AND_SET_NULL(m_mapLayeredGraph[iLayerId]);

            m_mapLayeredGraph[iLayerId] = newLayer;

            newLayer = NULL;

        }
    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

}
