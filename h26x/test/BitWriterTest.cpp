//
// Created by dburc on 11/17/2024.
//
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "../include/BufferStream.h"
#include "BitStream.h"
#include "ExpGolomb.h"

using namespace h26x;

using bytes = std::vector<uint8_t>;
//------------------------------------------------------------------------------
TEST(bitwriterTest, InitialState) {
    std::vector<uint8_t> buffer(0);

    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);

    EXPECT_EQ(0, w.position());
}

//------------------------------------------------------------------------------
TEST(bitwriterTest, WriteLessThanBufferNoOffset)
{
    std::vector<uint8_t> buffer(1);
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);

    ASSERT_NO_THROW(w.set(0xFD, 3));
    EXPECT_EQ(3, w.position());
    w.flush();
    EXPECT_EQ(bytes({0xA0}), buffer);
}

//------------------------------------------------------------------------------
TEST(bitwriterTest, WriteLessThanBufferWithOffset)
{
    std::vector<uint8_t> buffer(2);
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);
    ASSERT_NO_THROW(w.set(0xFF, 3));
    EXPECT_EQ(3, w.position());
    w.flush();
    EXPECT_EQ(bytes({0xE0, 0x00}), buffer);

    ASSERT_NO_THROW(w.set(0b1010, 4));
    EXPECT_EQ(7, w.position());
    w.flush();
    EXPECT_EQ(bytes({0xF4, 0x00}), buffer);
}
//------------------------------------------------------------------------------
TEST(bitwriterTest, WriteExactBufferNoOffset)
{
    std::vector<uint8_t> buffer(1);
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);

    ASSERT_NO_THROW(w.set(0x57, 8));
    EXPECT_EQ(8, w.position());
    EXPECT_EQ(bytes({0x57}), buffer);
}
//------------------------------------------------------------------------------
TEST(bitwriterTest, WriteExactBufferWithOffset)
{
    std::vector<uint8_t> buffer(1);
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);

    ASSERT_NO_THROW(w.set(0xFF, 3));
    EXPECT_EQ(3, w.position());
    w.flush();
    EXPECT_EQ(bytes({0xE0}), buffer);

    ASSERT_NO_THROW(w.set(0b1010, 4));
    EXPECT_EQ(7, w.position());
    w.flush();
    EXPECT_EQ(bytes({0xF4}), buffer);
}
//------------------------------------------------------------------------------
TEST(bitwriterTest, WriteMoreThanTwoBuffers)
{
    std::vector<uint8_t> buffer(4);
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);

    w.set(6, 4);
    EXPECT_EQ(4, w.position());

    w.set(0x112234, 24);
    EXPECT_EQ(28, w.position());
    w.flush();
    EXPECT_EQ(bytes({0x61, 0x12, 0x23, 0x40}), buffer);
}
//------------------------------------------------------------------------------
TEST(bitwriterTest, TestSkip)
{
    std::vector<uint8_t> buffer(3);
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);

    w.set(6, 4);
    w.skip(13);
    EXPECT_EQ(17, w.position());
    w.set(0b111, 3);
    EXPECT_EQ(20, w.position());
    w.flush();
    EXPECT_EQ(bytes({0x60, 0x00, 0x70}), buffer);
}
//------------------------------------------------------------------------------
TEST(bitwriterTest, WriteFromVar)
{
    std::vector<uint8_t> buffer(3);
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);
    uint8_t v = 0x42;
    w.set(v, 8);
    w.set(false);
    w.set(false);
    w.set(false);
    w.set(false);
    w.set(false);
    w.set(false);
    w.set(0, 2);
    w.set(30, 8);
    w.flush();
    EXPECT_EQ(bytes({0x42, 0x00, 0x1E}), buffer);
}
//------------------------------------------------------------------------------
TEST(bitwriterTest, SetExpGolomb)
{
    std::vector<uint8_t> buffer(3);
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream w(&rwbs);
    ExpGolomb::set(5, &w);
    EXPECT_EQ(5, w.position());
    ExpGolomb::setSigned(-42, &w);
    EXPECT_TRUE(w.isOk());
    w.flush();

    BufferStream robs(buffer.data(), buffer.capacity());
    BitStream r(&robs);
    EXPECT_EQ(5, ExpGolomb::get(&r));
    EXPECT_EQ(-42, ExpGolomb::getSigned(&r));
}