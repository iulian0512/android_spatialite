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
#include <geos/geom/Geometry.h>
#include <vector>

namespace geos {
namespace geom { // geos.geom
namespace util { // geos.geom.util

/**
 * \brief Extracts the lineal (LineString/LinearRing/CircularString/CompoundCurve/MultiLineString/MultiCurve)
 * elements from a Geometry.
 */
class GEOS_DLL LinealExtracter {

public:

    /**
     * Pushes the lineal elements from a geometry into the provided vector.
     * 
     * @param geom the geometry to extract from
     * @param lineals the vector to add the polygonal elements to
     */
    static void getLineals(const Geometry& geom, std::vector<const Geometry*>& lineals);

    static void getLineals(const Geometry* geom, std::vector<const Geometry*>& lineals);

    // Declare type as noncopyable
    LinealExtracter(const LinealExtracter& other) = delete;
    LinealExtracter& operator=(const LinealExtracter& rhs) = delete;
};

} // namespace geos.geom.util
} // namespace geos.geom
} // namespace geos

