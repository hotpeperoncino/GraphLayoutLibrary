#ifndef RANDOMLAYOUTGENERATOR_H
#define RANDOMLAYOUTGENERATOR_H

#include <GraphLayoutLibrary_global.h>

#include <Common/BoostGraphWrapper.h>
#include <ForceBasedLayout/Reingold.h>
#include <boost/graph/random_layout.hpp>
#include <math.h>

/**
 * @brief The RandomLayoutGenerator class
 *
 * The class provides helpers to apply ramdom layout to graphs.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT RandomLayoutGenerator
{
private:
    BoostGraphWrapper m_boostGraphWrapper; /*!< Wrapper for boost graph object */
    LayoutCompartment m_mainCompartment;   /*!< Current main compartment */
    struct Point    
    {
        int iXCoord;
        int iYCoord;
    }; /*!< Struct of X and Y Cordinate */

public:

/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type RandomLayoutGenerator.
 */

//@{
	/**
     Constructs new object of type RandomLayoutGenerator.
    @pre none

    @param none

    @return none

    @throw none
    */
    RandomLayoutGenerator();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of RandomLayoutGenerator.
  */
 //@{
    /** Applies Random Layout to main graph instance
    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @return none

    @throw none
    */

    void applyRandomLayout(SubGraph& gMainGraph);


    /** Applies Random Layout to a clustered subgraph instance
    @pre
        -# gInputGraph != NULL

    @param gInputGraph
        reference to input graph

    @param gRootGraph
        reference to root graph

    @return none

    @throw none
    */
    void applyRandomLayoutForClusteredGraph(SubGraph & gInputGraph, SubGraph &gRootGraph);

    /** Calculates the amount of offset in rows and column a point (node) needs to be displaced
    @pre none

    @param iBlockCount
        number of blocks

    @param iMaxHeight
        max height of node

    @param iMaxWidth
        max width of node

    @param oOffsetOfBlock
        Vector of offset blocks

    @return none

    @throw none
    */
    void calculateBlockOffsets(int &iBlockCount, int iMaxHeight, int iMaxWidth, std::vector<Point> &oOffsetOfBlock);
//@}
};

#endif // RANDOMLAYOUTGENERATOR_H
