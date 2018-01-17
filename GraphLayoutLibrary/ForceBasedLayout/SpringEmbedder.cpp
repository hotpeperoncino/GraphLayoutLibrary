#include "SpringEmbedder.h"
#include "MembershipInitializer.h"
#include <HierarchicalLayoutGenerator/HierarchicalLayouter.h>
#include <Common/LayoutEnum.h>
#include<Common/GraphType.h>


SpringEmbedder::SpringEmbedder()
{

}

void SpringEmbedder :: getRestrictedBounds(SubGraph& gMainGraph, QVector<int>&
                       vRestBound, QVector<SubGraph *>  vClusters )
{

    // returns a vector of cluster dimensions and rstricted bounds
    BoostGraphWrapper bGraphWrapper;
    QQueue<SubGraph*> qSubgraphs;             // a queue of subgraphs
    qSubgraphs.enqueue(&gMainGraph);
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
            if(boost::num_vertices(*gChildGraph) !=0)
            {
            vRestBound.append(bGraphWrapper.getGraphCenterCoordX(*gChildGraph));
            vRestBound.append(bGraphWrapper.getGraphCenterCoordY(*gChildGraph));
            vRestBound.append( bGraphWrapper.getGraphHeight(*gChildGraph));
            vRestBound.append(bGraphWrapper.getGraphWidth(*gChildGraph));
            vRestBound.append(bGraphWrapper.getGraphLeftTopCoordX(*gChildGraph));
            vRestBound.append(bGraphWrapper.getGraphLeftTopCoordY(*gChildGraph));
            vClusters.append(gChildGraph);
            }


        }

    }




}

void SpringEmbedder :: SpringEmbedderStep(SubGraph& gMainGraph)
{
    BoostGraphWrapper bgraphWrapper;

    int iNumnodes = bgraphWrapper.numVertices(gMainGraph);
    if(iNumnodes > 49 )
    {
        m_dIterations = 100;
    }
    else
    {
        m_dIterations = 50;
    }

    while(m_dIterations >0)
    {
        // relax edges
        SpringRelaxEdges(gMainGraph);
        // repel vertices
        SpringRepelVertices(gMainGraph);
        // move nodes
        SpringMoveNodes(gMainGraph);
        m_dIterations--;

    }

}

void SpringEmbedder :: setDesiredEdgeLength(double dEdgeLength)
{
    m_dDesiredEdgeLength = dEdgeLength;

}

void SpringEmbedder :: setNoOfIterations(double dNumIterations)
{
    m_dIterations = dNumIterations;

}



void SpringEmbedder :: SpringRelaxEdges(SubGraph& gMainGraph)
{
    BoostGraphWrapper  bGraphWrapper;
    EdgeIterator eStart, eEnd;

    for (boost::tie(eStart, eEnd) = edges(gMainGraph); eStart != eEnd; ++eStart)
    {
      VertexDescriptor verV = source(*eStart, gMainGraph);
      VertexDescriptor verU = target(*eStart, gMainGraph);

      double dXCoordV =  bGraphWrapper.getVertexCenterCoordX(verV,gMainGraph);
      double dYCoordV =  bGraphWrapper.getVertexCenterCoordY(verV,gMainGraph);

      double dXCoordU =  bGraphWrapper.getVertexCenterCoordX(verU,gMainGraph);
      double dYCoordU =  bGraphWrapper.getVertexCenterCoordY(verU,gMainGraph);

      double dXDisp =  bGraphWrapper.getVertXDisp(verV,gMainGraph);
      double dYDisp =  bGraphWrapper.getVertYDisp(verV,gMainGraph);

      double dXDisp_U = 0;
      double dYDisp_U = 0;

      dXDisp_U =  bGraphWrapper.getVertXDisp(verU,gMainGraph);
      dYDisp_U =  bGraphWrapper.getVertYDisp(verU,gMainGraph);

      // Logic for vertex size here

      double dXsizeU =  bGraphWrapper.getVertexWidth(verU,gMainGraph);
      double dXsizeV =  bGraphWrapper.getVertexWidth(verV,gMainGraph);

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



      double d_Vx = dXCoordV - dXCoordU;
      double d_Vy = dYCoordV - dYCoordU;

      cout<<"\n Delta X and Y at attractive force :  "<<d_Vy<<" "
            << d_Vx;




      double dDist = fabs(sqrt((d_Vx*d_Vx+d_Vy* d_Vy)));

      cout<<"Distance at attractive force :  "<<dDist;

      if(dDist == 0)
      {
          dDist = 0.0001;
      }




      // desired length = 50
      double dForce =  0.3 *(m_dDesiredEdgeLength - dDist)/dDist ; //0.3 force multiplier
      dForce = dForce * std::pow(0.7,in_degree(verV,gMainGraph)+in_degree(verU,gMainGraph)
                                 + out_degree(verU,gMainGraph)+out_degree(verV,gMainGraph) -2);  // not known 0.3

      double dx = dForce * d_Vx;
      double dy = dForce * d_Vy;


      dXDisp += dx;
      dYDisp += dy;

      dXDisp_U -= dx;
      dYDisp_U -= dy;

     cout<<"\n Attractive Forces Displacement " ;

     cout<<"\n at I X Disp : "<<dXDisp;
     cout<<"  Y Disp : "<<dYDisp;


     cout<<"\n at J  X Disp : "<<dXDisp_U;
     cout<<"   Y Disp : "<<dYDisp_U;

      bGraphWrapper.setVertXDisp(dXDisp,verV,gMainGraph);
      bGraphWrapper.setVertYDisp(dYDisp,verV,gMainGraph);

      bGraphWrapper.setVertXDisp(dXDisp_U,verU,gMainGraph);
      bGraphWrapper.setVertYDisp(dYDisp_U,verU,gMainGraph);


    }


}

