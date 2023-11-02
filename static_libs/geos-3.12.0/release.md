2023-06-27

- New things:
  - C++14 is now required.
  - Polygonal coverages: CoverageValidator, CoveragePolygonValidator,
    CoverageGapFinder, CoverageUnion (JTS-900, Martin Davis & Paul Ramsey)
  - Support reading and writing M values through WKB and WKT readers/writers
    (GH-721, Dan Baston)
  - Interpolate M values in overlay results (GH-802, Dan Baston)
  - CAPI: GEOSPreparedContainsXY, GEOSPreparedIntersectsXY (GH-677, Dan Baston)
  - Add CoordinateSequenceIterator (GH-685, Dan Baston)
  - Geometry clustering: DBSCAN, geometry intersection/distance, envelope
    intersection/distance (GH-688, Dan Baston)
  - CAPI: GEOSDisjointSubsetUnion (GH-692, Dan Baston)
  - CAPI: GEOSLineSubstring (GH-706, Dan Baston)
  - CAPI: GEOSEqualsIdentical (GH-810, Dan Baston)
  - CAPI: GEOSOrientPolygons (GH-818, Dan Baston)
  - CAPI: GEOSSTRtree_build (GH-835, Dan Baston)
  - CAPI: GEOSConcaveHullByLength (GH-849, Martin Davis)
  - CAPI: GEOSGeomGetM (GH-864, Mike Taves)
  - Voronoi: Add option to create diagram in order consistent with inputs (GH-781, Dan Baston)
  - Polygonal coverages: CoverageSimplifier (JTS-911, Martin Davis)
  - CAPI: GEOSCoverageIsValid, GEOSCoverageSimplifyVW (GH-867, Paul Ramsey)
  - CAPI: GEOSGeom_releaseCollection (GH-848)
  - CAPI: GEOSMinimumRotatedRectangle now uses MinimumAreaRectangle (Paul Ramsey)

- Breaking Changes
  - CoverageUnion now requires valid inputs to produce valid outputs
    and may return invalid outputs silently when fed invalid inputs.
    Use CoverageValidator first if you do not know the validity of your data.

- Fixes/Improvements:
  - WKTReader: Fix parsing of Z and M flags in WKTReader (#676 and GH-669, Dan Baston)
  - WKTReader: Throw exception on inconsistent geometry dimension (#1080, Dan Baston)
  - WKTReader: Throw exception if WKT contains extra text after end of geometry (#1095, Dan Baston)
  - GEOSIntersects: Fix crash with empty point inputs (#1110, Dan Baston)
  - GEOSIntersects: Improve performance/robustness by using PreparedGeometry algorithm (GH-775, Dan Baston)
  - LineMerger: Recursively collect all components from GeometryCollections (#401, Dan Baston)
  - GeometryPrecisionReducer: Return correct dimensionality for empty results (GH-684, Dan Baston)
  - Improve performance of coverage union (GH-681, Dan Baston)
  - Improve performance of prepared polygon intersection (GH-690, Dan Baston)
  - Improve performance of prepared polygon distance (GH-693, Dan Baston)
  - Implement indexed calculations for prepared geometry isWithinDistance (GH-691, Dan Baston)
  - Fix LineSegment.orientationIndex(LineSegment) (GH-699, Martin Davis)
  - Fix DepthSegment comparison operation (GH-707, Martin Davis)
  - Add OverlayNG support for simple GeometryCollection inputs (GH-716, Martin Davis)
  - Fix TopologyPreservingSimplifier to produce stable results for Multi inputs (GH-718, Martin Davis)
  - Improve ConvexHull radial sort robustness (GH-724, Martin Davis)
  - Use more robust Delaunay Triangulation frame size heuristic (GH-728, Martin Davis)
  - DiscreteFrechetDistance: Fix crash with empty inputs (GH-751, Dan Baston)
  - GEOSSimplify / DouglasPeuckerSimplifier: Allow ring origin to be removed (GH-773, Dan Baston)
  - GEOSTopologyPreserveSimplify / TopologyPreservingSimplifier: Allow ring origin to be removed (GH-784, Dan Baston)
  - PreparedLineStringIntersects: Fix incorrect result with mixed-dim collection (GH-774, Dan Baston)
  - GEOSIntersection: Fix FE_INVALID exception on intersection of disjoint geometries
    (GH-791, Joris Van den Bossche & Dan Baston)
  - Fix incorrect result from Envelope::disjoint (GH-791, Dan Baston)
  - Polygonizer: Fix duplicate lines return by getInvalidRingLines (GH-782, Martin Davis & Dan Baston)
  - GEOSUnaryUnion: Fix crash on collection containing empty point (GH-830, Dan Baston)
  - GEOSBuffer: Fix crash with Inf coordinates (GH-822, Dan Baston)
  - GEOSSTRtree_iterate: Do not return removed items (GH-833, Dan Baston)
  - IndexedFacetDistance: Fix crash with Inf coordinates (GH-821, Dan Baston)
  - HausdorffDistance: Fix crash on collection containing empty point (GH-840, Dan Baston)
  - MaximumInscribedCircle: Fix infinite loop with non-finite coordinates (GH-843, Dan Baston)
  - DistanceOp: Fix crash on collection containing empty point (GH-842, Dan Baston)
  - OffsetCurve: improve behaviour and add Joined mode (JTS-956, Martin Davis)
  - GeometryPrecisionReducer: preserve input collection types (GH-846, Paul Ramsey)
  - OffsetCurve: handle zero-distance offsets (GH-850, Martin Davis)
  - Tri: add exceptions for invalid indexes (GH-853, Martin Davis)
  - LargestEmptyCircle: enhance boundary to allow any polygonal geometry (GH-859, Martin Davis)
  - Fix MaximumInscribedCircle and LargestEmptyCircle performance and memory issues (GH-883, Martin Davis)
  - GEOSHasZ: Fix handling with empty geometries (GH-887, Mike Taves)
  - OffsetCurve: fix EndCap parameter handling (GH-899, Martin Davis)
  - Reduce artifacts in single-sided Buffers: (GH-665 #810 and #712, Sandro Santilli)
  - GeoJSONReader: Fix 2D empty geometry creation (GH-909, Mike Taves)
  - GEOSClipByRect: Fix case with POINT EMPTY (GH-913, Mike Taves)
  - Support mixed GeometryCollection in overlay ops (GH-797, Paul Ramsey)

- Changes:
  - Remove Orientation.isCCW exception to simplify logic and align with JTS (GH-878, Martin Davis)
  - Change MultiPoint WKT to use parentheses in sub-members (GH-903, Mike Taves)
  - Change WKBWriter default output dimension to 4 (GH-908, Mike Taves)
  - Change WKTWriter defaults output dimension to 4 and trim to 'on' (GH-915, Mike Taves)

