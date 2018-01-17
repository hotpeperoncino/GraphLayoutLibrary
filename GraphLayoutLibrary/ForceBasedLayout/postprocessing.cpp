#include "Postprocessing.h"

Postprocessing::Postprocessing()
{
}


void Postprocessing::applyPostProcessing(SubGraph& gMaingraph )
{
    // place empty clusters somewhere default
    BoostGraphWrapper bGraphWrapper;
    VertexDescriptor verAffected;
    VertexIterPair   verIter;
    for(tie(verIter.first,verIter.second)=vertices(gMaingraph)
                ;verIter.first!=verIter.second;++verIter.first)
    {

         verAffected = *verIter.first;
    }
    int iMainTopY = 0 ,iMainTopX = 0 ;
    iMainTopX = bGraphWrapper.getGraphLeftTopCoordX(gMaingraph);
    iMainTopY = bGraphWrapper.getGraphLeftTopCoordY(gMaingraph);


    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&gMaingraph );
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;iterChild++)
        {
            SubGraph* gChildGraph = &(*iterChild);
            if(boost::num_vertices(*gChildGraph) == 0)
            {
                bGraphWrapper.setGraphLeftTopCoordX(100,*gChildGraph);
                bGraphWrapper.setGraphLeftTopCoordY(100,*gChildGraph);
                bGraphWrapper.setGraphHeight(100,*gChildGraph);
                bGraphWrapper.setGraphWidth(100,*gChildGraph);

            }
        }
    }
}




void Postprocessing::applyPreProcessing(SubGraph& gMaingraph )
{
    // add a dummy vertex to empty subgraph
    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&gMaingraph );
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;iterChild++)
        {
            SubGraph* gChildGraph = &(*iterChild);
            if(boost::num_vertices(*gChildGraph) == 0)
            {

                BoostGraphWrapper bGraphWrapper;
               // VertexDescriptor verDiscp = bGraphWrapper.addVertex(,*gChildGraph);
                VertexDescriptor verDiscp =   boost::add_vertex(*gChildGraph);
                int igraphId = bGraphWrapper.getGraphClusterID(*gChildGraph);
                bGraphWrapper.setVertexClusterID(igraphId,verDiscp,*gChildGraph);                
                bGraphWrapper.setVertexCenterCoordX(verDiscp,*gChildGraph,00 + 50);
                bGraphWrapper.setVertexCenterCoordY(verDiscp,*gChildGraph,00 + 50);
                bGraphWrapper.setVertexHeight(verDiscp,*gChildGraph,40);
                bGraphWrapper.setVertexWidth(verDiscp,*gChildGraph,40);
                bGraphWrapper.setVertexIsInvisible(verDiscp,*gChildGraph,true);
                Reingold rein;
                rein.setCompartMentProps(*gChildGraph,10);

            }
        }
    }
}


void Postprocessing::membershipFixer(SubGraph& gMaingraph)
{
    // checks if membership of any subgraph is voilated

    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&gMaingraph );
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;iterChild++)
        {
            SubGraph* gChildGraph = &(*iterChild);
            qSubgraphs.enqueue(gChildGraph); // added later
            checkAndFixMembership(*gChildGraph);

        }
    }

}



