#include "Reingold.h"




Reingold::Reingold()
{
}

void Reingold :: ApplyLocalFR(SubGraph &l_graph)
{

  //  setCompartMentProps(l_graph);
    int clustercounter =0;
    int iTotalClusters =0;
    QQueue<SubGraph*> qSubgraphs;   // a queue of subgraphs
    qSubgraphs.enqueue(&l_graph);
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        iTotalClusters++;
        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;
            iterChild++)
        {
            clustercounter++;
            SubGraph* gChildGraph = &(*iterChild);
            qSubgraphs.enqueue(gChildGraph);
            SpringEmbedder *spe = new SpringEmbedder();
            spe->SpringEmbedderStep(*gChildGraph);



        }
    }

    if(clustercounter ==0)
    {
      //  ApplyReingold(l_graph);
    }
     // uncomment it later


}







void Reingold :: ApplyReingold(SubGraph &l_graph)
{

    // Actual working FR algo ....
    cout<<"Before Wrapper";
    BoostGraphWrapper b_gwrapper;
    int m_igHeight = b_gwrapper.getGraphHeight(l_graph);
    int m_igWidth = b_gwrapper.getGraphWidth(l_graph);

    cout<<"\nHeight "<<m_igHeight;
    cout<<"\nWidth  "<<m_igWidth;


    int m_igArea = m_igHeight*m_igWidth;
    int numVert = b_gwrapper.numVertices(l_graph);

    int inter = m_igArea/numVert;
    double temperature = m_igWidth/10;

    iK = sqrt(inter)*0.75 ;  // constant default is 0.75

    int iterations = 200;   // iterations = 50
    int counter = 0;



    double m_iXcordaffected ;
    double m_iYcordaffected ;
    double m_iXcordvisit ;
    double m_iYcordvisit ;
    double m_iDist;



    double iXdisp =0;
    double iYdisp =0;

    double iTotalXDisp ;
    double iTotalYDisp ;


    double iMinX = 0 , iMinY = 0, iMaxX =0, iMaxY =0;

    iMinX = b_gwrapper.getGraphLeftTopCoordX(l_graph);
    iMinY = b_gwrapper.getGraphLeftTopCoordY(l_graph);
    iMaxX = iMinX + m_igWidth;
    iMaxY = iMinY + m_igHeight;



    int maxDimension = std::max(m_igWidth,m_igHeight);  // set max later on





    VertexIterPair verIter,verIterEnd;
    VertexDescriptor verAffected,verVisitor;
    EdgeBoolPair edgebool;

    while( temperature != 1/maxDimension || iterations != 0 )
    {
        for(tie(verIter.first,verIter.second)=vertices(l_graph);verIter.first!=verIter.second;++verIter.first)
         {


           verAffected = *verIter.first;
           b_gwrapper.setVertXDisp(0,verAffected,l_graph);
           b_gwrapper.setVertYDisp(0,verAffected,l_graph);
           for(tie(verIterEnd.first,verIterEnd.second)=vertices(l_graph);
                   verIterEnd.first != verIterEnd.second;++verIterEnd.first)
             {
                     // to check loop running right or not
                    verVisitor = *verIterEnd.first;
                    counter++;
                    if(verAffected==verVisitor)
                       {
                                 // Do nothing
                       }
                   else
                        {

                            cout<<"\n*******************New Iteration *******************";
                            cout<<"\nInside Loop";


                            m_iXcordaffected = l_graph[verAffected].iCoord_X;
                            m_iYcordaffected = l_graph[verAffected].iCoord_Y;


                            cout<<"\nX Cordinate : "<<m_iXcordaffected;
                            cout<<"\nY Cordinate : "<<m_iYcordaffected;

                            m_iXcordvisit = l_graph[verVisitor].iCoord_X;
                            m_iYcordvisit = l_graph[verVisitor].iCoord_Y;

                            iXdisp = m_iXcordaffected - m_iXcordvisit;
                            iYdisp = m_iYcordaffected - m_iYcordvisit;



                            m_iDist = fabs(sqrt((iXdisp*iXdisp)
                            +(iYdisp*iYdisp)));

                            cout<<"\nDistance : "<<m_iDist;





                            iTotalXDisp = b_gwrapper.getVertXDisp(verAffected,l_graph);
                            iTotalYDisp = b_gwrapper.getVertYDisp(verAffected,l_graph);

                            cout<<"\nTotal Disp x : "<<iTotalXDisp;
                            cout<<"\nTotal Disp y : "<<iTotalYDisp;

                            if(!b_gwrapper.getIsVertexLocked(verAffected,l_graph))

                            {

                               iTotalXDisp = iTotalXDisp + (iXdisp/fabs(iXdisp))*getRepulsiveForce(m_iDist);
                               iTotalYDisp = iTotalYDisp + (iYdisp/fabs(iYdisp))*getRepulsiveForce(m_iDist);

                            }

                            edgebool= boost::edge(verAffected,verVisitor,l_graph);
                            if(edgebool.second==1)
                            {
                                if(!b_gwrapper.getIsVertexLocked(verAffected,l_graph))

                                {
                                     iTotalXDisp = iTotalXDisp - (iXdisp/fabs(iXdisp))*getAttractiveForce(m_iDist);
                                     iTotalYDisp = iTotalYDisp - (iYdisp/fabs(iYdisp))*getAttractiveForce(m_iDist);

                                }
                            }


                            if(!b_gwrapper.getIsVertexLocked(verAffected,l_graph))

                            {
                                     m_iXcordaffected = m_iXcordaffected + (iTotalXDisp/fabs(iTotalXDisp))*std::min(fabs((iTotalXDisp)),temperature);
                                     m_iYcordaffected = m_iYcordaffected + (iTotalYDisp/fabs(iTotalYDisp))*std::min(fabs((iTotalYDisp)),temperature);
                            }



                                     b_gwrapper.setVertXDisp(iTotalXDisp,verAffected,l_graph);
                                     b_gwrapper.setVertYDisp(iTotalYDisp,verAffected,l_graph);




                            cout<<"\nIteration No : "<<counter;
                            cout<<"\nAttractive Force : "<<getAttractiveForce(m_iDist);
                            cout<<"\nRepulsive Force : "<<getRepulsiveForce(m_iDist);








                    }


           }
              if(m_iXcordaffected > iMaxX  || m_iXcordaffected < iMinX ||
                      m_iYcordaffected >iMaxY || m_iYcordaffected < iMinY)
                      {
                          // set vertex lock here

                        //  bVertexLock = true;

                          b_gwrapper.setVertexLock(verAffected,l_graph);

                          if(m_iXcordaffected > iMaxX || m_iXcordaffected < iMinX )
                          {
                              if(m_iXcordaffected < iMinX)
                              {
                                  while(m_iXcordaffected < iMinX)
                                    {
                                     m_iXcordaffected = m_iXcordaffected +10 ; // use std rand instead 75
                                    }

                              }
                              if(m_iXcordaffected > iMaxX)
                              {
                                while(m_iXcordaffected > iMaxX)
                                  {
                                   m_iXcordaffected =  (iMaxX - 15); // use std rand instead 75
                                  }
                              }
                          }


                            if(m_iYcordaffected > iMaxY || m_iYcordaffected < iMinY)
                              {
                                if(m_iYcordaffected > iMaxY)
                                {
                                  while(m_iYcordaffected > iMaxY )

                                  {
                                      m_iYcordaffected =  (m_iYcordaffected - 15);
                                  }


                                }

                                if(m_iYcordaffected<iMinY)
                                {
                                    while(m_iYcordaffected < iMinY )

                                    {
                                        m_iYcordaffected =  (m_iYcordaffected + 10);
                                    }
                                }


                            }
                    }


           b_gwrapper.setVertexCenterCoordX(verAffected,l_graph,(int)m_iXcordaffected);
           b_gwrapper.setVertexCenterCoordY(verAffected,l_graph,(int)m_iYcordaffected);


             temperature = temperature*(1-counter/iterations);
        }


        iterations = iterations - 1;
    }
    cout<<"Loop Running : "<<counter;


 }


