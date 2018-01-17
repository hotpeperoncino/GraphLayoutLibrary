#include "GraphMLWriter.h"

GraphMLWriter::GraphMLWriter()
{

}

void GraphMLWriter::writeGraphml(SubGraph &gBoostGraph, QIODevice *outStreamFile)
{
    try
    {
        m_boostGraphWrapper = new BoostGraphWrapper(gBoostGraph);

        //cout << "Graph at Writer: \n";
        //print_graph(gBoostGraph);

        //cout <<"vertex-coordX, coordY \n";
        //    BGL_FORALL_VERTICES(v,gBoostGraph,SubGraph)
        //    {
        //        //print vertex, x, y
        //        //cout << PGL_VERTEX_INDEX(v,gBoostGraph)
        //             //<<" : " << m_boostGraphWrapper->getVertexCenterCoordX(v,gBoostGraph)
        //            //<<" - " << m_boostGraphWrapper->getVertexCenterCoordY(v,gBoostGraph)
        //              //<< endl;
        //    }

        populateDataTagProperties();

        //Set outstream
        m_txtStreamOut.setDevice(outStreamFile);
        m_txtStreamOut.setCodec("UTF-8");

        LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                       , "File stream must be write enabled"
                                       ,"Text Stream"));

        LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                      ,LayoutException(__FUNCTION__
                                       , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                       , "File stream encoding must be UTF 8"
                                       , "Text Stream"));

        m_txtStreamOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                       << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\">\n";

        //Generate <key> declarations in graphml and create a Map of it.
        //For example, Map("node","color") to "key0" key
        //Key Declaration Example:
        //<key id="key0" for="node" attr.name="color" attr.type="string">

        //cout << endl << "Writing graphml..." <<endl;

        generateKeyDeclaration();

        //iterate subgraph child subgraphs, direct nodes, edges
        generateGraphml(gBoostGraph);

        m_txtStreamOut << "</graphml>\n";

        m_txtStreamOut.flush();

    }
    catch(boost::exception &eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(LayoutException &eLayoutException)
    {
        throw eLayoutException;
    }
    catch(LayoutMemoryException &eMemoryException)
    {
        throw eMemoryException;
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }


}

void GraphMLWriter::generateGraphmlRecur(SubGraph &gSubGraph, SubGraph &gMainGraph, int iLevel)
{
    LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream must be write enabled"
                                   ,"Text Stream"));

    LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream encoding must be UTF 8"
                                   , "Text Stream"));

    QString sGraphCounterVal = QString::number(m_iGraphCounter);

    QString sGraphId = m_boostGraphWrapper->getGraphId(gSubGraph);

    //Current graph
    m_txtStreamOut << indent(iLevel).toUtf8()
                   <<"<graph " << GRAPH_ID << "=\""
                  <<sGraphId << "\" "
                 <<"edgedefault=\"" << DEFAULT_EDGE_TYPE <<"\""
                << ">\n";

    //Add current graph attributes
    addGraphDataAttribute(gSubGraph , iLevel+1 );

    //Subgraph

    VectorSubgraphRefs vecChildSubgraphRefs;
    m_boostGraphWrapper->getChildrenVector(gSubGraph , vecChildSubgraphRefs);
    for(VectorSubgraphRefs::iterator subgraphIter = vecChildSubgraphRefs.begin();
        subgraphIter != vecChildSubgraphRefs.end();
        subgraphIter++)
    {
        m_iGraphCounter++;
        sGraphCounterVal = QString::number(m_iGraphCounter);

        //add a new node for graph
        m_txtStreamOut << indent(iLevel + 1)
                       << "<node " << NODE_ID <<"=\""
                       << GRAPH_NODE_ID_PREFIX << sGraphId <<"\" "
                       << ">\n";

        //add graph to newly added node
        generateGraphmlRecur(**subgraphIter , gMainGraph, iLevel + 2);

        //cout << "Recuring through writer graph: \n";
        //print_graph(**subgraphIter);

        m_txtStreamOut << indent(iLevel+1) << "</node>\n";
    }

    //Vertices
    VertexIterator vertexIter , vertexIterEnd;

    for(boost::tie(vertexIter , vertexIterEnd) = vertices(gSubGraph);
        vertexIter != vertexIterEnd;
        vertexIter++)
    {
        VertexDescriptor vVertex = *vertexIter;

        //Skip expandable and invisible vertices
        if(m_boostGraphWrapper->getVertexIsInvisible(vVertex, gSubGraph)
            || m_boostGraphWrapper->getVertexExpandable(vVertex , gSubGraph) )
        {
            continue;
        }

        //Write only Graph Nodes
        LayoutEnum::NodeType enVertexType = m_boostGraphWrapper->getVertexType(vVertex , gSubGraph);
        if(enVertexType == LayoutEnum::GraphNode)
        {
            //add vertex
            addNode( vVertex , gSubGraph, iLevel+1 , gMainGraph);
        }
    }

    //Edges
    EdgeIterator edgeIter , edgeIterEnd;

    for(boost::tie(edgeIter , edgeIterEnd) = edges(gSubGraph);
        edgeIter != edgeIterEnd;
        edgeIter++)
    {
        EdgeDescriptor eEdge = *edgeIter;

        //Skip invisible edges
        if(m_boostGraphWrapper->getEdgeIsInvisible(eEdge , gSubGraph))
        {
            continue;
        }

        //Skip long edge segments
        LayoutEnum::EdgeType enEdgeType = m_boostGraphWrapper->getEdgeType(eEdge , gSubGraph);
        if(enEdgeType == LayoutEnum::LongEdgeSegment)
        {
            continue;
        }

        if(enEdgeType == LayoutEnum::GraphEdge)
        {
            //add edge
            addEdge(eEdge , gSubGraph , iLevel+1 , gMainGraph);
        }
    }

    //Current graph end
    m_txtStreamOut << indent(iLevel).toUtf8() << "</graph>\n";
}

