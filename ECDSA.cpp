#include "StableHeaders.hpp"
#include "ECDSA.hpp"

const BigInt ECDSA::p(
    "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");

const BigInt ECDSA::a(0);
const BigInt ECDSA::b(7);

//const BigInt ECDSA::n(
    //"fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
//const BigInt ECDSA::h(1);

const BigInt ECDSA::Gx(
    "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
const BigInt ECDSA::Gy(
    "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");

BigInt ECDSA::ModP(BigInt num) {
  if (num < BigInt::ZERO) {
    auto qr = num / ECDSA::p;
    num = qr.second + ECDSA::p;
  }

  if (num > ECDSA::p) {
    auto qr = num / ECDSA::p;
    num = qr.second;
  }

  return num;
}

// http://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
BigInt ECDSA::Inverse(BigInt num) {
  assert(num > 0 && num < ECDSA::p);

  BigInt t = BigInt::ZERO; BigInt newt = BigInt::ONE;
  BigInt r = ECDSA::p;     BigInt newr = num;

  BigInt tmp(0);
  while (newr != BigInt::ZERO) {
    auto qr = r / newr;
    BigInt quotient = qr.first;

    tmp = newr;
    newr = r - (quotient * newr);
    r = tmp;

    tmp = newt;
    newt = t - (quotient * newt);
    t = tmp;
  }

  assert(r <= BigInt::ONE);
  if (t < BigInt::ZERO) {
    t = t + ECDSA::p;
  }
  return t;
}

Point ECDSA::Add(Point P, Point Q) {
  Point r;

  BigInt numerator = Q.y - P.y;
  numerator = ModP(numerator);

  BigInt denominator = Q.x - P.x;
  denominator = ModP(denominator);

  BigInt lambda = numerator * Inverse(denominator);
  lambda = ModP(lambda);

  r.x = (lambda ^ BigInt(2)) - P.x - Q.x;
  r.x = ModP(r.x);

  r.y = (lambda * (P.x - r.x)) - P.y;
  r.y = ModP(r.y);

  return r;
}

Point ECDSA::Double(Point P) {
  Point r;

  BigInt numerator = BigInt(3) * (P.x ^ 2) + a;
  numerator = ModP(numerator);

  BigInt denominator = BigInt(2) * P.y;
  denominator = ModP(denominator);

  BigInt lambda = numerator * Inverse(denominator);
  lambda = ModP(lambda);

  r.x = (lambda ^ BigInt(2)) - (BigInt(2) * P.x);
  r.x = ModP(r.x);

  r.y = (lambda * (P.x - r.x)) - P.y;
  r.y = ModP(r.y);

  return r;
}
