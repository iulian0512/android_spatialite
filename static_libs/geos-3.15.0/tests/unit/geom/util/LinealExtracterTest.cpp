//
// Test Suite for geos::geom::util::LinealExtracter class.

#include <tut/tut.hpp>

#include <geos/geom/Geometry.h>
#include <geos/geom/LineString.h>
#include <geos/geom/util/LinealExtracter.h>

#include "utility.h"

namespace tut {

struct test_linealextracter_data {
    geos::io::WKTReader reader_;
};

typedef test_group<test_linealextracter_data> group;
typedef group::object object;

group test_linealextracter_group("geos::geom::util::LinealExtracter");

template<>
template<>
void object::test<1>()
{
    auto input = reader_.read(
        "GEOMETRYCOLLECTION ("
        "POINT (1 1),"
        "LINESTRING (0 0, 1 1),"
        "POLYGON ((0 0, 1 0, 1 1, 0 0)),"
        "CIRCULARSTRING (0 0, 1 1, 2 0),"
        "COMPOUNDCURVE(CIRCULARSTRING (0 0, 5 5, 10 0), (10 0, 20 0))"
        ")");

    std::vector<const Geometry*> lineals;
    geos::geom::util::LinealExtracter::getLineals(*input, lineals);

    ensure_equals(lineals.size(), 3u);
    ensure_equals_geometry(lineals[0], input->getGeometryN(1));
    ensure_equals_geometry(lineals[1], input->getGeometryN(3));
    ensure_equals_geometry(lineals[2], input->getGeometryN(4));
}

template<>
template<>
void object::test<2>()
{
    set_test_name("nested inputs");

    auto input = reader_.read(
        "GEOMETRYCOLLECTION ("
        "MULTILINESTRING ((0 0, 1 1), (2 2, 3 3)),"
        "GEOMETRYCOLLECTION ("
        "LINESTRING (4 4, 5 5),"
        "POINT (6 6),"
        "MULTILINESTRING ((7 7, 8 8))"
        ")"
        ")");

    std::vector<const Geometry*> lineals;
    geos::geom::util::LinealExtracter::getLineals(*input, lineals);

    ensure_equals(lineals.size(), 3u);
    ensure_equals_geometry(lineals[0], input->getGeometryN(0));
    ensure_equals_geometry(lineals[1], input->getGeometryN(1)->getGeometryN(0));
    ensure_equals_geometry(lineals[2], input->getGeometryN(1)->getGeometryN(2));
}

} // namespace tut
