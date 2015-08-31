#pragma once

#include <cstdint>

//----------------------------------------------------------------------------
// constexpr string hashing: md5

namespace cx
{
  // result of an md5 calculation - conventionally an md5 string is hex bytes
  // from least significant to most significant
  struct md5sum
  {
    uint64_t low;
    uint64_t high;
  };

  namespace detail
  {
    namespace md5
    {
      extern const char* md5_runtime_error;

      // shift amounts for the 4 rounds of the main function
      constexpr int r1shift[4] = { 7, 12, 17, 22 };
      constexpr int r2shift[4] = { 5, 9, 14, 20 };
      constexpr int r3shift[4] = { 4, 11, 16, 23 };
      constexpr int r4shift[4] = { 6, 10, 15, 21 };

      // magic constants for each round (actually the integer part of
      // abs(sin(i)) where i is the step number
      constexpr uint32_t r1const[16] =
      {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
      };
      constexpr uint32_t r2const[16] =
      {
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
      };
      constexpr uint32_t r3const[16] =
      {
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
      };
      constexpr uint32_t r4const[16] =
      {
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
      };

      // a context will keep the running value for the md5sum
      struct context
      {
        uint32_t a;
        uint32_t b;
        uint32_t c;
        uint32_t d;
        uint32_t buf[16];
      };

      // context utility functions: add, convert to sum
      constexpr context ctxadd(context c1, context c2)
      {
        return { c1.a + c2.a, c1.b + c2.b, c1.c + c2.c, c1.d + c2.d, {0} };
      }
      constexpr md5sum ctx2sum(context ctx)
      {
        return
        {
          (static_cast<uint64_t>(ctx.b) << 32) | static_cast<uint64_t>(ctx.a),
          (static_cast<uint64_t>(ctx.d) << 32) | static_cast<uint64_t>(ctx.c)
        };
      }

      // utility functions: string length, convert bytes to uint32_t
      constexpr uint64_t strlen(const char* s)
      {
        return *s == 0 ? 0 : 1 + strlen(s+1);
      }
      constexpr uint32_t word32le(const char* s)
      {
        return static_cast<uint32_t>(s[0])
          + (static_cast<uint32_t>(s[1]) << 8)
          + (static_cast<uint32_t>(s[2]) << 16)
          + (static_cast<uint32_t>(s[3]) << 24);
      }
      constexpr uint32_t word32le(const char* s, int len)
      {
        return
          (len > 0 ? static_cast<uint32_t>(s[0]) : 0)
          + (len > 1 ? (static_cast<uint32_t>(s[1]) << 8) : 0)
          + (len > 2 ? (static_cast<uint32_t>(s[2]) << 16) : 0)
          + (len > 3 ? (static_cast<uint32_t>(s[3]) << 24) : 0);
      }

      // the basic MD5 operations
      constexpr uint32_t F(uint32_t X, uint32_t Y, uint32_t Z)
      {
        return (X & Y) | (~X & Z);
      }
      constexpr uint32_t G(uint32_t X, uint32_t Y, uint32_t Z)
      {
        return (X & Z) | (Y & ~Z);
      }
      constexpr uint32_t H(uint32_t X, uint32_t Y, uint32_t Z)
      {
        return X ^ Y ^ Z;
      }
      constexpr uint32_t I(uint32_t X, uint32_t Y, uint32_t Z)
      {
        return Y ^ (X | ~Z);
      }
      constexpr uint32_t rotateL(uint32_t x, int n)
      {
        return (x << n) | (x >> (32-n));
      }
      constexpr uint32_t FF(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                            uint32_t x, int s, uint32_t ac)
      {
        return rotateL(a + F(b,c,d) + x + ac, s) + b;
      }
      constexpr uint32_t GG(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                            uint32_t x, int s, uint32_t ac)
      {
        return rotateL(a + G(b,c,d) + x + ac, s) + b;
      }
      constexpr uint32_t HH(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                            uint32_t x, int s, uint32_t ac)
      {
        return rotateL(a + H(b,c,d) + x + ac, s) + b;
      }
      constexpr uint32_t II(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                            uint32_t x, int s, uint32_t ac)
      {
        return rotateL(a + I(b,c,d) + x + ac, s) + b;
      }

