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
      // case when the remaining string length is longer than 16
      strncpy(rightmost, str + strlen(str) - 16, 16);
      rightmost[16] = '\0';
      result.data[i] = (uint64_t)strtoul(rightmost, end, 16);
      str[strlen(str) - 16] = '\0';
    } else if (!strlen(str)) {
      // case when the string is completely parsed, set the bit to zero
      result.data[i] = 0U;
    } else {
      // case when the remaining string length is between 0 and 16
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
  // determine whether or not to pad the result, zero for the highest bit
  int padded = 0;

  for (int i = 3; i >= 0; i--) {
    // start from the most significant bit
    uint64_t value = val.data[i];
    // the highest bit should not have unnecessary zeros
    if (!padded) {
      if (value) {
        sprintf(buf, "%lx", value); // format without leading 0s
        padded = 1;
      } else {
        buf[0] = '\0';
      }
    } else {
      // other bits can have leading zeros
      sprintf(buf, "%016lx", value);
    }
    // update hex 
    strncpy(hex + len, buf, strlen(buf));
    // increment len
    len += strlen(buf);
    // attach a terminating zero
    *(hex + len) = '\0';
  }
  // terminating zero at the end of the result string
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
  UInt256 product = uint256_create_from_u64(0U);
  for (unsigned i = 0; i < 256; i++) {
    if (uint256_bit_is_set(right, i)) {
      product = uint256_add(product, uint256_leftshift(left, i));
    }
  }
  return product;
}

// Determine whether a particular bit is set to 1.
// index is the index of the bit to be evaluated in UInt256 val.
int uint256_bit_is_set(UInt256 val, unsigned index) {
  // determine which element is the index in
  unsigned element = index / 64;
  return val.data[element] & (1UL << (index % 64));
}

// Left-shift a value by a specified number of positions:
UInt256 uint256_leftshift(UInt256 val, unsigned shift) {
  // string of binary digits
  char *bin = malloc(sizeof(char) * 257);
  // start from the last index
  int place = 255;
  bin[256] = '\0';

  // convert uint256 to binary string
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j <= 63; j++) {
      if (val.data > 0) {
        if (val.data[i] % 2 == 1) {
          bin[place - j] = '1';
        } else {
          bin[place - j] = '0';
        }
        val.data[i] /= 2;
      } else {
        bin[place - j] = '0';
      }
    }
    place -= 64;
  }
  
  // perform leftshift on the binary string
  for (unsigned i = 0; i < 256; i++) {
    if (i + shift < 256) {
      // higher digits are replaced by lower digits
      bin[i] = bin[i + shift];
    } else {
      // zeros are padded to the lowest digits
      bin[i] = '0';
    }
  }
  // convert shifted binary string to uint256
  val = uint256_create_from_bin(bin);
  free(bin);
  return val;
}

// Create a UInt256 value from a string of binary digits.
UInt256 uint256_create_from_bin(const char *bin) {
  UInt256 result;
  char *str = malloc((strlen(bin) + 1) * sizeof(char));
  strncpy(str, bin, strlen(bin)); // copy to non-const
  str[strlen(bin)] = '\0';
  char *rightmost = NULL;
  char **end = NULL;
  for (unsigned i = 0; i < 4; i++) {
    if (strlen(str) > 64) {
      // case when the remaining string length is longer than 16
      strncpy(rightmost, str + strlen(str) - 64, 64);
      rightmost[64] = '\0';
      result.data[i] = (uint64_t)strtoul(rightmost, end, 2);
      str[strlen(str) - 64] = '\0';
    } else if (!strlen(str)) {
      // case when the string is completely parsed, set the bit to zero
      result.data[i] = 0U;
    } else {
      // case when the remaining string length is between 0 and 16
      result.data[i] = (uint64_t)strtoul(str, end, 2);
      str[0] = '\0';
    }
  }
  free(str);
  return result;
}



