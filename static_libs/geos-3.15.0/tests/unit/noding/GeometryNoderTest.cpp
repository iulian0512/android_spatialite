#include <tut/tut.hpp>

#include <geos/geom/Geometry.h>
#include <geos/noding/GeometryNoder.h>

#include "utility.h"

using geos::geom::Geometry;
using geos::noding::GeometryNoder;

namespace tut {

struct test_geometrynoder_data {
    geos::io::WKTReader reader_;
};

typedef test_group<test_geometrynoder_data> group;
typedef group::object object;

group test_geometrynoder_group("geos::noding::GeometryNoder");

template<>
template<>
void object::test<1>()
{
    set_test_name("single input") ;

    auto input = reader_.read("MULTILINESTRING ((0 0, 10 10, 10 0, 0 10))");

    auto result = GeometryNoder::node(*input);
    ensure(result != nullptr);

    auto expected = reader_.read("MULTILINESTRING ((0 0, 5 5), (5 5, 10 10, 10 0, 5 5), (5 5, 0 10))");

    ensure_equals_exact_geometry_xyzm( result.get(), expected.get(), 0);
}

template<>
template<>
void object::test<2>()
{
    set_test_name("two inputs, output all edges") ;

    auto input1 = reader_.read("LINESTRING (0 0, 10 10)");
    auto input2 = reader_.read("LINESTRING (0 10, 10 0)");

    auto result = GeometryNoder::node(*input1, *input2);
    ensure(result != nullptr);

    auto expected = reader_.read("MULTILINESTRING ((0 0, 5 5), (5 5, 10 10), (0 10, 5 5), (5 5, 10 0))");

    ensure_equals_exact_geometry_xyzm(result.get(), expected.get(), 0);
}

template<>
template<>
void object::test<3>()
{
    set_test_name("two inputs, only output edges from first") ;

    auto input1 = reader_.read("LINESTRING (0 0, 10 10)");
    auto input2 = reader_.read("LINESTRING (0 10, 10 0)");

    GeometryNoder noder(*input1, *input2);
    noder.setOnlyFirstGeomEdges(true);

    auto result = noder.getNoded();
    ensure(result != nullptr);

    auto expected = reader_.read("MULTILINESTRING ((0 0, 5 5), (5 5, 10 10))");

    ensure_equals_exact_geometry_xyzm(result.get(), expected.get(), 0);
}

} // namespace tut
