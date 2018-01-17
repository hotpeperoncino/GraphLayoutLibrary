#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H
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
#include <ForceBasedLayout/Constants.h>
#include<ForceBasedLayout/VertexOverlapRemoval.h>

/**
 * @brief The Postprocessing class
 *
 * The class provides helpers to perform pre and post-processing on a graph layouted with force directed algorithm .
 */
class Postprocessing
{
public:

    
    int m_iRecursionIndex = 0; /*!< Number of times cluster overlap removal runs  */

    
    int m_iVertexOverlapConter = 0; /*!< Number of times vertex overlap removal runs  */

    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type MembershipInitializer. Performs a series of operations after layouting
     */

    //@{
    Postprocessing();
    //@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of Postprocessing.
  */
 //@{
    /** Handles Empty Subgraphs by them placing to some default location after
     * Layouting.
    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph
    @return none
    @throw boost graph exception
    */
    void applyPostProcessing(SubGraph& gMaingraph);


    /** Handles Empty Subgraphs by addind dummy vertex to them.

    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph
    @return none
    @throw none

    */
    void applyPreProcessing(SubGraph& gMaingraph);



    /** A wrapper method to call checkAndFixMembership.
    @pre
        -# gMaingraph != NULL
    @param gMaingraph
        reference to graph
    @return none
    @throw none

    */
    void membershipFixer(SubGraph& gMaingraph );


    /** As a part of post processing Checks whether any vertex of some other
     * subgraph has accindentally entered its bound and moves it, if so .

    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @return none

    @throw none

    */
    void checkAndFixMembership(SubGraph& gMaingraph);


    /** Check whether two vertices are too close to each other if so moves them
     *  some constant distance apart.

    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph
    @return none
    @throw none

    */
    void overlapRemoval(SubGraph& gMaingraph);


    /** Check whether two clusters  overlap each other if so
     * calls clusterOverlapRemovalWrapper.

    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph
    @return none
    @throw none

    */
    void checkClusterOverlap(SubGraph& gMaingraph);
//@}



private:


    /** Moves overlapping clusters to some distance apart.
    @pre
        -# gMaingraph != NULL
    @param gMaingraph
        reference to graph
    @return none
    @throw none
    */
    void clusterOverlapRemoval(SubGraph& gChildOne ,SubGraph& gChildSecond);



};

#endif // POSTPROCESSING_H
