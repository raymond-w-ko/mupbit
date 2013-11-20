#pragma once

class RIPEMD160 {
 public:

  RIPEMD160();
  ~RIPEMD160();

  void hash(const uint8_t* _data, uint64_t _len);
  void hash(const char* data, uint64_t len);
  void hash(std::string str);

  std::string toString();

 private:
  enum {
    RMDsize =  160,
  };

  typedef uint32_t dword;
  typedef uint8_t byte;
  // if this line causes a compiler error, adapt the defintion of dword above
  typedef int the_correct_size_was_chosen [sizeof (dword) == 4? 1: -1];

  void MDinit(dword *MDbuf);
  void compress(dword *MDbuf, dword *X);
  void MDfinish(dword *MDbuf, const byte *strptr, dword lswlen, dword mswlen);

  byte hashcode[RMDsize / 8];
};
