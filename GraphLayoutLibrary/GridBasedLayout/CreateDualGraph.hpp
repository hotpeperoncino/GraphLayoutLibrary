#ifndef CREATEDUALGRAPH_HPP
#define CREATEDUALGRAPH_HPP

#include <GraphLayoutLibrary_global.h>

#include <vector>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <GridBasedLayout/MyPlanarFaceTraversal.hpp>

#include <iostream>
#include <boost/graph/properties.hpp>
#include <boost/graph/lookup_edge.hpp>
#include <boost/ref.hpp>

#include <algorithm>

using namespace boost;

/**
 * A structure to maintain dual graph of a planar graph. Inherited from my_planar_face_traversal_visitor.
 */
template <typename InputGraph, typename OutputGraph, typename EdgeIndexMap>
struct dual_graph_visitor : public my_planar_face_traversal_visitor
{

    typedef typename graph_traits<OutputGraph>::vertex_descriptor vertex_t;
    typedef typename graph_traits<InputGraph>::edge_descriptor edge_t;
    typedef typename std::vector<vertex_t> vertex_vector_t;
    typedef iterator_property_map< typename vertex_vector_t::iterator, EdgeIndexMap > edge_to_face_map_t;
    typedef typename graph_traits<OutputGraph>::edge_descriptor edge_descriptor_t;
    typedef typename property_map<OutputGraph, edge_index_t>::type e_index_t;
    typedef typename graph_traits<OutputGraph>::edges_size_type edge_count_t;
    typedef typename graph_traits<OutputGraph>::edge_iterator edge_iteratot_t;

   dual_graph_visitor(InputGraph& arg_g, OutputGraph& arg_dual_g, EdgeIndexMap arg_em) :
        g(arg_g),
        dual_g(arg_dual_g),
        em(arg_em),
        edge_to_face_vector(num_edges(g), graph_traits<OutputGraph>::null_vertex()),
        edge_to_face(edge_to_face_vector.begin(), em)
    {
    }

    void begin_face()
    {
        //add a vertex (for current face being traversed) to dualG
        current_face = add_vertex(dual_g);
    }

    template <typename Edge, typename adjFaces, typename dualEdges>
    void next_edge(Edge& e, adjFaces &adjecentFaces, dualEdges& dual) //traverse edges bounding current face
    {
        vertex_t existing_face = edge_to_face[e];
        if (existing_face == graph_traits<OutputGraph>::null_vertex()) //if edge is traversed first time
        {
            edge_to_face[e] = current_face;
        }
        else //if edge is traversed second time
        {
            //add edge between its 2 adjecent faces (in dualG)
            edge_descriptor_t newEdge = add_edge(existing_face, current_face, dual_g).first;

            //Initialize the interior edge index in dualG
            e_index_t e_index = get(edge_index, dual_g);
            edge_count_t edge_count = 0;
            edge_iteratot_t ei, ei_end;
            for(boost::tie(ei, ei_end) = edges(dual_g); ei != ei_end; ++ei)
                put(e_index, *ei, edge_count++);

            //remember its dual edge from g
            dual[get(edge_index, dual_g, newEdge)] = e;
        }

        //remember current face as an 'adjecent face' to vertices on current edge (from g)
        adjecentFaces[get(vertex_index, g, source(e, g))].push_back(current_face); //adjecentFaces[u] <= current_face;
        adjecentFaces[get(vertex_index, g, target(e, g))].push_back(current_face); //adjecentFaces[v] <= current_face;
    }

    InputGraph& g;
    OutputGraph& dual_g;
    EdgeIndexMap em;
    vertex_t current_face;
    vertex_vector_t edge_to_face_vector;
    edge_to_face_map_t edge_to_face;
};


template <typename InputGraph, typename OutputGraph, typename PlanarEmbedding, typename EdgeIndexMap, typename adjFaces, typename dualEdges>
void create_dual_graph(InputGraph& g, OutputGraph& dual_g, PlanarEmbedding embedding, EdgeIndexMap em, adjFaces &adjecentFaces, dualEdges& dualE)
{
    dual_graph_visitor<InputGraph, OutputGraph, EdgeIndexMap> visitor(g, dual_g, em);
    planar_face_traversal(g, embedding, visitor, em, adjecentFaces, dualE);
}

template <typename InputGraph, typename OutputGraph, typename PlanarEmbedding, typename adjFaces, typename dualEdges>
void create_dual_graph(InputGraph& g, OutputGraph& dual_g, PlanarEmbedding embedding, adjFaces &adjecentFaces, dualEdges& dualE)
{
    create_dual_graph(g, dual_g, embedding, get(edge_index,g), adjecentFaces, dualE);
}

#endif // CREATEDUALGRAPH_HPP
