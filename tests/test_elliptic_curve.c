#include "check.h"

#include "gmp.h"

#include "elliptic.h"

START_TEST(test_point_is_on_elliptic_curve) {
  elliptic_curve_t ell;
  mpz_init_set_ui(ell.p, 17);
  mpz_init_set_ui(ell.a, 3);
  mpz_init_set_ui(ell.b, 2);

  elliptic_curve_point_t G;
  mpz_init_set_ui(G.x.value, 3);
  mpz_init_set_ui(G.y.value, 2);
  ell.G = G;

  int isOnCurve = pointIsOnCurve(&ell, &ell.G);

  ck_assert_msg(isOnCurve, "Point should be on curve");
}
END_TEST

START_TEST(test_point_is_not_on_elliptic_curve) {
  elliptic_curve_t ell;
  mpz_init_set_ui(ell.p, 17);
  mpz_init_set_ui(ell.a, 3);
  mpz_init_set_ui(ell.b, 2);

  elliptic_curve_point_t G;
  mpz_init_set_ui(G.x.value, 4);
  mpz_init_set_ui(G.y.value, 2);
  ell.G = G;

  int isOnCurve = pointIsOnCurve(&ell, &ell.G);

  ck_assert_msg(!isOnCurve, "Point shouldn't be on curve");
}
END_TEST

START_TEST(test_point_add) {

  elliptic_curve_t curve;
  mpz_init_set_ui(curve.p, 17);
  mpz_init_set_ui(curve.a, 3);
  mpz_init_set_ui(curve.b, 2);

  elliptic_curve_point_t G, result;
  mpz_t x, y;
  mpz_inits(x, y, NULL);
  mpz_set_ui(x, 3);
  mpz_set_ui(y, 2);
  ellipticCurvePointInitWithPoints(&G, &x, &y, &curve.b, 0);
  ellipticCurvePointInit(&result);

  ellipticCurvePointAdd(&result, &curve, &G, &G);

  ck_assert_msg(mpz_cmp_ui(result.x.value, 12) == 0 &&
                    mpz_cmp_ui(result.y.value, 7) == 0,
                "Doubling the point on this curve should yield (12, 7)");
}
END_TEST

Suite *elliptic_suite(void) {
  Suite *s = suite_create("Elliptic");
  TCase *tc = tcase_create("Core");

  tcase_add_test(tc, test_point_is_on_elliptic_curve);
  tcase_add_test(tc, test_point_is_not_on_elliptic_curve);
  tcase_add_test(tc, test_point_add);
  suite_add_tcase(s, tc);
  return s;
}

int main(void) {
  int failed;
  Suite *s = elliptic_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return failed == 0 ? 0 : 1;
}
