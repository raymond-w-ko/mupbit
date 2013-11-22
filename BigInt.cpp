#include "StableHeaders.hpp"
#include "BigInt.hpp"

BigInt::~BigInt() {
}

BigInt::BigInt()
    : sign(1) {
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
    i = -i;
  }

  unsigned int n = std::abs(i);
  digits.push_back(n);
}

BigInt::BigInt(std::string s, int base) {
  // TODO: implement other bases
  assert(base == 16);

  // empty string == 0
  if (s.size() == 0) {
    this->sign = 1;
    this->digits.push_back(0);
  }

  size_t numCharsPerSmallBaseDigit = sizeof(SmallBaseInt) * 2;
  size_t numWholeSmallBaseDigits = s.size() / numCharsPerSmallBaseDigit;

  size_t index = s.size() - 1;

  for (size_t i = 0; i < numWholeSmallBaseDigits; ++i) {
    size_t end = index + 1;
    size_t begin = end - numCharsPerSmallBaseDigit;
    if (i == (numWholeSmallBaseDigits - 1) &&
        (s[begin] == '-' || s[begin] == '+')) {
      break;
    }
    std::string chunk(s.begin() + begin, s.begin() + end);
    std::stringstream ss;
    ss << std::hex << chunk;
    SmallBaseInt n;
    ss >> n;
    this->digits.push_back(n);

    index -= numCharsPerSmallBaseDigit;
  }

  static const size_t underflow = size_t(0) - size_t(1);

  if (index == underflow) {
    this->sign = 1;
    return;
  }

  size_t begin = 0;
  if (s[0] == '-') {
    this->sign = -1;
    begin = 1;
  } else if (s[0] == '+') {
    this->sign = 1;
    begin = 1;
  } else {
    this->sign = 1;
  }

  if (begin < (index + 1)) {
    size_t end = index + 1;
    std::string finalChunk(s.begin() + begin, s.begin() + end);
    std::stringstream ss;
    ss << std::hex << finalChunk;
    SmallBaseInt n;
    ss >> n;
    this->digits.push_back(n);
  }
}

BigInt::BigInt(const BigInt& other) 
    : sign(other.sign),
      digits(other.digits) {
}

BigInt& BigInt::operator=(BigInt other) {
  other.swap(*this);
  return *this;
}

BigInt& BigInt::operator=(unsigned int n) {
  this->sign = 1;
  this->digits.clear();
  this->digits.push_back(n);

  return *this;
}

BigInt& BigInt::operator=(int n) {
  if (n >= 0) {
    this->sign = 1;
  } else {
    this->sign = -1;
    n = -n;
  }
  this->digits.clear();
  this->digits.push_back(static_cast<SmallBaseInt>(n));

  return *this;
}

