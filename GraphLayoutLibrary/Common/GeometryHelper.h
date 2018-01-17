#ifndef GEOMETRYHELPER_H
#define GEOMETRYHELPER_H
#include <Common/BoostGraphWrapper.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

/**
 * @brief The GeometryHelper class
 *
 * The class provides helpers for geometry of graphs in graph layouts.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT GeometryHelper
{
public:
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type GeometryHelper.
  */
 //@{
    GeometryHelper();
//@}
/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type GeometryHelper.
 */
//@{
    static bool isIntersectingEdges(EdgeDescriptor& firstEdge, EdgeDescriptor& secondEdge, SubGraph& subGraph);
//@}

private:
    // for edge intersection
    typedef boost::geometry::model::d2::point_xy<int> GeometryPoint;


};

#endif // GEOMETRYHELPER_H