      // initial context
      constexpr context init()
      {
        return { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, {0} };
      }
      // context from an existing context + buffer
      constexpr context init(context ctx, const char* buf)
      {
        return { ctx.a, ctx.b, ctx.c, ctx.d,
          { word32le(buf), word32le(buf+4), word32le(buf+8), word32le(buf+12),
            word32le(buf+16), word32le(buf+20), word32le(buf+24), word32le(buf+28),
            word32le(buf+32), word32le(buf+36), word32le(buf+40), word32le(buf+44),
            word32le(buf+48), word32le(buf+52), word32le(buf+56), word32le(buf+60) } };
      }

      // computing leftovers is messy: we need to pad the empty space to a
      // multiple of 64 bytes. the first pad byte is 0x80, the rest are 0.
      // the original length (in bits) is the last 8 bytes of padding.
      constexpr uint32_t pad(int len)
      {
        return len == 0 ? 0x00000080 :
          len == 1 ? 0x00008000 :
          len == 2 ? 0x00800000 :
          len == 3 ? 0x80000000 :
          0;
      }
      constexpr uint32_t origlenbytes(int origlen, int origlenpos)
      {
        return origlenpos == 0 ?
          static_cast<uint64_t>(origlen)*8 & 0xffffffff :
          origlenpos == -4 ?
          (static_cast<uint64_t>(origlen) >> 29) :
          0;
      }
      constexpr context leftover(context ctx, const char* buf,
                                 int len, int origlen, int origlenpos)
      {
        return { ctx.a, ctx.b, ctx.c, ctx.d,
          { word32le(buf, len) | pad(len) | origlenbytes(origlen, origlenpos),
            word32le(len >= 4 ? buf+4 : buf, len-4)
              | pad(len-4) | origlenbytes(origlen, origlenpos-4),
            word32le(len >= 8 ? buf+8 : buf, len-8)
              | pad(len-8) | origlenbytes(origlen, origlenpos-8),
            word32le(len >= 12 ? buf+12 : buf, len-12)
              | pad(len-12) | origlenbytes(origlen, origlenpos-12),
            word32le(len >= 16 ? buf+16 : buf, len-16)
              | pad(len-16) | origlenbytes(origlen, origlenpos-16),
            word32le(len >= 20 ? buf+20 : buf, len-20)
              | pad(len-20) | origlenbytes(origlen, origlenpos-20),
            word32le(len >= 24 ? buf+24 : buf, len-24)
              | pad(len-24) | origlenbytes(origlen, origlenpos-24),
            word32le(len >= 28 ? buf+28 : buf, len-28)
              | pad(len-28) | origlenbytes(origlen, origlenpos-28),
            word32le(len >= 32 ? buf+32 : buf, len-32)
              | pad(len-32) | origlenbytes(origlen, origlenpos-32),
            word32le(len >= 36 ? buf+36 : buf, len-36)
              | pad(len-36) | origlenbytes(origlen, origlenpos-36),
            word32le(len >= 40 ? buf+40 : buf, len-40)
              | pad(len-40) | origlenbytes(origlen, origlenpos-40),
            word32le(len >= 44 ? buf+44 : buf, len-44)
              | pad(len-44) | origlenbytes(origlen, origlenpos-44),
            word32le(len >= 48 ? buf+48 : buf, len-48)
              | pad(len-48) | origlenbytes(origlen, origlenpos-48),
            word32le(len >= 52 ? buf+52 : buf, len-52)
              | pad(len-52) | origlenbytes(origlen, origlenpos-52),
            word32le(len >= 56 ? buf+56 : buf, len-56)
              | pad(len-56) | origlenbytes(origlen, origlenpos-56),
            word32le(len >= 60 ? buf+60 : buf, len-60)
              | pad(len-60) | origlenbytes(origlen, origlenpos-60)} };
      }

