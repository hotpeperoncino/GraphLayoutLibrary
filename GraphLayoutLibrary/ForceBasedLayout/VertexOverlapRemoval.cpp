#include "VertexOverlapRemoval.h"

VertexOverlapRemoval::VertexOverlapRemoval()
{
}

void VertexOverlapRemoval::repelOverlappingVerticesWithOther(SubGraph& gMaingraph)
{
    cout<<"\n \n *********In Overlap Removal ******** \n \n";

    do
   {
    QVector<VertexIterator> vOverLappingVertices = getOverlappingVertices(gMaingraph);
    m_iVertexOverlapConter++;
    cout<<"\n Counter "<<m_iVertexOverlapConter;
    cout<<"\n Overlapping Vertices are : "<< vOverLappingVertices.size();
    cout<<"\n \n \n";   
    BoostGraphWrapper bGraphWrapper;

    if(!vOverLappingVertices.size() == 0)
    {
        for(int index =0 ; index < vOverLappingVertices.size();index++)
        {
            for(int innerIndex = 0 ; innerIndex < vOverLappingVertices.size(); innerIndex++)
            {
                if(innerIndex == index)
                {
                    continue;
                }

                VertexIterator vVertexOuterIterator = vOverLappingVertices.at(index);
                VertexIterator  vVertexInnerIterator = vOverLappingVertices.at(innerIndex);
                VertexDescriptor vVertexOuter = *vVertexOuterIterator;
                VertexDescriptor vVertexInner = *vVertexInnerIterator;

                if(vVertexInner == vVertexOuter)
                {
                    continue;
                }

                if(bIsOverlappingVertices(vVertexOuterIterator,vVertexInnerIterator,gMaingraph))
                {

                    double dXCoordV = bGraphWrapper.getVertexCenterCoordX(vVertexOuter,gMaingraph);
                    double dYCoordV = bGraphWrapper.getVertexCenterCoordY(vVertexOuter,gMaingraph);

                    double dXCoordU = bGraphWrapper.getVertexCenterCoordX(vVertexInner,gMaingraph);
                    double dYCoordU = bGraphWrapper.getVertexCenterCoordY(vVertexInner,gMaingraph);

                    if(dXCoordV < dXCoordU)

                {
                    if(dYCoordU < dYCoordV)        // WORKING LOGIC
                    {
                        dXCoordU = dXCoordU + std::rand()%20+ CORRECTED_DISTANCE/2 +10  ;    //+100;
                        dYCoordU = dYCoordU + std::rand()%20- CORRECTED_DISTANCE/4  ;
                        bGraphWrapper.setVertexCenterCoordX(vVertexInner,gMaingraph,dXCoordU);
                        bGraphWrapper.setVertexCenterCoordY(vVertexInner,gMaingraph,dYCoordU);
                    }
                    else
                    {
                        dXCoordU = dXCoordU + std::rand()%20+ CORRECTED_DISTANCE/2 +10  ;    //+100;
                        dYCoordU = dYCoordU + std::rand()%20+ CORRECTED_DISTANCE/4;
                        bGraphWrapper.setVertexCenterCoordX(vVertexInner,gMaingraph,dXCoordU);
                        bGraphWrapper.setVertexCenterCoordY(vVertexInner,gMaingraph,dYCoordU);

                    }
                }
                    else
                    {
                        if(dYCoordU < dYCoordV)        // WORKING LOGIC
                        {
                            dXCoordV = dXCoordV + std::rand()%20+ CORRECTED_DISTANCE/2 +10  ;    //+100;
                            dYCoordV = dYCoordV + std::rand()%20- CORRECTED_DISTANCE/4  ;
                            bGraphWrapper.setVertexCenterCoordX(vVertexOuter,gMaingraph,dXCoordU);
                            bGraphWrapper.setVertexCenterCoordY(vVertexOuter,gMaingraph,dYCoordU);
                        }
                        else
                        {
                            dXCoordV = dXCoordV + std::rand()%20+ CORRECTED_DISTANCE/2 +10  ;    //+100;
                            dYCoordV = dYCoordV + std::rand()%20+ CORRECTED_DISTANCE/4;
                            bGraphWrapper.setVertexCenterCoordX(vVertexOuter,gMaingraph,dXCoordU);
                            bGraphWrapper.setVertexCenterCoordY(vVertexOuter,gMaingraph,dYCoordU);

                        }

                    }

                   // bGraphWrapper.setVertexCenterCoordX(vVertexOuter,gMaingraph,(int)dXCoordV);
                   // bGraphWrapper.setVertexCenterCoordY(vVertexOuter,gMaingraph,(int)dYCoordV);


                }

            }
        }
    }
  }
    while(bOverlapExist(gMaingraph)&& m_iVertexOverlapConter < 100);   // while overlap

}


