#include "LayoutExceptionEnum.h"

LayoutExceptionEnum::LayoutExceptionEnum()
{
}


QString LayoutExceptionEnum::getStringForEnum(LayoutExceptionEnum::LayoutExceptionTypes enExceptionTypes)
{
    QString sExceptionTypeString;

    switch(enExceptionTypes)
    {
    case LAYOUTEXCEPTION:
        sExceptionTypeString = "LAYOUTEXCEPTION";
        break;
    case LAYOUT_FILEIOEXCEPTION:
        sExceptionTypeString = "FILEIOEXCEPTION";
        break;
    case LAYOUT_MEMORYEXCEPTION:
        sExceptionTypeString = "MEMORYEXCEPTION";
        break;
    default:
        sExceptionTypeString = "LAYOUTEXCEPTION";
        break;
    }

    return sExceptionTypeString;
}
