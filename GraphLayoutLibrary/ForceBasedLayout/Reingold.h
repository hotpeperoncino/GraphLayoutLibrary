#ifndef REINGOLD_H
#define REINGOLD_H
#include<Common/GraphType.h>
#include<Common/BoostGraphWrapper.h>
#include<Common/GraphType.h>
#include<algorithm>
#include<cmath>
#include <HierarchicalLayoutGenerator/HierarchicalLayouter.h>
#include<Common/GraphType.h>
#include<algorithm>
#include<math.h>
#include <ForceBasedLayout/SpringEmbedder.h>

/**
 * @brief The Reingold class
 *
 * The class provides implementation of Reingold-Fruchtermann algorithm.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT Reingold
{
public:
	double iK = 0; /*!< Inter vertex distance */

/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type Reingold.
 */
//@{
	/**
     Constructs new object of type Reingold.
    @pre none

    @param none

    @return none

    @throw none
    */
    Reingold();
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type Reingold.
 */
//@{
    /** Returns attractive force between two points

    @pre
        -# gGraph != NULL

    @param idist
        distance between two nodes


    @return attractive force

    @throw none

    */
    float getAttractiveForce(double idist);

    /** Returns repulsive force between two points

    @pre
        -# gGraph != NULL

    @param idist
        distance between two nodes

    @return repulsive force

    @throw none */
    float getRepulsiveForce(double idist);
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of Reingold.
  */
 //@{
	/** Reingold Fruchtermann algo
    @pre
        -# l_graph != NULL
    @param l_graph
        reference to graph
    @return none
    @throw none
    */
    void ApplyReingold(SubGraph& l_graph);
	
    /**
     * @brief setCompartMentProps
     *
     * Calculates and sets compartment properties of the graph and its subgraphs.
     *
     * @param l_graph
     *          reference to graph whose compartment properties are to be set.
     *
     * @param iMargine
     *          margine to be used for innermost cluster.
     *
     * @return integer value of margine used for outermost cluster.
     *
     * @throw LayoutException
     */
    int setCompartMentProps(SubGraph &l_graph, int iMargine);


    /** Iterates Clusters and calls ApplyReingold

    @pre
        -# l_graph != NULL

    @param l_graph
        reference to graph
    @return none
    @throw none
    */
    void ApplyLocalFR(SubGraph& l_graph);


    /** Calculates and set height of graph and its subgraphs , in use

    @pre
        -# l_graph != NULL
    @param l_graph
        reference to graph
    @return none
    @throw none
    */
    void  SetClusterCompartmentProps(SubGraph& l_graph);


    /** Calculates and set height of graph and its subgraphs for Force directed
     *  lay out.
    @pre
        -# l_graph != NULL
    @param l_graph
        reference to graph
    @return none
    @throw none
    */
    void  SetClusterCompartmentPropsNew(SubGraph &l_graph);
//@}

};

#endif // REINGOLD_H
