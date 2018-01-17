#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include <QIODevice>
#include <LayoutUtilities/LayoutTypes.h>
#include <Common/BoostGraphWrapper.h>
#include <GraphMLReader/GraphMLReader.h>
#include <GraphMLWriter/GraphMLWriter.h>
#include <GraphLayoutLibrary_global.h>
#include <HierarchicalLayoutGenerator/HierarchicalLayouter.h>
#include <CircularLayout/CircularLayoutGenerator.h>
#include <GridBasedLayout/GridBasedLayout.h>
#include <GridBasedLayout/GraphPreProcessor.h>
#include <GridBasedLayout/GridLayouter.h>
#include <RandomLayout/RandomLayoutGenerator.h>
#include <Common/LayoutEnum.h>
#include <QString>
#include <LayoutManager/GraphLayoutErrorCodes.h>
#include <ForceBasedLayout/SpringEmbedder.h>
#include <ForceBasedLayout/Reingold.h>
#include <ForceBasedLayout/ClusteredSpringEmbedder.h>
#include <ForceBasedLayout/MembershipInitializer.h>
#include <ForceBasedLayout/ForceBasedLayoutGenerator.h>
#include <Common/LayoutUtility.h>


#define SEPARATION 20

//using namespace boost;
/**
 * The LayoutManager class contains the main APIs of GraphLayoutLibrary. This is the starting poing for accessing the library.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayoutManager
{
    typedef std::vector< graph_traits< LayoutUtility::graph >::vertex_descriptor > CanonicalOrderStorage;
public:
    /**
     * Constructs an object of LayoutManager
     *
     */
    LayoutManager();




private:

    int iForceDirectedIterations;   // iteration count received from client

    //CanonicalOrderStorage m_oCanonicalOrder;
    std::vector<int> m_oCanonicalOrder;
    std::vector< GridBasedLayout::Position > m_oVertexPossition;
    GraphPreProcessor::EdgeOrderType m_oDummyEdges, m_oNonPlanarEdges, m_oParallelEdges;
    GraphPreProcessor::VertexOrderType m_oDummyVertices;
    //GraphPreProcessor::CorrespondingEdgesType m_oCrossingEdges;
    std::vector<GraphPreProcessor::EdgePairType> m_oCrossingEdges;

    enum enumVertexType {ORIGINAL_VERTEX, NEW_VERTEX};
    struct Node_Type_Pair
    {
        VertexDescriptor vNode;
        enumVertexType vertexType;
    };

    typedef std::vector<Node_Type_Pair> edgeType;


    //To Do: Function to return list of errors

    /*API for Layout Library
     */

    SubGraph& applyLayout(SubGraph& gMainGraph, const LayoutType& enLayoutType);

    bool applyLayout(LayoutType enLayoutType , QString strInputGraphmlPath , QString strOutputGraphMLPath);

