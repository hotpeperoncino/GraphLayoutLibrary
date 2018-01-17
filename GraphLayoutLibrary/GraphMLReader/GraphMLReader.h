#ifndef GRAPHMLREADER_H
#define GRAPHMLREADER_H

#include <qxml.h>
#include <QXmlSchemaValidator>
#include <QXmlSchema>
#include <GraphLayoutLibrary_global.h>
#include <ReaderWriterUtilities/AttributeConstants.h>
#include <Common/BoostGraphWrapper.h>
#include "dirtynode.h"
#include "dirtyedge.h"
#include "MessageHandler.h"
#include <LayoutException/LayoutFileIOException.h>


/** @name Datatype for key
  *  datatype for key
  */
typedef enum Keytype
{
    INTIGER_TYPE = 0,
    DOUBLE_TYPE,
    BOOLEAN_TYPE,
    QSTRING_TYPE
}KeyType;

/** @brief Datatype for information about key
  */
typedef struct KeyInfo
{
    QString sKeyId;
    KeyType keyType;
    QString sKeyName;
    QString sDomain;
    QString sDefaultValue;
}KeyInfo;

/** @brief Datatype for information about data
  */
typedef struct DataInfo
{
    QString sKeyId;
    QString sKeyValue;
    QString sDomainID;
    QString sDomain;
}DataInfo;

/** @brief Datatype for storing vertex and edge properties
  */
typedef struct EntityProperty
{
    int iCoordX;
    int iCoordY;
    int iHeight;
    int iWidth;
    bool bIsBidirectional;
    EntityProperty()
    {
        iCoordX = 0;
        iCoordY = 0;
        iHeight = 0;
        iWidth = 0;
        bIsBidirectional = false;
    }
}EntityProperty;

/** @brief Datatype for storing graph properties
  */
typedef struct GraphProperty
{
    int iCoordX;
    int iCoordY;
    int iHeight;
    int iWidth;
    GraphProperty()
    {
        iCoordX = 0;
        iCoordY = 0;
        iHeight = 0;
        iWidth = 0;
    }
}GraphProperty;

/** @brief Map of vertices to their IDs
  */
typedef QMap<QString , VertexDescriptor> VertexIdToVertexDescriptor;
/** @brief Map of vertices to their Dummy nodes
  */
typedef QMap<VertexDescriptor,bool> VertexDescriptorToIsDummyNode;
/** @brief Map of key-information to IDs of their keys
  */
typedef QMap<QString , KeyInfo> mapKeyIdToKeyInfo;

/**
  Class GraphMLReader :- It is about implementing Graphml reader's functionality using default handler
  provided by xml parsing in qt with SAX approach.

  Added 'GRAPHLAYOUTLIBRARYSHARED_EXPORT' before class name inorder to get access to Client.
  This ensures that the right macro is seen by both library and clients.
*/

class GRAPHLAYOUTLIBRARYSHARED_EXPORT GraphMLReader : public QXmlDefaultHandler
{
private:

    // Counter variables used to maintain state within handler functions
    unsigned int m_iNodeStartAt; /*!< Unsigned integer to store index of first node. Counter variable used to maintain state within handler functions. */
    unsigned int m_iNodeEndAt; /*!< Unsigned integer to store index of last node. Counter variable used to maintain state within handler functions. */
    unsigned int m_iGraphStartAt; /*!< Unsigned integer to store index of first graph. Counter variable used to maintain state within handler functions. */
    unsigned int m_iGraphEndAt; /*!< Unsigned integer to store index of last graph. Counter variable used to maintain state within handler functions. */
    unsigned int m_iNode_GraphFoundCounter; /*!< Unsigned integer to store count of graphs. Counter variable used to maintain state within handler functions. */

    DirtyNode m_dirtyNode; /*!< DirtyNode to store the node properties */
    DirtyEdge m_dirtyEdge; /*!< DirtyEdge to store the edge properties */