void GraphMLWriter::generateGraphml(SubGraph &gSubGraph)
{    
    LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream must be write enabled"
                                   ,"Text Stream"));

    LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream encoding must be UTF 8"
                                   , "Text Stream"));
    //reset graph counter
    m_iGraphCounter = 0;

    generateGraphmlRecur(gSubGraph ,gSubGraph, 0);
}

void GraphMLWriter::addGraphDataAttribute(SubGraph &gSubGraph, int iLevel)
{
    //cout << "Graph while writing data props: \n";
    //print_graph(gSubGraph);

    LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream must be write enabled"
                                   ,"Text Stream"));

    LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream encoding must be UTF 8"
                                   , "Text Stream"));
    Q_UNUSED(gSubGraph);

    //graph coord_x
    int iGraphCoordX = m_boostGraphWrapper->getGraphLeftTopCoordX(gSubGraph);
    QString sGraphCoordX = QString::number(iGraphCoordX);
    QString sGraphCoordXKey = getKey("graph" , "coord_x");

    //cout << " iGraphCoordX: " << iGraphCoordX
         //<< " - " << sGraphCoordX.toStdString()
            //<< endl;

    addData(sGraphCoordXKey , sGraphCoordX , iLevel);

    //graph coord_y
    int iGraphCoordY = m_boostGraphWrapper->getGraphLeftTopCoordY(gSubGraph);
    QString sGraphCoordY = QString::number(iGraphCoordY);
    QString sGraphCoordYKey = getKey("graph" , "coord_y");

    addData(sGraphCoordYKey , sGraphCoordY , iLevel);

    //graph height
    int iGraphHeight = m_boostGraphWrapper->getGraphHeight(gSubGraph);
    QString sGraphHeight = QString::number(iGraphHeight);
    QString sGraphHeightKey = getKey("graph" , "height");

    addData(sGraphHeightKey , sGraphHeight , iLevel);

    //graph width
    int iGraphWidth = m_boostGraphWrapper->getGraphWidth(gSubGraph);
    QString sGraphWidth = QString::number(iGraphWidth);
    QString sGraphWidthKey = getKey("graph" , "width");

    addData(sGraphWidthKey , sGraphWidth , iLevel);
}

