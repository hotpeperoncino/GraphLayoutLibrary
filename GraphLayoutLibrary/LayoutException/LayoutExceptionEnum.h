#ifndef LAYOUTEXCEPTIONENUM_H
#define LAYOUTEXCEPTIONENUM_H

#include <GraphLayoutLibrary_global.h>
#include <LayoutException/LayoutExceptionConstants.h>
#include <QString>


/**
 * @brief The LayoutExceptionEnum class
 *
 * The class defines layout exception enums.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT LayoutExceptionEnum
{
public:
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type LayoutExceptionEnum.
  */
 //@{
	/**
     Constructs new object of type LayoutExceptionEnum.
    @pre none

    @param none

    @return none

    @throw none
    */
    LayoutExceptionEnum();
//@}

/**
 * Enum for types of layout related exceptions.
 */
    enum  LayoutExceptionTypes
    {
        LAYOUTEXCEPTION = EXCEPTION_ERROR_CODE_START,
        LAYOUT_FILEIOEXCEPTION = LAYOUTEXCEPTION + EXCEPTION_ERROR_CODE_START_OFFSET,
        LAYOUT_MEMORYEXCEPTION = LAYOUT_FILEIOEXCEPTION + EXCEPTION_ERROR_CODE_START_OFFSET,
        LAYOUT_BOOSTEXCEPTION = LAYOUT_MEMORYEXCEPTION + EXCEPTION_ERROR_CODE_START_OFFSET
    };

/**
 * Enum for subtypes of layout related exceptions.
 */
    enum LayoutExceptionSubTypes
    {
        INVALID_PARAMETER = LAYOUTEXCEPTION,

        DUPLICATE_ID = LAYOUTEXCEPTION + 1,
        INVALID_ATTRIBUTE_VALUE = LAYOUTEXCEPTION + 2,
        MANDATORY_FIELD_MISSING = LAYOUTEXCEPTION + 3,

        EMPTY_CONTAINER = LAYOUTEXCEPTION + 4,
        INVALID_TYPE = LAYOUTEXCEPTION + 5,

        REQUIRED_PARAMETER_NOT_SET = LAYOUTEXCEPTION + 6,
        GRAPH_CONTAINS_CYCLE = LAYOUTEXCEPTION + 7,
        NOT_FOUND_IN_CONTAINER = LAYOUTEXCEPTION + 8,

        INCONSISTENT_DATASTRUCTURE = LAYOUTEXCEPTION + 9,
        INVALID_OPERATION = LAYOUTEXCEPTION + 10,

        INVALID_GRAPH_TYPE = LAYOUTEXCEPTION + 11,

        UNKNOWNLAYOUTEXCEPTION = LAYOUTEXCEPTION + EXCEPTION_ERROR_CODE_END_OFFSET
    };

/**
 * Enum for subtypes of file IO related exceptions.
 */
    enum FileIOExceptionSubTypes
    {
        FILE_NOT_FOUND = LAYOUT_FILEIOEXCEPTION,
        INVALID_FILE_NAME = LAYOUT_FILEIOEXCEPTION + 1,
        UNSUPPORTED_FILE_TYPE = LAYOUT_FILEIOEXCEPTION + 2,
        UNSUPPORTED_FILE_FORMAT = LAYOUT_FILEIOEXCEPTION + 3,
        FILE_READ_OPERATION_ERROR = LAYOUT_FILEIOEXCEPTION + 4,
        DIRECTORY_NOT_FOUND = LAYOUT_FILEIOEXCEPTION + 5,
        LAYOUT_INVALID_FILE_FORMAT = LAYOUT_FILEIOEXCEPTION + 6,
        INVALID_FILE_OPERATION = LAYOUT_FILEIOEXCEPTION + 7,
        UNKNOWNFILEIOEXCEPTION = LAYOUT_FILEIOEXCEPTION + EXCEPTION_ERROR_CODE_END_OFFSET
    };

/**
 * Enum for subtypes of memory related exceptions.
 */
    enum MemoryExceptionSubTypes
    {
        NULL_POINTER_EXCEPTION = LAYOUT_MEMORYEXCEPTION,
        UNKNOWNMEMORYEXCEPTION = LAYOUT_MEMORYEXCEPTION + EXCEPTION_ERROR_CODE_END_OFFSET
    };

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type GraphCycleHandler.
 */
//@{
	/** static member function to convert layout exception enum to string

    @pre
    
    @param enExceptionTypes
        Enum of type LayoutExceptionTypes
    
    @return QString

    @throw boost graph exception
    */
    static QString getStringForEnum(LayoutExceptionEnum::LayoutExceptionTypes enExceptionTypes);
//@}
};

#endif // LAYOUTEXCEPTIONENUM_H
