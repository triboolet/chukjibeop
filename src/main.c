#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"

#include "elliptic.h"

int main() {
  /*
  elliptic_curve_t ell;
  mpz_init_set_ui(ell.p, 17);
  mpz_init_set_ui(ell.a, 3);
  mpz_init_set_ui(ell.b, 2);

  elliptic_curve_point_t G;
  mpz_init_set_ui(G.x.value, 3);
  mpz_init_set_ui(G.x.p, 17);
  mpz_init_set_ui(G.y.value, 2);
  mpz_init_set_ui(G.y.p, 17);
  G.infinity = 0;
  ell.G = G;

  field_element_t res;
  mpz_init(res.value);
  computePointCurve(&res, &ell, &ell.G);
  gmp_printf("res = %Zd\n", res.value);
  mpz_clear(res.value);

  int isOnCurve = pointIsOnCurve(&ell, &ell.G);
  printf("Is it on the curve ? %d\n", isOnCurve);

  elliptic_curve_point_t G2;
  mpz_init_set_ui(G2.x.value, 3);
  mpz_init_set_ui(G2.x.p, 17);
  mpz_init_set_ui(G2.y.value, 2);
  mpz_init_set_ui(G2.y.p, 17);

  printf("Equal ? %d\n", ellipticCurvePointEqual(&G, &G2));

  elliptic_curve_point_t added;
  mpz_inits(added.x.value, added.y.value, added.x.p, added.y.p, NULL);
  mpz_set(added.x.p, ell.p);
  mpz_set(added.y.p, ell.p);
  added.infinity = 0;

  ellipticCurvePointAdd(&added, &ell, &G, &G2);
  gmp_printf("added value x = %Zd\n", added.x.value);

  elliptic_curve_point_t multiplied;
  mpz_inits(multiplied.x.value, multiplied.y.value, multiplied.x.p,
            multiplied.y.p, NULL);
  mpz_set(multiplied.x.p, ell.p);
  mpz_set(multiplied.y.p, ell.p);
  mpz_t scalar;
  mpz_init_set_ui(scalar, 2);
  ellipticCurveScalarMultiply(&multiplied, &ell, &G, scalar);

  gmp_printf("multiplied's x : %Zd\n", multiplied.x.value);
  gmp_printf("multiplied's y : %Zd\n", multiplied.y.value);

  elliptic_curve_point_t result;
  ellipticCurvePointInit(&result);
  ellipticCurvePointAdd(&result, &curve, &G, &G);
  gmp_printf("2G = (%Zd, %Zd)\n", result.x.value, result.y.value);
  */
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

  // Set curve.G.x.p and curve.G.y.p to the field prime
  mpz_init_set(curve.G.x.p, curve.p);
  mpz_init_set(curve.G.y.p, curve.p);

  curve.G.infinity = 0;

  int isOnCurve2 = pointIsOnCurve(&curve, &curve.G);
  printf("Is it on the curve ? %d\n", isOnCurve2);

  elliptic_curve_point_t doubleG;
  ellipticCurvePointInit(&doubleG);

  ellipticCurvePointAdd(&doubleG, &curve, &curve.G, &curve.G);

  gmp_printf("Double x : %Zd\n", doubleG.x.value);
  gmp_printf("Double y : %Zd\n", doubleG.y.value);

  // key pair
  mpz_t k;
  mpz_init(k);
  mpz_set_ui(k, 2);
  elliptic_curve_point_t publicKey;
  ellipticCurvePointInit(&publicKey);

  ellipticCurveScalarMultiply(&publicKey, &curve, &curve.G, k);

  gmp_printf("Public key x : %Zd\n", publicKey.x.value);
  gmp_printf("Public key y : %Zd\n", publicKey.y.value);

  return 0;
}
