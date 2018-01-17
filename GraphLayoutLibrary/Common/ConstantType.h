#ifndef CONSTANTVARIABLE_H
#define CONSTANTVARIABLE_H

#include <boost/exception/get_error_info.hpp>
#include <LayoutUtilities/CommonLayoutConstants.h>
#include <LayoutException/LayoutExceptionConstants.h>
#include <LayoutException/LayoutMemoryException.h>
#include <LayoutException/LayoutFileIOException.h>
#include <QObject>

#define PI boost::math::constants::pi<double>()

/**
 * @brief The ConstantType class
 *
 * The class provides constants and operators on these constants used in graph layouts.
 */
template <typename VariableType>
class ConstantType
{
public:
    ConstantType<VariableType>() : m_bAssigned(false) {}

    ConstantType& operator=(VariableType value)
    {
        //Q_ASSERT_X(!m_bAssigned, "Generic Constant Variable" , "Modifying constant variable");
        LAYOUT_ASSERT(m_bAssigned == false , LayoutException(__FUNCTION__ , LayoutExceptionEnum::INVALID_OPERATION
                                                             , "Modifying constant variable after first assignment"
                                                             , "Generic Constant Variable"));
        if(m_bAssigned == false)
        {
            m_Value = value;
            m_bAssigned = true;
        }
        return *this;
    }
    operator VariableType() const
    {
        //Q_ASSERT_X(m_bAssigned , "Generic Constant Variable" , "Value not set");
        LAYOUT_ASSERT(m_bAssigned == true , LayoutException(__FUNCTION__ , LayoutExceptionEnum::REQUIRED_PARAMETER_NOT_SET
                                                            , "Generic Constant Variable"
                                                           , ""));

        return m_Value;
    }

    bool isSet()
    {
        return m_bAssigned;
    }

    void reset()
    {
        m_bAssigned = false;
    }

private:
    VariableType m_Value;
    bool m_bAssigned;
};

#endif // CONSTANTVARIABLE_H