void Postprocessing::checkClusterOverlap(SubGraph& gMaingraph )
{

    // make it default **** Main method ****

    m_iRecursionIndex++;  // increment recursion index

    bool bClusterOverlap = false;
    BoostGraphWrapper bGraphWrapper;
    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&gMaingraph );
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;iterChild++)
        {
            SubGraph* gChildGraph = &(*iterChild);
            ChildrenIterator iterChildInner , iterChildEndInner;
            for(boost::tie(iterChildInner , iterChildEndInner) = gSubgraph1->children();
                iterChildInner != iterChildEndInner;iterChildInner++)
            {
                SubGraph* gGraph = &(*iterChildInner);

                if(gChildGraph == gGraph)
                {
                    continue;
                }
                double dVCenterX = bGraphWrapper.getGraphCenterCoordX(*gChildGraph);
                double dVCenterY = bGraphWrapper.getGraphCenterCoordY(*gChildGraph);
                double dVHeight =  bGraphWrapper.getGraphHeight(*gChildGraph);
                double dVWidth =   bGraphWrapper.getGraphWidth(*gChildGraph);
                double dVTopLeftX = bGraphWrapper.getGraphLeftTopCoordX(*gChildGraph);
                double dVTopLeftY = bGraphWrapper.getGraphLeftTopCoordY(*gChildGraph);
                double dUCenterX = bGraphWrapper.getGraphCenterCoordX(*gGraph);
                double dUCenterY = bGraphWrapper.getGraphCenterCoordY(*gGraph);
                double dUHeight = bGraphWrapper.getGraphHeight(*gGraph);
                double dUWidth = bGraphWrapper.getGraphWidth(*gGraph);
                double dUTopLeftX = bGraphWrapper.getGraphLeftTopCoordX(*gGraph);
                double dUTopLeftY = bGraphWrapper.getGraphLeftTopCoordY(*gGraph);

                if((dUTopLeftX - CLUSTER_OVERLAP_BOUNDRY) <= (dVTopLeftX - CLUSTER_OVERLAP_BOUNDRY) &&
                        (dUTopLeftX+ dUWidth+ CLUSTER_OVERLAP_BOUNDRY) > (dVTopLeftX- CLUSTER_OVERLAP_BOUNDRY))
                {
                    if((dUTopLeftY- CLUSTER_OVERLAP_BOUNDRY < dVTopLeftY- CLUSTER_OVERLAP_BOUNDRY ) &&
                            (dUTopLeftY + dUHeight+ CLUSTER_OVERLAP_BOUNDRY) >= (dVTopLeftY- CLUSTER_OVERLAP_BOUNDRY) )
                    {
                        // overlap case 1
                        bClusterOverlap = true;

                    }
                    if((dVTopLeftY-CLUSTER_OVERLAP_BOUNDRY < dUTopLeftY-CLUSTER_OVERLAP_BOUNDRY ) &&
                            (dVTopLeftY + dVHeight+ CLUSTER_OVERLAP_BOUNDRY) >= (dUTopLeftY-CLUSTER_OVERLAP_BOUNDRY) )
                    {
                        // overlap case 2
                        bClusterOverlap = true;

                    }
                }
                if((dVTopLeftX- CLUSTER_OVERLAP_BOUNDRY)  <= (dUTopLeftX - CLUSTER_OVERLAP_BOUNDRY) &&
                        (dVTopLeftX+ dVWidth + CLUSTER_OVERLAP_BOUNDRY) > (dUTopLeftX- CLUSTER_OVERLAP_BOUNDRY))
                {
                    if((dVTopLeftY- CLUSTER_OVERLAP_BOUNDRY) < (dUTopLeftY - CLUSTER_OVERLAP_BOUNDRY) &&
                            (dVTopLeftY + dVHeight+CLUSTER_OVERLAP_BOUNDRY) >= (dUTopLeftY- CLUSTER_OVERLAP_BOUNDRY) )
                    {
                        // overlap
                        bClusterOverlap = true;

                    }
                    if((dUTopLeftY- CLUSTER_OVERLAP_BOUNDRY) < (dVTopLeftY - CLUSTER_OVERLAP_BOUNDRY) &&
                            (dUTopLeftY + dUHeight+CLUSTER_OVERLAP_BOUNDRY) >= (dVTopLeftY- CLUSTER_OVERLAP_BOUNDRY) )
                    {
                        // overlap
                        bClusterOverlap = true;

                    }
                }
                if( bClusterOverlap)
                {
                    bClusterOverlap = false;
                    clusterOverlapRemoval(*gChildGraph,*gGraph);
                    // remove overlap
                }
            }
            qSubgraphs.enqueue(gChildGraph);


        }

    }

  while(m_iRecursionIndex < CLUSTER_OVERLAP_REMOVAL_PASSES )
    {
      checkClusterOverlap(gMaingraph);

    }



}

 void Postprocessing::clusterOverlapRemoval(SubGraph& gChildOne ,
                                               SubGraph& gChildSecond)
{

     // default cluster overlap removal code
    BoostGraphWrapper bGraphWrapper;

    double dVCenterX = bGraphWrapper.getGraphCenterCoordX(gChildOne);
    double dVCenterY = bGraphWrapper.getGraphCenterCoordY(gChildOne);
    double dVHeight =  bGraphWrapper.getGraphHeight(gChildOne);
    double dVWidth =   bGraphWrapper.getGraphWidth(gChildOne);
    double dVTopLeftX = bGraphWrapper.getGraphLeftTopCoordX(gChildOne);
    double dVTopLeftY = bGraphWrapper.getGraphLeftTopCoordY(gChildOne);
    double dUCenterX = bGraphWrapper.getGraphCenterCoordX(gChildSecond);
    double dUCenterY = bGraphWrapper.getGraphCenterCoordY(gChildSecond);
    double dUHeight = bGraphWrapper.getGraphHeight(gChildSecond);
    double dUWidth = bGraphWrapper.getGraphWidth(gChildSecond);
    double dUTopLeftX = bGraphWrapper.getGraphLeftTopCoordX(gChildSecond);
    double dUTopLeftY = bGraphWrapper.getGraphLeftTopCoordY(gChildSecond);

    double dIntersectionHorizontal = 0 , dIntersectionVertical = 0  ;
    VertexDescriptor verAffected;
    VertexIterPair   verIter;

    cout<<"\n \n Intersection Found";

    dIntersectionHorizontal = fabs(dUTopLeftX - (dVTopLeftX + dVWidth))*
               fabs(dUTopLeftX - (dVTopLeftX + dVWidth))
            +  (fabs(dUTopLeftY - (dVTopLeftY + dVWidth)) *
               fabs(dUTopLeftY - (dVTopLeftY + dVWidth))) ; // change y coordinate
    dIntersectionHorizontal = sqrt(dIntersectionHorizontal);


    if(dVTopLeftX < dUTopLeftX)
    {

            cout<<"\n \n Intersection = "<<dIntersectionHorizontal;

            for(tie(verIter.first,verIter.second)=vertices(gChildOne)
                        ;verIter.first!=verIter.second;++verIter.first)
            {

                 verAffected = *verIter.first;
                 int iCoordX = bGraphWrapper.getVertexCenterCoordX(verAffected,gChildOne);
                 //iCoordX -= (dIntersectionHorizontal/2 + 150);      //100*(-dYDisp/fabs(dYDisp));
                 bGraphWrapper.setVertexCenterCoordX(verAffected,gChildOne,iCoordX);
            }
            for(tie(verIter.first,verIter.second)=vertices(gChildSecond)
                        ;verIter.first!=verIter.second;++verIter.first)
            {

                 verAffected = *verIter.first;
                 int iCoordX = bGraphWrapper.getVertexCenterCoordX(verAffected,gChildSecond);
                 iCoordX += (dIntersectionHorizontal/2 + 150);      //100*(-dYDisp/fabs(dYDisp));
                 bGraphWrapper.setVertexCenterCoordX(verAffected,gChildSecond,iCoordX);
            }
    }
    else
    {

        for(tie(verIter.first,verIter.second)=vertices(gChildOne)
                    ;verIter.first!=verIter.second;++verIter.first)
        {

             verAffected = *verIter.first;
             int iCoordX = bGraphWrapper.getVertexCenterCoordX(verAffected,gChildOne);
             iCoordX += (dIntersectionHorizontal/2 + 150);      //100*(-dYDisp/fabs(dYDisp));
             bGraphWrapper.setVertexCenterCoordX(verAffected,gChildOne,iCoordX);
        }
        for(tie(verIter.first,verIter.second)=vertices(gChildSecond)
                    ;verIter.first!=verIter.second;++verIter.first)
        {

             verAffected = *verIter.first;
             int iCoordX = bGraphWrapper.getVertexCenterCoordX(verAffected,gChildSecond);
            // iCoordX -= (dIntersectionHorizontal/2 + 150);      //100*(-dYDisp/fabs(dYDisp));
             bGraphWrapper.setVertexCenterCoordX(verAffected,gChildSecond,iCoordX);
        }

    }
    Reingold rein ;
    rein.SetClusterCompartmentPropsNew(gChildOne);
    rein.SetClusterCompartmentPropsNew(gChildSecond);

}