      // compute a step of each round
      constexpr context round1step(context ctx, const uint32_t* block, int step)
      {
        return {
          FF(ctx.a, ctx.b, ctx.c, ctx.d, block[step], r1shift[step&3], r1const[step]),
            ctx.b, ctx.c, ctx.d, {0}
        };
      }
      constexpr context round2step(context ctx, const uint32_t* block, int step)
      {
        return {
          GG(ctx.a, ctx.b, ctx.c, ctx.d, block[(1+step*5)%16], r2shift[step&3], r2const[step]),
            ctx.b, ctx.c, ctx.d, {0}
        };
      }
      constexpr context round3step(context ctx, const uint32_t* block, int step)
      {
        return {
          HH(ctx.a, ctx.b, ctx.c, ctx.d, block[(5+step*3)%16], r3shift[step&3], r3const[step]),
            ctx.b, ctx.c, ctx.d, {0}
        };
      }
      constexpr context round4step(context ctx, const uint32_t* block, int step)
      {
        return {
          II(ctx.a, ctx.b, ctx.c, ctx.d, block[(step*7)%16], r4shift[step&3], r4const[step]),
            ctx.b, ctx.c, ctx.d, {0}
        };
      }

      // rotate contexts right and left (each round step does this)
      constexpr context rotateCR(context ctx, int n)
      {
        return n == 0 ? ctx :
          rotateCR({ ctx.d, ctx.a, ctx.b, ctx.c, {0} }, n-1);
      }
      constexpr context rotateCL(context ctx, int n)
      {
        return n == 0 ? ctx :
          rotateCL({ ctx.b, ctx.c, ctx.d, ctx.a, {0} }, n-1);
      }

      // the 4 rounds are each the result of recursively running the respective
      // round step (16 times for a block of 64 bytes)
      constexpr context round1(context ctx, const uint32_t* msg, int n)
      {
        return n == 0 ? round1step(ctx, msg, n) :
          rotateCL(round1step(rotateCR(round1(ctx, msg, n-1), n), msg, n), n);
      }
      constexpr context round2(context ctx, const uint32_t* msg, int n)
      {
        return n == 0 ? round2step(ctx, msg, n) :
          rotateCL(round2step(rotateCR(round2(ctx, msg, n-1), n), msg, n), n);
      }
      constexpr context round3(context ctx, const uint32_t* msg, int n)
      {
        return n == 0 ? round3step(ctx, msg, n) :
          rotateCL(round3step(rotateCR(round3(ctx, msg, n-1), n), msg, n), n);
      }
      constexpr context round4(context ctx, const uint32_t* msg, int n)
      {
        return n == 0 ? round4step(ctx, msg, n) :
          rotateCL(round4step(rotateCR(round4(ctx, msg, n-1), n), msg, n), n);
      }

      // the complete transform, for a message that is a multiple of 64 bytes
      constexpr context md5transform(context ctx)
      {
        return ctxadd(ctx,
                      round4(
                          round3(
                              round2(
                                  round1(ctx, ctx.buf, 15),
                                  ctx.buf, 15),
                              ctx.buf, 15),
                          ctx.buf, 15));
      }

      // three conditions:
      // 1. as long as we have a 64-byte block to do, we'll recurse on that
      // 2. when we have 56 bytes or more, we need to do a whole empty block to
      //    fit the 8 bytes of length after padding
      // 3. otherwise we have a block that will fit both padding and the length
      constexpr context md5update(context ctx, const char* msg, int len, int origlen)
      {
        return
          len >= 64 ?
          md5update(md5transform(init(ctx, msg)), msg+64, len-64, origlen) :
          len >= 56 ?
          md5update(md5transform(leftover(ctx, msg, len, origlen, 100)), msg+len, -100, origlen) :
          md5transform(leftover(ctx, msg, len, origlen, 56));
      }

      constexpr md5sum md5(const char* msg)
      {
        return ctx2sum(md5update(init(), msg, strlen(msg), strlen(msg)));
      }
    }
  }
  constexpr md5sum md5(const char* s)
  {
    return true ? detail::md5::md5(s) :
      throw detail::md5::md5_runtime_error;
  }
}
