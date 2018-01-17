#include "LayoutManager.h"
LayoutManager::LayoutManager()
{
}

SubGraph &LayoutManager::applyLayout(SubGraph &gMainGraph, const LayoutType &enLayoutType)
{
    switch(enLayoutType)
   {
        case CircularLayout:
        {
            //Apply Circular Layout
            CircularLayoutGenerator CircularLayouter;
            LayoutEnum::VertexOrderCriteria enVertexOrder;
            CircularLayouter.applyCircularLayout(gMainGraph, enVertexOrder);
        }
            break;
        case HierarchicalLayout:
        {
            //Apply Hierarchical Layout
            HierarchicalLayouter hierarchicalLayouter;
            hierarchicalLayouter.applyHierarchicalLayout(gMainGraph);
        }
            break;

        case RandomLayout:
        {
            //Apply Hierarchical Layout
            RandomLayoutGenerator randomLayouter;
            randomLayouter.applyRandomLayout(gMainGraph);
        }
            break;

        case GridBasedLayout:
        {
            //Apply Grid Based Layout
            this->applyGridBasedLayout(&gMainGraph);
        }
            break;


        case ForceDirectedLayout:
      {
        //Apply Force Directed Layout

        applyForceDirectedLayout(&gMainGraph,iForceDirectedIterations);

      }
        break;




      return gMainGraph;
   }
}


bool LayoutManager::applyLayout(LayoutType enLayoutType , QString strInputGraphmlPath,
                                QString strOutputGraphMLPath)
{
    LAYOUT_ASSERT(!strInputGraphmlPath.isEmpty() && !strOutputGraphMLPath.isEmpty(),
                  LayoutException(__FUNCTION__
                                  ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                  ,"Input and Output graphml path"
                                  ,"applyLayout"));



    bool bLayoutStatus = false;

    //Read graphml
    QFile inFile(strInputGraphmlPath);
    GraphMLReader reader;
    SubGraph &gInGraph = reader.readGraphML(&inFile);

    //Initialize order property
    BoostGraphWrapper boostGraphWrapper;
    boostGraphWrapper.initGraphVertexOrderValues(gInGraph);

    //Apply layout
    SubGraph &gLaidoutGraph = applyLayout(gInGraph , enLayoutType);

    //Write back
    QFile outFile(strOutputGraphMLPath);
    if (!outFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        //cout<<"Write path file could not be open. Exiting.\n";
    }
    else
    {
        GraphMLWriter writer;
        writer.writeGraphml(gLaidoutGraph , &outFile);
        bLayoutStatus = true;
    }

    return bLayoutStatus;
}

