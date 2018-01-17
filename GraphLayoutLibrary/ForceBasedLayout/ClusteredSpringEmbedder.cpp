#include "ClusteredSpringEmbedder.h"



ClusteredSpringEmbedder::ClusteredSpringEmbedder()
{

}

void ClusteredSpringEmbedder :: springForEachCluster(SubGraph& gMaingraph, int iIterations)
{

    LAYOUT_ASSERT( &gMaingraph != NULL,
                  LayoutException(__FUNCTION__
                                  ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                  ,"Input and Output graphml path"
                                  ,"applyLayout"));


    Postprocessing postProcessor, preprocessor;
    preprocessor.applyPreProcessing(gMaingraph);
    m_gMaingraph = gMaingraph;
    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&gMaingraph);
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();

        // call each step of algo with a subgraph
        springStep((*gSubgraph1),gMaingraph,iIterations);

        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;iterChild++)
        {
            SubGraph* gChildGraph = &(*iterChild);
            if(boost::num_vertices(*gChildGraph) ==0)
            {
                continue;  // handle empty compartments
            }
            qSubgraphs.enqueue(gChildGraph);
        }
    }


    // set Vertices Displacements as zero
    springSetVerticesDisplacementZero();
     // handle empty clusters
    postProcessor.applyPostProcessing(gMaingraph);
    // Check if two cluster overlaps
    postProcessor.checkClusterOverlap(gMaingraph);

    // try to reduce vertex overlap trivially
    postProcessor.overlapRemoval(gMaingraph);

   // VertexOverlapRemoval vertexOverlapRemove;           to be included
   // vertexOverlapRemove.removeOverlaps(gMaingraph);     separately

    // check if membership of any subgraph is violated
    postProcessor.checkAndFixMembership(gMaingraph);
    postProcessor.membershipFixer(gMaingraph);




}

void ClusteredSpringEmbedder:: springTestRecursive(SubGraph& gMaingraph)
{
    // To do : Test this approach
    ChildrenIterator itrChild, itrEndChild;
    for(boost::tie(itrChild, itrEndChild) = gMaingraph.children();
        itrChild != itrEndChild; ++itrChild)
    {
        this->springStep(*itrChild, gMaingraph,100);
    }

}

void ClusteredSpringEmbedder:: springStep(SubGraph& gSubgraph,
                                          SubGraph& gMaingraph,
                                          int iIterations)
{


    while(iIterations)
    {
        // relax only immediate edges of subgraphs
        springRelaxImmediate(gSubgraph);

        // relax interedges
        springRelaxInterEdges(gMaingraph);

        // repel only immediate subgraphs
        springRepelImmediate(gSubgraph);

        // clusters repelling other nodes only
        springRepelFromClusters(gSubgraph);

        // repel cluster to cluster
        springTestRepelClusterToCluster(gSubgraph);

        // move clusters a nudge
        springMoveClusters(gSubgraph);

        // move nodes a nudge
        springMoveRestricted(gSubgraph);


        iIterations--;
    }
    Reingold *rein = new Reingold();
    rein->setCompartMentProps(gSubgraph,10); // set height , width

}

void ClusteredSpringEmbedder:: springRelaxImmediate(SubGraph& gSubgraph)
{

    int iGraphId = m_bGraphWrapper.getGraphClusterID(gSubgraph);
    int iVertexGraphId1, iVertexGraphId2;

    EdgeIterator eStart, eEnd;

    for (boost::tie(eStart, eEnd) = edges(gSubgraph); eStart != eEnd; ++eStart)
    {
      VertexDescriptor verV = source(*eStart, gSubgraph);
      VertexDescriptor verU = target(*eStart, gSubgraph);

      iVertexGraphId1 = m_bGraphWrapper.getVertexClusterID(verV,gSubgraph);
      iVertexGraphId2 = m_bGraphWrapper.getVertexClusterID(verU,gSubgraph);

      if(!(iVertexGraphId1 == iVertexGraphId2 && iVertexGraphId1 == iGraphId) )
      {
          continue;  // not an inter edges
      }

      double dXCoordV =  m_bGraphWrapper.getVertexCenterCoordX(verV,gSubgraph);
      double dYCoordV =  m_bGraphWrapper.getVertexCenterCoordY(verV,gSubgraph);

      double dXCoordU =  m_bGraphWrapper.getVertexCenterCoordX(verU,gSubgraph);
      double dYCoordU =  m_bGraphWrapper.getVertexCenterCoordY(verU,gSubgraph);

      double dXDisp =  m_bGraphWrapper.getVertXDisp(verV,gSubgraph);
      double dYDisp =  m_bGraphWrapper.getVertYDisp(verV,gSubgraph);

      double dXDispU = 0;
      double dYDispU = 0;

      dXDispU =  m_bGraphWrapper.getVertXDisp(verU,gSubgraph);
      dYDispU =  m_bGraphWrapper.getVertYDisp(verU,gSubgraph);

      // Logic for vertex size here

      double dXsizeU =  m_bGraphWrapper.getVertexWidth(verU,gSubgraph);
      double dXsizeV =  m_bGraphWrapper.getVertexWidth(verV,gSubgraph);

      if(dXCoordU > dXCoordV)
      {
          dXCoordV = dXCoordV + dXsizeV/2;
          dXCoordU = dXCoordU - dXsizeU/2;
      }
      else
      {
          dXCoordV = dXCoordV - dXsizeV/2;
          dXCoordU = dXCoordU + dXsizeU/2;

      }
      double dVx = dXCoordV - dXCoordU;
      double dVy = dYCoordV - dYCoordU;

      // calculate movement vectors

      cout<<"\n Delta X and Y at attractive force :  "<<dVy<<" "
            << dVx;
      double dDist = fabs(sqrt((dVx*dVx+dVy* dVy)));
      cout<<"Distance at attractive force :  "<<dDist;

      if(dDist == 0)
      {
          dDist = 0.0001;
      }

      // desired length = 50   MACRO
      // HOOK'S LAW
      double dForce =  ATTRACTION_MULTIPLIER *( DEFAULT_EDGE_LENGTH - dDist)/dDist ;

      // reduce force if degree of node is high ---- Degree Based charge KUMAR
      dForce = dForce * std::pow(0.7,in_degree(verV,gSubgraph)+in_degree(verU,gSubgraph)
                                 + out_degree(verU,gSubgraph)+out_degree(verV,gSubgraph) -2);

      double dx = dForce * dVx;
      double dy = dForce * dVy;


      // add to displacement
      dXDisp += dx;
      dYDisp += dy;

      dXDispU -= dx;
      dYDispU -= dy;

     cout<<"\n Attractive Forces Displacement " ;

     cout<<"\n at I X Disp : "<<dXDisp;
     cout<<"  Y Disp : "<<dYDisp;


     cout<<"\n at J  X Disp : "<<dXDispU;
     cout<<"   Y Disp : "<<dYDispU;

      m_bGraphWrapper.setVertXDisp(dXDisp,verV,gSubgraph);
      m_bGraphWrapper.setVertYDisp(dYDisp,verV,gSubgraph);

      m_bGraphWrapper.setVertXDisp(dXDispU,verU,gSubgraph);
      m_bGraphWrapper.setVertYDisp(dYDispU,verU,gSubgraph);
    }
}

