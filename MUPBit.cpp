#include "StableHeaders.hpp"
#include "SHA256.hpp"
#include "RIPEMD160.hpp"
#include "BigInt.hpp"
#include "ECDSA.hpp"

#ifdef _DEBUG
#  define INTEGRITY_ERROR { assert(false); return false; }
#else
#  define INTEGRITY_ERROR { return false; }
#endif

static bool doSHA256Checks() {
  SHA256 hasher;

  // empty test
  hasher.hash(std::string(""));
  if (hasher.toString() != "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") INTEGRITY_ERROR;

  // 1 char test
  hasher.hash(std::string("a"));
  if (hasher.toString() != "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb") INTEGRITY_ERROR;

  // 1 chunk, < 448 bits
  hasher.hash(std::string("The quick brown fox jumps over the lazy dog."));
  if (hasher.toString() != "ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c") INTEGRITY_ERROR;

  // 2 chunk, > 448 bits
  hasher.hash(std::string("Pneumonoultramicroscopicsilicovolcanoconiosis"));
  if (hasher.toString() != "0984363991e13bb6c2c6a9442d2ae50ec3b86843131fc02889f4ee2d1c8ca65c") INTEGRITY_ERROR;

  // 2 chunk
  hasher.hash(std::string("Pneumonoultramicroscopicsilicovolcanoconiosis is a really, really, really long word."));
  if (hasher.toString() != "15064c79fd7d70624a804acc001c0fb4529d2907f82e1af2f231b4832a93a573") INTEGRITY_ERROR;

  return true;
}

