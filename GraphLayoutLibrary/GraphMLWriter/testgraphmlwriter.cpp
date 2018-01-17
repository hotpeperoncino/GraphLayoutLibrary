#include "testgraphmlwriter.h"
//#include "graphmlgenerator.h"

TestGraphmlWriter::TestGraphmlWriter()
{
}

void TestGraphmlWriter::test(int iExtraVertices)
{
    //select file
    m_sFileName = "boostGraph.graphml";

    QFile file(m_sFileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        cout<<"File could not be open. Exiting.";
        return;
    }

    //provide boost graph

    cout << "Generating graph.." << endl;

    generateMainGraph2(iExtraVertices);

    int iTotalNodes = num_vertices(m_gMainGraph);
    int iTotalEdges = num_edges(m_gMainGraph);

    cout<< "Graph,\nTotal Nodes: " << iTotalNodes <<
           " Total Edges: " << iTotalEdges << endl;
    QTime t;
    t.currentTime();
    t.start();

    //write graphml
    //GraphMLGenerator graphmlGenerator;
    //graphmlGenerator.writeGraphml(m_gMainGraph, &file);

    cout<<"Graphml generation time duration: "<<t.elapsed() <<" ms."<<endl;

}

void TestGraphmlWriter::generateMainGraph1()
{
    Graph& subGraph1 = m_gMainGraph.create_subgraph();
    Graph& subGraph2 = subGraph1.create_subgraph();

    Graph::vertex_descriptor v0 = add_vertex(m_gMainGraph);
    Graph::vertex_descriptor v1 = add_vertex(m_gMainGraph);
    Graph::vertex_descriptor v2 = add_vertex(m_gMainGraph);
    Graph::vertex_descriptor v3 = add_vertex(m_gMainGraph);
    Graph::vertex_descriptor v4 = add_vertex(m_gMainGraph);
    Graph::vertex_descriptor v5 = add_vertex(m_gMainGraph);
    Graph::vertex_descriptor v6 = add_vertex(m_gMainGraph);
    Graph::vertex_descriptor v7 = add_vertex(m_gMainGraph);
    m_gMainGraph[v0].sId  = "S_1";
    m_gMainGraph[v1].sId  = "S_2";
    m_gMainGraph[v2].sId  = "S_3";
    m_gMainGraph[v3].sId  = "S_4";
    m_gMainGraph[v4].sId  = "S_5";
    m_gMainGraph[v5].sId  = "S_6";
    m_gMainGraph[v6].sId  = "S_7";
    m_gMainGraph[v7].sId  = "S_8";


    Graph::vertex_descriptor v8 = add_vertex(subGraph1);
    Graph::vertex_descriptor v9 = add_vertex(subGraph1);
    Graph::vertex_descriptor v10 = add_vertex(subGraph1);
    Graph::vertex_descriptor v11 = add_vertex(subGraph1);
    Graph::vertex_descriptor v12 = add_vertex(subGraph1);
    Graph::vertex_descriptor v13 = add_vertex(subGraph1);
    subGraph1[v8].sId = "S_9";
    subGraph1[v9].sId = "S_10";
    subGraph1[v10].sId = "S_11";
    subGraph1[v11].sId = "S_12";
    subGraph1[v12].sId = "S_13";
    subGraph1[v13].sId = "S_14";

    // add edge returns pair< edge_descriptor , bool >
    auto e0 = add_edge(v0,v1,m_gMainGraph);
    auto e1 = add_edge(v0,v2,m_gMainGraph);
    auto e2 = add_edge(v1,v3,m_gMainGraph);
    auto e3 = add_edge(v1,v4,m_gMainGraph);
    auto e4 = add_edge(v3,v5,m_gMainGraph);
    auto e5 = add_edge(v3,v6,m_gMainGraph);
    auto e6 = add_edge(v4,v7,m_gMainGraph);


    auto e7 = add_edge(v2,subGraph1.local_to_global(v8),m_gMainGraph); //intergraph edge

    m_gMainGraph[e0.first].sId = "E_1";
    m_gMainGraph[e1.first].sId = "E_2";
    m_gMainGraph[e2.first].sId = "E_3";
    m_gMainGraph[e3.first].sId = "E_4";
    m_gMainGraph[e4.first].sId = "E_5";
    m_gMainGraph[e5.first].sId = "E_6";
    m_gMainGraph[e6.first].sId = "E_7";
    m_gMainGraph[e7.first].sId = "E_8";

    auto e8 = add_edge(v8,v9,subGraph1);
    auto e9 = add_edge(v8,v10,subGraph1);
    auto e10 = add_edge(v9,v11,subGraph1);
    auto e11 =  add_edge(v9,v12,subGraph1);
    auto e12 = add_edge(v10,v13,subGraph1);

    subGraph1[e8.first].sId = "E_9";
    subGraph1[e9.first].sId = "E_10";
    subGraph1[e10.first].sId = "E_11";
    subGraph1[e11.first].sId = "E_12";
    subGraph1[e12.first].sId = "E_13";


    Graph::vertex_descriptor v14 = add_vertex(subGraph2);
    Graph::vertex_descriptor v15 = add_vertex(subGraph2);
    Graph::vertex_descriptor v16 = add_vertex(subGraph2);
    subGraph2[v14].sId = "S_15";
    subGraph2[v15].sId = "S_16";
    subGraph2[v16].sId = "S_17";

    auto e13 = add_edge(v0, subGraph2.local_to_global(v15), m_gMainGraph);

    m_gMainGraph[e13.first].sId ="E14 intergraph";

    auto e14 = add_edge(subGraph1.local_to_global(v11),
                        subGraph2.local_to_global(v15),
                        m_gMainGraph);
    m_gMainGraph[e14.first].sId = "E_15 intergraph global edge";
}