void ClusteredSpringEmbedder :: springRepelImmediate(SubGraph& gSubgraph)
{
    VertexDescriptor verAffected,verVisitor;
    VertexIterPair   verIter,verIterEnd;

    int iGraphId = m_bGraphWrapper.getGraphClusterID(gSubgraph);
    int iVertexGraphId1, iVertexGraphId2;

    for(tie(verIter.first,verIter.second)=vertices(gSubgraph)
                ;verIter.first!=verIter.second;++verIter.first)
    {

        verAffected = *verIter.first;
        double dDx = 0;
        double dDy = 0;
        double dXDisp =0;
        double dYDisp = 0;

        double dXCoordV = m_bGraphWrapper.getVertexCenterCoordX(verAffected,gSubgraph);
        double dYCoordV = m_bGraphWrapper.getVertexCenterCoordY(verAffected,gSubgraph);

        for(tie(verIterEnd.first,verIterEnd.second)=vertices(gSubgraph);
                       verIterEnd.first != verIterEnd.second;++verIterEnd.first)
        {

            verVisitor = *verIterEnd.first;


            iVertexGraphId1 = m_bGraphWrapper.getVertexClusterID(verAffected,gSubgraph);
            iVertexGraphId2 = m_bGraphWrapper.getVertexClusterID(verVisitor,gSubgraph);

            if(!(iVertexGraphId1 == iVertexGraphId2 && iVertexGraphId1 == iGraphId) )
            {
                continue; // not immediate neighbours
            }

           double dXCoordU = m_bGraphWrapper.getVertexCenterCoordX(verVisitor,gSubgraph);
           double dYCoordU = m_bGraphWrapper.getVertexCenterCoordY(verVisitor,gSubgraph);


                if(verAffected == verVisitor)
               {
                    continue;
               }

                // Logic for vertex size here

                double dXsizeU = m_bGraphWrapper.getVertexWidth(verVisitor,gSubgraph);
                double dXsizeV = m_bGraphWrapper.getVertexWidth(verAffected,gSubgraph);
               //  try to comment it
                if(dXCoordU > dXCoordV)
                {
                    dXCoordV = dXCoordV + dXsizeV/2;
                    dXCoordU = dXCoordU - dXsizeU/2;

                }
                else
                {
                    dXCoordV = dXCoordV - dXsizeV/2;
                    dXCoordU = dXCoordU + dXsizeU/2;
                }

                // get cliiping points here cordinates here.

                   double dVx = dXCoordV - dXCoordU;
                   double dVy = dYCoordV - dYCoordU;

                   // calculate square distance
                   double dDist = fabs((dVx*dVx + dVy* dVy));

                    cout<<"\nDistance at Repulsive force :  "<<dDist;

                    if(dDist == 0 || dDist < 500 )
                    {
                        // too close move some random distance away
                        dDist = std::rand()%300+100;
                    }
                    if(dDist < REPULSION_RANGE )
                      {
                       dDx += REPULSION_MULTIPLIER*dVx/dDist;  // factor =1 def works well for 1000
                       dDy += REPULSION_MULTIPLIER*dVy/dDist;
                      }
                    else{
                        cout<<"\n Skipping due to distance";
                    }
                    cout<<"\ndx and dy at Repulsive force :  "<<dDx<<" "<<dDy;

            }
            double dLen = dDx*dDx+ dDy*dDy;
            dLen = sqrt(fabs(dLen))/2;
             if(dLen > 0)
             {

                dXDisp = m_bGraphWrapper.getVertXDisp(verAffected,gSubgraph);
                dYDisp = m_bGraphWrapper.getVertYDisp(verAffected,gSubgraph);

                // add displacement
                dXDisp += (dDx/dLen);
                dYDisp += (dDy/dLen);
                m_bGraphWrapper.setVertXDisp(dXDisp,verAffected,gSubgraph);
                m_bGraphWrapper.setVertYDisp(dYDisp,verAffected,gSubgraph);
                cout<<"\nDisplacement at Repulsive force :  "<<dYDisp<<" "
                   << dXDisp;
           }
        }
}