QVector<VertexIterator> VertexOverlapRemoval::getOverlappingVertices(SubGraph& gMaingraph)
{

     QVector<VertexIterator> vOverLappingVertices;
     VertexDescriptor verAffected,verVisitor;
     VertexIterPair   verIter,verIterEnd;
     BoostGraphWrapper bGraphWrapper;
     for(tie(verIter.first,verIter.second)=vertices(gMaingraph)
                 ;verIter.first!=verIter.second;++verIter.first)
     {

         verAffected = *verIter.first;

         double dXCoordV = bGraphWrapper.getVertexCenterCoordX(verAffected,gMaingraph);
         double dYCoordV = bGraphWrapper.getVertexCenterCoordY(verAffected,gMaingraph);

         for(tie(verIterEnd.first,verIterEnd.second)=vertices(gMaingraph);
                        verIterEnd.first != verIterEnd.second;++verIterEnd.first)
         {

            verVisitor = *verIterEnd.first;
            double dXCoordU = bGraphWrapper.getVertexCenterCoordX(verVisitor,gMaingraph);
            double dYCoordU = bGraphWrapper.getVertexCenterCoordY(verVisitor,gMaingraph);

                 if(verAffected == verVisitor)
                {
                     continue;

                }

                 // Logic for vertex size here

                 double dWidthU = bGraphWrapper.getVertexWidth(verVisitor,gMaingraph);
                 double dHeightU = bGraphWrapper.getVertexHeight(verVisitor,gMaingraph);
                 double dWidthV = bGraphWrapper.getVertexWidth(verAffected,gMaingraph);
                 double dHeightV = bGraphWrapper.getVertexHeight(verAffected,gMaingraph );


                 double dXLeftTopU = dXCoordU - dWidthU/2 -5;
                 double dXLeftTopV = dXCoordV - dWidthV/2 -5;
                 double dYLeftTopU = dYCoordU - dHeightU/2 -5;
                 double dYLeftTopV = dYCoordV - dHeightV/2 -5;



                    if( (dXLeftTopV >= dXLeftTopU && dXLeftTopV <= dXLeftTopU+ dWidthU)||
                         (dXLeftTopV <= dXLeftTopU && dXLeftTopV+ dWidthV >= dXLeftTopU))
                     {
                        // x intercept

                         if((dYLeftTopV >= dYLeftTopU && dYLeftTopU + dHeightU >= dYLeftTopV)
                         || (dYLeftTopU >= dYLeftTopV && dYLeftTopV + dHeightV >= dYLeftTopU)  )
                         {
                             // check if already added

                             if(!vOverLappingVertices.contains(verIterEnd.first))
                             {
                                 vOverLappingVertices.push_back(verIterEnd.first);
                             }

                             if(!vOverLappingVertices.contains(verIter.first))
                             {
                                 vOverLappingVertices.push_back(verIter.first);

                             }


                         }


                    }
         }
     }


     return vOverLappingVertices;
}

 GraphLayoutErrorCodes::LayoutErrorCode VertexOverlapRemoval::removeOverlaps(QString sInputGraphMLFilePath,
                                          QString sOutputGraphMLFilePath)    //SubGraph& gMaingraph)
{

    GraphLayoutErrorCodes::LayoutErrorCode enVertexOverlapErrorCode =
                           GraphLayoutErrorCodes::LAYOUT_SUCCESS;

      if( (sInputGraphMLFilePath.isEmpty() == true))
      {
          enVertexOverlapErrorCode = GraphLayoutErrorCodes::INVALID_FILE_NAME;
      }

      if((sInputGraphMLFilePath.trimmed().endsWith(GRAPHML, Qt::CaseInsensitive) == false)
              && (enVertexOverlapErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
      {
          enVertexOverlapErrorCode = GraphLayoutErrorCodes::UNSUPPORTED_FILE_FORMAT;
      }

      if((QFile::exists(sInputGraphMLFilePath) == false) && (enVertexOverlapErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS))
      {
          enVertexOverlapErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
      }

      // create global graph pointer.

      SubGraph *gMaingraph = NULL ;


      if(enVertexOverlapErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
      {
          // read input GraphML file
          QFile inFile(sInputGraphMLFilePath);
          GraphMLReader reader;
          try
          {
              SubGraph &gInputGraph = reader.readGraphML(&inFile);
              gMaingraph = &(gInputGraph);
              inFile.close();
          }
          catch(LayoutFileIOException& eException)
          {
              inFile.close();
              //We return the file exception enums
              enVertexOverlapErrorCode = (GraphLayoutErrorCodes::LayoutErrorCode)eException.getErrorCode();
          }
      }

    cout<<"\n \n *********In Overlap Removal ******** \n \n";

    if(enVertexOverlapErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {


    do
   {
    QVector<VertexIterator> vOverLappingVertices = getOverlappingVertices(*gMaingraph);
    m_iVertexOverlapConter++;
    cout<<"\n Counter "<<m_iVertexOverlapConter;
    cout<<"\n Overlapping Vertices are : "<< vOverLappingVertices.size();
    cout<<"\n \n \n";
    VertexDescriptor  verOther;
    VertexIterPair    verIterInner;
    BoostGraphWrapper bGraphWrapper;

    if(!vOverLappingVertices.size() == 0)
    {
        for(int index =0 ; index < vOverLappingVertices.size();index++)
        {
            for(int innerIndex = 0 ; innerIndex < vOverLappingVertices.size(); innerIndex++)
            {
                if(innerIndex == index)
                {
                    continue;
                }

                VertexIterator vVertexOuterIterator = vOverLappingVertices.at(index);
                VertexIterator  vVertexInnerIterator = vOverLappingVertices.at(innerIndex);
                VertexDescriptor vVertexOuter = *vVertexOuterIterator;
                VertexDescriptor vVertexInner = *vVertexInnerIterator;

                if(vVertexInner == vVertexOuter)
                {
                    continue;
                }

                if(bIsOverlappingVertices(vVertexOuterIterator,vVertexInnerIterator,*gMaingraph))
                {
                    // define a moement vector
                    int iMovementVectorX = 0;
                    int iMovementVectorY = 0;

                    double dXCoordV = bGraphWrapper.getVertexCenterCoordX(vVertexOuter,*gMaingraph);
                    double dYCoordV = bGraphWrapper.getVertexCenterCoordY(vVertexOuter,*gMaingraph);

                    double dXCoordU = bGraphWrapper.getVertexCenterCoordX(vVertexInner,*gMaingraph);
                    double dYCoordU = bGraphWrapper.getVertexCenterCoordY(vVertexInner,*gMaingraph);


                    iMovementVectorX = dXCoordV - dXCoordU;
                    iMovementVectorY = dYCoordV - dYCoordU;

                    double dDistance = iMovementVectorX*iMovementVectorX +
                                       iMovementVectorY*iMovementVectorY;

                    dDistance = sqrt(dDistance);
                    if(dDistance == 0)
                    {
                        dDistance = 0.001; //avoid nan
                    }

                    iMovementVectorX = 200*iMovementVectorX/dDistance;
                    iMovementVectorY = 200*iMovementVectorY/dDistance;
                    //


                    for(tie(verIterInner.first,verIterInner.second)=vertices(*gMaingraph);
                         verIterInner.first != verIterInner.second;++verIterInner.first)
                    {
                        // repel with all other vertices
                        verOther = *verIterInner.first;

                        double dXCoordOther = bGraphWrapper.getVertexCenterCoordX(verOther,*gMaingraph);
                        double dYCoordOther = bGraphWrapper.getVertexCenterCoordY(verOther,*gMaingraph);

                        double dDeltaX = dXCoordV - dXCoordOther;
                        double dDeltaY = dYCoordV - dYCoordOther;

                        double dDistanceOther = dDeltaX*dDeltaX +
                                           dDeltaY*dDeltaY;
                       if(dDistanceOther == 0)
                        {
                            dDistanceOther = 0.001;
                        }


                       iMovementVectorX += 20*dDeltaX/dDistanceOther;
                       iMovementVectorY += 20*dDeltaY/dDistanceOther;

                   }


                    //
                    dXCoordV += 1*iMovementVectorX;
                    dYCoordV += 1*iMovementVectorY;
                    // move
                    bGraphWrapper.setVertexCenterCoordX(vVertexOuter,*gMaingraph,(int)dXCoordV);
                    bGraphWrapper.setVertexCenterCoordY(vVertexOuter,*gMaingraph,(int)dYCoordV);


                }

            }
        }
    }
  }
    while(bOverlapExist(*gMaingraph)&& m_iVertexOverlapConter < 5000);
    // while overlap

    Reingold resetClusterProps;
    resetClusterProps.setCompartMentProps(*gMaingraph,10);


    }


    if(enVertexOverlapErrorCode  == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
    {
        // write file to output path
        QFile outFile(sOutputGraphMLFilePath);
        if ((!outFile.open(QFile::WriteOnly | QFile::Truncate)))
        {
            enVertexOverlapErrorCode = GraphLayoutErrorCodes::FILE_NOT_FOUND;
        }

        if(enVertexOverlapErrorCode == GraphLayoutErrorCodes::LAYOUT_SUCCESS)
        {
            GraphMLWriter writer;
            try
            {
                writer.writeGraphml(*gMaingraph , &outFile);
                outFile.flush();
                outFile.close();
            }
            catch(...)
            {
                outFile.close();
                DELETE_AND_SET_NULL(gMaingraph);
                enVertexOverlapErrorCode  = GraphLayoutErrorCodes::UNKNOWN_LAYOUT_EXCEPTION;
                outFile.close();
            }
        }
    }

    Postprocessing postProcess;             //Remove clusters if there are
    postProcess.checkClusterOverlap(*gMaingraph);

    return enVertexOverlapErrorCode;

}

bool VertexOverlapRemoval::bIsOverlappingVertices(VertexIterator  vVertexFirstIterator
                                                 ,VertexIterator vVertexSecondIterator
                                                 ,SubGraph& gMaingraph)

{
    // return true if vertices overlap

    bool bOverlap = false;
    BoostGraphWrapper bGraphWrapper;

    VertexDescriptor vVertexFirst = *vVertexFirstIterator;
    VertexDescriptor vVertexSecond = *vVertexSecondIterator;

    double dXCoordV = bGraphWrapper.getVertexCenterCoordX(vVertexFirst,gMaingraph);
    double dYCoordV = bGraphWrapper.getVertexCenterCoordY(vVertexFirst,gMaingraph);

    double dXCoordU = bGraphWrapper.getVertexCenterCoordX(vVertexSecond,gMaingraph);
    double dYCoordU = bGraphWrapper.getVertexCenterCoordY(vVertexSecond,gMaingraph);

    double dWidthU = bGraphWrapper.getVertexWidth(vVertexSecond,gMaingraph);
    double dHeightU = bGraphWrapper.getVertexHeight(vVertexSecond,gMaingraph);
    double dWidthV = bGraphWrapper.getVertexWidth(vVertexFirst,gMaingraph);
    double dHeightV = bGraphWrapper.getVertexHeight(vVertexFirst,gMaingraph );


    double dXLeftTopU = dXCoordU - dWidthU/2 - 5;
    double dXLeftTopV = dXCoordV - dWidthV/2 - 5;
    double dYLeftTopU = dYCoordU - dHeightU/2 -5;
    double dYLeftTopV = dYCoordV - dHeightV/2 - 5;



       if( (dXLeftTopV >= dXLeftTopU && dXLeftTopV <= dXLeftTopU+ dWidthU)||
            (dXLeftTopV <= dXLeftTopU && dXLeftTopV+ dWidthV >= dXLeftTopU))
        {
           // x intercept

            if((dYLeftTopV >= dYLeftTopU && dYLeftTopU + dHeightU >= dYLeftTopV)
            || (dYLeftTopU >= dYLeftTopV && dYLeftTopV + dHeightV >= dYLeftTopU))
            {
                bOverlap = true;

            }


         }

       return bOverlap;

 }


bool VertexOverlapRemoval::bOverlapExist(SubGraph& gMaingraph)
{
    // returns true if graph has overlappng vertices.
    bool bOverlap = false;
    VertexDescriptor verAffected,verVisitor;
    VertexIterPair   verIter,verIterEnd;
    BoostGraphWrapper bGraphWrapper;
    for(tie(verIter.first,verIter.second)=vertices(gMaingraph)
                ;verIter.first!=verIter.second;++verIter.first)
    {

        verAffected = *verIter.first;

        double dXCoordV = bGraphWrapper.getVertexCenterCoordX(verAffected,gMaingraph);
        double dYCoordV = bGraphWrapper.getVertexCenterCoordY(verAffected,gMaingraph);

        for(tie(verIterEnd.first,verIterEnd.second)=vertices(gMaingraph);
                       verIterEnd.first != verIterEnd.second;++verIterEnd.first)
        {

           verVisitor = *verIterEnd.first;
           double dXCoordU = bGraphWrapper.getVertexCenterCoordX(verVisitor,gMaingraph);
           double dYCoordU = bGraphWrapper.getVertexCenterCoordY(verVisitor,gMaingraph);

                if(verAffected == verVisitor)
               {
                    continue;

               }

                // Logic for vertex size here

                double dWidthU = bGraphWrapper.getVertexWidth(verVisitor,gMaingraph);
                double dHeightU = bGraphWrapper.getVertexHeight(verVisitor,gMaingraph);
                double dWidthV = bGraphWrapper.getVertexWidth(verAffected,gMaingraph);
                double dHeightV = bGraphWrapper.getVertexHeight(verAffected,gMaingraph );


                double dXLeftTopU = dXCoordU - dWidthU/2 - 5;
                double dXLeftTopV = dXCoordV - dWidthV/2 - 5;
                double dYLeftTopU = dYCoordU - dHeightU/2 - 5;
                double dYLeftTopV = dYCoordV - dHeightV/2 -5;



                   if( (dXLeftTopV >= dXLeftTopU && dXLeftTopV <= dXLeftTopU+ dWidthU)||
                        (dXLeftTopV <= dXLeftTopU && dXLeftTopV+ dWidthV >= dXLeftTopU))
                    {
                       // x intercept

                        if((dYLeftTopV >= dYLeftTopU && dYLeftTopU + dHeightU >= dYLeftTopV)
                        || (dYLeftTopU >= dYLeftTopV && dYLeftTopV + dHeightV >= dYLeftTopU)  )
                        {
                           return true;

                        }
                   }
        }
    }


    return bOverlap;
}





