void GraphMLWriter::addNode(VertexDescriptor &vVertex, SubGraph &gSubGraph, int iLevel, SubGraph &gMainGraph)
{
    LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream must be write enabled"
                                   ,"Text Stream"));

    LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream encoding must be UTF 8"
                                   , "Text Stream"));

    VertexDescriptor vGlobalVertex = gSubGraph.local_to_global(vVertex);

    int iGlobalVertexIndex = get(vertex_index , gMainGraph , vGlobalVertex);

    //visit each vertex only once
    if(isGlobalVertexVisited(iGlobalVertexIndex))
    {
        return;
    }

    //Get vertex property values

    //node id
    QString sNodeId = m_boostGraphWrapper->getVertexId(vVertex , gSubGraph);

    m_txtStreamOut << indent(iLevel)
                   << "<node " << NODE_ID << "=\""
                   << sNodeId.toUtf8() << "\""
                   <<">\n";

    //node height
    int iNodeHeight = m_boostGraphWrapper->getVertexHeight(vGlobalVertex , gMainGraph);

    QString sNodeHeight;
    sNodeHeight =QString::number(iNodeHeight);

        QString sNodeHeightKey = getKey("node","height");
        addData( sNodeHeightKey , sNodeHeight , iLevel+1);

    //node width
    int iNodeWidth = m_boostGraphWrapper->getVertexWidth(vGlobalVertex , gMainGraph);


    QString sNodeWidth;
    sNodeWidth =QString::number(iNodeWidth);

        QString sNodeWidthKey = getKey("node" ,"width");
        addData( sNodeWidthKey , sNodeWidth , iLevel+1);

    //node coord_x                --------- changing pritish
    int iNodeCoordX = m_boostGraphWrapper->getVertexLeftCoordX(vGlobalVertex , gMainGraph);
    QString sNodeCoordX =QString::number(iNodeCoordX);



        QString sNodeCoordXKey = getKey("node","coord_x");
        addData( sNodeCoordXKey , sNodeCoordX , iLevel+1);

    //node coord_y
    int iNodeCoordY = m_boostGraphWrapper->getVertexLeftCoordY(vGlobalVertex , gMainGraph);
    QString sNodeCoordY =QString::number(iNodeCoordY);

        QString sNodeCoordYKey = getKey("node","coord_y");
        addData( sNodeCoordYKey , sNodeCoordY , iLevel+1);

    m_txtStreamOut << indent(iLevel) << "</node>\n";

    markGlobalVertexVisited(iGlobalVertexIndex);
}

void GraphMLWriter::addEdge(EdgeDescriptor &eEdge, SubGraph &gSubGraph, int iLevel, SubGraph &gMainGraph)
{
    LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream must be write enabled"
                                   ,"Text Stream"));

    LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream encoding must be UTF 8"
                                   , "Text Stream"));

    EdgeDescriptor eGlobalEdge = gSubGraph.local_to_global(eEdge);

    int iGlobalEdgeIndex = get(edge_index , gMainGraph , eGlobalEdge);

    //visit each edge only once
    if(isGlobalEdgeVisited(iGlobalEdgeIndex))
    {
        return;
    }

    //Get edge properties

    //edge id
    QString sEdgeId = m_boostGraphWrapper->getEdgeId(eGlobalEdge , gMainGraph);

    //edge source and target

    //Taking out source and target vertex from boost edge e
    VertexDescriptor vGlobalSourceVertex = gSubGraph.local_to_global(source(eEdge , gSubGraph));
    VertexDescriptor vGlobalTargetVertex = gSubGraph.local_to_global(target(eEdge , gSubGraph));

    QString sEdgeSource = m_boostGraphWrapper->getVertexId(vGlobalSourceVertex , gMainGraph);
    QString sEdgeTarget = m_boostGraphWrapper->getVertexId(vGlobalTargetVertex , gMainGraph);

    m_txtStreamOut << indent(iLevel)
                   << "<edge "<<EDGE_ID<<"=\""
                   << sEdgeId.toUtf8() << "\" "
                   << "source=\"" << sEdgeSource << "\" "
                   << "target=\"" << sEdgeTarget << "\">\n";

    //bidirectional - true or false

    bool bEdgeBidirectional = m_boostGraphWrapper->getEdgeBidirectional(eGlobalEdge , gMainGraph);

    QString sEdgeBidirectional;
    if(bEdgeBidirectional)
    {
        sEdgeBidirectional = "true";
    }
    else
    {
        sEdgeBidirectional = "false";
    }

    QString sBidirectionalKey = getKey("edge" , "bidirectional");
    addData(sBidirectionalKey , sEdgeBidirectional , iLevel+1 );

    // add bends HERE
    QString sBendDataKey = getKey("edge" , "bendpoints");
    addBendPointsData(sBendDataKey, iLevel + 1, eEdge, gSubGraph);

    m_txtStreamOut << indent(iLevel) << "</edge>\n";

    markGlobalEdgeVisited(iGlobalEdgeIndex);
}

