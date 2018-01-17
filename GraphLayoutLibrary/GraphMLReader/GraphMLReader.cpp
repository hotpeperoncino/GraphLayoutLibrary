#include "GraphMLReader.h"

#include <iostream>
#include <qstring.h>
#include <QTime>
#include <QDebug>
#include <QMap>

//create graph with subgraphs - global
SubGraph* gMainGraph = NULL;
SubGraph* gCurrentSubGraph = NULL;

GraphMLReader::GraphMLReader()
{
    //SubGraph gMainGraph;
    gMainGraph = new SubGraph();

    // this will maintain the cluster hirarchy in the subgraph structure
    gCurrentSubGraph = gMainGraph;

    // to track the start and end of the graphml tag.
    m_bGraphMLTag = false;

    // to track node occures after graph immediately
    m_bGraphTag = false;

    // to track bend inside data which inside edge tags.
    m_bEdgeTag = false;
    m_bEdgeDataTag = false;

    // to perform validation of data values for nodes, graphs and edges.
    // for graph
    m_bIsGraphCoordXSet = false;
    m_bIsGraphCoordYSet = false;
    m_bIsGraphHeightSet = false;
    m_bIsGraphWidthSet = false;

    // for nodes
    m_bIsNodeCoordXSet = false;
    m_bIsNodeCoordYSet = false;
    m_bIsNodeHeightSet = false;
    m_bIsNodeWidthSet = false;

    // for edge
    m_bIsEdgeDirectionSet = false;
}

//Function Definitions
SubGraph& GraphMLReader::readGraphML(QFile *file)
{
    GraphMLReader graphMLReader;
    //**** Timer Start
    QTime t;
    qDebug() << t.currentTime();
    t.start();

    //**** Counter Start
    m_iNodeStartAt = 0;
    m_iNodeEndAt = 0;
    m_iGraphStartAt = 0;
    m_iNode_GraphFoundCounter = 0;

    try
    {
        // validation of graphml file
        QString sFileName = file->fileName();
        //cout<<"Input File Validation : File Name "<<sFileName.toStdString()<<endl;

        //validateGraphML(sFileName); //Commented to test Bend points reading
    }
    catch(LayoutFileIOException& eException)
    {
        throw LayoutFileIOException(__FUNCTION__,
                              LayoutExceptionEnum::LAYOUT_INVALID_FILE_FORMAT,
                              eException.getFileName());
    }

    try
    {
        /*
            This calls default handler of SAX of Qt and takes the functionality of reading the
            graphML
        */

        QXmlInputSource xmlInputSource(file);
        QXmlSimpleReader xmlSimpleReader;
        xmlSimpleReader.setContentHandler(&graphMLReader);
        xmlSimpleReader.parse(xmlInputSource);
    }
    catch(LayoutFileIOException& e)
    {
        file->close();
        if(e.getExceptionSubType() == LayoutExceptionEnum::UNSUPPORTED_FILE_TYPE)
        {

        }
        QString sFileName = e.getFileName();
        QString sErrorMessage = e.getErrorMessage();
        //cout<<"Exception : "<<sErrorMessage.toStdString()<<endl;
        throw LayoutFileIOException(__FUNCTION__,
                              LayoutExceptionEnum::FILE_READ_OPERATION_ERROR,
                              sFileName);
    }
    catch(LayoutMemoryException& eException)
    {
        file->close();
        QString sErrorMessage = eException.getErrorMessage();
        QString sObjectName = eException.getObjectName();
        //cout<<"Exception : "<<sErrorMessage.toStdString()<<" "<<sObjectName.toStdString()<<endl;
        throw LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, sObjectName);
    }
    catch(LayoutException& eException)
    {
        file->close();
        QString sErrorMessage = eException.getErrorMessage();
        int iErrorCode = eException.getErrorCode();
        QString sEntityValue = eException.getEntityValue();
        QString sEntityType = eException.getEntityType();
        //cout<<"Exception : "<<sErrorMessage.toStdString()<<" "<<iErrorCode<<" "<<sEntityType.toStdString()<<" "<<sEntityValue.toStdString()<<endl;
        throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_PARAMETER,sEntityValue,sEntityType);
    }
    catch(boost::exception& eBoostException)
    {
        file->close();
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }
    catch(...)
    {
        file->close();
        throw;
    }

    qDebug() << "Duration : " << t.elapsed();
    int iTime=t.elapsed();
    qDebug()<<"\nTime : "<<iTime;

    // cleaning for maingraph will be done by the layout manager.
    return *gMainGraph;
}

bool GraphMLReader::isSupportedGraphmlFile(QString sGraphMLFileName)
{
    bool bIsSupportedFormat = false;
    if(sGraphMLFileName.trimmed().isEmpty() == false)
    {
        if(sGraphMLFileName.trimmed().endsWith(GRAPHML,Qt::CaseInsensitive) == true)
        {
            bIsSupportedFormat = true;
        }
    }
    else
    {
        bIsSupportedFormat = false;
    }

    return bIsSupportedFormat;
}

