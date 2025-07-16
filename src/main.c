#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"

#include "elliptic.h"

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
  mpz_t k;
  mpz_init(k);
  mpz_set_ui(k, 7);
  elliptic_curve_point_t publicKey;
  ellipticCurvePointInit(&publicKey);

  ellipticCurveScalarMultiply(&publicKey, &curve, &curve.G, k);

  gmp_printf("Private key : %Zd\n", k);
  gmp_printf("Public key x : %Zd\n", publicKey.x.value);
  gmp_printf("Public key y : %Zd\n", publicKey.y.value);

  return 0;
}
