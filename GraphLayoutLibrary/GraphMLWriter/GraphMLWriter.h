#ifndef GRAPHMLWRITER_H
#define GRAPHMLWRITER_H

#include <QTextStream>
#include <QList>
#include <exception>

#include <boost/graph/graphml.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <string>

#include <Common/BoostGraphWrapper.h>

#include <ReaderWriterUtilities/AttributeConstants.h>
#include <ReaderWriterUtilities/DefaultValues.h>
#include <ReaderWriterUtilities/Typedefs.h>

#include <GraphLayoutLibrary_global.h>

#include <QQueue>

using namespace std;
using namespace boost;

/**
 * @brief The GraphMLWriter class writes provided boost graph into given file in graphml file format
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT GraphMLWriter
{
private:

    BoostGraphWrapper* m_boostGraphWrapper; /*!< BoostGraphWrapper pointer used to access graph properties */
    long int m_iGraphCounter; /*!< Integer to store encountered graph count */
    QTextStream m_txtStreamOut; /*!< QTextStream object used to write graphML file */
    //*************************************************************
    // To Do: Make a separate class for this property mapper
    MapGraphElementToAttributeList m_mapGraphElementToAttributeList; /*!< Map between elements to their attribute lists */
    //For attributes e.g. "node" "height" - "key0";or "edge" "direction" - "key1"
    MapElementNameAndPropertyToKey m_mapElementNameAndPropertyToKey; /*!< Map between element name : property to their keys */
    MapVisitedVertices m_mapVisitedVertices; /*!< Map between vertices to their keys. Used to maintain visited vertices. */
    MapVisitedEdges m_mapVisitedEdges; /*!< Map between edges to their keys. Used to maintain visited edges. */

    /**
     This function creates a map of the node, edge, graph and their properties.
     This map is later used to generate key declarations for these node, edge or graph.

    @pre

    @param

    @return none

    @throw none
    */
    void populateDataTagProperties();

    //*************************************************************

    /**
     This function marks vertex as visited as the indication that it has been written to the output file.

    @pre
    -# iVertex != NULL

    @param iVertex
    a non visited vertex of the graph

    @return none

    @throw none
    */
    void markGlobalVertexVisited(int iVertexId);

    /**
     This function checks if the vertex is visited or not,
     as the indication that it has been written to the output file or not.

    @pre
    -# iVertex != NULL

    @param iVertex
    a vertex of the graph

    @return none

    @throw none
    */
    bool isGlobalVertexVisited(int iVertexId);

    /**
     This function marks Edge as visited as the indication that it has been written to the output file.

    @pre
    -# iEdgeId != NULL

    @param iEdgeId
    an Edge of the graph

    @return none

    @throw none
    */
    void markGlobalEdgeVisited(int iEdgeId);

    /**
     This function checks if the Edge is visited or not, as the indication that it has been written to the output file or not.

    @pre
    -# iEdgeId != NULL

    @param iEdgeId
    a Edge of the graph

    @return none

    @throw none
    */
    bool isGlobalEdgeVisited(int iEdgeId);

    /**
     This function generates key declaration for node, edge and graph attributes.
     These key ids are used to denote the attribute in the output graphml file.

    @pre
    -# m_txtStreamOut.device()->isWritable() == true
    -# strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
    -# m_mapGraphElementToAttributeList.empty() == false

    @param

    @return none

    @throw LayoutException
    -# REQUIRED_PARAMETER_NOT_SET if the output text stream is not set writable or does not have UTF 8 encoding
    -# EMPTY_CONTAINER if map GraphElementToAttributeList is empty

    */
    void generateKeyDeclaration();

    /**
     This function prints the attribute list of node, edge and graph
     which are considered for writing into the output graphml file.

    @pre
    -# m_mapGraphElementToAttributeList.empty() == false

    @param

    @return none

    @throw none
    */
    void printAttributeTable();

    /**
     This function prints data tag for the given attribute with given value
     with proper indent space aacording to the level or depth of that tag.

    @pre
    -# m_txtStreamOut.device()->isWritable() == true
    -# strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
    -# sKey.length() > 0
    -# iLevel >= 0

    @param sKey
        key id that describes the attribute of graph

    @param sValue
        value of the attrbute which is described by the key id

    @param iLevel
        specifies the nesting depth of current data tag

    @return none

    @throw LayoutException
    -# REQUIRED_PARAMETER_NOT_SET if the output text stream is not set writable or does not have UTF 8 encoding
    -# INVALID_PARAMETER if level value negative or Key is invalid

    */
    void addData(QString sKey , QString sValue, int iLevel);

    /**
     This function prints data tag for bends in the edges.
    @pre
    -# m_txtStreamOut.device()->isWritable() == true
    -# strcmp((const char*)m_txtStreamOut.codec(),"UTF-8")
    -# sKey.length() > 0
    -# iLevel >= 0

    @param sKey
        key id that describes the attribute of edge

    @param sValue
        value of the attrbute which is described by the key id

    @param iLevel
        specifies the nesting depth of current data tag

    @return none

    @throw none
    */
    void addBendPointsData(QString sKey, int iLevel,EdgeDescriptor& eEdge, SubGraph& gSubgraph);

    /**
     This function To retrieves unique key assigned to particular attribute of graph, node or edge

    @pre
    -# sElementName.length() > 0
    -# sAttributeName.length() > 0

    @param sElementName
        describes name of graph element i.e. node, edge or graph

    @param sAttributeName
        describes the attribute name

    @return QString
        key id in a QString

    @throw LayoutException
    -# EMPTY_CONTAINER if an empty key is found

    */
    QString getKey(QString sElementName , QString sAttributeName);

    /**
     This function returns indent space according to the given nesting level

    @pre
    -# iLevel >= 0

    @param iLevel
        specifies the nesting depth of the any tag

    @return none

    @throw none
    */
    QString indent(int iLevel);


