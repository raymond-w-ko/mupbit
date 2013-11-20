#include "StableHeaders.hpp"
#include "SHA256.hpp"

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

  // 1 chunk, > 448 bits
  hasher.hash(std::string("Pneumonoultramicroscopicsilicovolcanoconiosis"));
  if (hasher.toString() != "0984363991e13bb6c2c6a9442d2ae50ec3b86843131fc02889f4ee2d1c8ca65c") return false;

  // 2 chunk
  hasher.hash(std::string("Pneumonoultramicroscopicsilicovolcanoconiosis is a really, really, really long word."));
  if (hasher.toString() != "15064c79fd7d70624a804acc001c0fb4529d2907f82e1af2f231b4832a93a573") return false;

  return true;
}

static bool doIntegrityChecks() {
  if (!doSHA256Checks()) {
    std::cerr << "SHA256 hash algorithm is not working correctly!\n";
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
