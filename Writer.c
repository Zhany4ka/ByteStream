#include <cstring>
#include <iostream>
#include <vector>
#include <zlib.h>
#include <bitset>

using namespace std;

class Writer {
 public:
  static void WriteScString(vector<unsigned char>& buffer, const string& value) {
    if (value.empty()) {
      buffer.push_back(0);
      buffer.push_back(0);
      buffer.push_back(0);
      buffer.push_back(0);
    } else {
      int length = value.size();
      buffer.push_back((length >> 24) & 0xff);
      buffer.push_back((length >> 16) & 0xff);
      buffer.push_back((length >> 8) & 0xff);
      buffer.push_back(length & 0xff);

      for (int i = 0; i < length; i++) {
        buffer.push_back(value[i]);
      }
    }
  }

  static void WriteVInt(vector<unsigned char>& buffer, int value) {
    bitset<32> bits(value);
    int length = bits.count();

    int first = (1 << (7 - length % 7)) - 1;
    first |= (value & ((1 << (7 - length % 7)) - 1)) << (length % 7);
    buffer.push_back(first);

    int pos = 7 - length % 7;
    while (pos < 32) {
      int next = value >> pos & 0x7f;
      if (pos + 7 < 32) {
        next |= 0x80;
      }
      buffer.push_back(next);
      pos += 7;
    }
  }

  static void WriteScId(vector<unsigned char>& buffer, int high, int low) {
    WriteVInt(buffer, high);
    WriteVInt(buffer, low);
  }

  static void WriteCompressedString(vector<unsigned char>& buffer,
                                     const string& value, bool indicate) {
    int dataLength = value.size();
    vector<unsigned char> data(dataLength);
    for (int i = 0; i < dataLength; i++) {
      data[i] = value[i];
    }

    uLongf compressedLength = compressBound(dataLength);
    vector<unsigned char> compressed(compressedLength);

    compress(&compressed[0], &compressedLength, &data[0], dataLength);

    if (indicate) {
      buffer.push_back(1);
    }

    buffer.push_back((compressedLength + 4) >> 24 & 0xff);
    buffer.push_back((compressedLength + 4) >> 16 & 0xff);
    buffer.push_back((compressedLength + 4) >> 8 & 0xff);
    buffer.push_back((compressedLength + 4) & 0xff);

    buffer.push_back(dataLength >> 24 & 0xff);
    buffer.push_back(dataLength >> 16 & 0xff);
    buffer.push_back(dataLength >> 8 & 0xff);
    buffer.push_back(dataLength & 0xff);

  }
     
