#ifndef HIERARCHICALLAYOUTTYPEDEFS_H
#define HIERARCHICALLAYOUTTYPEDEFS_H

#include <QMap>
#include <Common/GraphType.h>
#include <QQueue>
#include <HierarchicalLayoutGenerator/NestingTreeSubgraphNode.h>
#include <HierarchicalLayoutGenerator/LayerNode.h>
#include <HierarchicalLayoutGenerator/ReducedNestingTreeNode.h>


/*! Layer Data structure:
 *  It can access graph hierararchy horizontally
 */
//This map can be used to map horizontal position of vertex to the vertex descriptor
typedef QMap<std::size_t , VertexDescriptor> MapPositionToVertexDescriptor;

/*! Iterator type for MapPositionToVertexDescriptor */
typedef QMapIterator<std::size_t , VertexDescriptor> IteratorMapPositionToVertexDescriptor;

/*! This map is used to list and access LayerNodes of vertices having same rank
 by their relative position(int) within that layer */
typedef QMap<double , LayerNode*> MapPositionToLayerNode;

typedef QMapIterator<double , LayerNode*> IteratorMapPositionToLayerNode; /*!< Iterator type for LayerNodes at Layer that is MapPositionToLayerNode >*/

typedef QMultiMap<double , LayerNode*> MultiMapBarryCenterToLayerNode; /*!< This is a multimap, key is barry center and value is corresponding LayerNode >*/

typedef QMapIterator<double , LayerNode*> IteratorMultiMapBarryCenterToLayerNode; /*!< Iterator type for LayerNodes of MultiMapBarryCenterToLayerNode >*/

/*! Layers map: This map is used to list and access LayerNodes by their levels */
typedef QMap<int , MapPositionToLayerNode*> MapLayerIdToLayerRef;

/*! Iterator type for Layers that is MapLayerIdToLayerRef */
typedef QMapIterator<int , MapPositionToLayerNode*> IteratorMapLayerIdToLayerRef;


/*! Map of Reduced Nesting trees for every layer
 */
typedef QMap<int , ReducedNestingTreeNode*> MapLayerIdtoReducedNestedTreeRef;


typedef QQueue<NestingTreeSubgraphNode*> QueueNestingTreeSubgraphNodesRef;

/*! Direction Enum - Used for specifying top-down or bottom-up directions
 *  in crossing reduction functions
 */
enum ProcessDirection { UpDirection = 2000 , DownDirection = 2001 };

enum NeighborNodes { Succesor = 2005 , Predecessor = 2006 }; /*!< Neighbour nodes enum >*/

enum LayerType { GraphVerticesLayer = 2500 , BorderVerticesLayer = 2501 , AnyTypeLayer = 2502}; /*!< Layer type enum >*/


#endif // HIERARCHICALLAYOUTTYPEDEFS_H
