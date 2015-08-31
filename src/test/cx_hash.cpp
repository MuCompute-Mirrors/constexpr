#include <cx_fnv1.h>
#include <cx_md5.h>
#include <cx_murmur3.h>

#include <iostream>
using namespace std;

constexpr uint64_t endianswap(uint64_t x)
{
  return ((x & 0xff) << 56)
    | (((x >> 8) & 0xff) << 48)
    | (((x >> 16) & 0xff) << 40)
    | (((x >> 24) & 0xff) << 32)
    | (((x >> 32) & 0xff) << 24)
    | (((x >> 40) & 0xff) << 16)
    | (((x >> 48) & 0xff) << 8)
    | ((x >> 56) & 0xff);
}

void test_cx_hash()
{
  //----------------------------------------------------------------------------
  // FNV1 / FNV1A
  static_assert(cx::fnv1("hello, world") == 733686394982303293ull,
                "fnv1(\"hello, world\")");
  static_assert(cx::fnv1a("hello, world") == 1702823495152329533ull,
                "fnv1a(\"hello, world\")");

  //----------------------------------------------------------------------------
  // Murmur3
  static_assert(cx::murmur3_32("hello, world", 0) == 345750399,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world1", 0) == 3714214180,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world12", 0) == 83041023,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world123", 0) == 209220029,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world1234", 0) == 4241062699,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world", 1) == 1868346089,
                "murmur3(\"hello, world\")");

  //----------------------------------------------------------------------------
  // MD5
  constexpr const char* const md5tests[8] = {
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
    "hello, world",
  };

  constexpr cx::md5sum md5sums[8] =  {
    { 0xd41d8cd98f00b204, 0xe9800998ecf8427e },
    { 0x0cc175b9c0f1b6a8, 0x31c399e269772661 },
    { 0x900150983cd24fb0, 0xd6963f7d28e17f72 },
    { 0xf96b697d7cb7938d, 0x525a2f31aaf161d0 },
    { 0xc3fcd3d76192e400, 0x7dfb496cca67e13b },
    { 0xd174ab98d277d9f5, 0xa5611c2c9f419d9f },
    { 0x57edf4a22be3c955, 0xac49da2e2107b67a },
    { 0xe4d7f1b4ed2e42d1, 0x5898f4b27b019da4 }
  };

  static_assert(endianswap(cx::md5(md5tests[0]).low)  == md5sums[0].low &&
                endianswap(cx::md5(md5tests[0]).high) == md5sums[0].high,
                "md5(\"\")");
  static_assert(endianswap(cx::md5(md5tests[1]).low)  == md5sums[1].low &&
                endianswap(cx::md5(md5tests[1]).high) == md5sums[1].high,
                "md5(\"a\")");
  static_assert(endianswap(cx::md5(md5tests[2]).low)  == md5sums[2].low &&
                endianswap(cx::md5(md5tests[2]).high) == md5sums[2].high,
                "md5(\"abc\")");
  static_assert(endianswap(cx::md5(md5tests[3]).low)  == md5sums[3].low &&
                endianswap(cx::md5(md5tests[3]).high) == md5sums[3].high,
                "md5(\"message digest\")");
  static_assert(endianswap(cx::md5(md5tests[4]).low)  == md5sums[4].low &&
                endianswap(cx::md5(md5tests[4]).high) == md5sums[4].high,
                "md5(\"abcdefghijklmnopqrstuvwxyz\")");
  static_assert(endianswap(cx::md5(md5tests[5]).low)  == md5sums[5].low &&
                endianswap(cx::md5(md5tests[5]).high) == md5sums[5].high,
                "md5(\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\")");
  static_assert(endianswap(cx::md5(md5tests[6]).low)  == md5sums[6].low &&
                endianswap(cx::md5(md5tests[6]).high) == md5sums[6].high,
                "md5(\"12345678901234567890123456789012345678901234567890123456789012345678901234567890\")");
  static_assert(endianswap(cx::md5(md5tests[7]).low)  == md5sums[7].low &&
                endianswap(cx::md5(md5tests[7]).high) == md5sums[7].high,
                "md5(\"hello, world\")");

  constexpr auto silly = cx::md5(
#include <helloworld>
  );
  static_assert(endianswap(silly.low)  == md5sums[7].low &&
                endianswap(silly.high) == md5sums[7].high,
                "md5(#include<helloworld>)");
}