#include "GeometryHelper.h"


GeometryHelper::GeometryHelper()
{
}

bool GeometryHelper::isIntersectingEdges(EdgeDescriptor& firstEdge, EdgeDescriptor& secondEdge, SubGraph& gSubgraph)
{
    BoostGraphWrapper boostGraphWrapper;
    boost::geometry::model::linestring<GeometryPoint> firstLine, secondLine;
    //Get x,y coordinates of source and target for firstEdge
    int iFirstEdgeSourceX = boostGraphWrapper.getEdgeSourceX(firstEdge, gSubgraph);
    int iFirstEdgeSourceY = boostGraphWrapper.getEdgeSourceY(firstEdge, gSubgraph);
    int iFirstEdgeTargetX = boostGraphWrapper.getEdgeTargetX(firstEdge, gSubgraph);
    int iFirstEdgeTargetY = boostGraphWrapper.getEdgeTargetY(firstEdge, gSubgraph);

    //Get x,y coordinates of source and target for secondEdge
    int iSecondEdgeSourceX = boostGraphWrapper.getEdgeSourceX(secondEdge, gSubgraph);
    int iSecondEdgeSourceY = boostGraphWrapper.getEdgeSourceY(secondEdge, gSubgraph);
    int iSecondEdgeTargetX = boostGraphWrapper.getEdgeTargetX(secondEdge, gSubgraph);
    int iSecondEdgeTargetY = boostGraphWrapper.getEdgeTargetY(secondEdge, gSubgraph);

    //Convert coordinates into string to pass it to geometry linestring
    QString sFirstEdgeSourceX = QString::number(iFirstEdgeSourceX);
    QString sFirstEdgeSourceY = QString::number(iFirstEdgeSourceY);
    QString sFirstEdgeTargetX = QString::number(iFirstEdgeTargetX);
    QString sFirstEdgeTargetY = QString::number(iFirstEdgeTargetY);

    QString sFirstLine = sFirstEdgeSourceX + " " + sFirstEdgeSourceY +
            "," + sFirstEdgeTargetX + " " + sFirstEdgeTargetY;

    QString sSecondEdgeSourceX = QString::number(iSecondEdgeSourceX);
    QString sSecondEdgeSourceY = QString::number(iSecondEdgeSourceY);
    QString sSecondEdgeTargetX = QString::number(iSecondEdgeTargetX);
    QString sSecondEdgeTargetY = QString::number(iSecondEdgeTargetY);

    QString sSecondLine = sSecondEdgeSourceX + " " + sSecondEdgeSourceY +
            "," + sSecondEdgeTargetX + " " + sSecondEdgeTargetY;

    //Initialize geometry point and line
    boost::geometry::read_wkt("linestring(" + sFirstLine.toStdString() + ")",firstLine);
    boost::geometry::read_wkt("linestring(" + sSecondLine.toStdString() + ")",secondLine);
    // Check intersection of edges (Geometry)
    return boost::geometry::intersects(firstLine, secondLine);
}


