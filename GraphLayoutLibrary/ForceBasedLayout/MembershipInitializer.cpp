#include "MembershipInitializer.h"


MembershipInitializer ::MembershipInitializer()
{
}

void MembershipInitializer::initializeMembers(SubGraph& gMainGraphRef)
{
    // Assigns clusterId's and VertexIds in subgraph

    //int iTotalClusters = 0;
    BoostGraphWrapper bGraphWrap;
    VertexIterPair viPair;
    QQueue<SubGraph*> qSubgraphs; //a queue of subgraphs

    int iClusterIDGenerator = 0;
    m_MapSubgraphsId.insert(999,&(gMainGraphRef)); //map of ids and subgraphs

    qSubgraphs.enqueue(&gMainGraphRef);
    SubGraph* gSubgraph1 = NULL;
    while(qSubgraphs.isEmpty() == false) //iterating subgraphs till they exist
    {
        gSubgraph1 = qSubgraphs.dequeue();
        ChildrenIterator iterChild , iterChildEnd;

        //for each of its child
        for(boost::tie(iterChild , iterChildEnd) = gSubgraph1->children(); iterChild != iterChildEnd;iterChild++)
        {
            //iTotalClusters++;
            //cout<<" Cluster No : "<<iTotalClusters;
            iClusterIDGenerator++;
            SubGraph* gChildGraph = &(*iterChild);
            qSubgraphs.enqueue(gChildGraph);
            //assign ClusterID to child
            bGraphWrap.setGraphClusterID(iClusterIDGenerator,*gChildGraph);
            m_MapSubgraphsId.insert(iClusterIDGenerator,gChildGraph);

            //for each of its vertex
            for(tie(viPair.first,viPair.second)=vertices(*gChildGraph); viPair.first!= viPair.second; ++viPair.first)
                //assign ClusterID to vertex
                bGraphWrap.setVertexClusterID(iClusterIDGenerator,*viPair.first,*gChildGraph);
        }
    }
}

QMap<int,SubGraph*> MembershipInitializer::getIdToSubgraphMap()
{
   // returns map ids and subgraphs
    return m_MapSubgraphsId;

}





























