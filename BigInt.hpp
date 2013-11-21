#pragma once

class BigInt {
  typedef uint32_t SmallBaseInt;
  typedef uint64_t BigBaseInt;

 public:
  BigInt(unsigned int n);
  BigInt(int n);

  ~BigInt();
  BigInt(const BigInt& other);
  BigInt& operator=(BigInt other);
  BigInt(BigInt&& other);

  BigInt operator+(const BigInt& rhs) const;
  BigInt operator-(const BigInt& rhs) const;
  BigInt operator-() const;

  bool operator<(const BigInt& rhs) const;
  bool operator>(const BigInt& rhs) const;
  bool operator==(const BigInt& rhs) const;
  bool operator<=(const BigInt& rhs) const;

  std::string str(int base = 16);

 private:
  BigInt();
  void swap(BigInt& other);

  char sign;
  std::vector<SmallBaseInt> digits;
};
