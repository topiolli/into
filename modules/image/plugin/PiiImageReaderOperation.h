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

#ifndef _PIIIMAGEREADEROPERATION_H
#define _PIIIMAGEREADEROPERATION_H

#include <PiiDefaultOperation.h>
#include <QImage>
#include "PiiImageGlobal.h"

/**
 * A base class for image sources.
 *
 * Inputs
 * ------
 *
 * @in trigger - an optional trigger input. A new image is emitted
 * whenever any object is received in this input.
 *
 * Outputs
 * -------
 *
 * @out image - the image output. Emits either four-channel color
 * or grayscale (the default) images in 8-bit (unsigned char) channel
 * format.
 *
 */
class PII_IMAGE_EXPORT PiiImageReaderOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The maximum number of images the source will emit. This is the
   * upper limit for the number of images the source will ever emit,
   * regardless of the total amount of images available. -1 means
   * eternally.
   */
  Q_PROPERTY(int maxImages READ maxImages WRITE setMaxImages);

  /**
   * The (zero-based) index of the next image to be emitted.
   */
  Q_PROPERTY(int currentImageIndex READ currentImageIndex);

  /**
   * The type of the images that are written out. If the type of the
   * opened image does not match the one indicated here, the image
   * will be automatically converted to the correct type. The default
   * type is Original.
   */
  Q_PROPERTY(ImageType imageType READ imageType WRITE setImageType);
  Q_ENUMS(ImageType);
public:
  /**
   * Possible image types.
   *
   * - `GrayScale` - the images will be of an 8-bit gray scale type
   * (unsigned char)
   *
   * - `Color` - the images will be of a 32-bit RGBA color type
   * (PiiColor4<unsigned char>). This is the native color format
   * supported by QImage.
   *
   * - `Original` - the output type will be either gray scale or
   * color, depending on the type of the input image.
   */
  enum ImageType { GrayScale, Color, Original };

  void check(bool reset);

  virtual int maxImages() const;
  virtual void setMaxImages(int cnt);
  virtual ImageType imageType() const;
  virtual void setImageType(ImageType type);
  virtual int currentImageIndex() const;

protected:
  /**
   * Creates a PiiQImage<unsigned char> out of *img* and emits
   * it through the image output. The ownership of the data buffer in
   * *img* is transferred to the PiiQImage, and may have been
   * deleted already when the function returns.
   */
  void emitGrayImage(QImage& img);
  /**
   * Creates a PiiQImage<PiiColor4<> > out of *img* and emits
   * it through the image output. The ownership of the data buffer in
   * *img* is transferred to the PiiQImage, and may have been
   * deleted already when the function returns.
   */
  void emitColorImage(QImage& img);
  /**
   * Creates either a PiiMatrix<PiiColor4<> > or PiiMatrix<unsigned
   * char> depending on the type of *img*, and emits the result
   * through the image output.
   */
  void emitImage(QImage& img);

  /// @internal
  class PII_IMAGE_EXPORT Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiInputSocket* pTriggerInput;
    PiiOutputSocket* pImageOutput;

    ImageType imageType;
    /**
     * The maximum number of images the source will emit.
     */
    int iMaxImages;
    /**
     * The index of the current (to-be-emitted) image.
     */
    int iCurrentIndex;
  };
  PII_D_FUNC;

  PiiImageReaderOperation(Data* data);
};


#endif //_PIIIMAGEREADEROPERATION_H