BigInt& BigInt::operator=(std::string s) {
  BigInt tmp(s, 16);
  tmp.swap(*this);
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

std::string BigInt::str(int base) {
  // TODO: implement other bases
  assert(base == 16);

  std::string s;

  char buf[9];
  for (size_t i = 0; i < digits.size(); ++i) {
    size_t index = digits.size() - i - 1;
    SmallBaseInt n = digits[index];
    sprintf(buf, "%08x", n);
    s += buf;
  }

  size_t trimAmount = 0;
  while (trimAmount < (s.size() - 1) && s[trimAmount] == '0') {
    trimAmount++;
  }

  s = std::string(s.begin() + trimAmount, s.end());
  if (sign < 0)
    s = "-" + s;
#ifdef _DEBUG
  _lastString = s;
#endif
  return s;
}

BigInt BigInt::operator-() const {
  BigInt n(*this);
  n.sign = n.sign > 0 ? -1 : 1;
  return n;
}

BigInt BigInt::operator+(const BigInt& rhs) const {
  const BigInt& lhs = *this;

  if (lhs.sign > 0 && rhs.sign < 0) {
    // (+123) + (-345) == (+123) - (+345)
    return lhs.operator-(-rhs);
  } else if (lhs.sign < 0 && rhs.sign > 0) {
    // (-123) + (+345) == (+345) - (+123)
    return rhs.operator-(-lhs);
  }

  /*
  if (lhs.sign > 0 && rhs.sign > 0) {
    // (+123) + (+345) == 123 + 345
  }
  else if (lhs.sign < 0 && rhs.sign < 0) {
    // (-123) + (-345) == -(123 + 345)
  }
  */
  assert((lhs.sign > 0 && rhs.sign > 0) || (lhs.sign < 0 && rhs.sign < 0));

  BigInt sum;

  const std::vector<SmallBaseInt>& bigger =
      lhs.digits.size() >= rhs.digits.size() ? lhs.digits : rhs.digits;
  const std::vector<SmallBaseInt>& smaller =
      lhs.digits.size() < rhs.digits.size() ? lhs.digits : rhs.digits;

  BigBaseInt carry = 0;
  size_t i;
  for (i = 0; i < smaller.size(); ++i) {
    BigBaseInt wideDigitSum =
        static_cast<BigBaseInt>(carry) +
        static_cast<BigBaseInt>(bigger[i]) +
        static_cast<BigBaseInt>(smaller[i]);
    SmallBaseInt digitSum = static_cast<SmallBaseInt>(wideDigitSum);
    sum.digits.push_back(digitSum);
    carry = wideDigitSum >> sizeof(SmallBaseInt) * 8;
  }
  for (; i < bigger.size(); ++i) {
    BigBaseInt wideDigitSum =
        static_cast<BigBaseInt>(carry) +
        static_cast<BigBaseInt>(bigger[i]);
    SmallBaseInt digitSum = static_cast<SmallBaseInt>(wideDigitSum);
    sum.digits.push_back(digitSum);
    carry = wideDigitSum >> sizeof(SmallBaseInt) * 8;
  }
  if (carry > 0) {
    sum.digits.push_back(static_cast<SmallBaseInt>(carry));
  }
  sum.sign = lhs.sign;
  return sum;
}

BigInt BigInt::operator-(const BigInt& rhs) const {
  const BigInt& lhs = *this;

  if (lhs.sign > 0 && rhs.sign < 0) {
    // (+123) - (-345) == (+123) + (+345)
    return lhs.operator+(-rhs);
  } else if (lhs.sign < 0 && rhs.sign > 0) {
    // (-123) - (+345) == -((+345) + (+123))
    BigInt tmp = rhs.operator+(-lhs);
    tmp.sign = -1;
    return tmp;
  }

  char operandSignFlip;
  if (lhs.sign > 0 && rhs.sign > 0) {
    // (+123) - (+345) == 123 - 345
    operandSignFlip = 1;
  }
  else /*if (lhs.sign < 0 && rhs.sign < 0)*/ {
    // (-123) - (-345) == (+345) - 123
    operandSignFlip = -1;
  }
  assert((lhs.sign > 0 && rhs.sign > 0) || (lhs.sign < 0 && rhs.sign < 0));

  BigInt diff;

  bool absLesser = AbsDigitCompare(lhs.digits, rhs.digits);

  const std::vector<SmallBaseInt>& bigger =
      absLesser ? rhs.digits : lhs.digits;
  const std::vector<SmallBaseInt>& smaller =
      absLesser ? lhs.digits : rhs.digits;

  char magnitudeFlip = absLesser ? -1 : 1;

  size_t borrow = 0;
  for (size_t i = 0; i < bigger.size(); ++i) {
    BigBaseInt wideDigitDiff =
        static_cast<BigBaseInt>(bigger[i]) -
        static_cast<BigBaseInt>(smaller[i]) -
        static_cast<BigBaseInt>(borrow);
    SmallBaseInt digitDiff = static_cast<SmallBaseInt>(wideDigitDiff);
    diff.digits.push_back(digitDiff);
    borrow = ((borrow >> 32) > 0 ? 1 : 0);
  }
  assert(borrow == 0);

  diff.sign = operandSignFlip * magnitudeFlip;
  return diff;
}

bool BigInt::operator<(const BigInt& rhs) const {
  const BigInt& lhs = *this;

  if (lhs.sign > rhs.sign) {
    return true;
  } else if (lhs.sign < rhs.sign) {
    return false;
  }

  if (lhs.digits.size() < rhs.digits.size()) {
    return lhs.sign > 0 ? true : false;
  } else if (lhs.digits.size() > rhs.digits.size()) {
    return lhs.sign > 0 ? false : true;
  }

  assert(lhs.sign == rhs.sign);

  bool absLess = AbsDigitCompare(lhs.digits, rhs.digits);
  return lhs.sign > 0 ? absLess : !absLess;
}

bool BigInt::AbsDigitCompare(
    const std::vector<SmallBaseInt>& lhs,
    const std::vector<SmallBaseInt>& rhs) {
  assert(lhs.size() == rhs.size());
  for (size_t i = 0; i < lhs.size(); ++i) {
    size_t index = lhs.size() - i - 1;
    SmallBaseInt m = lhs[index];
    SmallBaseInt n = rhs[index];

    if (m < n) {
      return true;
    } else if (m > n ) {
      return false;
    }
  }

  // they are equal
  return false;
}

bool BigInt::operator<=(const BigInt& rhs) const {
  const BigInt& lhs = *this;
  return lhs < rhs || lhs == rhs;
}

bool BigInt::operator>(const BigInt& rhs) const {
  return !(rhs < *this);
}

bool BigInt::operator==(const BigInt& rhs) const {
  const BigInt& lhs = *this;
  return lhs.sign == rhs.sign && lhs.digits == rhs.digits;
}

bool BigInt::operator!=(const BigInt& rhs) const {
  const BigInt& lhs = *this;
  return !(lhs == rhs);
}


BigInt BigInt::operator*(const BigInt& rhs) const {
  BigInt product = 0;
  return product;
}