void GraphMLReader::validateGraphML(QString sGraphMLInstanceFileName)
{
    LAYOUT_ASSERT((sGraphMLInstanceFileName.trimmed().isEmpty() != true),
                  LayoutFileIOException(__FUNCTION__,
                                  LayoutExceptionEnum::INVALID_FILE_NAME,
                                  sGraphMLInstanceFileName));

//    LAYOUT_ASSERT(isSupportedGraphmlFile(sGraphMLInstanceFileName) != true,
//                  FileIOException(__FUNCTION__,
//                                  ExceptionEnum::INVALID_FILE_NAME,
//                                  sGraphMLInstanceFileName));

    QString sSchemaFileName = SCHEMA_FILE_PATH;
    if (sSchemaFileName.isEmpty())
    {
        throw LayoutFileIOException(__FUNCTION__,
                              LayoutExceptionEnum::FILE_NOT_FOUND,
                              sSchemaFileName);
        return;
    }

    qDebug() << "Selected schema = " << sSchemaFileName;
    qDebug() << "Selected instance = " << sGraphMLInstanceFileName;

    QFile qSchemaFile(sSchemaFileName);
    if (!qSchemaFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw LayoutFileIOException(__FUNCTION__,
                              LayoutExceptionEnum::FILE_READ_OPERATION_ERROR,
                              sSchemaFileName,qSchemaFile.errorString());
        return;
    }

    QFile qInstanceFile(sGraphMLInstanceFileName);
    if (!qInstanceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw LayoutFileIOException(__FUNCTION__,
                              LayoutExceptionEnum::FILE_READ_OPERATION_ERROR,
                              sGraphMLInstanceFileName,qInstanceFile.errorString());
        return;
    }
    // reads the schema file into bytearray variable
    const QByteArray schemaData = qSchemaFile.readAll();
    // reads the graphml file into bytearray variable
    const QByteArray instanceData = qInstanceFile.readAll();

    // declare the messagehandler for getting the wrong instance of graphml file.
    MessageHandler messageHandler;
    // declare the xml schema object.
    QXmlSchema schema;
    schema.setMessageHandler(&messageHandler);

    bool bErrorOccurred = false;

    qDebug() << "Loading Schema";
    if (schema.load(schemaData) == true)
    {
        qDebug() << "Validating Schema";
        if (schema.isValid())
        {
            QXmlSchemaValidator validator(schema);
            qDebug() << "Validating Instance";
            if (!validator.validate(instanceData))
            {
                // set the flag of whether error present or not.
                bErrorOccurred = true;
            }
        }
    }
    else
    {
        bErrorOccurred = true;
    }

    if (bErrorOccurred)
    {
        qDebug() << "Validation Un - Successful";
        qDebug() << messageHandler.getStatusMessage()
                 <<" "<<messageHandler.getLineNumber()
                 <<" "<<messageHandler.getColumnNumber();
        std::cout<< messageHandler.getStatusMessage().toStdString()<<" "<<messageHandler.getLineNumber()<<" "<<messageHandler.getColumnNumber()<<endl;
        throw LayoutFileIOException(__FUNCTION__,
                              LayoutExceptionEnum::LAYOUT_INVALID_FILE_FORMAT,
                              sGraphMLInstanceFileName,messageHandler.getStatusMessage());
    }
    else
    {
        qDebug() << "Validation Successful";
    }

    // Close the files once task is done.
    qSchemaFile.close();
    qInstanceFile.close();
}

