#ifndef LAYERSSUBGRAPHSORTER_H
#define LAYERSSUBGRAPHSORTER_H

#include <GraphLayoutLibrary_global.h>

#include <HierarchicalLayoutGenerator/SubgraphOrderingGraph.h>
#include <HierarchicalLayoutGenerator/HierarchicalLayoutTypedefs.h>

/**
 * @brief The LayersSubgraphSorter class
 *
 * The class provides helpers to sort layers in hierarchhical layout.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayersSubgraphSorter
{
private:
    MapLayerIdToLayerRef * m_mapLayeredGraphRef;        /*!< Reference to map of layer id to reference */
    NestingTreeSubgraphNode * m_rootNestingTreeNodeRef; /*!< Reference to nesting tree node reference */

public:

/** @name Creators
 *  The methods under this section are responsible for constructing
 *  an instance of type LayersSubgraphSorter.
 */

//@{
	/**
     Constructs new object of type LayersSubgraphSorter.
    @pre none

    @param none

    @return none

    @throw none
    */
    LayersSubgraphSorter();

	/**
     Constructs new object of type LayersSubgraphSorter.
    @pre none

    @param rootNestingTreeNode
		reference of NestingTreeSubgraphNode
		
	@param layeredGraph
		reference of MapLayerIdToLayerRef

    @return none

    @throw none
    */
    LayersSubgraphSorter(NestingTreeSubgraphNode& rootNestingTreeNode , MapLayerIdToLayerRef &layeredGraph);
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type LayersSubgraphSorter.
 */
//@{
    /*! Generate SubgraphOrdering Graph
     *Give topological order to vertices according to Subgraph Ordering Graph
     *Sort Layered Graph According to Subgraph Ordering Graph */

    void printStructures()
    {
        qDebug() << " Layered Graph From Layer Sorter";

        IteratorMapLayerIdToLayerRef iterLayer(*m_mapLayeredGraphRef);
        while(iterLayer.hasNext())
        {
            iterLayer.next();
            int iKey = iterLayer.key();
            qDebug() << iKey;
        }


    }
//@}

private:
    //Remove cycles in Subgraph Ordering Graphs



};

#endif // LAYERSSUBGRAPHSORTER_H
