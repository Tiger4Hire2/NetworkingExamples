// GTEST module
#include "protobuf.h"

namespace testing { 
    namespace internal
    {
        inline bool operator==(ConstDataBlock a, ConstDataBlock b)
        {
            return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b));
        }
        std::ostream& operator<<(std::ostream& os, const std::byte val)
        {
            os << (int)val;
            return os;
        }
        std::ostream& operator<<(std::ostream& os, ConstDataBlock val)
        {
            os << "[" << val.size() << "]{" << std::hex;
            bool first = true;
            for (const auto v:val)
            {
                os << (first?"":",")<<(int)v;
                first = false;
            }
            os << "}";
            return os;
        }
    }
}
#include <gtest/gtest.h>

TEST(ProtoBuf, BytePush)
{
    DataBlock tgt;
    std::byte test_bytes[] = {std::byte{0xFF}, std::byte{0x00}, std::byte{0x40}, std::byte{0x20}, 
                                std::byte{0x10}, std::byte{0x08}, std::byte{0x04}, std::byte{0x02}, std::byte{0x01} };
    for(const auto v: test_bytes)
        tgt << v;
    EXPECT_EQ(ConstDataBlock{test_bytes}, as_const(tgt));
}

TEST(ProtoBuf, VarInt)
{
    std::byte test_0_bytes[] = {std::byte{0x00}};
    std::byte test_1_bytes[] = {std::byte{0x01}};
    std::byte test_150_bytes[] = {std::byte{0x96}, std::byte{0x01} };
    std::byte test_300_bytes[] = {std::byte{0xAC}, std::byte{0x02} };
    std::byte test_neg1_bytes[] = {std::byte{0xFF}, std::byte{0xFF} , std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
                                    std::byte{0xFF}, std::byte{0xFF} , std::byte{0xFF}, std::byte{0xFF}, std::byte{0x01}};
    std::vector<std::pair<ConstDataBlock, int>> test_pairs = { {test_0_bytes, 0}, {test_1_bytes, 1},
                                                            {test_150_bytes, 150}, {test_neg1_bytes, -1}, {test_300_bytes, 300} };
    for (const auto& pair : test_pairs)
    {
        DataBlock tgt;
        WriteAsVarint(tgt, pair.second);
        EXPECT_EQ(ConstDataBlock{pair.first}, as_const(tgt));
    }
}

TEST(ProtoBuf, SignedVarInt)
{
    std::byte test_0_bytes[] = {std::byte{0x00}};
    std::byte test_neg1_bytes[] = {std::byte{0x01}};
    std::byte test_1_bytes[] = {std::byte{0x02}};
    std::byte test_negmax_bytes[] = {std::byte{0xFF}, std::byte{0xFF} , std::byte{0xFF}, std::byte{0xFF}, std::byte{0x0F}};
    std::byte test_max_bytes[] = {std::byte{0xFE}, std::byte{0xFF} , std::byte{0xFF}, std::byte{0xFF},  std::byte{0x0F}};
    std::vector<std::pair<ConstDataBlock, int>> test_pairs = { {test_0_bytes, 0}, {test_neg1_bytes, -1}, {test_1_bytes, 1},
                                     {test_negmax_bytes, -2147483648}, {test_max_bytes, 2147483647} };
    for (const auto& pair : test_pairs)
    {
        using namespace testing::internal;
        DataBlock tgt;
        WriteAsSignedVarint32(tgt, pair.second);
        std::cout << as_const(tgt)<<"\n";

        EXPECT_EQ(ConstDataBlock{pair.first}, as_const(tgt)) << "encoding:" << std::dec << pair.second << "(" << std::hex << pair.second << ")";
    }
}
