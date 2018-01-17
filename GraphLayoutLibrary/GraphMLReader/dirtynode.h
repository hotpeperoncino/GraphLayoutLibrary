#ifndef DIRTYNODE_H
#define DIRTYNODE_H

#include <QString>
#include <LayoutException/LayoutException.h>
#include <ReaderWriterUtilities/AttributeConstants.h>

/**
  Modifier Class
  DirtyNode : This class is for maintaining the attributes of nodes as we cannot directly access
  the attributes of nodes in the endElement part of the Sax Handler
  So store the attributes of node into member variables of this class using set() functiionality.
  and have to fetch those attributes in endElement using get() functionality.
  */


class DirtyNode
{
private:
    QString m_sId; /*!< String to store a ID of the dirty node*/

public:

    bool m_bIsDirtyNode; /*!< Boolean flag used to maintain the state for node whether it should be added in the graph or not. */
    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type DirtyNode.
     */

    //@{
	/**
     Constructs new object of type DirtyNode.
    @pre none

    @param none

    @return none

    @throw none
    */
	DirtyNode();
	//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of DirtyNode.
  */
 //@{
    /**
      This function writes the node id

      @pre sId != empty
      @param sId
      @return none
      @throw LayoutException
        -# INVALID_PARAMETER if invalid id value is passed to set
     */
    void setID(QString sId);
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type DirtyNode.
 */
//@{
    /**
      This function returns the id of a node which is already written
      @pre id should be already written
      @param none
      @return sId of node
      @throw LayoutException
        -# INVALID_ATTRIBUTE_VALUE if invalid attribute value is returned.
    */
    QString getID();
//@}
};

#endif // DIRTYNODE_H
