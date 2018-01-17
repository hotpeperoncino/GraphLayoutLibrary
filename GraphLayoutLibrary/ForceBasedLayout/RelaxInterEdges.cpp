#include "RelaxInterEdges.h"

RelaxInterEdges::RelaxInterEdges()
{
}
 void RelaxInterEdges :: interEdgesCaseOne(SubGraph& gMaingraph,
                                           VertexDescriptor& vSource,
                                           VertexDescriptor& vTarget,
                                           int* iOuterIntersect)
 {
     cout<<"\n CASE 1 : ";
     // calculate attractive force here ******
     int iUCoordX , iUCoordY , iVCoordX , iVCoordY;

     iUCoordX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
     iUCoordY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
     iVCoordX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
     iVCoordY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);

     cout<<"\n Initial X and Y Cordinates "<<iUCoordX<<" "<<iUCoordY;
     cout<<"\n Initial X and Y Cordinates "<<iVCoordX<<" "<<iVCoordY;

     double dVx = iVCoordX - iOuterIntersect[0];
     double dVy = iVCoordY - iOuterIntersect[1];

     double dUx =  iUCoordX - iOuterIntersect[0] ;
     double dUy =  iUCoordY - iOuterIntersect[1]  ;

     cout<<"\n Delta X and Y at attractive force :  "<<dVy<<" "
           << dVx;
     cout<<"\n Delta X and Y at attractive force :  "<<dUy<<" "
           << dUx;

     double dDist1 = fabs(sqrt((dVx*dVx+dVy* dVy)));
     double dDist2 = fabs(sqrt((dUx*dUx+dUy* dUy)));
     cout<<"\n Distance at attractive force in Inter edges :  "<<dDist1<<" "<< dDist2;

     // avoid nan
     if(dDist1 == 0)
     {
         dDist1 = 0.0001;
     }
     if(dDist2 == 0)
     {
         dDist2 = 0.0001;
     }
     // desired length = 50
     double dForce1 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist1)/dDist1 ; //0.3 force multiplier
     double dForce2 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist2)/dDist2 ; //0.3 force multiplier
     cout<<"\n Force between interedges "<<dForce1<<"  "  <<dForce2;

     // maintain minimum distance
     if(dDist1 < MINIMUM_INTEREDGE_DISTANCE )   // 60 earlier
     {
         dDist1 = 0.0001;
         dForce1 = 0;
     }
     if(dDist2 < MINIMUM_INTEREDGE_DISTANCE)
     {
         dDist2 = 0.0001;
         dForce2 = 0;
     }
     double dx1 = dForce1 * dVx;
     double dy1 = dForce1 * dVy;
     double dx2 = dForce2 * dUx;
     double dy2 = dForce2 * dUy;
     double dXDispV =  m_bGraphWrapper.getVertXDisp(vTarget,gMaingraph);
     double dYDispV =  m_bGraphWrapper.getVertYDisp(vTarget,gMaingraph);
     double dXDispU = 0;
     double dYDispU = 0;
     dXDispU =  m_bGraphWrapper.getVertXDisp(vSource,gMaingraph);
     dYDispU =  m_bGraphWrapper.getVertYDisp(vSource,gMaingraph);
     dXDispV += dx1;
     dYDispV += dy1;
     dXDispU += dx2;
     dYDispU += dy2;
     cout<<"\n Disp between interedges "<<dx1<<"  "  <<dy1;
     cout<<"\n Disp between interedges "<<dx2<<"  "  <<dy2;
     m_bGraphWrapper.setVertXDisp(dXDispV,vTarget,gMaingraph);
     m_bGraphWrapper.setVertYDisp(dYDispV,vTarget,gMaingraph);
     m_bGraphWrapper.setVertXDisp(dXDispU,vSource,gMaingraph);
     m_bGraphWrapper.setVertYDisp(dYDispU,vSource,gMaingraph);


     cout<<"\n Final X and Y Cordinates "<<iUCoordX<<" "<<iUCoordY;
     cout<<"\n Final X and Y Cordinates "<<iVCoordX<<" "<<iVCoordY;
 }



 void RelaxInterEdges :: interEdgesCaseTwoSourceOuter(SubGraph& gMaingraph,
                                                      VertexDescriptor& vSource,
                                                      VertexDescriptor& vTarget,
                                                      int* iOuterIntersect,
                                                      SubGraph* gTarget)

 {
     LAYOUT_ASSERT( gTarget != NULL,
                   LayoutException(__FUNCTION__
                                   ,LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                   ,"Input and Output graphml path"
                                   ,"springRepelInterEdges"));



     cout<<"\n Source is Outer";
     int iSourceX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
     int iSourceY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
     int iTargetX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
     int iTargetY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);
     int iInnerIntersect[2] = {0,0};  // x and y
     int iHeight,iLeftTopX,iLeftTopY,iWidth;

     // outer compartment of inner vertex
     iHeight = m_bGraphWrapper.getGraphHeight(*gTarget);
     iWidth = m_bGraphWrapper.getGraphWidth(*gTarget);
     iLeftTopX = m_bGraphWrapper.getGraphLeftTopCoordX(*gTarget);
     iLeftTopY = m_bGraphWrapper.getGraphLeftTopCoordY(*gTarget);

     cout<<"\n Inner cluster Compartments";
     cout<<"\n Top Left X " << iLeftTopX;
     cout<<"\n Top Left Y " << iLeftTopY;
     cout<<"\n Height " <<iHeight;
     cout<<"\n Width "<<iWidth;
     // Form 4 piars of P Q R S with Edge e ===> PQ , QR , RS , SP
      typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> Points;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pP;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pQ;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pR;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pS;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pX;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pY;

     typedef boost::geometry::model::segment<Points> Segment;
     std::vector<Points> pInnerIntersect;

     bool bIsNoInnerIntersection = false;

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

     // get different intersection points

     if(boost::geometry::intersects(Edge,PQ))
     {
         boost::geometry::intersection(Edge,PQ,pInnerIntersect);
         cout<<" \n Inner Clip Found with Edge PQ";
     }
     else if( boost::geometry::intersects(Edge,QR))
     {
         boost::geometry::intersection(Edge,QR,pInnerIntersect);
         cout<<" \n Inner Clip Found with Edge QR";
     }
     else
     {
         if(boost::geometry::intersects(Edge,RS))
         {
             boost::geometry::intersection(Edge,RS,pInnerIntersect);
             cout<<" \n Inner Clip Found with Edge RS";
         }
         else if(boost::geometry::intersects(Edge,SP))
         {
             boost::geometry::intersection(Edge,SP,pInnerIntersect);
             cout<<" \n Inner Clip Found with Edge SP";
         }
         else
         {
             // no intersection , not possible but handle
             cout<<"\n *** No Intersection in Inner  ***  Check Cluster cordinates" ;
             bIsNoInnerIntersection = true;
         }
     }

     if(!bIsNoInnerIntersection)
     {
       Points pIntersection = pInnerIntersect[0];
       iInnerIntersect[0] = pIntersection.get<0>();
       iInnerIntersect[1] = pIntersection.get<1>();
     }

     cout<<"\n Inner Intersection Points "<< " "<<iInnerIntersect[0]<< "  "
           <<iInnerIntersect[1];

     int iUCoordX , iUCoordY , iVCoordX , iVCoordY;
     iUCoordX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
     iUCoordY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
     iVCoordX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
     iVCoordY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);

     double dVx = iVCoordX - iInnerIntersect[0];
     double dVy = iVCoordY - iInnerIntersect[1];

     double dUx = iUCoordX - iOuterIntersect[0];
     double dUy = iUCoordY - iOuterIntersect[1];

     cout<<"\n Delta X and Y at attractive force case 2  :  "<<dVy<<" "
           << dVx;

     double dDist1 = fabs(sqrt((dVx*dVx+dVy* dVy)));
     double dDist2 = fabs(sqrt((dUx*dUx+dUy* dUy)));
     cout<<"\n Distance at attractive force in Inter edges case 2:  "<<dDist1;

     if(dDist1 == 0)
     {
         dDist1 = 0.0001;
     }

     if(dDist2 == 0)
     {
         dDist2 = 0.0001;
     }

     // desired length = 100 , adjust force
     double dForce1 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist1)/dDist1 ; //0.3 force multiplier
     double dForce2 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist2)/dDist2 ; //0.3 force multiplier
     cout<<"\n Force between Case 2  interedges "<<dForce1<<"  "  <<dForce2;


     if(dDist1 < MINIMUM_INTEREDGE_DISTANCE )
     {
         dDist1 = 0.0001;
         dForce1 = 0;
     }
     if(dDist2 < MINIMUM_INTEREDGE_DISTANCE)
     {
         dDist2 = 0.0001;
         dForce2 = 0;
     }

     double dx1 = dForce1 * dVx;
     double dy1 = dForce1 * dVy;

     double dx2 = dForce2 * dUx;
     double dy2 = dForce2 * dUy;

     double dXDispV =  m_bGraphWrapper.getVertXDisp(vTarget,gMaingraph);
     double dYDispV =  m_bGraphWrapper.getVertYDisp(vTarget,gMaingraph);

     double dXDispU = 0;
     double dYDispU = 0;

     dXDispU =  m_bGraphWrapper.getVertXDisp(vSource,gMaingraph);
     dYDispU =  m_bGraphWrapper.getVertYDisp(vSource,gMaingraph);

     dXDispV += dx1;
     dYDispV += dy1;

     dXDispU += dx2;
     dYDispU += dy2;

     cout<<"\n Disp between interedges case 2 "<<dx1<<"  "  <<dy1;
     cout<<"\n Disp between interedges case 2 "<<dx2<<"  "  <<dy2;


     m_bGraphWrapper.setVertXDisp(dXDispV,vTarget,gMaingraph);
     m_bGraphWrapper.setVertYDisp(dYDispV,vTarget,gMaingraph);

     m_bGraphWrapper.setVertXDisp(dXDispU,vSource,gMaingraph);
     m_bGraphWrapper.setVertYDisp(dYDispU,vSource,gMaingraph);



 }

 void RelaxInterEdges :: interEdgesCaseTwoSourceInner(SubGraph& gMaingraph,
                                                      VertexDescriptor& vSource,
                                                      VertexDescriptor& vTarget,
                                                      int* iOuterIntersect,
                                                      SubGraph* gSource)
{
     LAYOUT_ASSERT( gSource != NULL,
                   LayoutException(__FUNCTION__
                                   ,LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                   ,"Input and Output graphml path"
                                   ,"springRepelInterEdges"));


     // source is inner
     cout<<"\n Source is Inner";

     // inner clipping for source graph
     int iSourceX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
     int iSourceY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
     int iTargetX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
     int iTargetY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);

     int iInnerIntersect[2];  // x and y

     int iHeight,iLeftTopX,iLeftTopY,iWidth; // outer compartment of inner vertex

     iHeight = m_bGraphWrapper.getGraphHeight(*gSource);
     iWidth = m_bGraphWrapper.getGraphWidth(*gSource);
     iLeftTopX = m_bGraphWrapper.getGraphLeftTopCoordX(*gSource);
     iLeftTopY = m_bGraphWrapper.getGraphLeftTopCoordY(*gSource);



     // Form 4 piars of P Q R S with Edge e ===> PQ , QR , RS , SP
      typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> Points;

     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pP;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pQ;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pR;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pS;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pX;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pY;
     typedef boost::geometry::model::segment<Points> Segment;
     std::vector<Points> pInnerIntersect;

     bool bIsNoInnerIntersection = false;

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

     // get inner clip

     if(boost::geometry::intersects(Edge,PQ))
     {
         boost::geometry::intersection(Edge,PQ,pInnerIntersect);
         cout<<" \n Inner Clip Found with Edge PQ";
     }
     else if( boost::geometry::intersects(Edge,QR))
     {
         boost::geometry::intersection(Edge,QR,pInnerIntersect);
         cout<<" \n Inner Clip Found with Edge QR";
     }
     else
     {
         if(boost::geometry::intersects(Edge,RS))
         {
             boost::geometry::intersection(Edge,RS,pInnerIntersect);
             cout<<" \n Inner Clip Found with Edge RS";
         }
         else if(boost::geometry::intersects(Edge,SP))
         {
             boost::geometry::intersection(Edge,SP,pInnerIntersect);
             cout<<" \n Inner Clip Found with Edge SP";
         }

         else
         {
             // no intersection , not possible but handle
             cout<<"\n *** No Intersection in Inner  ***  Check Cluster cordinates" ;
             bIsNoInnerIntersection = true;
         }
     }
     if(!bIsNoInnerIntersection)
     {
       Points pIntersection = pInnerIntersect[0];
       iInnerIntersect[0] = pIntersection.get<0>();
       iInnerIntersect[1] = pIntersection.get<1>();
     }
     int iUCoordX , iUCoordY , iVCoordX , iVCoordY;
     iUCoordX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
     iUCoordY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
     iVCoordX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
     iVCoordY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);

     double dVx = iVCoordX - iOuterIntersect[0];
     double dVy = iVCoordY - iOuterIntersect[1];

     double dUx = iUCoordX - iInnerIntersect[0];
     double dUy = iUCoordY - iInnerIntersect[1];

     cout<<"\n Delta X and Y at attractive force case 2  :  "<<dVy<<" "
           << dVx;

     double dDist1 = fabs(sqrt((dVx*dVx+dVy* dVy)));
     double dDist2 = fabs(sqrt((dUx*dUx+dUy* dUy)));
     cout<<"\n Distance at attractive force in Inter edges case 2:  "<<dDist1;

     if(dDist1 == 0)
     {
         dDist1 = 0.0001;
     }

     if(dDist2 == 0)
     {
         dDist2 = 0.0001;
     }

     // desired length = 50
     double dForce1 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist1)/dDist1 ; //0.3 force multiplier
     double dForce2 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist2)/dDist2 ; //0.3 force multiplier
     cout<<"\n Force between Case 2  interedges "<<dForce1<<"  "  <<dForce2;
     if(dDist1 < MINIMUM_INTEREDGE_DISTANCE )
     {
         dDist1 = 0.0001;
         dForce1 = 0;
     }

     if(dDist2 < MINIMUM_INTEREDGE_DISTANCE)
     {
         dDist1 = 0.0001;
         dForce2 = 0;
     }

     double dx1 = dForce1 * dVx;
     double dy1 = dForce1 * dVy;

     double dx2 = dForce2 * dUx;
     double dy2 = dForce2 * dUy;

     double dXDispV =  m_bGraphWrapper.getVertXDisp(vTarget,gMaingraph);
     double dYDispV =  m_bGraphWrapper.getVertYDisp(vTarget,gMaingraph);

     double dXDispU = 0;
     double dYDispU = 0;

     dXDispU =  m_bGraphWrapper.getVertXDisp(vSource,gMaingraph);
     dYDispU =  m_bGraphWrapper.getVertYDisp(vSource,gMaingraph);

     dXDispV += dx1;
     dYDispV += dy1;

     dXDispU += dx2;
     dYDispU += dy2;

     cout<<"\n Disp between interedges case 2 "<<dx1<<"  "  <<dy1;
     cout<<"\n Disp between interedges case 2 "<<dx2<<"  "  <<dy2;


     m_bGraphWrapper.setVertXDisp(dXDispV,vTarget,gMaingraph);
     m_bGraphWrapper.setVertYDisp(dYDispV,vTarget,gMaingraph);

     m_bGraphWrapper.setVertXDisp(dXDispU,vSource,gMaingraph);
     m_bGraphWrapper.setVertYDisp(dYDispU,vSource,gMaingraph);
}

 void RelaxInterEdges :: OverLapCaseSourceOuter(SubGraph& gMaingraph,
                                                      VertexDescriptor& vSource,
                                                      VertexDescriptor& vTarget,
                                                      SubGraph* gTarget,
                                                      int iSourceGraphId)

