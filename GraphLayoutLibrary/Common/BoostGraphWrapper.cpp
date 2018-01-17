#include "BoostGraphWrapper.h"

BoostGraphWrapper::BoostGraphWrapper()
{
    //Initialize vertex order values of main graph; m_GRaph
    initGraphVertexOrderValues(m_Graph);
}

BoostGraphWrapper::BoostGraphWrapper(SubGraph graph)
{
    m_Graph = graph;

    //Initialize vertex order values of main graph; m_GRaph
    initGraphVertexOrderValues(m_Graph);
}

VertexDescriptor BoostGraphWrapper::addVertex()
{
    return addVertex(m_Graph);
}

VertexDescriptor BoostGraphWrapper::addVertex(VertexDescriptor &vertex)
{
    return addVertex(vertex,m_Graph);
}

VertexDescriptor BoostGraphWrapper::addVertex(SubGraph& subgraph)
{
    VertexDescriptor newVertex;
    try
    {
      newVertex = add_vertex(subgraph);
    }
    catch(boost::exception& eBoostException)
    {
        //cout<<"Boost AddVertex Error"<<endl;
        std::cerr << *boost::get_error_info<errmsg_info>(eBoostException);

        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    setAsOwnVertex(newVertex , subgraph);

    initVertexOrderValue(newVertex,subgraph);

    return newVertex;
}

VertexDescriptor BoostGraphWrapper::addVertex(VertexDescriptor& vertex, SubGraph& subgraph)
{
    VertexDescriptor newVertex = add_vertex(vertex , subgraph);

    //Initialize vertex order
    initVertexOrderValue(newVertex,subgraph);

    return newVertex;
}

VertexDescriptor BoostGraphWrapper::addVertex(SubGraph &subgraph, LayoutEnum::NodeType enVertexType)
{
    VertexDescriptor vNewVertex = addVertex(subgraph);

    //Set vertex type
    setVertexType(vNewVertex , subgraph ,enVertexType);

    //Adding border vertex entry to the graph
    if(enVertexType == LayoutEnum::UpperBorderNode)
    {
        setGraphUpperBorderVertex(vNewVertex , subgraph);
    }
    else if(enVertexType == LayoutEnum::LowerBorderNode)
    {
        setGraphLowerBorderVertex(vNewVertex , subgraph);
    }

    return vNewVertex;
}

EdgeBoolPair BoostGraphWrapper::addEdge(VertexDescriptor &vertex1, VertexDescriptor &vertex2)
{
    return addEdge(vertex1,vertex2,m_Graph);
}

EdgeBoolPair BoostGraphWrapper::addEdge(VertexDescriptor &vertex1, VertexDescriptor &vertex2, SubGraph &subgraph)
{
    return add_edge(vertex1,vertex2,subgraph);
}

EdgeBoolPair BoostGraphWrapper::addEdge(VertexDescriptor &vertex1, VertexDescriptor &vertex2, SubGraph &subgraph, LayoutEnum::EdgeType enEdgeType)
{
    EdgeBoolPair newEdge = addEdge(vertex1 , vertex2 ,subgraph);
    subgraph[newEdge.first].enEdgeType = enEdgeType;

    return newEdge;
}

void BoostGraphWrapper::removeEdge(EdgeDescriptor &eEdge, SubGraph &gGraph)
{
    remove_edge(eEdge , gGraph);
}

void BoostGraphWrapper::removeEdge(EdgeDescriptor &eEdge)
{
    removeEdge(eEdge , m_Graph);
}

void BoostGraphWrapper::removeEdge(VertexDescriptor &vSource, VertexDescriptor &vTarget, SubGraph &gGraph)
{
    remove_edge(vSource , vTarget , gGraph);
}

void BoostGraphWrapper::removeEdge(VertexDescriptor &vSource, VertexDescriptor &vTarget)
{
    removeEdge(vSource , vTarget , m_Graph);
}

void BoostGraphWrapper::printGraph()
{
    //Print graph
    print_graph(m_Graph);
}

void BoostGraphWrapper::printGraph(SubGraph &subgraph)
{
    print_graph(subgraph);
}

int BoostGraphWrapper::numVertices()
{
    return num_vertices(m_Graph);
}

int BoostGraphWrapper::numVertices(SubGraph &gSubgraph)
{
    return num_vertices(gSubgraph);
}

int BoostGraphWrapper::getVertexIndex(VertexDescriptor& vertex)
{
    return get(vertex_index,m_Graph,vertex);
}

SubGraph& BoostGraphWrapper::addSubgraph()
{
    return addSubgraph(m_Graph);
}

SubGraph& BoostGraphWrapper::addSubgraph(SubGraph &subgraph)
{
    SubGraph& newSubGraph = subgraph.create_subgraph();

    //Initialize vertex order values
//    initGraphVertexOrderValues(newSubGraph);

    return newSubGraph;
}

SubGraph& BoostGraphWrapper::getGraph()
{
    return m_Graph;
}

QString BoostGraphWrapper::getVertexId(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].sId;
}