void ClusteredSpringEmbedder :: springMoveRestricted(SubGraph& gSubgraph)
{

    int iGHeight , iGwidth, iMinX , iMaxY,iMinY;

    iGHeight = m_bGraphWrapper.getGraphHeight(gSubgraph);
    iGwidth  = m_bGraphWrapper.getGraphWidth(gSubgraph);

    iMinX = m_bGraphWrapper.getGraphLeftTopCoordX(gSubgraph);
    iMaxY = m_bGraphWrapper.getGraphLeftTopCoordY(gSubgraph);
    iMaxY = iMinX + iGwidth;
    iMinY = iMaxY - iGHeight;

    VertexDescriptor verAffected;
    VertexIterPair   verIter;

    int iGraphId = m_bGraphWrapper.getGraphClusterID(gSubgraph);
    int iVertexGraphId1;
    for(tie(verIter.first,verIter.second)=vertices(gSubgraph)
                ;verIter.first!=verIter.second;++verIter.first)
    {

        verAffected = *verIter.first;
        iVertexGraphId1 = m_bGraphWrapper.getVertexClusterID(verAffected,gSubgraph);
        if(iGraphId  != iVertexGraphId1)
        {
            continue; // not part of current graph
        }
      double  dXCoordV = m_bGraphWrapper.getVertexCenterCoordX
              (verAffected,gSubgraph);
      double dYCoordV = m_bGraphWrapper.getVertexCenterCoordY
              (verAffected,gSubgraph);

      double dXDisp = m_bGraphWrapper.getVertXDisp(verAffected,gSubgraph);
      double dYDisp = m_bGraphWrapper.getVertYDisp(verAffected,gSubgraph);

      m_bGraphWrapper.setVertXDisp(dXDisp/4,verAffected,gSubgraph);  // /4
      m_bGraphWrapper.setVertYDisp(dYDisp/4,verAffected,gSubgraph);  //  /4

      // limit displacement to max value
      if(dXDisp < 0)
      {
          dXDisp = std::max(dXDisp,- UNIT_DISPLACEMENT);
      }
      else
      {
          dXDisp = std::min(dXDisp,UNIT_DISPLACEMENT);
      }
      if(dYDisp < 0)
      {
           dYDisp = std::max(dYDisp,- UNIT_DISPLACEMENT);
      }
      else
      {
          dYDisp = std::min(dYDisp,UNIT_DISPLACEMENT);
      }



        m_bGraphWrapper.setVertexCenterCoordX(verAffected,gSubgraph,
                                               dXCoordV+dXDisp);
        m_bGraphWrapper.setVertexCenterCoordY(verAffected,gSubgraph,
                                               dYCoordV+dYDisp);


    }
}