void SpringEmbedder :: SpringRepelVertices(SubGraph& gMainGraph)
{
    VertexDescriptor verAffected,verVisitor;
    VertexIterPair   verIter,verIterEnd;

    BoostGraphWrapper bGraphWrapper;
    for(tie(verIter.first,verIter.second)=vertices(gMainGraph)
                ;verIter.first!=verIter.second;++verIter.first)
    {

        double dx = 0;
        double dy = 0;

        double XDisp =0;
        double YDisp = 0;

        double dXCoordV = bGraphWrapper.getVertexCenterCoordX(verAffected,gMainGraph);
        double dYCoordV = bGraphWrapper.getVertexCenterCoordY(verAffected,gMainGraph);

        verAffected = *verIter.first;

        for(tie(verIterEnd.first,verIterEnd.second)=vertices(gMainGraph);
                       verIterEnd.first != verIterEnd.second;++verIterEnd.first)
        {

            verVisitor = *verIterEnd.first;



           double dXCoordU = bGraphWrapper.getVertexCenterCoordX(verVisitor,gMainGraph);
           double dYCoordU = bGraphWrapper.getVertexCenterCoordY(verVisitor,gMainGraph);


                if(verAffected == verVisitor)
               {
                    continue;

               }

                // Logic for vertex size here

                double dXsizeU = bGraphWrapper.getVertexWidth(verVisitor,gMainGraph);
                double dXsizeV = bGraphWrapper.getVertexWidth(verAffected,gMainGraph);

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

                   double dvx = dXCoordV - dXCoordU;
                   double dvy = dYCoordV - dYCoordU;

                   double dDist = fabs((dvx*dvx + dvy* dvy));

                    cout<<"\nDistance at Repulsive force :  "<<dDist;

                    if(dDist == 0 || dDist < 500 )
                    {
                        dDist = std::rand()%300+100;
                    }

                    if(dDist < 100*100*100 )
                      {

                       dx += 1*dvx/dDist;  // factor =1 def works well for 1000
                       dy += 1*dvy/dDist;

                      }
                    else{
                        cout<<"\n Skipping due to distance";
                    }

                    cout<<"\ndx and dy at Repulsive force :  "<<dx<<" "<<dy;

            }

            double dLen = dx*dx+ dy*dy;

            dLen = sqrt(fabs(dLen))/2;

             if(dLen > 0)
             {

                XDisp = bGraphWrapper.getVertXDisp(verAffected,gMainGraph);
                YDisp = bGraphWrapper.getVertYDisp(verAffected,gMainGraph);

                XDisp += (dx/dLen);
                YDisp += (dy/dLen);

                bGraphWrapper.setVertXDisp(XDisp,verAffected,gMainGraph);
                bGraphWrapper.setVertYDisp(YDisp,verAffected,gMainGraph);

                cout<<"\nDisplacement at Repulsive force :  "<<YDisp<<" "
                   << XDisp;
           }



        }


}


void SpringEmbedder :: SpringMoveNodes(SubGraph& gMainGraph)
{
    BoostGraphWrapper bGraphWrapper;
    VertexDescriptor verAffected;
    VertexIterPair   verIter;

    for(tie(verIter.first,verIter.second)=vertices(gMainGraph)
                ;verIter.first!=verIter.second;++verIter.first)
    {

      verAffected = *verIter.first;

      double  dXCoordV = bGraphWrapper.getVertexCenterCoordX
              (verAffected,gMainGraph);
      double dYCoordV = bGraphWrapper.getVertexCenterCoordY
              (verAffected,gMainGraph);

      double dXDisp = bGraphWrapper.getVertXDisp(verAffected,gMainGraph);
      double dYDisp = bGraphWrapper.getVertYDisp(verAffected,gMainGraph);

      bGraphWrapper.setVertXDisp(dXDisp/4,verAffected,gMainGraph);
      bGraphWrapper.setVertYDisp(dYDisp/4,verAffected,gMainGraph);

      dXDisp = std::min(dXDisp,10.0);
      dYDisp = std::min(dYDisp,10.0);

      bGraphWrapper.setVertexCenterCoordX(verAffected,gMainGraph,
                                              dXCoordV+dXDisp);

      bGraphWrapper.setVertexCenterCoordY(verAffected,gMainGraph,
                                              dYCoordV+dYDisp);

    }



}

















