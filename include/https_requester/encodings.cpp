#include "https_requester.h"
#include <iostream>
#include <string>
#include <zlib.h>

using namespace std;

string decompressGZip(const string& compressedData) {
    if (compressedData.empty()) return "";

    // Estimated initial output size (2x compressed size)
    size_t estimatedSize = compressedData.size() * 2;
    string decompressedData(estimatedSize, '\0');

    z_stream strm = {};
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    strm.avail_in = compressedData.size();
    strm.next_out = reinterpret_cast<Bytef*>(&decompressedData[0]);
    strm.avail_out = decompressedData.size();

    // Initialize zlib for GZip decompression
    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
        cerr << "Failed to initialize zlib decompression!" << endl;
        return "";
    }

    // Perform decompression
    int result;
    while ((result = inflate(&strm, Z_NO_FLUSH)) == Z_OK) {
        // Expand output buffer if needed
        if (strm.avail_out == 0) {
            size_t oldSize = decompressedData.size();
            decompressedData.resize(oldSize * 2);
            strm.next_out = reinterpret_cast<Bytef*>(&decompressedData[oldSize]);
            strm.avail_out = oldSize;
        }
    }

    // Clean up
    inflateEnd(&strm);

    if (result != Z_STREAM_END) {
        cerr << "Decompression failed! Error code: " << result << endl;
        return "";
    }

    // Resize to actual output size
    decompressedData.resize(strm.total_out);
    return decompressedData;
}