#ifndef MEMBERSHIPINITIALIZER_H
#define MEMBERSHIPINITIALIZER_H

#include<Common/GraphType.h>
#include<Common/BoostGraphWrapper.h>
#include <HierarchicalLayoutGenerator/HierarchicalLayouter.h>
#include <Common/LayoutEnum.h>

/**
 * @brief The MembershipInitializer class
 *
 * The class provides helpers to manage membership relation between subgraphs and their innet elements.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT MembershipInitializer
{
public:

    QMap<int,SubGraph*> m_MapSubgraphsId; /*!< Map of subgraphs and their IDs */


    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type MembershipInitializer.
     */

    //@{
    MembershipInitializer();
    //@}


/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of MembershipInitializer.
  */
 //@{
    /** Assigns clusterId's and VertexIds in subgraph and graphs

    @pre
        -# mainGraphref != NULL

    @param mainGraphref
        reference to graph


    @return none

    @throw boost graph exception

    */
    void initializeMembers(SubGraph& mainGraphref);
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type MembershipInitializer.
 */
//@{
    /** Create  clusterId's and pointer to subgraph.

    @pre
        -# mainGraphref != NULL

    @param none


    @return m_MapSubgraphsId
         map to subgraph id and pointers

    @throw boost graph exception

    */
    QMap<int,SubGraph*> getIdToSubgraphMap();
//@}




};

#endif // MEMBERSHIPINITIALIZER_H
