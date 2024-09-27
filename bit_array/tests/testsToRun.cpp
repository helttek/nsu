#include </home/helttek/CLionProjects/test0/tests/googletest-main/googletest/include/gtest/gtest.h>
#include </home/helttek/CLionProjects/test0/bitarray.h>

TEST(emptConstr, test1) {
    BitArray a;
    EXPECT_EQ(a.to_string(), "");
}

TEST(secondConstr, test1) {
    BitArray a(5);
    EXPECT_EQ(a.to_string(), "00000");
    BitArray b(8);
    EXPECT_EQ(b.to_string(), "00000000");
    BitArray c(5, 5);
    EXPECT_EQ(c.to_string(), "00000");
    BitArray d(8, 5);
    EXPECT_EQ(d.to_string(), "00000000");
    BitArray e(64, 5);
    EXPECT_EQ(e.to_string(), "0000000000000000000000000000000000000000000000000000000000000101");
    BitArray f(67, 5);
    EXPECT_EQ(f.to_string(), "0000000000000000000000000000000000000000000000000000000000000101000");

    EXPECT_THROW(BitArray g(-23), std::length_error);
    EXPECT_THROW(BitArray h(-24, 1), std::length_error);
}

TEST(copyConstr, test1) {
    BitArray f(67, 5);
    EXPECT_EQ(f.to_string(), "0000000000000000000000000000000000000000000000000000000000000101000");
    BitArray f1(f);
    EXPECT_EQ(f.to_string(), f1.to_string());
}

TEST(swap, test1) {
    BitArray a1(64, 5), a2(64, 3);
    a2.swap(a1);
    EXPECT_EQ(a2.to_string(), "0000000000000000000000000000000000000000000000000000000000000101");
    EXPECT_EQ(a1.to_string(), "0000000000000000000000000000000000000000000000000000000000000011");
    BitArray a3(64, 5), a4(67, 3);
    a4.swap(a3);
    EXPECT_EQ(a4.to_string(), "0000000000000000000000000000000000000000000000000000000000000101");
    EXPECT_EQ(a3.to_string(), "0000000000000000000000000000000000000000000000000000000000000011000");
    BitArray a5(67, 5), a6(64, 3);
    a6.swap(a5);
    EXPECT_EQ(a6.to_string(), "0000000000000000000000000000000000000000000000000000000000000101000");
    EXPECT_EQ(a5.to_string(), "0000000000000000000000000000000000000000000000000000000000000011");
    BitArray a7(67, 5), a8(67, 3);
    a8.swap(a7);
    EXPECT_EQ(a8.to_string(), "0000000000000000000000000000000000000000000000000000000000000101000");
    EXPECT_EQ(a7.to_string(), "0000000000000000000000000000000000000000000000000000000000000011000");
}

TEST(opEquals, test1) {
    BitArray a, b(64, 255);
    a = b;
    EXPECT_EQ(a.to_string(), b.to_string());
    BitArray a1(5), b1(64, 255);
    a1 = b1;
    EXPECT_EQ(a1.to_string(), b1.to_string());
    BitArray a2(68, 1), b2(64, 255);
    a2 = b2;
    EXPECT_EQ(a2.to_string(), b2.to_string());
    BitArray a3, b3(67, 255);
    a3 = b3;
    EXPECT_EQ(a3.to_string(), b3.to_string());
    BitArray a4(5), b4(67, 255);
    a4 = b4;
    EXPECT_EQ(a4.to_string(), b4.to_string());
    BitArray a5(68, 1), b5(67, 255);
    a5 = b5;
    EXPECT_EQ(a5.to_string(), b5.to_string());
}

TEST(resize, test1) {
    BitArray a1;
    a1.resize(4);
    EXPECT_EQ(a1.to_string(), "0000");
    a1.resize(2);
    EXPECT_EQ(a1.to_string(), "00");
    a1.resize(5, true);
    EXPECT_EQ(a1.to_string(), "00111");
}

TEST(clear, test1) {
    BitArray a, b(69, 232);
    a.clear();
    EXPECT_EQ(a.to_string(), "");
    b.clear();
    EXPECT_EQ(b.to_string(), "");
}