void Postprocessing::checkAndFixMembership(SubGraph& gSubgraph)
{
    BoostGraphWrapper bGraphWrapper;
    VertexDescriptor verAffected;
    VertexIterPair   verIter;
    int iSubGraphId = bGraphWrapper.getGraphClusterID(gSubgraph);
    SpringEmbedder *spl = new SpringEmbedder();
    QVector<int> vRestBound;
    QVector<SubGraph *>  vClusters;
    spl->getRestrictedBounds(gSubgraph,vRestBound,vClusters);
    for(tie(verIter.first,verIter.second)=vertices(gSubgraph)
                ;verIter.first!= verIter.second;++verIter.first)
    {

         verAffected = *verIter.first;
         int iVertexId = bGraphWrapper.getVertexClusterID(verAffected,gSubgraph);
         if(iVertexId != iSubGraphId)
         {
             continue;
         }
         int iCoordX = bGraphWrapper.getVertexCenterCoordX(verAffected,gSubgraph);
         int iCoordY = bGraphWrapper.getVertexCenterCoordY(verAffected,gSubgraph);
         int iVertexLeftTopX = bGraphWrapper.getVertexLeftCoordX(verAffected,gSubgraph);
         int iVertexLeftTopY = bGraphWrapper.getVertexLeftCoordY(verAffected,gSubgraph);
         int iRestrictedAreas = vRestBound.size();
         if(iRestrictedAreas !=0)
         {
            iRestrictedAreas = iRestrictedAreas/6;
         }
         int iIndex = 0;
         for(int i = 0;i< iRestrictedAreas ; i++)
         {
             int iHeight = vRestBound.at(iIndex+2);
             int iWidth = vRestBound.at(iIndex+3);
             int iLeftTopX = vRestBound.at(iIndex+4);
             int iLeftTopY = vRestBound.at(iIndex+5);
             bool bVoilated = false;
             // orignally 50 instead of 80
             if(iCoordX >= iLeftTopX - 80 && iCoordX <= iLeftTopX+iWidth+80   )
             {
                 if(iCoordY >= iLeftTopY- 80 && iCoordY <= iLeftTopY + iHeight + 80  )
                 {
                    bVoilated = true;
                 }
             }

             if(bVoilated)
             {
                cout<<"\n\n ********* Voilated **************";
                int iDxLeft = 0 , iDyTop = 0, iDxRight =0 , iDyBottom = 0 ;
                iDxLeft = sqrt((iCoordX - iLeftTopX)*(iCoordX - iLeftTopX));
                iDxRight = sqrt((iCoordX - (iLeftTopX + iWidth))*
                                 (iCoordX - (iLeftTopX + iWidth)));

                iDyTop = sqrt((iCoordY - iLeftTopY)*(iCoordY - iLeftTopY));
                iDyBottom = sqrt((iCoordY - (iLeftTopY + iHeight))*
                                (iCoordY - (iLeftTopY + iHeight)));
                // working formulae

//                if(abs(iDxLeft) > abs(iDxRight) )
//                {
//                    iCoordX = iLeftTopX + iWidth + CORRECTED_DISTANCE +
//                              std::rand()%CORRECTED_DISTANCE;
//                }
//                else
//                {
//                     iCoordX = iLeftTopX -  CORRECTED_DISTANCE -
//                             std::rand()%CORRECTED_DISTANCE;
//                }
//                {
//                    if(abs(iDyTop) > abs(iDyBottom) )
//                    {
//                        iCoordY = iLeftTopY + iHeight + CORRECTED_DISTANCE +
//                                std::rand()%CORRECTED_DISTANCE;
//                    }
//                    else
//                    {
//                        iCoordY = iLeftTopY - CORRECTED_DISTANCE -
//                                std::rand()%CORRECTED_DISTANCE;
//                    }
//                }

               // trying new for time being


                int iSmallest = std::min(iDxLeft,std::min(iDxRight,std::min(iDyTop,iDyBottom)));

                if( iDxLeft == iSmallest)
                {
                    iCoordX = iCoordX - iSmallest - CORRECTED_DISTANCE - std::rand()%CORRECTED_DISTANCE;
                }

                if(iDxRight == iSmallest)
                {
                    iCoordX = iCoordX + iSmallest + CORRECTED_DISTANCE + std::rand()%CORRECTED_DISTANCE;

                }


                if(iDyBottom == iSmallest)
                {
                    iCoordY = iCoordY + iSmallest + CORRECTED_DISTANCE + std::rand()%CORRECTED_DISTANCE;
                }

                if(iDyTop == iSmallest)
                {
                     iCoordY = iCoordY - iSmallest - CORRECTED_DISTANCE - std::rand()%CORRECTED_DISTANCE;
                }




                bGraphWrapper.setVertexCenterCoordX(verAffected,gSubgraph,iCoordX);
                bGraphWrapper.setVertexCenterCoordY(verAffected,gSubgraph,iCoordY);
             }
         }
    }
}

