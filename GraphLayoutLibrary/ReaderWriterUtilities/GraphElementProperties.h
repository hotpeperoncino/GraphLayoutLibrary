#ifndef GRAPHELEMENTPROPERTIES_H
#define GRAPHELEMENTPROPERTIES_H

#include <ReaderWriterUtilities/Attribute.h>
#include <QListIterator>
/**
 * @brief This class holds the list of graph element properties
 * for example 'node' is a graph element and it can have list of attributes
 */
struct GraphElementProperties
{
    AttributeList lstAttributeList;

    /**
     * Function to print attribute list of contained in GraphElementProperties
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
        QListIterator<Attribute> attributeIter (lstAttributeList);
        while (attributeIter.hasNext())
        {
            Attribute attribute = attributeIter.next();
            attribute.print();

        }
    }

    /**
     * Function to return a java style iterator for Attribute list contained in this class GraphElementProperties
     *
     * @pre none
     *
     * @param none
     *
     * @return QListIterator of Attribute List contained current class
     *
     * @throw none
     */
    QListIterator<Attribute> iterator()
    {
        QListIterator<Attribute> lstIter(lstAttributeList);
        return lstIter;
    }
};

#endif // GRAPHELEMENTPROPERTIES_H
