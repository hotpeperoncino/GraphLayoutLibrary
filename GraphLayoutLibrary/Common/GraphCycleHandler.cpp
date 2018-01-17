#include "GraphCycleHandler.h"

GraphCycleHandler::GraphCycleHandler()
{

}

void GraphCycleHandler::detectBackEdges(SubGraph &gGraph, VectorEdgeDescriptor &vectBackEdges)
{

    //create DFS visitors
    back_edge_recorder backEdgeRecorder(vectBackEdges);

    try{
        //Boost DFS
        depth_first_search(gGraph , visitor(backEdgeRecorder));
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {
        throw LayoutException( __FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

    VectorEdgeDescriptor::iterator iterVectEdges;

    //qDebug() << "Printing Back Edges: \n";
//    for(iterVectEdges = vectBackEdges.begin();
//        iterVectEdges != vectBackEdges.end();
//        iterVectEdges++)
//    {
//        //qDebug() << m_BoostGraphWrapper.edgeToQString(*iterVectEdges , gGraph);
//    }

    //qDebug() << "=======================\n";
}

void GraphCycleHandler::reverseEdges(SubGraph &gGraph, VectorEdgeDescriptor &vectBackEdges)
{
    BoostGraphWrapper graphWrapper;
    //Exchange the source and destinition vertices of edge

    VectorEdgeDescriptor::iterator iterEdges;

     qDebug() << "Reversing Edges 11111111111111111111111";
    for(iterEdges = vectBackEdges.begin();
        iterEdges != vectBackEdges.end();
        iterEdges++)
    {
        EdgeDescriptor eEdge = *iterEdges;
        VertexDescriptor vSource = graphWrapper.getEdgeSource(eEdge , gGraph);
        VertexDescriptor vDest = graphWrapper.getEdgeTarget(eEdge , gGraph);

        try{
            //Delete edge
            graphWrapper.removeEdge(vSource , vDest , gGraph);
            qDebug() << "Delete: "<<(int)vSource<<" , "<<(int)vDest;
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
            qDebug() << "Skip: "<<(int)vSource<<" , "<<(int)vDest;
            //Skip edge
            continue;
        }

        //Add edge
        EdgeDescriptor reversedEdge = (graphWrapper.addEdge(vDest , vSource , gGraph)).first;

        qDebug() << "Reverse add: "<<(int)vSource<<" , "<<(int)vDest;

        try{
            //Setting reversed edge property
            graphWrapper.setEdgeReversed(reversedEdge , gGraph , true);
        }
        catch(boost::exception& eBoostException)
        {
            throw *boost::get_error_info<errmsg_info>(eBoostException);
        }
        catch(...)
        {
            throw LayoutException( __FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
        }

        qDebug() << "---------";
    }
    qDebug() << "=================================";
}

bool GraphCycleHandler::doesEdgeCreateCycle(VertexDescriptor vSource, VertexDescriptor vTarget, SubGraph &gGraph)
{
    //This function assumes that already there is no cylce present in the graph.


    //Follow path from target vertex
    //if source vertex is found then cycle
    //else no cycle

    bool bEdgeCreateCycle = false;
    BoostGraphWrapper graphWrapper;

    QQueue<VertexDescriptor> qVerticesReachable;

    qVerticesReachable.enqueue(vTarget);
    while(qVerticesReachable.empty() != false)
    {
        VertexDescriptor vCurrentVertex = qVerticesReachable.dequeue();

        if(vCurrentVertex == vSource)
        {
            bEdgeCreateCycle = true;
            break;
        }

        try
        {
            OutEdgeIterator iterOutEdge , iterEndOutEdge;
            for(boost::tie(iterOutEdge , iterEndOutEdge) = out_edges(vCurrentVertex , gGraph);
                iterOutEdge != iterEndOutEdge;
                iterOutEdge++)
            {
                VertexDescriptor vEdgeTarget = graphWrapper.getEdgeTarget(*iterOutEdge , gGraph);
                qVerticesReachable.enqueue(vEdgeTarget);
            }
        }
        catch(boost::exception& eBoostException)
        {
            throw *boost::get_error_info<errmsg_info>(eBoostException);
        }
        catch(...)
        {
            throw LayoutException( __FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
        }

    }

    return bEdgeCreateCycle;
}
