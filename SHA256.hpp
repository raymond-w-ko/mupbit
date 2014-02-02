#pragma once

class SHA256 {
 public:
  SHA256();
  ~SHA256();

  void hash(const uint8_t* data, uint64_t len);
  void hash(const char* data, uint64_t len);
  void hash(std::string str);

  std::string toString();

 private:
  void reset();
  void processChunk(const uint8_t* chunk);
  void createHashcodeBytes();

  uint32_t h0, h1, h2, h3, h4, h5, h6, h7;
  uint8_t hashcode[32];
};
