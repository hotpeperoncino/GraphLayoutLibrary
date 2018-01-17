#ifndef LAYOTENUM_H
#define LAYOTENUM_H
#include <GraphLayoutLibrary_global.h>
#include <Common/CommonConstants.h>

/**
 * @brief The LayoutEnum class
 *
 * The class provides enumerable types used in layouting and helpers around them.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayoutEnum
{
public:

//***************************GRAPH ENUMS***************************************
	/**
     * enum that defines type of a node in a graph
     */
    enum NodeType {  UpperBorderNode = 999,
                     LowerBorderNode = 1000,
                     DummyNode = 1001,
                     GraphNode = 1002,
                     ExpandableNode = 1003,
                     VerticalBorderNode = 1004,
                     // for adding if empty graph is found
                     InvisibleNode = 1005
                  };
	/**
     * enum that defines type of an edge in a graph
     */
    enum EdgeType {  NestingEdge = 1100,
                     GraphEdge = 1101,
                     LongEdgeSegment = 1102,
                     InvisibleEdge = 1103,
                     VerticalBorderEdgeSegment = 1104
                  };

//***************************GRAPH ENUMS END***********************************

//***************************CIRCULAR LAYOUT ENUMS*****************************
	/**
     * enum that defines ordering criterion of a node in a circular layout
     */
    enum VertexOrderCriteria
    {
        TopologicalOrder = 0,
        ConnectedComponentOrder = 1,
        DefaultOrder = 2
    };

//****************************CIRCULAR LAYOUT ENUMS END************************

//****************************HIERARCHICAL LAYOUT ENUMS************************

//****************************HIERARCHICAL LAYOUT ENUMS END********************
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type LayoutEnum.
  */
 //@{
    /**
     Constructs new object of type Enum.
    @pre none

    @param none

    @return none

    @throw none
    */
    LayoutEnum();
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type LayoutEnum.
 */
//@{
    /**
    This fucntion returns true if specified vertex ordering criteria is valid otherwise false

    @pre none

    @param enVertexOrderCriteria
        vertex ordering criteria containing Enum::VertexOrderCriteria enVertexOrderCriteria type

    @return true if valid vertex ordering criteria otherwise false

    @throw none
    */
    static bool isValidVertexOrderingCriteria(LayoutEnum::VertexOrderCriteria enVertexOrderCriteria);


    /**
    This fucntion returns true if specified vertex type is valid otherwise false

    @pre none

    @param enVertexType
        vertex type containing Enum::VertexType type

    @return true if valid vertex type otherwise false

    @throw none
    */
    static bool isValidCircularVertexType(LayoutEnum::NodeType enVertexType);
//@}
};

#endif // LAYOTENUM_H
