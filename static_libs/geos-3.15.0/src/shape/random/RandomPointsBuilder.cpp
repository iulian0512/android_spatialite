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
 * Last port: shape/random/RandomPointsBuilder.java e92970e3c0
 *
 **********************************************************************/

#include <geos/shape/random/RandomPointsBuilder.h>

#include <geos/algorithm/locate/IndexedPointInAreaLocator.h>
#include <geos/algorithm/locate/SimplePointInAreaLocator.h>
#include <geos/util/IllegalArgumentException.h>

#include <algorithm>
#include <random>

using geos::geom::Coordinate;
using geos::geom::CoordinateXY;
using geos::geom::Envelope;
using geos::geom::Geometry;
using geos::geom::GeometryFactory;
using geos::geom::Point;

namespace geos::shape::random {

RandomPointsBuilder::RandomPointsBuilder(const GeometryFactory* gf)
    : GeometricShapeBuilder(gf)
    , rd{}
    , rng{rd()}
{
}

void
RandomPointsBuilder::setExtent(const Geometry& mask)
{
    if (!mask.isDimensionStrict(geom::Dimension::A)) {
        throw geos::util::IllegalArgumentException(
            "RandomPointsBuilder: Only polygonal extents are supported");
    }

    maskPoly = mask.clone();
    GeometricShapeBuilder::setExtent(*mask.getEnvelopeInternal());

    if (maskPoly->hasCurvedComponents()) {
        extentLocator = std::make_unique<algorithm::locate::SimplePointInAreaLocator>(maskPoly.get());
    } else {
        extentLocator = std::make_unique<algorithm::locate::IndexedPointInAreaLocator>(*maskPoly);
    }
}

std::unique_ptr<Geometry>
RandomPointsBuilder::getGeometry()
{
    std::vector<std::unique_ptr<Point>> pts(numPts);
    std::size_t i = 0;
    while (i < numPts) {
        const CoordinateXY p = createRandomCoord(getExtent());
        if (extentLocator && !isInExtent(p)) {
            continue;
        }
        pts[i++] = geometryFactory->createPoint(p);
    }

    return geometryFactory->createMultiPoint(std::move(pts));
}

bool
RandomPointsBuilder::isInExtent(const CoordinateXY &p) const
{
    if (extentLocator) {
        return extentLocator->locate(&p) != geom::Location::EXTERIOR;
    }
    return getExtent().contains(p);
}

CoordinateXY
RandomPointsBuilder::createRandomCoord(const Envelope& env)
{
    const double x = env.getMinX() + env.getWidth() * dist(rng);
    const double y = env.getMinY() + env.getHeight() * dist(rng);

    return createCoord(x, y);
}

}
