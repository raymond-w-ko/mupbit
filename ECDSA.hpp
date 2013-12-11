#pragma once

#include "BigInt.hpp"

struct Point {
  Point() : x(0), y(0) {}

  BigInt x;
  BigInt y;
};

class ECDSA {
 public:
  static BigInt ModP(BigInt num);
  static BigInt Inverse(BigInt num);

  static Point Add(Point P, Point r);
  static Point Double(Point P);

  static const BigInt p;
  static const BigInt a;
  static const BigInt b;
  //static const BigInt n;
  //static const BigInt h;
  static const BigInt Gx;
  static const BigInt Gy;

 private:
};