    VertexIdToVertexDescriptor mapVertexIdToVertexDescriptorObj; /*!< map of node id to node descriptor object */
    VertexDescriptorToIsDummyNode mapVertexDescriptorToIsDummyNode; /*!< map of whether the node is dummy or not */
    mapKeyIdToKeyInfo m_mapKeyIdToKeyInfo; /*!< map of key id to key info */

    /* data variables to store the information for the various tags as a domain
     *and its information as a domain information.*/
    QString m_sCurrentText; /*!< String to store a tag as text*/
    QString m_sCurrentKeyId; /*!< String to store id*/
    QString m_sCurrentDomainId; /*!< String to store tag-id*/
    QString m_sCurrentDomain; /*!< String to store tag*/
    QString m_sDefaultValue; /*!< String to store default value of a tag*/

    // data variables to store the information about the key maping properties to various entities.
    vector<KeyInfo> m_vecKeyInfo; /*!< Vector of KeyInfo to store list of keys*/
    int m_iCurrentKeyValue; /*!< Integer to store numeric keys value*/
    bool m_bCurrentKeyValue; /*!< Boolean to store true/false keys value*/
    double m_dCurrentKeyValue; /*!< Double to store decimal keys value*/
    QString m_sCurrentKeyValue; /*!< String to store keys value read from file*/

    // Structue for storing data values
    EntityProperty structEntityProperty; /*!< EntityProperty to store properties of an entity */
    GraphProperty structGraphProperty; /*!< GraphProperty to store properties of a graph */
    KeyInfo structKeyInfo; /*!< Integer to store information of key */

    // Error
    QString m_sErrorText; /*!< String to store error */
    bool m_bGraphMLTag; /*!< Boolean to track start/end of graphML tag */

    // for graph after node bounding in single pass parser
    bool m_bGraphTag; /*!< Boolean to track start/end of graph tag */

    // for the bend points
    // edge flag
    bool m_bEdgeTag; /*!< Boolean to track start/end of edge tag */
    bool m_bEdgeDataTag; /*!< Boolean to track start/end of edge-data tag */

    // data values validation(using flags).
    // for graph
    bool m_bIsGraphCoordXSet; /*!< Boolean to track whether valid value for x-coordinte of graph is present */
    bool m_bIsGraphCoordYSet; /*!< Boolean to track whether valid value for y-coordinte of graph is present */
    bool m_bIsGraphHeightSet; /*!< Boolean to track whether valid value for height of graph is present */
    bool m_bIsGraphWidthSet; /*!< Boolean to track whether valid value for width of graph is present */

    // for nodes
    bool m_bIsNodeCoordXSet; /*!< Boolean to track whether valid value for x-coordinte of nodes is present */
    bool m_bIsNodeCoordYSet; /*!< Boolean to track whether valid value for y-coordinte of nodes is present */
    bool m_bIsNodeHeightSet; /*!< Boolean to track whether valid value for height of nodes is present */
    bool m_bIsNodeWidthSet; /*!< Boolean to track whether valid value for width of nodes is present */

    // for edge
    bool m_bIsEdgeDirectionSet; /*!< Boolean to track whether valid direction of edges is present */

    // Private Helper Functions
    /** @name Helper
     *  The methods under this section are helper functions of GraphMLReader.
     */
    /**
        This method adds a vertex to the current graph.

        @pre sVertexId != EMPTY_STRING_VALUE

        @pre mapVertexIdToVertexDescriptor.contains(sVertexId) != true

        @param sVertexId

        @return none

        @throw none
    */
    void addVertexToSubgraph(QString sVertexId);

    /**
        This method adds a edge to the main graph.

        @pre sSourceVertexId != EMPTY_STRING_VALUE

        @pre sTargetVertexId != EMPTY_STRING_VALUE

        @param sSourceVertexId

        @param sTargetVertexId

        @param sEdgeId

        @return none

        @throw MemoryException
            -# NULL_POINTER_EXCEPTION if referenced entity is null

        @throw LayoutException
            -# INVALID_PARAMETER if unknown parameter is passed
    */
    void addEdgeToMainGraph(QString sSourceVertexId, QString sTargetVertexId,QString sEdgeId);

