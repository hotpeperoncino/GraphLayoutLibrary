#ifndef GRIDBASEDLAYOUT_H
#define GRIDBASEDLAYOUT_H

#include <GraphLayoutLibrary_global.h>

#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/lookup_edge.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include <Common/BoostGraphWrapper.h>
#include <Common/LayoutUtility.h>

using namespace boost;

#define ONE 1
#define TWO 2
#define SEPARATION 20

/**
 * @brief The GridBasedLayout class
 *
 * A class to apply grid based layout on a non-clustered graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT GridBasedLayout
{
public:
    /**
     * @brief The Graph structure
     *
     * Structure to store local graph which is used in finding canonical order.
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
     * @brief The Vertices vector
     *
     * Vector of vertices.
     */
	typedef std::vector<graph_traits< graph >::vertex_descriptor> VertexOrderType; /*!< Vector of vertices */
    /**
     * @brief The Position struct
     *
     * A stucture to store X and Y coordinates of a vertex.
     */
    struct Position
    {
        int iCoordX;
        int iCoordY;
    };

public:
/** @name Creators
 *
 *  The methods under this section are responsible for constructing
 *  an instance of type GridBasedLayout.
 */
//@{
	/**
     Constructs new object of type GridBasedLayout.
    @pre none

    @param none

    @return none

    @throw none
    */
    GridBasedLayout();
//@}

 /** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type GridBasedLayout.
 */
//@{
	/**
      this method reads graph, finds the canonical ordering of the vertices
      and writes the canonical order to the supplied parameter

     * @brief findCanonicalOrderProcess
     *
     * @pre gInputGraph ! = NULL
     *
     * @param gInputGraph
     *          maximal planar graph in which the canonical order of vertices is to be found.
     *
     * @param oVertexIndexCanonicalOrder
     *          vector of integers which will store indices of vertices in canonical order.
     *
     * @return none
     *
     * @throw none
     */
     void findCanonicalOrder(SubGraph &gInputGraph, std::vector<int> & oVertexIndexCanonicalOrder);

     /**
       this method reads graph and the canonical order of vertices
       and finds out the grid based layout of the graph

      * @brief findGridBasedLayoutProcess
      *
      * @pre gInputGraph ! = NULL && oCanonicalOrder != NULL
      *
      * @param gInputGraph
      *         maximal planar graph which is to be layed out.
      *
      * @param oCanonicalOrder
      *         vector of vertex_desciptors that contains canonical order of graph vertices.
      *
      * @param oVertexPosition
      *         vector of Position to store x and y coordinates of vertices.
      *
      * @return none
      *
      * @throw LayoutException
      */
      void findGridBasedLayout(SubGraph & gInputGraph, std::vector<int> & oCanonicalOrder,
                               std::vector< GridBasedLayout::Position > & oVertexPosition);
 //@}
    

