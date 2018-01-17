#ifndef DIRTYEDGE_H
#define DIRTYEDGE_H
#include <QString>
#include <LayoutException/LayoutException.h>
#include <ReaderWriterUtilities/AttributeConstants.h>
#include <Common/GraphType.h>
/**
  DirtyEdge : This class is for maintaining the attributes of edges as we cannot directly access
  the attributes of edges in the endElement part of the Sax Handler
  So store the attributes of edge into member variables of this class using set() functiionality.
  and have to fetch those attributes in endElement using get() functionality.

  */

class DirtyEdge
{
private:
    QString m_sSourceId; /*!< String to store a ID of source nede of dirty edge*/
    QString m_sTargetId; /*!< String to store a ID of target nede of dirty edge*/
    QString m_sEdgeId; /*!< String to store a ID of the dirty edge*/
    BendPoints *m_strBendPoints; /*!< Structure to bound the bend point's  x and y coordinates into single object and store it into the list. */

public:
    bool m_bIsDirtyEdge; /*!< Boolean flag used to maintain the state for edge whether it should be added in the graph or not. */
    QVectorBendPoints m_vecBendPoints; /*!< List containing the objects of bendpoints. */

    /** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type DirtyEdge.
     */

    //@{
	/**
     Constructs new object of type DirtyEdge.
    @pre none

    @param none

    @return none

    @throw none
    */
	DirtyEdge();
	//@}

/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of DirtyEdge.
  */
 //@{
    /**
        This method writes the souce and target id's of the edge

        @pre sSourceId.trimmed().isEmpty() == false
             sTargetId.trimmed().isEmpty() == false

        @param sSourceId

        @param sTargetId

        @return none

        @throw LayoutException
            -# INVALID_PARAMETER if invalid values is passsed to function.
    */
    void setSourceTarget(QString sSourceId , QString sTargetId);

    /**
        This method writes the edge id of the edge

        @pre sEdgeId.trimmed().isEmpty() == false

        @param sEdgeId

        @return none

        @throw none
    */
    void setId(QString sEdgeId);
	
	/**
        This method sets the bend points coordinates to the list

        @pre none

        @param iValueX
            X coordinate value

        @param iValueY
            Y coordinate value

        @return none

        @throw none
    */
    void setEdgeBendPointsCoordinates(int iValueX, int iValueY);
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type DirtyEdge.
 */
//@{
    /**
        This method returns the source id of the edge

        @pre id should be written already

        @param sEdgeId
        value of edge id

        @return soruce id of a edge

        @throw none
    */
    QString getSource();

    /**
        This method returns the target id of the edge

        @pre id should be written already

        @param none

        @return target id of edge

        @throw none
    */
    QString getTarget();

    /**
        This method returns the edge id of the edge

        @pre id should be already written

        @param none

        @return id of edge

        @throw none
    */
    QString getId();
//@}
	
    

};

#endif // DIRTYEDGE_H
