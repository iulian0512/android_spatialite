
2026-09-01

- Breaking Changes:
  - BufferOp returns POLYGON EMPTY when fed Inf/Nan coords (GH-1332)
  - Return Inf when calculating distance to an empty geometry (GH-1345, Even Rouault)
  - Overlay operations now produce a LineString geometry in cases that would previously
    produce a MultiLineString with contiguous sub-geometries. (GH-1459, Dan Baston)
  - Overlay operations that previously rotated rings by one vertex no longer do so
    (GH-1412, Dan Baston)

- New things:
  - Add GEOSMinimumSpanningTree (Paul Ramsey)
  - Add GEOSCoverageEdges (Paul Ramsey)
  - Add new C API functions for Hausdorff distance (GH-1352, Sven Jensen)
  - Add GEOSSubdivideByGrid (GH-1232, Dan Baston)
  - Add support for curved geometries to the following operations:
    - BoundaryOp (GH-1394, Dan Baston)
    - DistanceOp (GH-1428, Dan Baston)
    - GeometryNoder (GH-1347, Dan Baston)
    - GeometryPrecisionReducer (pointwise only, GH-1402; Dan Baston)
    - LineMerger (GH-1441, Dan Baston)
    - Normalize (GH-1397, Dan Baston)
    - OverlayNG (GH-1427, Dan Baston)
    - Polygonizer (GH-1420, Dan Baston)
  - Add GEOSLineToCurve, GEOSCurveToLine (GH-1382, Dan Baston)
  - Add GeometrySplitter (GH-1424, Dan Baston)
  - Add progress reporting to GEOSCoverageSimplify, GEOSUnaryUnion (GH-1466, Even Rouault / Dan Baston)

- Fixes/Improvements:
  - Buffer of Linestring includes spurious hole (GH-1217, Moritz Kirmse)
  - Preserve M values in GEOSInterpolate (GH-1390, Dan Baston)
  - Preserve M values in GEOSLineMerge (GH-1364, Dan Baston)
  - Preserve M values in GEOSCoverageSimplify (GH-1384, Paul Ramsey)
  - Preserve M values in GEOSDensify (GH-1319, Dan Baston)
  - Preserve M values in GEOSSimplify (GH-1317, Dan Baston)
  - Preserve M values in GEOSPolygonize (GH-1363 Dan Baston)
  - Fix some cases of dropped M values in overlay (GH-1364,GH-1388,GH-1407,GH-1408,GH-1474; Dan Baston)
  - GEOSClusterDBSCAN fix unsassigned clusters with minPoints <= 1 (GH-1386, Dan Baston)
  - Fix crash in GEOSConvexHull (GH-1358, Dan Baston)
  - Overlay performance improvements (GH-1353, arriopolis, Martin Davis)
  - Fix intersection result for slightly overlapping polygons (GH-1499, Petr Belohlavek)


