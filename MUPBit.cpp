#include "StableHeaders.hpp"
#include "SHA256.hpp"
#include "RIPEMD160.hpp"
#include "BigInt.hpp"

static bool doSHA256Checks() {
  SHA256 hasher;

  // empty test
  hasher.hash(std::string(""));
  if (hasher.toString() != "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") return false;

  // 1 char test
  hasher.hash(std::string("a"));
  if (hasher.toString() != "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb") return false;

  // 1 chunk, < 448 bits
  hasher.hash(std::string("The quick brown fox jumps over the lazy dog."));
  if (hasher.toString() != "ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c") return false;

  // 2 chunk, > 448 bits
  hasher.hash(std::string("Pneumonoultramicroscopicsilicovolcanoconiosis"));
  if (hasher.toString() != "0984363991e13bb6c2c6a9442d2ae50ec3b86843131fc02889f4ee2d1c8ca65c") return false;

  // 2 chunk
  hasher.hash(std::string("Pneumonoultramicroscopicsilicovolcanoconiosis is a really, really, really long word."));
  if (hasher.toString() != "15064c79fd7d70624a804acc001c0fb4529d2907f82e1af2f231b4832a93a573") return false;

  return true;
}

static bool doRIPEMD160Checks() {
  RIPEMD160 hasher;

  hasher.hash(std::string(""));
  if (hasher.toString() != "9c1185a5c5e9fc54612808977ee8f548b2258d31") return false;

  hasher.hash(std::string("a"));
  if (hasher.toString() != "0bdc9d2d256b3ee9daae347be6f4dc835a467ffe") return false;

  hasher.hash(std::string("abc"));
  if (hasher.toString() != "8eb208f7e05d987a9b044a8e98c6b087f15a0bfc") return false;

  hasher.hash(std::string("message digest"));
  if (hasher.toString() != "5d0689ef49d2fae572b881b123a85ffa21595f36") return false;

  hasher.hash(std::string("abcdefghijklmnopqrstuvwxyz"));
  if (hasher.toString() != "f71c27109c692c1b56bbdceb5b9d2865b3708dbc") return false;

  hasher.hash(std::string("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"));
  if (hasher.toString() != "12a053384a9c0c88e405a06c27dcf49ada62eb2b") return false;

  hasher.hash(std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
  if (hasher.toString() != "b0e20b6e3116640286ed3a87a5713079b21f5189") return false;

hasher.hash(std::string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"));
  if (hasher.toString() != "9b752e45573d4b39f4dbd3323cab82bf63326bfb") return false;

  return true;
}

static bool doBigIntChecks() {
  BigInt a = 0x89ABCDEF;
  BigInt b = 0xFEDCBA98;

  BigInt c = a + b + a + b;
  std::cout << c.str() << "\n";

  std::cout << (a - b).str() << "\n";

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
    std::cerr << "The RIPEMD160 hash algorithm is not working correctly!\n";
    return false;
  }

  return true;
}

int main(int argc, char* argv) {
  if (!doIntegrityChecks()) {
    assert(false);
    return 1;
  }

  return 0;
}