void ClusteredSpringEmbedder :: springRepelFromClusters(SubGraph& gSubgraph)
{

    cout<<"\n ***********Repelling Vertices with Cluster First ***********";
    VertexDescriptor verAffected;
    VertexIterPair   verIter;

    int iGraphId = m_bGraphWrapper.getGraphClusterID(gSubgraph);
    int iVertexGraphId1 ;
    double dDx = 0,dDy = 0;
    // for each immediate cluster of received subgraph
    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&gSubgraph);
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;
            iterChild++)
        {
            SubGraph* gChildGraph = &(*iterChild);           
            for(tie(verIter.first,verIter.second)=vertices(gSubgraph)
                        ;verIter.first!=verIter.second;++verIter.first)
            {
                verAffected = *verIter.first;
                iVertexGraphId1 = m_bGraphWrapper.getVertexClusterID(verAffected,gSubgraph);
                if(iVertexGraphId1 != iGraphId)
                {
                    continue; // not part of subGraph
                }
                int iClusterTopLeftXCoord = m_bGraphWrapper.getGraphLeftTopCoordX(*gChildGraph);
                int iClusterTopLeftYCoord = m_bGraphWrapper.getGraphLeftTopCoordY(*gChildGraph);
                int iClusterHeight = m_bGraphWrapper.getGraphHeight(*gChildGraph);
                int iClusterWidth = m_bGraphWrapper.getGraphWidth(*gChildGraph);
                int iClusterCentreXCoord = m_bGraphWrapper.getGraphCenterCoordX(*gChildGraph);
                int iClusterCentreYCoord = m_bGraphWrapper.getGraphCenterCoordY(*gChildGraph);
                int iVertexXCoord = m_bGraphWrapper.getVertexCenterCoordX(verAffected,gSubgraph);
                int iVertexYCoord = m_bGraphWrapper.getVertexCenterCoordY(verAffected,gSubgraph);
                int iClusterClipXCoord , iClusterClipYCoord;

                //Form 4 pairs forclusters and edge between two centres
                typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> Points;
                Points pP,pQ,pR,pS,pX,pY;

                typedef boost::geometry::model::segment<Points> Segment;
                std::vector<Points> pOutput;
                pP.set<0>(iClusterTopLeftXCoord);
                pP.set<1>(iClusterTopLeftYCoord);
                pQ.set<0>(iClusterTopLeftXCoord);
                pQ.set<1>(iClusterTopLeftYCoord + iClusterHeight);
                pR.set<0>(iClusterTopLeftXCoord + iClusterWidth);
                pR.set<1>(iClusterTopLeftYCoord + iClusterHeight);
                pS.set<0>(iClusterTopLeftXCoord+ iClusterWidth);
                pS.set<1>(iClusterTopLeftYCoord);
                pX.set<0>(iClusterCentreXCoord);
                pX.set<1>(iClusterCentreYCoord);
                pY.set<0>(iVertexXCoord);
                pY.set<1>(iVertexYCoord);
                Segment PQ( pP,pQ );
                Segment QR( pQ,pR );
                Segment RS( pR,pS );
                Segment SP( pS,pP );
                Segment Edge (pX,pY);
                bool bNoIntersection = false;
                cout<<"\n ***********Repelling Vertices with Cluster";
                if(boost::geometry::intersects(Edge,PQ))
                {
                    boost::geometry::intersection(Edge,PQ,pOutput);
                    cout<<" \n Cluster Repulsion Clip Found with Edge PQ";
                }
                else if( boost::geometry::intersects(Edge,QR))
                {
                    boost::geometry::intersection(Edge,QR,pOutput);
                    cout<<" \n Cluster Repulsion Clip Found with Edge QR";
                }
                else
                {
                    if(boost::geometry::intersects(Edge,RS))
                    {
                        boost::geometry::intersection(Edge,RS,pOutput);
                        cout<<" \n Cluster Repulsion Clip Found with Edge RS";
                    }
                    else if(boost::geometry::intersects(Edge,SP))
                    {
                        boost::geometry::intersection(Edge,SP,pOutput);
                        cout<<" \n Cluster Repulsion Clip Found with Edge SP";
                    }
                    else
                    {
                        cout<<"\n *** No Intersection in Outer  ***  Check Cluster cordinates" ;
                        bNoIntersection = true;
                    }
                }
                if(!bNoIntersection)
                {
                  // intersection found
                  Points pIntersection = pOutput[0];
                  iClusterClipXCoord = pIntersection.get<0>();
                  iClusterClipYCoord = pIntersection.get<1>();
                  cout<<"\n Intersection at "<<iClusterClipXCoord <<" "<<iClusterClipYCoord;
                  double dVx = iClusterClipXCoord - iVertexXCoord;
                  double dVy = iClusterClipYCoord - iVertexYCoord;
                  double dDist = fabs((dVx*dVx + dVy* dVy));
                 cout<<"\nDistance at Cluster Repulsion  :  "<<dDist;
                 if(dDist == 0 )
                 {
                     dDist = 0.001;
                 }
                 bool bTooClose = false;
                 if( dDist < CLUSTER_REPULSION_RANGE )              // within range only
                   {
                     dDx += CLUSTER_REPULSION_MULTIPLIER*dVx/dDist;  // factor = 1 def , works well for 1000
                     dDy += CLUSTER_REPULSION_MULTIPLIER*dVy/dDist;

                     if(dDist < CLOSE_TO_BOUNDRY )        // Close to boundry 200 default
                     {
                         bTooClose = true;
                     }
                      cout<<"\ndx and dy at Repulsive force :  "<<dDx<<" "<<dDy;
                      double dLen = dDx*dDx+ dDy*dDy;
                      dLen = sqrt(fabs(dLen))/2;
                      double dXDisp = 0,dYDisp = 0;
                      int iClusterXDisp= 0 , iClusterYDisp = 0;
                      dXDisp = m_bGraphWrapper.getVertXDisp(verAffected,gSubgraph);
                      dYDisp = m_bGraphWrapper.getVertYDisp(verAffected,gSubgraph);
                      if(bTooClose)
                    {
                      // vertex is too close to cluster boundry
                      if(dDx > 0)
                      {
                        dXDisp += std::min(20.0,dDx/dLen);
                      }
                      else
                      {
                        dXDisp += std::max( -20.0,dDx/dLen);
                      }
                      {
                      if(dDy > 0)
                      {
                        dYDisp += std::min(20.0,dDy/dLen);
                      }
                      else
                      {
                        dYDisp += std::max(-20.0,dDy/dLen);
                      }
                      }
                    }
                      else
                      {
                          if(dDx > 0)
                          {
                            dXDisp += CLOSE_TO_BOUNDRY - dDist;
                          }
                          else
                          {
                            dXDisp += - CLOSE_TO_BOUNDRY + dDist;
                          }
                          {
                          if(dDy > 0)
                          {
                            dYDisp += CLOSE_TO_BOUNDRY - dDist;
                          }
                          else
                          {
                            dYDisp += - CLOSE_TO_BOUNDRY + dDist;
                          }
                          }
                          bTooClose = false;
                      }
                      m_bGraphWrapper.setVertXDisp(dXDisp,verAffected,gSubgraph);
                      m_bGraphWrapper.setVertYDisp(dYDisp,verAffected,gSubgraph);
                      cout<<"\nX Disp "<<dDx/dLen<<" "<<"Y Disp "<<dDy/dLen;
                      iClusterXDisp = m_bGraphWrapper.getClusterXDisplacement(*gChildGraph);
                      iClusterYDisp = m_bGraphWrapper.getClusterYDisplacement(*gChildGraph);
                      if(dDx > 0)
                      {
                        iClusterXDisp += (int)std::min(2.0,dDx/dLen);
                      }
                      else
                      {
                        iClusterXDisp += (int)std::max( - 2.0,dDx/dLen);
                      }
                     {
                      if(dDy > 0)
                      {
                        iClusterYDisp += (int)std::min(2.0,dDy/dLen);
                      }
                      else
                      {
                        iClusterYDisp += (int)std::max(-2.0,dDy/dLen);
                      }
                     }
                      m_bGraphWrapper.setClusterXDisplacement(*gChildGraph,iClusterXDisp);
                      m_bGraphWrapper.setClusterYDisplacement(*gChildGraph,iClusterYDisp);
                      cout<<"\nDisplacement at Cluster  Repulsion :  "<<dYDisp<<" "<< dXDisp;
                }
            }
        }
        }
    }
}
void ClusteredSpringEmbedder :: springRepelClustersToCluster(SubGraph& gSubgraph)
{

    // NOT USED ANY MORE REMOVE

}
/* Algo for interedges
 *
 * 1> For each edge E do
 * 2> get vertices u and v
 * 3> check graph id's
 *
 * Case i) u = 999 and v = immediate subgraph
 *         get Clipping Points
 *         Apply RF attractive or spring embedder
 *
 * Case ii)u = 999 and v = non immediate subgraph
 *         get outermost parent of v
 *         get clipping points1 for 999 and outer most parent of v
 *         Apply RF attractive to clip point1 and u
 *         get clipping points2 for 999 and subgraph of v
 *         Apply RF attractive to clip point2 and v.
 *
 * Case iii) u = XXX and v = YYY
 *           get Clip point1 with subgraph u and v
 *           apply rf attractive with clip point1 and u.
 *           get Clip point2 with subgraph v and and u.
 *           apply rf attractive with clip point2 and v.

*/

// case 3 and 4
// get two outermost rectangles , check whether they overlap
// if they overlap then case 3
// else case 4
// Overlap i.e case 3
// Algorithm

  /* 1> Get two vertices
     2> get their graphids
     3> get their graphs by iterating
     4> check whether overlapping or not

     5> Overlapping
     6> find out which graph cointains other thus finding outer and inner
     7> repeat case 2 :)
    */


