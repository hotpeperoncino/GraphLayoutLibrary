#ifndef LAYOUTTYPEDEFS_H
#define LAYOUTTYPEDEFS_H

#include <QString>
#include <QMap>
#include <QPair>
#include <ReaderWriterUtilities/GraphElementProperties.h>

/**
 * User defined type for element name to property name pair.
 */
typedef QPair<QString, QString> ElementNamePropertyNamePair;

/**
 * User defined type for map of graph element name to list of attributes.
 */
typedef QMap <QString , GraphElementProperties > MapGraphElementToAttributeList;

//For attributes e.g. "node" "height" - "key0";or "edge" "direction" - "key1"

/**
 * User defined type for map of element name to it's key name.
 * e.g. "node" "height" - "key0";or "edge" "direction" - "key1"
 */
typedef QMap < ElementNamePropertyNamePair , QString > MapElementNameAndPropertyToKey;

/**
 * User defined type for vertex index to its boolean value indicating if that vertex is already visited or not.
 */
typedef QMap <int , bool> MapVisitedVertices;

/**
 * User defined type for edge index to its boolean value indicating if that edge is already visited or not.
 */
typedef QMap <int , bool> MapVisitedEdges;


#endif // TYPEDEFS_H
