#ifndef GRAPHPREPROCESSOR_H
#define GRAPHPREPROCESSOR_H

#include <GraphLayoutLibrary_global.h>

#include <iostream>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/lookup_edge.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <boost/graph/make_connected.hpp>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>

#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <Common/BoostGraphWrapper.h>
#include <Common/LayoutUtility.h>
#include <GridBasedLayout/CreateDualGraph.hpp>

using namespace boost;
/**
 * @brief The GraphPreProcessor class
 *
 * A class to handle all preprocessing and postprocessing operations on the graph for grid based layouting.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT GraphPreProcessor
{
public:
    /**
     * @brief graph
     *
     * adjecency_list graph type used by the layout algorithm for maintaining local copy of input graph.
     */
    typedef adjacency_list
        < listS,
          vecS,
          undirectedS,
          property<vertex_index_t, int>,
          property<edge_index_t, int>
        >
        graph;
    /**
     * @brief UndirEdgeOrderType
     *
     * vector of edges in graph.
     */
    typedef std::vector<graph_traits< graph >::edge_descriptor > UndirEdgeOrderType;
	/**
     * @brief UndirVertexOrderType
     *
     * vector of vertices in graph.
     */
    typedef std::vector<graph_traits< graph >::vertex_descriptor > UndirVertexOrderType;
    /**
     * @brief dijkstra_graph
     *
     * adjecency_list undirected and weighted graph type used by the Dijkstra's algorithm for converting non-planar graph to planar.
     */
    typedef adjacency_list
        < listS,
          vecS,
          undirectedS,//derectedS
          property<vertex_index_t, int>,//no_property,
          property <  edge_weight_t, int, property<edge_index_t, int> >
        >
        dijkstra_graph;
    /**
     * @brief dijkstra_vertex_descriptor
     *
     * adjecency_list graph type used by the layout algorithm for maintaining local copy of input graph.
     */
    typedef graph_traits < dijkstra_graph >::vertex_descriptor dijkstra_vertex_descriptor;
    /**
     * @brief EdgeOrderType
     *
     * vector of edges in dijkstra_graph.
     */
    typedef std::vector< EdgeDescriptor > EdgeOrderType;
    /**
     * @brief VertexOrderType
     *
     * vector of vertices in dijkstra_graph.
     */
    typedef std::vector< VertexDescriptor > VertexOrderType;
    /**
     * @brief EdgePairType
     *
     * vector of VertexOrderType.
     */
    typedef std::vector<VertexOrderType> EdgePairType;
    /**
     * @brief CorrespondingEdgesType
     *
     * vector of EdgeOrderType.
     */
    typedef std::vector<EdgeOrderType> CorrespondingEdgesType;