/*Handler functionality*/
bool GraphMLReader::startElement( const QString &namespaceURI, const QString &sTagElement, const QString &qName,
                                  const QXmlAttributes &lstAttributes)
{

    Q_UNUSED(namespaceURI);
    Q_UNUSED(qName);

    //  Clock counter to keep ordering between the tag elements
    ++m_iNode_GraphFoundCounter;

    if (!m_bGraphMLTag && sTagElement != GRAPHML)
    {
        // if graphml file do not contain the graphml tag then set the error.
        m_sErrorText = NOT_GRAPHML_FILE;
        return false;
    }

    if(sTagElement == XML_EXTN)
    {
        // Graphml file's first tag is xml and check its version and encoding.
        QString sVersion = lstAttributes.value(VERSION);
        QString sEncoding = lstAttributes.value(ENCODING);

        // check graphml version and encoding suported by reader
        if ((!sVersion.isEmpty() || sVersion != VERSION_VALUE))
        {
            // set the the error value if version is wrong.
            m_sErrorText = INVALID_VERSION;
            throw LayoutFileIOException(__FUNCTION__,
                                  LayoutExceptionEnum::UNSUPPORTED_FILE_FORMAT,
                                  VERSION);
        }
        else if(!sEncoding.isEmpty() || sEncoding != ENCODING_VALUE)
        {
            // set the error value if encoding id wrong.
            m_sErrorText = INVALID_ENCODING;
            throw LayoutFileIOException(__FUNCTION__,
                                  LayoutExceptionEnum::UNSUPPORTED_FILE_FORMAT,
                                  VERSION);
        }
    }

    if (sTagElement == GRAPHML)
    {
        QString sVersion = lstAttributes.value(VERSION);
        if (!sVersion.isEmpty() && sVersion != VERSION_VALUE)
        {
            m_sErrorText = INVALID_VERSION;
            return false;
        }
        m_bGraphMLTag = true;
    }

    else if(sTagElement.compare(GRAPH,Qt::CaseInsensitive) == 0)
    {
        /*
            if the tag element is <graph>
        */

        // clear the current characters value.
        m_sCurrentText.clear();

        // set the value of graph start to global counter.
        m_iGraphStartAt = m_iNode_GraphFoundCounter;

        // this flag is for having bounding between node occures after graph immediately.
        m_bGraphTag = true;

        // to get the id from <graph tag> which is attribute of graph
        QString strId;
        int iAttrCount=lstAttributes.count();
        for( int iIndex=0; iIndex<iAttrCount; iIndex++ )
        {
            if( lstAttributes.localName( iIndex ) == GRAPH_ID )
            {
                strId = lstAttributes.value( iIndex );
            }
        }

        // set the graph id to current domain id in the structure
        m_sCurrentDomainId = strId;
        m_sCurrentDomain = GRAPH;
        // Checking if <graph> tag is found just after <node> tag
        if(m_iGraphStartAt - m_iNodeStartAt==1)
        {
            // Add SubGraph and Reset dirtyNode;
            SubGraph& gNewChildSubgraph= gCurrentSubGraph->create_subgraph();
            gCurrentSubGraph = &gNewChildSubgraph;

            // for childgraphs : Storing properties
            boost::ref_property_map<SubGraph*, GraphProperties>
                    mapGraphReferenceToId(boost::get_property(*gCurrentSubGraph,graph_custom_prop));
            mapGraphReferenceToId[gCurrentSubGraph].sId = strId;

            m_dirtyNode.m_bIsDirtyNode = false;
        }
        else
        {
            // for maingraph :- Storing properties
            boost::ref_property_map<SubGraph*, GraphProperties>
                    mapGraphReferenceToProperty(boost::get_property(*gCurrentSubGraph,graph_custom_prop));
            mapGraphReferenceToProperty[gCurrentSubGraph].sId = strId;
        }
    }

    if(sTagElement.compare(NODE,Qt::CaseInsensitive) == 0)
    {
        /*
            if the tag element is the <node> tag.
        */

        /**
            As node tag follows immediately to graph tag always so after reading graph tag we need to store the
            graph properties in the specific graph. hence store the graph properties at the start of the node tag.
            Actully these properties need to be stored at the start of the edge tag also but there cannot be an edge
            without the node hence we have decided to store these properties at the start of ned tag only.
        */

        /* set graph properties start */
        if(m_bGraphTag == true)
        {
            /**
                Actually Writer graphml file contains the x and y coordinates which are already leftX and topY
                coordinates of compartment.
                Hence while reading we have to treat those coordinates as leftX and topY coordinates.
            */


            /*
                Check for all madatory fields for the graph are present or not like
                coordinate X, coordinate Y, Height and Width.
            */
            if(m_bIsGraphCoordXSet && m_bIsGraphCoordYSet && m_bIsGraphHeightSet && m_bIsGraphWidthSet)
            {
                BoostGraphWrapper boostGraphWrapper;
                // retrieve properties from graph property strcuture.
                // Here we take X and Y as leftX and leftY.
                int iGraphLeftTopCoordX = structGraphProperty.iCoordX;
                int iGraphLeftTopCoordY = structGraphProperty.iCoordY;
                int iGraphHeight = structGraphProperty.iHeight;
                int iGraphWidth = structGraphProperty.iWidth;

                // center X and Y coordinates calculations
                int iGraphCenterCoordX = (iGraphLeftTopCoordX + (iGraphWidth/2));
                int iGraphCenterCoordy = (iGraphLeftTopCoordY + (iGraphHeight/2));

                QString sGraphId = boostGraphWrapper.getGraphId(*gCurrentSubGraph);
//                //cout<<"Setting The Graph Properties : "<<sGraphId.toStdString()<<" "<<iGraphLeftTopCoordX<<" "<<iGraphLeftTopCoordY
//                   <<" "<<iGraphHeight<<" "<<iGraphWidth<<endl;

                // set graph properties to boost graph
                boostGraphWrapper.setGraphCenterCoordX(iGraphCenterCoordX, *gCurrentSubGraph);
                boostGraphWrapper.setGraphCenterCoordY(iGraphCenterCoordy, *gCurrentSubGraph);
                boostGraphWrapper.setGraphHeight(iGraphHeight, *gCurrentSubGraph);
                boostGraphWrapper.setGraphWidth(iGraphWidth, *gCurrentSubGraph);

                // set left and top properties to boost graph
                boostGraphWrapper.setGraphLeftTopCoordX(iGraphLeftTopCoordX, *gCurrentSubGraph);
                boostGraphWrapper.setGraphLeftTopCoordY(iGraphLeftTopCoordY, *gCurrentSubGraph);

                // set the imediate graph tag before this node tag as false.
                m_bGraphTag = false;

                // reset the flag for data values for this graph
                m_bIsGraphCoordXSet = false;
                m_bIsGraphCoordYSet = false;
                m_bIsGraphHeightSet = false;
                m_bIsGraphWidthSet = false;
            }
            else
            {
                /**
                    This case indicates that one or more values of mandatory fields is absent for the graph entity.

                */
                throw LayoutException(__FUNCTION__, LayoutExceptionEnum::MANDATORY_FIELD_MISSING, GRAPH);
            }
        }
        /* set graph properties End */

        m_sCurrentText.clear();

        // set the global counter value to node counter.
        m_iNodeStartAt = m_iNode_GraphFoundCounter;
        m_dirtyNode.m_bIsDirtyNode = true;

        // process for the getting and setting the node id.
        QString sNodeId;
        int iAttrCount = lstAttributes.count();
        for( int iIndex=0; iIndex<iAttrCount; iIndex++ )
        {
            if( lstAttributes.localName( iIndex ) == NODE_ID )
            {
                sNodeId = lstAttributes.value( iIndex );
                // To store attributes which can be used in endElement(), set those into DirtyNode class.
                qDebug()<<"Set Node Ids:"<<sNodeId;
                try
                {
                    // This will call the setter for the NODE class and set its properties.
                    m_dirtyNode.setID(sNodeId);
                }
                catch(LayoutException& eException)
                {
                    //cout<<"Exception : Empty id to the node found"<<endl;
                    throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_PARAMETER,
                                          eException.getEntityValue(), eException.getEntityType());
                }
            }
        }

        // set the value of node's id to current domain id.
        m_sCurrentDomainId = sNodeId;
        m_sCurrentDomain = NODE;
    }

    if(sTagElement.compare(EDGE,Qt::CaseInsensitive) == 0)
    {
        m_sCurrentText.clear();
        m_dirtyEdge.m_bIsDirtyEdge = true;
        // getting and setting the edge ids
        QString sEdgeId,sSourceVertexId,sTargetVertexId;
        int iAttrCount=lstAttributes.count();
        for( int iIndex=0; iIndex<iAttrCount; iIndex++ )
        {
            if( lstAttributes.localName( iIndex ) == SOURCE_OF_EDGE)
            {
                sSourceVertexId = lstAttributes.value( iIndex );
            }
            if(lstAttributes.localName(iIndex)==TARGET_OF_EDGE)
            {
                sTargetVertexId = lstAttributes.value( iIndex );
            }
            if(lstAttributes.localName(iIndex) == EDGE_ID)
            {
                sEdgeId = lstAttributes.value( iIndex );
                try
                {
                    m_dirtyEdge.setId(sEdgeId);
                }
                catch(...)
                {
                    /* We don't have any processing related issue if any
                     * exception is geeting thrown here.
                     * Hence we are ignoring this exception.
                    */
                }
            }
        }

        // set the value of edge id to the current domain id.
        m_sCurrentDomainId = sEdgeId;
        m_sCurrentDomain = EDGE;

        // To store attributes which can be used in endElement(), set those into DirtyEdge class.
        try
        {
            m_dirtyEdge.setSourceTarget(sSourceVertexId,sTargetVertexId);
        }
        catch(LayoutException& eException)
        {
            LayoutException(__FUNCTION__,
                            LayoutExceptionEnum::INVALID_PARAMETER,
                            eException.getEntityValue(),
                            eException.getEntityType());
        }
        catch(...)
        {}

        // set the flag for edge true so that we can map it in the data tag.
        m_bEdgeTag = true;
    }
    if(sTagElement.compare(KEY,Qt::CaseInsensitive) == 0)
    {
        // set the information retrieved from the key tag
        structKeyInfo.sKeyId = lstAttributes.value(KEY_ID);
        structKeyInfo.sKeyName = lstAttributes.value(ATTR_NAME);
        structKeyInfo.keyType = QSTRING_TYPE;
        structKeyInfo.sDomain = lstAttributes.value(FOR);

        // set the type of the data need to be read.
        if(lstAttributes.value(ATTR_TYPE) == BOOL_TYPE_STRING)
        {
            structKeyInfo.keyType = BOOLEAN_TYPE;
        }
        else if(lstAttributes.value(ATTR_TYPE) == DOUBL_TYPE_STRING)
        {
            structKeyInfo.keyType = DOUBLE_TYPE;
        }
        else if(lstAttributes.value(ATTR_TYPE) == INT_TYPE_STRING)
        {
            structKeyInfo.keyType = INTIGER_TYPE;
        }

        //Add entries to map
        QString sKeyId = structKeyInfo.sKeyId;
        m_mapKeyIdToKeyInfo.insert(sKeyId,structKeyInfo);
        m_vecKeyInfo.push_back(structKeyInfo);
    }
    if(sTagElement.compare(DEFAULT,Qt::CaseInsensitive) == 0)
    {
        // default values need to be considered.
    }

    if(sTagElement.compare(DATA,Qt::CaseInsensitive) == 0)
    {
        // check for the data tag and get the information for which key this data belong to.
        QString sKey;
        int iAttrCount=lstAttributes.count();
        for( int iIndex=0; iIndex<iAttrCount; iIndex++ )
        {
            if( lstAttributes.localName( iIndex ) == KEY )
            {
                // get attribute of data tag.
                sKey = lstAttributes.value( iIndex );
            }
        }

        // set the current key id.
        m_sCurrentKeyId = sKey;

        // set the data flag true checking only if edge tag is true.
        if(m_bEdgeTag == true)
        {
            m_bEdgeDataTag = true;
        }

    }

    if(sTagElement.compare(BEND,Qt::CaseInsensitive) == 0)
    {
        // this tag is set when data for this edge is encontered.
        if(m_bEdgeDataTag == true)
        {
            QString sValueX, sValueY;
            int iIndex = 0;

            // get the first attribute value.
            if( lstAttributes.localName( iIndex ) == XVALUE)
            {
                sValueX = lstAttributes.value( iIndex );
            }

            // get the second attribute value.
            if(lstAttributes.localName(iIndex + 1)== YVALUE)
            {
                sValueY = lstAttributes.value( iIndex + 1 );
            }

            // store the values into dirty edge one by one for this edge.
//            //cout<<"Inside Bendss : "<<m_sCurrentDomainId.toStdString()<<" "
//               <<sValueX.toStdString()<<" "
//              <<sValueY.toStdString()<<" "<<endl;

            int iValueX = sValueX.toInt();
            int iValueY = sValueY.toInt();
//            //cout<<"Values for bends : "<<iValueX<<" "<<iValueY<<endl;
            m_dirtyEdge.setEdgeBendPointsCoordinates(iValueX, iValueY);

        }
    }
    return true;
}

