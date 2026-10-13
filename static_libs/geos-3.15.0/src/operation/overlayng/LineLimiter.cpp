/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2020 Paul Ramsey <pramsey@cleverelephant.ca>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/operation/overlayng/LineLimiter.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/Coordinate.h>
#include <geos/util.h>

#include <algorithm>

using geos::geom::CoordinateSequence;

namespace geos {      // geos
namespace operation { // geos.operation
namespace overlayng { // geos.operation.overlayng

/*public*/
std::vector<std::unique_ptr<CoordinateSequence>>&
LineLimiter::limit(const CoordinateSequence *pts)
{
    // Reset for new limit run
    lastOutside = nullptr;
    ptList.reset(nullptr);
    sections.clear();
    hasZ = pts->hasZ();
    hasM = pts->hasM();

    pts->forEach([this](const auto& p) {
        if (limitEnv->intersects(p)) {
            addPoint(p);
        }
        else {
            addOutside(p);
        }
    });

    // finish last section, if any
    finishSection();
    return sections;
}

/*private*/
template<typename CoordType>
void
LineLimiter::addPoint(const CoordType& p)
{
    startSection<CoordType>();
    ptList->add(p, false);
}

/*private*/
template<typename CoordType>
void
LineLimiter::addOutside(const CoordType& p)
{
    bool segIntersects = isLastSegmentIntersecting(p);
    if (!segIntersects) {
        finishSection();
    }
    else {
        if(lastOutside != nullptr) {
            addPoint(*static_cast<const CoordType*>(lastOutside));
        }
        addPoint(p);
    }
    lastOutside = &p;
}

/*private*/
bool
LineLimiter::isLastSegmentIntersecting(const CoordinateXY& p) const
{
    if (lastOutside == nullptr) {
        // last point must have been inside
        if (isSectionOpen())
            return true;
        return false;
    }
    return limitEnv->intersects(*lastOutside, p);
}

/*private*/
bool
LineLimiter::isSectionOpen() const
{
    return ptList != nullptr;
}

/*private*/
template<typename CoordType>
void
LineLimiter::startSection()
{
    if (!isSectionOpen()) {
        ptList = std::make_unique<CoordinateSequence>(0, hasZ, hasM);
    }

    if (lastOutside != nullptr) {
        ptList->add(*static_cast<const CoordType*>(lastOutside), false);
    }
    lastOutside = nullptr;
}

/*private*/
void
LineLimiter::finishSection()
{
    if (!isSectionOpen())
        return;

    // finish off this section
    if (lastOutside != nullptr) {
        ptList->add(*lastOutside, false);
        lastOutside = nullptr;
    }

    // remove repeated points from the section
    assert(!ptList->hasRepeatedPoints());
    //ptList->erase(std::unique(ptList->begin(), ptList->end()), ptList->end());

    sections.push_back(std::move(ptList));
    ptList.reset(nullptr);
}




} // namespace geos.operation.overlayng
} // namespace geos.operation
} // namespace geos