public:
/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of GraphCycleHandler.
  */
 //@{
    /**
      this method reads graphML file, apply circular layout on the input file data
      and writes the updated layout changes to the output file in the form of graphML file

     * @brief applyCircularLayoutProcess
     *
     * @pre sInputFileName.trimmed().isEmpty() != true
     *
     * @param sInputFileName
     *
     * @param sOutputFileName
     *
     * @param enVertexOrder
     *
     * @return GraphLayoutErrorCodes
     *
     * @throw none
     */
    GraphLayoutErrorCodes::LayoutErrorCode applyCircularLayout(QString sInputGraphMLFilePath, QString sOutputGraphMLFilePath, LayoutEnum::VertexOrderCriteria enVertexOrder = LayoutEnum::TopologicalOrder);

    /**
      this method reads graphML file, applies Hierarchical Layout on the input file data
      and writes the updated layout changes to the output file in the form of graphML file

     * @brief applyHierarchicalLayoutProcess
     *
     * @pre sInputFileName.trimmed().isEmpty() != true
     *
     * @param sInputFileName
     * input graphml file path
     *
     * @param sOutputFileName
     * output graphml file path
     *
     * @return GraphLayoutErrorCodes
     *
     * @throw none
     */
    GraphLayoutErrorCodes::LayoutErrorCode applyHierarchicalLayout(QString sInputGraphMLFilePath, QString sOutputGraphMLFilePath);

    /**
      This method reads graph from input file, applies grid based layout on the input graph data
      and writes the updated layout changes to the output file in the form of graphML file

     * @brief applyGridBasedLayoutProcess
     *          applies Grid Based Layout algorithm to supplied graph
     *          and writes the output to graphml file.
     *
     * @pre gMainInputGraph not empty, sOutputGraphMLFilePath is valid.
     *
     * @param sInputGraphMLFilePath
     *          input file which contains graph on which Grid Based Layout algorithm is to be applied.
     *
     * @param sOutputGraphMLFilePath
     *          output graphml file path where the output is to be written.
     *
     * @return GraphLayoutErrorCodes
     *
     * @throw LayoutException
     */
    GraphLayoutErrorCodes::LayoutErrorCode applyGridBasedLayout(QString sInputGraphMLFilePath, QString sOutputGraphMLFilePath);

    /**
      This method reads main input graph, applies grid based layout on the input graph data

    GraphLayoutErrorCodes::LayoutErrorCode applyForceDirectedLayout(SubGraph *gMaingraph,
                                                                             int iIterations);
     *          and writes the output to graphml file.
     *
     * @pre gMainInputGraph not empty, sOutputGraphMLFilePath is valid.
     *
     * @param gMainInputGraph
     *          input graph on which Grid Based Layout algorithm is to be applied.
     *
     * @return none
     *
     * @throw LayoutException
     */
    void applyGridBasedLayout(SubGraph *gMainInputGraph);

	/**
      this method reads Graph object, applies force directed layout.

     * @brief applyForceDirectedLayout
     *
     * @param gMainInputGraph
     * 		input graph.
	 *
	 * @param sInputGraphMLFilePath
     * 		input graph file path
	 *
	 * @param iIterations
     * 		number of iterations
     *
     * @return GraphLayoutErrorCodes::LayoutErrorCode
     *
     * @throw none
     */
    GraphLayoutErrorCodes::LayoutErrorCode applyForceDirectedLayout(SubGraph &gMainInputGraph,
                                                                QString sInputGraphMLFilePath,
                                                                int iIterations);
	
	/**
      this method reads Graph object, applies force directed layout.

     * @brief applyForceDirectedLayout
     *
     * @param gMainInputGraph
     * 		input graph.
	 *
	 * @param iIterations
     * 		number of iterations
     *
     * @return GraphLayoutErrorCodes::LayoutErrorCode
     *
     * @throw none
     */
    GraphLayoutErrorCodes::LayoutErrorCode applyForceDirectedLayout(SubGraph *gMainInputGraph
                                                                    ,int iIterations);
	
	/**
      this method reads Graph object, applies force directed layout.

     * @brief applyForceDirectedLayout
     *
     * @param sInputGraphMLFilePath
     * 		input graph file path
	 *
	 * @param sOutputGraphMLFilePath
     * 		output graph file path
	 *
	 * @param iIterations
     * 		number of iterations
     *
     * @return GraphLayoutErrorCodes::LayoutErrorCode
     *
     * @throw none
     */
    GraphLayoutErrorCodes::LayoutErrorCode applyForceDirectedLayout(QString sInputGraphMLFilePath,
                                                                    QString sOutputGraphMLFilePath,
                                                                    int iIterations);

    /**
      this method reads Graph Object, applies Force Directed on it.

     * @brief applyHierarchicalLayoutProcess
     *
     * @pre sInputFileName.trimmed().isEmpty() != true
     *
     * @pre gMainGraph != NULL
     *
     * @param gMainGraph
     * input graph object
     *
     * @param sInsputgraphPath
     * input graphML file path.
     *
     *
     * @param iIteration
     * number of iterations
     *
     *
     *
     * @return none
     *
     * @throw none
     */
    void setNumberOfIterations(int iterations);

	/**
      this method reads Graph from input file, removes vertex overlaps, writes the resulting graph to output file.

     * @brief applyVertexOverlapRemoval
     *
     * @pre sInputGraphMLFilePath.trimmed().isEmpty() != true
     *
     * @pre sOutputGraphMLFilePath.trimmed().isEmpty() != true
     *
     * @param sInsputgraphPath
     * 		input graphML file path.
	 *
	 * @param sOutputGraphMLFilePath
     * 		output graph file path
     *
     * @return GraphLayoutErrorCodes::LayoutErrorCode
     *
     * @throw none
     */
    GraphLayoutErrorCodes::LayoutErrorCode applyVertexOverlapRemoval(QString sInputGraphMLFilePath, QString sOutputGraphMLFilePath);
//@}



};

#endif // LAYOUTMANAGER_H
