#pragma once

class BigInt {
  typedef uint32_t SmallBaseInt;
  typedef uint64_t BigBaseInt;
  typedef int64_t SignedBigBaseInt;

 public:
  BigInt(uint32_t n);
  BigInt(int32_t n);
  BigInt(uint64_t n);
  BigInt(int64_t n);
  BigInt(std::string s, int base = 16);

  ~BigInt();
  BigInt(const BigInt& other);
  BigInt& operator=(BigInt other);
  BigInt& operator=(uint32_t n);
  BigInt& operator=(int32_t n);
  BigInt& operator=(uint64_t n);
  BigInt& operator=(int64_t n);
  BigInt& operator=(std::string s);
  BigInt(BigInt&& other);

  BigInt operator-() const;

  BigInt operator+(const BigInt& rhs) const;
  BigInt operator-(const BigInt& rhs) const;
  BigInt operator*(const BigInt& rhs) const;
  std::pair<BigInt, BigInt> operator/(const BigInt& rhs) const;

  // power operator, not XOR operator
  BigInt operator^(const BigInt& rhs) const;

  BigInt operator<<(const BigInt& rhs) const;
  BigInt operator>>(const BigInt& rhs) const;

  bool operator<(const BigInt& rhs) const;
  bool operator>(const BigInt& rhs) const;
  bool operator==(const BigInt& rhs) const;
  bool operator!=(const BigInt& rhs) const;
  bool operator<=(const BigInt& rhs) const;
  bool operator>=(const BigInt& rhs) const;

  const SmallBaseInt& operator[](size_t index) const;
  const SmallBaseInt& safe_at(SignedBigBaseInt index) const;

  std::string str(int base = 16) const;
  uint64_t uint64() const;

  static const BigInt ZERO;
  static const BigInt ONE;
  static const BigInt BASE;

 private:
  static bool AbsDigitCompare(
      const std::vector<SmallBaseInt>& lhs,
      const std::vector<SmallBaseInt>& rhs);

  BigInt();
  void swap(BigInt& other);
  void trimLeadingZeros();
  void padWithLeadingZeros(size_t num_digits);

  signed int sign;
  std::vector<SmallBaseInt> digits;

#ifdef _DEBUG
  mutable std::string _lastString;
#endif
};