void TestGraphmlWriter::generateMainGraph2(int iGenerateExtraVertice)
{
    const int generateVertexIdFrom = 100;
    Graph& G1 = m_gMainGraph.create_subgraph();
    Graph& G2 = m_gMainGraph.create_subgraph();
    Graph& G4 = G2.create_subgraph();
    Graph& G3 = m_gMainGraph.create_subgraph();

    Graph::vertex_descriptor n3 = add_vertex(G1);
    Graph::vertex_descriptor n4 = add_vertex(G1);
    Graph::vertex_descriptor n5 = add_vertex(G1);
    G1[n3].sId = "n3";
    G1[n4].sId = "n4";
    G1[n5].sId = "n5";

    int iMaxVertexCount = iGenerateExtraVertice;
    iMaxVertexCount /=5; // Because we have 5 graphs in subgraph tree,
                        //and in each iteration we insert one vertex to every graph

    //Adding extra vertices and edges to the graph
    iMaxVertexCount += generateVertexIdFrom;
    int iVertexId;
    string sVertexIdMaingraph, sVertexIdG1, sVertexIdG2, sVertexIdG3, sVertexIdG4 ;

    for(int vertexCounter = generateVertexIdFrom ; vertexCounter < iMaxVertexCount ; vertexCounter++)
    {
        Graph::vertex_descriptor vMainGraph = add_vertex(m_gMainGraph);
        iVertexId = vertexCounter;
        sVertexIdMaingraph =   lexical_cast<string>(iVertexId);
        m_gMainGraph[vMainGraph].sId = "n" + sVertexIdMaingraph;

        Graph::vertex_descriptor vG1= add_vertex(G1);
        iVertexId = vertexCounter + iMaxVertexCount;
        sVertexIdG1 = lexical_cast<string>(iVertexId);
        G1[vG1].sId = "n"+ sVertexIdG1;

        Graph::vertex_descriptor vG2 = add_vertex(G2);
        iVertexId = vertexCounter + iMaxVertexCount * 2;
        sVertexIdG2 = lexical_cast<string>(iVertexId);
        G2[vG2].sId = "n" + sVertexIdG2;

        Graph::vertex_descriptor vG3 = add_vertex(G3);
        iVertexId = vertexCounter + iMaxVertexCount * 3;
        sVertexIdG3 = lexical_cast<string>(iVertexId);
        G3[vG3].sId = "n" + sVertexIdG3;

        Graph::vertex_descriptor vG4 = add_vertex(G4);
        iVertexId = vertexCounter + iMaxVertexCount * 4;
        sVertexIdG4 = lexical_cast<string>(iVertexId);
        G4[vG4].sId = "n" + sVertexIdG4;


        auto eMainGraphG1 = add_edge(vMainGraph , G1.local_to_global(vG1), m_gMainGraph);
        m_gMainGraph[eMainGraphG1.first].sId = "e" + sVertexIdMaingraph;
        m_gMainGraph[eMainGraphG1.first].bBidirectional = true;

        auto eMainGraphG2 = add_edge(vMainGraph , G2.local_to_global(vG2), m_gMainGraph);
        m_gMainGraph[eMainGraphG2.first].sId = "e" + sVertexIdG1;

        auto eG2G3 = add_edge(G2.local_to_global(vG2), G3.local_to_global(vG3), m_gMainGraph);
        m_gMainGraph[eG2G3.first].sId = "e" + sVertexIdG2;

        auto eG2G4 = add_edge(G2.local_to_global(vG2), G4.local_to_global(vG4), m_gMainGraph);
        m_gMainGraph[eG2G4.first].sId = "e" + sVertexIdG3;

    }

    auto e0 = add_edge(n3, n4, G1);
    auto e1 = add_edge(n4, n5, G1);
    G1[e0.first].sId = "e0";
    G1[e1.first].sId = "e1";

    Graph::vertex_descriptor n6 = add_vertex(G2);
    Graph::vertex_descriptor n7 = add_vertex(G2);
    G2[n6].sId = "n6";
    G2[n7].sId = "n7";

    Graph::vertex_descriptor n12 = add_vertex(G4);
    Graph::vertex_descriptor n13 = add_vertex(G4);
    G4[n12].sId = "n12";
    G4[n13].sId = "n13";

    auto e3 = add_edge(n12, n13, G4);
    G4[e3.first].sId = "e3";

    Graph::vertex_descriptor n9 = add_vertex(G3);
    Graph::vertex_descriptor n10 = add_vertex(G3);
    G3[n9].sId = "n9";
    G3[n10].sId = "n10";

    auto e4 = add_edge(G1.local_to_global(n3),G2.local_to_global(n6),m_gMainGraph);
    auto e5 = add_edge(G1.local_to_global(n4),G2.local_to_global(n7),m_gMainGraph);
    auto e6 = add_edge(G1.local_to_global(n5),G3.local_to_global(n10),m_gMainGraph);
    m_gMainGraph[e4.first].sId = "e4";
    m_gMainGraph[e5.first].sId = "e5";
    m_gMainGraph[e6.first].sId = "e6";
}
