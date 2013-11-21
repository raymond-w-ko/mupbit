#include "StableHeaders.hpp"
#include "BigInt.hpp"

BigInt::~BigInt() {
}

BigInt::BigInt()
    : sign(1) {
  digits.push_back(0);
}

BigInt::BigInt(unsigned int n) 
    : sign(1) {
  digits.push_back(n);
}

BigInt::BigInt(int i) {
  if (i >= 0) {
    sign = 1;
  } else {
    sign = -1;
  }

  unsigned int n = std::abs(i);
  digits.push_back(n);
}

BigInt::BigInt(const BigInt& other) 
    : sign(other.sign),
      digits(other.digits) {
}

BigInt& BigInt::operator=(BigInt other) {
  other.swap(*this);
  return *this;
}

BigInt::BigInt(BigInt&& other)
    : sign(other.sign),
      digits(std::move(other.digits)) {
}

void BigInt::swap(BigInt& other) {
  std::swap(this->sign, other.sign);
  std::swap(this->digits, other.digits);
}

BigInt operator+(const BigInt& lhs, const BigInt& rhs) {
  std::vector<uint32_t> bigger =
      lhs.digits.size() >= rhs.digits.size() ? lhs.digits : rhs.digits;
  const std::vector<uint32_t>& smaller =
      lhs.digits.size() < rhs.digits.size() ? lhs.digits : rhs.digits;

  for (int i = 0; i < smaller.size(); ++i) {
    uint32_t digitSum = bigger[i] + smaller[i];
    uint32_t digitOverflow = digitSum < bigger[i] ? 1 : 0;

    if (digitOverflow > 0) {
      const int nextIndex = i + 1;
      if (nextIndex >= bigger.size()) {
        bigger.push_back(digitOverflow);
      } else {
        // XXX: doesn't work , can cause overflow!
        bigger[nextIndex] += digitOverflow;
      }
    }
  }
  return lhs;
}
