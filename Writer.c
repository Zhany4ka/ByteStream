#include <cstring>
#include <iostream>
#include <vector>
#include <zlib.h>
#include <bitset>
#include <stdexcept>

using namespace std;

class Writer {
public:
    static void WriteScString(vector<unsigned char>& buffer, const string& value) {
        if (value.empty()) {
            buffer.insert(buffer.end(), 4, 0);
        } else {
            int length = value.size();
            WriteIntToBuffer(buffer, length);
            buffer.insert(buffer.end(), value.begin(), value.end());
        }
    }

    static void WriteVInt(vector<unsigned char>& buffer, int value) {
        if (value < 0) {
            throw invalid_argument("Negative values are not supported.");
        }

        if (value <= 0x3F) {
            buffer.push_back(static_cast<unsigned char>(value & 0x3F));
        } else if (value < 0x2000) {
            buffer.push_back(static_cast<unsigned char>((value & 0x3F) | 0x80));
            buffer.push_back(static_cast<unsigned char>((value >> 6) & 0x7F));
        } else if (value < 0x100000) {
            buffer.push_back(static_cast<unsigned char>((value & 0x3F) | 0x80));
            buffer.push_back(static_cast<unsigned char>(((value >> 6) & 0x3F) | 0x80));
            buffer.push_back(static_cast<unsigned char>((value >> 13) & 0x7F));
        } else {
            buffer.push_back(static_cast<unsigned char>((value & 0x3F) | 0x80));
            buffer.push_back(static_cast<unsigned char>(((value >> 6) & 0x3F) | 0x80));
            buffer.push_back(static_cast<unsigned char>(((value >> 13) & 0x3F) | 0x80));
            buffer.push_back(static_cast<unsigned char>((value >> 20) & 0x7F));
        }
    }

    static void WriteScId(vector<unsigned char>& buffer, int high, int low) {
        WriteVInt(buffer, high);
        WriteVInt(buffer, low);
    }

    static void WriteCompressedString(vector<unsigned char>& buffer, const string& value, bool indicate) {
        vector<unsigned char> data(value.begin(), value.end());
        uLongf compressedLength = compressBound(data.size());
        vector<unsigned char> compressed(compressedLength);

        if (compress(compressed.data(), &compressedLength, data.data(), data.size()) != Z_OK) {
            throw runtime_error("Compression failed.");
        }

        if (indicate) {
            buffer.push_back(1);
        }

        WriteIntToBuffer(buffer, compressedLength + 4); 
        WriteIntToBuffer(buffer, data.size()); 
        buffer.insert(buffer.end(), compressed.begin(), compressed.begin() + compressedLength);
    }

private:
    static void WriteIntToBuffer(vector<unsigned char>& buffer, int value) {
        buffer.push_back((value >> 24) & 0xff);
        buffer.push_back((value >> 16) & 0xff);
        buffer.push_back((value >> 8) & 0xff);
        buffer.push_back(value & 0xff);
    }
};