    /**
        This method calls QXmlDefaultHandler's handler to start processing of graphML tags

        @pre none

        @param sNamespaceURI

        @param sTagElement

        @param sName

        @param attribute

        @return true if processed elements else false

        @throw MemoryException
            -# NULL_POINTER_EXCEPTION if referenced entity is null

        @throw LayoutException
            -# INVALID_PARAMETER if unknown parameter is passed
            -# INVALID_ATTRIBUTE_VALUE if invalid attribute value is returned.

        @throw FileIOException
            -# INVALID_FILE_FORMAT if other file than graphml is imported
    */
    bool startElement( const QString&, const QString&, const QString&,
                       const QXmlAttributes& attribute);

    /**
        This method gets the text values.

        @pre none

        @param sCurrentString

        @return true if processed text value else false

        @throw none
    */
    bool characters(const QString &sCurrentString);


    /**
        This method calls QXmlDefaultHandler's handler to start coresponding element from startElement of graphML tags

        @pre none

        @param sNamespaceURI

        @param sTagElement

        @param sName

        @return true if processed element else false

        @throw MemoryException
            -# NULL_POINTER_EXCEPTION if referenced entity is null

        @throw LayoutException
            -# INVALID_PARAMETER if unknown parameter is passed
            -# INVALID_ATTRIBUTE_VALUE if invalid attribute value is returned.

        @throw FileIOException
            -# INVALID_FILE_FORMAT if other file than graphml is imported
    */
    bool endElement( const QString&, const QString&, const QString&);

    /**
        This method gets the error text value

        @pre errorText should be already written.

        @return error string value

        @throw none
    */
    QString errorString() const;

    /**
        This method converts the text values into integer value.

        @pre sKeyValue != empty

        @param sKeyValue

        @return none

        @throw none
    */
    void convertTextToInt(QString sKeyValue);

    /**
        This method converts the text values into boolean value.

        @pre sKeyValue != empty

        @param sKeyValue

        @return none

        @throw none
    */
    void convertTextToBool(QString sKeyValue);

    /**
        This method converts the text values into double value.

        @pre sKeyValue != empty

        @param sKeyValue

        @return none

        @throw none
    */
    void convertTextToDouble(QString sKeyValue);

    /**
       this method checks whether the input graphML file is valid or not.
     * @brief validateGraphML
     *
     * @pre sGraphMLInstanceFileName != empty
     *
     * @param sGraphMLInstanceFileName
     *
     * @return none
     *
     * @throw none
     */
    void validateGraphML(QString sGraphMLInstanceFileName);

    /**
     *this method returns whether the file is supported by library or not.
     *@pre !sGraphMLFileName.empty()
     *
     *@param file name in string
     *
     *@return true if file supported otherwise false.
     *
     *@throw none
    */
    bool isSupportedGraphmlFile(QString sGraphMLFileName);


public:
    /** @name Creators
     *  The methods under this section are responsible for constructing or
     *  destructing an instance of type LayoutException.
     */
	//@{
    /**
     Constructs new object of GraphMLReader.
    @pre none

    @param none

    @return none

    @throw  none
    */
    GraphMLReader();

    /**
     Destroys object of GraphMLReader.
    @pre none

    @param none

    @return none

    @throw none
    */
    ~GraphMLReader();
	//@}
	
/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of GraphMLReader.
  */
 //@{
    /**
     *this method returns the graph object which stores the graph contents with subgraph hierarchy.
     *@pre file != NULL
     *
     *@param pointer to file
     *
     *@return subgraph reference
     *
     *@throw none
    */
    SubGraph& readGraphML(QFile *file);
//@}
  };

#endif // XMLSAXHANDLER_H
