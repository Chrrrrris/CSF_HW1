#include "uint256.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a UInt256 value from a single uint64_t value.
// Only the least-significant 64 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u64(uint64_t val) {
  UInt256 result;
  result.data[0] = val;
  result.data[1] = 0U;
  result.data[2] = 0U;
  result.data[3] = 0U;
  return result;
}

// Create a UInt256 value from an array of 4 uint64_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint64_t data[4]) {
  UInt256 result;
  for (unsigned i = 0; i < 4; i++) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  char *str = malloc((strlen(hex) + 1) * sizeof(char));
  strncpy(str, hex, strlen(hex)); // copy to non-const
  str[strlen(hex)] = '\0';
  char *rightmost = NULL;
  char **end = NULL;
  for (unsigned i = 0; i < 4; i++) {
    if (strlen(str) > 16) {
      strncpy(rightmost, str + strlen(str) - 16, 16);
      rightmost[16] = '\0';
      result.data[i] = (uint64_t)strtoul(rightmost, end, 16);
      str[strlen(str) - 16] = '\0';
    } else if (!strlen(str)) {
      result.data[i] = 0U;
    } else {
      result.data[i] = (uint64_t)strtoul(str, end, 16);
      str[0] = '\0';
    }
  }
  free(str);
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = malloc((4 * 16 + 1) * sizeof(char));
  int len = 0; // incremental string length
  char *buf = malloc(17 * sizeof(char));
  int padded = 0;

  for (int i = 3; i >= 0; i--) {
    uint64_t value = val.data[i];
    if (!padded) {
      if (value) {
        sprintf(buf, "%lx", value); // format without leading 0s
        padded = 1;
      } else {
        buf[0] = '\0';
      }
    } else {
      sprintf(buf, "%016lx", value);
    }
    strncpy(hex + len, buf, strlen(buf));
    len += strlen(buf);
    *(hex + len) = '\0';
  }
  hex[len * sizeof(char)] = '\0';
  if (!len) { // check for 0
    hex[0] = '0';
    hex[1] = '\0';
  }
  free(buf);
  return hex;
}

// Get 64 bits of data from a UInt256 value.
// Index 0 is the least significant 64 bits, index 3 is the most
// significant 64 bits.
uint64_t uint256_get_bits(UInt256 val, unsigned index) {
  uint64_t bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 final_sum;
  int overflowed = 0;
  for (unsigned i = 0; i < 4; i++) {
    uint64_t leftval = left.data[i];
    uint64_t rightval = right.data[i];
    uint64_t sum = leftval + rightval;
    if (overflowed) {
      sum += 1;
      overflowed = 0;
    }
    if (sum < leftval) {
      overflowed = 1;
    }
    final_sum.data[i] = sum;
  }
  return final_sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Compute the product of two UInt256 values.
UInt256 uint256_mul(UInt256 left, UInt256 right) {
  UInt256 product;
  // TODO: implement
  return product;
}
