#ifndef MD5__H
#define MD5__H

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac) \
          do { \
          a += F(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a, (unsigned)s); \
          a += b; \
          } while(0)

#define GG(a,b,c,d,x,s,ac) \
          do { \
          a += G(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a, (unsigned)s); \
          a += b; \
          } while(0)

#define HH(a,b,c,d,x,s,ac) \
          do { \
          a += H(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a, (unsigned)s); \
          a += b; \
          } while(0)

#define II(a,b,c,d,x,s,ac) \
          do { \
          a += I(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a, (unsigned)s); \
          a += b; \
          } while(0)

#endif //MD5__H