{
     LAYOUT_ASSERT( gTarget != NULL,
                   LayoutException(__FUNCTION__
                                   ,LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                   ,"Input and Output graphml path"
                                   ,"springRepelInterEdges"));


     bool bIsSourceOuter = true;
     bool bIsNull = false;
     bool bParentFound = false;// source is outer
     SubGraph* gTempGraph = (gTarget);
     while( !bParentFound )
     {

        // SubGraph& gOuterParentGraph =  (*gTempGraph).parent();
         SubGraph* gOuterParentGraph =  &((*gTempGraph).parent());
         if(!gOuterParentGraph)
         {
            // null graph received
             bIsNull = true;
             break;
         }
         int iParentGraphId = m_bGraphWrapper.getGraphClusterID(*gOuterParentGraph);
         if(iParentGraphId == iSourceGraphId)
         {
             bParentFound = true;
         }
         else
         {
          //gTempGraph = & gOuterParentGraph;
            gTempGraph = & (*gOuterParentGraph);
         }
     }

     if(!bIsNull)
   {

     int iTargetGraphHeight = m_bGraphWrapper.getGraphHeight(*gTarget);
     int iTargetTopXLeft = m_bGraphWrapper.getGraphLeftTopCoordX(*gTarget);
     int iTargetGraphWidth = m_bGraphWrapper.getGraphWidth(*gTarget);
     int iTargetTopYLeft = m_bGraphWrapper.getGraphLeftTopCoordY(*gTarget);

     int iUCoordX , iUCoordY , iVCoordX , iVCoordY;
     iUCoordX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
     iUCoordY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
     iVCoordX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
     iVCoordY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);

     int iHeight,iLeftTopX,iLeftTopY,iWidth; // outer compartment of inner vertex
     iHeight = m_bGraphWrapper.getGraphHeight(*gTempGraph);
     iWidth = m_bGraphWrapper.getGraphWidth(*gTempGraph);
     iLeftTopX = m_bGraphWrapper.getGraphLeftTopCoordX(*gTempGraph);
     iLeftTopY = m_bGraphWrapper.getGraphLeftTopCoordY(*gTempGraph);
     int iOuterMostGraphID = m_bGraphWrapper.getGraphClusterID(*gTempGraph);

     cout<< "\n Outer Cordinates of Clusters Overlapping Case " ;
     cout<<"\n Outer Graph ID "<<iOuterMostGraphID;
     cout<< " Height "<<iHeight;
     cout<< " Width "<<iWidth;
     cout<< " Lefttop X "<<iLeftTopX;
     cout<< " Lefttop Y "<<iLeftTopY;
     // For inner clipping points
     // Form 4 piars of P Q R S with Edge e ===> PQ , QR , RS , SP and AB , BC ,CD , DA
      typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> Points;

      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pP;
      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pQ;
      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pR;
      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pS;
      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pA;
      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pB;
      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pC;
      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pD;


      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pX;
      boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pY;

     typedef boost::geometry::model::segment<Points> Segment;
     std::vector<Points> pOverlapOuterIntersect , pOverlapInnerIntersect;

     bool bIsNoInnerIntersection = false , bIsNoOuterIntersection = false;

     pP.set<0>(iLeftTopX);
     pP.set<1>(iLeftTopY);
     pQ.set<0>(iLeftTopX);
     pQ.set<1>(iLeftTopY + iHeight);
     pR.set<0>(iLeftTopX + iWidth);
     pR.set<1>(iLeftTopY + iHeight);
     pS.set<0>(iLeftTopX+ iWidth);
     pS.set<1>(iLeftTopY);

     pA.set<0>(iTargetTopXLeft);                     // For inner Clips
     pA.set<1>(iTargetTopYLeft);
     pB.set<0>(iTargetTopXLeft);
     pB.set<1>(iTargetTopYLeft + iTargetGraphHeight);
     pC.set<0>(iTargetTopXLeft + iTargetGraphWidth);
     pC.set<1>(iTargetTopYLeft + iTargetGraphHeight);
     pD.set<0>(iTargetTopXLeft+ iTargetGraphWidth);
     pD.set<1>(iTargetTopYLeft);

     pX.set<0>(iUCoordX);  // Source X and Y
     pX.set<1>(iUCoordY);

     pY.set<0>( iVCoordX); // Target X and Y
     pY.set<1>(iVCoordY);

     Segment PQ( pP,pQ );
     Segment QR( pQ,pR );
     Segment RS( pR,pS );
     Segment SP( pS,pP );
     Segment Edge (pX,pY);
     Segment AB(pA,pB);
     Segment BC(pB,pC);
     Segment CD(pC,pD);
     Segment DA(pD,pA);

     // check outer intersection
     if(boost::geometry::intersects(Edge,PQ))
     {
         boost::geometry::intersection(Edge,PQ,pOverlapOuterIntersect);
         cout<<" \n OverLapping Outer Clip Found with Edge PQ";
     }
     else if( boost::geometry::intersects(Edge,QR))
     {
         boost::geometry::intersection(Edge,QR,pOverlapOuterIntersect);
         cout<<" \n OverLapping Outer Clip Found with Edge QR";
     }
     else
     {
         if(boost::geometry::intersects(Edge,RS))
         {
             boost::geometry::intersection(Edge,RS,pOverlapOuterIntersect);
             cout<<" \n OverLapping Outer Clip Found with Edge RS";
         }
         else if(boost::geometry::intersects(Edge,SP))
         {
             boost::geometry::intersection(Edge,SP,pOverlapOuterIntersect);
             cout<<" \n OverLapping Outer Clip Found with Edge SP";
         }
         else
         {
             // no intersection , not possible but handle
             cout<<"\n *** No Intersection in Overlap  ***  Check Cluster cordinates" ;
             bIsNoOuterIntersection = true;
         }
     }
     // check inner intersection
     { // For separating two if else loops

         if(boost::geometry::intersects(Edge,AB))
         {
             boost::geometry::intersection(Edge,AB,pOverlapInnerIntersect);
             cout<<" \n OverLapping Inner Clip Found with Edge AB";
         }
         else if( boost::geometry::intersects(Edge,BC))
         {
             boost::geometry::intersection(Edge,BC,pOverlapInnerIntersect);
             cout<<" \n OverLapping Inner Clip Found with Edge BC";
         }
         else
         {
             if(boost::geometry::intersects(Edge,CD))
             {

                 boost::geometry::intersection(Edge,CD,pOverlapInnerIntersect);
                 cout<<" \n OverLapping Inner Clip Found with Edge CD";
             }
             else if(boost::geometry::intersects(Edge,DA))
             {

                 boost::geometry::intersection(Edge,DA,pOverlapInnerIntersect);
                 cout<<" \n OverLapping Inner Clip Found with Edge DA";
             }
             else
             {
                 // no intersection , not possible but handle
                 cout<<"\n *** No Inner Intersection in Overlap  ***  Check Cluster cordinates" ;
                 bIsNoInnerIntersection = true;
             }
         }
     }
     int iOuterIntersectX,iOuterIntersectY,iInnerIntersectX,iInnerIntersectY;
     if(!bIsNoOuterIntersection)
     {
       Points pIntersection = pOverlapOuterIntersect[0];
        iOuterIntersectX = pIntersection.get<0>();
        iOuterIntersectY = pIntersection.get<1>();
     }

     if(!bIsNoInnerIntersection)
     {
       Points pIntersection = pOverlapInnerIntersect[0];
        iInnerIntersectX = pIntersection.get<0>();
        iInnerIntersectY = pIntersection.get<1>();
     }
     // attract source with outer and target with inner
     if(!bIsNoOuterIntersection && !bIsNoInnerIntersection )
     {
         // attract only if both intersections are found
         double dVx = iVCoordX - iInnerIntersectX;
         double dVy = iVCoordY - iInnerIntersectY;
         double dUx = iUCoordX - iOuterIntersectX;
         double dUy = iUCoordY - iOuterIntersectY;

         cout<<"\n Delta X and Y at attractive force case 2  :  "<<dVy<<" "
               << dVx;
         double dDist1 = fabs(sqrt((dVx*dVx+dVy* dVy)));
         double dDist2 = fabs(sqrt((dUx*dUx+dUy* dUy)));
         cout<<"\n Distance at attractive force in Inter edges case 2:  "<<dDist1;

         if(dDist1 == 0)
         {
             dDist1 = 0.0001;
         }

         if(dDist2 == 0)
         {
             dDist2 = 0.0001;
         }

         // desired length = 50
         double dForce1 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist1)/dDist1 ; //0.3 force multiplier
         double dForce2 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist2)/dDist2 ; //0.3 force multiplier
         cout<<"\n Force between Case 2  interedges "<<dForce1<<"  "  <<dForce2;
         if(dDist1 < MINIMUM_INTEREDGE_DISTANCE )
         {
             dDist1 = 0.0001;
             dForce1 = 0;
         }
         if(dDist2 < MINIMUM_INTEREDGE_DISTANCE)
         {
             dDist2 = 0.0001;
             dForce2 = 0;
         }

         // get displacement

         double dx1 = dForce1 * dVx;
         double dy1 = dForce1 * dVy;
         double dx2 = dForce2 * dUx;
         double dy2 = dForce2 * dUy;

         double dXDispV =  m_bGraphWrapper.getVertXDisp(vTarget,gMaingraph);
         double dYDispV =  m_bGraphWrapper.getVertYDisp(vTarget,gMaingraph);
         double dXDispU = 0;
         double dYDispU = 0;
         dXDispU =  m_bGraphWrapper.getVertXDisp(vSource,gMaingraph);
         dYDispU =  m_bGraphWrapper.getVertYDisp(vSource,gMaingraph);

         dXDispV += dx1;
         dYDispV += dy1;
         dXDispU += dx2;
         dYDispU += dy2;

         cout<<"\n Disp between interedges case 2 "<<dx1<<"  "  <<dy1;
         cout<<"\n Disp between interedges case 2 "<<dx2<<"  "  <<dy2;
         m_bGraphWrapper.setVertXDisp(dXDispV,vTarget,gMaingraph);
         m_bGraphWrapper.setVertYDisp(dYDispV,vTarget,gMaingraph);
         m_bGraphWrapper.setVertXDisp(dXDispU,vSource,gMaingraph);
         m_bGraphWrapper.setVertYDisp(dYDispU,vSource,gMaingraph);

     }

  }

}

 void RelaxInterEdges :: OverLapCaseSourceInner(SubGraph& gMaingraph,
                                                      VertexDescriptor& vSource,
                                                      VertexDescriptor& vTarget,
                                                      SubGraph* gSource,
                                                      int iTargetGraphId)

