#include "StableHeaders.hpp"
#include "BigInt.hpp"

const BigInt BigInt::ZERO = BigInt(0);
const BigInt BigInt::ONE = BigInt(1);
const BigInt BigInt::BASE =
  BigInt(2) ^ BigInt(static_cast<uint32_t>(sizeof(SmallBaseInt)) * 8);

BigInt::~BigInt() {
}

BigInt::BigInt()
    : sign(1) {
}

BigInt::BigInt(uint32_t n) 
    : sign(1) {
  digits.push_back(n);
}

BigInt::BigInt(int32_t n) {
  if (n >= 0) {
    sign = 1;
  } else {
    sign = -1;
    n = -n;
  }

  digits.push_back(n);
}

BigInt::BigInt(uint64_t n)
    : sign(1) {
  digits.push_back(static_cast<uint32_t>(n));
  uint32_t upper = static_cast<uint32_t>(n >> 32);
  if (upper > 0) {
    digits.push_back(upper);
  }
}

BigInt::BigInt(int64_t n) {
  if (n >= 0) {
    sign = 1;
  } else {
    sign = -1;
    n = -n;
  }

  digits.push_back(static_cast<uint32_t>(n));
  uint32_t upper = static_cast<uint32_t>(n >> 32);
  if (upper > 0) {
    digits.push_back(upper);
  }
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

BigInt& BigInt::operator=(uint32_t n) {
  BigInt tmp(n);
  tmp.swap(*this);
  return *this;
}

BigInt& BigInt::operator=(int32_t n) {
  BigInt tmp(n);
  tmp.swap(*this);
  return *this;
}

BigInt& BigInt::operator=(uint64_t n) {
  BigInt tmp(n);
  tmp.swap(*this);
  return *this;
}

BigInt& BigInt::operator=(int64_t n) {
  BigInt tmp(n);
  tmp.swap(*this);
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

std::string BigInt::str(int base) const {
  // TODO: implement other bases
  assert(base == 16);

  std::stringstream ss;
  ss << "%0" << (sizeof(SmallBaseInt) * 2) << "x";
  std::string formatSpecifier = ss.str();

  std::string s;

  char buf[9];
  for (size_t i = 0; i < digits.size(); ++i) {
    size_t index = digits.size() - i - 1;
    SmallBaseInt n = digits[index];
    sprintf(buf, formatSpecifier.c_str(), n);
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

  bool absLesser;
  if (lhs.digits.size() == rhs.digits.size()) {
    absLesser = AbsDigitCompare(lhs.digits, rhs.digits);
  } else {
    absLesser = lhs.digits.size() < rhs.digits.size();
  }


  const std::vector<SmallBaseInt>& bigger =
      absLesser ? rhs.digits : lhs.digits;
  const std::vector<SmallBaseInt>& smaller =
      absLesser ? lhs.digits : rhs.digits;

  char magnitudeFlip = absLesser ? -1 : 1;

  size_t borrow = 0;
  for (size_t i = 0; i < bigger.size(); ++i) {
    SmallBaseInt smallerDigit = i < smaller.size() ? smaller[i] : 0;
    BigBaseInt wideDigitDiff =
        static_cast<BigBaseInt>(bigger[i]) -
        static_cast<BigBaseInt>(smallerDigit) -
        static_cast<BigBaseInt>(borrow);
    SmallBaseInt digitDiff = static_cast<SmallBaseInt>(wideDigitDiff);
    diff.digits.push_back(digitDiff);
    borrow = ((wideDigitDiff >> 32) > 0 ? 1 : 0);
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
  return !(*this < rhs);
}

bool BigInt::operator>=(const BigInt& rhs) const {
  const BigInt& lhs = *this;
  return lhs > rhs || lhs == rhs;
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
  const BigInt& lhs = *this;

  // Handbook of Applied Cryptography Algorithm 14.12
  BigInt product = 0;
  const std::vector<SmallBaseInt>& x = lhs.digits;
  const std::vector<SmallBaseInt>& y = rhs.digits;
  std::vector<SmallBaseInt>& w = product.digits;

  size_t n = lhs.digits.size() - 1;
  size_t t = rhs.digits.size() - 1;

  w.resize((n + t + 1) + 1, 0x0);

  for (size_t i = 0; i <= t; ++i ) {
    SmallBaseInt c = 0;
    SmallBaseInt u = 0;
    for (size_t j = 0; j <= n; ++j) {
      BigBaseInt sum = 0;
      BigBaseInt w_i_plus_j = w[i + j];
      BigBaseInt x_j_mul_y_i =
          static_cast<BigBaseInt>(x[j]) *
          static_cast<BigBaseInt>(y[i]);
      sum = w_i_plus_j + x_j_mul_y_i + c;
      u = static_cast<SmallBaseInt>(sum >> (sizeof(SmallBaseInt) * 8));
      SmallBaseInt v = static_cast<SmallBaseInt>(sum);

      w[i + j] = v;
      c = u;
    }

    w[i + n + 1] = u;
  }

  product.sign = lhs.sign * rhs.sign;

  // trim leading zero digits
  product.trimLeadingZeros();
  return product;
}

std::pair<BigInt, BigInt> BigInt::operator/(const BigInt& rhs) const {
  if (rhs == ZERO) {
    throw std::overflow_error("division by zero exception");
  }

  // Handbook of Applied Cryptography Algorithm 14.20

  // we have to apply a normalization multiplier to ensure that the most
  // significant bit of y is 1, or this algorithm basically never terminates.
  SmallBaseInt lastDigit = rhs[rhs.digits.size() - 1];
  BigInt normalizationShift = 0;
  while ((lastDigit >> (sizeof(SmallBaseInt) * 8 - 1)) == 0) {
    lastDigit <<= 1;
    normalizationShift = normalizationShift + 1;
  }

  BigInt x = *this << normalizationShift;
  const BigInt& y = rhs << normalizationShift;
  size_t n = x.digits.size() - 1;
  size_t t = y.digits.size() - 1;

  assert(n >= t && t >= 1);

  BigInt quot;
  //BigInt rem;
  std::vector<SmallBaseInt>& q = quot.digits;
  //std::vector<SmallBaseInt>& r = rem.digits;
  q.resize(n - t + 1, 0);
  //r.resize(t + 1);

  BigInt ybnt = y * (BigInt::BASE ^ (BigInt(n) - BigInt(t)));
  while (x >= ybnt) {
    q[n - t]++;
    x = x - ybnt;
  }

  BigInt l2 = ((BigInt(y[t]) * BigInt::BASE) + BigInt(y[t - 1]));
  for (size_t i = n; i >= (t + 1); --i) {
    if (x[i] == y[t]) {
      q[i - t - 1] = SmallBaseInt(0) - SmallBaseInt(1);
    } else {
      BigBaseInt floored = x[i] * BigInt::BASE.uint64();
      floored += x[i - 1];
      floored /= y[t];
      q[i - t - 1] = static_cast<SmallBaseInt>(floored);
    }

    BigInt r =
        (BigInt(x[i - 0]) * (BigInt::BASE ^ 2)) + 
        (BigInt(x[i - 1]) *  BigInt::BASE) + 
        (BigInt(x[i - 2]));
    for (;;) {
      BigInt l = BigInt(q[i - t - 1]) * l2;
      if (l <= r) {
        break;
      }
      
      q[i - t - 1]--;
    }

    BigInt ybit1 = y * (BigInt::BASE ^ BigInt(i - t - 1));
    x = x - (BigInt(q[i - t - 1]) * ybit1);

    if (x.sign < 0) {
      x = x + ybit1;
      q[i - t - 1]--;
    }
  }

  //rem = x;

  quot.sign = x.sign * y.sign;
  x.sign = quot.sign;

  quot.trimLeadingZeros();
  x.trimLeadingZeros();
  // remove normalization multiplier
  x = x >> normalizationShift;
  x.trimLeadingZeros();

  return std::make_pair(quot, x);
}

BigInt BigInt::operator^(const BigInt& rhs) const {
  const BigInt& lhs = *this;

  // x ^ 0 == 1 always
  if (rhs == ZERO) {
    return ONE;
  }

  BigInt n = BigInt(1);

  for (BigInt i = BigInt(0); i < rhs; i = i + 1) {
    n = n * lhs;
  }

  return n;
}

uint64_t BigInt::uint64() const {
  uint64_t n = 0;
  n = digits[0];
  uint64_t upper = 0;
  if (digits.size() >= 2) {
    upper = digits[1];
    upper <<= sizeof(SmallBaseInt) * 8;
  }
  n += upper;
  return n;
}

const BigInt::SmallBaseInt& BigInt::operator[](size_t index) const {
  return digits[index];
}

void BigInt::trimLeadingZeros() {
  size_t numZerosToRemove = 0;
  for (size_t i = (digits.size() -1); i > 0; --i) {
    if (digits[i] == 0) {
      numZerosToRemove++;
    } else {
      break;
    }
  }
  digits.resize(digits.size() - numZerosToRemove);
}

BigInt BigInt::operator<<(const BigInt& rhs) const {
  static const SmallBaseInt baseInBits = sizeof(SmallBaseInt) * 8;

  assert(rhs > BigInt::ZERO);

  if (rhs == BigInt::ZERO) {
    return *this;
  }

  BigInt shifted;
  BigInt bigShiftsRem = rhs;
  for (; bigShiftsRem >= 32; bigShiftsRem = bigShiftsRem - 32) {
    shifted.digits.push_back(0);
  }
  assert(bigShiftsRem.digits.size() == 1);
  SmallBaseInt shiftsRem = bigShiftsRem[0];
  assert(shiftsRem < baseInBits);

  const BigInt& n = *this;

  SmallBaseInt overflow = 0;
  for (size_t i = 0; i < n.digits.size(); ++i) {
    SmallBaseInt newDigit = n[i] << shiftsRem;
    newDigit |= overflow;
    overflow = n[i] >> (baseInBits - shiftsRem);
    shifted.digits.push_back(newDigit);
  }
  if (overflow > 0) {
    shifted.digits.push_back(overflow);
  }

  shifted.sign = n.sign;
  return shifted;
}

BigInt BigInt::operator>>(const BigInt& rhs) const {
  static const SmallBaseInt baseInBits = sizeof(SmallBaseInt) * 8;

  assert(rhs > BigInt::ZERO);

  if (rhs == BigInt::ZERO) {
    return *this;
  }

  BigInt shifted;

  size_t digitsDropped = 0;
  BigInt bigShiftsRem = rhs;
  for (; bigShiftsRem >= 32; bigShiftsRem = bigShiftsRem - 32) {
    digitsDropped++;
  }
  assert(bigShiftsRem.digits.size() == 1);
  SmallBaseInt shiftsRem = bigShiftsRem[0];
  assert(shiftsRem < baseInBits);

  const BigInt& n = *this;

  for (size_t i = digitsDropped; i < n.digits.size(); ++i) {
    SmallBaseInt digit = n[i] >> shiftsRem;
    SmallBaseInt borrowed = 0;
    if (i < (n.digits.size() - 1)) {
      borrowed = n[i + 1] << (baseInBits - shiftsRem);
    }
    digit |= borrowed;
    shifted.digits.push_back(digit);
  }

  // shifted too far, nothing left
  if (shifted.digits.size() == 0) {
    shifted.digits.push_back(0);
  }

  shifted.sign = n.sign;
  return shifted;
}