static bool doRIPEMD160Checks() {
  RIPEMD160 hasher;

  hasher.hash(std::string(""));
  if (hasher.toString() != "9c1185a5c5e9fc54612808977ee8f548b2258d31") INTEGRITY_ERROR;

  hasher.hash(std::string("a"));
  if (hasher.toString() != "0bdc9d2d256b3ee9daae347be6f4dc835a467ffe") INTEGRITY_ERROR;

  hasher.hash(std::string("abc"));
  if (hasher.toString() != "8eb208f7e05d987a9b044a8e98c6b087f15a0bfc") INTEGRITY_ERROR;

  hasher.hash(std::string("message digest"));
  if (hasher.toString() != "5d0689ef49d2fae572b881b123a85ffa21595f36") INTEGRITY_ERROR;

  hasher.hash(std::string("abcdefghijklmnopqrstuvwxyz"));
  if (hasher.toString() != "f71c27109c692c1b56bbdceb5b9d2865b3708dbc") INTEGRITY_ERROR;

  hasher.hash(std::string("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"));
  if (hasher.toString() != "12a053384a9c0c88e405a06c27dcf49ada62eb2b") INTEGRITY_ERROR;

  hasher.hash(std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
  if (hasher.toString() != "b0e20b6e3116640286ed3a87a5713079b21f5189") INTEGRITY_ERROR;

hasher.hash(std::string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"));
  if (hasher.toString() != "9b752e45573d4b39f4dbd3323cab82bf63326bfb") INTEGRITY_ERROR;

  return true;
}

static bool doBigIntChecks() {
  BigInt a = 0;
  BigInt b = 0;
  BigInt c = 0;

  a = 0;
  b = 0;
  c = a + b;
  if (c.str() != "0") INTEGRITY_ERROR;

  a = 1;
  b = 2;
  c = a + b;
  if (c.str() != "3") INTEGRITY_ERROR;

  a = 0xFFFFFFFF;
  b = 0xFFFFFFFF;
  c = a + b;
  if (c.str() != "1fffffffe") INTEGRITY_ERROR;

  a = "";
  if (a.str() != "0") INTEGRITY_ERROR;
  a = "1";
  if (a.str() != "1") INTEGRITY_ERROR;
  a = "+1";
  if (a.str() != "1") INTEGRITY_ERROR;
  a = "-1";
  if (a.str() != "-1") INTEGRITY_ERROR;
  a = "-123";
  if (a.str() != "-123") INTEGRITY_ERROR;
  a = "123";
  if (a.str() != "123") INTEGRITY_ERROR;
  a = "1fffffffe";
  if (a.str() != "1fffffffe") INTEGRITY_ERROR;
  a = "-1fffffffe";
  if (a.str() != "-1fffffffe") INTEGRITY_ERROR;
  // secp256k1 constants
  a = "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f";
  if (a.str() != "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f") INTEGRITY_ERROR;
  a = "0479be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8";
  if (a.str() != "479be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8") INTEGRITY_ERROR;

  a = 123;
  b = -345;
  c = -222;
  if ((a + b) != c) INTEGRITY_ERROR;
  a = -123;
  b = 345;
  c = 222;
  if ((a + b) != c) INTEGRITY_ERROR;
  a = -123;
  b = -345;
  c = -468;
  if ((a + b) != c) INTEGRITY_ERROR;

  a = "876543218765432187654321";
  b = "12341234";
  c = "876543218765432199995555";
  if ((a + b) != c) INTEGRITY_ERROR;
  if ((b + a) != c) INTEGRITY_ERROR;

  a = 123;
  b = -345;
  c = 468;
  if ((a - b) != c) INTEGRITY_ERROR;
  a = -123;
  b = 345;
  c = -468;
  if ((a - b) != c) INTEGRITY_ERROR;
  a = 123;
  b = 345;
  c = -222;
  if ((a - b) != c) INTEGRITY_ERROR;
  a = -123;
  b = -345;
  c = 222;
  if ((a - b) != c) INTEGRITY_ERROR;
  
  a = 16;
  b = 16;
  c = 256;
  if ((a * b) != c) INTEGRITY_ERROR;

  a = "123412341234123412341234123412341234";
  b = "1234123412341234000000000000000000000000";
  c = "-1233ffffffffffffedcbedcbedcbedcbedcbedcc";
  if ((a - b) != c) INTEGRITY_ERROR;

  a = "0479be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8";
  c = "1407d8be78249cd888f879013ad89d61b056e492ef173394d08a50a202635bc21ec11e40557bb9e35f5a9a0849c78ee8badaef6f710f44dda54e03094e29715c1a67053cad5f3bd952bccfafce1e187df36ad35d1c70fcd7df6a337b6772d2bfd9c372d1541278bdcbe4f80e3d9e8b333f432bee128fd65b7c0ac2a7cc17c14440";
  if ((a * a) != c) INTEGRITY_ERROR;
  if ((c / a).first != a) INTEGRITY_ERROR;

  //std::pair<BigInt, BigInt> qr = a / b;
  
  a = 42;
  b = 0;
  if ((a ^ b) != BigInt::ONE) INTEGRITY_ERROR;

  a = 42;
  b = 1;
  if ((a ^ b) != a) INTEGRITY_ERROR;

  a = 42;
  b = 42;
  c = "5919417cd6a11dbdf2f413657bbc03bc842ac76ef3932640000000000";
  if ((a ^ b) != c) INTEGRITY_ERROR;
  if ((c * BigInt::ZERO) != BigInt::ZERO) INTEGRITY_ERROR;

  a = "123412341234123412341234123412341234";
  b = "1234123412341234";
  c = "100000000000000010000";
  auto qr = a / b;
  if (qr.first != c) INTEGRITY_ERROR;
  c = "1234";
  if (qr.second != c) INTEGRITY_ERROR;

  return true;
}

static bool doECDSAChecks() {
  Point G;
  G.x = ECDSA::Gx;
  G.y = ECDSA::Gy;

  Point p = ECDSA::Double(G);
  return true;
}

static bool doIntegrityChecks() {
  if (!doSHA256Checks()) {
    std::cerr << "The SHA256 hash algorithm is not working correctly!\n";
    return false;
  }

  if (!doRIPEMD160Checks()) {
    std::cerr << "The RIPEMD160 hash algorithm is not working correctly!\n";
    return false;
  }

  if (!doBigIntChecks()) {
    std::cerr << "The big integer implementation is not working correctly!\n";
    return false;
  }

  if (!doECDSAChecks()) {
    std::cerr << "The ECDSA implementation is not working correctly!\n";
    return false;
  }


  return true;
}

int main(int argc, char* argv) {
  if (!doIntegrityChecks()) {
    return 1;
  }

  return 0;
}