TEST(pushBack, test1) {
    BitArray a;
    a.push_back(0);
    EXPECT_EQ(a.to_string(), "0");
    a.push_back(1);
    EXPECT_EQ(a.to_string(), "01");
    a.resize(8, 0);
    a.push_back(1);
    EXPECT_EQ(a.to_string(), "010000001");
}

TEST(opAnd, test1) {
    BitArray a(64, 129), b(64, 255);
    b &= a;
    EXPECT_EQ(b[56], true);
    EXPECT_EQ(b[60], false);
    EXPECT_EQ(b[63], true);

    BitArray a1(67, 129), b1(64, 255);
    b1 &= a1;
    EXPECT_EQ(b1[56], true);
    EXPECT_EQ(b1[60], false);
    EXPECT_EQ(b1[63], true);

    BitArray a2(64, 129), b2(62, 255);
    b2 &= a2;
    EXPECT_EQ(b2[56], true);
    EXPECT_EQ(b2[60], false);
}

TEST(opOr, test1) {
    BitArray a(64, 129), b(64, 0);
    b |= a;
    EXPECT_EQ(b[56], true);
    EXPECT_EQ(b[60], false);
    EXPECT_EQ(b[63], true);

    BitArray a1(67, 129), b1(64, 0);
    b1 |= a1;
    EXPECT_EQ(b1[56], true);
    EXPECT_EQ(b1[60], false);
    EXPECT_EQ(b1[63], true);

    BitArray a2(64, 129), b2(62, 0);
    b2 |= a2;
    EXPECT_EQ(b2[56], true);
    EXPECT_EQ(b2[60], false);
}

TEST(opXor, test1) {
    BitArray a(64, 129), b(64, 255);
    b ^= a;
    EXPECT_EQ(b[56], false);
    EXPECT_EQ(b[60], true);
    EXPECT_EQ(b[63], false);

    BitArray a1(67, 129), b1(64, 255);
    b1 ^= a1;
    EXPECT_EQ(b1[56], false);
    EXPECT_EQ(b1[60], true);
    EXPECT_EQ(b1[63], false);

    BitArray a2(64, 129), b2(62, 255);
    b2 ^= a2;
    EXPECT_EQ(b2[56], false);
    EXPECT_EQ(b2[60], true);
}

TEST(leftShiftAndAssign, test1) {
    BitArray a1(64, 255);
    a1 <<= 1;
    EXPECT_EQ(a1.to_string(), "0000000000000000000000000000000000000000000000000000000111111110");
    a1 <<= 3;
    EXPECT_EQ(a1.to_string(), "0000000000000000000000000000000000000000000000000000111111110000");
    a1 <<= 11;
    EXPECT_EQ(a1.to_string(), "0000000000000000000000000000000000000000011111111000000000000000");
    BitArray a3(67, 255);
    a3 <<= 1;
    EXPECT_EQ(a3.to_string(), "0000000000000000000000000000000000000000000000000000000111111110000");
    a3 <<= 3;
    EXPECT_EQ(a3.to_string(), "0000000000000000000000000000000000000000000000000000111111110000000");
    a3 <<= 11;
    EXPECT_EQ(a3.to_string(), "0000000000000000000000000000000000000000011111111000000000000000000");
    BitArray a2(64,UINT64_MAX);
    a2 <<= 64;
    EXPECT_EQ(a2.to_string(), "0000000000000000000000000000000000000000000000000000000000000000");
    EXPECT_THROW(a1 <<= -1, std::underflow_error);
}

TEST(rightShiftAndAssign, test1) {
    BitArray a1(64, UINT64_MAX);
    a1 >>= 1;
    EXPECT_EQ(a1.to_string(), "0111111111111111111111111111111111111111111111111111111111111111");
    a1 >>= 3;
    EXPECT_EQ(a1.to_string(), "0000111111111111111111111111111111111111111111111111111111111111");
    a1 >>= 11;
    EXPECT_EQ(a1.to_string(), "0000000000000001111111111111111111111111111111111111111111111111");
    BitArray a3(67, UINT64_MAX);
    a3 >>= 1;
    EXPECT_EQ(a3.to_string(), "0111111111111111111111111111111111111111111111111111111111111111100");
    a3 >>= 3;
    EXPECT_EQ(a3.to_string(), "0000111111111111111111111111111111111111111111111111111111111111111");
    a3 >>= 11;
    EXPECT_EQ(a3.to_string(), "0000000000000001111111111111111111111111111111111111111111111111111");
    BitArray a2(64,UINT64_MAX);
    a2 >>= 67;
    EXPECT_EQ(a2.to_string(), "0000000000000000000000000000000000000000000000000000000000000000");
    EXPECT_THROW(a2 >>= -3, std::underflow_error);
}

