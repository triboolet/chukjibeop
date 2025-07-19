#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"

#include "crypto.h"
#include "elliptic.h"

int generateBitcoinAddress(unsigned char *resultingAddress,
                           unsigned char *publicKey, unsigned char versionByte);

void print_hex(const unsigned char *data, size_t len) {
  for (size_t i = 0; i < len; i++)
    printf("%02x", data[i]);
  printf("\n");
}

int main() {
  // secp256k1
  elliptic_curve_t curve;
  mpz_init_set_str(
      curve.p,
      "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 16);
  mpz_init_set_ui(curve.a, 0);
  mpz_init_set_ui(curve.b, 7);
  mpz_init_set_str(
      curve.n,
      "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141", 16);
  mpz_init_set_ui(curve.h, 1);

  // Base point G
  mpz_init_set_str(
      curve.G.x.value,
      "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798", 16);
  mpz_init_set_str(
      curve.G.y.value,
      "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8", 16);

  mpz_init_set(curve.G.x.p, curve.p);
  mpz_init_set(curve.G.y.p, curve.p);

  curve.G.infinity = 0;

  // key pair
  mpz_t privateKey;
  mpz_init(privateKey);
  mpz_set_ui(privateKey, 6);
  elliptic_curve_point_t publicKey;
  ellipticCurvePointInit(&publicKey);

  ellipticCurveScalarMultiply(&publicKey, &curve, &curve.G, privateKey);

  gmp_printf("Private key : %Zd\n", privateKey);
  gmp_printf("Public key x : %Zd\n", publicKey.x.value);
  gmp_printf("Public key y : %Zd\n", publicKey.y.value);

  // hex

  unsigned char publicKeyCompressedForm[33];
  getPublicKeyCompressedForm(&publicKeyCompressedForm, &publicKey);
  printf("Public key (compressed, hex) : ");
  print_hex(publicKeyCompressedForm, 33);

  // btc adress
  unsigned char resultingAddress[RIPEMD160_DIGEST_LENGTH + 5];
  generateBitcoinAddress(resultingAddress, publicKeyCompressedForm, 0x6f);
  printf("Address : ");
  print_hex(resultingAddress, sizeof(resultingAddress));

  return 0;
}
