#include "LayoutException.h"

LayoutException::LayoutException(QString sFunctionName, LayoutExceptionEnum::LayoutExceptionTypes enExceptionType)
{
    m_enExceptionType = enExceptionType;
    m_sExceptionFunctionName = sFunctionName;

    m_enExceptionSubType = LayoutExceptionEnum::UNKNOWNLAYOUTEXCEPTION;
}


LayoutException::LayoutException(QString sFunctionName,
                                 LayoutExceptionEnum::LayoutExceptionSubTypes enLayoutExceptionSubType)
{
    m_enExceptionType = LayoutExceptionEnum::LAYOUTEXCEPTION;
    m_sExceptionFunctionName = sFunctionName;

    m_enExceptionSubType = enLayoutExceptionSubType;
}

LayoutException::LayoutException(QString sFunctionName,
                                 LayoutExceptionEnum::LayoutExceptionSubTypes enLayoutExceptionSubType,
                                 QString sEntityValue,
                                 QString sEntityType)
{
    m_enExceptionType = LayoutExceptionEnum::LAYOUTEXCEPTION;
    m_sExceptionFunctionName = sFunctionName;

    m_enExceptionSubType = enLayoutExceptionSubType;
    m_sEntityValue = sEntityValue;
    m_sEntityType =  sEntityType;

    QString sErrorMessage = this->constructMessageForExceptionSubType();
    this->setErrorMessage(sErrorMessage);
}

LayoutException::LayoutException(QString sFunctionName,
                                 LayoutExceptionEnum::LayoutExceptionSubTypes enLayoutExceptionSubType,
                                 QString sEntityValue,
                                 QString sEntityType,
                                 QString sAttributeName,
                                 QString sAttributeValue)
{
    m_enExceptionType = LayoutExceptionEnum::LAYOUTEXCEPTION;
    m_sExceptionFunctionName = sFunctionName;

    m_enExceptionSubType = enLayoutExceptionSubType;
    m_sEntityValue = sEntityValue;
    m_sEntityType =  sEntityType;

    m_sAttributeName = sAttributeName;
    m_sAttributeValue = sAttributeValue;

    QString sErrorMessage = this->constructMessageForExceptionSubType();
    this->setErrorMessage(sErrorMessage);
}

LayoutException::~LayoutException() throw()
{

}

QString LayoutException::constructMessageForExceptionSubType()
{
    QString sErrorMessage = " ";

    switch(m_enExceptionSubType)
    {

    case LayoutExceptionEnum::INVALID_PARAMETER:
    {
        sErrorMessage = LAYOUTEXCEPTION_INVALID_PARAMETER_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityType, m_sEntityValue);
    }
        break;

    case LayoutExceptionEnum::DUPLICATE_ID:
    {
        sErrorMessage = LAYOUTEXCEPTION_DUPLICATE_ID_FOUND_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityId, m_sEntityType);
    }
        break;

    case LayoutExceptionEnum::INVALID_ATTRIBUTE_VALUE:
    {
        sErrorMessage = LAYOUTEXCEPTION_INVALID_ATTRIBUTE_VALUE_MESAAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityType, m_sAttributeName, m_sAttributeValue);
    }
        break;

    case LayoutExceptionEnum::MANDATORY_FIELD_MISSING:
    {
        sErrorMessage = LAYOUTEXCEPTION_MANDATORY_ATTRIBUTE_MISSING_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sAttributeName,m_sEntityType);
    }
        break;

    case LayoutExceptionEnum::EMPTY_CONTAINER:
    {
        sErrorMessage == LAYOUTEXCEPTION_EMPTY_CONTAINER_FOUND_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityType);
    }
        break;

    case LayoutExceptionEnum::INVALID_TYPE:
    {
        sErrorMessage = LAYOUTEXCEPTION_INVALID_TYPE_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityValue);
    }
        break;

    case LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET:
    {
        sErrorMessage = LAYOUTEXCEPTION_REQUIRED_PARAMETER_NOT_SET_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityValue);
    }
        break;

    case LayoutExceptionEnum::NOT_FOUND_IN_CONTAINER:
    {
        sErrorMessage = LAYOUTEXCEPTION_NOT_FOUND_IN_CONTAINER_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityType, m_sEntityValue);
    }
        break;

    case LayoutExceptionEnum::INVALID_OPERATION:
    {
        sErrorMessage = LAYOUTEXCEPTION_INVALID_OPERATION_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityValue , m_sEntityType);
    }
        break;
    case LayoutExceptionEnum::INCONSISTENT_DATASTRUCTURE:
    {
        sErrorMessage = LAYOUTEXCEPTION_INCONSISTENT_DATASTRUCTURE_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityType, m_sEntityValue);
    }
        break;
    case LayoutExceptionEnum::INVALID_GRAPH_TYPE:
    {
        sErrorMessage = LAYOUTEXCEPTION_INVALID_GRAPH_TYPE_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sEntityValue, m_sEntityType);
    }
        break;
    default:
        sErrorMessage = LAYOUTEXCEPTION_UNKNOWN_EXCEPTION_MESSAGE;
        break;
    }
    return sErrorMessage;
}


void LayoutException::setExceptionType(LayoutExceptionEnum::LayoutExceptionTypes enExceptionType)
{
    m_enExceptionType = enExceptionType;
}

void LayoutException::setErrorMessage(QString sErrorMessage)
{
    m_sErrorMessage = sErrorMessage;
}


void LayoutException::setExceptionFunctionName(QString sFunctionName)
{
    m_sExceptionFunctionName = sFunctionName;
}

unsigned int LayoutException::getExceptionType()
{
    return m_enExceptionType;
}

QString LayoutException::getErrorMessage()
{
    return m_sErrorMessage;
}

QString LayoutException::getExceptionFunctionName()
{
    return m_sExceptionFunctionName;
}

int LayoutException::getErrorCode()
{
    return m_enExceptionSubType;
}

QString LayoutException::getEntityValue()
{
    return m_sEntityValue;
}

QString LayoutException::getEntityType()
{
    return m_sEntityType;
}

LayoutExceptionEnum::LayoutExceptionSubTypes LayoutException::getExceptionSubType()
{
    return m_enExceptionSubType;
}

LayoutException::LayoutException()
{

}