void ClusteredSpringEmbedder :: springRelaxInterEdges(SubGraph& gMaingraph)
{
    LAYOUT_ASSERT( &gMaingraph != NULL,
                  LayoutException(__FUNCTION__
                                  ,LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                  ,"Input and Output graphml path"
                                  ,"springRepelInterEdges"));

    RelaxInterEdges *relaxInterEdges = new RelaxInterEdges();
    std::cout<< "\n  **************** Continuing inter edges *************** ";
    EdgeIterator eStart, eEnd;
    for (boost::tie(eStart, eEnd) = edges(gMaingraph); eStart != eEnd; ++eStart)
    {
      std::cout<< "\n  **************** Continuing inter edges *************** ";
      VertexDescriptor vSource = source(*eStart, gMaingraph);
      VertexDescriptor vTarget = target(*eStart, gMaingraph);

      int iSourceGraphId = m_bGraphWrapper.getVertexClusterID(vSource,gMaingraph);
      int iTargetGraphId = m_bGraphWrapper.getVertexClusterID(vTarget,gMaingraph);

      if(iSourceGraphId == iTargetGraphId  )
      {
          continue;  // not an interedge
      }

       int iParentGraphId = 0;
       bool bParentFound = false , bIsSourceOuter = false;
       bool bISCase3OR4 = false;
       SubGraph* gSource = NULL  , *gTarget = NULL ;

       // decide the type of interedge
      if(iSourceGraphId == 999 || iTargetGraphId == 999  )
      {
          if(iSourceGraphId == 999)
          {
              gSource = &gMaingraph;
              bIsSourceOuter = true;                  // check case 1,2 or 3,4
          }
          else if(iTargetGraphId == 999)
          {
              gTarget = &gMaingraph;
              bIsSourceOuter = false ;
          }
      }
      else
      {
          bISCase3OR4 = true;
      }
    if(!bISCase3OR4)
    {
      QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
      qSubgraphs.enqueue(&gMaingraph);
      SubGraph* gSubgraph1 = NULL;
      while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
      {
          gSubgraph1 = qSubgraphs.dequeue();
          ChildrenIterator iterChild , iterChildEnd;
          for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
              iterChild != iterChildEnd;iterChild++)
          {
              SubGraph* gChildGraph = &(*iterChild);
              int iClusterId = m_bGraphWrapper.getGraphClusterID(*gChildGraph);

              // decide whether source is outer or inner
              if(bIsSourceOuter)
              {
                  if(iClusterId == iTargetGraphId)
                  {
                       gTarget = gChildGraph;

                  }
              }
              else if(!bIsSourceOuter)
              {
                  if(iClusterId == iSourceGraphId)
                  {
                       gSource = gChildGraph;
                  }
              }
              else
              {
                  // case 3
                  if(iClusterId == iTargetGraphId)
                  {
                       gTarget = gChildGraph;
                  }
                  if(iClusterId == iSourceGraphId)
                  {
                       gSource = gChildGraph;
                  }
              }
              qSubgraphs.enqueue(gChildGraph);
          }
      }
      // get two graph objects of ids of source and target
      bool bIsSourceRoot = (*gSource).is_root();  // for checking purpose
      bool bIsTatgetRoot = (*gTarget).is_root();
      cout<<"\n Source Root " << bIsSourceRoot;
      cout<<"\n Target Root " << bIsTatgetRoot;
      int iMapGraphid1 = m_bGraphWrapper.getGraphClusterID(*gSource);
      int iMapGraphid2 = m_bGraphWrapper.getGraphClusterID(*gTarget);
      cout<<"\n Source ID "<<iMapGraphid1;
      cout<<"\n Target ID "<<iMapGraphid2;

    }
    SubGraph *gTempGraph = NULL ;
      // Case 1 and 2  *** VIMP Logic
      if(iSourceGraphId == 999 || iTargetGraphId == 999)
      {
          // case 1 and 2 starts here
          std::cout<< "\n  ****************Case 1 or 2 detected : Continuing inter edges ";
          if(iSourceGraphId == 999)
          {
              bIsSourceOuter = true;        // source is outer
              gTempGraph = (gTarget);
              while( !bParentFound )
              {
                  SubGraph& gParentGraph =  (*gTempGraph).parent();
                  iParentGraphId = m_bGraphWrapper.getGraphClusterID(gParentGraph);
                  if(iParentGraphId == 999)
                  {
                      bParentFound = true;
                  }
                  else
                  {
                   gTempGraph = & gParentGraph;
                  }
              }
              //temp graph has outermost parent of target vertex
              // get cordinates of outer most parent and call do intersect with
              // edge cordinates if returns true call getIntersection points
          }
          else
          {
              gTempGraph = gSource;

              // find the parent graph of inner graphs
              while( !bParentFound )
              {
                  SubGraph& gParentGraph = (*gTempGraph).parent();
                  iParentGraphId = m_bGraphWrapper.getGraphClusterID(gParentGraph);
                  if(iParentGraphId == 999)
                  {
                      bParentFound = true;
                  }
                  else
                  {
                   gTempGraph = & gParentGraph;
                  }
              }
          }
          int iHeight,iLeftTopX,iLeftTopY,iWidth; // outer compartment of inner vertex
          iHeight = m_bGraphWrapper.getGraphHeight(*gTempGraph);
          iWidth = m_bGraphWrapper.getGraphWidth(*gTempGraph);
          iLeftTopX = m_bGraphWrapper.getGraphLeftTopCoordX(*gTempGraph);
          iLeftTopY = m_bGraphWrapper.getGraphLeftTopCoordY(*gTempGraph);
          int iOuterMostGraphID = m_bGraphWrapper.getGraphClusterID(*gTempGraph);

          cout<< "\n Outer Cordinates of Clusters " ;
          cout<<"\n Outer Graph ID "<<iOuterMostGraphID;
          cout<< "Height "<<iHeight;
          cout<< "Width "<<iWidth;
          cout<< "Lefttop X "<<iLeftTopX;
          cout<< "Lefttop Y "<<iLeftTopY;

          int iSourceX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
          int iSourceY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
          int iTargetX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
          int iTargetY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);

          cout<< "\n Source X "<<iSourceX;
          cout<< "\n Source Y "<<iSourceY;

          cout<< "\n Target X "<<iTargetX;
          cout<< "\n Target Y "<<iTargetY;
          // Form 4 piars of P Q R S with Edge e ===> PQ , QR , RS , SP
           typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> Points;
           boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> pP;
           boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pQ;
           boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pR;
           boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pS;

           boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pX;
           boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pY;
          typedef boost::geometry::model::segment<Points> Segment;
          std::vector<Points> pOutput;
          pP.set<0>(iLeftTopX);
          pP.set<1>(iLeftTopY);
          pQ.set<0>(iLeftTopX);
          pQ.set<1>(iLeftTopY + iHeight);
          pR.set<0>(iLeftTopX + iWidth);
          pR.set<1>(iLeftTopY + iHeight);
          pS.set<0>(iLeftTopX+ iWidth);
          pS.set<1>(iLeftTopY);

          pX.set<0>(iSourceX);
          pX.set<1>(iSourceY);

          pY.set<0>(iTargetX);
          pY.set<1>(iTargetY);

          Segment PQ( pP,pQ );
          Segment QR( pQ,pR );
          Segment RS( pR,pS );
          Segment SP( pS,pP );
          Segment Edge (pX,pY);
          int iOuterIntersect[2] = {0,0};  // x and y
          bool bNoOuterIntersection = false;

          // need 5 line segments
          if(boost::geometry::intersects(Edge,PQ))
          {
              boost::geometry::intersection(Edge,PQ,pOutput);
              cout<<" \n Outer Clip Found with Edge PQ";
          }
          else if( boost::geometry::intersects(Edge,QR))
          {
              boost::geometry::intersection(Edge,QR,pOutput);
              cout<<" \n Outer Clip Found with Edge QR";
          }
          else
          {
              if(boost::geometry::intersects(Edge,RS))
              {
                  boost::geometry::intersection(Edge,RS,pOutput);
                  cout<<" \n Outer Clip Found with Edge RS";
              }
              else if(boost::geometry::intersects(Edge,SP))
              {
                  boost::geometry::intersection(Edge,SP,pOutput);
                  cout<<" \n Outer Clip Found with Edge SP";
              }
              else
              {
                  // no intersection , not possible but handle
                  cout<<"\n *** No Intersection in Outer  ***  Check Cluster cordinates" ;
                  bNoOuterIntersection = true;
              }
          }
          if(!bNoOuterIntersection)
          {
            Points pIntersection = pOutput[0];
            iOuterIntersect[0] = pIntersection.get<0>();
            iOuterIntersect[1] = pIntersection.get<1>();
          }
         cout<<"\n Outer Intersection Points "<< " "<<iOuterIntersect[0]<< "  "
               <<iOuterIntersect[1];
      // check whether it was case one or two, for case one calculate attractive force
      // for case two get inner clipping points.
          // checking for case
          bool bISCase1 = false;
          if(bIsSourceOuter)
          {
              SubGraph &gTemp = (*gTarget).parent();
              int iTempId = m_bGraphWrapper.getGraphClusterID(gTemp);
              if(iTempId == 999)
              {
                  bISCase1 = true;
              }
          }
          else
          {
              SubGraph &gTemp = (*gSource).parent();
              int iTempId = m_bGraphWrapper.getGraphClusterID(gTemp);
              if(iTempId == 999)
              {
                  bISCase1 = true;
              }
          }
          // call ISCase1
          if(bISCase1 )
          {
              if((!bNoOuterIntersection))
              {
              relaxInterEdges->interEdgesCaseOne(gMaingraph,
                                                 vSource,vTarget,iOuterIntersect);
              }
          }
          else
          {

              cout<<"\n Case 2 detected ";
              // Case 2 here
              // calculate inner clipping points
              if(bIsSourceOuter)           // Source is outer
              {
                  relaxInterEdges->interEdgesCaseTwoSourceOuter(gMaingraph,vSource,vTarget,
                                                                iOuterIntersect,gTarget);
                  // Call cse2source is outer here
              }
              else
              {
                  // Call case2sourceInner

                  relaxInterEdges->interEdgesCaseTwoSourceInner(gMaingraph,vSource,vTarget,
                                                                iOuterIntersect,gTarget);

              }
          }
          // ********************* Case 1 and 2 ends here *******************
      }

      else
      {

          cout<<"\n \n  ***** Case 3 **** Detected";
          bool bIsOverlapCase = false;
          bIsSourceOuter = false ;
          QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
          qSubgraphs.enqueue(&gMaingraph);
          SubGraph* gSubgraph1 = NULL;
          while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
          {
              gSubgraph1 = qSubgraphs.dequeue();
              ChildrenIterator iterChild , iterChildEnd;
              for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
                  iterChild != iterChildEnd;iterChild++)
              {
                  SubGraph* gChildGraph = &(*iterChild);
                  int iClusterId = m_bGraphWrapper.getGraphClusterID(*gChildGraph);
                      if(iClusterId == iTargetGraphId)
                      {
                           gTarget = gChildGraph;
                      }
                      if(iClusterId == iSourceGraphId)
                      {
                          gSource = gChildGraph;
                      }
                  qSubgraphs.enqueue(gChildGraph);
              }
          }
          int iSourceGraphHeight = m_bGraphWrapper.getGraphHeight(*gSource);
          int iSourceTopXLeft = m_bGraphWrapper.getGraphLeftTopCoordX(*gSource);
          int iSourceGraphWidth = m_bGraphWrapper.getGraphWidth(*gSource);
          int iSourceTopYLeft = m_bGraphWrapper.getGraphLeftTopCoordY(*gSource);

          int iTargetGraphHeight = m_bGraphWrapper.getGraphHeight(*gTarget);
          int iTargetTopXLeft = m_bGraphWrapper.getGraphLeftTopCoordX(*gTarget);
          int iTargetGraphWidth = m_bGraphWrapper.getGraphWidth(*gTarget);
          int iTargetTopYLeft = m_bGraphWrapper.getGraphLeftTopCoordY(*gTarget);

          int iInnerIntersectX = 0 ,iInnerIntersectY = 0,iOuterIntersectX = 0,iOuterIntersectY = 0;

          // check overlap
          // 1 . Source outer

          if(((iSourceTopXLeft < iTargetTopXLeft) &&(iSourceTopXLeft + iSourceGraphWidth) >
            (iTargetTopXLeft +iTargetGraphWidth))||((iSourceTopXLeft == iTargetTopXLeft) &&
            (iSourceTopXLeft + iSourceGraphWidth) > (iTargetTopXLeft +iTargetGraphWidth)) ||
            ((iSourceTopXLeft < iTargetTopXLeft) &&(iSourceTopXLeft + iSourceGraphWidth) ==
            (iTargetTopXLeft +iTargetGraphWidth))  )
          {
              // source is outer
              // add check for y axis in both cases.

              if( iSourceTopYLeft < iTargetTopYLeft || iSourceTopYLeft == iTargetTopYLeft )
             {
              bIsOverlapCase = true;
              bIsSourceOuter = true;
              cout<< "\n Overlapping and Source is Outer with id "<< iSourceGraphId;
             }
          }
          else if(((iSourceTopXLeft > iTargetTopXLeft &&(iSourceTopXLeft + iSourceGraphWidth) <
                 (iTargetTopXLeft +iTargetGraphWidth)) || (iSourceTopXLeft == iTargetTopXLeft &&
                 (iSourceTopXLeft + iSourceGraphWidth) < (iTargetTopXLeft +iTargetGraphWidth))||
                 (iSourceTopXLeft > iTargetTopXLeft) &&(iSourceTopXLeft + iSourceGraphWidth) ==
                                    (iTargetTopXLeft +iTargetGraphWidth)))
          {
              if( iTargetTopYLeft < iSourceTopYLeft || iSourceTopYLeft == iTargetTopYLeft )
             {
              // source is inner
              bIsOverlapCase = true;
              bIsSourceOuter = false;
              cout<< "\n Overlapping and Source is Inner with id "<<iTargetGraphId;
             }
          }
           else
          {
               bIsOverlapCase = false;
               cout<< "\n Non  Overlapping ";
              // Non overlapping Case
          }
          // Start
          bParentFound = false;
          if(bIsOverlapCase)
          {
              if(bIsSourceOuter)
              {
                  relaxInterEdges->OverLapCaseSourceOuter(gMaingraph,vSource,
                                                         vTarget,gTarget,iSourceGraphId);
                  // Call OverLapCaseSourceisOuter
              }
              else
              {
                  relaxInterEdges->OverLapCaseSourceInner(gMaingraph,vSource,
                                   vTarget,gSource,iTargetGraphId);
              }
          }
          else
          {
              relaxInterEdges->nonOverLapCase(gMaingraph,vSource,vTarget,gSource,gTarget);
              // call NonOverLapcase
          }

      }

    }

}
void ClusteredSpringEmbedder :: springMoveClusters(SubGraph& gSubgraph)
{

    Reingold *rein = new Reingold();
    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&gSubgraph );
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false)     // Iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        ChildrenIterator iterChild , iterChildEnd;
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children();
            iterChild != iterChildEnd;iterChild++)
        {
            SubGraph* gChildGraph = &(*iterChild);

            // dividing by two for testing , remove later
            int iXClusterdisp = (m_bGraphWrapper.getClusterXDisplacement(*gChildGraph))/2;
            int iYClusterdisp = (m_bGraphWrapper.getClusterYDisplacement(*gChildGraph))/2;
            VertexDescriptor verAffected;
            VertexIterPair   verIter;
            for(tie(verIter.first,verIter.second)=vertices(*gChildGraph)
                        ;verIter.first!=verIter.second;++verIter.first)
            {
                verAffected = *verIter.first;
                int iXCoord = m_bGraphWrapper.getVertexCenterCoordX(verAffected,*gChildGraph);
                int iYCoord = m_bGraphWrapper.getVertexCenterCoordY(verAffected,*gChildGraph);
                m_bGraphWrapper.setVertexCenterCoordX(verAffected,*gChildGraph,
                iXCoord+iXClusterdisp);
                m_bGraphWrapper.setVertexCenterCoordY(verAffected,*gChildGraph,
                                                      iYCoord+iYClusterdisp);
            }
            m_bGraphWrapper.setClusterXDisplacement(*gChildGraph,0);
            m_bGraphWrapper.setClusterYDisplacement(*gChildGraph,0);
            rein->setCompartMentProps(*gChildGraph,10);
        }
    }
}


