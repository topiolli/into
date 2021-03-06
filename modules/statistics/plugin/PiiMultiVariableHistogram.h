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

#ifndef _PIIMULTIDIMENSIONALHISTOGRAM_H
#define _PIIMULTIDIMENSIONALHISTOGRAM_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>

/**
 * An operation that builds histograms out of correlated variables.
 * The most typical use of this operation may be in creating a 2-D or
 * 3-D color histogram out of color channels, but the operation can
 * create multi-dimensional distributions out of any data quantized to
 * integers. Assume you have separated the color channels of a 2-by-2
 * RGB image like this:
 *
 * ~~~
 *   R     G     B
 * +---+ +---+ +---+
 * |0 1| |1 0| |3 2|
 * |2 3| |2 3| |0 1|
 * +---+ +---+ +---+
 * ~~~
 *
 * Assume also that the maximum value for each channel is 3, i.e.
 * there are 4 distinct values. The length of the resulting histogram
 * in `JointDistribution` would be 4 * 4 * 4 = 64. The indices of the
 * three-dimensional colors in the resulting histogram would be (from
 * upper left corner) 0 + 4 * 1 + 4 * 4 * 3 = 52, 1 + 4 * 0 + 4 * 4 *
 * 2 = 32 etc. In `MarginalDistributions` mode the histograms are
 * calculated for each cannel separately, and concatenated together.
 * In the example above, the length of the histogram would be 4 + 4 +
 * 4 = 12.
 *
 * Inputs
 * ------
 *
 * @in matrixX - input matrices. X is a zero-based index, and its
 * maximum value depends on the number of levels. Any real-valued
 * matrix will be accepted. For maximum performance, input integer
 * matrices.
 *
 * Outputs
 * -------
 *
 * @out histogram - a multi-dimensional histogram folded into a
 * one-dimensional row matrix, or multiple one-dimensional histograms
 * concatenated into a row matrix (PiiMatrix<int>).
 *
 */
class PiiMultiVariableHistogram : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * A quantization level for each dimension. In the example above,
   * this list would have been created like this:
   *
   * ~~~(c++)
   * histogram->setProperty("levels", QVariantList() << 4 << 4 << 4);
   * ~~~
   *
   * The minimum number of levels is one. There can be at most eight
   * levels. In `JointDistribution` mode, the product of the
   * levels can be at most 2^24 (16M), which is already too much
   * for practical use. In theory, this allows one to create a
   * three-dimensional color histogram out of three 8-bit color
   * channels. In `MarginalDistributions` mode, the same limit holds
   * for the sum of levels.
   */
  Q_PROPERTY(QVariantList levels READ levels WRITE setLevels);

  /**
   * Scaling factors for each dimension. Each element in the input
   * matrices will be multiplied by the corresponding scale factor
   * before placing into the histogram. This makes it possible to
   * quantize the input at the same time. Set this property to an
   * empty list to disable scaling.
   *
   * If the RGB images in the example above had 256 levels, one should
   * scale the input channels down:
   *
   * ~~~(c++)
   * histogram->setProperty("scales", QVariantList() << 4.0/256 << 4.0/256 << 4.0/256);
   * ~~~
   */
  Q_PROPERTY(QVariantList scales READ scales WRITE setScales);

  /**
   * The type of distribution to create. The default is
   * `JointDistribution`.
   */
  Q_PROPERTY(DistributionType distributionType READ distributionType WRITE setDistributionType);
  Q_ENUMS(DistributionType);

  /**
   * Output normalization. If set to `true` output histograms will be
   * normalized.
   */
  Q_PROPERTY(bool normalized READ normalized WRITE setNormalized);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Output distribution types.
   *
   * - `JointDistribution` - a joint distribution will be created.
   * The length of the histogram will be \(\prod_i l_i\), where
   * \(l_i\) represents the ith entry in the [levels] list. Note that
   * one should use a low number of levels to avoid exhaustive memory
   * usage and an mostly empty histograms.
   *
   * - `MarginalDistributions` - marginal distributions will be
   * created for each input and concatenated together. The length of
   * the histogram will be \(\sum_i l_i\).
   */
  enum DistributionType { JointDistribution, MarginalDistributions };

  PiiMultiVariableHistogram();
  ~PiiMultiVariableHistogram();

  QVariantList levels() const;
  void setLevels(const QVariantList& levels);
  void setDistributionType(DistributionType distributionType);
  DistributionType distributionType() const;
  void setScales(const QVariantList& scales);
  QVariantList scales() const;

  void setNormalized(bool normalize);
  bool normalized() const;

protected:
  void process();
  void check(bool reset);

private:
  inline void throwTooLong() { PII_THROW(PiiExecutionException, tr("The resulting histogram would be too long. Please reduce levels.")); }
  template <class T> inline PiiMatrix<int> scale(const PiiVariant& obj, double factor);
  void setInputCount(int cnt);
  void jointHistogram(const QList<PiiMatrix<int> >& matrices, int rows, int columns, PiiMatrix<int>* result);
  void marginalHistograms(const QList<PiiMatrix<int> >& matrices, int rows, int columns, PiiMatrix<int>* result);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    QVector<int> vecLevels;
    QVector<int> vecSteps;
    QVector<double> vecScales;
    PiiOutputSocket* pHistogramOutput;
    DistributionType distributionType;
    bool bNormalized;
  };
  PII_D_FUNC;
};

#endif //_PIIMULTIDIMENSIONALHISTOGRAM_H