{
     LAYOUT_ASSERT( gSource != NULL,
                   LayoutException(__FUNCTION__
                                   ,LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                   ,"Input and Output graphml path"
                                   ,"springRepelInterEdges"));


     cout<<"\n Target is outer ";
     // Source is inner and target is outer
     bool bIsSourceOuter = false, bParentFound = false;        // source is inner
     SubGraph* gTempGraph = (gSource);
     bool bIsNull = false;
     while( !bParentFound )
     {

         SubGraph* gOuterParentGraph =  &((*gTempGraph).parent());
         if(!gOuterParentGraph)
         {
            // null graph received
             bIsNull = true;
             break;
         }
         int iParentGraphId = m_bGraphWrapper.getGraphClusterID(*gOuterParentGraph);
         if(iParentGraphId == iTargetGraphId)
         {
             bParentFound = true;

         }
         else
         {
          gTempGraph = & (*gOuterParentGraph); // changed goutergraph to pointer
         }
     }

     if(!bIsNull)
    {

     int iSourceGraphHeight = m_bGraphWrapper.getGraphHeight(*gSource);
     int iSourceTopXLeft = m_bGraphWrapper.getGraphLeftTopCoordX(*gSource);
     int iSourceGraphWidth = m_bGraphWrapper.getGraphWidth(*gSource);
     int iSourceTopYLeft = m_bGraphWrapper.getGraphLeftTopCoordY(*gSource);

     int iUCoordX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
     int iUCoordY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
     int iVCoordX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
     int iVCoordY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);
     int iHeight,iLeftTopX,iLeftTopY,iWidth;
     // outer compartment of inner vertex
     iHeight = m_bGraphWrapper.getGraphHeight(*gTempGraph);
     iWidth = m_bGraphWrapper.getGraphWidth(*gTempGraph);
     iLeftTopX = m_bGraphWrapper.getGraphLeftTopCoordX(*gTempGraph);
     iLeftTopY = m_bGraphWrapper.getGraphLeftTopCoordY(*gTempGraph);
     int iOuterMostGraphID = m_bGraphWrapper.getGraphClusterID(*gTempGraph);

     cout<< "\n Outer Cordinates of Clusters Overlapping Case " ;
     cout<<"\n Outer Graph ID "<<iOuterMostGraphID;
     cout<< " Height "<<iHeight;
     cout<< " Width "<<iWidth;
     cout<< " Lefttop X "<<iLeftTopX;
     cout<< " Lefttop Y "<<iLeftTopY;

     // For inner clipping points
     // Form 4 piars of P Q R S with Edge e ===> PQ , QR , RS , SP and AB , BC ,CD , DA
     typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> Points;

     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pP;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pQ;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pR;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pS;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pA;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pB;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pC;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pD;

     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pX;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pY;
     typedef boost::geometry::model::segment<Points> Segment;
     std::vector<Points> pOverlapOuterIntersect , pOverlapInnerIntersect;

     bool bIsNoInnerIntersection = false , bIsNoOuterIntersection = false;
     pP.set<0>(iLeftTopX);
     pP.set<1>(iLeftTopY);
     pQ.set<0>(iLeftTopX);
     pQ.set<1>(iLeftTopY + iHeight);
     pR.set<0>(iLeftTopX + iWidth);
     pR.set<1>(iLeftTopY + iHeight);
     pS.set<0>(iLeftTopX+ iWidth);
     pS.set<1>(iLeftTopY);


     pA.set<0>(iSourceTopXLeft);                     // For inner Clips
     pA.set<1>(iSourceTopYLeft);
     pB.set<0>(iSourceTopXLeft);
     pB.set<1>(iSourceTopYLeft + iSourceGraphHeight);
     pC.set<0>(iSourceTopXLeft + iSourceGraphWidth);
     pC.set<1>(iSourceTopYLeft + iSourceGraphHeight);
     pD.set<0>(iSourceTopXLeft + iSourceGraphWidth);
     pD.set<1>(iSourceTopYLeft);

     pX.set<0>(iUCoordX);  // Source X and Y
     pX.set<1>(iUCoordY);
     pY.set<0>( iVCoordX); // Target X and Y
     pY.set<1>(iVCoordY);

     Segment PQ( pP,pQ );
     Segment QR( pQ,pR );
     Segment RS( pR,pS );
     Segment SP( pS,pP );
     Segment Edge (pX,pY);
     Segment AB(pA,pB);
     Segment BC(pB,pC);
     Segment CD(pC,pD);
     Segment DA(pD,pA);

     // check outer intersection
     if(boost::geometry::intersects(Edge,PQ))
     {
         boost::geometry::intersection(Edge,PQ,pOverlapOuterIntersect);
         cout<<" \n OverLapping Outer Clip Found with Edge PQ";
     }
     else if( boost::geometry::intersects(Edge,QR))
     {
         boost::geometry::intersection(Edge,QR,pOverlapOuterIntersect);
         cout<<" \n OverLapping Outer Clip Found with Edge QR";
     }
     else
     {
         if(boost::geometry::intersects(Edge,RS))
         {
             boost::geometry::intersection(Edge,RS,pOverlapOuterIntersect);
             cout<<" \n OverLapping Outer Clip Found with Edge RS";
         }
         else if(boost::geometry::intersects(Edge,SP))
         {
             boost::geometry::intersection(Edge,SP,pOverlapOuterIntersect);
             cout<<" \n OverLapping Outer Clip Found with Edge SP";
         }
         else
         {
             // no intersection , not possible but handle
             cout<<"\n *** No Intersection in Overlap  ***  Check Cluster cordinates" ;
             bIsNoOuterIntersection = true;
         }
     }
     // check inner intersection
     { // For separating two if else loops

         if(boost::geometry::intersects(Edge,AB))
         {
             boost::geometry::intersection(Edge,AB,pOverlapInnerIntersect);
             cout<<" \n OverLapping Inner Clip Found with Edge AB";
         }
         else if( boost::geometry::intersects(Edge,BC))
         {
             boost::geometry::intersection(Edge,BC,pOverlapInnerIntersect);
             cout<<" \n OverLapping Inner Clip Found with Edge BC";
         }
         else
         {
             if(boost::geometry::intersects(Edge,CD))
             {
                 boost::geometry::intersection(Edge,CD,pOverlapInnerIntersect);
                 cout<<" \n OverLapping Inner Clip Found with Edge CD";
             }
             else if(boost::geometry::intersects(Edge,DA))
             {
                 boost::geometry::intersection(Edge,DA,pOverlapInnerIntersect);
                 cout<<" \n OverLapping Inner Clip Found with Edge DA";
             }
             else
             {
                 // no intersection , not possible but handle
                 cout<<"\n *** No Inner Intersection in Overlap  ***  Check Cluster cordinates" ;
                 bIsNoInnerIntersection = true;
             }
         }
     }
     int iOuterIntersectX,iOuterIntersectY,iInnerIntersectX,iInnerIntersectY;
     if(!bIsNoOuterIntersection)
     {
       Points pIntersection = pOverlapOuterIntersect[0];
       iOuterIntersectX = pIntersection.get<0>();
       iOuterIntersectY = pIntersection.get<1>();
     }
     if(!bIsNoInnerIntersection)
     {
       Points pIntersection = pOverlapInnerIntersect[0];
       iInnerIntersectX = pIntersection.get<0>();
       iInnerIntersectY = pIntersection.get<1>();
     }
     // attract source with inner and target with outer
     if(!bIsNoOuterIntersection && !bIsNoInnerIntersection )
     {
         // attract only if both intersections are found

         double dVx = iVCoordX - iOuterIntersectX;
         double dVy = iVCoordY - iOuterIntersectY;
         double dUx = iUCoordX - iInnerIntersectX;
         double dUy = iUCoordY - iInnerIntersectY;
         cout<<"\n Delta X and Y at attractive force case 2  :  "<<dVy<<" "
               << dVx;
         double dDist1 = fabs(sqrt((dVx*dVx+dVy* dVy)));
         double dDist2 = fabs(sqrt((dUx*dUx+dUy* dUy)));
         cout<<"\n Distance at attractive force in Inter edges case 2:  "<<dDist1;
         if(dDist1 == 0)
         {
             dDist1 = 0.0001;
         }

         if(dDist2 == 0)
         {
             dDist2 = 0.0001;
         }

         // desired length = 50
         double dForce1 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist1)/dDist1 ; //0.3 force multiplier
         double dForce2 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist2)/dDist2 ; //0.3 force multiplier

         cout<<"\n Force between Case 2  interedges "<<dForce1<<"  "  <<dForce2;
         if(dDist1 < MINIMUM_INTEREDGE_DISTANCE )
         {
             dDist1 = 0.0001;
             dForce1 = 0;
         }
         if(dDist2 < MINIMUM_INTEREDGE_DISTANCE)
         {
             dDist2 = 0.0001;
             dForce2 = 0;
         }
         double dx1 = dForce1 * dVx;
         double dy1 = dForce1 * dVy;
         double dx2 = dForce2 * dUx;
         double dy2 = dForce2 * dUy;

         double dXDispV =  m_bGraphWrapper.getVertXDisp(vTarget,gMaingraph);
         double dYDispV =  m_bGraphWrapper.getVertYDisp(vTarget,gMaingraph);
         double dXDispU = 0;
         double dYDispU = 0;
         dXDispU =  m_bGraphWrapper.getVertXDisp(vSource,gMaingraph);
         dYDispU =  m_bGraphWrapper.getVertYDisp(vSource,gMaingraph);
         dXDispV += dx1;
         dYDispV += dy1;
         dXDispU += dx2;
         dYDispU += dy2;

         cout<<"\n Disp between interedges case 2 "<<dx1<<"  "  <<dy1;
         cout<<"\n Disp between interedges case 2 "<<dx2<<"  "  <<dy2;
         m_bGraphWrapper.setVertXDisp(dXDispV,vTarget,gMaingraph);
         m_bGraphWrapper.setVertYDisp(dYDispV,vTarget,gMaingraph);
         m_bGraphWrapper.setVertXDisp(dXDispU,vSource,gMaingraph);
         m_bGraphWrapper.setVertYDisp(dYDispU,vSource,gMaingraph);
     }
   }

}


 void RelaxInterEdges :: nonOverLapCase(SubGraph& gMaingraph,
                                        VertexDescriptor& vSource,
                                        VertexDescriptor& vTarget,
                                        SubGraph* gSource, SubGraph* gTarget)
{

     LAYOUT_ASSERT( gSource != NULL,
                   LayoutException(__FUNCTION__
                                   ,LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                   ,"Input and Output graphml path"
                                   ,"springRepelInterEdges"));

     LAYOUT_ASSERT( gTarget != NULL,
                   LayoutException(__FUNCTION__
                                   ,LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE
                                   ,"Input and Output graphml path"
                                   ,"springRepelInterEdges"));

     int iSourceGraphHeight = m_bGraphWrapper.getGraphHeight(*gSource);
     int iSourceTopXLeft = m_bGraphWrapper.getGraphLeftTopCoordX(*gSource);
     int iSourceGraphWidth = m_bGraphWrapper.getGraphWidth(*gSource);
     int iSourceTopYLeft = m_bGraphWrapper.getGraphLeftTopCoordY(*gSource);

     int iTargetGraphHeight = m_bGraphWrapper.getGraphHeight(*gTarget);
     int iTargetTopXLeft = m_bGraphWrapper.getGraphLeftTopCoordX(*gTarget);
     int iTargetGraphWidth = m_bGraphWrapper.getGraphWidth(*gTarget);
     int iTargetTopYLeft = m_bGraphWrapper.getGraphLeftTopCoordY(*gTarget);

     int iSourceXIntersect =0 , iSourceYIntersect = 0 , iTargetYIntersect =0 ,
         iTargetXIntersect = 0;
     // Non overlapping code here
     // For inner clipping points of both subgraphs
     // Form 4 piars of P Q R S with Edge e ===> PQ , QR , RS , SP and AB , BC ,CD , DA
      typedef boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian> Points;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pP;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pQ;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pR;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pS;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pA;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pB;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pC;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pD;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pX;
     boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>pY;
     typedef boost::geometry::model::segment<Points> Segment;
     std::vector<Points> pSourceIntersect , pTargetIntersect;
     bool bIsNoInnerIntersection = false , bIsNoOuterIntersection = false;


     pP.set<0>(iSourceTopXLeft);
     pP.set<1>(iSourceTopYLeft);                      // For Source
     pQ.set<0>(iSourceTopXLeft);
     pQ.set<1>(iSourceTopYLeft+ iSourceGraphHeight);
     pR.set<0>(iSourceTopXLeft+ iSourceGraphWidth);
     pR.set<1>(iSourceTopYLeft + iSourceGraphHeight);
     pS.set<0>(iSourceTopXLeft+ iSourceGraphWidth);
     pS.set<1>(iSourceTopYLeft);


     pA.set<0>(iTargetTopXLeft);                     // For Target
     pA.set<1>(iTargetTopYLeft);
     pB.set<0>(iTargetTopXLeft);
     pB.set<1>(iTargetTopYLeft + iTargetGraphHeight);
     pC.set<0>(iTargetTopXLeft + iTargetGraphWidth);
     pC.set<1>(iTargetTopYLeft + iTargetGraphHeight);
     pD.set<0>(iTargetTopXLeft + iTargetGraphWidth);
     pD.set<1>(iTargetTopYLeft);


     int iUCoordX = m_bGraphWrapper.getVertexCenterCoordX(vSource,gMaingraph);
     int iUCoordY = m_bGraphWrapper.getVertexCenterCoordY(vSource,gMaingraph);
     int iVCoordX = m_bGraphWrapper.getVertexCenterCoordX(vTarget,gMaingraph);
     int iVCoordY = m_bGraphWrapper.getVertexCenterCoordY(vTarget,gMaingraph);


     pX.set<0>(iUCoordX);  // Source X and Y
     pX.set<1>(iUCoordY);
     pY.set<0>(iVCoordX); // Target X and Y
     pY.set<1>(iVCoordY);
     Segment PQ( pP,pQ );
     Segment QR( pQ,pR );
     Segment RS( pR,pS );
     Segment SP( pS,pP );
     Segment Edge (pX,pY);
     Segment AB(pA,pB);
     Segment BC(pB,pC);
     Segment CD(pC,pD);
     Segment DA(pD,pA);

     {
     if(boost::geometry::intersects(Edge,PQ))
     {
         boost::geometry::intersection(Edge,PQ,pSourceIntersect);
         cout<<" \n OverLapping Outer Clip Found with Edge PQ";
     }
     else if( boost::geometry::intersects(Edge,QR))
     {
         boost::geometry::intersection(Edge,QR,pSourceIntersect);
         cout<<" \n OverLapping Outer Clip Found with Edge QR";
     }
     else
     {
         if(boost::geometry::intersects(Edge,RS))
         {
             // get insetersect points
             boost::geometry::intersection(Edge,RS,pSourceIntersect);
             cout<<" \n OverLapping Outer Clip Found with Edge RS";

         }
         else if(boost::geometry::intersects(Edge,SP))
         {
             boost::geometry::intersection(Edge,SP,pSourceIntersect);
             cout<<" \n OverLapping Outer Clip Found with Edge SP";
         }
         else
         {
             // no intersection , not possible but handle
             cout<<"\n *** No Intersection in Overlap  ***  Check Cluster cordinates" ;
             bIsNoOuterIntersection = true;
         }
       }
     }
     { // For separating two if else loops

         if(boost::geometry::intersects(Edge,AB))
         {
             boost::geometry::intersection(Edge,AB,pTargetIntersect);
             cout<<" \n OverLapping Inner Clip Found with Edge AB";

         }
         else if( boost::geometry::intersects(Edge,BC))
         {
             boost::geometry::intersection(Edge,BC,pTargetIntersect);
             cout<<" \n OverLapping Inner Clip Found with Edge BC";
         }
         else
         {
             if(boost::geometry::intersects(Edge,CD))
             {
                 boost::geometry::intersection(Edge,CD,pTargetIntersect);
                 cout<<" \n OverLapping Inner Clip Found with Edge CD";
             }
             else if(boost::geometry::intersects(Edge,DA))
             {
                 boost::geometry::intersection(Edge,DA,pTargetIntersect);
                 cout<<" \n OverLapping Inner Clip Found with Edge DA";
             }
             else
             {
                 // no intersection , not possible but handle
                 cout<<"\n *** No Inner Intersection in Overlap  ***  Check Cluster cordinates" ;
                 bIsNoInnerIntersection = true;
             }

         }
     }
     // attract here
     if(!bIsNoOuterIntersection)
     {
       Points pIntersection = pSourceIntersect[0];
       iSourceXIntersect = pIntersection.get<0>();
       iSourceYIntersect = pIntersection.get<1>();
     }

     if(!bIsNoInnerIntersection)
     {
       Points pIntersection = pTargetIntersect[0];
       iTargetXIntersect = pIntersection.get<0>();
       iTargetYIntersect = pIntersection.get<1>();
     }
     if(!bIsNoOuterIntersection && !bIsNoInnerIntersection )
     {
         // attract only if both intersections are found

         double dVx = iVCoordX - iTargetXIntersect;
         double dVy = iVCoordY - iTargetYIntersect;

         double dUx = iUCoordX - iSourceXIntersect;
         double dUy = iUCoordY - iSourceYIntersect;

         cout<<"\n Delta X and Y at attractive force case 2  :  "<<dVy<<" "
               << dVx;

         double dDist1 = fabs(sqrt((dVx*dVx+dVy* dVy)));
         double dDist2 = fabs(sqrt((dUx*dUx+dUy* dUy)));
         cout<<"\n Distance at attractive force in Inter edges case 2:  "<<dDist1;

         if(dDist1 == 0)
         {
             dDist1 = 0.0001;
         }

         if(dDist2 == 0)
         {
             dDist2 = 0.0001;
         }

         // desired length = 50 // force = 0.3
         double dForce1 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist1)/dDist1 ; //0.3 force multiplier
         double dForce2 =  INTEREDGES_ATTRACTION_MULTIPLIER *(INTEREDGES_DEFAULT_LENGTH - dDist2)/dDist2 ; //0.3 force multiplier
         cout<<"\n Force between Case 2  interedges "<<dForce1<<"  "  <<dForce2;


         if(dDist1 < MINIMUM_INTEREDGE_DISTANCE )
         {
             dDist1 = 0.0001;
             dForce1 = 0;
         }

         if(dDist2 < MINIMUM_INTEREDGE_DISTANCE)
         {
             dDist2 = 0.0001;
             dForce2 = 0;
         }

         double dx1 = dForce1 * dVx;
         double dy1 = dForce1 * dVy;

         double dx2 = dForce2 * dUx;
         double dy2 = dForce2 * dUy;

         double dXDispV =  m_bGraphWrapper.getVertXDisp(vTarget,gMaingraph);
         double dYDispV =  m_bGraphWrapper.getVertYDisp(vTarget,gMaingraph);

         double dXDispU = 0;
         double dYDispU = 0;

         dXDispU =  m_bGraphWrapper.getVertXDisp(vSource,gMaingraph);
         dYDispU =  m_bGraphWrapper.getVertYDisp(vSource,gMaingraph);

         dXDispV += dx1;
         dYDispV += dy1;

         dXDispU += dx2;
         dYDispU += dy2;

         cout<<"\n Disp between interedges case 2 "<<dx1<<"  "  <<dy1;
         cout<<"\n Disp between interedges case 2 "<<dx2<<"  "  <<dy2;


         m_bGraphWrapper.setVertXDisp(dXDispV,vTarget,gMaingraph);
         m_bGraphWrapper.setVertYDisp(dYDispV,vTarget,gMaingraph);
         m_bGraphWrapper.setVertXDisp(dXDispU,vSource,gMaingraph);
         m_bGraphWrapper.setVertYDisp(dYDispU,vSource,gMaingraph);

     }

}

