public:
	/** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type GraphPreProcessor.
     */

    //@{
	/**
      Constructs object of type GraphPreProcessor.
     @pre
        none

     @throw
        none
    */
    GraphPreProcessor();
	//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of GraphPreProcessor.
  */
 //@{
    /**
      This method reads a planar graph, converts it to maximal planar graph by adding some extra edges, if needed,
      and writes the edges added in the supplied parameter.

     * @brief toMaximalPlanarProcess
     *          converts planar graph to maximal planar.
     *
     * @pre gInputGraph is planar
     *
     * @param gInputGraph
     *          graph which is to be made maximal planar.
     *
     * @param oDummyEdges
     *          vector of edges that will store dummy edges that will be added in gInputGraph.
     *
     * @return none
     *
     * @throw LayoutException
     */
     void toMaximalPlanar(SubGraph &gInputGraph, EdgeOrderType &oDummyEdges);

     /**
       This method reads a maximal planar graph (by toMaximalPlanar()),
       and converts it to planar graph by removing the extra edges. The extra dummy edges
       need to be supplied in the parameter.

      * @brief removeDummyEdgesProcess
      *         converts maximal planar graph obtained from toMaximalPlanar() to its original graph.
      *
      * @pre edges in oDummyEdges must be present in gInputGraph
      *
      * @param gInputGraph
      *         graph from which dummy vertices need to be removed.
      *
      * @param oDummyEdges
      *          vector of edges that stores dummy edges that have to be removed from gInputGraph
      *
      * @return none
      *
      * @throw LayoutException
      */
      void removeDummyEdges(SubGraph &gInputGraph, EdgeOrderType &oDummyEdges);

      /**
        This method reads a non-planar graph, converts it to planar graph by adding some extra vertices
        and writes the vertices added in the supplied parameter. Dummy vertex replaces each edge crossing.
        The process replaces crossing pair of edges by a dummy vertex and four edges connecting that dummy vertex
        to endpoints of crossing edges.

       * @brief toPlanarProcess
       *        converts gInputGraph to a planar graph.
       *
       * @param gInputGraph
       *        graph from which dummy vertices need to be removed.
       *
       * @param oDummyVertices
       *          vector of vertices that will store dummy vertices that will be added in gInputGraph
       *
       * @param correspondingCrossingEdgeList
       *          vector of vector of vector (3D vector) of vertices to store crossing edges
       *          that a dummy vertex will replace
       *
       * @return none
       *
       * @throw LayoutException
       */
       void toPlanar(SubGraph & gInputGraph, VertexOrderType &oDummyVertexList,
                     std::vector<GraphPreProcessor::EdgePairType> & correspondingCrossingEdgeList);
					 
		/**
          this method reads a planar graph (which is output of layout algorithm),
          and converts it to non-planar graph by removing the extra vertices and edges
          in the supplied parameter

       * @brief removeDummyVerticesProcess
       *        replaces dummy vertices with corresponding crossing pair of edges.
       *
       * @param gInputGraph
       *
       * @param oDummyVertices
       *          vector of vertices that stores dummy vertices that will be removed from gInputGraph
       *
       * @param correspondingCrossingEdge
       *          vector of vector of vector (3D vector) of vertices that stores crossing edges
       *          that will replace a dummy vertex
       *
       * @return none
       *
       * @throw LayoutException
       */
       void removeDummyVertices(SubGraph & gInputGraph, VertexOrderType &oDummyVertices,
                                std::vector<GraphPreProcessor::EdgePairType> & correspondingCrossingEdge);

       /**
         this method reads a non-planar graph, removes the parallel edges from input graph
         and stores them in the supplied parameter

        * @brief removeParallelEdgesProcess
        *
        * @pre gInputGraph ! = NULL
        *
        * @param gInputGraph
        *
        * @param oParallelEdgeList
        *          vector of edges that will store parallel edges that are removed from gInputGraph
        *
        * @return none
        *
        * @throw LayoutException
        */
        void removeParallelEdges(SubGraph & gInputGraph, EdgeOrderType & oParallelEdgeList, std::vector<QString> & oParrallelEdgeIDs);

        /**
          this method reads a non-planar graph, adds the parallel edges stored in the supplied parameter
          to input graph

         * @brief addParallelEdgesProcess
         *
         * @pre gInputGraph ! = NULL
         *
         * @param gInputGraph
         *
         * @param oParallelEdgeList
         *          vector of edges that stores parallel edges that are added to gInputGraph
         *
         * @return none
         *
         * @throw LayoutException
         */
         void addParallelEdges(SubGraph & gInputGraph, EdgeOrderType & oParallelEdgeList, std::vector<QString> & oParrallelEdgeIDs);

       /**
         this method reads a non-planar graph, converts it to planar graph by removing some crossing edges
         and writes the edges removed in the supplied parameter

        * @brief toPlanarAlternateProcess
        *
        * @pre gInputGraph ! = NULL
        *
        * @param gInputGraph
        *
        * @param oNonPlanarEdges
        *          vector of edges that will store edges that caused crossings and will be removed from gInputGraph
        *
        * @return none
        *
        * @throw none
        */
        void toPlanarAlternate(SubGraph & gInputGraph, EdgeOrderType &oNonPlanarEdges);


        /**
          this method reads planar output graph and adds the edges that were making it non-planar.
          These edges are supplied in the parameter

         * @brief addNonPlanarEdgesProcess
         *
         * @pre gInputGraph ! = NULL
         *
         * @param gInputGraph
         *
         * @param oNonPlanarEdges
         *          vector of edges that will store edges that caused crossings and will be removed from gInputGraph
         *
         * @return none
         *
         * @throw none
         */
         void addNonPlanarEdges(SubGraph & gInputGraph, EdgeOrderType &oNonPlanarEdges);
//@}