TEST(leftShift, test1) {
    BitArray a1(64, 255);
    EXPECT_EQ((a1<<1).to_string(), "0000000000000000000000000000000000000000000000000000000111111110");
    EXPECT_EQ((a1<<3).to_string(), "0000000000000000000000000000000000000000000000000000011111111000");
    EXPECT_EQ((a1<<11).to_string(), "0000000000000000000000000000000000000000000001111111100000000000");
    BitArray a3(67, 255);
    EXPECT_EQ((a3<<1).to_string(), "0000000000000000000000000000000000000000000000000000000111111110000");
    EXPECT_EQ((a3<<3).to_string(), "0000000000000000000000000000000000000000000000000000011111111000000");
    EXPECT_EQ((a3<<11).to_string(), "0000000000000000000000000000000000000000000001111111100000000000000");
    BitArray a2(64,UINT64_MAX);
    EXPECT_EQ((a2<<64).to_string(), "0000000000000000000000000000000000000000000000000000000000000000");
}

TEST(rightShift, test1) {
    BitArray a1(64, UINT64_MAX);
    EXPECT_EQ((a1>>1).to_string(), "0111111111111111111111111111111111111111111111111111111111111111");
    EXPECT_EQ((a1>>3).to_string(), "0001111111111111111111111111111111111111111111111111111111111111");
    EXPECT_EQ((a1>>11).to_string(), "0000000000011111111111111111111111111111111111111111111111111111");
    BitArray a3(67, UINT64_MAX);
    EXPECT_EQ((a3>>1).to_string(), "0111111111111111111111111111111111111111111111111111111111111111100");
    EXPECT_EQ((a3>>3).to_string(), "0001111111111111111111111111111111111111111111111111111111111111111");
    EXPECT_EQ((a3>>11).to_string(), "0000000000011111111111111111111111111111111111111111111111111111111");
    BitArray a2(64,UINT64_MAX);
    EXPECT_EQ((a2>>67).to_string(), "0000000000000000000000000000000000000000000000000000000000000000");
}

TEST(set, test1) {
    BitArray a(16);
    a.set(2);
    EXPECT_EQ(a.to_string(), "0010000000000000");
    a.set(7);
    EXPECT_EQ(a.to_string(), "0010000100000000");
    a.set(11);
    EXPECT_EQ(a.to_string(), "0010000100010000");
    a.set(7, false);
    EXPECT_EQ(a.to_string(), "0010000000010000");
    a.set(11, false);
    EXPECT_EQ(a.to_string(), "0010000000000000");
    BitArray a1(18);
    a1.set(2);
    EXPECT_EQ(a1.to_string(), "001000000000000000");
    a1.set(7);
    EXPECT_EQ(a1.to_string(), "001000010000000000");
    a1.set(11);
    EXPECT_EQ(a1.to_string(), "001000010001000000");
    a1.set(7, false);
    EXPECT_EQ(a1.to_string(), "001000000001000000");
    a1.set(11, false);
    EXPECT_EQ(a1.to_string(), "001000000000000000");
    EXPECT_THROW(a1.set(-4), std::underflow_error);
}

TEST(setAll, test1) {
    BitArray a(13), b(16), c(2);
    a.set();
    b.set();
    c.set();
    EXPECT_EQ(a.to_string(), "1111111111111");
    EXPECT_EQ(b.to_string(), "1111111111111111");
    EXPECT_EQ(c.to_string(), "11");
}

TEST(reset, test1) {
    BitArray a(13, UINT64_MAX), b(16,UINT64_MAX), c(2,UINT64_MAX);
    a.reset(3);
    EXPECT_EQ(a.to_string(), "1110111111111");
    a.reset(7);
    EXPECT_EQ(a.to_string(), "1110111011111");
    a.reset(9);
    EXPECT_EQ(a.to_string(), "1110111010111");
    b.reset(3);
    EXPECT_EQ(b.to_string(), "1110111111111111");
    b.reset(7);
    EXPECT_EQ(b.to_string(), "1110111011111111");
    b.reset(9);
    EXPECT_EQ(b.to_string(), "1110111010111111");
    c.reset(1);
    EXPECT_EQ(c.to_string(), "10");
    c.reset(0);
    EXPECT_EQ(c.to_string(), "00");
    EXPECT_THROW(a.reset(-5), std::underflow_error);
}

