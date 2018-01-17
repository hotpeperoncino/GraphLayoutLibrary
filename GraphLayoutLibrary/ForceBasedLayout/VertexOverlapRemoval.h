#ifndef VERTEXOVERLAPREMOVAL_H
#define VERTEXOVERLAPREMOVAL_H


#include<Common/GraphType.h>
#include<Common/BoostGraphWrapper.h>
#include <ForceBasedLayout/SpringEmbedder.h>
#include "MembershipInitializer.h"
#include <HierarchicalLayoutGenerator/HierarchicalLayouter.h>
#include <Common/LayoutEnum.h>
#include <Common/GraphType.h>
#include <ForceBasedLayout/Reingold.h>
#include <ForceBasedLayout/Postprocessing.h>
#include <ForceBasedLayout/RelaxInterEdges.h>
#include<LayoutException/LayoutExceptionConstants.h>
#include<ForceBasedLayout/Constants.h>
#include<LayoutManager/GraphLayoutErrorCodes.h>
#include <GraphLayoutLibrary_global.h>
#include<LayoutException/LayoutExceptionEnum.h>
#include <GraphMLReader/GraphMLReader.h>
#include <GraphMLWriter/GraphMLWriter.h>

/**
 * @brief The VertexOverlapRemoval class
 *
 * The class provides helpers to manage vertex overlaps in graph with force directed layout .
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT VertexOverlapRemoval
{
public:
    int m_iVertexOverlapConter = 0; /*!< counter to maintain vertex overlap */ 

    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type VertexOverlapRemoval.
     */
    //@{
	/**
     Constructs new object of type VertexOverlapRemoval.
    @pre none

    @param none

    @return none

    @throw none
    */
    VertexOverlapRemoval();
    //@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of VertexOverlapRemoval.
  */
 //@{
    /** Repels overlapping vertices with other ones.

    @pre
        -# gMainGraph != NULL

    @param gMaingraph
    @return none
    @throw none
    */
    void repelOverlappingVerticesWithOther(SubGraph& gMaingraph);


    /** Removes overlapping from the  vertices .

    @pre
        -# gMainGraph != NULL
    @param gMaingraph
    @return none
    @throw none
    */
    GraphLayoutErrorCodes::LayoutErrorCode removeOverlaps(QString sInputGraphMLFilePath,
                        QString sOutputGraphMLFilePath);
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type VertexOverlapRemoval.
 */
//@{
    /** Returns a vector of  overlapping from the  vertices .

    @pre
        -# gMainGraph != NULL

    @param sInputGraphMLFilePath
    @param sOutputGraphMLFilePath
    @return  LayoutEnum
    @throw none
    */
    QVector<VertexIterator> getOverlappingVertices(SubGraph& gMaingraph);


    /** Returns true if two vertices overlap .

    @pre
        -# gMainGraph != NULL

    @param vVertexFirst
            Vertex Descriptor for first Vertex

    @param vVertexSecond
            Vertex Descriptor for second Vertex

    @param gMaingraph

    @return vOverlappingVertices
            A vector of overlapping vertices

    @throw none

    */
    bool bIsOverlappingVertices(VertexIterator   vVertexFirst
                               ,VertexIterator  vVertexSecond
                               ,SubGraph& gMaingraph);


    /** Returns true if overlap is present in graph .

    @pre
        -# gMainGraph != NULL
    @param gMaingraph

    @return bOverlapExist
            bool value for overlaps
    @throw none
    */
    bool bOverlapExist(SubGraph& gMaingraph);
//@}


};

#endif // VERTEXOVERLAPREMOVAL_H