public:
/** @name Creators
 *  The methods under this section are responsible for constructing or
 *  destructing an instance of type GraphMLWriter.
 */
//@{
    /**
     Constructs new object of GraphMLReader.
    @pre none

    @param none

    @return none

    @throw  none
    */
    GraphMLWriter();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of GraphMLWriter.
  */
 //@{
    /**
     This function writes boost graph to graphml file

    @pre
    -# gBoostGraph != NULL
    -# outStreamFile != NULL
    -# outStreamFile->isWritable() == true

    @param gBoostGraph
        reference to input graph
    @param outStreamFile
        output file stream

    @return none

    @throw LayoutException

    @throw BoostException

    @throw MemoryException
    */
    void writeGraphml(SubGraph& gBoostGraph, QIODevice *outStreamFile);
//@}

private:

    /**
     This function iterates the graphs recursively and writes them to graphml file stream

    @pre
    -# gBoostGraph != NULL
    -# outStreamFile != NULL
    -# outStreamFile->isWritable() == true

    @param gBoostGraph
        reference to current graph
    @param gMainGraph
        reference to main graph
    @param iLevel
        specifies the depth of current graph in graphs-subgraph tree

    @return none

    @throw none
    */
    void generateGraphmlRecur(SubGraph& gSubGraph , SubGraph &gMainGraph, int iLevel);

    /**
     This function generates graphml file from boost subgraph, internally uses generateGraphmlRecur()

    @pre
    -# gSubGraph != NULL
    -# m_txtStreamOut.device()->isWritable() == true
    -# strcmp((const char*)m_txtStreamOut.codec(),"UTF-8") == true

    @param gSubGraph
        reference to input graph

    @return none

    @throw LayoutException
    -# REQUIRED_PARAMETER_NOT_SET if the output text stream is not set writable or does not have UTF 8 encoding

    */
    void generateGraphml(SubGraph& gSubGraph);

    /**
     This function iterates the graphs recursively and writes them to graphml file stream

    @pre
    -# gBoostGraph != NULL
    -# outStreamFile != NULL
    -# outStreamFile->isWritable() == true

    @param gBoostGraph
        reference to current graph
    @param gMainGraph
        reference to main graph
    @param iLevel
        specifies the depth of current graph in graphs-subgraph tree

    @return none

    @throw LayoutException
    -# REQUIRED_PARAMETER_NOT_SET if the output text stream is not set writable or does not have UTF 8 encoding

    */
    void addGraphDataAttribute(SubGraph& gSubGraph , int iLevel);

    /**
     This function writes a node for a graph vertex to the output graphml file

    @pre
    -# gSubGraph != NULL
    -# gMainGraph != NULL
    -# iLevel >= 0
    -# vVertex >= 0

    @param vVertex
        reference to the vertex to be written in the output graphml file

    @param gSubGraph
        reference to current graph

    @param gMainGraph
        reference to main graph

    @param iLevel
        specifies the nesting depth of current graph in graphs-subgraph tree

    @return none

    @throw LayoutException
    -# REQUIRED_PARAMETER_NOT_SET if the output text stream is not set writable or does not have UTF 8 encoding

    */
    void addNode(VertexDescriptor &vVertex, SubGraph &gSubGraph , int iLevel, SubGraph &gMainGraph);

    /**
     This function writes a edge for a graph edge to the output graphml file

    @pre
    -# gSubGraph != NULL
    -# gMainGraph != NULL
    -# iLevel >= 0
    -# eEdge >= 0

    @param vEdge
        reference to the edge to be written in the output graphml file

    @param gSubGraph
        reference to current graph

    @param gMainGraph
        reference to main graph

    @param iLevel
        specifies the nesting depth of current graph in graphs-subgraph tree

    @return none

    @throw LayoutException
    -# REQUIRED_PARAMETER_NOT_SET if the output text stream is not set writable or does not have UTF 8 encoding

    */
    void addEdge(EdgeDescriptor &eEdge, SubGraph &gSubGraph , int iLevel, SubGraph &gMainGraph);

};

#endif // GRAPHMLWRITER_H