float Reingold:: getAttractiveForce(double idist)
{
     float attra = (idist*idist)/iK;


     return attra;



}

float Reingold :: getRepulsiveForce(double idist)
{
    float repul = (iK*iK)/idist;

    return repul;

}

int Reingold::setCompartMentProps(SubGraph &l_graph, int iMargine)
{
    LAYOUT_ASSERT(iMargine > 0,
                  LayoutException("setCompartMentProps",
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  "iMargine",
                                  " "));
    BoostGraphWrapper bGraphWrap;

    int iMaxMargine = 10; //default margine; used by innermost cluster
    ChildrenIterator itrChild, itrEndChild;

    //set children's props and find max margine among the children---------------------------------------------
    //for each child of current graph
    for(boost::tie(itrChild, itrEndChild) = l_graph.children(); itrChild != itrEndChild; ++itrChild)
    {
        //recursive call to set child's properties
        int iCurrentMargine = this->setCompartMentProps(*itrChild, iMargine);
        //update maximum margine
        if(iCurrentMargine > iMaxMargine)
        {
            iMaxMargine = iCurrentMargine;
        }
    }
    //---------------------------------------------------------------------------------------------------------

    //set current graph's props--------------------------------------------------------------------------------
    int iXMin = 2147483647; //x-coordinate of leftmost node
    int iYMin = 2147483647; //y-coordinate of topmost node
    int iXMax = -2147483648;//x-coordinate of rightmost node
    int iYMax = -2147483648;//y-coordinate of bottommost node
    int iHeightOfTopmost = 0;
    int iHeightOfLowermost = 0;
    int iWidthOfLeftmost = 0;
    int iWidthOfRightmost = 0;

        //for each vertex, find if it is leftmost/rightmost/topmost/lowermost--------------------------------------
        VertexIterPair verIter;
        for(tie(verIter.first,verIter.second) = vertices(l_graph); verIter.first!=verIter.second; ++verIter.first)
        {
            VertexDescriptor verDis = *verIter.first;
            int iXval = bGraphWrap.getVertexCenterCoordX(verDis, l_graph);
            int iYval = bGraphWrap.getVertexCenterCoordY(verDis, l_graph);
            int iVertexHeight = bGraphWrap.getVertexHeight(verDis, l_graph) ;
            int iVertexWidth = bGraphWrap.getVertexWidth(verDis, l_graph);

            if(iXval <= iXMin) //if it is leftmost node
            {
                iXMin = iXval;
                if(iVertexWidth > iWidthOfLeftmost)
                    iWidthOfLeftmost = iVertexWidth;
            }
            if(iXval >= iXMax) //if it is rightmost node
            {
                iXMax = iXval;
                if(iVertexWidth > iWidthOfRightmost)
                    iWidthOfRightmost = iVertexWidth;
            }
            if(iYval <= iYMin) //if it is lowermost node
            {
                iYMin = iYval;
                if(iVertexHeight > iHeightOfLowermost)
                    iHeightOfLowermost = iVertexHeight;
            }
            if(iYval >= iYMax) //if it is topmost node
            {
                iYMax = iYval;
                if(iVertexHeight > iHeightOfTopmost)
                    iHeightOfTopmost = iVertexHeight;
            }
        }
        LAYOUT_ASSERT((iXMax>=iXMin)&(iYMax>=iYMin),
                      LayoutException("setCompartMentProps",
                                      LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE,
                                      "min/max x-y coordinates",
                                      "vertices"));
        //---------------------------------------------------------------------------------------------------------

        //set properties-------------------------------------------------------------------------------------------
        int iGraphWidth = std::abs(iXMin - iXMax);
        int iGraphHeight = std::abs(iYMin - iYMax);
        int iLeftMargin = (iWidthOfLeftmost/2) + iMaxMargine;
        int iRightMargin = (iWidthOfRightmost/2) + iMaxMargine;
        int iTopMargin = (iHeightOfTopmost/2) + iMaxMargine;
        int iLowerMargin = (iHeightOfLowermost/2) + iMaxMargine;

        iGraphWidth = iGraphWidth + iLeftMargin + iRightMargin;
        iGraphHeight = iGraphHeight + iTopMargin + iLowerMargin;
        LAYOUT_ASSERT((iGraphWidth>0)&(iGraphHeight>0),
                      LayoutException("setCompartMentProps",
                                      LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE,
                                      "width/height",
                                      "vertices"));

        bGraphWrap.setGraphLeftTopCoordX(iXMin - iLeftMargin, l_graph);
        bGraphWrap.setGraphLeftTopCoordY(iYMin - iLowerMargin, l_graph);

        bGraphWrap.setGraphHeight(iGraphHeight, l_graph);
        bGraphWrap.setGraphWidth(iGraphWidth, l_graph);
        //---------------------------------------------------------------------------------------------------------

//    cout<<"iXMin: "<<iXMin<< "\n";
//    cout<<"iYMin: "<<iYMin<< "\n";

//    cout<<"iXMax: "<<iXMax<< "\n";
//    cout<<"iYMax: "<<iYMax<< "\n";

//    cout<<"iHeightOfTopmost: "<<iHeightOfTopmost<< "\n";
//    cout<<"iHeightOfLowermost: "<<iHeightOfLowermost<< "\n";
//    cout<<"iWidthOfLeftmost: "<<iWidthOfLeftmost<< "\n";
//    cout<<"iWidthOfRightmost: "<<iWidthOfRightmost<< "\n";

//    cout<<"iLeftMargin: "<<iLeftMargin<< "\n";
//    cout<<"iRightMargin: "<<iRightMargin<< "\n";
//    cout<<"iTopMargin: "<<iTopMargin<< "\n";
//    cout<<"iLowerMargin: "<<iLowerMargin<< "\n";

//    cout<<"Height:" << iGraphHeight << ",  Width:" << iGraphWidth << "\n";
//    cout<<"TOPLEFT:(" << iXMin - iLeftMargin << "," << iYMin - iLowerMargin << ")\n";
    //----------------------------------------------------------------------------------------------------------

    return iMaxMargine+10;
}




