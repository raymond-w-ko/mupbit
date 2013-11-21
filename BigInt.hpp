#pragma once

class BigInt {
  friend BigInt operator+(const BigInt& lhs, const BigInt& rhs);
 public:
  // 0 constructor
  BigInt();

  BigInt(unsigned int n);
  BigInt(int n);

  ~BigInt();
  BigInt(const BigInt& other);
  BigInt& operator=(BigInt other);
  BigInt(BigInt&& other);

 private:
  void swap(BigInt& other);

  char sign;
  std::vector<uint32_t> digits;
};

BigInt operator+(const BigInt& lhs, const BigInt& rhs);
