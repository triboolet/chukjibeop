/*
 * Elliptic curve representation and operations.
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"

// Elliptic curves structures
typedef struct {
  mpz_t value;
  mpz_t p;
} field_element_t;

typedef struct {
  field_element_t x;
  field_element_t y;
  int infinity;
} elliptic_curve_point_t;

typedef struct {
  mpz_t p; // prime modulus
  mpz_t a; // curve parameter a
  mpz_t b; // curve parameter b

  elliptic_curve_point_t G; // generator point
  mpz_t n;                  // order of the base point
  mpz_t h;                  // cofactor
} elliptic_curve_t;

// Field elements functions
void fieldElementInit(field_element_t *field_element) {
  mpz_init(field_element->value);
  mpz_init(field_element->p);
}

void fieldElementInitWithValue(field_element_t *field_element,
                               const mpz_t *value, const mpz_t *p) {
  mpz_init(field_element->value);
  mpz_set(field_element->value, *value);
  mpz_init(field_element->p);
  mpz_set(field_element->p, *p);
}

void fieldElementClear(field_element_t *field_element) {
  mpz_clear(field_element->value);
  mpz_clear(field_element->p);
}

int fieldElementEqual(const field_element_t *elem1,
                      const field_element_t *elem2) {
  return (mpz_cmp(elem1->value, elem2->value) == 0 &&
          mpz_cmp(elem1->p, elem2->p) == 0);
}

// Elliptic curve points functions
void ellipticCurvePointInit(elliptic_curve_point_t *elliptic_curve_point) {
  fieldElementInit(&elliptic_curve_point->x);
  fieldElementInit(&elliptic_curve_point->y);
  elliptic_curve_point->infinity = 0;
}

void ellipticCurvePointInitWithPoints(
    elliptic_curve_point_t *elliptic_curve_point, const mpz_t *x,
    const mpz_t *y, const mpz_t *p, int infinity) {
  fieldElementInitWithValue(&elliptic_curve_point->x, x, p);
  fieldElementInitWithValue(&elliptic_curve_point->y, y, p);
  elliptic_curve_point->infinity = infinity;
}

void ellipticCurvePointClear(elliptic_curve_point_t *elliptic_curve_point) {
  fieldElementClear(&elliptic_curve_point->x);
  fieldElementClear(&elliptic_curve_point->y);
}

int ellipticCurvePointEqual(const elliptic_curve_point_t *point1,
                            const elliptic_curve_point_t *point2) {
  return (fieldElementEqual(&point1->x, &point2->x) &&
          fieldElementEqual(&point1->y, &point2->y));
}

// Curve operations
int computePointCurve(field_element_t *pointOnCurve,
                      const elliptic_curve_t *curve,
                      const elliptic_curve_point_t *point) {
  if (!curve || !point) {
    return -1;
  }
  // x**3 + a*x + b
  mpz_t x_cubed;
  mpz_init_set(x_cubed, point->x.value);
  mpz_pow_ui(x_cubed, x_cubed, 3);

  mpz_t a_x_squared;
  mpz_init(a_x_squared);
  mpz_mul(a_x_squared, point->x.value, curve->a);

  mpz_init_set(pointOnCurve->p, curve->p);

  mpz_add(pointOnCurve->value, x_cubed, a_x_squared);
  mpz_add(pointOnCurve->value, pointOnCurve->value, curve->b);
  mpz_mod(pointOnCurve->value, pointOnCurve->value, curve->p);

  mpz_clear(x_cubed);
  mpz_clear(a_x_squared);
  return 0;
}

int pointIsOnCurve(const elliptic_curve_t *curve,
                   const elliptic_curve_point_t *point) {
  if (!curve || !point) {
    return -1;
  }
  field_element_t y = point->y;

  // y squared
  mpz_t y_lhs;
  mpz_init(y_lhs);
  mpz_powm_ui(y_lhs, y.value, 2, curve->p);

  // y rhs
  field_element_t y_rhs;
  mpz_init(y_rhs.value);
  computePointCurve(&y_rhs, curve, point);
  int pointIsOnCurve = (mpz_cmp(y_lhs, y_rhs.value) == 0);
  mpz_clear(y_lhs);
  mpz_clear(y_rhs.value);
  mpz_clear(y_rhs.p);
  return pointIsOnCurve;
}

int ellipticCurvePointAdd(elliptic_curve_point_t *result,
                          const elliptic_curve_t *curve,
                          const elliptic_curve_point_t *point1,
                          const elliptic_curve_point_t *point2) {
  if (!curve || !point1 || !point2) {
    return -1;
  }

  // if one of the point is at infinity, the results is the other point
  if (point1->infinity && point2->infinity) {
    result->infinity = 1;
    return 0;
  }
  if (point1->infinity) {
    mpz_set(result->x.value, point2->x.value);
    mpz_set(result->y.value, point2->y.value);
    result->infinity = point2->infinity;
    return 0;
  }
  if (point2->infinity) {
    mpz_set(result->x.value, point1->x.value);
    mpz_set(result->y.value, point1->y.value);
    result->infinity = point1->infinity;
    return 0;
  }
  // if the two points are opposite (hence P + (-P) = inf)
  if (mpz_cmp(point1->x.value, point2->x.value) == 0) {
    mpz_t y_sum;
    mpz_init(y_sum);
    mpz_add(y_sum, point1->y.value, point2->y.value);
    mpz_mod(y_sum, y_sum, curve->p);
    if (mpz_cmp_ui(y_sum, 0) == 0) {
      result->infinity = 1;
      mpz_clear(y_sum);
      return 0;
    }
    mpz_clear(y_sum);
  }

  if (!ellipticCurvePointEqual(point1, point2)) {
    mpz_t num, denom, denom_inv, lambda, new_x, new_x_diff, new_y;

    // lambda = (y2 - y1)/(x2 - x1) mode p
    // <==> lambda = (y2 - y1)*(x2 - x1)^-1 mod p
    mpz_inits(num, denom, denom_inv, lambda, new_x, new_x_diff, new_y, NULL);
    mpz_sub(num, point2->y.value, point1->y.value);
    mpz_mod(num, num, curve->p);

    mpz_sub(denom, point2->x.value, point1->x.value);
    mpz_mod(denom, denom, curve->p);

    if (mpz_invert(denom_inv, denom, curve->p) == 0) {
      printf("Modular inverse does not exist (denominator = 0 mod p)\n");
      mpz_clears(num, denom, denom_inv, lambda, new_x, new_x_diff, new_y);
      return 1;
    }

    mpz_mul(lambda, num, denom_inv);
    mpz_mod(lambda, lambda, curve->p);

    // x = lambda**2 - x1 - x2 mod p
    mpz_mul(new_x, lambda, lambda);
    mpz_sub(new_x, new_x, point1->x.value);
    mpz_sub(new_x, new_x, point2->x.value);
    mpz_mod(new_x, new_x, curve->p);

    // y = lambda*(x1 - x3) - y1 mod p
    mpz_sub(new_x_diff, point1->x.value, new_x);
    mpz_mul(new_y, lambda, new_x_diff);
    mpz_sub(new_y, new_y, point1->y.value);
    mpz_mod(new_y, new_y, curve->p);

    mpz_set(result->x.value, new_x);
    mpz_set(result->y.value, new_y);
    result->infinity = 0;

    mpz_clears(num, denom, denom_inv, lambda, new_x, new_x_diff, new_y);
  } else {
    mpz_t num, three_x1_squared, denom, denom_inv, two_x1, lambda, new_x,
        new_x_diff, new_y;

    // lambda = (3x1**2 +a)/2y1 mod p
    mpz_inits(num, three_x1_squared, denom, denom_inv, two_x1, lambda, new_x,
              new_x_diff, new_y, NULL);

    mpz_mul(three_x1_squared, point1->x.value, point1->x.value);
    mpz_mul_ui(three_x1_squared, three_x1_squared, 3);

    mpz_add(num, three_x1_squared, curve->a);
    mpz_mod(num, num, curve->p);

    mpz_mul_ui(denom, point1->y.value, 2);
    mpz_mod(denom, denom, curve->p);

    if (mpz_invert(denom_inv, denom, curve->p) == 0) {
      printf("Modular inverse does not exist (denominator = 0 mod p)\n");
      mpz_clears(num, denom, denom_inv, three_x1_squared, lambda, two_x1, new_x,
                 new_x_diff, new_y);
      return 1;
    }

    mpz_mul(lambda, num, denom_inv);
    mpz_mod(lambda, lambda, curve->p);

    // x = lambda**2 - 2x1 mod p
    mpz_mul_ui(two_x1, point1->x.value, 2);
    mpz_mul(new_x, lambda, lambda);
    mpz_sub(new_x, new_x, two_x1);
    mpz_mod(new_x, new_x, curve->p);

    // y = lambda*(x1 - x3) - y1 mod p
    mpz_sub(new_x_diff, point1->x.value, new_x);
    mpz_mul(new_y, lambda, new_x_diff);
    mpz_sub(new_y, new_y, point1->y.value);
    mpz_mod(new_y, new_y, curve->p);

    mpz_set(result->x.value, new_x);
    mpz_set(result->y.value, new_y);
    result->infinity = 0;

    mpz_clears(num, denom, denom_inv, three_x1_squared, lambda, two_x1, new_x,
               new_x_diff, new_y);
  }

  return 0;
}

int ellipticCurveScalarMultiply(elliptic_curve_point_t *result,
                                const elliptic_curve_t *curve,
                                const elliptic_curve_point_t *point,
                                const mpz_t scalar) {
  /*
   * Multiplying over the given elliptic curve (curve over a prime field)
   * means running *scalar* additions of the point on the curve.
   * One way to do it efficiently is to use the "Double and Add" algorithm,
   * cf https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication
   */
  if (!curve || !point || !result || mpz_sgn(scalar) == 0)
    return -1;

  elliptic_curve_point_t R;
  ellipticCurvePointInit(&R);
  mpz_set(R.x.p, curve->p);
  mpz_set(R.y.p, curve->p);
  R.infinity = 1;

  elliptic_curve_point_t Q;
  ellipticCurvePointInitWithPoints(&Q, &point->x.value, &point->y.value,
                                   &curve->p, point->infinity);
  result->infinity = 1;

  size_t nbits = mpz_sizeinbase(scalar, 2);
  for (ssize_t i = nbits - 1; i >= 0; i--) {
    // R = R + R
    ellipticCurvePointAdd(&R, curve, &R, &R);
    if (mpz_tstbit(scalar, i)) {
      // R = R + Q
      ellipticCurvePointAdd(&R, curve, &R, &Q);
    }
  }

  mpz_set(result->x.value, R.x.value);
  mpz_set(result->y.value, R.y.value);
  result->infinity = R.infinity;

  ellipticCurvePointClear(&R);
  ellipticCurvePointClear(&Q);
  return 0;
}