bool GraphMLReader::characters(const QString &sCurrentString)
{
    // This function is default handler's function.
    // This function is used to retrieve the character text value betweent the tags.
    m_sCurrentText = sCurrentString;
    return true;
}

void GraphMLReader::convertTextToInt(QString sKeyValue)
{
    /**
        as we are retriving the values in the form of string only from the graphml file,
        hence we need to process it to convert it into the respective type.
        This function converts the string value into the integer value.
    */
    int iKeyValue = sKeyValue.toInt();
    m_iCurrentKeyValue = iKeyValue;
}

void GraphMLReader::convertTextToBool(QString sKeyValue)
{
    /**
        as we are retriving the values in the form of string only from the graphml file,
        hence we need to process it to convert it into the respective type.
        This function converts the string value into the boolean value.
    */
    if(sKeyValue.compare(TRUE_VALUE,Qt::CaseInsensitive) == 0)
    {
        m_bCurrentKeyValue = true;
    }
    if(sKeyValue.compare(FALSE_VALUE,Qt::CaseInsensitive) == 0)
    {
        m_bCurrentKeyValue = false;
    }
}

void GraphMLReader::convertTextToDouble(QString sKeyValue)
{
    /**
        as we are retriving the values in the form of string only from the graphml file,
        hence we need to process it to convert it into the respective type.
        This function converts the string value into the double value.
    */
    double dKeyValue = sKeyValue.toDouble();
    m_dCurrentKeyValue = dKeyValue;
}


