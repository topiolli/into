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

#ifndef _PIITRACKEDPOINTERHOLDER_H
#define _PIITRACKEDPOINTERHOLDER_H

#include "PiiSerializationGlobal.h"

/**
 * A class that is used by PiiOutputArchive to store tracked
 * pointers while serializing them. When a tracked pointer of type 
 * `T` is saved first time, PiiOutputArchive calls
 * PiiSerialization::createTrackedPointerHolder(T*) and inserts the
 * returned object to the list of tracked pointers. The list
 * is consulted whenever a tracked pointer is being saved. If its
 * address already exists, it won't be saved again. The stored
 * PiiTrackedPointerHolder instances will be deleted when the archive
 * is destroyed.
 *
 * If a pointer type requires special actions when saved, one can
 * create a specialization of
 * PiiSerialization::createTrackedPointerHolder(). The following
 * example shows how to ensure a refcounted object won't be deleted
 * during serialization.
 *
 *
 * ~~~
 * class MyRefCountedObjHolder : public PiiTrackedPointerHolder
 * {
 *   public:
 *     MyRefCountedObjHolder(const MyRefCountedObj* pointer) :
 *       PiiTrackedPointerHolder(pointer)
 *     {
 *       pointer->increaseRefCount();
 *     }
 *
 *     ~MyRefCountedObjHolder()
 *     {
 *       reinterpret_cast<const MyRefCountedObj*>(_pointer)->decreaseRefCount();
 *     }
 * };
 *
 * namespace PiiSerialization
 * {
 *   inline PiiTrackedPointerHolder* createTrackedPointerHolder(const MyRefCountedObj* ptr)
 *   {
 *     return new MyRefCountedObjHolder(ptr);
 *   }
 * }
 * ~~~
 *
 */
class PII_SERIALIZATION_EXPORT PiiTrackedPointerHolder
{
public:
  /**
   * Creates a new pointer holder.
   *
   * @param pointer the memory address of a tracked object
   *
   * @param pointerIndex a serial number generated by
   * PiiOutputArchive.
   *
   * @param reference a flag that indicates whether the object was
   * saved through a reference.
   */
  PiiTrackedPointerHolder(const void* pointer = 0,
                          int pointerIndex = 0,
                          bool reference = false);
  
  virtual ~PiiTrackedPointerHolder();

  /**
   * Returns the held pointer.
   */
  const void* pointer() const { return _pointer; }
  /**
   * Sets the held pointer.
   */
  void setPointer(const void* pointer) { _pointer = pointer; }

  /**
   * Set the pointer index.
   */
  void setPointerIndex(int pointerIndex) { _iPointerIndex = pointerIndex; }
  /**
   * Returns the pointer index.
   */
  int pointerIndex() const { return _iPointerIndex; }

  /**
   * Returns `true` if the object pointed to by the held pointer was
   * saved by a reference, and `false` otherwise.
   */
  bool isSavedByReference() const { return _bSavedByReference; }
  /**
   * Sets the "saved by reference" flag.
   */
  void setSavedByReference(bool saved) { _bSavedByReference = saved; }
  
protected:
  /// The address of the tracked object.
  const void* _pointer;

private:
  int _iPointerIndex;
  bool _bSavedByReference;
};

namespace PiiSerialization
{
  /**
   * Create an instance of PiiTrackedPointerHolder. Override this
   * function if you want to provide a custom holder for your type.
   *
   * @relates PiiTrackedPointerHolder
   */
  inline PiiTrackedPointerHolder* createTrackedPointerHolder(const void* ptr)
  {
    return new PiiTrackedPointerHolder(ptr);
  }
}

#endif //_PIITRACKEDPOINTERHOLDER_H
