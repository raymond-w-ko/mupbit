#pragma once

class BigInt {
  typedef uint32_t SmallBaseInt;
  typedef uint64_t BigBaseInt;

 public:
  BigInt(unsigned int n);
  BigInt(int n);
  BigInt(std::string s, int base = 16);

  ~BigInt();
  BigInt(const BigInt& other);
  BigInt& operator=(BigInt other);
  BigInt& operator=(unsigned int n);
  BigInt& operator=(int n);
  BigInt& operator=(std::string s);
  BigInt(BigInt&& other);

  BigInt operator-() const;

  BigInt operator+(const BigInt& rhs) const;
  BigInt operator-(const BigInt& rhs) const;
  BigInt operator*(const BigInt& rhs) const;
  BigInt operator/(const BigInt& rhs) const;

  bool operator<(const BigInt& rhs) const;
  bool operator>(const BigInt& rhs) const;
  bool operator==(const BigInt& rhs) const;
  bool operator!=(const BigInt& rhs) const;
  bool operator<=(const BigInt& rhs) const;

  std::string str(int base = 16);

 private:
  BigInt();
  void swap(BigInt& other);

  static bool AbsDigitCompare(
      const std::vector<SmallBaseInt>& lhs,
      const std::vector<SmallBaseInt>& rhs);

  char sign;
  std::vector<SmallBaseInt> digits;

#ifdef _DEBUG
  std::string _lastString;
#endif
};