bool GraphMLReader::endElement( const QString &namespaceURI, const QString &sTagElement, const QString &qName )
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(qName);

    // increment the global counter
    ++m_iNode_GraphFoundCounter;

    if(sTagElement.compare(GRAPHML, Qt::CaseInsensitive) == 0)
    {
        // declare the end of graphml tag.
        m_bGraphMLTag = false;
    }
    if(sTagElement.compare(GRAPH,Qt::CaseInsensitive) == 0)
    {
        // set the global counter value to the graph end counter value.
        m_iGraphEndAt = m_iNode_GraphFoundCounter;

        // add the properties of graph if node is not present in the graph
        if(m_bGraphTag == true)
        {

            /*
                Check for all madatory fields for the graph are present or not like
                coordinate X, coordinate Y, Height and Width.
            */
            if(m_bIsGraphCoordXSet && m_bIsGraphCoordYSet && m_bIsGraphHeightSet && m_bIsGraphWidthSet)
            {
                // store all propertes to graph
                BoostGraphWrapper boostGraphWrapper;
                // retrieve properties from graph property strcuture.
                // Here we take X and Y as leftX and leftY.
                int iGraphLeftTopCoordX = structGraphProperty.iCoordX;
                int iGraphLeftTopCoordY = structGraphProperty.iCoordY;
                int iGraphHeight = structGraphProperty.iHeight;
                int iGraphWidth = structGraphProperty.iWidth;

                // center X and Y coordinates calculations
                int iGraphCenterCoordX = (iGraphLeftTopCoordX + (iGraphWidth/2));
                int iGraphCenterCoordy = (iGraphLeftTopCoordY + (iGraphHeight/2));

//                QString sGraphId = boostGraphWrapper.getGraphId(*gCurrentSubGraph);
//                //cout<<"Setting The Graph Properties : "<<sGraphId.toStdString()<<" "<<iGraphLeftTopCoordX<<" "<<iGraphLeftTopCoordY
//                   <<" "<<iGraphHeight<<" "<<iGraphWidth<<endl;

                // set graph properties to boost graph
                boostGraphWrapper.setGraphCenterCoordX(iGraphCenterCoordX, *gCurrentSubGraph);
                boostGraphWrapper.setGraphCenterCoordY(iGraphCenterCoordy, *gCurrentSubGraph);
                boostGraphWrapper.setGraphHeight(iGraphHeight, *gCurrentSubGraph);
                boostGraphWrapper.setGraphWidth(iGraphWidth, *gCurrentSubGraph);

                // set left and top properties to boost graph
                boostGraphWrapper.setGraphLeftTopCoordX(iGraphLeftTopCoordX, *gCurrentSubGraph);
                boostGraphWrapper.setGraphLeftTopCoordY(iGraphLeftTopCoordY, *gCurrentSubGraph);

                m_bGraphTag = false;

                // reset the data vales present values's flag for graph
                m_bIsGraphCoordXSet = false;
                m_bIsGraphCoordYSet = false;
                m_bIsGraphHeightSet = false;
                m_bIsGraphWidthSet = false;
            }
            else
            {
                /**
                    This case indicates that one or more values of mandatory fields is absent for the graph entity.

                */
                throw LayoutException(__FUNCTION__, LayoutExceptionEnum::MANDATORY_FIELD_MISSING, GRAPH);
            }
        }

        // At the end of <graph> maintain the state of parent graph.
        SubGraph& gCurrentParentGraph = gCurrentSubGraph->parent();
        gCurrentSubGraph = &gCurrentParentGraph;
    }

    if(sTagElement.compare(NODE,Qt::CaseInsensitive) == 0)
    {

        // set the global counter value to node end counter.
        m_iNodeEndAt = m_iNode_GraphFoundCounter;

        QString strId;
        try
        {
        // fetch the attribute from DirtyNode.
        strId = m_dirtyNode.getID();
        }
        catch(LayoutException& eException)
        {
            throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE, EMPTY_ID_FOUND, NODE);
        }

        if( (m_iNodeEndAt < m_iNodeStartAt) && (m_iNodeStartAt < m_iGraphStartAt ) )
        {
            // Ignore Dummy node end
            m_dirtyNode.m_bIsDirtyNode = false;

        }
        /* Condition :- (NodeStartAt < NodeEndAt) AND NOT(NodeStartAt < GraphStartAt < NodeEndAt)
            Means <node>
                  <data/>
                  ....
                  </node>
                  AND No <graph> tag present in between NodeStart and NodeEnd.
                  I splitted above NOT(...) condition in c++ compiler readable way as:-
                  (a<b<c) means ( (a<b) && (b<c) ).

        */
        else if( ( m_iNodeStartAt < m_iNodeEndAt )
                 && !( ( m_iNodeStartAt < m_iGraphStartAt ) && ( m_iGraphStartAt < m_iNodeEndAt ) ) )
        {
            // Add vertex to graph;
            if( m_dirtyNode.m_bIsDirtyNode == true )
            {
                try
                {
                    addVertexToSubgraph(strId);
                    m_dirtyNode.m_bIsDirtyNode = false;
                }
                catch(LayoutException & eException)
                {
                    QString sErrorMessage = eException.getErrorMessage();
                    throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_PARAMETER, EMPTY_ID_FOUND, sErrorMessage);
                }
                catch(LayoutMemoryException& eException)
                {
                    QString sErrorMessage = eException.getErrorMessage();
                    //cout<<"Exception : Local_toClobal Null pointer excepton found"<<sErrorMessage.toStdString()<<endl;
                    throw LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, sErrorMessage);
                }
                catch(boost::exception& eBoostException)
                {
                    throw *boost::get_error_info<errmsg_info>(eBoostException);
                }
                catch(...)
                {
                    throw;
                }
            }
        }
    }

    if( sTagElement.compare(EDGE,Qt::CaseInsensitive) == 0 )
    {
        QString sSourceId , sTargetId , sEdgeId;

        // get the sourceId, targetId and EdgeId for the current edge.
        sSourceId = m_dirtyEdge.getSource();
        sTargetId = m_dirtyEdge.getTarget();
        sEdgeId = m_dirtyEdge.getId();

        // add edge to the graph
        if( m_dirtyEdge.m_bIsDirtyEdge == true )
        {
            try
            {
                addEdgeToMainGraph(sSourceId,sTargetId,sEdgeId);
                m_dirtyEdge.m_bIsDirtyEdge = false;

                // make it flase as edge end element occured
                m_bEdgeTag = false;
//                //cout<<"End Of Edge"<<endl;
            }
            catch(LayoutException& eException)
            {
                if(eException.getExceptionSubType() == LayoutExceptionEnum::INVALID_PARAMETER)
                {
                    throw LayoutException(__FUNCTION__,
                                          LayoutExceptionEnum::INVALID_PARAMETER,
                                          eException.getEntityValue(),
                                          eException.getEntityType());
                }
            }
            catch(LayoutMemoryException& eException)
            {
                throw LayoutMemoryException(__FUNCTION__,
                                      LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                      eException.getObjectName());
            }
        }
    }

    if(sTagElement.compare(KEY,Qt::CaseInsensitive) == 0)
    {
        // Get default value for current key
        structKeyInfo.sDefaultValue = m_sDefaultValue;
    }
    if(sTagElement.compare(DEFAULT,Qt::CaseInsensitive) == 0)
    {
        // Set default value
//         //cout<<"Default Value ~~~~~~~~~~~~~~~~~~~~~~"<<m_sCurrentText<<" "<<m_sCurrentKeyId<<endl;
        m_sDefaultValue = m_sCurrentText;
    }
    if( sTagElement.compare(DATA,Qt::CaseInsensitive) == 0)
    {
        // get the current data values in the structure.
        DataInfo structDataInfo;
        structDataInfo.sDomainID = m_sCurrentDomainId;
        structDataInfo.sDomain = m_sCurrentDomain;
        structDataInfo.sKeyId = m_sCurrentKeyId;
        size_t iNumKeyElements = m_vecKeyInfo.size();

        for(unsigned int iIndex = 0; iIndex < iNumKeyElements; iIndex++)
        {
            // TODO:: Use keys map instead of this vector.
            if(m_vecKeyInfo[iIndex].sKeyId == m_sCurrentKeyId)
                break;

            if(iIndex+1 == iNumKeyElements)
            {
                QFile file("C:/GraphmlErrorLog.txt");
                if (!file.open(QIODevice::Append |QIODevice::Text))
                {
                    qDebug() << "error:file could not be opened";
                }
                QTextStream out(&file);
                out << "The key " << m_sCurrentKeyId << "is not declared before\n";
                file.close();
            }
        }
        structDataInfo.sKeyValue = m_sCurrentText.remove("\r\n");

        KeyInfo objKeyInfo= m_mapKeyIdToKeyInfo.value(structDataInfo.sKeyId);

        QString sDomainName = structDataInfo.sDomain;
        QString sPropertyName = objKeyInfo.sKeyName;
        QString sKeyValue = structDataInfo.sKeyValue;
        QString sKeyDefaultValue = objKeyInfo.sDefaultValue;

        qDebug()<<"Default value : "<<structDataInfo.sKeyId<<"  "<<sKeyDefaultValue;
        /* Convert (Text Value) to attr.type(Specific type)*/
        if(objKeyInfo.keyType == INTIGER_TYPE)
        {
            convertTextToInt(sKeyValue);
        }
        if(objKeyInfo.keyType == DOUBLE_TYPE)
        {
            convertTextToDouble(sKeyValue);
        }
        if(objKeyInfo.keyType == BOOLEAN_TYPE)
        {
            convertTextToBool(sKeyValue);
        }
        if(objKeyInfo.keyType == QSTRING_TYPE)
        {
            m_sCurrentKeyValue = sKeyValue;
        }
        /****************************************************/

        if(sDomainName.compare(GRAPH,Qt::CaseInsensitive) == 0)
        {
            if(sPropertyName.compare(NODE_COORDINATE_X,Qt::CaseInsensitive) == 0)
            {
                // Store coord_x into GraphProperties
                structGraphProperty.iCoordX = m_iCurrentKeyValue;

                // set the value for the graph's x coordinate as preset value
                m_bIsGraphCoordXSet = true;
            }
            else if(sPropertyName.compare(NODE_COORDINATE_Y,Qt::CaseInsensitive) == 0)
            {
                // Store coord_y into GraphProperties
                structGraphProperty.iCoordY = m_iCurrentKeyValue;

                // set the value for the graph's y coordinate as preset value
                m_bIsGraphCoordYSet = true;
            }
            else if(sPropertyName.compare(NODE_HEIGHT,Qt::CaseInsensitive) == 0)
            {
                // Store height into GraphProperties
//                //cout<<"GraphProp Height : "<<structDataInfo.sDomainID.toStdString()<<" "<<m_iCurrentKeyValue<<endl;
                structGraphProperty.iHeight = m_iCurrentKeyValue;

                // set the value for the graph's height as preset value
                m_bIsGraphHeightSet = true;
            }
            else if(sPropertyName.compare(NODE_WIDTH,Qt::CaseInsensitive) == 0)
            {
                // Store width into GraphProperties
//                //cout<<"GraphProp Width : "<<m_iCurrentKeyValue<<endl;
                structGraphProperty.iWidth = m_iCurrentKeyValue;

                // set the value for the graph's width as preset value
                m_bIsGraphWidthSet  = true;
            }
            else
            {
                throw LayoutException(__FUNCTION__,
                                      LayoutExceptionEnum::MANDATORY_FIELD_MISSING,
                                      "graph attributes",GRAPH);
            }
        }
        else if(sDomainName.compare(NODE,Qt::CaseInsensitive) == 0)
        {
            if(sPropertyName.compare(NODE_COORDINATE_X,Qt::CaseInsensitive) == 0)
            {
                // Store coord_x(int) into VertexProperty
                structEntityProperty.iCoordX = m_iCurrentKeyValue;

                // set the value for the node's x coordinate as preset value
                m_bIsNodeCoordXSet = true;
            }
            else if(sPropertyName.compare(NODE_COORDINATE_Y,Qt::CaseInsensitive) == 0)
            {
                // Store coord_y into VertexProperty
                structEntityProperty.iCoordY = m_iCurrentKeyValue;

                // set the value for the node's Y coordinate as preset value
                m_bIsNodeCoordYSet = true;
            }
            else if(sPropertyName.compare(NODE_HEIGHT,Qt::CaseInsensitive) == 0)
            {
                // Store height into VertexProperty
                structEntityProperty.iHeight = m_iCurrentKeyValue;

                // set the value for the height of node as preset value
                m_bIsNodeHeightSet = true;
            }
            else if(sPropertyName.compare(NODE_WIDTH,Qt::CaseInsensitive) == 0)
            {
                // Store width into VertexProperty
                structEntityProperty.iWidth = m_iCurrentKeyValue;

                // set the value for the width of node as preset value
                m_bIsNodeWidthSet = true;
            }
            else
            {
                throw LayoutException(__FUNCTION__,
                                      LayoutExceptionEnum::MANDATORY_FIELD_MISSING,
                                      "node attribues",NODE);
            }
        }
        else if(sDomainName.compare(EDGE,Qt::CaseInsensitive) == 0)
        {
            if(sPropertyName.compare(EDGE_BIDIRECTIONAL,Qt::CaseInsensitive) == 0)
            {
                // Store bidirectional(bool) into EdgeProperty
//                //cout<<"Edge Bidirectional : "<<m_bCurrentKeyValue<<" "<<sPropertyName.toStdString()<<" "<<m_iCurrentKeyValue<<endl;
                structEntityProperty.bIsBidirectional = m_bCurrentKeyValue;
            }
            else if(sPropertyName.compare(EDGE_BENDPOINTS,Qt::CaseInsensitive) == 0)
            {
//                //cout<<"Bend Points Key value : "
//                   <<m_iCurrentKeyValue<<" "
//                  << m_sCurrentDomain.toStdString()<<" "
//                     <<m_sCurrentKeyId.toStdString()<<" "<<endl;
            }
        }
        // unset the data tag value for the edge(used for the bend points)
        m_bEdgeDataTag =false;
    }

    return true;
}

