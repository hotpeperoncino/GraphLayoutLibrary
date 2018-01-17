#ifndef FORCEBASEDLAYOUTGENERATOR_H
#define FORCEBASEDLAYOUTGENERATOR_H
#include <GraphLayoutLibrary_global.h>
#include <Common/BoostGraphWrapper.h>
#include <boost/graph/fruchterman_reingold.hpp>

/**
 * @brief The ForceBasedLayoutGenerator class
 *
 * The class provides force based layout of  graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT ForceBasedLayoutGenerator
{

    BoostGraphWrapper m_boostGraphWrapper; /*!< member boost graph wrapper */
public:

    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type ForceBasedLayoutGenerator.
     */

    //@{
    ForceBasedLayoutGenerator();
    //@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of ForceBasedLayoutGenerator.
  */
 //@{
    /**
     This functions processes the graph to apply force based layout.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        referece to input graph

    @return none

    @throw none
    */

    void applyForceBasedLayout(SubGraph& gSubgraph);
//@}

};

#endif // FORCEBASEDLAYOUTGENERATOR_H