void Reingold :: SetClusterCompartmentProps(SubGraph &l_graph)
{

    //This function is used to set sub graph height and width
    VertexDescriptor verDisEnd;
    VertexIterPair verIterEnd;
    BoostGraphWrapper bGraphWrap;

    int iXMin =INT_MAX;
    int iYMin =INT_MAX;
    int iXMax = INT_MIN;
    int iYMax =INT_MIN;

    int iXval = 0;
    int iYval = 0;

    int iHeight =0;
    int iWidth=0;
    bool bIsVirtual = false;

   // setCompartMentProps(l_graph);
    int clustercounter =0;

    SubGraph* gChildGraph = NULL;
    int iTotalClusters =0;
    QQueue<SubGraph*> qSubgraphs;   // a queue of subgraphs
    qSubgraphs.enqueue(&l_graph);
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        iTotalClusters++;
        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;
            iterChild++)
        {
            clustercounter++;
            gChildGraph = &(*iterChild);
            qSubgraphs.enqueue(gChildGraph);
        }




            for(tie(verIterEnd.first,verIterEnd.second)=vertices((*gSubgraph1));
             verIterEnd.first!=verIterEnd.second;++verIterEnd.first)
              {


               verDisEnd = *verIterEnd.first;


               iXval = bGraphWrap.getVertexCenterCoordX(verDisEnd,(*gSubgraph1));
               iYval = bGraphWrap.getVertexCenterCoordY(verDisEnd,(*gSubgraph1));
               bIsVirtual = bGraphWrap.getIsVirtualNode(verDisEnd,(*gSubgraph1));



               if(!bIsVirtual) // skip virtual nodes
             {
               if(iXval < iXMin)
                 {
                  iXMin = iXval;
                 }
               if(iXval > iXMax)
                 {
                  iXMax = iXval;
                 }
               if(iYval < iYMin)
                 {
                  iYMin = iYval;
                 }
               if(iYval > iYMax)
                 {
                  iYMax = iYval;
                 }
             }
             bIsVirtual = false;





            }

         iWidth = std::abs(iXMin - iXMax);
         iHeight= std::abs(iYMin - iYMax);
         bGraphWrap.setGraphLeftTopCoordX(iXMin-10,(*gSubgraph1));  // +10
         bGraphWrap.setGraphLeftTopCoordY(iYMin-10,(*gSubgraph1));  // +10

         bGraphWrap.setGraphHeight(iHeight+20,(*gSubgraph1));    // +10
         bGraphWrap.setGraphWidth(iWidth+20,(*gSubgraph1));      // +10


         bGraphWrap.setGraphCenterCoordX((iXMin+iXMax)/2);
         bGraphWrap.setGraphCenterCoordY((iYMax+iYMin)/2);

         cout<<"\n ***** Cluster Id : "<<bGraphWrap.getGraphClusterID(*gSubgraph1);
         cout<<"  Height : "<<iHeight;
         cout<<"  Width :  "<<iWidth;
         cout<<"  Top Left X  :  "<<iXMin - 30 ;
         cout<<"  Top Left Y  :  "<<iYMin - 30 ;

         cout<<"***********\n";


          iXMin =INT_MAX;
          iYMin =INT_MAX;
          iXMax = INT_MIN;
          iYMax =INT_MIN;

          iXval = 0;
          iYval = 0;

          iHeight =0;
          iWidth=0;

        }

    }




