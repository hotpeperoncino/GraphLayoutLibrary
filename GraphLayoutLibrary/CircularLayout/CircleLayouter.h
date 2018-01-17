#ifndef CIRCLELAYOUTER_H
#define CIRCLELAYOUTER_H

#include <GraphLayoutLibrary_global.h>
#include <boost/config/no_tr1/cmath.hpp>
#include <boost/math/constants/constants.hpp>
#include <Common/BoostGraphWrapper.h>
#include <Common/GraphCycleHandler.h>
#include <LayoutUtilities/CommonLayoutConstants.h>
#include <CircularLayout/SizeManager.h>
#include <QDebug>
#include <Common/ConstantType.h>

#ifndef BOOST_NO_STDC_NAMESPACE
using std::sin;
using std::cos;
#endif // BOOST_NO_STDC_NAMESPACE

/**
 * @brief The CircleLayouter class
 *
 * The class provides and maintains circular layout of a graph.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT CircleLayouter
{

private:    
    /**
     * Object of BoostGraphWrapper which is wrapper on BOOST SubGraph object
     */
    BoostGraphWrapper m_boostGraphWrapper;
    /**
     * Default value for height, if not mentioned in the input parameters
     */
    int m_iDefaultHeight;
    /**
     * Default value for width, if not mentioned in the input parameters
     */
    int m_iDefaultWidth;

public:

