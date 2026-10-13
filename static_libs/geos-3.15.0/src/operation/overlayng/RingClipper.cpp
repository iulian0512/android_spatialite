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

#include <geos/operation/overlayng/RingClipper.h>

using geos::geom::CoordinateSequence;
using geos::geom::CoordinateXYZM;

namespace geos {      // geos
namespace operation { // geos.operation
namespace overlayng { // geos.operation.overlayng


/*public*/
std::unique_ptr<CoordinateSequence>
RingClipper::clip(const CoordinateSequence* cs) const
{
    std::unique_ptr<CoordinateSequence> pts;
    for (int edgeIndex = 0; edgeIndex < 4; edgeIndex++) {
        bool closeRing = (edgeIndex == 3);
        pts = clipToBoxEdge(cs, edgeIndex, closeRing);
        if (pts->size() == 0)
            return pts;
        cs = pts.get();
    }
    return pts;
}

/*private*/
std::unique_ptr<CoordinateSequence>
RingClipper::clipToBoxEdge(const CoordinateSequence* pts, int edgeIndex, bool closeRing) const
{
    // TODO: is it possible to avoid copying array 4 times?
    auto ptsClip = std::make_unique<CoordinateSequence>(0, pts->hasZ(), pts->hasM());

    CoordinateXYZM p0;
    pts->getAt(pts->size() - 1, p0);
    for (std::size_t i = 0; i < pts->size(); i++) {
        CoordinateXYZM p1;
        pts->getAt(i, p1);
        if (isInsideEdge(p1, edgeIndex)) {
            if (!isInsideEdge(p0, edgeIndex)) {
                CoordinateXY intPt;
                intersection(p0, p1, edgeIndex, intPt);
                ptsClip->add(intPt, false);
            }
            // TODO: avoid copying so much?
            ptsClip->add(p1, false);

        }
        else if (isInsideEdge(p0, edgeIndex)) {
            CoordinateXY intPt;
            intersection(p0, p1, edgeIndex, intPt);
            ptsClip->add(intPt, false);
        }

        // else p0-p1 is outside box, so it is dropped
        p0 = p1;
    }

    // add closing point if required
    if (closeRing) {
        ptsClip->closeRing();
    }

    return ptsClip;
}

/*private*/
void
RingClipper::intersection(const CoordinateXY& a, const CoordinateXY& b, int edgeIndex, CoordinateXY& rsltPt) const
{
    switch (edgeIndex) {
    case BOX_BOTTOM:
        rsltPt = CoordinateXY(intersectionLineY(a, b, clipEnv.getMinY()), clipEnv.getMinY());
        break;
    case BOX_RIGHT:
        rsltPt = CoordinateXY(clipEnv.getMaxX(), intersectionLineX(a, b, clipEnv.getMaxX()));
        break;
    case BOX_TOP:
        rsltPt = CoordinateXY(intersectionLineY(a, b, clipEnv.getMaxY()), clipEnv.getMaxY());
        break;
    case BOX_LEFT:
    default:
        rsltPt = Coordinate(clipEnv.getMinX(), intersectionLineX(a, b, clipEnv.getMinX()));
    }
    return;
}

/*private*/
double
RingClipper::intersectionLineY(const CoordinateXY& a, const CoordinateXY& b, double y)
{
    double m = (b.x - a.x) / (b.y - a.y);
    double intercept = (y - a.y) * m;
    return a.x + intercept;
}

/*private*/
double
RingClipper::intersectionLineX(const CoordinateXY& a, const CoordinateXY& b, double x)
{
    double m = (b.y - a.y) / (b.x - a.x);
    double intercept = (x - a.x) * m;
    return a.y + intercept;
}

/*private*/
bool
RingClipper::isInsideEdge(const CoordinateXY& p, int edgeIndex) const
{
    if (clipEnv.isNull()) {
        return false;
    }

    bool isInside = false;
    switch (edgeIndex) {
    case BOX_BOTTOM: // bottom
        isInside = p.y > clipEnv.getMinY();
        break;
    case BOX_RIGHT: // right
        isInside = p.x < clipEnv.getMaxX();
        break;
    case BOX_TOP: // top
        isInside = p.y < clipEnv.getMaxY();
        break;
    case BOX_LEFT:
    default: // left
        isInside = p.x > clipEnv.getMinX();
    }
    return isInside;
}



} // namespace geos.operation.overlayng
} // namespace geos.operation
} // namespace geos
