/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2026 ISciences LLC
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/export.h>

#include <vector>
#include <geos/geom/GeometryComponentFilter.h>

namespace geos::geom {
class SimpleCurve;
}

namespace geos::geom::util {

/**
 * Extracts all SimpleCurve components from a Geometry
 */
class GEOS_DLL SimpleCurveExtracter: public GeometryComponentFilter {

public:

    static void getCurves(const Geometry& geom, std::vector<const SimpleCurve*>& ret);

    explicit SimpleCurveExtracter(std::vector<const SimpleCurve*>& newComps);

    void filter_ro(const Geometry* geom) override;

private:

    std::vector<const SimpleCurve*>& comps;

    // Declare type as noncopyable
    SimpleCurveExtracter(const SimpleCurveExtracter& other) = delete;
    SimpleCurveExtracter& operator=(const SimpleCurveExtracter& rhs) = delete;

};

} // namespace geos.geom.util

