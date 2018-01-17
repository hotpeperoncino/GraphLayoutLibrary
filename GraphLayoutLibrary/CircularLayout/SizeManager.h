#ifndef SIZEMANAGER_H
#define SIZEMANAGER_H

#include <GraphLayoutLibrary_global.h>
#include <Common/GraphType.h>
#include <Common/BoostGraphWrapper.h>
#include <Common/ConstantType.h>

/**
This class implements the functionality for adjusting the size in terms of Height, Width
CenterCoordX, CenterCoordY, LeftCoordX, and TopCoordY and calculating space required for the
Graph and all clusters.
*/

class GRAPHLAYOUTLIBRARYSHARED_EXPORT SizeManager
{
private:
    /**
     * Refers to the object of the BoostGraphWrapper
     */
    BoostGraphWrapper m_boostGraphWrapper;        

    /**
    This function calculates the area of node.

    @pre
    -# gSubgraph != NULL

    @param vVertex
        vertex descriptor
    @param gSubgraph
        reference to graph

    @return area of node

    @throw none
    */
    double calculateAreaOfNode(VertexDescriptor vVertex, SubGraph& gSubgraph);


    /**
    This function calculates the area of cluster.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @return area of cluster

    @throw none
    */
    double calculateAreaOfCluster(SubGraph& gSubgraph);


    /**
    This function calculates maximum radius for every subgraph of cluster iteratively using DFS traversing.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @return maximum radius length

    @throw MemoryException
    -# NULL_POINTER_EXCEPTION if referenced entity is null

    @throw LayoutException
    -# INVALID_PARAMETER if invalid value is passed

    @throw BoostException
    -# if exception caused by bost library
    */
    double calculateGraphMaxRadiusUsingDFS(SubGraph& gSubgraph);


    // functionality to calculate Graph size properties as Height, Width, Left X and Top Y

    /**
   This function calculates the width of cluster.

   @pre
   -# gSubgraph != NULL

   @param gSubgraph
       reference to graph

   @return width of cluster

   @throw none
   */
    int calculateGraphWidth(SubGraph& gSubgraph);

    /**
   This function calculates the height of cluster.

   @pre
   -# gSubgraph != NULL

   @param gSubgraph
       reference to graph

   @return height of cluster

   @throw none
   */
    int calculateGraphHeight(SubGraph& gSubgraph);

public:
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type SizeManager.
  */
 //@{
    /**
        constructs the object of type SizeManager.
    */
    SizeManager();
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type SizeManager.
 */
//@{
    /**
     This function calculates the diagonal length of the node.
    * @brief claculateDiagonalOfNode
    * @pre
    * -# gSubgraph != NULL
    * @param vVertex
    *      vertex descriptor
    * @param gSubgraph
    *      reference to graph
    * @return length of diagonal
    *
    * @throw BoostException
    *  -# if exception is thrown by boost library
    */
    double calculateDiagonalOfNode(VertexDescriptor &vVertex, SubGraph &gSubgraph);

    /**
    This function calculates the left X coordinate of the node.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @param vVertex
        vertex descriptor

    @return left x coordinate of node


    @throw none
    */
    int calculateNodeLeftXFromCenterX(VertexDescriptor vVertex, SubGraph& gSubgraph);


    /**
    This function calculates the top Y coordinate of the node.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @param vVertex
        vertex descriptor

    @return top y coordinate of node

    @throw none
    */
    int calculateNodeTopYFromCenterY(VertexDescriptor vVertex, SubGraph& gSubgraph);

   /**
   This function calculates the center x coordinate of cluster.

   @pre
   -# gSubgraph != NULL

   @param gSubgraph
       reference to graph

   @return center x coordinate of cluster

   @throw none
   */
    int calculateGraphCenterCoordX(SubGraph& gSubgraph);


   /**
   This function calculates the center Y coordinate of cluster.

   @pre
   -# gSubgraph != NULL

   @param gSubgraph
       reference to graph

   @return center y cordinate of cluster

   @throw none
   */
    int calculateGraphCenterCoordY(SubGraph& gSubgraph);

   /**
   This function calculates the left X coordinate of the cluster.

   @pre
   -# gSubgraph != NULL

   @param gSubgraph
       reference to graph

   @return left x coordinate of cluster

   @throw none
   */
    int calculateGraphLeftXFromCenterX(SubGraph& gSubgraph);


   /**
   This function calculates the top y coordinate of the cluster.

   @pre
   -# gSubgraph != NULL

   @param gSubgraph
       reference to graph

   @return top y coordinate of cluster

   @throw none
   */
    int calculateGraphTopYFromCenterY(SubGraph& gSubgraph);
//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of CircularLayoutGenerator.
  */
 //@{
   /**
   This function processes the graph and calculates x, y, height and width.

   @pre
   -# gSubgraph != NULL

   @param gSubgraph
       reference to graph

   @return none

    @throw MemoryException
    -# NULL_POINTER_EXCEPTION if referenced entity is null

    @throw LayoutException
    -# INVALID_PARAMETER if invalid value is passed
    -# INVALID_ATTRIBUTE_VALUE if invalid value to any entity is passed

    @throw BoostException
    -# if exception is thrown by boost library function.
   */
    void processSizeManager(SubGraph& gSubgraph);
//@}
};

#endif // SIZEMANAGER_H
