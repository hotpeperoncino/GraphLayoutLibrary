#ifndef ATTRIBUTECONSTANTS_H
#define ATTRIBUTECONSTANTS_H

//**********************************************************
   /* TAG NAMES */
//***********************************************************
#define GRAPHML "graphml"
#define XML_EXTN "xml"
#define NODE "node"
#define GRAPH "graph"
#define EDGE "edge"
#define DATA "data"
#define KEY "key"
#define DEFAULT "default"

#define NODE_ID "id"
#define NODE_TYPE "type"
#define NODE_HEIGHT "height"
#define NODE_WIDTH "width"
#define NODE_COORDINATE_X "coord_x"
#define NODE_COORDINATE_Y "coord_y"

#define EDGE_ID "id"
#define EDGE_TYPE "type"
#define SOURCE_OF_EDGE "source"
#define TARGET_OF_EDGE "target"
#define EDGE_TYPE "type"
#define EDGE_BIDIRECTIONAL "bidirectional"
#define EDGE_BENDPOINTS "bendpoints"
#define BEND "bend"
#define XVALUE "X"
#define YVALUE "Y"

#define GRAPH_ID "id"
#define GRAPH_NODE_ID_PREFIX "n_"
#define GRAPH_ID_PREFIX "G"

#define KEY_ID_PREFIX "key"
#define FOR "for"
#define KEY_ID "id"
#define ATTR_NAME "attr.name"
#define ATTR_TYPE "attr.type"
#define BOOL_TYPE_STRING "boolean"
#define INT_TYPE_STRING "integer"
#define DOUBL_TYPE_STRING "double"
#define TRUE_VALUE "true"
#define FALSE_VALUE "false"

#define NOT_GRAPHML_FILE "not graphml file"
#define INVALID_VERSION "invalid version"
#define INVALID_ENCODING "invalid encoding"
#define VERSION "version"
#define VERSION_VALUE "1.0"
#define ENCODING "encoding"
#define ENCODING_VALUE "UTF-8"
#define OPEN_GRAPHML_FILE    "Open GraphML File"
#define GRAPHML_FILE         "GraphML Files (*.graphml *.xml)"
#define FILE_LOADED          "File loaded"
#define GRAPHML_READER       "GraphML Reader"
#define CANT_READ_FILE       "Cannot read file %1:\n%2."
#define GRAPHML_VALIDATION   "GraphML Validation"
//#define SCHEMA_FILE_PATH "D:\\SVN\\Graph_Layouting_Algorithms\\GraphLayout\\Dependencies\\graphML1.xsd"
#define SCHEMA_FILE_PATH ":/validation/graphML1.xsd"

// Exception Handling Constants
#define EMPTY_ID_FOUND "empty id found for the node"
#define EMPTY_STRING_VALUE ""
#define INVALID_EDGE_PARAMETER "invalid value for source or target vertex"
#define LOCAL_TO_GLOBAL "local to global index problem"
#define FORWARD_REFERENCE "forward reference problem for node in add edge"
#endif // ATTRIBUTECONSTANTS_H