QString GraphMLReader::errorString() const
{
    // set the error if present.
    return m_sErrorText;
}

void GraphMLReader::addVertexToSubgraph( QString sVertexId )
{
    LAYOUT_ASSERT((sVertexId != EMPTY_STRING_VALUE) || (mapVertexIdToVertexDescriptorObj.contains(sVertexId) != true),
                  LayoutException(__FUNCTION__,
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  EMPTY_ID_FOUND, VERTEX));
    // create new vertex descriptor.
    VertexDescriptor *vdNode=new VertexDescriptor();
    //Using boost graph wrapper function for vertx ordering and own vertex list
    BoostGraphWrapper graphWrapper;
    try
    {
        // add this vertex to boost current graph.
        *vdNode = graphWrapper.addVertex(*gCurrentSubGraph);
    }
    catch(boost::exception& eBoostException)
    {
        DELETE_AND_SET_NULL(vdNode);
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }

    try
    {
        // get the global vertex descriptor of this vertex and map it with its id.
        VertexDescriptor vdGlobalNode = gCurrentSubGraph->local_to_global(*vdNode);
        mapVertexIdToVertexDescriptorObj.insert(sVertexId,vdGlobalNode);
    }
    catch(LayoutMemoryException& eException)
    {
        DELETE_AND_SET_NULL(vdNode);
        QString sErrorMessage = eException.getErrorMessage();
        throw LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, LOCAL_TO_GLOBAL + sErrorMessage);
    }

    //Set node id into graph
    (*gCurrentSubGraph)[*vdNode].sId = sVertexId;

    // Set the node is not dummy node
    bool bIsDummyNode = false;
    (*gCurrentSubGraph)[*vdNode].bIsExpandable = bIsDummyNode;

    // keep map of the vertex descriptor to dummy node or not.
    mapVertexDescriptorToIsDummyNode.insert(*vdNode,bIsDummyNode);

    /**
    Check the data values for this nodes are set for all mandatory fields, for
    coordinate X, coorcinate Y, Height and Width.
    */
    if(m_bIsNodeCoordXSet && m_bIsNodeCoordYSet && m_bIsNodeHeightSet && m_bIsNodeWidthSet)
    {
        /*Adding Data Values to node*/
        int iLeftCoordX = structEntityProperty.iCoordX;
        int iLeftCoordY = structEntityProperty.iCoordY;
        int iHeight = structEntityProperty.iHeight;
        int iWidth = structEntityProperty.iWidth;

        // set the values to the vertex property of a graph.
        (*gCurrentSubGraph)[*vdNode].iLeftCoordX = iLeftCoordX;
        (*gCurrentSubGraph)[*vdNode].iLeftCoordY = iLeftCoordY;
        (*gCurrentSubGraph)[*vdNode].iHeight = iHeight;
        (*gCurrentSubGraph)[*vdNode].iWidth = iWidth;

        // calculate center coordinates of this node
        int iHalfWidthFactor = (iWidth / 2);
        int iHalfHeightFactor = (iHeight / 2);
        int iCenterCoordX = iLeftCoordX + iHalfWidthFactor;
        int iCenterCoordY = iLeftCoordY + iHalfHeightFactor;

        // set the center coordinates of this node.
        graphWrapper.setVertexCenterCoordX(*vdNode, *gCurrentSubGraph, iCenterCoordX);
        graphWrapper.setVertexCenterCoordY(*vdNode, *gCurrentSubGraph, iCenterCoordY);
    }
    else
    {
        /***
         * This case indiacets that one of the mandatory field for the node is absent in
         * the input geraphml file.
         */
        throw LayoutException(__FUNCTION__, LayoutExceptionEnum::MANDATORY_FIELD_MISSING, sVertexId, NODE);
    }
}


