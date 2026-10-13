// tut
#include <tut/tut.hpp>
// geos
#include <geos/shape/random/RandomPointsBuilder.h>
#include <geos/geom/Coordinate.h>
#include <geos/io/WKTReader.h>
// std

using geos::shape::random::RandomPointsBuilder;
using geos::geom::Coordinate;
using geos::geom::Envelope;
using geos::geom::GeometryFactory;
using geos::io::WKTReader;

namespace tut {

// Common data used by tests
struct test_randompointsbuilder_data {
    GeometryFactory::Ptr factory_;
    WKTReader reader_;

    test_randompointsbuilder_data() :
        factory_(GeometryFactory::create()),
        reader_(factory_.get())
    {}

    void
    checkPointsInGeometry(const std::string& wkt, std::size_t nPts) const
    {
        auto g = reader_.read(wkt);

        RandomPointsBuilder rpb(factory_.get());
        rpb.setNumPoints(nPts);
        rpb.setExtent(*g);

        auto mp = rpb.getGeometry();

        ensure_equals(mp->getGeometryTypeId(), geos::geom::GEOS_MULTIPOINT);
        ensure_equals(mp->getNumGeometries(), nPts);

        for (std::size_t i = 0; i < mp->getNumGeometries(); i++) {
            const auto* pt = mp->getGeometryN(i);
            ensure(g->intersects(pt));
        }
    }
};

typedef test_group<test_randompointsbuilder_data> group;
typedef group::object object;

group test_randompointsbuilder_group("geos::shape::random::RandomPointsBuilder");

template<>
template<>
void object::test<1>()
{
    set_test_name("points within Envelope");

    Envelope e(-20, -10, 8, 9);

    RandomPointsBuilder rpb(factory_.get());
    rpb.setNumPoints(9);
    rpb.setExtent(e);

    auto mp = rpb.getGeometry();

    ensure_equals(mp->getGeometryTypeId(), geos::geom::GEOS_MULTIPOINT);
    ensure_equals(mp->getNumGeometries(), 9u);

    for (std::size_t i = 0; i < mp->getNumGeometries(); i++) {
        const auto* pt = mp->getGeometryN(i);
        ensure(e.contains(*pt->getCoordinate()));
    }
}

template<>
template<>
void object::test<2>()
{
    set_test_name("points within linear Geometry");

    checkPointsInGeometry("POLYGON ((0 20, 20 20, 20 0, 18 0, 18 18, 0 18, 0 20))", 9);
}

template<>
template<>
void object::test<3>()
{
    set_test_name("points within curved Geometry");

    checkPointsInGeometry("CURVEPOLYGON (COMPOUNDCURVE ((0 18, 0 20, 20 20, 20 0, 18 0), CIRCULARSTRING (18 0, 12.5 12.5, 0 18)))", 7);
}

} // namespace tut
