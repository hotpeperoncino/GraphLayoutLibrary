#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <iostream>

using namespace std;

/**
 * A structure to store the attributes of graph elements present in GraphML file.
 */
struct Attribute
{
	
    QString sName; /*!< Name of the attribute */
    QString sType; /*!< Type of the attribute */
    bool bHasDefaultValue; /*!< Flag to specify whether the attribute is associated with some default value */
    QString sDefaultValue; /*!< Default value for the attribute */

    /**
     * Assign empty values to the structure variable
     */
    Attribute()
    {
        sName ="";
        sType ="";
        bHasDefaultValue = false;
        sDefaultValue = DEFAULT_NONE;
    }

    /**
     * Assign name and type to the structure variable
     *
     * @param sAttributeName
     *     string containing the attribute name
     *
     * @param sAttributeType
     *     string containing the attribute type
     */
    Attribute(QString sAttributeName , QString sAttributeType)
    {
        sName = sAttributeName;
        sType = sAttributeType;
        bHasDefaultValue = false;
        sDefaultValue = DEFAULT_NONE;
    }

    /**
     * Assign supplied values to the structure variable
     *
     * @param sAttributeName
     *     string containing the attribute name
     *
     * @param sAttributeType
     *     string containing the attribute type
     *
     * @param hasDefaultValue
     *     bool value that describes if attribute has default value or not
     *
     * @param sAttributeDefaultValue
     *     string containing the default value for that attribute
     */
    Attribute(QString sAttributeName, QString sAttributeType,
              bool hasDefaultValue, QString sAttributeDefaultValue)
    {
        sName = sAttributeName;
        sType = sAttributeType;
        bHasDefaultValue = hasDefaultValue;
        sDefaultValue = sAttributeDefaultValue;
    }

    /**
     * Print the structure variable on standard output
     *
     * @pre none
     *
     * @param none
     *
     * @return none
     *
     * @throw none
     */
    void print()
    {
        cout << sName.toStdString() << " " << sType.toStdString()
             << "  " << bHasDefaultValue << " "
             << "  " << sDefaultValue.toStdString() << endl;
    }
};

/**
 * A user defined type for list of Attribute structures.
 */
typedef QList<Attribute> AttributeList;


#endif // ATTRIBUTE_H
