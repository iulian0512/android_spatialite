/**********************************************************************
*
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2016 Martin Davis
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: shape/GeometricShapeBuilder.java c2e8e1d069
 *
 **********************************************************************/

#include <geos/shape/GeometricShapeBuilder.h>

using geos::geom::Coordinate;
using geos::geom::CoordinateXY;
using geos::geom::Envelope;
using geos::geom::LineSegment;
using geos::geom::GeometryFactory;

namespace geos::shape {

GeometricShapeBuilder::GeometricShapeBuilder(const GeometryFactory *gf)
    : geometryFactory(gf)
{
}

void
GeometricShapeBuilder::setExtent(const Envelope &env)
{
    extent = env;
}

const Envelope&
GeometricShapeBuilder::getExtent() const
{
    return extent;
}

CoordinateXY
GeometricShapeBuilder::getCentre() const
{
    CoordinateXY centre;
    extent.centre(centre);
    return centre;
}

double
GeometricShapeBuilder::getDiameter() const
{
    return std::min(extent.getHeight(), extent.getWidth());
}

double
GeometricShapeBuilder::getRadius() const
{
    return getDiameter() / 2;
}

LineSegment
GeometricShapeBuilder::getSquareBaseLine() const
{
    const double radius = getRadius();
    const CoordinateXY centre = getCentre();

    const Coordinate p0(centre.x - radius, centre.y - radius);
    const Coordinate p1(centre.x + radius, centre.y - radius);

    return LineSegment(p0, p1);
}

Envelope
GeometricShapeBuilder::getSquareExtent() const
{
    const double radius = getRadius();
    const CoordinateXY centre = getCentre();

    return Envelope(centre.x - radius, centre.x + radius, centre.y - radius, centre.y + radius);
}

void
GeometricShapeBuilder::setNumPoints(std::size_t n)
{
    numPts = n;
}

CoordinateXY
GeometricShapeBuilder::createCoord(double x, double y) const
{
    CoordinateXY pt(x, y);
    geometryFactory->getPrecisionModel()->makePrecise(pt);
    return pt;
}

}