GraphLayoutErrorCodes::LayoutErrorCode LayoutManager::applyCircularLayout(QString sInputGraphMLFilePath, QString sOutputGraphMLFilePath, LayoutEnum::VertexOrderCriteria enVertexOrder)
{

    GraphLayoutErrorCodes::LayoutErrorCode enCircularLayoutErrorCode= GraphLayoutErrorCodes::LAYOUT_SUCCESS;


    if( (sInputGraphMLFilePath.isEmpty() == true))
    {
        enCircularLayoutErrorCode = GraphLayoutErrorCodes::INVALID_FILE_NAME;
    }

    if((sInputGraphMLFilePath.trimmed().endsWith(GRAPHML, Qt::CaseInsensitive) == false)
            && (enCircularLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
    {
        enCircularLayoutErrorCode = GraphLayoutErrorCodes::UNSUPPORTED_FILE_FORMAT;
    }

    if((QFile::exists(sInputGraphMLFilePath) == false) && (enCircularLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
    {
        enCircularLayoutErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
    }

    // create global graph pointer.
    SubGraph *gMainGraph;

    if(enCircularLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        // read input GraphML file
        QFile inFile(sInputGraphMLFilePath);
        GraphMLReader reader;
        try
        {
            SubGraph &gInputGraph = reader.readGraphML(&inFile);
            gMainGraph = &(gInputGraph);
            inFile.close();
        }
        catch(LayoutFileIOException& eException)
        {
            inFile.close();
            //We return the file exception enums
            enCircularLayoutErrorCode = (GraphLayoutErrorCodes::LayoutErrorCode)eException.getErrorCode();
        }
        catch(...)
        {
            inFile.close();
            enCircularLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
        }
    }

    if(enCircularLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        // apply circular layout
        CircularLayoutGenerator circularLayoutGenerator;
        try
        {
            circularLayoutGenerator.applyCircularLayout(*gMainGraph, enVertexOrder);
        }
        catch(...)
        {
            DELETE_AND_SET_NULL(gMainGraph);

            enCircularLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
        }


    }

    if(enCircularLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        // write file to output path
        QFile outFile(sOutputGraphMLFilePath);
        if ((!outFile.open(QFile::WriteOnly | QFile::Truncate)))
        {
            enCircularLayoutErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
        }

        if(enCircularLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
        {
            GraphMLWriter writer;
            try
            {
                writer.writeGraphml(*gMainGraph , &outFile);
                outFile.close();
            }
            catch(...)
            {
                DELETE_AND_SET_NULL(gMainGraph);
                enCircularLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
                outFile.close();
            }

        }
    }
    return enCircularLayoutErrorCode;

}

GraphLayoutErrorCodes::LayoutErrorCode LayoutManager::applyGridBasedLayout(QString sInputGraphMLFilePath,
                                             QString sOutputGraphMLFilePath)
{
    GraphLayoutErrorCodes::LayoutErrorCode enGridBasedLayoutErrorCode= GraphLayoutErrorCodes::LAYOUT_SUCCESS;

    //Read input file-------------------------------------------------------------------------------------------
    if( (sInputGraphMLFilePath.isEmpty() == true))
    {
        enGridBasedLayoutErrorCode = GraphLayoutErrorCodes::INVALID_FILE_NAME;
    }

    if((sInputGraphMLFilePath.trimmed().endsWith(GRAPHML, Qt::CaseInsensitive) == false)
            && (enGridBasedLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
    {
        enGridBasedLayoutErrorCode = GraphLayoutErrorCodes::UNSUPPORTED_FILE_FORMAT;
    }

    if((QFile::exists(sInputGraphMLFilePath) == false) && (enGridBasedLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
    {
        enGridBasedLayoutErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
    }

    // create global graph pointer.
    SubGraph *gMainGraph;

    if(enGridBasedLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        // read input GraphML file
        QFile inFile(sInputGraphMLFilePath);
        GraphMLReader reader;
        try
        {
            SubGraph &gInputGraph = reader.readGraphML(&inFile);
            gMainGraph = &(gInputGraph);
            inFile.close();
        }
        catch(LayoutFileIOException& eException)
        {
            inFile.close();
            //We return the file exception enums
            enGridBasedLayoutErrorCode = (GraphLayoutErrorCodes::LayoutErrorCode)eException.getErrorCode();
        }
        catch(...)
        {

            enGridBasedLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
        }
    }
    //---------------------------------------------------------------------------------------------------------

    this->applyGridBasedLayout(gMainGraph);

    //Write output file----------------------------------------------------------------------------------------
    if(enGridBasedLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        QFile outFile(sOutputGraphMLFilePath);
        if ((!outFile.open(QFile::WriteOnly | QFile::Truncate)))
        {
            enGridBasedLayoutErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
        }

        if(enGridBasedLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
        {
            GraphMLWriter writer;
            try
            {
                writer.writeGraphml(*gMainGraph , &outFile);
                outFile.close();
            }
            catch(...)
            {
                outFile.close();
                //DELETE_AND_SET_NULL(*gMainInputGraph);
                enGridBasedLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
            }
        }
    }
    //---------------------------------------------------------------------------------------------------------

    return enGridBasedLayoutErrorCode;
}


void LayoutManager::applyGridBasedLayout(SubGraph * gMainInputGraph)
{
    //FIND LAYOUT--------------------------------------------------------------------------------------------
    //Initialize cluster and vertex IDs, that will be used to identify which vertex is in which cluster
    MembershipInitializer membershipInitializer;
    membershipInitializer.initializeMembers(*gMainInputGraph);

    //Remove parallel edges
    int iInitialNumEdges = boost::num_edges(*gMainInputGraph);
    GraphPreProcessor graphPreProcessor;
    std::vector<QString> oParallelEdgeIDs;
    graphPreProcessor.removeParallelEdges(*gMainInputGraph, m_oParallelEdges, oParallelEdgeIDs);
    LAYOUT_ASSERT(iInitialNumEdges == (boost::num_edges(*gMainInputGraph) + size(m_oParallelEdges)),
                  LayoutException("removeParallelEdges",
                                  LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                  "gMainInputGraph",
                                  "after removeParallelEdges"));

    //Call gridLayouterForClusteredGraph()
    GridLayouter gridLayouter;
    gridLayouter.gridLayouterForClusteredGraph(*gMainInputGraph, *gMainInputGraph);
//    gridLayouter.gridLayouterForNonClusteredGraph(*gMainInputGraph);

    //Add parallel edges back
    int iIntermediateNumEdges = boost::num_edges(*gMainInputGraph);
    graphPreProcessor.addParallelEdges(*gMainInputGraph, m_oParallelEdges, oParallelEdgeIDs);
    LAYOUT_ASSERT(boost::num_edges(*gMainInputGraph) == (iIntermediateNumEdges + size(m_oParallelEdges)),
                  LayoutException("addParallelEdges",
                                  LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE,
                                  "gMainInputGraph",
                                  "after addParallelEdges"));
    //------------------------------------------------------------------------------------------------------

    //set default cluster size------------------------------------------------------------------------------
    BoostGraphWrapper boostGraphWrapper;

    int iDefaultGraphHeight = boostGraphWrapper.getGraphHeight(*gMainInputGraph);
    int iDefaultGraphWidth = boostGraphWrapper.getGraphWidth(*gMainInputGraph);
    boostGraphWrapper.setGraphHeight(iDefaultGraphHeight + 130, *gMainInputGraph);
    boostGraphWrapper.setGraphWidth(iDefaultGraphWidth + 130, *gMainInputGraph);

    int iDefaultGraphLeftTopX = boostGraphWrapper.getGraphLeftTopCoordX(*gMainInputGraph);
    int iDefaultGraphLeftTopY = boostGraphWrapper.getGraphLeftTopCoordY(*gMainInputGraph);
    boostGraphWrapper.setGraphLeftTopCoordX(iDefaultGraphLeftTopX - 65, *gMainInputGraph);
    boostGraphWrapper.setGraphLeftTopCoordY(iDefaultGraphLeftTopY - 65, *gMainInputGraph);
    //------------------------------------------------------------------------------------------------------

    //set NodeType and EdgeType-----------------------------------------------------------------------------
    VertexIterator itrVert, itrVertEnd;
    for(boost::tie(itrVert, itrVertEnd)=vertices(*gMainInputGraph); itrVert!=itrVertEnd; ++itrVert)
    {
        VertexDescriptor vCurrentVertex = *itrVert;
        //Enum::NodeType enNodeType = Enum::GraphNode;
        boostGraphWrapper.setVertexType(vCurrentVertex, *gMainInputGraph, LayoutEnum::GraphNode);
    }
    EdgeIterator itrEdge, itrEdgeEnd;
    for(boost::tie(itrEdge, itrEdgeEnd)=edges(*gMainInputGraph); itrEdge!=itrEdgeEnd; ++itrEdge)
    {
        EdgeDescriptor eCurrentEdge = *itrEdge;
        //Enum::EdgeType enEdgeType = Enum::GraphEdge;
        boostGraphWrapper.setEdgeType(eCurrentEdge, *gMainInputGraph, LayoutEnum::GraphEdge);
    }
    //------------------------------------------------------------------------------------------------------
}


GraphLayoutErrorCodes::LayoutErrorCode  LayoutManager::applyHierarchicalLayout(QString sInputGraphMLFilePath, QString sOutputGraphMLFilePath)
{
    GraphLayoutErrorCodes::LayoutErrorCode enHierarchicalLayoutErrorCode = GraphLayoutErrorCodes::LAYOUT_SUCCESS;


    if( (sInputGraphMLFilePath.isEmpty() == true))
    {
        enHierarchicalLayoutErrorCode = GraphLayoutErrorCodes::INVALID_FILE_NAME;
    }

    if((sInputGraphMLFilePath.trimmed().endsWith(GRAPHML, Qt::CaseInsensitive) == false)
            && (enHierarchicalLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
    {
        enHierarchicalLayoutErrorCode = GraphLayoutErrorCodes::UNSUPPORTED_FILE_FORMAT;
    }

    if((QFile::exists(sInputGraphMLFilePath) == false) && (enHierarchicalLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
    {
        enHierarchicalLayoutErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
    }

    // create global graph pointer.
    SubGraph *gMainGraph;

    if(enHierarchicalLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        // read input GraphML file
        QFile inFile(sInputGraphMLFilePath);
        GraphMLReader reader;
        try
        {
            SubGraph &gInputGraph = reader.readGraphML(&inFile);
            gMainGraph = &(gInputGraph);
        }
        catch(LayoutFileIOException& eException)
        {
            //We return the file exception enums
            enHierarchicalLayoutErrorCode = (GraphLayoutErrorCodes::LayoutErrorCode)eException.getErrorCode();
        }
        catch(...)
        {

            enHierarchicalLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
        }
    }

    if(enHierarchicalLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        // apply hierarchical layout
        HierarchicalLayouter hierarchicalLayouter;
        try
        {
            hierarchicalLayouter.applyHierarchicalLayout(*gMainGraph);
        }
        catch(...)
        {
            DELETE_AND_SET_NULL(gMainGraph);

            enHierarchicalLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
        }
    }

    if(enHierarchicalLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        // write file to output path
        QFile outFile(sOutputGraphMLFilePath);
        if ((!outFile.open(QFile::WriteOnly | QFile::Truncate)))
        {
            enHierarchicalLayoutErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
        }

        if(enHierarchicalLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
        {
            GraphMLWriter writer;
            try
            {
                writer.writeGraphml(*gMainGraph , &outFile);
                outFile.flush();
                outFile.close();
            }
            catch(...)
            {
                DELETE_AND_SET_NULL(gMainGraph);
                enHierarchicalLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
            }
        }
    }
    return enHierarchicalLayoutErrorCode;
}

GraphLayoutErrorCodes::LayoutErrorCode LayoutManager::applyForceDirectedLayout(SubGraph *gMainInputGraph
                                                                               ,int iIterations)

{

    GraphLayoutErrorCodes::LayoutErrorCode enForceLayoutErrorCode = GraphLayoutErrorCodes::LAYOUT_SUCCESS;


    LAYOUT_ASSERT(iIterations != 0,LayoutException(__FUNCTION__
                                  ,LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE
                                  ,"ITERATIONS"
                                  ,"Iterations"));

    LAYOUT_ASSERT(iIterations < 5000,LayoutException(__FUNCTION__
                                  ,LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE
                                  ,"ITERATIONS"
                                  ,"Set Iteration less than 5000"));


    if(enForceLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {



        try{

       //     LAYOUT_ASSERT(num_vertices(*gMainInputGraph) != 0,LayoutException(__FUNCTION__
       //                                   ,ExceptionEnum::INVALID_ATTRIBUTE_VALUE
       //                                   ,"applyForceDirectedLayout"
       //                                   ,"Number of vertices"));


                MembershipInitializer *memInit = new MembershipInitializer();
                memInit->initializeMembers(*gMainInputGraph);

                Reingold *reingold = new Reingold();
             //   reingold->SetClusterCompartmentPropsNew(*gMainInputGraph);
                reingold->setCompartMentProps(*gMainInputGraph,10);

                ClusteredSpringEmbedder *clusteredSpring = new ClusteredSpringEmbedder();
                clusteredSpring->springForEachCluster(*gMainInputGraph,iIterations);
               // clusteredSpring->springTestRecursive(gMainInputGraph);


            //    reingold->SetClusterCompartmentPropsNew(*gMainInputGraph);
                reingold->setCompartMentProps(*gMainInputGraph,10);
            }

        catch(boost::exception &eBoostException)
        {

            throw *boost::get_error_info<errmsg_info>(eBoostException);
            enForceLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
        }
        catch(LayoutException &eLayoutException)
        {
            throw eLayoutException;
        }

   }

}

GraphLayoutErrorCodes::LayoutErrorCode LayoutManager::applyForceDirectedLayout(QString sInputGraphMLFilePath,
                                                   QString sOutputGraphMLFilePath,
                                                   int iIterations)

{

        GraphLayoutErrorCodes::LayoutErrorCode enForceLayoutErrorCode =
                               GraphLayoutErrorCodes::LAYOUT_SUCCESS;

          if( (sInputGraphMLFilePath.isEmpty() == true))
          {
              enForceLayoutErrorCode = GraphLayoutErrorCodes::INVALID_FILE_NAME;
          }

          if((sInputGraphMLFilePath.trimmed().endsWith(GRAPHML, Qt::CaseInsensitive) == false)
                  && (enForceLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
          {
              enForceLayoutErrorCode = GraphLayoutErrorCodes::UNSUPPORTED_FILE_FORMAT;
          }

          if((QFile::exists(sInputGraphMLFilePath) == false) && (enForceLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
          {
              enForceLayoutErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
          }

          // create global graph pointer.
          SubGraph *gMainGraph;

          if(enForceLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
          {
              // read input GraphML file
              QFile inFile(sInputGraphMLFilePath);
              GraphMLReader reader;
              try
              {
                  SubGraph &gInputGraph = reader.readGraphML(&inFile);
                  gMainGraph = &(gInputGraph);
                  inFile.close();
              }
              catch(LayoutFileIOException& eException)
              {
                  inFile.close();
                  //We return the file exception enums
                  enForceLayoutErrorCode = (GraphLayoutErrorCodes::LayoutErrorCode)eException.getErrorCode();
              }
          }

          if(enForceLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
          {
              // apply Force Based layout

                 applyForceDirectedLayout(gMainGraph,iIterations);

          }

          if(enForceLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
          {
              // write file to output path
              QFile outFile(sOutputGraphMLFilePath);
              if ((!outFile.open(QFile::WriteOnly | QFile::Truncate)))
              {
                  enForceLayoutErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
              }

              if(enForceLayoutErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
              {
                  GraphMLWriter writer;
                  try
                  {
                      writer.writeGraphml(*gMainGraph , &outFile);
                      outFile.flush();
                      outFile.close();
                  }
                  catch(...)
                  {
                      outFile.close();
                      DELETE_AND_SET_NULL(gMainGraph);
                      enForceLayoutErrorCode = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
                      outFile.close();
                  }
              }
          }

          //return gMainGraph;

          return enForceLayoutErrorCode;

}

GraphLayoutErrorCodes::LayoutErrorCode LayoutManager::applyVertexOverlapRemoval(QString sInputGraphMLFilePath,
                                                           QString sOutputGraphMLFilePath)

{

    GraphLayoutErrorCodes::LayoutErrorCode enVertexOVerlapErrorcode;

    VertexOverlapRemoval verOverlapRemoval;

    enVertexOVerlapErrorcode = verOverlapRemoval.removeOverlaps(sInputGraphMLFilePath
                                                                ,sOutputGraphMLFilePath);

    return enVertexOVerlapErrorcode;

}

void LayoutManager::setNumberOfIterations(int iterations)
{
    iForceDirectedIterations = iterations;
}


