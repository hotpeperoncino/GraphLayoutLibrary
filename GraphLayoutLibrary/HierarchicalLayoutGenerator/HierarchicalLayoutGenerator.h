#ifndef HIERARCHICALLAYOUTGENERATOR_H
#define HIERARCHICALLAYOUTGENERATOR_H

#include <GraphLayoutLibrary_global.h>
#include <Common/BoostGraphWrapper.h>
#include <Common/LayoutUtility.h>
#include <Common/GraphCycleHandler.h>

/**
 * @brief The HierarchicalLayoutGenerator class
 *
 * The class provides helpers to manage hierarchical lyout of a graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT HierarchicalLayoutGenerator
{

    BoostGraphWrapper *boostGraphWrapper; /*!< Pointer to boost graph wrapper object >*/
    int m_iHorizontalStep; /*!< Horizontal step counter >*/
    int m_iVerticalStep;   /*!< Verical step counter >*/
    int m_iTreeDepth;      /*!< Tree Depth >*/
    int m_iTreeWidth;      /*!< Tree Width >*/
    LayoutCompartment m_mainCompartment;

public:

    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type HierarchicalLayoutGenerator.
     */

    //@{
	/**
     Constructs new object of type HierarchicalLayoutGenerator.
    @pre none

    @param none

    @return none

    @throw none
    */
    HierarchicalLayoutGenerator();
    //@}

    void test();

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of HierarchicalLayoutGenerator.
  */
 //@{
    /** Applies Hierarchical Layout

    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @return none

    @throw none

    */
    void applyHierarchicalLayout(SubGraph& gMainGraph);


    /** Applies X and Y coordinates to verices from position and rank

    @pre
        -# gMaingraph != NULL

    @param gMaingraph
        reference to graph

    @param iVerticleStep
        Vertical Step

    @param iHorizontalStep
        Horizontal Step

    @return none

    @throw none

    */
    void applyCoordinatesFromPositionAndRank(SubGraph& gMainGraph, int iVerticleStep, int iHorizontalStep);
//@}
private:

    /** Sets tree width by traversing subgraph

    @pre
        -# &subgraph != NULL

    @param subgraph
        reference to subgraph

    @param iDistanceFromRoot
        Distance from root

    @param rootVertex
        Root vertex

    @return none

    @throw none

    */
    int setTreeWidth(VertexDescriptor& rootVertex , int iDistanceFromRoot, SubGraph &subgraph);

    void setCoordinates(VertexDescriptor& vRootVertex , SubGraph& gSubgraph);
};

#endif // HIERARCHICALLAYOUTGENERATOR_H
