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

#ifndef _PIIBACKGROUNDEXTRACTOR_H
#define _PIIBACKGROUNDEXTRACTOR_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include <PiiColor.h>

/**
 * An operation that models static background of a scene with moving
 * objects. The background model is based on the mean and covariance
 * values of the image pixels. The background model is updated
 * according to the following formula:
 *
 * \[
 * B_{t+1} = B_t + (\alpha_1 * (1 - I_t) + \alpha_2 * I_t) * (I_t - B_t),
 * \]
 *
 * where \(B_t\) is the background model at the time moment *t* and
 * \(I_t\) the current intensity of a pixel. \(\alpha_1\) and
 * \(\alpha_2\) are learning weights that control the speed at which
 * the foreground pixels are merged in the background. Note that the
 * input image is normalized so that the maximum pixel intensity is
 * always one.
 *
 * Inputs
 * ------
 *
 * @in image - any color or gray-scale image.
 *
 * Outputs
 * -------
 *
 * @out image - an image (PiiMatrix<int>) in which background pixels
 * are zero. The value of a pixel determines the number of successive
 * frames the pixel has belonged to foreground.
 *
 * @out movement - this output emits a boolean value that determines
 * if there is significant movement in the current frame. The emitted
 * value will be `true` if the relative number of detected foreground
 * pixels is above [movementThreshold], and `false` otherwise.
 *
 */
class PiiBackgroundExtractor : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The minimum difference between the background model and the
   * current frame that will be considered a change. The default value
   * is 25.
   */
  Q_PROPERTY(double threshold READ threshold WRITE setThreshold);

  /**
   * The first learning weight (0.1 by default).
   */
  Q_PROPERTY(double alpha1 READ alpha1 WRITE setAlpha1);

  /**
   * The second learning weight (0.01 by default).
   */
  Q_PROPERTY(double alpha2 READ alpha2 WRITE setAlpha2);

  /**
   * The maximum number of successive frames a pixel can belong to
   * foreground. This value makes it possible to kill burnt in objects
   * before the adaptation catches them. The default value is 1000.
   */
  Q_PROPERTY(int maxStillTime READ maxStillTime WRITE setMaxStillTime);

  /**
   * The maximum fraction of pixels that can be classified as
   * foreground before "significant movement" is detected. The default
   * value is 1.
   */
  Q_PROPERTY(double movementThreshold READ movementThreshold WRITE setMovementThreshold);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiBackgroundExtractor();

  double threshold() const;
  void setThreshold(double threshold);
  double alpha1() const;
  void setAlpha1(double alpha1);
  double alpha2() const;
  void setAlpha2(double alpha2);
  void setMaxStillTime(int maxStillTime);
  int maxStillTime() const;
  void setMovementThreshold(double movementThreshold);
  float movementThreshold() const;

protected:
  void process();

private:
  template <class T> void operate(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    bool bFirst;
    double dThreshold;
    double dAlpha1;
    double dAlpha2;
    PiiMatrix<int> matStillCounter;
    PiiMatrix<float> matBackground;
    PiiMatrix<float> matForeground;

    int iMaxStillTime;
    double dMovementThreshold;
  };
  PII_D_FUNC;
};

#endif //_PIIBACKGROUNDEXTRACTOR_H