TEST(resetAll, test1) {
    BitArray a(13,UINT64_MAX), b(16,UINT64_MAX), c(2,UINT64_MAX);
    a.reset();
    b.reset();
    c.reset();
    EXPECT_EQ(a.to_string(), "0000000000000");
    EXPECT_EQ(b.to_string(), "0000000000000000");
    EXPECT_EQ(c.to_string(), "00");
}

TEST(any, test1) {
    BitArray a(3,UINT64_MAX), b(8,UINT64_MAX), c(13,UINT64_MAX);
    EXPECT_EQ(a.any(), true);
    EXPECT_EQ(b.any(), true);
    EXPECT_EQ(c.any(), true);
    a.reset();
    b.reset();
    c.reset();
    EXPECT_EQ(a.any(), false);
    EXPECT_EQ(b.any(), false);
    EXPECT_EQ(c.any(), false);
}

TEST(none, test1) {
    BitArray a(3), b(8), c(13);
    EXPECT_EQ(a.none(), true);
    EXPECT_EQ(b.none(), true);
    EXPECT_EQ(c.none(), true);
    a.set();
    b.set();
    c.set();
    EXPECT_EQ(a.none(), false);
    EXPECT_EQ(b.none(), false);
    EXPECT_EQ(c.none(), false);
}

TEST(bitInverse, test1) {
    BitArray a(6), b(8), c(11);
    a.set(2);
    a.set(4);
    b.set(2);
    b.set(4);
    b.set(7);
    c.set(2);
    c.set(4);
    c.set(7);
    c.set(9);
    BitArray t = ~a;
    EXPECT_EQ(t.to_string(), "110101");
    t = ~b;
    EXPECT_EQ(t.to_string(), "11010110");
    t = ~c;
    EXPECT_EQ(t.to_string(), "11010110101");
}

TEST(count, test1) {
    BitArray a(6), b(8), c(11);
    a.set(2);
    a.set(4);
    b.set(2);
    b.set(4);
    b.set(7);
    c.set(2);
    c.set(4);
    c.set(7);
    c.set(9);
    EXPECT_EQ(a.count(), 2);
    EXPECT_EQ(b.count(), 3);
    EXPECT_EQ(c.count(), 4);
}

TEST(brackets, test1) {
    BitArray a(6), b(8), c(11);
    a.set(2);
    a.set(4);
    b.set(2);
    b.set(4);
    b.set(7);
    c.set(2);
    c.set(4);
    c.set(7);
    c.set(9);
    EXPECT_EQ(a[2], true);
    EXPECT_EQ(a[4], true);
    EXPECT_EQ(b[2], true);
    EXPECT_EQ(b[4], true);
    EXPECT_EQ(b[7], true);
    EXPECT_EQ(c[2], true);
    EXPECT_EQ(c[4], true);
    EXPECT_EQ(c[7], true);
    EXPECT_EQ(c[9], true);
    EXPECT_EQ(c[0], false);
    EXPECT_EQ(b[1], false);
    EXPECT_EQ(a[5], false);
}

TEST(size, test1) {
    BitArray a(6), b(8), c(11);
    EXPECT_EQ(a.size(), 6);
    EXPECT_EQ(b.size(), 8);
    EXPECT_EQ(c.size(), 11);
}

TEST(empty, test1) {
    BitArray a, b(1);
    EXPECT_EQ(a.empty(), true);
    EXPECT_EQ(b.empty(), false);
}

TEST(equal, test1) {
    BitArray a, b, c(2), d(2);
    EXPECT_EQ(a==b, true);
    EXPECT_EQ(c==d, true);
    EXPECT_NE(a==c, true);
}

TEST(inequal, test1) {
    BitArray a, b, c(2), d(2);
    EXPECT_EQ(a!=b, false);
    EXPECT_EQ(c!=d, false);
    EXPECT_NE(a!=c, false);
}
