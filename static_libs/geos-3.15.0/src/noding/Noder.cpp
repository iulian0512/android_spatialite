/**********************************************************************
*
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2026 ISciences, LLC
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <filesystem>
#include <geos/noding/Noder.h>
#include <geos/noding/PathString.h>
#include <geos/noding/SegmentString.h>

namespace geos::noding {

void
Noder::computePathNodes(const std::vector<PathString*>& inputPaths)
{
    std::vector<SegmentString*> segStrings(inputPaths.size());
    for (std::size_t i = 0; i < inputPaths.size(); i++) {
        auto* path = inputPaths[i];

        if (auto* ss = dynamic_cast<SegmentString*>(path)) {
            segStrings[i] = ss;
        } else {
            throw util::UnsupportedOperationException("Noder does not support curved paths");
        }
    }

    computeNodes(segStrings);
}

std::vector<std::unique_ptr<PathString>>
Noder::getNodedPaths()
{
    auto nodedSS = getNodedSubstrings();
    std::vector<std::unique_ptr<PathString>> ret(nodedSS.size());
    for (std::size_t i = 0; i < nodedSS.size(); i++) {
        ret[i] = std::move(nodedSS[i]);
    }
    return ret;
}




}