void Postprocessing::overlapRemoval(SubGraph& gMaingraph )
{
    // save vertices and x y cordinates ,
    // sort on x
    // look for min separation of width/2
    // if not min separation

    m_iVertexOverlapConter++;

    VertexDescriptor verAffected,verVisitor;
    VertexIterPair   verIter,verIterEnd;
    BoostGraphWrapper bGraphWrapper;
    for(tie(verIter.first,verIter.second)=vertices(gMaingraph)
                ;verIter.first!=verIter.second;++verIter.first)
    {

        verAffected = *verIter.first;

        double X_Coord_V = bGraphWrapper.getVertexCenterCoordX(verAffected,gMaingraph);
        double Y_Coord_V = bGraphWrapper.getVertexCenterCoordY(verAffected,gMaingraph);

        for(tie(verIterEnd.first,verIterEnd.second)=vertices(gMaingraph);
                       verIterEnd.first != verIterEnd.second;++verIterEnd.first)
        {

           verVisitor = *verIterEnd.first;
           double X_Coord_U = bGraphWrapper.getVertexCenterCoordX(verVisitor,gMaingraph);
           double Y_Coord_U = bGraphWrapper.getVertexCenterCoordY(verVisitor,gMaingraph);

                if(verAffected == verVisitor)
               {
                    continue;

               }

                // Logic for vertex size here

                double d_Width_U = bGraphWrapper.getVertexWidth(verVisitor,gMaingraph);
                double d_Height_U = bGraphWrapper.getVertexHeight(verVisitor,gMaingraph);
                double d_Width_V = bGraphWrapper.getVertexWidth(verAffected,gMaingraph);
                double d_Height_V = bGraphWrapper.getVertexHeight(verAffected,gMaingraph );

                double dSizeU = std::max(d_Width_U,0.0);
                double dSizeV = std:: max(d_Width_V,0.0);

                double d_LeftTop_X_U = X_Coord_U - d_Width_U/2;
                double d_LeftTop_X_V = X_Coord_V - d_Width_V/2;
                double d_LeftTop_Y_U = Y_Coord_U - d_Height_U/2;
                double d_LeftTop_Y_V = Y_Coord_V - d_Height_U/2;





                       double vx = X_Coord_V - X_Coord_U;
                       double vy = Y_Coord_V - Y_Coord_U;
                       double dDist = fabs((vx*vx + vy* vy));
                       dDist = sqrt(dDist);

                    cout<<"\nDistance at Overlap removal :  "<<dDist;

                   if( (d_LeftTop_X_V > d_LeftTop_X_U && d_LeftTop_X_V < d_LeftTop_X_U+ d_Width_U)||
                        (d_LeftTop_X_V < d_LeftTop_X_U && d_LeftTop_X_V+ d_Width_V > d_LeftTop_X_U))                 //dDist == 0 || dDist < 60)
                    {
                       // x intersept

                        if((d_LeftTop_Y_V > d_LeftTop_Y_U && d_LeftTop_Y_U + d_Height_U > d_LeftTop_Y_V)
                        || (d_LeftTop_Y_U > d_LeftTop_Y_V && d_LeftTop_Y_V + d_Height_V > d_LeftTop_Y_U)  )                      //(X_Coord_U - dSizeU/2 > X_Coord_V - dSizeV/2 )   // replace with overlap condition
                        {


                            if(Y_Coord_U < Y_Coord_V)        // WORKING LOGIC
                            {
                                X_Coord_U = X_Coord_U + std::rand()%20+ CORRECTED_DISTANCE/2 +10  ;    //+100;
                                Y_Coord_U = Y_Coord_U + std::rand()%20- CORRECTED_DISTANCE/4  ;
                                bGraphWrapper.setVertexCenterCoordX(verVisitor,gMaingraph,X_Coord_U);
                                bGraphWrapper.setVertexCenterCoordY(verVisitor,gMaingraph,Y_Coord_U);
                            }
                            else
                            {
                                X_Coord_U = X_Coord_U + std::rand()%20+ CORRECTED_DISTANCE/2 +10  ;    //+100;
                                Y_Coord_U = Y_Coord_U + std::rand()%20+ CORRECTED_DISTANCE/4;
                                bGraphWrapper.setVertexCenterCoordX(verVisitor,gMaingraph,X_Coord_U);
                                bGraphWrapper.setVertexCenterCoordY(verVisitor,gMaingraph,Y_Coord_U);

                            }


//                            double dIntersectionHorizontal = 0;
//                           if(d_LeftTop_X_U < d_LeftTop_X_V)
//                           {
//                             dIntersectionHorizontal = (d_LeftTop_X_V - (d_LeftTop_X_U + d_Width_U))
//                                                       *(d_LeftTop_X_V - (d_LeftTop_X_U + d_Width_U));

//                             dIntersectionHorizontal = sqrt(dIntersectionHorizontal);

//                             X_Coord_U = X_Coord_U + std::rand()%30+ dIntersectionHorizontal;
//                             bGraphWrapper.setVertexCenterCoordX(verVisitor,gMaingraph,X_Coord_U);




//                           }
//                           else
//                           {
//                               dIntersectionHorizontal = (d_LeftTop_X_U - (d_LeftTop_X_V + d_Width_V))
//                                                         *(d_LeftTop_X_U - (d_LeftTop_X_V + d_Width_V));

//                               dIntersectionHorizontal = sqrt(dIntersectionHorizontal);

//                               X_Coord_V = X_Coord_V + std::rand()%30+ dIntersectionHorizontal;
//                               bGraphWrapper.setVertexCenterCoordX(verVisitor,gMaingraph,X_Coord_V);


//                           }


                        }
//                        else
//                        {
//                            if(Y_Coord_V < Y_Coord_U)
//                          {
//                            X_Coord_V = X_Coord_V + std::rand()%20+ CORRECTED_DISTANCE/2;    //100;
//                            Y_Coord_V = Y_Coord_V + std::rand()%20- CORRECTED_DISTANCE/4;
//                            bGraphWrapper.setVertexCenterCoordX(verAffected,gMaingraph,X_Coord_V);
//                            bGraphWrapper.setVertexCenterCoordY(verAffected,gMaingraph,Y_Coord_V);
//                          }
//                            else
//                            {
//                                X_Coord_V = X_Coord_V + std::rand()%20 + CORRECTED_DISTANCE/2;    //100;
//                                Y_Coord_V = Y_Coord_V + std::rand()%20 + CORRECTED_DISTANCE/4;
//                                bGraphWrapper.setVertexCenterCoordX(verAffected,gMaingraph,X_Coord_V);
//                                bGraphWrapper.setVertexCenterCoordY(verAffected,gMaingraph,Y_Coord_V);
//                            }
//                        }
                    }



        }

    }


    if(m_iVertexOverlapConter < 25)       //VERTEX_OVERLAP_REMOVAL_PASSES)
    {
        overlapRemoval(gMaingraph);
    }



}











