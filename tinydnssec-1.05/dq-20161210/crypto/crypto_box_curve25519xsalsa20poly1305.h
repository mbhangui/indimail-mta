#ifndef crypto_box_curve25519xsalsa20poly1305_H
#define crypto_box_curve25519xsalsa20poly1305_H

#define crypto_box_curve25519xsalsa20poly1305_tinynacl_PUBLICKEYBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_tinynacl_SECRETKEYBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_tinynacl_BEFORENMBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_tinynacl_NONCEBYTES 24
#define crypto_box_curve25519xsalsa20poly1305_tinynacl_ZEROBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_tinynacl_BOXZEROBYTES 16
extern int crypto_box_curve25519xsalsa20poly1305_tinynacl(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tinynacl_open(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tinynacl_keypair(unsigned char *,unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tinynacl_beforenm(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tinynacl_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tinynacl_open_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

#define crypto_box_curve25519xsalsa20poly1305 crypto_box_curve25519xsalsa20poly1305_tinynacl
#define crypto_box_curve25519xsalsa20poly1305_open crypto_box_curve25519xsalsa20poly1305_tinynacl_open
#define crypto_box_curve25519xsalsa20poly1305_keypair crypto_box_curve25519xsalsa20poly1305_tinynacl_keypair
#define crypto_box_curve25519xsalsa20poly1305_beforenm crypto_box_curve25519xsalsa20poly1305_tinynacl_beforenm
#define crypto_box_curve25519xsalsa20poly1305_afternm crypto_box_curve25519xsalsa20poly1305_tinynacl_afternm
#define crypto_box_curve25519xsalsa20poly1305_open_afternm crypto_box_curve25519xsalsa20poly1305_tinynacl_open_afternm
#define crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_tinynacl_PUBLICKEYBYTES
#define crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_tinynacl_SECRETKEYBYTES
#define crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_tinynacl_BEFORENMBYTES
#define crypto_box_curve25519xsalsa20poly1305_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_tinynacl_NONCEBYTES
#define crypto_box_curve25519xsalsa20poly1305_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_tinynacl_ZEROBYTES
#define crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_tinynacl_BOXZEROBYTES
#define crypto_box_curve25519xsalsa20poly1305_IMPLEMENTATION "tinynacl"
#define crypto_box_curve25519xsalsa20poly1305_VERSION "-"

#endif
