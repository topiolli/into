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

#include "PiiReadWriteLock.h"

#include <QThread>

// Note: Similarities to QReadWriteLock are not coincidental.

PiiReadWriteLock::Data::Data(bool recursive) :
  currentWriter(0),
  bRecursive(recursive),
  iActiveReaders(0),
  iActiveWriters(0),
  iWaitingReaders(0),
  iWaitingWriters(0)
{}

PiiReadWriteLock::PiiReadWriteLock() : d(new Data(false))
{
}

PiiReadWriteLock::PiiReadWriteLock(RecursionMode mode) : d(new Data(mode == Recursive))
{
}

PiiReadWriteLock::~PiiReadWriteLock()
{
  delete d;
}

void PiiReadWriteLock::lockForRead()
{
  QMutexLocker lock(&d->mutex);

  Qt::HANDLE self = 0;
  // If the lock is recursive, must check if we currently hold it.
  if (d->bRecursive)
    {
      self = QThread::currentThreadId();

      ThreadHash::iterator it = d->hashCurrentReaders.find(self);
      // Re-acquiring a read lock
      if (it != d->hashCurrentReaders.end())
        {
          ++it.value();
          ++d->iActiveReaders;
          return;
        }
      // Using a write lock for reading
      if (d->currentWriter == self)
        {
          d->hashCurrentReaders.insert(self, 1);
          ++d->iActiveReaders;
          return;
        }
    }

  // Must wait for all writers to finish.
  while (d->iActiveWriters > 0 || d->iWaitingWriters > 0)
    {
      ++d->iWaitingReaders;
      d->readerWait.wait(&d->mutex);
      --d->iWaitingReaders;
    }
  if (d->bRecursive)
    d->hashCurrentReaders.insert(self, 1);

  ++d->iActiveReaders;
}

void PiiReadWriteLock::lockForWrite()
{
  QMutexLocker lock(&d->mutex);

  Qt::HANDLE self = 0;
  int iRemainingReaders = 0;
  if (d->bRecursive)
    {
      self = QThread::currentThreadId();

      // Recursive lock can be locked for writing again.
      if (d->currentWriter == self)
        {
          ++d->iActiveWriters;
          return;
        }
      // See if we currently hold a read lock.
      ThreadHash::iterator it = d->hashCurrentReaders.find(self);
      // Yes, we must leave one reader in the queue.
      if (it != d->hashCurrentReaders.end())
        iRemainingReaders = it.value();
    }

  while (d->iActiveWriters > 0 || d->iActiveReaders > iRemainingReaders)
    {
      ++d->iWaitingWriters;
      d->writerWait.wait(&d->mutex);
      --d->iWaitingWriters;
    }

  d->currentWriter = self;
  ++d->iActiveWriters;
}

void PiiReadWriteLock::unlockRead()
{
  QMutexLocker lock(&d->mutex);

  Q_ASSERT(d->iActiveReaders > 0);

  if (d->bRecursive)
    {
      Qt::HANDLE self = QThread::currentThreadId();
      ThreadHash::iterator it = d->hashCurrentReaders.find(self);
      if (it != d->hashCurrentReaders.end() &&
          --it.value() <= 0)
        d->hashCurrentReaders.erase(it);
    }

  if (--d->iActiveReaders == 0 && d->iActiveWriters == 0)
    wakeUp();
}

void PiiReadWriteLock::unlockWrite()
{
  QMutexLocker lock(&d->mutex);

  Q_ASSERT(d->iActiveWriters > 0);

  if (--d->iActiveWriters == 0)
    {
      d->currentWriter = 0;
      if (d->iActiveReaders == 0)
        wakeUp();
    }
}

void PiiReadWriteLock::wakeUp()
{
  if (d->iWaitingWriters)
    d->writerWait.wakeOne();
  else if (d->iWaitingReaders)
    d->readerWait.wakeAll();
}
