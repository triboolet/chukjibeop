#include <memory.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <stdio.h>

#include "gmp.h"

#include "elliptic.h"

const char *BASE58_ALPHABET =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

void getPublicKeyCompressedForm(unsigned char *buffer,
                                const elliptic_curve_point_t *privateKey) {
  int yIsOdd = mpz_odd_p(privateKey->y.value);
  if (yIsOdd) {
    buffer[0] = 03;
  } else {
    buffer[0] = 02;
  }
  size_t count;
  mpz_export(buffer + 1, &count, 1, 1, 1, 0, privateKey->x.value);
}

void base58_encode(const unsigned char *data, size_t len,
                   unsigned char *b58_out) {
  uint8_t buf[128] = {0};
  size_t buf_len = 1;

  for (size_t i = 0; i < len; ++i) {
    int carry = data[i];
    for (size_t j = 0; j < buf_len; ++j) {
      carry += buf[j] << 8;
      buf[j] = carry % 58;
      carry /= 58;
    }
    while (carry) {
      buf[buf_len++] = carry % 58;
      carry /= 58;
    }
  }

  // Leading zeros
  size_t zeros = 0;
  while (zeros < len && data[zeros] == 0)
    zeros++;

  size_t i = 0;
  for (; i < zeros; ++i)
    b58_out[i] = '1';

  for (size_t j = 0; j < buf_len; ++j)
    b58_out[i + j] = BASE58_ALPHABET[buf[buf_len - 1 - j]];

  b58_out[i + buf_len] = '\0';
}

int generateBitcoinAddress(unsigned char *resultingAddress,
                           unsigned char *publicKey,
                           unsigned char versionByte) {
  resultingAddress[0] = versionByte;
  unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
  unsigned char ripemd160_hash[RIPEMD160_DIGEST_LENGTH];
  unsigned char version_ripemd160_hash[RIPEMD160_DIGEST_LENGTH + 1];
  unsigned char checksum[SHA256_DIGEST_LENGTH];

  SHA256(publicKey, 33, sha256_hash);

  RIPEMD160(sha256_hash, SHA256_DIGEST_LENGTH, ripemd160_hash);

  version_ripemd160_hash[0] = versionByte;
  memcpy(version_ripemd160_hash + 1, ripemd160_hash, RIPEMD160_DIGEST_LENGTH);
  memcpy(resultingAddress + 1, ripemd160_hash, RIPEMD160_DIGEST_LENGTH);

  SHA256(version_ripemd160_hash, 33, checksum);
  SHA256(checksum, 33, checksum);

  memcpy(resultingAddress + RIPEMD160_DIGEST_LENGTH + 1, checksum, 4);

  base58_encode(resultingAddress, sizeof(resultingAddress), resultingAddress);
  return 0;
}