void Reingold :: SetClusterCompartmentPropsNew(SubGraph &l_graph)
{

    ChildrenIterator itrChild, itrEndChild;
       for(boost::tie(itrChild, itrEndChild) = l_graph.children(); itrChild != itrEndChild; ++itrChild)
       {
           this->SetClusterCompartmentPropsNew(*itrChild);
       }

       VertexDescriptor verDis;
       VertexIterPair verIter;
       BoostGraphWrapper bGraphWrap;

       int iXMin = 2147483647;
       int iYMin = 2147483647;
       int iXMax = -2147483648;
       int iYMax = -2147483648;
       int iMaxHeight = 0;
       int iMaxWidth = 0;

       int iHeightOfTopmost = 0;
       int iHeightOfLowermost = 0;
       int iWidthOfLeftmost = 0;
       int iWidthOfRightmost = 0;

       for(tie(verIter.first,verIter.second) = vertices(l_graph); verIter.first!=verIter.second; ++verIter.first)
       {
           verDis = *verIter.first;
           int iXval = bGraphWrap.getVertexCenterCoordX(verDis, l_graph);
           int iYval = bGraphWrap.getVertexCenterCoordY(verDis, l_graph);
           int iVertexHeight = bGraphWrap.getVertexHeight(verDis, l_graph) ;
           int iVertexWidth = bGraphWrap.getVertexWidth(verDis, l_graph);

           if(iXval < iXMin)
           {
               iXMin = iXval;
               if(iVertexWidth > iWidthOfLeftmost)
                   iWidthOfLeftmost = iVertexWidth;
           }
           if(iXval > iXMax)
           {
               iXMax = iXval;
               if(iVertexWidth > iWidthOfRightmost)
                   iWidthOfRightmost = iVertexWidth;
           }
           if(iYval < iYMin)
           {
               iYMin = iYval;
               if(iVertexHeight > iHeightOfLowermost)
                   iHeightOfLowermost = iVertexHeight;
           }
           if(iYval > iYMax)
           {
               iYMax = iYval;
               if(iVertexHeight > iHeightOfTopmost)
                   iHeightOfTopmost = iVertexHeight;
           }
           if(iVertexHeight > iMaxHeight)
               iMaxHeight = iVertexHeight;
           if(iVertexWidth > iMaxWidth)
               iMaxWidth = iVertexWidth;
       }

       int iGraphWidth = std::abs(iXMin - iXMax);
       int iGraphHeight = std::abs(iYMin - iYMax);

       int iLeftMargin = (iWidthOfLeftmost/2) + 20;
       int iRightMargin = (iWidthOfRightmost/2) + 20;
       int iTopMargin = (iHeightOfTopmost/2) + 20;
       int iLowerMargin = (iHeightOfLowermost/2) + 20;

       iGraphWidth = iGraphWidth + iLeftMargin + iRightMargin;
       iGraphHeight = iGraphHeight + iTopMargin + iLowerMargin;

       bGraphWrap.setGraphLeftTopCoordX(iXMin - iLeftMargin, l_graph);
       bGraphWrap.setGraphLeftTopCoordY(iYMin - iLowerMargin, l_graph);

       bGraphWrap.setGraphHeight(iGraphHeight, l_graph);
       bGraphWrap.setGraphWidth(iGraphWidth, l_graph);



}












