#ifndef LAYOUTEXCEPTIONCONSTANTS_H
#define LAYOUTEXCEPTIONCONSTANTS_H

// Macro to show LAYOUT_ASSERT in debug build and throw exceptions in release builds.
#ifdef QT_DEBUG
#define LAYOUT_ASSERT(CONDITION, EXCEPTION) Q_ASSERT(CONDITION)
#else
#define LAYOUT_ASSERT(CONDITION, EXCEPTION) \
    if(false == (CONDITION)) \
    { \
        throw EXCEPTION; \
    }
#endif

// Exception Error codes constants
#define EXCEPTION_ERROR_CODE_START  1
#define EXCEPTION_ERROR_CODE_START_OFFSET 100  /*!< 100 exception subtypes for each type of exception */
#define EXCEPTION_ERROR_CODE_END_OFFSET 99 /*!< Defines the upper bound offset for each exception type */


// LAYOUTException constants
#define LAYOUTEXCEPTION_INVALID_PARAMETER_MESSAGE QObject::tr("Invalid %1 parameter '%2'.")
#define LAYOUTEXCEPTION_EMPTY_CONTAINER_FOUND_MESSAGE QObject::tr("empty container found '%1'")
#define LAYOUTEXCEPTION_INVALID_TYPE_MESSAGE QObject::tr("Invalid type specified '%1'.")
#define LAYOUTEXCEPTION_DUPLICATE_ID_FOUND_MESSAGE QObject::tr("Duplicate id '%1' found for graph entity'%2'.")
#define LAYOUTEXCEPTION_INVALID_ATTRIBUTE_VALUE_MESAAGE QObject::tr("Invalid '%1' attribute for graph entity '%2'.")
#define LAYOUTEXCEPTION_MANDATORY_ATTRIBUTE_MISSING_MESSAGE QObject::tr("Missing mandatory attribute for entity '%1'.")
#define LAYOUTEXCEPTION_REQUIRED_PARAMETER_NOT_SET_MESSAGE QObject::tr("Required parameter not set '%1'")
#define LAYOUTEXCEPTION_NOT_FOUND_IN_CONTAINER_MESSAGE QObject::tr("'%1' not found in container '%2'")
#define LAYOUTEXCEPTION_INVALID_VALUE_MESSAGE QObject::tr("Invalid parameter '%1' value provided for '%2'.")
#define LAYOUTEXCEPTION_INVALID_OPERATION_MESSAGE QObject::tr("Invalid operation '%1' in '%2'.")
#define LAYOUTEXCEPTION_INCONSISTENT_DATASTRUCTURE_MESSAGE QObject::tr("Inconsistent datastructure '%1' '%2'.")
#define LAYOUTEXCEPTION_INVALID_GRAPH_TYPE_MESSAGE QObject::tr("Graph '%1' is '%2'.")
#define LAYOUTEXCEPTION_UNKNOWN_EXCEPTION_MESSAGE QObject::tr("Unknown exception FROM LayoutException.")

// FileIOException Message Constants
#define FILEIOEXCEPTION_DIRECTORY_NOT_FOUND_EXCEPTION_MESSAGE QObject::tr("Could not find a directory with name '%1'.")
#define FILEIOEXCEPTION_FILENOTFOUND_EXCEPTION_MESSAGE QObject::tr("Could not find a file with name '%1'.")
#define FILEIOEXCEPTION_UNSUPPORTEDFILETYPE_EXCEPTION_MESSAGE QObject::tr("Unsupported file '%1'.")
#define FILEIOEXCEPTION_UNSUPPORTEDFILEFORMAT_EXCEPTION_MESSAGE QObject::tr("Unsupported format for file '%1'.")
#define FILEIOEXCEPTION_UNKNOWN_EXCEPTION_MESSAGE QObject::tr("Unknown file IO exception.")
#define FILEIOEXCEPTION_FILEREADOPERATIONERROR_ERRROR_MESSAGE QObject::tr("Could not open or read the file %1.")
#define FILEIOEXCEPTION_INVALIDFILENAME_MESSAGE QObject::tr("Invalid filename specified '%1'.")
#define FILEIOEXCEPTION_INVALIDFILEFORMAT_EXCEPTION_MESSAGE QObject::tr("Invalid file format '%1'.")
#define FILEIOEXCEPTION_INVALIDFILEOPERATION_EXCEPTION_MESSAGE QObject::tr("Not a %1 file: '%2'.")

// MemoryException Message Constants
#define MEMORYEXCEPTION_NULL_POINTER_EXCEPTION_MESSAGE QObject::tr("Null pointer exception occurred for '%1'.")
#define MEMORYEXCEPTION_UNKNOWNEXCEPTION_MESSAGE QObject::tr("Unknown memory exception occurred.")


#endif // LAYOUTEXCEPTIONCONSTANTS_H
