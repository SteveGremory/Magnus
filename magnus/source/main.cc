#include <cmath>
#include <magnus.hh>

#include "sys/sysinfo.h"
#include "sys/types.h"
#include <compression/lz4_wrapper.hh>
#include <hashing/blake3_wrapper.hh>
#include <iostream>

int main()
{
    LibMagnus::Magnus new_magnus("/home/steve/Music/The Downward Spiral (1994)/14 - Hurt.fla");
    auto data = new_magnus.get_data();

    // Compression
    std::ofstream stream_compression("compressed.jpg.zst", std::ios::out | std::ios::binary);
    auto lz4_compressor = LZ4(data);

    lz4_compressor.compress();

    stream_compression.write(
        lz4_compressor.get_string()->data(),
        lz4_compressor.get_string()->size());

    // Decompression
    std::ofstream stream_decompression("decompressed.jpg", std::ios::out | std::ios::binary);

    auto compressed_string = lz4_compressor.get_string_view();
    auto LZ4_decompressor = LZ4(compressed_string);

    LZ4_decompressor.decompress();

    stream_decompression.write(
        LZ4_decompressor.get_string()->data(),
        LZ4_decompressor.get_string()->size());

    /*
    |-------------------|
    |    |    |    |    |
    |    |    |    |    |
    |    |    |    |    |
    |    |    |    |    |
    |___________________|

    first, divide into parts by:
        let f(x) = x % (ceil(sqrt(x)) + 2)
        (size_t)part_size = f(file_size)
        (float)number_of_parts = file_size / part_size

        Now, extract the decimal part from number_of_parts alongside the one's place;
        for example:
            123324.42 -> 123320 + 4.42
                 ____
    */

    double x = ceil(sqrt(data.size())) + 2;
    double part_size = remainder(x, data.size());
    double number_of_parts = data.size() / part_size;
    double last_packet_padding = 1 - modf(number_of_parts, &last_packet_padding);
    // HELL YEAH IT WORKS!
    // Now, add the padding to the end of the last packet.

    std::cout << "Part size: " << part_size << std::endl
              << "Number of parts: " << number_of_parts << std::endl
              << "Last packet's padding: " << last_packet_padding << std::endl
              << "Full file size: " << data.size() << std::endl
              << "Full file size reconstruction: " << (size_t)(part_size * number_of_parts) << std::endl;

    auto blake3_hash = BLAKE3(lz4_compressor.get_string_view());
    auto hash = blake3_hash.hash();
    std::cout << hash << std::endl;
}