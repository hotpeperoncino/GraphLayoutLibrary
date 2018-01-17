#include "ForceBasedLayoutGenerator.h"

ForceBasedLayoutGenerator::ForceBasedLayoutGenerator()
{
}

void ForceBasedLayoutGenerator::applyForceBasedLayout(SubGraph &gSubgraph)
{

    // default method works
//    try{
//    typedef square_topology<> Topology;
//    boost::minstd_rand gen;
//    Topology topology(gen,1000.0);
//    Topology::point_type origin;
//    origin[0] = origin[1] = 100.0;
//    Topology::point_difference_type extent;
//    extent[0] = extent[1] = 100.0;
//    fruchterman_reingold_force_directed_layout
//            (gSubgraph,
//             get(vertex_position, gSubgraph),
//             topology,
//             attractive_force(square_distance_attractive_force()).
//             cooling(linear_cooling<double>(100)));
//    //cout<<"Force Based Layout Applied"<<endl;
//    }
//    catch(boost::exception& eBoostException)
//    {
//        //throw *boost::get_error_info<errmsg_info>(eBoostException);
//        //cout<<"Exception in FBL"<<endl;
//    }

//    // for every subgraph apply force based layout
//    ChildrenIterator iterSubgraph, iterSubgraphEnd;
//    for(boost::tie(iterSubgraph,iterSubgraphEnd) = gSubgraph.children();
//        iterSubgraph != iterSubgraphEnd;
//        iterSubgraph++)
//    {
//        typedef square_topology<> Topology;
//        boost::minstd_rand gen;
//        Topology topology(gen,100.0);
//        Topology::point_type origin;
//        origin[0] = origin[1] = 100.0;
//        Topology::point_difference_type extent;
//        extent[0] = extent[1] = 100.0;
//        fruchterman_reingold_force_directed_layout
//                (*iterSubgraph,
//                 get(vertex_position, *iterSubgraph),
//                 topology,
//                 attractive_force(square_distance_attractive_force()).
//                 cooling(linear_cooling<double>(100)));
//        //cout<<"Force Based Layout Applied"<<endl;
//    }

//    VertexIterator itrVertex, itrVertexEnd;
//    for(boost::tie(itrVertex, itrVertexEnd) = vertices(*iterSubgraph);
//        itrVertex != itrVertexEnd;
//        itrVertex++)
//    {
//        int iCoordX = (int)get(vertex_position, *iterSubgraph)[*itrVertex][0];
//        int iCoordY = (int)get(vertex_position, *iterSubgraph)[*itrVertex][0];
//        //cout<<"Force Based Coordinates : "<<iCoordX<<" "<<iCoordY<<endl;
//        VertexDescriptor vVertex = *itrVertex;
//        m_boostGraphWrapper.setVertexCenterCoordX(vVertex, *iterSubgraph, iCoordX);
//        m_boostGraphWrapper.setVertexCenterCoordY(vVertex, *iterSubgraph, iCoordY);
//    }


    // New force algo check
  /*  typedef square_topology<> Topology;
    boost::minstd_rand gen;
    Topology topology(gen,100.0);
    Topology::point_type origin;
    origin[0] = origin[1] = 100.0;
    Topology::point_difference_type extent;
    extent[0] = extent[1] = 100.0;
    std::vector<Topology::point_difference_type> displacements(num_vertices(gSubgraph));
    fruchterman_reingold_force_directed_layout
            (gSubgraph,
             get(vertex_position, gSubgraph),
             topology,
             attractive_force(square_distance_attractive_force()).
             cooling(linear_cooling<double>(100)));  */


    // wORKS FROM HERE

    try{
        typedef square_topology<> Topology;
        boost::minstd_rand gen;
        Topology topology(gen,1000.0);
        Topology::point_type origin;
        origin[0] = origin[1] = 100.0;
        Topology::point_difference_type extent;
        extent[0] = extent[1] = 100.0;
       fruchterman_reingold_force_directed_layout
                (gSubgraph,
                 get(vertex_position, gSubgraph),
                 topology,
                 attractive_force(square_distance_attractive_force()).
                 cooling(linear_cooling<double>(200)));
        cout<<"Force Based Layout Applied"<<endl;



         }
         catch(boost::exception& eBoostException)
        {
             //throw *boost::get_error_info<errmsg_info>(eBoostException);
            cout<<"Exception in FBL"<<endl;
        }

    VertexIterator itrVertex, itrVertexEnd;
      for(boost::tie(itrVertex, itrVertexEnd) = vertices(gSubgraph);
           itrVertex != itrVertexEnd;
           itrVertex++)
       {
           int iCoordX = (int)get(vertex_position, gSubgraph)[*itrVertex][0];
           int iCoordY = (int)get(vertex_position, gSubgraph)[*itrVertex][1];
           cout<<"\nForce Based Coordinates : "<<iCoordX<<" "<<iCoordY<<endl;
           VertexDescriptor vVertex = *itrVertex;
           m_boostGraphWrapper.setVertexCenterCoordX(vVertex, gSubgraph, iCoordX);
           m_boostGraphWrapper.setVertexCenterCoordY(vVertex, gSubgraph, iCoordY);
       }


}
























































