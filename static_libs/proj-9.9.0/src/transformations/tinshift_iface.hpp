/******************************************************************************
 * Project:  PROJ
 * Purpose:  Functionality related to TIN based transformations
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2025, Even Rouault, <even.rouault at spatialys.com>
 *
 * SPDX-License-Identifier: MIT
 *****************************************************************************/

#ifndef TINSHIFT_IFACE_H
#define TINSHIFT_IFACE_H

// ---------------------------------------------------------------------------

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

/** Interface to evaluate the transformation of a coordinate */
class TINShiftEvaluator {
  public:
    inline virtual ~TINShiftEvaluator() = default;

    /** Evaluate displacement of a position given by (x,y,z,t) and
     * return it in (x_out,y_out_,z_out).
     */
    virtual bool forward(double x, double y, double z, double &x_out,
                         double &y_out, double &z_out) = 0;

    /** Apply inverse transformation. */
    virtual bool inverse(double x, double y, double z, double &x_out,
                         double &y_out, double &z_out) = 0;
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TINSHIFT_IFACE_H