/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type CircleLayouter.
  */
 //@{
    /**
      Constructs object of type CircleLayouter.
     @pre
        none

     @throw
        none
    */
    CircleLayouter();
 //@}

 /** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of CircleLayouter.
  */
 //@{
    /**
     This function aaplies the Circle layout to the specified Subgraph 'gGraph'. It also takes the specified radius, origin and vertex order into consideration while applying layout.

    @pre
    -# gGraph != NULL
    -# radius > 0

    @param gGraph
        reference to input graph.
    @param position
        stores boost point coordinate position for each vertex
    @param radius
        contains value for radius of a circle

    @return none

    @throw MemoryException
    -# NULL_POINTER_EXCEPTION if referenced entity is null

    @throw LayoutException
    -# INVALID_PARAMETER if invalid value is passed
    -# EMPTY_CONTAINER if empty value is passed

    @throw BoostException
    -# if exception caused by bost library
    */
    template<typename Subgraph, typename PositionMap, typename Radius>
    void applyCircleGraphLayout(Subgraph &gGraph, PositionMap positionMap, Radius radius,
                                int iCenterCoordX, int iCenterCoordY, MapOrderVertex& mapOrderVertex)
    {
        /*To implement vertex ordering We are using topological sort of Boost
          and for topological sort we need DAG hence we will process through graph
          detect the backedges and reversing those backedges if any present in the graph
            */

        /*
            CENTER SHOULD NOT BE GARBAGE
            RADIUS SHOULD BE GREATER THAN 0
            ANGLE SPACE VALUE SHOULD BE BETWEEN O TO (2 * PI)
        */
        LAYOUT_ASSERT(&gGraph != NULL, LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, NULL_GRAPH_FOUND));
        LAYOUT_ASSERT(radius > 0, LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_PARAMETER, INVALID_RADIUS_VALUE, ""));
        LAYOUT_ASSERT(!mapOrderVertex.empty(), LayoutException(__FUNCTION__, LayoutExceptionEnum::EMPTY_CONTAINER, EMPTY_MAP_FOUND, " "));

        // For every vertex, Calculate x and y coordinates
        size_t iOrderValue = 1;

        IteratorMapOrderVertex mapIterOrderVertex(mapOrderVertex);

        double dAngleSpace = 0;
        SizeManager sizeManager;

        double dPrevNodeSpace = 0;

        // Iterate vertices in ordered mannar
        while(mapIterOrderVertex.hasNext())
        {
            mapIterOrderVertex.next();
            VertexDescriptor vVertex = mapIterOrderVertex.value();

            // Calculate angle space for this node
            double dAngleSpacePresentNode;
            try
            {
                dAngleSpacePresentNode = calculateAngleSpace(vVertex, gGraph);
            }
            catch(LayoutMemoryException& eException)
            {
                throw LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, eException.getObjectName());
            }
            catch(LayoutException&)
            {
                QString sVertexId = m_boostGraphWrapper.getVertexId(vVertex, gGraph);
                qDebug()<<"\nDefault Angle space caluclated for vertex " << sVertexId;
                int iTotalVertices = num_vertices(gGraph);
                dAngleSpacePresentNode = ((2 * PI) / iTotalVertices);
            }
            catch(boost::exception& eBoostException)
            {
                throw *boost::get_error_info<errmsg_info>(eBoostException);
            }
            catch(...)
            {
                QString sVertexId = m_boostGraphWrapper.getVertexId(vVertex, gGraph);
                qDebug()<<"\nDefault Angle space caluclated for vertex "<<sVertexId;
                int iTotalVertices = num_vertices(gGraph);
                dAngleSpacePresentNode = ((2 * PI) / iTotalVertices);
            }

            dAngleSpace += (dPrevNodeSpace / 2.0);
            if(dPrevNodeSpace != 0) // To consider first node's anglespace
            {
                dAngleSpace += (dAngleSpacePresentNode / 2.0);
            }

            dPrevNodeSpace = dAngleSpacePresentNode;

            /*
             * Add another order property to vertex to be used by space utilizer
               for getting list of vertices between min vertex and max vertex
            */

            // store positions of laid out vertices
            positionMap[vVertex][0] = iCenterCoordX + radius * cos(dAngleSpace);
            positionMap[vVertex][1] = iCenterCoordY + radius * sin(dAngleSpace);

            // Setting the shifted coordinates in the global graph
            m_boostGraphWrapper.setVertexCenterCoordX(vVertex, gGraph, positionMap[vVertex][0]);
            m_boostGraphWrapper.setVertexCenterCoordY(vVertex, gGraph, positionMap[vVertex][1]);

            // calculate leftx and topy coordinates of the vertex
            int iLeftCoordX = sizeManager.calculateNodeLeftXFromCenterX(vVertex, gGraph);
            // set leftx in the vertex property
            m_boostGraphWrapper.setVertexLeftCoordX(vVertex, gGraph, iLeftCoordX);

            // calculate topy coordinate of the vertex
            int iTopCoordY = sizeManager.calculateNodeTopYFromCenterY(vVertex, gGraph);

            // set topy in the vertex property
            m_boostGraphWrapper.setVertexLeftCoordY(vVertex, gGraph, iTopCoordY);

            iOrderValue++;
        }
    }

    /**
     This functions puts the vertices of graph on circumference of a arc in ordered manner considering the start and end angle.

    @pre
    -# gGraph != NULL
    -# radius >= 0

    @param gGraph
        reference to input graph
    @param startAngle
        value for staring angle for arc in degrees
    @param endAngle
        value for ending angle for arc in degrees
    @param position
        stores boost point coordinate positions
    @param radius
        contains value for radius of a circle

    @return none

    @throw none
    */
    template<typename PositionMap>
    void applyArcLayout(SubGraph &gGraph, double dStartAngle, double dEndAngle, PositionMap position, double dRadius, MapOrderVertex& mapOrderVertex)
    {
        // For every vertex, Calculate x and y coordinates
        // VertexSizeType vertexCount = num_vertices(gGraph);
        VertexSizeType vertexCount = mapOrderVertex.size();

        VertexSizeType indexCount = 0;
        double dDegreeToRadian = 0.0174532925;
        double dRadianToDegree = 57.32484076433121;
        double dAngleSpace;

        if(dStartAngle > dEndAngle)
        {
            dAngleSpace = ((((360 - dStartAngle) + dEndAngle) * dDegreeToRadian) / vertexCount);
        }
        else
        {
            dAngleSpace = ((abs(dEndAngle - dStartAngle) * dDegreeToRadian) / vertexCount);
        }

        IteratorMapOrderVertex mapIter(mapOrderVertex);

        dStartAngle = (dStartAngle + ((dAngleSpace) / 2));

        // Iterate vertices in ordered mannar
        while(mapIter.hasNext())
        {
            mapIter.next();
            std::size_t key = mapIter.key();
            VertexDescriptor vVertex = mapOrderVertex[key];
            double dCosValue = cos(dStartAngle + (indexCount * dAngleSpace));
            double dSInValue = sin(dStartAngle + (indexCount * dAngleSpace));
            position[vVertex][0] = dRadius * dCosValue;
            position[vVertex][1] = dRadius * dSInValue;
            indexCount++;
        }
    }

/** @name Querries
  *  The methods under this section are responsible for accessing
  *  an instance of CircleLayouter.
  */
 //@{
    /**
    This function calculates circumference of cluster.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @param vVertex
        vertex descriptor

    @return circumference of cluster

    @throw MemoryException
    -# NULL_POINTER_EXCEPTION if referenced entity is null

    @throw BoostException
    -# if exception caused by bost library
    */
    double calculateCircumference(SubGraph& gSubgraph);


    /**
    This function calculates angle space required for a node.

    @pre
    -# gSubgraph != NULL

    @param gSubgraph
        reference to graph

    @param vVertex
        vertex descriptor

    @return angle space for onr node

    @throw MemoryException
    -# NULL_POINTER_EXCEPTION if referenced entity is null

    @throw BoostException
    -# if exception caused by bost library
    */
    double calculateAngleSpace(VertexDescriptor &vVertex, SubGraph& gSubgraph);
	//@}
};

#endif // CIRCLELAYOUTER_H
