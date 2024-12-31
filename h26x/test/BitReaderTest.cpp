//
// Created by dburc on 11/16/2024.
//
#include <gtest/gtest.h>
#include "../include/BufferStream.h"
#include "../include/BitStream.h"
#include "ExpGolomb.h"

using namespace h26x;
//------------------------------------------------------------------------------
TEST(bitstreamTest, setData)
{
    const uint8_t data[] = {0xFF, 0x11, 0x22, 0x33};
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);

    EXPECT_EQ(0, br.position());
    EXPECT_EQ(32, br.available());
}
//------------------------------------------------------------------------------
TEST(bitstreamTest, read_aligned_nonfull)
{
    const uint8_t data[] = {0xFF, 0x11, 0x22, 0x33};
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);

    EXPECT_EQ(0xFF, br.get<uint8_t>(8));
    EXPECT_EQ(8, br.position());
    EXPECT_EQ(24, br.available());
}
//------------------------------------------------------------------------------
TEST(bitstreamTest, read_aligned_full)
{
    const uint8_t data[] = {0xFF, 0x11, 0x22, 0x33};
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);

    EXPECT_EQ(0xFF112233, br.get<uint32_t>(32));
    EXPECT_EQ(32, br.position());
    EXPECT_EQ(0, br.available());
}

//------------------------------------------------------------------------------
TEST(bitstreamTest, read_nonaligned_nonfull)
{
    const uint8_t data[] = {0xFF, 0x11, 0x22, 0x33};
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);

    EXPECT_EQ(0xF, br.get<uint8_t>(4));
    EXPECT_EQ(4, br.position());
    EXPECT_EQ(28, br.available());
    EXPECT_EQ(0xF11223, br.get<uint32_t>(24));
    EXPECT_EQ(28, br.position());
    EXPECT_EQ(4, br.available());
    EXPECT_EQ(0x3, br.get<uint8_t>(4));
    EXPECT_EQ(32, br.position());
    EXPECT_EQ(0, br.available());
}
//------------------------------------------------------------------------------
TEST(bitstreamTest, read_aligned_64_cross)
{
    const uint8_t data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);

    EXPECT_EQ(0x11, br.get<uint8_t>(8));
    EXPECT_EQ(8, br.position());
    EXPECT_EQ(64, br.available());
    EXPECT_EQ(0x2233445566778899, br.get<uint64_t>(64));
    EXPECT_EQ(72, br.position());
    EXPECT_EQ(0, br.available());
}

//------------------------------------------------------------------------------
TEST(bitstreamTest, read_nonaligned_64_cross)
{
    const uint8_t data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);
    EXPECT_EQ(0x01, br.get<uint8_t>(4));
    EXPECT_EQ(4, br.position());
    EXPECT_EQ(68, br.available());
    EXPECT_EQ(0x1223344556677889, br.get<uint64_t>(64));
    EXPECT_EQ(68, br.position());
    EXPECT_EQ(4, br.available());
    EXPECT_EQ(0x09, br.get<uint16_t>(4));
    EXPECT_EQ(72, br.position());
    EXPECT_EQ(0, br.available());
}
//------------------------------------------------------------------------------
TEST(bitstreamTest, exp_golomb_k0)
{
    const uint8_t data[] = {0b1'010'011'0, 0b0100'0000 };
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);

    EXPECT_EQ(0, ExpGolomb::get(&br));
    EXPECT_EQ(1, ExpGolomb::get(&br));
    EXPECT_EQ(2, ExpGolomb::get(&br));
    EXPECT_EQ(3, ExpGolomb::get(&br));
}
//------------------------------------------------------------------------------
TEST(bitstreamTest, exp_golomb_signed)
{
    const uint8_t data[] = {0b00111'010};
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);

    EXPECT_EQ(-3, ExpGolomb::getSigned(&br));
    EXPECT_EQ(1, ExpGolomb::getSigned(&br));
}
//------------------------------------------------------------------------------
TEST(bitstreamTest, overread)
{
    const uint8_t data[] = {0xFF, 0x11, 0x22, 0x33};
    BufferStream bs(data, sizeof(data));
    BitStream br(&bs);
    EXPECT_EQ(0xFF112233, br.get<uint32_t>(32));
    EXPECT_TRUE(br.isOk());
    EXPECT_EQ(0, br.get<uint32_t>(32));
    EXPECT_FALSE(br.isOk());
}

