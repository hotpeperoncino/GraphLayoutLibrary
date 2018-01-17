
#include "dirtynode.h"


DirtyNode::DirtyNode()
{
    m_bIsDirtyNode = false;
}

void DirtyNode:: setID(QString sId)
{
    LAYOUT_ASSERT((sId.trimmed().isEmpty() != true),
                  LayoutException(__FUNCTION__,LayoutExceptionEnum::INVALID_PARAMETER,EMPTY_ID_FOUND,NODE));
//    if((sId.trimmed().isEmpty() == true))
//    {
//        throw LayoutException(__FUNCTION__,ExceptionEnum::INVALID_PARAMETER,EMPTY_ID_FOUND,NODE);
//    }
    m_sId = sId;
}

QString DirtyNode::getID()
{
    LAYOUT_ASSERT((m_sId != EMPTY_STRING_VALUE),
                  LayoutException(__FUNCTION__,
                                  LayoutExceptionEnum::INVALID_PARAMETER,
                                  EMPTY_ID_FOUND,
                                  NODE));
    return m_sId;
}
