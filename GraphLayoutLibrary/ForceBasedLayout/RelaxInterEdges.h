#ifndef RELAXINTEREDGES_H
#define RELAXINTEREDGES_H

#include "ClusteredSpringEmbedder.h"
#include "MembershipInitializer.h"
#include <HierarchicalLayoutGenerator/HierarchicalLayouter.h>
#include <Common/LayoutEnum.h>
#include <Common/GraphType.h>
#include <ForceBasedLayout/Reingold.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <ForceBasedLayout/Postprocessing.h>

/**
 * @brief The RelaxInterEdges class
 *
 * The class provides helpers to manage inter-subgraph edge in graph; used in force directed layout to stabilize edges crossing clusters.
 */
class RelaxInterEdges
{
public:
    BoostGraphWrapper m_bGraphWrapper; /*!< Single object for accessing properties */

    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type RelaxInterEdges.
     */

    //@{
	/**
     Constructs new object of type RelaxInterEdges.
    @pre none

    @param none

    @return none

    @throw none
    */
    RelaxInterEdges();
    //@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of RelaxInterEdges.
  */
 //@{
    /** Calculates and sets  displacement of vertices when one vertex is
     *  in outermost graph and other is in its immediate subgraph.
    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @param vSource
        Source Vertex Discriptor

    @param vTarget
        Target Vertex Discriptor

    @param iOuterIntersect
        Pointer to outer intersect points.

    @return none

    @throw none
    */
    void interEdgesCaseOne(SubGraph& gMaingraph,VertexDescriptor& vSource,
                           VertexDescriptor& vTarget,int* iOuterIntersect);

    /** Calculates and sets  displacement of vertices when Source vertex is
     *  in outermost graph and other is in its non-immediate subgraph.


    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @param vSource
        Source Vertex Discriptor

    @param vTarget
        Target Vertex Discriptor

    @param gTarget
        pointer to target graph

    @param iOuterIntersect
        Pointer to outer intersect points.

    @return none


    @throw none

    */


    void  interEdgesCaseTwoSourceOuter(SubGraph& gMaingraph,
                            VertexDescriptor& vSource,VertexDescriptor& vTarget,
                            int* iOuterIntersect,SubGraph* gTarget);

    /** Calculates and sets  displacement of vertices when Target vertex is
     *  in outermost graph and other is in its non-immediate subgraph.


    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @param vSource
        Source Vertex Discriptor

    @param vTarget
        Target Vertex Discriptor

    @param iOuterIntersect
        Pointer to outer intersect points.

    @param gTarget
        pointer to target graph

    @return none


    @throw LayoutException

    */



    void  interEdgesCaseTwoSourceInner(SubGraph& gMaingraph,
                                       VertexDescriptor& vSource,VertexDescriptor& vTarget,
                                       int* iOuterIntersect,SubGraph* gTarget);

    /** Calculates and sets  displacement of vertices when both Source vertex is
     *  and other is in some overlapping graph.


    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @param vSource
        Source Vertex Discriptor

    @param vTarget
        Target Vertex Discriptor

    @param gTarget
        Pointer to target graph

    @param iSourceGraphId
        GraphId of source graph.

    @return none


    @throw LayoutException

    */


    void  OverLapCaseSourceOuter(SubGraph& gMaingraph, VertexDescriptor& vSource,
                                 VertexDescriptor& vTarget,SubGraph* gTarget,int iSourceGraphId);


    /** Calculates and sets  displacement of vertices when both Source vertex is
     *  and other is in some overlapping graph.


    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @param vSource
        Source Vertex Discriptor

    @param gSource
        Pointer Source graph

    @param vTarget
        Target Vertex Discriptor

    @param iTargetGraphId
        GraphId of target graph.

    @return none


    @throw LayoutException

    */



    void OverLapCaseSourceInner(SubGraph& gMaingraph,VertexDescriptor& vSource,
                                VertexDescriptor& vTarget,
                                SubGraph* gSource,int iTargetGraphId);

    /** Calculates and sets  displacement of vertices when both Source vertex is
     *  and other is in some overlapping graph.


    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @param vSource
        Source Vertex Discriptor

    @param gSource
        Pointer to Source graph

    @param gTarget
        Pointer to Target graph

    @param vTarget
        Target Vertex Discriptor


    @return none


    @throw LayoutException

    */

    void nonOverLapCase(SubGraph& gMaingraph,VertexDescriptor& vSource,
                        VertexDescriptor& vTarget,SubGraph* gSource, SubGraph* gTarget);
//@}

};

#endif // RELAXINTEREDGES_H