void GraphMLWriter::populateDataTagProperties()
{
    //add those attributes which should be represented in a <data> tag

    //node property list
    GraphElementProperties propListNode;

    propListNode.lstAttributeList.push_back(Attribute("height","integer",true,"40"));
    propListNode.lstAttributeList.push_back(Attribute("width","integer",true,"40"));

    propListNode.lstAttributeList.push_back(Attribute("coord_x","integer"));
    propListNode.lstAttributeList.push_back(Attribute("coord_y","integer"));

    //edge property list
    GraphElementProperties propListEdge;

    propListEdge.lstAttributeList.push_back(Attribute("bidirectional","boolean",true,"false"));
    propListEdge.lstAttributeList.push_back(Attribute("bendpoints", "interger"));

    //graph property list
    GraphElementProperties propListGraph;

    propListGraph.lstAttributeList.push_back(Attribute("coord_x" , "integer"));
    propListGraph.lstAttributeList.push_back(Attribute("coord_y" , "integer"));
    propListGraph.lstAttributeList.push_back(Attribute("height" , "integer"));
    propListGraph.lstAttributeList.push_back(Attribute("width" , "integer"));
    //Add element along with their atrribute list in map

    m_mapGraphElementToAttributeList.insert("node" , propListNode);
    m_mapGraphElementToAttributeList.insert("edge", propListEdge);
    m_mapGraphElementToAttributeList.insert("graph", propListGraph);
}

void GraphMLWriter::markGlobalVertexVisited(int iVertexId)
{
    m_mapVisitedVertices.insert(iVertexId , true);
}

bool GraphMLWriter::isGlobalVertexVisited(int iVertexId)
{
    return m_mapVisitedVertices.value(iVertexId);
}

void GraphMLWriter::markGlobalEdgeVisited(int iEdgeId)
{
    m_mapVisitedEdges.insert(iEdgeId , true);
}

bool GraphMLWriter::isGlobalEdgeVisited(int iEdgeId)
{
    return m_mapVisitedEdges.value(iEdgeId);
}

void GraphMLWriter::generateKeyDeclaration()
{

    LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream must be write enabled"
                                   ,"Text Stream"));

    LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream encoding must be UTF 8"
                                   , "Text Stream"));

    LAYOUT_ASSERT(m_mapGraphElementToAttributeList.empty() == false
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::EMPTY_CONTAINER
                                   , ""
                                   , "MapGraphElementToAttributeList"));

    //Example: <key id="d0" for="node" attr.name="color" attr.type="string">
    int iKeyCounter;
    QString sElementName;
    QString sAttributeName;
    QString sAttributeType;
    QString sKeyId;

    iKeyCounter = 0;
    QMapIterator<QString , GraphElementProperties > elementIter(m_mapGraphElementToAttributeList);
    while(elementIter.hasNext())
    {
        elementIter.next();
        sElementName = elementIter.key(); // "node", "graph" or "edge"...

        QListIterator<Attribute> iterAttributeList = ((GraphElementProperties)(elementIter.value())).iterator();
        while(iterAttributeList.hasNext())
        {
            Attribute attribute = iterAttributeList.next();
            // create key id
            sKeyId = KEY_ID_PREFIX + QString::number(iKeyCounter++);

            //Attribute name
            sAttributeName = attribute.sName;

            //Attribute Type
            sAttributeType = attribute.sType;

            //set <<ElementName,ElementAttribute>,Key> map
            //Example: << "node", "coord" > "key0" >

            ElementNamePropertyNamePair elementNamePropertyPair
                    = ElementNamePropertyNamePair( sElementName
                                                , sAttributeName );

            m_mapElementNameAndPropertyToKey.insert(elementNamePropertyPair
                                                    , sKeyId);

            //Add Key Declaration to graphml file
            m_txtStreamOut <<"<key id = \"" << sKeyId.toUtf8() <<"\" "
                           <<"for = \"" << sElementName.toUtf8() << "\" "
                          <<"attr.name = \"" << sAttributeName.toUtf8() <<"\" "
                         <<"attr.type = \"" << sAttributeType.toUtf8() <<"\"";

            //Default value
            if(attribute.bHasDefaultValue)
            {
                m_txtStreamOut << ">\n";

                //add default value tag
                m_txtStreamOut << indent(1)
                               <<"<default>"<<attribute.sDefaultValue.toUtf8()
                              <<"</default>\n";

                m_txtStreamOut << "</key>\n";
            }
            else
            {
                m_txtStreamOut << "/>\n";
            }
        }
    }
}