QString BoostGraphWrapper::getVertexId(VertexDescriptor &vertex)
{
    return getVertexId(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexId(VertexDescriptor &vertex, SubGraph &subgraph, QString sId)
{
    subgraph[vertex].sId = sId;
}

void BoostGraphWrapper::setVertexId(VertexDescriptor &vertex, QString sId)
{
    setVertexId(vertex, m_Graph , sId);
}

int BoostGraphWrapper::getVertexHeight(VertexDescriptor &vertex, SubGraph &subgraph)
{
    try
    {
        return subgraph[vertex].iHeight;
    }catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
}

int BoostGraphWrapper::getVertexHeight(VertexDescriptor &vertex)
{
    return getVertexHeight(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexHeight(VertexDescriptor &vertex, SubGraph &subgraph, int iHeight)
{
    subgraph[vertex].iHeight = iHeight;
}

void BoostGraphWrapper::setVertexHeight(VertexDescriptor &vertex, int iHeight)
{
    setVertexHeight(vertex , m_Graph , iHeight);
}

int BoostGraphWrapper::getVertexWidth(VertexDescriptor &vertex, SubGraph &subgraph)
{
    try{
    return subgraph[vertex].iWidth;
    }catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
}

int BoostGraphWrapper::getVertexWidth(VertexDescriptor &vertex)
{
    return getVertexWidth(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexWidth(VertexDescriptor &vertex, SubGraph &subgraph, int iWidth)
{
    subgraph[vertex].iWidth = iWidth;
}

void BoostGraphWrapper::setVertexWidth(VertexDescriptor &vertex, int iWidth)
{
    setVertexWidth(vertex , m_Graph , iWidth);
}

int BoostGraphWrapper::getVertexLeftCoordX(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].iLeftCoordX;
}

int BoostGraphWrapper::getVertexLeftCoordX(VertexDescriptor &vertex)
{
    return getVertexLeftCoordX(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexLeftCoordX(VertexDescriptor &vertex, int iLeftCoordX)
{
    setVertexLeftCoordX(vertex , m_Graph , iLeftCoordX);
}

void BoostGraphWrapper::setVertexLeftCoordX(VertexDescriptor &vertex, SubGraph &subgraph, int iLeftCoordX)
{
    subgraph[vertex].iLeftCoordX = iLeftCoordX;
}

int BoostGraphWrapper::getVertexLeftCoordY(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].iLeftCoordY;
}

int BoostGraphWrapper::getVertexLeftCoordY(VertexDescriptor &vertex)
{
    return getVertexLeftCoordY(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexLeftCoordY(VertexDescriptor &vertex, SubGraph &subgraph, int iLeftCoordY)
{
    subgraph[vertex].iLeftCoordY = iLeftCoordY;
}

void BoostGraphWrapper::setVertexLeftCoordY(VertexDescriptor &vertex, int iLeftCoordY)
{
    setVertexLeftCoordY(vertex , m_Graph , iLeftCoordY);
}


// Function to get and set Center coodinates of node
int BoostGraphWrapper::getVertexCenterCoordX(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].iCoord_X;
}

int BoostGraphWrapper::getVertexCenterCoordX(VertexDescriptor &vertex)
{
    return getVertexCenterCoordX(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexCenterCoordX(VertexDescriptor &vertex, int iCoordX)
{
    setVertexCenterCoordX(vertex , m_Graph , iCoordX);
}

void BoostGraphWrapper::setVertexCenterCoordX(VertexDescriptor &vertex, SubGraph &subgraph, int iCoordX)
{
    subgraph[vertex].iCoord_X = iCoordX;
    int iLeftTopX = iCoordX - (subgraph[vertex].iWidth)/2;
    subgraph[vertex].iLeftCoordX = iLeftTopX;
}

int BoostGraphWrapper::getVertexCenterCoordY(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].iCoord_Y;
}

int BoostGraphWrapper::getVertexCenterCoordY(VertexDescriptor &vertex)
{
    return getVertexCenterCoordY(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexCenterCoordY(VertexDescriptor &vertex, SubGraph &subgraph, int iCoordY)
{
    subgraph[vertex].iCoord_Y = iCoordY;
    int iLeftTopY = iCoordY - (subgraph[vertex].iHeight)/2;
    subgraph[vertex].iLeftCoordY = iLeftTopY;
}

void BoostGraphWrapper::setVertexCenterCoordY(VertexDescriptor &vertex, int iCoordY)
{
    setVertexCenterCoordY(vertex , m_Graph , iCoordY);
}




bool BoostGraphWrapper::getVertexIsInvisible(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].bIsInvisible;
}

bool BoostGraphWrapper::getVertexIsInvisible(VertexDescriptor &vertex)
{
    return getVertexIsInvisible(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexIsInvisible(VertexDescriptor &vertex, SubGraph &subgraph, bool bIsInvisible)
{
    subgraph[vertex].bIsInvisible = bIsInvisible;
}

void BoostGraphWrapper::setVertexIsInvisible(VertexDescriptor &vertex, bool bIsInvisible)
{
    setVertexIsInvisible(vertex , m_Graph , bIsInvisible);
}

bool BoostGraphWrapper::getVertexExpandable(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].bIsExpandable;
}

bool BoostGraphWrapper::getVertexExpandable(VertexDescriptor &vertex)
{
    return getVertexExpandable(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexExpandable(VertexDescriptor &vertex, SubGraph &subgraph, bool bIsExpandable)
{
    subgraph[vertex].bIsExpandable = bIsExpandable;
}

void BoostGraphWrapper::setVertexExpandable(VertexDescriptor &vertex, bool bIsExpandable)
{
    setVertexExpandable(vertex , m_Graph , bIsExpandable);
}

int BoostGraphWrapper::getVertexTreeWidth(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].iTreeWidth;
}

int BoostGraphWrapper::getVertexTreeWidth(VertexDescriptor &vertex)
{
    return getVertexTreeWidth(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexTreeWidth(VertexDescriptor &vertex, SubGraph &subgraph, int iTreeWidth)
{
    subgraph[vertex].iTreeWidth = iTreeWidth;
}

void BoostGraphWrapper::setVertexTreeWidth(VertexDescriptor &vertex, int iTreeWidth)
{
    setVertexTreeWidth(vertex , m_Graph , iTreeWidth);
}

int BoostGraphWrapper::getVertexDistanceFromRoot(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].iDistanceFromRoot;
}

int BoostGraphWrapper::getVertexDistanceFromRoot(VertexDescriptor &vertex)
{
    return getVertexDistanceFromRoot(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexDistanceFromRoot(VertexDescriptor &vertex, SubGraph &subgraph, int iDistanceFromRoot)
{
    subgraph[vertex].iDistanceFromRoot = iDistanceFromRoot;
}

void BoostGraphWrapper::setVertexDistanceFromRoot(VertexDescriptor &vertex, int iDistanceFromRoot)
{
    setVertexDistanceFromRoot(vertex , m_Graph , iDistanceFromRoot);
}

int BoostGraphWrapper::getVertexRank(VertexDescriptor &vertex, SubGraph &subgraph)
{
    Q_ASSERT_X(&subgraph != NULL, "Get Vertex Rank" , "NULL graph");
    return subgraph[vertex].iRank;
}

int BoostGraphWrapper::getVertexRank(VertexDescriptor &vertex)
{
    return getVertexRank(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexRank(VertexDescriptor &vertex, SubGraph &subgraph, int iRank)
{
    subgraph[vertex].iRank = iRank;
}

void BoostGraphWrapper::setVertexRank(VertexDescriptor &vertex, int iRank)
{
    setVertexRank(vertex , m_Graph , iRank);
}

int BoostGraphWrapper::getVertexHorizontalPosition(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].iHorizontalPosition;
}

int BoostGraphWrapper::getVertexHorizontalPosition(VertexDescriptor &vertex)
{
    return getVertexHorizontalPosition(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexHorizontalPosition(VertexDescriptor &vertex, SubGraph &subgraph, int iHorizontalPosition)
{
    subgraph[vertex].iHorizontalPosition = iHorizontalPosition;
}

void BoostGraphWrapper::setVertexHorizontalPosition(VertexDescriptor &vertex, int iHorizontalPosition)
{
    setVertexHorizontalPosition(vertex , m_Graph , iHorizontalPosition);
}

double BoostGraphWrapper::getVertexBarryCenter(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].dBarryCenter;
}

double BoostGraphWrapper::getVertexBarryCenter(VertexDescriptor &vertex)
{
    return getVertexBarryCenter(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexBarryCenter(VertexDescriptor &vertex, SubGraph &subgraph, double dBarryCenter)
{
    subgraph[vertex].dBarryCenter = dBarryCenter;
}

void BoostGraphWrapper::setVertexBarryCenter(VertexDescriptor &vertex, double dBarryCenter)
{
    setVertexBarryCenter(vertex , m_Graph , dBarryCenter);
}



int BoostGraphWrapper::getVertexTreeLeftX(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].iTreeLeftX;
}

int BoostGraphWrapper::getVertexTreeLeftX(VertexDescriptor &vertex)
{
    return getVertexTreeLeftX(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexTreeLeftX(VertexDescriptor &vertex, SubGraph &subgraph, int iTreeLeftX)
{
    subgraph[vertex].iTreeLeftX = iTreeLeftX;
}

void BoostGraphWrapper::setVertexTreeLeftX(VertexDescriptor &vertex, int iTreeLeftX)
{
    setVertexTreeLeftX(vertex , m_Graph , iTreeLeftX);
}

LayoutEnum::NodeType BoostGraphWrapper::getVertexType(VertexDescriptor &vertex, SubGraph &subgraph)
{
    try{
    return subgraph[vertex].enVertexType;
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
}

LayoutEnum::NodeType BoostGraphWrapper::getVertexType(VertexDescriptor &vertex)
{
    return getVertexType(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexType(VertexDescriptor &vertex, SubGraph &subgraph, LayoutEnum::NodeType enVertexType)
{
    subgraph[vertex].enVertexType = enVertexType;
}

void BoostGraphWrapper::setVertexType(VertexDescriptor &vertex, LayoutEnum::NodeType enVertexType)
{
    setVertexType(vertex , m_Graph , enVertexType);
}

bool BoostGraphWrapper::getVertexVisited(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return subgraph[vertex].bVisited;
}

bool BoostGraphWrapper::getVertexVisited(VertexDescriptor &vertex)
{
    return getVertexVisited(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexVisited(VertexDescriptor &vertex, SubGraph &subgraph, bool bVisited)
{
    subgraph[vertex].bVisited = bVisited;
}

void BoostGraphWrapper::setVertexVisited(VertexDescriptor &vertex, bool bVisited)
{
    setVertexVisited(vertex , m_Graph , bVisited);
}

int BoostGraphWrapper::getVertexTopologicalOrder(VertexDescriptor &vertex, SubGraph &subgraph)
{
    Q_ASSERT_X(&subgraph != NULL, "Get Vertex TopologicalOrder" , "NULL graph");
    return subgraph[vertex].iTopologicalOrder;
}

int BoostGraphWrapper::getVertexTopologicalOrder(VertexDescriptor &vertex)
{
    return getVertexTopologicalOrder(vertex , m_Graph);
}

void BoostGraphWrapper::setVertexTopologicalOrder(int iTopologicalOrder , VertexDescriptor &vertex, SubGraph &subgraph)
{
    ////qDebug() <<"Topo Val:"<<iTopologicalOrder;
    subgraph[vertex].iTopologicalOrder = iTopologicalOrder;
}

void BoostGraphWrapper::setVertexTopologicalOrder(int iTopologicalOrder , VertexDescriptor &vertex )
{
    setVertexTopologicalOrder(iTopologicalOrder , vertex , m_Graph);
}

QString BoostGraphWrapper::getEdgeId(EdgeDescriptor &edge, SubGraph &subgraph)
{
    return subgraph[edge].sId;
}

QString BoostGraphWrapper::getEdgeId(EdgeDescriptor &edge)
{
    return getEdgeId(edge , m_Graph);
}

void BoostGraphWrapper::setEdgeId(EdgeDescriptor &edge, SubGraph &subgraph, QString sId)
{
    subgraph[edge].sId = sId;
}

void BoostGraphWrapper::setEdgeId(EdgeDescriptor &edge, QString sId)
{
    setEdgeId(edge, m_Graph , sId);
}

std::size_t BoostGraphWrapper::getEdgeIndex(EdgeDescriptor &edge, SubGraph &subgraph)
{
    return subgraph[edge].iEdgeIndex;
}

std::size_t BoostGraphWrapper::getEdgeIndex(EdgeDescriptor &edge)
{
    return getEdgeIndex(edge , m_Graph);
}

void BoostGraphWrapper::setEdgeIndex(EdgeDescriptor &edge, SubGraph &subgraph, std::size_t iEdgeIndex)
{
    subgraph[edge].iEdgeIndex = iEdgeIndex;
}

void BoostGraphWrapper::setEdgeIndex(EdgeDescriptor &edge, std::size_t iEdgeIndex)
{
    setEdgeIndex(edge, m_Graph , iEdgeIndex);
}

bool BoostGraphWrapper::getEdgeBidirectional(EdgeDescriptor &edge, SubGraph &subgraph)
{
    return subgraph[edge].bBidirectional;
}

bool BoostGraphWrapper::getEdgeBidirectional(EdgeDescriptor &edge)
{
    return getEdgeBidirectional(edge , m_Graph);
}

void BoostGraphWrapper::setEdgeBidirectional(EdgeDescriptor &edge, SubGraph &subgraph, bool bBidirectional)
{
    subgraph[edge].bBidirectional = bBidirectional;
}

void BoostGraphWrapper::setEdgeBidirectional(EdgeDescriptor &edge, bool bBidirectional)
{
    setEdgeBidirectional(edge , m_Graph , bBidirectional);
}

bool BoostGraphWrapper::getEdgeIsInvisible(EdgeDescriptor &edge, SubGraph &subgraph)
{
    return subgraph[edge].bIsInvisible;
}

bool BoostGraphWrapper::getEdgeIsInvisible(EdgeDescriptor &edge)
{
    return getEdgeIsInvisible(edge , m_Graph);
}

void BoostGraphWrapper::setEdgeIsInvisible(EdgeDescriptor &edge, SubGraph &subgraph, bool bIsInvisible)
{
    subgraph[edge].bIsInvisible = bIsInvisible;
}

void BoostGraphWrapper::setEdgeIsInvisible(EdgeDescriptor &edge, bool bIsInvisible)
{
    setEdgeIsInvisible(edge , m_Graph , bIsInvisible);
}

bool BoostGraphWrapper::getEdgeVisited(EdgeDescriptor &edge, SubGraph &subgraph)
{
    return subgraph[edge].bVisited;
}

bool BoostGraphWrapper::getEdgeVisited(EdgeDescriptor &edge)
{
    return getEdgeVisited(edge , m_Graph);
}

void BoostGraphWrapper::setEdgeVisited(EdgeDescriptor &edge, SubGraph &subgraph, bool bVisited)
{
    subgraph[edge].bVisited = bVisited;
}

void BoostGraphWrapper::setEdgeVisited(EdgeDescriptor &edge, bool bVisited)
{
    setEdgeVisited(edge, m_Graph , bVisited);
}

bool BoostGraphWrapper::getEdgeReversed(EdgeDescriptor &edge, SubGraph &subgraph)
{
    return subgraph[edge].bReversed;
}

bool BoostGraphWrapper::getEdgeReversed(EdgeDescriptor &edge)
{
    return getEdgeReversed(edge , m_Graph);
}

void BoostGraphWrapper::setEdgeReversed(EdgeDescriptor &edge, SubGraph &subgraph, bool bReversed)
{
    subgraph[edge].bReversed = bReversed;
}

void BoostGraphWrapper::setEdgeReversed(EdgeDescriptor &edge, bool bReversed)
{
    setEdgeReversed(edge, m_Graph , bReversed);
}

bool BoostGraphWrapper::getEdgeIsConflicted(EdgeDescriptor &edge, SubGraph &subgraph)
{
    return subgraph[edge].bIsConflicted;
}

bool BoostGraphWrapper::getEdgeIsConflicted(EdgeDescriptor &edge)
{
    return getEdgeIsConflicted(edge , m_Graph);
}

void BoostGraphWrapper::setEdgeIsConflicted(EdgeDescriptor &edge, SubGraph &subgraph, bool bIsConflicted)
{
    subgraph[edge].bIsConflicted = bIsConflicted;
}

void BoostGraphWrapper::setEdgeIsConflicted(EdgeDescriptor &edge, bool bIsConflicted)
{
    setEdgeIsConflicted(edge , m_Graph , bIsConflicted);
}

LayoutEnum::EdgeType BoostGraphWrapper::getEdgeType(EdgeDescriptor eEdge, SubGraph &gGraph)
{
    try{
    return gGraph[eEdge].enEdgeType;
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
}

LayoutEnum::EdgeType BoostGraphWrapper::getEdgeType(EdgeDescriptor eEdge)
{
    return getEdgeType(eEdge , m_Graph);
}

void BoostGraphWrapper::setEdgeType(EdgeDescriptor eEdge, SubGraph &subgraph, LayoutEnum::EdgeType enEdgeType)
{
    subgraph[eEdge].enEdgeType = enEdgeType;
}

void BoostGraphWrapper::setEdgeType(EdgeDescriptor eEdge, LayoutEnum::EdgeType enEdgeType)
{
    setEdgeType(eEdge, m_Graph, enEdgeType);
}

std::pair<IteratorQVectorBendPoints, IteratorQVectorBendPoints> BoostGraphWrapper::edgeBendPointsIter(EdgeDescriptor& eEdge, SubGraph &gSubgraph)
{
    // get the values bend points iterator
    IteratorQVectorBendPoints iterBendPointsStart = gSubgraph[eEdge].vecBendPoints.begin();
    IteratorQVectorBendPoints iterBendPointsEnd = gSubgraph[eEdge].vecBendPoints.end();

    // prepare pair for the iterartors
    std::pair<IteratorQVectorBendPoints, IteratorQVectorBendPoints> pairIteratorBendPoints;

    // get data into pair
    pairIteratorBendPoints.first = iterBendPointsStart;
    pairIteratorBendPoints.second = iterBendPointsEnd;

    // return pair of iterators
    return pairIteratorBendPoints;
}

void BoostGraphWrapper::addBendPoint(BendPoints *bendPointRef, EdgeDescriptor eEdge, SubGraph &gGraph)
{
    gGraph[eEdge].vecBendPoints.push_back(bendPointRef);
}
QString BoostGraphWrapper::getGraphId(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].sId;
}

QString BoostGraphWrapper::getGraphId()
{
    return getGraphId(m_Graph);
}

void BoostGraphWrapper::setGraphId(QString sId, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].sId = sId;
}

void BoostGraphWrapper::setGraphId(QString sId)
{
    setGraphId(sId , m_Graph);
}

size_t BoostGraphWrapper::getGraphDummyNodeIndex(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iDummyNodeIndex;
}

size_t BoostGraphWrapper::getGraphDummyNodeIndex()
{
    return getGraphDummyNodeIndex(m_Graph);
}

void BoostGraphWrapper::setGraphDummyNodeIndex(size_t iDummyNodeIndex, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iDummyNodeIndex = iDummyNodeIndex;
}

void BoostGraphWrapper::setGraphDummyNodeIndex(size_t iDummyNodeIndex)
{
    setGraphDummyNodeIndex(iDummyNodeIndex , m_Graph);
}

int BoostGraphWrapper::getGraphLeftTopCoordX(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iLeftTopCoordX;
}

int BoostGraphWrapper::getGraphLeftTopCoordX()
{
    return getGraphLeftTopCoordX(m_Graph);
}

void BoostGraphWrapper::setGraphLeftTopCoordX(int iLeftTopCoordX, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iLeftTopCoordX = iLeftTopCoordX;
}

void BoostGraphWrapper::setGraphLeftTopCoordX(int iLeftTopCoordX)
{
    setGraphLeftTopCoordX(iLeftTopCoordX , m_Graph);
}

int BoostGraphWrapper::getGraphLeftTopCoordY(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iLeftTopCoordY;
}

int BoostGraphWrapper::getGraphLeftTopCoordY()
{
    return getGraphLeftTopCoordY(m_Graph);
}

void BoostGraphWrapper::setGraphLeftTopCoordY(int iLeftTopCoordY, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iLeftTopCoordY = iLeftTopCoordY;
}

void BoostGraphWrapper::setGraphLeftTopCoordY(int iLeftTopCoordY)
{
    setGraphLeftTopCoordY(iLeftTopCoordY , m_Graph);
}

int BoostGraphWrapper::getGraphCenterCoordX(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iCenterCoordX;
}

int BoostGraphWrapper::getGraphCenterCoordX()
{
    return getGraphCenterCoordX(m_Graph);
}

void BoostGraphWrapper::setGraphCenterCoordX(int iCenterCoordX, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iCenterCoordX = iCenterCoordX;
}

void BoostGraphWrapper::setGraphCenterCoordX(int iCenterCoordX)
{
    setGraphCenterCoordX(iCenterCoordX , m_Graph);
}

int BoostGraphWrapper::getGraphCenterCoordY(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iCenterCoordY;
}

int BoostGraphWrapper::getGraphCenterCoordY()
{
    return getGraphCenterCoordY(m_Graph);
}

void BoostGraphWrapper::setGraphCenterCoordY(int iCenterCoordY, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iCenterCoordY = iCenterCoordY;
}

void BoostGraphWrapper::setGraphCenterCoordY(int iCenterCoordY)
{
    setGraphCenterCoordY(iCenterCoordY , m_Graph);
}

double BoostGraphWrapper::getGraphRadius(SubGraph &gSubgraph)
{
    try{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].dRadius;
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
}

double BoostGraphWrapper::getGraphRadius()
{
    return getGraphRadius(m_Graph);
}

void BoostGraphWrapper::setGraphRadius(double dRadius, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].dRadius = dRadius;
}

void BoostGraphWrapper::setGraphRadius(double dRadius)
{
    setGraphRadius(dRadius, m_Graph);
}

int BoostGraphWrapper::getGraphHeight(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iHeight;
}

int BoostGraphWrapper::getGraphHeight()
{
    return getGraphHeight(m_Graph);
}

void BoostGraphWrapper::setGraphHeight(int iHeight, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iHeight = iHeight;
}

void BoostGraphWrapper::setGraphHeight(int iHeight)
{
    setGraphHeight(iHeight , m_Graph);
}

int BoostGraphWrapper::getGraphWidth(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iWidth;
}

int BoostGraphWrapper::getGraphWidth()
{
    return getGraphWidth(m_Graph);
}

void BoostGraphWrapper::setGraphWidth(int iWidth, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iWidth = iWidth;
}

void BoostGraphWrapper::setGraphWidth(int iWidth)
{
    setGraphWidth(iWidth , m_Graph);
}

int BoostGraphWrapper::getGraphCoordX(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iCoordX;
}

int BoostGraphWrapper::getGraphCoordX()
{
    return getGraphCoordX(m_Graph);
}

void BoostGraphWrapper::setGraphCoordX(int iCoordX, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iCoordX = iCoordX;
}

void BoostGraphWrapper::setGraphCoordX(int iCoordX)
{
    setGraphCoordX(iCoordX , m_Graph);
}

int BoostGraphWrapper::getGraphCoordY(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iCoordY;
}

int BoostGraphWrapper::getGraphCoordY()
{
    return getGraphCoordY(m_Graph);
}

void BoostGraphWrapper::setGraphCoordY(int iCoordY, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iCoordY = iCoordY;
}

void BoostGraphWrapper::setGraphCoordY(int iCoordY)
{
    setGraphCoordY(iCoordY , m_Graph);
}

int BoostGraphWrapper::getGraphDistanceBetweenCentoidAndCenterCoordX(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iXDistanceFromDummyNodeCentroid;
}

int BoostGraphWrapper::getGraphDistanceBetweenCentoidAndCenterCoordX()
{
    return getGraphDistanceBetweenCentoidAndCenterCoordX(m_Graph);
}

void BoostGraphWrapper::setGraphDistanceBetweenCentoidAndCenterCoordX(int iDistanceCoordX, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iXDistanceFromDummyNodeCentroid = iDistanceCoordX;
}

void BoostGraphWrapper::setGraphDistanceBetweenCentoidAndCenterCoordX(int iDistanceCoordX)
{
setGraphDistanceBetweenCentoidAndCenterCoordX(iDistanceCoordX , m_Graph);
}

int BoostGraphWrapper::getGraphDistanceBetweenCentoidAndCenterCoordY(SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    return mapGraphProp[&gSubgraph].iYDistanceFromDummyNodeCentroid;
}

int BoostGraphWrapper::getGraphDistanceBetweenCentoidAndCenterCoordY()
{
    return getGraphDistanceBetweenCentoidAndCenterCoordY(m_Graph);
}

void BoostGraphWrapper::setGraphDistanceBetweenCentoidAndCenterCoordY(int iDistanceCoordY, SubGraph &gSubgraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
    mapGraphProp[&gSubgraph].iYDistanceFromDummyNodeCentroid = iDistanceCoordY;
}

void BoostGraphWrapper::setGraphDistanceBetweenCentoidAndCenterCoordY(int iDistanceCoordY)
{
    setGraphDistanceBetweenCentoidAndCenterCoordY(iDistanceCoordY , m_Graph);
}

void BoostGraphWrapper::setAsOwnVertex(VertexDescriptor vVertex, SubGraph &gGraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gGraph);
    mapGraphProp[&gGraph].vectOwnVertices.push_back(vVertex);
}

bool BoostGraphWrapper::isOwnVertex(VertexDescriptor vVertex, SubGraph &gGraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gGraph);
    return mapGraphProp[&gGraph].vectOwnVertices.contains(vVertex);
}

std::pair<IteratorQVectorUInt, IteratorQVectorUInt> BoostGraphWrapper::ownVerticesIter(SubGraph &gGraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gGraph);
    IteratorQVectorUInt iterOwnVerticesBegin = mapGraphProp[&gGraph].vectOwnVertices.begin();
    IteratorQVectorUInt iterOwnVerticesEnd = mapGraphProp[&gGraph].vectOwnVertices.end();

    std::pair<IteratorQVectorUInt, IteratorQVectorUInt> pairOwnVerticesIters;

    pairOwnVerticesIters.first = iterOwnVerticesBegin;
    pairOwnVerticesIters.second = iterOwnVerticesEnd;

    return pairOwnVerticesIters;
}

int BoostGraphWrapper::getCountOfOwnVertices(SubGraph &gGraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gGraph);
    int iNumOwnVertices = (mapGraphProp[&gGraph].vectOwnVertices).size();

    return iNumOwnVertices;
}

std::size_t BoostGraphWrapper::getGraphUpperBorderVertex(SubGraph &gGraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gGraph);
    return mapGraphProp[&gGraph].vUpperBorderVertex;
}

std::size_t BoostGraphWrapper::getGraphUpperBorderVertex()
{
    return getGraphUpperBorderVertex(m_Graph);
}

void BoostGraphWrapper::setGraphUpperBorderVertex(std::size_t vUpperBorderVertex, SubGraph &gGraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gGraph);
    mapGraphProp[&gGraph].vUpperBorderVertex = vUpperBorderVertex;
}

void BoostGraphWrapper::setGraphUpperBorderVertex(std::size_t vUpperBorderVertex)
{
    setGraphUpperBorderVertex(vUpperBorderVertex , m_Graph);
}

std::size_t BoostGraphWrapper::getGraphLowerBorderVertex(SubGraph &gGraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gGraph);
    return mapGraphProp[&gGraph].vLowerBorderVertex;
}

std::size_t BoostGraphWrapper::getGraphLowerBorderVertex()
{
    return getGraphLowerBorderVertex(m_Graph);
}

void BoostGraphWrapper::setGraphLowerBorderVertex(std::size_t vLowerBorderVertex, SubGraph &gGraph)
{
    PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gGraph);
    mapGraphProp[&gGraph].vLowerBorderVertex = vLowerBorderVertex;
}

void BoostGraphWrapper::setGraphLowerBorderVertex(std::size_t vLowerBorderVertex)
{
    setGraphLowerBorderVertex(vLowerBorderVertex , m_Graph);
}

VertexDescriptor BoostGraphWrapper::getEdgeTarget(EdgeDescriptor edge, SubGraph &subgraph)
{
    return target(edge , subgraph);
}

VertexDescriptor BoostGraphWrapper::getEdgeTarget(EdgeDescriptor edge)
{
    return getEdgeTarget(edge , m_Graph);
}

VertexDescriptor BoostGraphWrapper::getEdgeSource(EdgeDescriptor edge, SubGraph &subgraph)
{
    return source(edge , subgraph);
}

VertexDescriptor BoostGraphWrapper::getEdgeSource(EdgeDescriptor edge)
{
    return getEdgeSource(edge , m_Graph);
}

VertexIterPair BoostGraphWrapper::getVertices()
{
    return vertices(m_Graph);
}

VertexIterPair BoostGraphWrapper::getVertices(SubGraph &subgraph)
{
    return vertices(subgraph);
}

ChildrenIterPair BoostGraphWrapper::getChildren()
{
    return m_Graph.children();
}

ChildrenIterPair BoostGraphWrapper::getChildren(SubGraph &subgraph)
{
    return subgraph.children();
}

InEdgeIterPair BoostGraphWrapper::getInEdges(VertexDescriptor& vertex)
{
    return getInEdges(vertex,m_Graph);
}

InEdgeIterPair BoostGraphWrapper::getInEdges(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return in_edges(vertex,subgraph);
}

OutEdgeIterPair BoostGraphWrapper::getOutEdges(VertexDescriptor &vertex)
{
    return getOutEdges(vertex,m_Graph);
}

OutEdgeIterPair BoostGraphWrapper::getOutEdges(VertexDescriptor &vertex, SubGraph &subgraph)
{
    return out_edges(vertex,subgraph);
}

void BoostGraphWrapper::getChildrenVector(SubGraph &gSubgraph, VectorSubgraphRefs& vecChildSubgraphs)
{
    ChildrenIterator itrChild, itrChildEnd;
    for(boost::tie(itrChild,itrChildEnd) = gSubgraph.children();
        itrChild != itrChildEnd;
        ++itrChild)
    {
        vecChildSubgraphs.push_back(&(*itrChild));
        //qDebug() << "Graph in PGL Iterator:\n";
        print_graph(*vecChildSubgraphs.back());
    }
}

bool BoostGraphWrapper::isValidOrder(std::size_t order)
{
    bool bValidOrder = false;

    if(order>0)
    {
        bValidOrder = true;
    }

    return bValidOrder;
}

int BoostGraphWrapper::getVertexOrder(VertexDescriptor &vertex, SubGraph &subgraph)
{
    try{
    VertexDescriptor vGlobalVertex = subgraph.local_to_global(vertex);
    SubGraph &gRootGraph = subgraph.root();

    int iOrder = gRootGraph[vGlobalVertex].iOrder;

    return iOrder;
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
}

bool BoostGraphWrapper::isValidShift(std::size_t iOrderSource, MapOrderVertex &mapOrderVertex, int iShiftBy, ShiftDirection enDirection)
{
    bool isValidShift = false;
    int iCounter = iShiftBy;
    //check if iOrderSource is valid key in mapOrderVertex
    if(mapOrderVertex.contains(iOrderSource))
    {
        //iOrderSource is valid key in mapOrderVertex


        //Check shifting is valid or not

        if(iShiftBy==0)
        {
            isValidShift=false;
        }
        else
        {
            MapOrderVertex::const_iterator iter1;
            iter1=mapOrderVertex.find(iOrderSource);

            if(enDirection==LeftDirection)
            {
                while(iter1!=mapOrderVertex.constBegin())
                {
                    iCounter--;
                    iter1--;
                }
                if(iCounter<=0)
                {
                    //cout<<"I am left";
                    isValidShift=true;
                }

            }
            else if(enDirection==RightDirection)
            {
                while(iter1!=mapOrderVertex.constEnd())
                {
                    iCounter--;
                    iter1++;
                }
                if(iCounter<=-1)
                {
                    //cout<<"I am right";
                    isValidShift=true;
                }
            }
        }
    }
    else
    {
        //iOrderSource is not a valid order value in map 'mapOrderVertex'
        cout << directionEnumString(enDirection).toStdString();
        //cout<< " shift not possible. Invalid order: " << iOrderSource << "." << endl;
    }
    return isValidShift;
}


void BoostGraphWrapper::initGraphVertexOrderValues(SubGraph &subgraph)
{
    //Remove later
   // cout << "Initializing graph vertex order" << endl;

    try{
    // Get root graph
    SubGraph& gMaingraph = subgraph.root();
    //Create a map for vertex to iOrder property
    PGL_MAP_VERTEX_ORDER(mapGlobalVertexOrder,subgraph)

    //Initialize vertex iOrder value
    BGL_FORALL_VERTICES(vertex,subgraph,SubGraph)
    {
        //Convert local to global vertex descriptor
        VertexDescriptor globalVertex = subgraph.local_to_global(vertex);

        mapGlobalVertexOrder[globalVertex] =  PGL_VERTEX_INDEX(globalVertex,gMaingraph) + 1;
        cout << mapGlobalVertexOrder[globalVertex] << " ";
    }
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
}

void BoostGraphWrapper::getMinMaxVertexOrder(SubGraph &gSubgraph, int& iMinVertexOrder, int& iMaxVertexOrder)
{
PGL_MAP_VERTEX_ORDER(mapVertexOrder,gSubgraph);
MapOrderVertex mapOrderVertex;
mapOrderVertex = getMapOrderedVertices(gSubgraph, mapVertexOrder);

IteratorMapOrderVertex mapIter(mapOrderVertex);
bool bFirstTime = true;
while(mapIter.hasNext())
{
    mapIter.next();
    std::size_t key = mapIter.key();
    if(bFirstTime == true)
    {
        iMinVertexOrder = key;
        bFirstTime = false;
    }
    else
    {
        iMaxVertexOrder = key;
    }
}
//cout<<"Min Max Value : "<<iMinVertexOrder<<" "<<iMaxVertexOrder<<endl;
}

void BoostGraphWrapper::getMinMaxVertices(SubGraph &gSubgraph, VertexDescriptor& vMinVertex, VertexDescriptor& vMaxVertex)
{
    PGL_MAP_VERTEX_ORDER(mapVertexOrder,gSubgraph);
    MapOrderVertex mapOrderVertex;
    mapOrderVertex = getMapOrderedVertices(gSubgraph, mapVertexOrder);
    int iMaxOrder;
    int iMinOrder;
    getMinMaxVertexOrder(gSubgraph, iMinOrder, iMaxOrder);

    vMinVertex = mapOrderVertex.value(iMinOrder);
    vMaxVertex = mapOrderVertex.value(iMaxOrder);
    //cout<< "Min - Max Vertices : "<<vMinVertex<<" "<<vMaxVertex<<endl;
}

VertexDescriptor BoostGraphWrapper::addDummyVertexForChildGraph(SubGraph &gParentGraph, SubGraph &gChildGraph)
{
    /*
        This function adds the dummy nodes to graph and set dummy node
        index in the corresponding subgraph for which it is created
    */

    Q_ASSERT_X(&gParentGraph != NULL,
               "Add dummy vertx",
               "Trying to ad dummy node in the empty graph");

    Q_ASSERT_X(&gChildGraph != NULL,
               "Ad dummy vertex",
               "Treing to add dummy vertex in the empty graph");

    // add vertex and set own vertex
    VertexDescriptor vVertex = addVertex(gParentGraph, LayoutEnum::DummyNode);

    // get index
    size_t iDummyNodeIndex = getVertexIndex(vVertex);

    // setDummy node index
    setGraphDummyNodeIndex(iDummyNodeIndex, gChildGraph);
    return vVertex;
}

// Private Functionality
void BoostGraphWrapper::initVertexOrderValue(VertexDescriptor &vertex, SubGraph &subgraph)
{
    //Assert:check vertex with same index is not present in global graph


    VertexDescriptor globalVertex = subgraph.local_to_global(vertex);

    SubGraph &gRootGraph = subgraph.root();

    //Check if order value is assigned already or not (0= not assigned)
    if(gRootGraph[globalVertex].iOrder==0)
    {
        gRootGraph[globalVertex].iOrder = PGL_VERTEX_INDEX(globalVertex,gRootGraph) + 1;
        //qDebug() << "Vertex order set\n";
    }
    else
    {
        //qDebug() << "order is set already"<< endl;
    }

}

QString BoostGraphWrapper::directionEnumString(ShiftDirection enDirection)
{
    QString sDirection;

    if(enDirection==LeftDirection)
    {
        sDirection = "Left";
    }
    else if(enDirection==RightDirection)
    {
        sDirection = "Right";
    }

    return sDirection;
}

QString BoostGraphWrapper::edgeToQString(EdgeDescriptor eEdge , SubGraph gGraph)
{
    QString sEdge = "";
    sEdge.append(QString::number(getEdgeSource(eEdge , gGraph)));

    sEdge.append(" , ");

    sEdge.append(QString::number(getEdgeTarget(eEdge , gGraph)));

    return sEdge;
}


// Getting edge x and y coordinates
int BoostGraphWrapper::getEdgeSourceX(EdgeDescriptor eEdge, SubGraph& subGraph)
{
    VertexDescriptor sourceVertex = source(eEdge,subGraph);
    return subGraph[sourceVertex].iCoord_X;
}

int BoostGraphWrapper::getEdgeSourceY(EdgeDescriptor eEdge, SubGraph& subGraph)
{
    VertexDescriptor sourceVertex = source(eEdge, subGraph);
    return subGraph[sourceVertex].iCoord_Y;
}

int BoostGraphWrapper::getEdgeTargetX(EdgeDescriptor eEdge, SubGraph& subGraph)
{
    VertexDescriptor targetVertex = target(eEdge, subGraph);
    return subGraph[targetVertex].iCoord_X;
}

int BoostGraphWrapper::getEdgeTargetY(EdgeDescriptor eEdge, SubGraph& subGraph)
{
    VertexDescriptor targetVertex = target(eEdge, subGraph);
    return subGraph[targetVertex].iCoord_Y;
}

int BoostGraphWrapper::getEdgeCrossingCount(SubGraph &gGraph)
{
    LAYOUT_ASSERT(&gGraph != NULL,LayoutMemoryException(__FUNCTION__,LayoutExceptionEnum::NULL_POINTER_EXCEPTION, "Graph"));

    int iIntersectCount = 0;

    BGL_FORALL_EDGES(eFirstEdge,gGraph,SubGraph)
    {
        BGL_FORALL_EDGES(eSecondEdge,gGraph,SubGraph)
        {
            if(eFirstEdge == eSecondEdge)
            {
                break;
            }
            else
            {

                try
                {
                    // check intersection of two edges
                    bool bIsIntersecting = GeometryHelper::isIntersectingEdges(eFirstEdge,eSecondEdge,gGraph);

                    if(bIsIntersecting == true)
                    {
                        VertexDescriptor vFirstSource = source(eFirstEdge, gGraph);
                        VertexDescriptor vFirstTarget = target(eFirstEdge, gGraph);
                        VertexDescriptor vSecondSource = source(eSecondEdge, gGraph);
                        VertexDescriptor vSecondTarget = target(eSecondEdge, gGraph);

                        if((vFirstSource == vSecondSource) || (vFirstSource == vSecondTarget))
                        {
                            /* intersecting at the end (at the node position)
                         then it should not be considered as intersection.
                        */
                        }
                        else if((vFirstTarget == vSecondSource) || (vFirstTarget == vSecondTarget))
                        {
                            /* intersecting at the end (at the node position)
                         then it should not be considered as intersection.
                        */
                        }
                        else
                        {
                            // Count only real intersection between the edges.
                            iIntersectCount++;
                        }
                    }
                }
                catch(...)
                {
                    throw;
                }
            }
        }
   }
    return iIntersectCount;
}

/* This function will provide ordering to vertices
 *on the basis of connected components
*/
void BoostGraphWrapper::applyConnectedComponent(SubGraph &gGraph, MapOrderVertex& mapOrderVertex)
{   
    LAYOUT_ASSERT(&gGraph != NULL,LayoutException(__FUNCTION__
                                                  , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                  , "gGraph"
                                                  , ""));

    std::vector<int> vecConnectedComponent(num_vertices(gGraph));
    int iConnectedComponentCount = connected_components(gGraph, &vecConnectedComponent[0]);
//    std::cout<<"Number of connected components : "<<iConnectedComponentCount<<endl;

    // initialize the vertex order TO DO : Shift this step at end of import
    //initGraphVertexOrderValues(gGraph);

    VectorVertexOrder vecVertexOrder;
    PGL_MAP_VERTEX_ORDER(mapVertexOrder,gGraph);
    std::size_t iOrder;
    for(int iComponentId = 0 ; iComponentId < iConnectedComponentCount ; iComponentId++)
    {
        BGL_FORALL_VERTICES(vVertex, gGraph, SubGraph)
        {
            // get order of vertex
            if(iComponentId == vecConnectedComponent[vVertex])
            {
                iOrder = mapVertexOrder[vVertex];
                vecVertexOrder.push_back(iOrder);
            }
        }
    }

    // Sorting the vertices according to order which we found through Topological ordering
    sortVerticesByOrderList(vecVertexOrder, gGraph);
    mapOrderVertex = getMapOrderedVertices(gGraph, mapVertexOrder);
}

/* This function will provide ordering to vertices
 on the basis of Smallest degree last order.
*/
void BoostGraphWrapper::applyTopologicalVertexOrdering(SubGraph &gGraph, MapOrderVertex &mapOrderVertex)
{
    LAYOUT_ASSERT(&gGraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));
    std::deque<int> deqVertexTopologicalOrder(num_vertices(gGraph));
    try
    {
        topological_sort(gGraph, front_inserter(deqVertexTopologicalOrder));
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
//    //cout<<"Topological Order :- "<<endl;
//    for(int iItrOrder = 0 ; iItrOrder < deqVertexTopologicalOrder.size()/2 ; iItrOrder++)
//    {
//        //cout<<" "<<deqVertexTopologicalOrder[iItrOrder];
//    }

    VectorVertexOrder vecVertexOrder;
    PGL_MAP_VERTEX_ORDER(mapVertexOrder,gGraph);
    std::size_t iOrder;
    int iDequeSize = deqVertexTopologicalOrder.size()/2;
    for(int iItrOrder = 0 ; iItrOrder < iDequeSize ; iItrOrder++)
    {
        iOrder = mapVertexOrder[deqVertexTopologicalOrder[iItrOrder]];
        vecVertexOrder.push_back(iOrder);
    }

    // Sorting the vertices according to order which we found through connected components
//    //cout<<"Before sorting vertices order: "<<endl;
    sortVerticesByOrderList(vecVertexOrder, gGraph);
//    //cout<<"After sorting vertices order : "<<endl;
    mapOrderVertex = getMapOrderedVertices(gGraph, mapVertexOrder);
//    //cout<<"Afetr getMapOrderedVertices : "<<endl;
}

/* This function will sort the vertex descriptors according to the order which we are taking
input as a parameter */
void BoostGraphWrapper::sortVerticesByOrderList(VectorVertexOrder vecVertexOrder, SubGraph &gGraph)
{

    LAYOUT_ASSERT(&gGraph != NULL,
                  LayoutMemoryException(__FUNCTION__,
                                  LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                  GRAPH));

    LAYOUT_ASSERT(!vecVertexOrder.empty(),
                  LayoutException(__FUNCTION__,
                                  LayoutExceptionEnum::EMPTY_CONTAINER,
                                  EMPTY,VECTOR_CONTENTS));
    // Valid VectorVertexOrder
    int iVectorSize = vecVertexOrder.size();
    for(int iOrderIndex = 0 ; iOrderIndex < iVectorSize ; iOrderIndex++)
    {
//        if(!isValidOrder(vecVertexOrder[iOrderIndex]))
//        {
//            cout<<"iOrder Index : "<<iOrderIndex<<" Vertex : "<<vecVertexOrder[iOrderIndex]<<endl;
//            printGraph(gGraph);
//        }
        LAYOUT_ASSERT(isValidOrder(vecVertexOrder[iOrderIndex]),LayoutException(__FUNCTION__
                                                 , LayoutExceptionEnum::INVALID_PARAMETER
                                                 , "in vector vecVertexOrder"
                                                 , "vertex order"));
    }
    //Create map
    PGL_MAP_VERTEX_ORDER(mapVertexOrder,gGraph);
    MapOrderVertex mapOrderVertex = getMapOrderedVertices(gGraph, mapVertexOrder);

    // Bubble Sort
    for(int iIter = 0 ; iIter < iVectorSize ; iIter++)
    {
        for(int iSeek = 0 ; iSeek < iVectorSize-1 ; iSeek++)
        {
            if( vecVertexOrder[iSeek] < vecVertexOrder[iSeek + 1])
            {
                //Swap original order values
                swapVertexOrderByOrder(vecVertexOrder[iSeek], vecVertexOrder[iSeek + 1],
                        mapOrderVertex, mapVertexOrder);
                //Swap
                swap(vecVertexOrder[iSeek] , vecVertexOrder[iSeek + 1]);
            }
        }
    }
}



// Force Directed Props

 void BoostGraphWrapper::setVertXDisp(double iDispX,VertexDescriptor verDis, SubGraph& l_graph )
 {
    l_graph[verDis].iVertXDisp = iDispX;
 }




 void BoostGraphWrapper::setVertYDisp(double iDispY,VertexDescriptor verDis, SubGraph& l_graph )
 {
    l_graph[verDis].iVertYDisp = iDispY;
 }


 int BoostGraphWrapper :: getVertXDisp(VertexDescriptor verDis, SubGraph &l_graph)
 {
     return l_graph[verDis].iVertXDisp;
 }

 int BoostGraphWrapper :: getVertYDisp(VertexDescriptor verDis, SubGraph &l_graph)
 {
     return l_graph[verDis].iVertYDisp;
 }

 void BoostGraphWrapper::setVertexLock(VertexDescriptor verDis, SubGraph &l_graph)
 {
     l_graph[verDis].bIsVertexLocked = true;
 }

 bool BoostGraphWrapper::getIsVertexLocked(VertexDescriptor verDis, SubGraph &l_graph)
{
     return l_graph[verDis].bIsVertexLocked;
}

 void BoostGraphWrapper::setVertexClusterID(int iClusterID, VertexDescriptor vDisc, SubGraph& gSubgraph )
 {
     gSubgraph[vDisc].iClusterIdentifier = iClusterID;
 }


 int BoostGraphWrapper::getVertexClusterID(VertexDescriptor vDisc, SubGraph& gSubgraph )
 {
     return gSubgraph[vDisc].iClusterIdentifier;
 }

 int BoostGraphWrapper::getGraphClusterID(SubGraph &gSubgraph)
 {
     PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
     return mapGraphProp[&gSubgraph].iClusterID;
 }

 void BoostGraphWrapper::setGraphClusterID(int iClusterID, SubGraph& gSubgraph)
 {
      PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
      mapGraphProp[&gSubgraph].iClusterID = iClusterID;
 }

 bool BoostGraphWrapper::setIsVirtualNode(VertexDescriptor vertex, SubGraph &subgraph)
{
     subgraph[vertex].bIsVirtualNode = true;

}

 bool BoostGraphWrapper::getIsVirtualNode(VertexDescriptor vertex, SubGraph &subgraph)
{
    return subgraph[vertex].bIsVirtualNode ;

}

  int BoostGraphWrapper::getClusterXDisplacement(SubGraph& gSubgraph)
  {
      PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
      return mapGraphProp[&gSubgraph].iClusterXDisplacement;
  }

  void BoostGraphWrapper::setClusterXDisplacement(SubGraph& gSubgraph,int iDisp)
  {
      PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
      mapGraphProp[&gSubgraph].iClusterXDisplacement = iDisp;
  }

  int BoostGraphWrapper::getClusterYDisplacement(SubGraph& gSubgraph)
  {
      PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
      return mapGraphProp[&gSubgraph].iClusterYDisplacement;
  }

  void BoostGraphWrapper::setClusterYDisplacement(SubGraph& gSubgraph,int iDisp)
  {
      PGL_MAP_GRAPH_REF_PROPERTY(mapGraphProp , gSubgraph);
      mapGraphProp[&gSubgraph].iClusterYDisplacement = iDisp;
  }