private:
      /**
        this method reads a vertex and finds out vertices in the canonical order
        which are neighbours of the vertex in the input graph

       * @brief findNeighboursProcess
       *
       * @pre gInputGraph ! = NULL && oCanonicalOrder != NULL && vNewVertex != NULL
       *
       * @param gInputGraph
       *        maximal planar graph which is to be layed out.
       *
       * @param oContour
       *        vector of vertex_desciptors that contains contour of graph in previous stage
       *
       * @param vNewVertex
       *        vertex whose neighbours are to be found out
       *
       * @param p
       *        index in contour of neighbour of vNewVertex which comes first in contour
       *
       * @param q
       *        index in contour of neighbour of vNewVertex which comes last in contour
       *
       * @return none
       *
       * @throw none
       */
       void findNeighbours(SubGraph &gInputGraph, std::vector<VertexDescriptor> & oContour,
                           int & iNewVertexIndex, int& p, int& q);

       /**
         this method shifts the vertices to their right by one or two positions in current layout
         in order to accomodate the new vertex to be plotted

        * @brief shiftRightProcess
        *
        * @pre vecAssociated ! = NULL && vecPositon != NULL && (iShiftBy == 1 || iShiftBy == 2)
        *
        * @param iShiftBy
        *        by how much you want to shift the vertices, possible values are 1 or 2
        *
        * @param gInputGraph
        *        maximal planar graph which is to be layed out.
        *
        * @param vecPositon
        *        vector of Positions of all the vertices in input graph
        *
        * @param vecAssociated
        *        vector of (vectors of vertices) which are associated to a vertex
        *
        * @param oContour
        *        vector of vertex_desciptors that contains contour of graph
        *
        * @param p
        *        index in contour of neighbour of new vertex which comes first in contour
        *
        * @param q
        *        index in contour of neighbour of new vertex which comes last in contour
        *
        * @return none
        *
        * @throw LayoutException
        */
        void shiftRight(int iShiftBy, SubGraph &gInputGraph, std::vector< Position > & vecPositon,
                        std::vector< std::vector<VertexDescriptor> > & vecAssociated,
                        std::vector<VertexDescriptor> & oContour,
                        int& p, int& q);

        /**
          this method shifts the vertices to their right in current layout
          in order to remove the overlapping caused by the new vertex

         * @brief secondShiftRightProcess
         *
         * @pre vecAssociated ! = NULL && vecPositon != NULL && (iShiftBy == 1 || iShiftBy == 2)
         *
         * @param iOverlapLeft
         *        amount by which new vertex overlaps the vertex at its left
         *
         * @param iOverlapRight
         *        amount by which new vertex overlaps the vertex at its right
         *
         * @param gInputGraph
         *        maximal planar graph which is to be layed out.
         *
         * @param vecPositon
         *        vector of Positions of all the vertices in input graph
         *
         * @param vecAssociated
         *        vector of (vectors of vertices) which are associated to a vertex
         *
         * @param oContour
         *        vector of vertex_desciptors that contains contour of graph
         *
         * @param p
         *        index in contour of left neighbour of new vertex
         *
         * @return none
         *
         * @throw LayoutException
         */
         void secondShiftRight(int & iOverlapLeft, int & iOverlapRight, SubGraph &gInputGraph,
                               std::vector< Position > & vecPositon,
                               std::vector< std::vector<VertexDescriptor> > & vecAssociated,
                               std::vector<VertexDescriptor> & oContour,
                               int& p);


        /**
          this method finds out position of the new vertex on grid
          which is to be added to current layout

         * @brief findNewPositionProcess
         *
         * @pre vNewVertex ! = NULL && vecPositon != NULL
         *
         * @param vNewVertex
         *          vertex whose position is to be found out
         *
         * @param gInputGraph
         *        maximal planar graph which is to be layed out.
         *
         * @param p
         *        index in contour of neighbour of new vertex which comes first in contour
         *
         * @param q
         *        index in contour of neighbour of new vertex which comes last in contour
         *
         * @param vecPositon
         *        vector of Positions of vertices
         *
         * @param oContour
         *        vector of vertex_desciptors that contains contour of graph in previous stage
         *
         * @return none
         *
         * @throw none
         */
         void findNewPosition(int & vNewVertexIntex, int& p, int& q, SubGraph &gInputGraph,
                              std::vector< Position > & vecPositon,
                              std::vector<VertexDescriptor> & oContour);

         /**
           this method updates the contour of the graph after a new vertex is added to the layout

          * @brief updateContourProcess
          *
          * @pre oContour ! = NULL && vNewVertex != NULL
          *
          * @param oContour
          *        vector of vertex_desciptors that contains contour of graph in previous stage
          *
          * @param gInputGraph
          *        maximal planar graph which is to be layed out.
          *
          * @param vNewVertex
          *        vertex which is added to graph
          *
          * @param p
          *        index in contour of neighbour of new vertex which comes first in contour
          *
          * @param q
          *        index in contour of neighbour of new vertex which comes last in contour
          *
          * @return none
          *
          * @throw none
          */
          void updateContour(std::vector<VertexDescriptor> & oContour, SubGraph & gInputGraph,
                             int & iNewVertexIndex,
                             int& p, int& q);

          /**
            this method updates list of vertices associated with a vertex just added to the layout

           * @brief updateAssociaedVerticesProcess
           *
           * @pre vecAssociated ! = NULL && oContour != NULL && vNewVertex != NULL
           *
           * @param vecAssociated
           *
           * @param gInputGraph
           *        maximal planar graph which is to be layed out.
           *
           * @param oContour
           *        vector of vertex_desciptors that contains contour of graph in previous stage
           *
           * @param vNewVertex
           *          vertex which is added to graph
           *
           * @param p
           *        index in contour of neighbour of new vertex which comes first in contour
           *
           * @param q
           *        index in contour of neighbour of new vertex which comes last in contour
           *
           * @return none
           *
           * @throw none
           */
           void updateAssociaedVertices(std::vector<std::vector<VertexDescriptor> > &vecAssociated, SubGraph &gInputGraph,
                                        std::vector<VertexDescriptor> &oContour,
                                        int &iNewVertexIndex, int& p, int& q);

           /**
             this method reads the intermediate contour and returns the amount of overlap of
             newly added vertex with vertices to its both sides in current countour

            * @brief findOverlapsProcess
            *
            * @param p
            *        index in contour of neighbour of new vertex which comes first in contour
            *
            * @param oContour
            *        vector of vertex_desciptors that contains contour of graph in previous stage
            *
            * @param gInputGraph
            *        object of the original graph
            *
            * @param vecPositon
            *        vector of Positions of all the vertices in input graph
            *
            * @param iOverlapLeft
            *        amount of overlap of new node with the node at left in the contour
            *
            * @param iOverlapRight
            *        amount of overlap of new node with the node at right in the contour
            *
            * @return none
            *
            * @throw LayoutException
            */
            void findOverlaps(int & p, std::vector<VertexDescriptor> & oContour,
                              SubGraph & gInputGraph, std::vector<Position> &vecPositon,
                              int & iOverlapLeft, int & iOverlapRight);

            /**
              this method finds the vertex in current contour in the index range (p....q)
              which has highest top point and returns its object the the supplied parameter

             * @brief findTopmostVertexProcess
             *
             * @pre oContour ! = NULL
             *
             * @param p
             *        index in contour of neighbour of new vertex which comes first in contour
             *
             * @param q
             *        index in contour of neighbour of new vertex which comes last in contour
             *
             * @param oContour
             *        vector of vertex_desciptors that contains contour of graph in previous stage
             *
             * @param gInputGraph
             *        object of the original graph
             *
             * @param vecPositon
             *        vector of Positions of all the vertices in input graph
             *
             * @param iIndexOfTopmostVertex
             *        index of vertex (in input graph) which has highest top point
             *
             * @return none
             *
             * @throw none
             */
             void findTopmostVertex(int& p, int& q, std::vector<VertexDescriptor> & oContour,
                                    SubGraph & gInputGraph, std::vector<Position> &vecPositon,
                                    int &iIndexOfTopmostVertex);

             /**
               this method reads the vertex (under newly added vertex) which has highest top point,
               finds whether newly added vertex overlaps with it and shifts the new vertex upwards if needed

              * @brief shiftVerticallyProcess
              *
              * @pre oContour ! = NULL
              *
              * @param iIndexOfTopmostVertex
              *        index of vertex (in input graph) which has highest top point
              *
              * @param iNewVertexIndex
              *        index of vertex (in input graph) which is newly added to graph
              *
              * @param gInputGraph
              *        object of the original graph
              *
              * @param vecPositon
              *        vector of Positions of all the vertices in input graph
              *
              * @return none
              *
              * @throw none
              */
              void shiftVertically(int &iIndexOfTopmostVertex, int &iNewVertexIndex,
                                   SubGraph & gInputGraph, std::vector<Position> &vecPositon);

};

#endif // GRIDBASEDLAYOUT_H
