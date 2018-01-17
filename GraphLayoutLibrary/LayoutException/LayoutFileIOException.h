#ifndef LAYOUTFILEIOEXCEPTION_H
#define LAYOUTFILEIOEXCEPTION_H

#include <GraphLayoutLibrary_global.h>
#include <LayoutException/LayoutException.h>
#include <LayoutException/LayoutExceptionConstants.h>

/**
 * @brief The LayoutFileIOException class
 *
 * The class defines layout file IO exception enums. Inherites LayoutException.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayoutFileIOException : public LayoutException
{
private:

    QString m_sFileName; /*!< File name */
    QString m_sExpectedFileType; /*!< Expected File name */

    LayoutExceptionEnum::FileIOExceptionSubTypes m_enExceptionSubType; /*!< stores the subtype of FileIOException */

    // Private Helper Functions
    /** @name Helper
     *  The methods under this section are helper functions of FileIOException.
     */
//@{
    /**
    Constructs the error message string for the specified exception sub type.

    @pre none

    @param none

    @return string containing error message specific to the exception subtype.

    @throw none
     */
    QString constructMessageForExceptionSubType();
//@}

protected:
public:

    //Creator
    /** @name Creators
     *  The methods under this section are responsible for constructing or
     *  destructing an instance of type FileIOException.
     */
//@{
	/**
     Constructs new object of type LayoutFileIOException.
    @pre none

    @param none

    @return none

    @throw none
    */
	LayoutFileIOException();

    /**
     Constructs new object of FileIOException using specified parameters.

    @pre none

    @param sFunctionName
        name of the function at which the exception has occurred.
    @param enExceptionSubType
        valid value of FileIOExceptionSubTypes enum.
    @param sFileName
        name of the file for which FileIOException is thrown.
    @param sExpectedFileType
        Expected file type for the operartion.

    @throw none
     */
	LayoutFileIOException(QString sFunctionName, LayoutExceptionEnum::FileIOExceptionSubTypes enExceptionSubType, QString sFileName, QString sExpectedFileType ="");

	/**
     Destroys object of type LayoutFileIOException.
	 
	 @throw exception
    */
    ~LayoutFileIOException() throw();
//@}

    //Queries
    /** @name Queries
     *  The methods under this section are responsible for querying
     *  an instance of type FileIOException.
     */
//@{
    /**
     Gets the file name for which exception is occurred.

    @pre none

    @param none

    @return string containing file name.

    @throw none
     */
    QString getFileName();

    /**
     This function returns the error code associated with the exception.

    @pre none

    @param none

    @return the error code associated with the exception.

    @throw none
    */
    int getErrorCode();

	/**
     This function returns the subtype of the exception.

    @pre none

    @param none

    @return FileIOExceptionSubTypes

    @throw none
    */
    LayoutExceptionEnum::FileIOExceptionSubTypes getExceptionSubType();
//@}
};

#endif // LAYOUTFILEIOEXCEPTION_H
