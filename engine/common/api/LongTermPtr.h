#ifndef POPULUSSC_LONGTERMPTR_H
#define POPULUSSC_LONGTERMPTR_H

/******************************************************************************
**
**   File:        LongTermPtr.h
**   Description:
**
**   Copyright (C) 2017 Luxoft GmbH
**
**   This file is part of Safe Renderer.
**
**   Safe Renderer is free software: you can redistribute it and/or
**   modify it under the terms of the GNU Lesser General Public
**   License as published by the Free Software Foundation.
**
**   Safe Renderer is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**   Lesser General Public License for more details.
**
**   You should have received a copy of the GNU Lesser General Public
**   License along with Safe Renderer.  If not, see
**   <http://www.gnu.org/licenses/>.
**
**   SPDX-License-Identifier: LGPL-3.0
**
******************************************************************************/

#include "PscTypes.h"

#include <stdint.h>

namespace psc
{

/**
 * Class provides functionality to store a pointer to some object
 * with safety mechanisms to protect this pointer.
 * This class should be used for long-term variables.
 *
 * As safety mechanism, class uses negative checking.
 *
 * @tparam the type of objects, the pointer to which will be stored inside @cPointerHolder object.
 */
template <class T>
class LongTermPtr P_FINAL
{
public:
    /**
     * Initialize object with NULL pointer.
     */
    LongTermPtr();

    /**
     * Initialize object with the pointer.
     *
     * @param ptr the pointer to be stored.
     */
    LongTermPtr(T* ptr);

    LongTermPtr(const LongTermPtr<T>& rhs);

    LongTermPtr<T>& operator=(const LongTermPtr<T>& rhs);

    /**
     * Return stored pointer.
     *
     * @return NULL if stored pointer doesn't pass correctness check,
     *         otherwise pointer to the object.
     */
    T* getPtr() const;

    /**
     * Check pointer for conformity to the internal data of the @c LongTermPtr object.
     *
     * @param[in] ptr pointer which shall be checked.
     *
     * @return @c true if pointer is exactly the same as the one stored inside the object and
     *         if it passed negative checking, @c false otherwise.
     */
    bool checkPtr(const T* ptr) const;

    ~LongTermPtr();

private:
    T* m_ptr;
    uintptr_t m_invertedPtr;
};

template <class T>
LongTermPtr<T>::LongTermPtr()
    : m_ptr(NULL)
    , m_invertedPtr(0U)
{
}

template <class T>
LongTermPtr<T>::LongTermPtr(T* ptr)
    : m_ptr(ptr)
    , m_invertedPtr((NULL != ptr) ? ~reinterpret_cast<uintptr_t>(ptr) : 0U)
{
}

template <class T>
LongTermPtr<T>::LongTermPtr(const LongTermPtr<T>& rhs)
    : m_ptr(rhs.m_ptr)
    , m_invertedPtr(rhs.m_invertedPtr)
{
}

template <class T>
LongTermPtr<T>::~LongTermPtr()
{
}

template <class T>
LongTermPtr<T>& LongTermPtr<T>::operator=(const LongTermPtr<T>& rhs)
{
    if (this != &rhs)
    {
        m_ptr = rhs.m_ptr;
        m_invertedPtr = rhs.m_invertedPtr;
    }
    return *this;
}

template <class T>
T* LongTermPtr<T>::getPtr() const
{
    T* ptr = NULL;

    if (checkPtr(m_ptr))
    {
        ptr = m_ptr;
    }

    return ptr;
}

template <class T>
bool LongTermPtr<T>::checkPtr(const T* ptr) const
{
    uintptr_t ptrValue = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t storedPtrValue = reinterpret_cast<uintptr_t>(m_ptr);
    uintptr_t invPtr = ~ ptrValue;

    return (ptrValue == storedPtrValue && invPtr == m_invertedPtr);
}

} // namespace psc

#endif // POPULUSSC_LONGTERMPTR_H
