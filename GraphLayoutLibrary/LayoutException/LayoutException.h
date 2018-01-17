#ifndef LAYOUTEXCEPTION_H
#define LAYOUTEXCEPTION_H

#include <GraphLayoutLibrary_global.h>
#include <LayoutException/LayoutExceptionEnum.h>
#include <QString>
#include <QObject>
#include <stdexcept>
#include <boost/exception/exception.hpp>

/**
 * @brief The LayoutException class
 *
 * The class defines layout related exceptions. Inherited fron standard exceptions and Boost exceptions.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayoutException : std::exception, boost::exception
{
private:

    LayoutExceptionEnum :: LayoutExceptionTypes m_enExceptionType; /*!< Exception Type */
    QString m_sErrorMessage;    /*!< Error Message */
    QString m_sExceptionFunctionName;   /*!< Function where exception occured */
    LayoutExceptionEnum :: LayoutExceptionSubTypes m_enExceptionSubType;

    /**  stores the id(VertexDescriptor, EdgeDescriptor, SubGraph) for the entity */
    QString m_sEntityId;
    QString m_sEntityType;
    QString m_sEntityValue;

    /**  stores the attribute name and value */
    QString m_sAttributeName;
    QString m_sAttributeValue;

    // Private Helper Functions
    /** @name Helper
     *  The methods under this section are helper functions of LayoutException.
     */

    /**
    Constructs the error message string for the specified exception sub type.

    @pre none

    @param none

    @return string containing error message specific to the exception subtype.

    @throw none
     */
    QString constructMessageForExceptionSubType();

protected:

    //Modifiers
/** @name Modifiers
 *  The methods under this section are responsible for modifying
 *  an instance of type LayoutException.
 */
//@{
    /**
    Sets Current exception in member variable

    @pre none

    @param LayoutExceptionEnum
    layout exception enum

    @return none

    @throw none
     */
    void setExceptionType(LayoutExceptionEnum::LayoutExceptionTypes enExceptionType);

    /**
    Sets Current error message in member variable

    @pre none

    @param sErrorMessage
    Error message

    @return none

    @throw none
     */
    void setErrorMessage(QString sErrorMessage);
//@}

public:

    //Creator
/** @name Creators
 *  The methods under this section are responsible for constructing or
 *  destructing an instance of type LayoutException.
 */
//@{
	/**
     Constructs new object of type LayoutException.
    @pre none

    @param none

    @return none

    @throw none
    */
    LayoutException();

	/**
     Constructs new object of type LayoutException.
    @pre none

    @param sFunctionName
		Name of function which threw exception 
	
	@param enExceptionType
		Type of exception. Enum of type LayoutExceptionTypes. default : LAYOUTEXCEPTION
	
	@return none

    @throw none
    */
    LayoutException(QString sFunctionName, LayoutExceptionEnum::LayoutExceptionTypes enExceptionType = LayoutExceptionEnum::LAYOUTEXCEPTION);

	/**
     Constructs new object of type LayoutException.
    @pre none

    @param sFunctionName
		Name of function which threw exception 
	
	@param enLayoutExceptionSubType
		Subtype of exception. Enum of type LayoutExceptionSubTypes.

    @return none

    @throw none
    */
    LayoutException(QString sFunctionName, LayoutExceptionEnum::LayoutExceptionSubTypes enLayoutExceptionSubType);

	/**
     Constructs new object of type LayoutException.
    @pre none

    @param sFunctionName
		Name of function which threw exception 
	
	@param enLayoutExceptionSubType
		Subtype of exception. Enum of type LayoutExceptionSubTypes.
		
	@param sEntityValue
		String 
	
	@param sEntityType
		String

    @return none

    @throw none
    */
    LayoutException(QString sFunctionName,
                    LayoutExceptionEnum::LayoutExceptionSubTypes enLayoutExceptionSubType,
                    QString sEntityValue,
                    QString sEntityType = "");

	/**
     Constructs new object of type LayoutException.
    @pre none

    @param sFunctionName
		Name of function which threw exception 
	
	@param enLayoutExceptionSubType
		Subtype of exception. Enum of type LayoutExceptionSubTypes.
		
	@param sEntityValue
		String 
	
	@param sEntityType
		String
		
	@param sAttributeName
		String 
	
	@param sAttributeValue
		String

    @return none

    @throw none
    */
    LayoutException(QString sFunctionName,
                    LayoutExceptionEnum::LayoutExceptionSubTypes enLayoutExceptionSubType,
                    QString sEntityValue,
                    QString sEntityType,
                    QString sAttributeName,
                    QString sAttributeValue);
    
    /**
     Destroys object of type LayoutException.
	 
	 @throw exception
    */
    ~LayoutException() throw();
//@}


    //Queries
/** @name Queries
 *  The methods under this section are responsible for querying
 *  an instance of type LayoutException.
 */
//@{
    /**
     This function returns the type of the exception.

    @pre none

    @param none

    @return the type of the exception.

    @throw none
    */
    virtual unsigned int getExceptionType();

    /**
     This function returns the error message associated with the exception.

    @pre none

    @param none

    @return the error message associated with the exception.

    @throw none
    */
    QString getErrorMessage();


    /**
     This function returns the name of the function which has thrown the exception.

    @pre none

    @param none

    @return the name of the function which has thrown the exception.

    @throw none
    */
    QString getExceptionFunctionName();


    /**
     This function returns the error code associated with the exception.

    @pre none

    @param none

    @return the error code associated with the exception.

    @throw none
    */
    virtual int getErrorCode();


    /**
     This function returns the string containing entity value

    @pre none

    @param none

    @return the string containing entity value associated with the exception.

    @throw none
    */
    QString getEntityValue();

    /**
     This function returns the string containing type of the entity associated with the exception.

    @pre none

    @param none

    @return the string containing entity value associated with the exception.

    @throw none
    */
    QString getEntityType();

    /**
     This function returns enum Layout Exception SubType associated with the exception.

    @pre none

    @param none

    @return enum Layout Exception SubType associated with the exception.

    @throw none
    */
    LayoutExceptionEnum::LayoutExceptionSubTypes getExceptionSubType();
//@}

	//Modifiers
/** @name Modifiers
 *  The methods under this section are responsible for modifying
 *  an instance of type LayoutException.
 */
//@{
    void setExceptionFunctionName(QString sFunctionName);
//@}
};

#endif // LAYOUTEXCEPTION_H
