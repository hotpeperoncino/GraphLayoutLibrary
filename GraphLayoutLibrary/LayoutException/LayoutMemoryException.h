#ifndef LAYOUTMEMORYEXCEPTION_H
#define LAYOUTMEMORYEXCEPTION_H

#include <GraphLayoutLibrary_global.h>
#include <LayoutException/LayoutException.h>
#include <LayoutException/LayoutExceptionConstants.h>
#include <QString>

/**
 * @brief The LayoutMemoryException class
 *
 * The class defines layout memory exception enums. Inherites LayoutException.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayoutMemoryException : public LayoutException
{

private:
    QString m_sObjectName; /*!< Name of object that triggered exception */

    LayoutExceptionEnum::MemoryExceptionSubTypes m_enExceptionSubType; /*!< Memory exception subtype */


    // Private Helper Functions
    /** @name Helper
     *  The methods under this section are helper functions of MemoryException.
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
public:

    //Creator
    /** @name Creators
     *  The methods under this section are responsible for constructing or
     *  destructing an instance of type MemoryException.
     */
//@{
    /**
     Constructs new object of MemoryException using specified parameters.

    @pre none

    @param sFunctionName
        name of the function at which the exception has occurred.
    @param enExceptionSubType
        valid value of MemoryExceptionSubTypes enum.
    @param sObjectName
        name of the object for which the MemoryException is thrown.

    @throw none
     */
    LayoutMemoryException(QString sFunctionName, LayoutExceptionEnum::MemoryExceptionSubTypes enExceptionSubType,
                    QString sObjectName);

    /**
     Constructs new object of MemoryException.
    */
    LayoutMemoryException();

    /**
        destroy object of typr memoryexception
    */
    ~LayoutMemoryException() throw();
//@}


    //Queries
    /** @name Queries
     *  The methods under this section are responsible for querying
     *  an instance of type MemoryException.
     */
//@{
    /**
     Gets the object name for which exception is occurred.

    @pre none

    @param none

    @return string containing object name.

    @throw none
     */
    QString getObjectName();

    /**
     This function returns the error code associated with the exception.

    @pre none

    @param none

    @return the error code associated with the exception.

    @throw none
    */
    int getErrorCode();

    /**
     This function returns the eception subtype associated with the exception.

    @pre none

    @param none

    @return the exception subtype associated with the exception.

    @throw none
    */
    LayoutExceptionEnum::MemoryExceptionSubTypes getExceptionSubType();
//@}

};

#endif // LAYOUTMEMORYEXCEPTION_H
