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

#include <geos/geom/util/SurfaceExtracter.h>

#include <geos/geom/Surface.h>

#include <vector>

namespace geos::geom::util {

void
SurfaceExtracter::getSurfaces(const Geometry& geom, std::vector<const Surface*>& ret)
{
    if (!geom.hasDimension(Dimension::A)) {
        return;
    }

    SurfaceExtracter pe(ret);
    geom.apply_ro(&pe);
}

SurfaceExtracter::SurfaceExtracter(std::vector<const Surface*>& newComps)
    :
    comps(newComps)
{}

void
SurfaceExtracter::filter_rw(Geometry* geom)
{
    if(const Surface* p = dynamic_cast<const Surface*>(geom)) {
        comps.push_back(p);
    }
}

void
SurfaceExtracter::filter_ro(const Geometry* geom)
{
    if(const Surface* p = dynamic_cast<const Surface*>(geom)) {
        comps.push_back(p);
    }
}
}
