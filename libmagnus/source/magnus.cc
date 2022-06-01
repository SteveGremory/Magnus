#include "magnus.hh"
#include <filesystem>
#include <string.h>

// For mmap
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace LibMagnus {

Magnus::Magnus(std::string filepath)
{
    // Check if the file exists
    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error("[ERROR]: File does not exist.");
    }

    // Open the file, UNIX style
    _fd = open(filepath.c_str(), O_RDONLY);
    if (_fd == -1)
        throw std::runtime_error("[ERROR]: Could not open file.");

    // Map the file
    map_file();

    // Populate data
    _data = std::string_view(static_cast<const char*>(_addr), static_cast<const char*>(_addr) + _length);
}

Magnus::~Magnus()
{
    munmap((void*)_addr, _length);
    close(_fd);
}

void Magnus::map_file()
{
    // Get file length
    struct stat sb;
    if (fstat(_fd, &sb) == -1)
        throw std::runtime_error("[ERROR]: Could not fstat");

    _length = sb.st_size;

    _addr = mmap(NULL, _length, PROT_READ, MAP_PRIVATE, _fd, 0u);
    if (_addr == MAP_FAILED)
        throw std::runtime_error("[ERROR]: Could not mmap");
}

std::string_view Magnus::get_data()
{
    return _data;
}
}
