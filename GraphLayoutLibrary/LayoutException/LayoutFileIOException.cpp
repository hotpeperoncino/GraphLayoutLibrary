#include "LayoutFileIOException.h"

LayoutFileIOException::LayoutFileIOException()
{
}

LayoutFileIOException::LayoutFileIOException(QString sFunctionName,
                                 LayoutExceptionEnum::FileIOExceptionSubTypes enExceptionSubType,
                                 QString sFileName, QString sExpectedFileType):
    LayoutException(sFunctionName, LayoutExceptionEnum::LAYOUT_FILEIOEXCEPTION)
{
    m_sFileName = sFileName;
    m_enExceptionSubType = enExceptionSubType;
    m_sExpectedFileType = sExpectedFileType;

    QString sExceptionErrorMessage = constructMessageForExceptionSubType();
    this->setErrorMessage(sExceptionErrorMessage);
}

LayoutFileIOException::~LayoutFileIOException() throw()
{
}


QString LayoutFileIOException::getFileName()
{
    return m_sFileName;
}


int LayoutFileIOException::getErrorCode()
{
    return m_enExceptionSubType;
}


QString LayoutFileIOException::constructMessageForExceptionSubType()
{
    QString sErrorMessage;

    switch(m_enExceptionSubType)
    {
    case LayoutExceptionEnum::FILE_NOT_FOUND:
        sErrorMessage = FILEIOEXCEPTION_FILENOTFOUND_EXCEPTION_MESSAGE;
        break;

    case LayoutExceptionEnum::UNSUPPORTED_FILE_TYPE:
        sErrorMessage = FILEIOEXCEPTION_UNSUPPORTEDFILETYPE_EXCEPTION_MESSAGE;
        break;

    case LayoutExceptionEnum::UNSUPPORTED_FILE_FORMAT:
        sErrorMessage = FILEIOEXCEPTION_UNSUPPORTEDFILEFORMAT_EXCEPTION_MESSAGE;
        break;

    case LayoutExceptionEnum::FILE_READ_OPERATION_ERROR:
        sErrorMessage = FILEIOEXCEPTION_FILEREADOPERATIONERROR_ERRROR_MESSAGE;
        break;

    case LayoutExceptionEnum::INVALID_FILE_NAME:
        sErrorMessage = FILEIOEXCEPTION_INVALIDFILENAME_MESSAGE;
        break;

    case LayoutExceptionEnum::DIRECTORY_NOT_FOUND:
        sErrorMessage = FILEIOEXCEPTION_DIRECTORY_NOT_FOUND_EXCEPTION_MESSAGE;
        break;

    case LayoutExceptionEnum::LAYOUT_INVALID_FILE_FORMAT:
        sErrorMessage = FILEIOEXCEPTION_INVALIDFILEFORMAT_EXCEPTION_MESSAGE;
        break;

    case LayoutExceptionEnum::INVALID_FILE_OPERATION:
    {
        sErrorMessage = FILEIOEXCEPTION_INVALIDFILEOPERATION_EXCEPTION_MESSAGE;
        sErrorMessage = sErrorMessage.arg(m_sExpectedFileType);
    }
        break;

    default:
        sErrorMessage = FILEIOEXCEPTION_UNKNOWN_EXCEPTION_MESSAGE;
        break;
    }

    // Replace %1 with file name
    sErrorMessage = sErrorMessage.arg(m_sFileName);

    qDebug(sErrorMessage.toStdString().c_str());

    return sErrorMessage;
}

LayoutExceptionEnum::FileIOExceptionSubTypes LayoutFileIOException::getExceptionSubType()
{
    return m_enExceptionSubType;
}