void GraphMLWriter::printAttributeTable()
{

    QMapIterator<QString , GraphElementProperties > iter(m_mapGraphElementToAttributeList);
    while (iter.hasNext()) {
        iter.next();
        //cout << (iter.key()).toStdString() << endl;
        ((GraphElementProperties)(iter.value())).print();
    }

}

QString GraphMLWriter::indent(int iLevel)
{
    QString sIndent = "";
    int iIndentSize = 4 * iLevel;
    for(int indentSizeCounter = 0;
        indentSizeCounter < iIndentSize;
        indentSizeCounter++)
    {
        sIndent += " ";
    }

    return sIndent;
}

QString GraphMLWriter::getKey(QString sElementName , QString sAttributeName)
{
    QString sKey;
    try{
        sKey = m_mapElementNameAndPropertyToKey
                .value(ElementNamePropertyNamePair(sElementName,sAttributeName));
    }
    catch(...)
    {
        throw LayoutException(__FUNCTION__ , LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION);
    }

    if(sKey == "")
    {

        throw LayoutException(__FUNCTION__
                              , LayoutExceptionEnum::EMPTY_CONTAINER
                              , ""
                              ,"Key");
    }
    return sKey;
}

void GraphMLWriter::addData(QString sKey , QString sValue, int iLevel)
{
    LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream must be write enabled"
                                   ,"Text Stream"));

    LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream encoding must be UTF 8"
                                   , "Text Stream"));

    LAYOUT_ASSERT(sKey.length() > 0
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , sKey
                                   ,"Key"));
    LAYOUT_ASSERT(iLevel >= 0
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , QString::number(iLevel)
                                   ,"Level"));

    m_txtStreamOut << indent(iLevel)
                   << "<data key=\"" << sKey << "\">"
                   <<sValue<<"</data>\n";
}

void GraphMLWriter::addBendPointsData(QString sKey, int iLevel, EdgeDescriptor& eEdge, SubGraph& gSubgraph)
{
    LAYOUT_ASSERT(m_txtStreamOut.device()->isWritable() == true
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream must be write enabled"
                                   ,"Text Stream"));

    LAYOUT_ASSERT(strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "File stream encoding must be UTF 8"
                                   , "Text Stream"));

    LAYOUT_ASSERT(sKey.length() > 0
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                   , "string sKey"
                                   , ""));


    LAYOUT_ASSERT(iLevel >= 0
                  ,LayoutException(__FUNCTION__
                                   , LayoutExceptionEnum::INVALID_PARAMETER
                                   , "it must be positive"
                                   , "iLevel"));


    /**
      Encorporate Bend Points in the following format:-

        <edge target="s13" id="s12_R_15_s13" source="s12">
           <data key="key_8">
           <bend X="10" Y "30" />
           </data>
        </edge>
    */

    // start of the data tag inside a edge tag for encorporating the bends.
    m_txtStreamOut << indent(iLevel)
                   << "<data key=\"" <<sKey << "\">"
                   << "\n";

    // get the number of BendPoints objects in the list of bendpoints.
    // get the x and y coordinates from the vector which has list of edge's bend points.
    // create equal number of bend tags in that edge.
    // end of data tag inside a edge tag for bends.

    IteratorQVectorBendPoints iterBendPoints, iterBendPointsEnd;
    BoostGraphWrapper boostGraphWrapper;
    for(boost::tie(iterBendPoints, iterBendPointsEnd) = boostGraphWrapper.edgeBendPointsIter(eEdge, gSubgraph);
        iterBendPoints != iterBendPointsEnd;
        iterBendPoints++)
    {
        BendPoints structBendPoints = **iterBendPoints;
        QString sEdgeId = boostGraphWrapper.getEdgeId(eEdge,gSubgraph);
        //cout<<"Bend Points for Edge : "<<sEdgeId.toStdString()<<" "
           //<<structBendPoints.iCoordinateX<<" "<<structBendPoints.iCoordinateY<<endl;
        m_txtStreamOut << indent(iLevel+1)
                       << "<bend X = \"" <<structBendPoints.iCoordinateX
                       <<"\" Y = \"" << structBendPoints.iCoordinateY << "\">"
                       << "\n";
        m_txtStreamOut << indent(iLevel+1)
                      << "</bend>\n";
    }

    m_txtStreamOut << indent(iLevel)
                   << "</data>\n";

}