void GraphMLReader::addEdgeToMainGraph(QString sSourceVertexId,QString sTargetVertexId,QString sEdgeId)
{
    LAYOUT_ASSERT((sSourceVertexId != EMPTY_STRING_VALUE) || (sTargetVertexId != EMPTY_STRING_VALUE),
            LayoutException(__FUNCTION__,
                            LayoutExceptionEnum::INVALID_PARAMETER,
                            EMPTY_ID_FOUND, EDGE));

    // create new edge for this edge id.
    EdgeDescriptor *edEdge = new EdgeDescriptor();
    bool bIsValidSourceId = mapVertexIdToVertexDescriptorObj.contains(sSourceVertexId);
    bool bIsValidTargetId = mapVertexIdToVertexDescriptorObj.contains(sTargetVertexId);

    // check forward reference for the node
    if((bIsValidSourceId == true) && (bIsValidTargetId == true))
    {
        // forward reference problem not present in file
        VertexDescriptor vdSource = mapVertexIdToVertexDescriptorObj.value(sSourceVertexId);
        VertexDescriptor vdTarget = mapVertexIdToVertexDescriptorObj.value(sTargetVertexId);

        if((vdSource >= 0) || (vdTarget >= 0))
        {
            try
            {
                // add edge to this boost graph.
                *edEdge  = (add_edge(vdSource,vdTarget,(*gMainGraph))).first;
            }
            catch(LayoutMemoryException& eException)
            {
                DELETE_AND_SET_NULL(edEdge);
                throw LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, eException.getObjectName());
            }
            catch(boost::exception& eBoostException)
            {
                DELETE_AND_SET_NULL(edEdge);
                throw *boost::get_error_info<errmsg_info>(eBoostException);
            }
        }
        else
        {
            DELETE_AND_SET_NULL(edEdge);
            throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_PARAMETER, INVALID_EDGE_PARAMETER, EDGE);
        }

        // set the edge id into graph
        (*gMainGraph)[*edEdge].sId = sEdgeId;

        /*Adding Data Values to Edge*/
        (*gMainGraph)[*edEdge].bBidirectional = structEntityProperty.bIsBidirectional;

        /* Adding bend properties into the edge structure */
        (*gMainGraph)[*edEdge].vecBendPoints = m_dirtyEdge.m_vecBendPoints;

        //Clear bend list for dirty edge just added to graph
         m_dirtyEdge.m_vecBendPoints.clear();


    }
    else
    {
        // forward reference problem present in file
        cout<<"Forward refrence problem while adding edge "
           <<sSourceVertexId.toStdString()<<" "<<sTargetVertexId.toStdString()<<endl;
        DELETE_AND_SET_NULL(edEdge);
        throw LayoutException(__FUNCTION__,
                              LayoutExceptionEnum::INVALID_PARAMETER,
                              FORWARD_REFERENCE,EDGE,
                              sSourceVertexId,sTargetVertexId);
    }
}

GraphMLReader::~GraphMLReader()
{
    DELETE_AND_SET_NULL(gCurrentSubGraph);
}
