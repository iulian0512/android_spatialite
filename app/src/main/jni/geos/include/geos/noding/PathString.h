/**********************************************************************
*
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2025 ISciences, LLC
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/export.h>

#include <cstddef>
#include <memory>
#include <vector>

namespace geos::geom {
    class CoordinateSequence;
}

namespace geos::noding {

/// A PathString represents a contiguous line/arc to be used as an input or output
/// of a noding process.
class GEOS_DLL PathString {
public:
    explicit PathString(const void* p_context = nullptr) : context(p_context) {}

    virtual ~PathString() = default;

    virtual std::size_t getSize() const = 0;

    virtual double getLength() const = 0;

    /** \brief
     * Gets the user-defined data for this segment string.
     *
     * @return the user-defined data
     */
    const void*
    getData() const
    {
        return context;
    }

    /** \brief
     * Sets the user-defined data for this segment string.
     *
     * @param data an Object containing user-defined data
     */
    void
    setData(const void* data)
    {
        context = data;
    }

    /// \brief
    /// Return a pointer to the CoordinateSequence associated
    /// with this PathString.
    virtual const std::shared_ptr<const geom::CoordinateSequence>& getCoordinates() const = 0;

    std::vector<PathString*>
    static toRawPointerVector(const std::vector<std::unique_ptr<PathString>> & segStrings);

private:

    const void* context;
};

}