void ClusteredSpringEmbedder:: springSetVerticesDisplacementZero()
{

    // reset displacement to zero

    VertexDescriptor verAffected;
    VertexIterPair   verIter;
    for(tie(verIter.first,verIter.second)=vertices(m_gMaingraph)
                ;verIter.first!=verIter.second;++verIter.first)
    {

         verAffected = *verIter.first;
         m_bGraphWrapper.setVertXDisp(0,verAffected,m_gMaingraph);
         m_bGraphWrapper.setVertYDisp(0,verAffected,m_gMaingraph);

    }
    int iMainTopY = 0 ,iMainTopX = 0 ;
    iMainTopX = m_bGraphWrapper.getGraphLeftTopCoordX(m_gMaingraph);
    iMainTopY = m_bGraphWrapper.getGraphLeftTopCoordY(m_gMaingraph);


    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&m_gMaingraph );
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
                m_bGraphWrapper.setGraphLeftTopCoordX(iMainTopX,*gChildGraph);
                m_bGraphWrapper.setGraphLeftTopCoordY(iMainTopY,*gChildGraph);
            }
        }
    }

}

void ClusteredSpringEmbedder:: springTestRepelClusterToCluster(SubGraph &gSubgraph)
{
    // make default

    BoostGraphWrapper bGraphWrapper;
    VertexDescriptor verAffected;
    VertexIterPair   verIter;
    QQueue<SubGraph*> qSubgraphs;        // a queue of subgraphs
    qSubgraphs.enqueue(&gSubgraph );
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

                // repel code here
                 typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Points;

                // Find intersection to clusters
                Points pP,pQ,pR,pS,pX,pY,pA,pB,pC,pD;
                typedef boost::geometry::model::segment<Points> Segment;
                std::vector<Points> pPintersect,pAintersect;
                pP.set<0>(dVTopLeftX);
                pP.set<1>(dVTopLeftY);
                pQ.set<0>(dVTopLeftX);
                pQ.set<1>(dVTopLeftY + dVHeight);
                pR.set<0>(dVTopLeftX + dVWidth);
                pR.set<1>(dVTopLeftY + dVHeight);
                pS.set<0>(dVTopLeftX+ dVWidth);
                pS.set<1>(dVTopLeftY);
                pA.set<0>(dUTopLeftX);
                pA.set<1>(dUTopLeftY);
                pB.set<0>(dUTopLeftX);
                pB.set<1>(dUTopLeftY + dUHeight);
                pC.set<0>(dUTopLeftX + dUWidth);
                pC.set<1>(dUTopLeftY + dUHeight);
                pD.set<0>(dUTopLeftX+ dUWidth);
                pD.set<1>(dUTopLeftY);
                pX.set<0>(dVCenterX);
                pX.set<1>(dVCenterY);
                pY.set<0>(dUCenterX);
                pY.set<1>(dUCenterY);
                Segment PQ( pP,pQ );
                Segment QR( pQ,pR );
                Segment RS( pR,pS );
                Segment SP( pS,pP );
                Segment AB( pA,pB );
                Segment BC( pB,pC );
                Segment CD( pC,pD );
                Segment DA( pD,pA );
                Segment Edge (pX,pY);

                bool bNoPIntersection = false,bNoAIntersection = false;
                // Find intersecting edge
                if(boost::geometry::intersects(Edge,PQ))
                {
                    boost::geometry::intersection(Edge,PQ,pPintersect);
                    cout<<" \n  Clip Found while Repelling Clusters to Clusters Edge PQ";
                }
                else if( boost::geometry::intersects(Edge,QR))
                {
                    boost::geometry::intersection(Edge,QR,pPintersect);
                    cout<<" \n Clip Found while Repelling Clusters to Clusters Edge QR";
                }
                else
                {
                    if(boost::geometry::intersects(Edge,RS))
                    {
                        boost::geometry::intersection(Edge,RS,pPintersect);
                        cout<<" \n Clip Found while Repelling Clusters to Clusters Edge RS";
                    }
                    else if(boost::geometry::intersects(Edge,SP))
                    {
                        boost::geometry::intersection(Edge,SP,pPintersect);
                        cout<<" \n Clip Found while Repelling Clusters to Clusters Edge SP";
                    }
                    else
                    {
                        // no intersection , not possible but handle
                        cout<<"\n *** No Intersection in Inner  ***  Check Cluster cordinates" ;
                        bNoPIntersection = true;
                    }
                }
                { // For separating two if else loops
                    if(boost::geometry::intersects(Edge,AB))
                    {
                        boost::geometry::intersection(Edge,AB,pAintersect);
                        cout<<" \n OverLapping Inner Clip Found with Edge AB";
                    }
                    else if( boost::geometry::intersects(Edge,BC))
                    {
                        boost::geometry::intersection(Edge,BC,pAintersect);
                        cout<<" \n OverLapping Inner Clip Found with Edge BC";
                    }
                    else
                    {
                        if(boost::geometry::intersects(Edge,CD))
                        {
                            boost::geometry::intersection(Edge,CD,pAintersect);
                            cout<<" \n OverLapping Inner Clip Found with Edge CD";
                        }
                        else if(boost::geometry::intersects(Edge,DA))
                        {
                            boost::geometry::intersection(Edge,DA,pAintersect);
                            cout<<" \n OverLapping Inner Clip Found with Edge DA";
                        }
                        else
                        {
                            cout<<"\n *** No Inner Intersection in Overlap  ***  Check Cluster cordinates" ;
                            bNoAIntersection = true;
                        }
                    }
                }

                if( !bNoAIntersection && !bNoPIntersection)
                {
                    double  dVIntersectX = 0 , dVIntersectY =0 ,dUIntersectY = 0
                     ,dUIntersectX = 0 ;
                    Points pPIntersection = pPintersect[0];
                    dVIntersectX = pPIntersection.get<0>();
                    dVIntersectY = pPIntersection.get<1>();
                    Points pAIntersection = pAintersect[0];
                    dUIntersectX = pAIntersection.get<0>();
                    dVIntersectY = pAIntersection.get<1>();
                    double dXDisp = dVIntersectX- dUIntersectX;
                    double dYDisp = dVIntersectY - dUIntersectY;
                    double dDist = fabs(dXDisp * dXDisp + dYDisp*dYDisp);
                    //double dDistance = sqrt(dCentreDist);
                    double dDx = 1000000 * dXDisp/dDist;
                    double dDy = 1000000 * dYDisp/dDist;
                    double dLen = dDx*dDx + dDy*dDy;
                    dLen = sqrt(fabs(dLen/2));

                    if(dLen < CLUSTER_TO_CLUSTER_REPULSION_RANGE)  // do it small
                   {


                    int iTotalXDisp = m_bGraphWrapper.getClusterXDisplacement(*gChildGraph);
                    int iTotalYDisp = m_bGraphWrapper.getClusterYDisplacement(*gChildGraph);

                    // limit movement to some extent
                    if(dDx < 0)
                    {
                        dDx = std::max(- UNIT_CLUSTER_DISPLACEMENT,dDx);
                    }
                    else
                    {
                        dDx = std::min(UNIT_CLUSTER_DISPLACEMENT, dDx);
                    }
                    {
                        if(dDy < 0)
                        {
                            dDy = std::max(- UNIT_CLUSTER_DISPLACEMENT,dDy);
                        }
                        else
                        {
                            dDy = std::min(UNIT_CLUSTER_DISPLACEMENT, dDy);
                        }
                    }
                    iTotalXDisp += dDx/dLen*100;
                    iTotalYDisp += dDy/dLen*100;
                    m_bGraphWrapper.setClusterXDisplacement(*gChildGraph,iTotalXDisp);
                    m_bGraphWrapper.setClusterYDisplacement(*gChildGraph,iTotalYDisp);

                }

                }

            }
            qSubgraphs.enqueue(gChildGraph);
        }
    }

}






