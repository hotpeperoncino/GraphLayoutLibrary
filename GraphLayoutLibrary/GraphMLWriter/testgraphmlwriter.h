#ifndef TESTGRAPHMLWRITER_H
#define TESTGRAPHMLWRITER_H

#include <QFile>
#include <iostream>
#include <QTime>

#include <boost/graph/graphml.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <QTextStream>


using namespace std;
using namespace boost;

/**
 * @brief VertexProperties
 *
 * Structure for vertex properties.
 */
struct VertexProperties
{
    string sId;
    string sHeight;
    string sWidth;
    string sCoord_X;
    string sCoord_Y;
};
/**
 * @brief EdgeProperties
 *
 * Structure for edge properties.
 */
struct EdgeProperties
{
    string sId;
    bool bBidirectional;
};
/**
 * @brief Graph
 *
 * Type definition for boost bidirectional graph type.
 */
typedef boost::subgraph<
	boost::adjacency_list<
		boost::vecS,
		boost::vecS,
		boost::bidirectionalS,
		boost::property<
			boost::vertex_index_t,
			int,
			VertexProperties
		>,
		boost::property<
			boost::edge_index_t,
			int,
			EdgeProperties
		>
	>
> Graph;
/**
 * @brief edgeDescriptor
 *
 * Type definition for boost bidirectional graph edge type.
 */
typedef graph_traits<Graph>::edge_descriptor edgeDescriptor;
/**
 * @brief vertexDescriptor
 *
 * Type definition for boost bidirectional graph vertex type.
 */
typedef graph_traits<Graph>::vertex_descriptor vertexDescriptor;

/**
 * @brief The TestGraphmlWriter class writes dummy boost graph into temporary file in graphml file format
 */
class TestGraphmlWriter
{
public:
	QString m_sFileName; /*!< QString to store name of temporary graphML file. */
    Graph m_gMainGraph; /*!< Graph used to test writter. */
    int m_iTotalNodes; /*!< Integer to store vertex count */
    int m_iTotalEdges; /*!< Integer to store edge count */
	
	/** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type TestGraphmlWriter.
     */

    //@{
    TestGraphmlWriter();
	//@}

    /**
     This function creates a map of the node, edge, graph and their properties.
     This map is later used to generate key declarations for these node, edge or graph.

    @pre

    @param

    @return none

    @throw none
    */
	void test(int iExtraVertices);
    /**
     This function creates a map of the node, edge, graph and their properties.
     This map is later used to generate key declarations for these node, edge or graph.

    @pre

    @param

    @return none

    @throw none
    */
	void generateMainGraph1();
    /**
     This function creates a map of the node, edge, graph and their properties.
     This map is later used to generate key declarations for these node, edge or graph.

    @pre

    @param

    @return none

    @throw none
    */
	void generateMainGraph2(int iGenerateExtraVertice);
};

#endif // TESTGRAPHMLWRITER_H
