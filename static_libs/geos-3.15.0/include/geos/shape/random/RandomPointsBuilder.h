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

#pragma once

#include <geos/export.h>

#include <geos/algorithm/locate/PointOnGeometryLocator.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>
#include <geos/shape/GeometricShapeBuilder.h>

#include <memory>
#include <random>

namespace geos::shape::random {

 /**
  * Creates random point sets contained in a
  * region defined by either a rectangular or a polygonal extent.
  *
  * @author mbdavis
  *
  */
class GEOS_DLL RandomPointsBuilder : public GeometricShapeBuilder {
public:

    explicit RandomPointsBuilder(const geom::GeometryFactory* gf);

    using GeometricShapeBuilder::setExtent;

    void setExtent(const geom::Geometry& mask);

    std::unique_ptr<geom::Geometry> getGeometry() override;

protected:

    bool isInExtent(const geom::CoordinateXY& p) const;

    geom::CoordinateXY createRandomCoord(const geom::Envelope& env);

    std::unique_ptr<geom::Geometry> maskPoly;
private:
    std::unique_ptr<algorithm::locate::PointOnGeometryLocator> extentLocator;
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist{0, 1};
};

}
