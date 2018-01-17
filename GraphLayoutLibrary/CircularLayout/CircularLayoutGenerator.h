#ifndef CIRCULARLAYOUTGENERATOR_H
#define CIRCULARLAYOUTGENERATOR_H
#include <GraphLayoutLibrary_global.h>
#include <Common/BoostGraphWrapper.h>
#include <CircularLayout/CircleLayouter.h>
#include <CircularLayout/SpaceUtilizer.h>
#include <LayoutUtilities/CommonLayoutConstants.h>
#include <Common/LayoutEnum.h>
#include <Common/ConstantType.h>
#include <qfile.h>

/**
 * @brief The CircularLayoutGenerator class
 *
 * The class provides circular layout of a graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT CircularLayoutGenerator
{
private:
    /**
     * X co-ordinate of the center point of the circle used for circular layout
     */
    int m_iCenterX;
    /**
     * Y co-ordinate of the center point of the circle used for circular layout
     */
    int m_iCenterY;   
    /**
     * Radius of the circle used for circular layout
     */
    double m_dRadius;
    /**
     * Vector containing subgraphs
     */
    vector<SubGraph*> m_vecSubgraphContainer;
    /**
     * Vector containing the subgraphs in BFS order
     */
    vector<SubGraph*> m_vecBFSOrderedSubgraphs;

    /**
     * Object of BoostGraphWrapper to perform various subgraph related operations.
     */
    BoostGraphWrapper m_boostGraphWrapper;
    /**
     * object of SizeManager to process the size of subgraphs.
     */
    SizeManager m_sizeManager;
    /**
     * object of SpaceUtilizer to process the space occupied by the subgraphs.
     */
    SpaceUtilizer m_spaceUtilizer;    
    LayoutEnum::VertexOrderCriteria enVertexOrder;

    QString m_sExecutionLogFileName;
    /**
     This function iterates the childs of graph upto deep level.

    @pre
    -# subgraphQueue != NULL

    @param subgraphQueue
        referece to vector of subgraph pointers

    @return none

    @throw LayoutException
    -# EMPTY_CONTAINER if empty vector without containing subgraphs is passed

    @throw BoostException
    -# if exception caused by bost library
    */
    void iterateChildrenGraphs(vector<SubGraph *> &subgraphQueue);

    /**
      This function calculates the radius using side of the node.

     * @brief calculateRadius
     * @pre
     * -# gSubgraph != NULL
     * @param gSubgraph
     *     reference to input graph
     *
     * @return radius value
     *
     * @throw MemoryException
        -# NULL_POINTER_EXCEPTION if referenced entity is null

       @throw LayoutException
        -# INVALID_PARAMETER if invalid value is passed

        @throw BoostException
        -# if exception caused by bost library
     */
    double calculateRadius(SubGraph &gSubgraph);


public:
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type CircularLayoutGenerator.
  */
 //@{
    /**
        Constructs the object of type CircularLayoutGenerator.
    */
    CircularLayoutGenerator();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of CircularLayoutGenerator.
  */
 //@{
    /**
     This functions processes the graph to apply circle layout. The function assigns new position to 'gInputGraph' vertices.

    @pre
    -# gInputGraph != NULL

    @param gInputGraph
        referece to input graph
    @param enVertexOrder
        enum value specifying the order in which subgraph vertices should be arranged

    @return none

    @throw MemoryException
    -# if violating any type mismatch from memoryexception

    @throw LayoutException
    -# if violating any type mismatch from layoutexception

    @throw BoostException
    -# if exception caused by bost library
    */
    void applyCircularLayout(SubGraph& gInputGraph, LayoutEnum::VertexOrderCriteria enVertexOrder);

    /**
    This function adds edges between dummy node coressponding to subgraph and all nodes of subgraph.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @return none

    @throw none
    */
    void addDummyEdgesForTopologicalOrder(SubGraph& gSubgraph);
//@}
};

#endif // CIRCULARLAYOUTGENERATOR_H
