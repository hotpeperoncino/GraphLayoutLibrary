#include "LayoutMemoryException.h"

LayoutMemoryException::LayoutMemoryException()
{
}

LayoutMemoryException::LayoutMemoryException(QString sFunctionName,
                                 LayoutExceptionEnum::MemoryExceptionSubTypes enExceptionSubType,
                                 QString sObjectName) :
    LayoutException(sFunctionName, LayoutExceptionEnum::LAYOUT_MEMORYEXCEPTION)
{
    m_sObjectName = sObjectName;
    m_enExceptionSubType = enExceptionSubType;

    QString sExceptionErrorMessage = constructMessageForExceptionSubType();
    this->setErrorMessage(sExceptionErrorMessage);
}

LayoutMemoryException::~LayoutMemoryException() throw()
{
}

QString LayoutMemoryException::getObjectName()
{
    return m_sObjectName;
}

int LayoutMemoryException::getErrorCode()
{
    return m_enExceptionSubType;
}


QString LayoutMemoryException::constructMessageForExceptionSubType()
{
    QString sErrorMessage;

    switch(m_enExceptionSubType)
    {
    case LayoutExceptionEnum::NULL_POINTER_EXCEPTION:
        sErrorMessage = MEMORYEXCEPTION_NULL_POINTER_EXCEPTION_MESSAGE;
        break;

    default:
        sErrorMessage = MEMORYEXCEPTION_UNKNOWNEXCEPTION_MESSAGE;
        break;
    }
    //replace '%1' object
    sErrorMessage = sErrorMessage.arg(m_sObjectName);

    qDebug(sErrorMessage.toStdString().c_str());

    return sErrorMessage;
}
LayoutExceptionEnum::MemoryExceptionSubTypes LayoutMemoryException::getExceptionSubType()
{
    return m_enExceptionSubType;
}

