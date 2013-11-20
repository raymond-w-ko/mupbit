#include "StableHeaders.hpp"
#include "SHA256.hpp"

uint32_t k[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
  0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
  0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
  0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
  0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
  0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

SHA256::SHA256() {
}

void SHA256::reset() {
  h0 = 0x6a09e667;
  h1 = 0xbb67ae85;
  h2 = 0x3c6ef372;
  h3 = 0xa54ff53a;
  h4 = 0x510e527f;
  h5 = 0x9b05688c;
  h6 = 0x1f83d9ab;
  h7 = 0x5be0cd19;
}

SHA256::~SHA256() {
}

static void printData(const std::vector<uint8_t>& data) {
  char buf[17];

  for (size_t i = 0; i < data.size(); ++i) {
    sprintf(buf, "%02x", data[i]);
    std::cout << buf;
  }
  std::cout << "\n";
}

void SHA256::hash(const uint8_t* data, uint64_t len) {
  this->reset();

  // preprocess
  std::vector<uint8_t> bytes(data, data + len);
  uint64_t ml = len * 8;
  uint64_t modLen = (ml + 1) % 512;
  uint64_t bitsToAdd;
  if (modLen <= 448) {
    bitsToAdd = 448 - modLen;
  } else {
    bitsToAdd = (512 - modLen) + 448;
  }
  bitsToAdd++;
  assert((bitsToAdd % 8) == 0);
  uint64_t bytesToAdd = bitsToAdd / 8;
  // pad with 1 bit == 1 and 0s for the rest
  bytes.push_back(0x80);
  bytesToAdd--;
  for (size_t i = 0; i < bytesToAdd; ++i) {
    bytes.push_back(0x00);
  }
  // append size
  uint64_t size = ml;
  uint64_t mask = 0xFF00000000000000ULL;
  int shift = 64 - 8;
  for (int i = 0; i < 8; ++i) {
    uint64_t byte = (size & mask) >> shift;
    mask >>= 8;
    shift -= 8;
    bytes.push_back(static_cast<uint8_t>(byte));
  }

  const ptrdiff_t chunkSizeInBytes = (512 / 8);
  uint64_t num_chunks = bytes.size() / chunkSizeInBytes;
  const uint8_t* ptr = &bytes[0];
  for (uint64_t i = 0; i < num_chunks; ++i) {
    processChunk(ptr);
    ptr += chunkSizeInBytes;
  }

  this->createHashcodeBytes();
}

void SHA256::hash(const char* data, uint64_t len) {
  assert(sizeof(char) == sizeof(uint8_t));
  this->hash(reinterpret_cast<const uint8_t*>(data), len);
}

void SHA256::hash(std::string str) {
  this->hash(str.c_str(), str.size());
}

static inline uint32_t bigEndianUnsigned(const uint8_t* ptr) {
  return
      (static_cast<uint32_t>(ptr[3]) << 0) +
      (static_cast<uint32_t>(ptr[2]) << 8) +
      (static_cast<uint32_t>(ptr[1]) << 16) +
      (static_cast<uint32_t>(ptr[0]) << 24);
}

static inline uint32_t rightrotate(uint32_t num, int bits) {
  return ((num >> bits) | (num << (32 - bits)));
}

void SHA256::processChunk(const uint8_t* chunk) {
  // this doesn't need to be initialized
  uint32_t w[64];

  for (int i = 0; i < 16; ++i) {
    w[i] = bigEndianUnsigned(chunk);
    chunk += 4;
  }

  for (int i = 16; i < 64; ++i) {
    uint32_t s0 = rightrotate(w[i-15], 7) ^ rightrotate(w[i-15], 18) ^ (w[i-15] >> 3);
    uint32_t s1 = rightrotate(w[i-2], 17) ^ rightrotate(w[i-2], 19) ^ (w[i-2] >> 10);
    w[i] = w[i-16] + s0 + w[i-7] + s1;
  }

  uint32_t a = h0;
  uint32_t b = h1;
  uint32_t c = h2;
  uint32_t d = h3;
  uint32_t e = h4;
  uint32_t f = h5;
  uint32_t g = h6;
  uint32_t h = h7;

  for (int i = 0; i < 64; ++i) {
    uint32_t S1 = rightrotate(e, 6) ^ rightrotate(e, 11) ^ rightrotate(e, 25);
    uint32_t ch = (e & f) ^ ((~e) & g);
    uint32_t temp1 = h + S1 + ch + k[i] + w[i];
    uint32_t S0 = rightrotate(a, 2) ^ rightrotate(a, 13) ^ rightrotate(a, 22);
    uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
    uint32_t temp2 = S0 + maj;
 
    h = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  }


  h0 += a;
  h1 += b;
  h2 += c;
  h3 += d;
  h4 += e;
  h5 += f;
  h6 += g;
  h7 += h;
}

std::string SHA256::toString() {
  char buf[9];
  std::string s;

  sprintf(buf, "%08x", h0);
  s += buf;
  sprintf(buf, "%08x", h1);
  s += buf;
  sprintf(buf, "%08x", h2);
  s += buf;
  sprintf(buf, "%08x", h3);
  s += buf;
  sprintf(buf, "%08x", h4);
  s += buf;
  sprintf(buf, "%08x", h5);
  s += buf;
  sprintf(buf, "%08x", h6);
  s += buf;
  sprintf(buf, "%08x", h7);
  s += buf;

  return s;
}

static inline void integerToBytes(uint32_t n, uint8_t* bytes) {
  bytes[0] = static_cast<uint8_t>((n & 0xFF000000) >> 24);
  bytes[1] = static_cast<uint8_t>((n & 0x00FF0000) >> 16);
  bytes[2] = static_cast<uint8_t>((n & 0x0000FF00) >>  8);
  bytes[3] = static_cast<uint8_t>((n & 0x000000FF)      );
}

void SHA256::createHashcodeBytes() {
  integerToBytes(h0, &hashcode[0]);
  integerToBytes(h1, &hashcode[4]);
  integerToBytes(h2, &hashcode[8]);
  integerToBytes(h3, &hashcode[12]);
  integerToBytes(h4, &hashcode[16]);
  integerToBytes(h5, &hashcode[20]);
  integerToBytes(h6, &hashcode[24]);
  integerToBytes(h7, &hashcode[28]);
}
