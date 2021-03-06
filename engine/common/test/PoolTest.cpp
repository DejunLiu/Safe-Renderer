/******************************************************************************
**
**   File:        PoolTest.cpp
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

#include "PoolTestTypes.h"

#include <PSCError.h>

#include <gtest/gtest.h>
#include <cstddef>

TYPED_TEST_CASE(PoolTest, PoolTestCases);

TYPED_TEST(PoolTest, Allocate)
{
    const std::size_t startPtr = reinterpret_cast<std::size_t>(this->allocate());
    EXPECT_TRUE(startPtr >= reinterpret_cast<std::size_t>(this->m_pool));

    std::size_t prevPtr = startPtr;
    std::size_t prevCellSize = 0U;

    for (std::size_t i = 1U; i < TypeParam::PoolSize::value; ++i)
    {
        std::size_t ptr = reinterpret_cast<std::size_t>(this->allocate());
        EXPECT_TRUE(ptr > prevPtr);

        std::size_t cellSize = ptr - prevPtr;
        EXPECT_TRUE(cellSize >= sizeof(typename TypeParam::ObjectType));

        if (prevCellSize > 0U)
        {
            EXPECT_EQ(prevCellSize, cellSize);
        }

        prevPtr = ptr;
        prevCellSize = cellSize;
    }

    EXPECT_EQ(PSCError(PSC_POOL_IS_FULL), this->allocateFailed());
}

TYPED_TEST(PoolTest, OverAllocate)
{
    this->fillUpPool();

    EXPECT_EQ(PSCError(PSC_POOL_IS_FULL), this->allocateFailed());
}

TYPED_TEST(PoolTest, Deallocate)
{
    std::size_t objects[TypeParam::PoolSize::value];

    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        objects[i] = reinterpret_cast<std::size_t>(this->allocate());
    }

    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        EXPECT_EQ(PSCError(PSC_NO_ERROR),
                  this->m_pool->deallocate(reinterpret_cast<void*>(objects[i])));
    }
}

TYPED_TEST(PoolTest, Defragmentation)
{
    std::size_t objects[TypeParam::PoolSize::value];

    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        objects[i] = reinterpret_cast<std::size_t>(this->allocate());
    }

    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        EXPECT_EQ(PSCError(PSC_NO_ERROR),
                  this->m_pool->deallocate(reinterpret_cast<void*>(objects[i])));
        // Check, that next allocation will be on the last empty object.
        std::size_t ptr = reinterpret_cast<std::size_t>(this->allocate());
        EXPECT_EQ(objects[i], ptr);
    }
}

TYPED_TEST(PoolTest, DeallocateTwice)
{
    std::size_t objects[TypeParam::PoolSize::value];

    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        objects[i] = reinterpret_cast<std::size_t>(this->allocate());
    }

    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        EXPECT_EQ(PSCError(PSC_NO_ERROR),
                  this->m_pool->deallocate(reinterpret_cast<void*>(objects[i])));
        EXPECT_EQ(PSCError(PSC_POOL_DOUBLE_DELETE),
                  this->m_pool->deallocate(reinterpret_cast<void*>(objects[i])));
        EXPECT_EQ(PSCError(PSC_POOL_DOUBLE_DELETE),
                  this->m_pool->deallocate(reinterpret_cast<void*>(objects[i])));
    }
}

TYPED_TEST(PoolTest, DeallocateFailedWithNullPointer)
{
    this->fillUpPool();

    EXPECT_EQ(PSCError(PSC_POOL_INVALID_OBJECT),
              this->m_pool->deallocate(NULL));
}

TYPED_TEST(PoolTest, DeallocateWithPointerFromAnotherBuffer)
{
    // Pointer to memory before storage
    U8* ptr1 = reinterpret_cast<U8*>(this->allocate());
    EXPECT_EQ(PSCError(PSC_POOL_INVALID_OBJECT),
              this->m_pool->deallocate(ptr1 - 0x50));

    const std::size_t storageSize = sizeof(this->m_corrupter->storage);

    // Pointer to memory after storage
    EXPECT_EQ(PSCError(PSC_POOL_INVALID_OBJECT),
              this->m_pool->deallocate(ptr1 + storageSize + 1));
}

TYPED_TEST(PoolTest, DeallocateWithShiftedPointer)
{
    U8* ptr1 = reinterpret_cast<U8*>(this->allocate());
    this->allocate();

    const std::size_t cellSize = sizeof(typename PoolTest<TypeParam>::CorrupterType::Node);
    const std::size_t storageSize = sizeof(this->m_corrupter->storage);

    // check at the position < cellSize
    EXPECT_EQ(PSCError(PSC_POOL_INVALID_OBJECT),
              this->m_pool->deallocate(ptr1 + 1));

    // check at the position > cellSize
    EXPECT_EQ(PSCError(PSC_POOL_INVALID_OBJECT),
              this->m_pool->deallocate(ptr1 + cellSize + 1));

    // check at very end
    U8* lastCell = ptr1 + storageSize - cellSize;
    EXPECT_EQ(PSCError(PSC_POOL_INVALID_OBJECT),
              this->m_pool->deallocate(lastCell + 1));

    // check after very end
    U8* end = ptr1 + storageSize;
    EXPECT_EQ(PSCError(PSC_POOL_INVALID_OBJECT),
              this->m_pool->deallocate(end));
}

TYPED_TEST(PoolTest, IsAllocatedObject)
{
    std::size_t objects[TypeParam::PoolSize::value];
    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        objects[i] = reinterpret_cast<std::size_t>(this->allocate());
    }

    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        EXPECT_TRUE(this->m_pool->isAllocated(reinterpret_cast<void*>(objects[i])));
    }
}

TYPED_TEST(PoolTest, IsAllocatedFreeObject)
{
    U8* ptr1 = reinterpret_cast<U8*>(this->allocate());
    U8* ptr2 = reinterpret_cast<U8*>(this->allocate());

    const std::size_t cellSize = sizeof(typename PoolTest<TypeParam>::CorrupterType::Node);

    // Calc address of next free object
    U8* nextPtr = ptr2 + cellSize;

    EXPECT_TRUE(this->m_pool->isAllocated(ptr1));
    EXPECT_TRUE(this->m_pool->isAllocated(ptr2));
    EXPECT_TRUE(this->m_pool->isAllocated(nextPtr));
}

TYPED_TEST(PoolTest, IsAllocatedSpoiledFreeObject)
{
    U8* ptr1 = reinterpret_cast<U8*>(this->allocate());
    U8* ptr2 = reinterpret_cast<U8*>(this->allocate());

    const std::size_t cellSize = sizeof(typename PoolTest<TypeParam>::CorrupterType::Node);
    U8* nextPtr = ptr2 + cellSize;

    // Lets spoiled this data
    memset(ptr1, 0x03, cellSize * 3);

    EXPECT_FALSE(this->m_pool->isAllocated(ptr1));
    EXPECT_FALSE(this->m_pool->isAllocated(ptr2));
    EXPECT_FALSE(this->m_pool->isAllocated(nextPtr));
}

TYPED_TEST(PoolTest, IsAllocatedObjectOutsideStorage)
{
    U8* ptr1 = reinterpret_cast<U8*>(this->allocate());
    const std::size_t storageSize = sizeof(this->m_corrupter->storage);

    // Before storage.
    EXPECT_FALSE(this->m_pool->isAllocated(ptr1 - 0x50));

    // After storage.
    EXPECT_FALSE(this->m_pool->isAllocated(ptr1 + storageSize + 1));
}

TYPED_TEST(PoolTest, IsAllocatedShiftedObject)
{
    U8* ptr1 = reinterpret_cast<U8*>(this->allocate());
    this->allocate();

    const std::size_t cellSize = sizeof(typename PoolTest<TypeParam>::CorrupterType::Node);
    const std::size_t storageSize = sizeof(this->m_corrupter->storage);

    // check at the position < cellSize
    EXPECT_FALSE(this->m_pool->isAllocated(ptr1 + 1));

    // check at the position > cellSize
    EXPECT_FALSE(this->m_pool->isAllocated(ptr1 + cellSize + 1));

    // check at very end
    const U8* lastCell = ptr1 + storageSize - cellSize;
    EXPECT_FALSE(this->m_pool->isAllocated(lastCell + 1));

    // check after very end
    const U8* end = ptr1 + storageSize;
    EXPECT_FALSE(this->m_pool->isAllocated(end));
}

TYPED_TEST(PoolTest, AlignmentTest)
{
    for (std::size_t i = 0U; i < TypeParam::PoolSize::value; ++i)
    {
        void* ptr = this->allocate();
        EXPECT_TRUE(reinterpret_cast<std::size_t>(ptr) % sizeof(void*) == 0);
    }
}

TYPED_TEST(PoolTest, CheckPool)
{
    EXPECT_TRUE(this->m_pool->checkPool());
}

TYPED_TEST(PoolTest, CheckNotFullPool)
{
    this->fillUpPool(TypeParam::PoolSize::value/2);

    EXPECT_TRUE(this->m_pool->checkPool());
}

TYPED_TEST(PoolTest, CheckFullPool)
{
    this->fillUpPool();

    EXPECT_TRUE(this->m_pool->checkPool());
}
