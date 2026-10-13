-- This file is hand generated.

-- Denmark

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_4937_TO_EPSG_5733','ETRS89 to DNN height',
    NULL,
    'EPSG','9665','Geographic3D to GravityRelatedHeight (gtx)',
    'EPSG','4937', -- source CRS (ETRS89)
    'EPSG','5733', -- target CRS (DNN height)
    NULL,
    'EPSG','8666','Geoid (height correction) model file','dnn.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,0);

INSERT INTO "usage" VALUES(
    'PROJ',
    'EPSG_4937_TO_EPSG_5733_USAGE',
    'grid_transformation',
    'PROJ',
    'EPSG_4937_TO_EPSG_5733',
    'EPSG','3237', -- area of use: Denmark onshore
    'EPSG','1024'  -- unknown
);

-- Sweden

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_4977_TO_EPSG_5613','SWEREF99 to RH2000 height',
    NULL,
    'EPSG','9665','Geographic3D to GravityRelatedHeight (gtx)',
    'EPSG','4977', -- source CRS (SWEREF99)
    'EPSG','5613', -- target CRS (RH2000 height)
    NULL,
    'EPSG','8666','Geoid (height correction) model file','SWEN17_RH2000.gtx',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,0);

INSERT INTO "usage" VALUES(
    'PROJ',
    'EPSG_4977_TO_EPSG_5613_USAGE',
    'grid_transformation',
    'PROJ',
    'EPSG_4977_TO_EPSG_5613',
    'EPSG','3313', -- area of use: Sweden onshore
    'EPSG','1024'  -- unknown
);

-- Switzerland

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_4937_TO_EPSG_5729','ETRS89 to LHN95 height',
    NULL,
    'EPSG','9665','Geographic3D to GravityRelatedHeight (gtx)',
    'EPSG','4937', -- source CRS (ETRS89)
    'EPSG','5729', -- target CRS (LHN95 height)
    NULL,
    'EPSG','8666','Geoid (height correction) model file','chgeo2004_ETRS.agr',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,0);

INSERT INTO "usage" VALUES(
    'PROJ',
    'EPSG_4937_TO_EPSG_5729_USAGE',
    'grid_transformation',
    'PROJ',
    'EPSG_4937_TO_EPSG_5729',
    'EPSG','1286', -- area of use: Europe - Liechtenstein and Switzerland
    'EPSG','1024'  -- unknown
);

INSERT INTO "grid_transformation" VALUES(
    'PROJ','EPSG_4937_TO_EPSG_5728','ETRS89 to LN02 height',
    NULL,
    'EPSG','9665','Geographic3D to GravityRelatedHeight (gtx)',
    'EPSG','4937', -- source CRS (ETRS89)
    'EPSG','5728', -- target CRS (LN02 height)
    NULL,
    'EPSG','8666','Geoid (height correction) model file','chgeo2004_htrans_ETRS.agr',
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,0);

INSERT INTO "usage" VALUES(
    'PROJ',
    'EPSG_4937_TO_EPSG_5728_USAGE',
    'grid_transformation',
    'PROJ',
    'EPSG_4937_TO_EPSG_5728',
    'EPSG','1286', -- area of use: Europe - Liechtenstein and Switzerland
    'EPSG','1024'  -- unknown
);
