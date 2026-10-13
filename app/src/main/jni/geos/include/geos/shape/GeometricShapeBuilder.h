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

#pragma once

#include <geos/export.h>

#include <geos/geom/Coordinate.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineSegment.h>

#include <cstddef>
#include <memory>

namespace geos {
namespace shape {

class GEOS_DLL GeometricShapeBuilder {
public:
    GeometricShapeBuilder();
    explicit GeometricShapeBuilder(const geom::GeometryFactory* gf);

    virtual ~GeometricShapeBuilder() = default;

    /**
     * Sets the extent as an envelope.
     *
     * @param env the extent envelope
     */
    void setExtent(const geom::Envelope& env);

    /**
     * Gets the extent envelope.
     *
     * @return the extent envelope
     */
    const geom::Envelope& getExtent() const;

    geom::CoordinateXY getCentre() const;

    double getDiameter() const;

    double getRadius() const;

    geom::LineSegment getSquareBaseLine() const;

    geom::Envelope getSquareExtent() const;

    /**
     * Sets the total number of points in the created {@link Geometry}.
     * The created geometry will have no more than this number of points,
     * unless more are needed to create a valid geometry.
     */
    void setNumPoints(std::size_t n);

    virtual std::unique_ptr<geom::Geometry> getGeometry() = 0;

protected:
    geom::CoordinateXY createCoord(double x, double y) const;

    const geom::GeometryFactory* geometryFactory;
    geom::Envelope extent;
    std::size_t numPts;
};

} // namespace shape
} // namespace geos