//         void searchAndRemoveDummyVertices(SubGraph & gInputGraph);
private:
       /**
         This method finds path between source and target given list of parents of each node
         in Dijkstra's minimal spanning tree. The path is in reverse order.

        * @brief findPathProcess
        *          calculates path from list of parents of vertices.
        *
        * @pre vTarget exists in parentVertex
        *
        * @param vTarget
        *          target vertex to which the path is to be found out (from source provided to Dijkstra's algorithm).
        *
        * @param myPath
        *          vector of vertices that stores path in reverse (target to source).
        *
        * @param parentVertex
        *          vector of vertices that contains parent of each vertex in Dijkstra's paths.
        *
        * @return none
        *
        * @throw LayoutException
        */
        void findPath(GraphPreProcessor::dijkstra_vertex_descriptor &vTarget,
                      std::vector<GraphPreProcessor::dijkstra_vertex_descriptor> &myPath,
                      std::vector<GraphPreProcessor::dijkstra_vertex_descriptor> &parentVertex);

        /**
          This method finds the edges which cross with other edges when present in the InputGraph.
          It generates a copy of graph with such edges removed and a list of these edges.

         * @brief findCrossingEdgesProcess
         *          separates out edges that introduce crossings in the graph.
         *
         * @param gInputGraph
         *          graph from which crossing edges need to be found out.
         *
         * @param gLocalGraph
         *          copy of graph which will be generated after removing crossing edges.
         *
         * @param localCrossingEdgeList
         *          vector of crossing edges.
         *
         * @return none
         *
         * @throw LayoutException
         */
        void findCrossingEdges(SubGraph & gInputGraph, GraphPreProcessor::graph & gLocalGraph,
                               std::vector< graph_traits< graph >::edge_descriptor > & localCrossingEdgeList);

        /**
          This method replaces each pair of crossing edge by a dummy vertex and four connecting edges.
          It stores the pair of replaced edges corresponding to each dummy vertex in the supplie parameter.

         * @brief replaceCrossingsByDummyVerticesProcess
         *          replaces each crossing by a dummy vertex.
         *
         * @param gInputGraph
         *          graph from which crossing edges need to be removed.
         *
         * @param gLocalGraph
         *          copy of graph which was generated after removing crossing edges.
         *
         * @param localCrossingEdgeList
         *          vector of crossing edges.
         *
         * @param oDummyVertexList
         *          vector of dummy vertices which will be added to the graph.
         *
         * @param correspondingCrossingEdgeList
         *          vector of vector of vector (3D vector) of vertices to store crossing edges
         *          that a dummy vertex will replace
         *
         * @return none
         *
         * @throw LayoutException
         */
        void replaceCrossingsByDummyVertices(SubGraph & gInputGraph, GraphPreProcessor::graph & gLocalGraph,
                                             std::vector< graph_traits< graph >::edge_descriptor > & localCrossingEdgeList,
                                             VertexOrderType &oDummyVertexList,
                                             std::vector<GraphPreProcessor::EdgePairType> & correspondingCrossingEdgeList);

        /**
          This method replaces each pair of crossing edge by a dummy vertex and four connecting edges.
          It stores the pair of replaced edges corresponding to each dummy vertex in the supplie parameter.

         * @brief createAugmentedDualGraphProcess
         *          constructs dual graph of a given graph augmented with source and terget vertices of a crossing edge.
         *
         * @param gLocalGraph
         *          copy of graph whose augmented dual graph need to be generated.
         *
         * @param gDualGraph
         *          augmented dual graph which will be generated.
         *
         * @param dualEdge
         *          vector of edges from LocalGraph which are dual of edges in DualGraph.
         *
         * @param oIndicesOfEndsOfCrossingEdge
         *          vector of vertex indices which need to be augmented to the DualGraph.
         *
         * @param oAugmentedVertices
         *          vector of vertices which will get augmented to the DualGraph.
         *
         * @return none
         *
         * @throw LayoutException
         */
        void createAugmentedDualGraph(GraphPreProcessor::graph & gLocalGraph,
                                      GraphPreProcessor::graph & gDualGraph,
                                      UndirEdgeOrderType & dualEdge,
                                      std::vector< int > & oIndicesOfEndsOfCrossingEdge,
                                      std::vector< graph_traits<GraphPreProcessor::graph>::vertex_descriptor > & oAugmentedVertices);

        /**
          This method replaces each pair of crossing edge by a dummy vertex and four connecting edges.
          It stores the pair of replaced edges corresponding to each dummy vertex in the supplie parameter.

         * @brief createAugmentedDualGraphProcess
         *          constructs dual graph of a given graph augmented with source and terget vertices of a crossing edge.
         *
         * @param gLocalGraph
         *          copy of graph whose augmented dual graph need to be generated.
         *
         * @param gDualGraph
         *          augmented dual graph which will be generated.
         *
         * @param dualEdge
         *          vector of edges from LocalGraph which are dual of edges in DualGraph.
         *
         * @param oIndicesOfEndsOfCrossingEdge
         *          vector of vertex indices which need to be augmented to the DualGraph.
         *
         * @param oAugmentedVertices
         *          vector of vertices which will get augmented to the DualGraph.
         *
         * @return none
         *
         * @throw LayoutException
         */
        void findShortestPath(GraphPreProcessor::graph & gDualGraph,
                              GraphPreProcessor::dijkstra_graph & gDijkstraGraph,
                              graph_traits<GraphPreProcessor::graph>::vertex_descriptor & vAugmentedSource,
                              graph_traits<GraphPreProcessor::graph>::vertex_descriptor & vAugmentedTarget,
                              std::vector<GraphPreProcessor::dijkstra_vertex_descriptor> & path);


};

#endif // GRAPHPREPROCESSOR_H
