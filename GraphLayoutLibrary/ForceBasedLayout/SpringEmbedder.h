#ifndef SKELETONGRAPH_H
#define SKELETONGRAPH_H
#include<Common/GraphType.h>
#include<Common/BoostGraphWrapper.h>
#include "MembershipInitializer.h"
#include <HierarchicalLayoutGenerator/HierarchicalLayouter.h>
#include <Common/LayoutEnum.h>
#include<Common/GraphType.h>


/**
     * @brief SpringEmbedder
     *
     *   Applies Eades Spring embedder algorithm to graphs , does not retain
     *   cluster information.
     */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT SpringEmbedder
{
public:

    QVector<VertexDescriptor> vVertex; /*!< Vector of vertices */

    QVector<double> vXpos; /*!< vector of X Cordinate */
    QVector<double> vYpos; /*!< vector of Y Cordinate */
    QVector<double> xDisp; /*!< Vector of X Displacement  */
    QVector<double> yDisp; /*!< Vector of Y  Displacement */
    QVector<double> xDelta; /*!< Vector of X Delta */
    QVector<double> yDelta; /*!< Vector of X Cordinate */



    double *dataXPos  = NULL; /*!< Pointer to X Cordinate */
    double *dataYPos = NULL; /*!< Pointer to Y Cordinate */
    double *dataXDisp =NULL; /*!< Pointer to X Displacement */
    double *dataYDisp = NULL; /*!< Pointer to Y Displacement */
    double *dataXDelta  = NULL; /*!< Difference to X values */
    double *dataYDelta = NULL; /*!< Differenceto Y values */

    double m_dDesiredEdgeLength = 75; /*!< Desired Edge Length */

    double m_dIterations = 25;  /*!< Desired number of Iterations */

  /** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type SpringEmbedder.
  */
 //@{
	/**
     Constructs new object of type SpringEmbedder.
    @pre none

    @param none

    @return none

    @throw none
    */
    SpringEmbedder();
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of SpringEmbedder.
  */
 //@{
    /** Sets the desired edge lengths (to be implemented )

    @pre
        -# gMainGraph != NULL

    @param edgeLength
        Length of the edge
    @return none
    @throw none
    */
    void setDesiredEdgeLength(double edgeLength);


    /** Sets the desired of iterations  (to be implemented )

    @pre
        -# gMainGraph != NULL

    @param num_iterations
       number of iterations
    @return none
    @throw none
    */
    void setNoOfIterations(double dNumIterations);
	
	/** Alternative method for Reingold Fruchtermann

    @pre
        -# gMainGraph != NULL

    @param gMainGraph
        reference to graph

    @return none
    @throw none
    */
    void  testSpringVectors(SubGraph& gMainGraph);
	
	/** Alternative method for Spring Embedder
    @pre
        -#gMainGraph != NULL
    @param gMainGraph
        reference to graph
    @return none
    @throw none

    */
    void  testSpring(SubGraph& gMainGraph);


    /** Main method for Eades Spring embedder , call Repulse and relax
    @pre
        -# gMainGraph != NULL
    @param gMainGraph
        reference to graph
    @return none
    @throw none
    */
    void  SpringEmbedderStep(SubGraph& gMainGraph);


    /** Relax edges one step at a time
    @pre
        -# gMainGraph != NULL
    @param gMainGraph
        reference to graph
    @return none
    @throw none
    */
    void  SpringRelaxEdges(SubGraph& gMainGraph);


    /** Repel nodes one step at a time
    @pre
        -# gMainGraph != NULL
    @param gMainGraph
        reference to graph
    @return none
    @throw boost graph exception
    */
    void  SpringRepelVertices(SubGraph& gMainGraph);


    /** Addes displacement to nodes
    @pre
        -# gMainGraph != NULL
    @param gMainGraph
        reference to graph
    @return none
    @throw none
    */
    void  SpringMoveNodes(SubGraph& gMainGraph);
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type SpringEmbedder.
 */
//@{
    /** Returns the dimensions of all the enclosing subgraphs as a vector

    @pre
        -# gMainGraph != NULL

    @param gMainGraph
        reference to graph

    @param vRestBound
        Restricted bounds for main graph

    @param vClusters
       Pointer to sub graphs

    @return none
    @throw none
    */
    void getRestrictedBounds(SubGraph& gMainGraph,QVector<int>&
        vRestBound, QVector<SubGraph* > vClusters);
		
		/** Returns attractive force between two points

    @pre
        -# gGraph != NULL
    @param idist
        distance between two nodes

    @param m_iK
        Value of constant K
    @return attractive force
    @throw none

    */
    double  getAttractiveForce(double idist,double m_iK);


    /** Returns repulsive force between two points

    @pre
        -# gMainGraph != NULL

    @param idist
        distance between two nodes

    @param m_iK
        Value of constant K
    @return repulsive force
    @throw none
    */
    double  getRepulsiveForce(double idist,double m_iK);
//@}

};

#endif // SKELETONGRAPH_H
