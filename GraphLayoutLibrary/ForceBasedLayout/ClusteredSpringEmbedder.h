#ifndef SPRINGEMBEDDERVARIABLESIZENODES_H
#define SPRINGEMBEDDERVARIABLESIZENODES_H
#include<Common/GraphType.h>
#include<Common/BoostGraphWrapper.h>
#include <ForceBasedLayout/SpringEmbedder.h>
#include "MembershipInitializer.h"
#include <HierarchicalLayoutGenerator/HierarchicalLayouter.h>
#include <Common/LayoutEnum.h>
#include <Common/GraphType.h>
#include <ForceBasedLayout/Reingold.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <ForceBasedLayout/Postprocessing.h>
#include <ForceBasedLayout/RelaxInterEdges.h>
#include<LayoutException/LayoutExceptionConstants.h>
#include<ForceBasedLayout/Constants.h>
#include<ForceBasedLayout/VertexOverlapRemoval.h>

/**
 * @brief The ClusteredSpringEmbedder class
 *
 * The class provides helpers for spring embedder algorithm on clustered graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT ClusteredSpringEmbedder
{
public:



    SubGraph *m_gSubgraph = NULL; /*!< Pointer to subgraph */
    SubGraph m_gMaingraph; /*!< Copy of Main graph*/
    BoostGraphWrapper m_bGraphWrapper; /*!< Single graph wrapper object to access properties */


/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type ClusteredSpringEmbedder.
 */
//@{
	/**
     Constructs new object of type ClusteredSpringEmbedder.
    @pre none

    @param none

    @return none

    @throw none
    */
    ClusteredSpringEmbedder();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of ClusteredSpringEmbedder.
  */
 //@{
    /** For each subgraph of received graph.

    @pre
        -# gMaingraph != NULL

    @param iIterations
        number of iterations for algorithm

    @param gMaingraph
        reference to main  graph

    @return none

    @throw LayoutException

    */

    void springForEachCluster(SubGraph& gMaingraph,int iIterations);
//@}


private:

    /** Perform main algo steps iterations time

    @pre
        -# gMainGraph != NULL
        -# gSubgraph != NULL

    @param gMaingraph
        reference to main  graph

    @param gSubgraph
        reference to Subgraph

    @param iIterations
         number of iterations


    @return none

    @throw none

    */

   void springStep(SubGraph& gSubgraph,SubGraph& gMaingraph,int iIterations);



   /** Relax only immediate nodes one step at a time

   @pre
       -# gMainGraph != NULL

   @param gSubgraph
       reference to graph


   @return none

   @throw none

   */


   void springRelaxImmediate(SubGraph& gSubgraph);

   /** Repel only immediate nodes one step at a time

   @pre
       -# gMainGraph != NULL

   @param gSubgraph
       reference to graph


   @return none

   @throw none

   */


   void springRepelImmediate(SubGraph& gSubgraph);

   /** Move the nodes one step in their bounds only

   @pre
       -# gMainGraph != NULL

   @param gSubgraph
       reference to graph


   @return none

   @throw none

   */

   void springMoveRestricted(SubGraph& gSubgraph);

   /** Repel only immediate nodes with immediate clusters  one step at a time.

   @pre
       -# gMainGraph != NULL

   @param gSubgraph
         reference to graph

   @return none

   @throw none

   */


   void springRepelFromClusters(SubGraph& gSubgraph);

   /** Repel only immediate nodes with immediate clusters  one step at a time.

   @pre
       -# gMaingraph != NULL

   @param gSubgraph
       Reference to sub graph

   @return none

   @throw none

   */

   void springRepelClustersToCluster(SubGraph& gSubgraph);

   /** Relax  all the inter edges such of received subgraph such that it does not cross clusters,
    *  generally will be of less power of force.

   @pre
       -# gMaingraph != NULL

   @param gMaingraph
        Reference to main graph

   @return none

   @throw LayoutException

   */

   void springRelaxInterEdges(SubGraph& gMaingraph );

   /** For each subgraph of received graph move clusters.

   @pre
       -# gsubgraph != NULL

   @param gsubgraph
       reference to graph


   @return none

   @throw LayoutException

   */



   void springMoveClusters(SubGraph& gsubgraph);

   /** For each subgraph of received graph. check whether clusters
    * needed to be expanded.

   @pre
       none

   @param none


   @return none

   @throw none

   */

   void springSetVerticesDisplacementZero();

   /** Testing method for recursive type of algorithm which starts lay out
    *  from inner subgraph to outer.
    *

   @pre
       -# gMaingraph != NULL

   @param gMaingraph
       reference to graph


   @return none

   @throw none

   */


   void springTestRecursive(SubGraph& gMaingraph);

   /** Repel only immediate clusters with immediate clusters  one step at a time.

   @pre
       -# gMainGraph != NULL

   @param gSubgraph
         reference to graph

   @return none

   @throw none

   */


   void  springTestRepelClusterToCluster(SubGraph &gSubgraph);







};

#endif // SPRINGEMBEDDERVARIABLESIZENODES_H
