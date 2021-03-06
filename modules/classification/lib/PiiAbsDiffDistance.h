/* This file is part of Into.
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#ifndef _PIIABSDIFFDISTANCE_H
#define _PIIABSDIFFDISTANCE_H

#include "PiiDistanceMeasure.h"

/**
 * Calculates the sum of absolute differences between corresponding
 * elements in two feature vectors. \(d = \sum |S_i - M_i|\), where
 * *S* and *M* represent the sample and model distributions,
 * respectively.
 *
 */
PII_DEFAULT_DISTANCE_MEASURE_DEF(PiiAbsDiffDistance)
{
  double distance = 0;
  for (int i=0; i<length; ++i)
    distance += Pii::abs(sample[i] - model[i]);
  return distance;
}

#endif //_PIIABSDIFFDISTANCE_H
