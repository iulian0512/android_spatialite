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

#include <geos/geom/util/LinealExtracter.h>

#include <geos/geom/LineString.h>
#include <geos/geom/MultiCurve.h>
#include <geos/geom/MultiLineString.h>

#include <vector>


namespace geos {
namespace geom { // geos.geom
namespace util { // geos.geom.util

void
LinealExtracter::getLineals(const Geometry& geom, std::vector<const Geometry*>& lineals)
{
    getLineals(&geom, lineals);
}

void
LinealExtracter::getLineals(const Geometry* geom, std::vector<const Geometry*>& lineals)
{
   if (dynamic_cast<const Curve*>(geom) != nullptr
         || dynamic_cast<const MultiLineString*>(geom) != nullptr
         || dynamic_cast<const MultiCurve*>(geom) != nullptr) {
  		lineals.push_back(geom);
  	}
  	else if (dynamic_cast<const GeometryCollection*>(geom) != nullptr) {
  	  for (std::size_t i = 0; i < geom->getNumGeometries(); i++) {
  	    getLineals(geom->getGeometryN(i), lineals);
  	  }
  	}
}

}
}
}
