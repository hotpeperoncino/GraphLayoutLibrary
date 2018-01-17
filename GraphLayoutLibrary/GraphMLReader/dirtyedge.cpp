
#include "dirtyedge.h"

DirtyEdge::DirtyEdge()
{
    m_bIsDirtyEdge = false;
}

void DirtyEdge::setSourceTarget(QString sSourceId, QString sTargetId)
{

    LAYOUT_ASSERT(((sSourceId.trimmed().isEmpty() == false) || (sTargetId.trimmed().isEmpty() == false)),
                  LayoutException(__FUNCTION__,
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  EMPTY_ID_FOUND,
                                  NODE));
    // write souce and target ids
    m_sSourceId = sSourceId;
    m_sTargetId = sTargetId;
}

void DirtyEdge::setId(QString sEdgeId)
{

    LAYOUT_ASSERT(sEdgeId.trimmed().isEmpty() == false,
                  LayoutException(__FUNCTION__,
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  EMPTY_ID_FOUND,
                                  NODE));
    // write edge id
    m_sEdgeId = sEdgeId;
}

QString DirtyEdge::getSource()
{
    return m_sSourceId;
}

QString DirtyEdge::getTarget()
{
    return m_sTargetId;
}

QString DirtyEdge::getId()
{
    return m_sEdgeId;
}

void DirtyEdge::setEdgeBendPointsCoordinates(int iValueX, int iValueY)
{
    // set the coordinates into the BendPoints Structure.
    BendPoints* strBendPoints = new BendPoints();
    strBendPoints->iCoordinateX = iValueX;
    strBendPoints->iCoordinateY = iValueY;

    //cout<<"Bends.... "<<strBendPoints.iCoordinateX<<" "<<strBendPoints.iCoordinateY<<endl;

    // add this structure objects in the list(vector of bendpoints).
    m_vecBendPoints.push_back(strBendPoints);
}
