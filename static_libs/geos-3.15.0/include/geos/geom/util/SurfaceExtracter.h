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
#include <geos/geom/GeometryFilter.h>
#include <vector>

namespace geos::geom {
class Surface;
}

namespace geos::geom::util {

/**
 * Extracts all the 2-dimensional (Surface) components from a Geometry.
 */
class GEOS_DLL SurfaceExtracter: public GeometryFilter {

public:

    /**
     * Pushes the Surface components from a single geometry into
     * the provided vector.
     */
    static void getSurfaces(const Geometry& geom, std::vector<const Surface*>& ret);

    /**
     * Constructs a SurfaceExtracter with a list in which
     * to store Surfaces found.
     */
    SurfaceExtracter(std::vector<const Surface*>& newComps);

    void filter_rw(Geometry* geom) override;

    void filter_ro(const Geometry* geom) override;

private:

    std::vector<const Surface*>& comps;

    // Declare type as noncopyable
    SurfaceExtracter(const SurfaceExtracter& other) = delete;
    SurfaceExtracter& operator=(const SurfaceExtracter& rhs) = delete;
};

} // namespace geos.geom.util

