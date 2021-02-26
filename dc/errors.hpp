#ifndef DC_ERRORS_HPP
#define DC_ERRORS_HPP

#include <stdexcept>

namespace DC
{

class UnsupportedVersion : public std::runtime_error
{
public:
    inline UnsupportedVersion() :
        std::runtime_error("Unsupported version.")
    {
    }
};

class CorruptedFile : public std::runtime_error
{
public:
    inline CorruptedFile() :
        std::runtime_error("Corrupted file.")
    {
    }
};

class Overflow : public std::runtime_error
{
public:
    inline Overflow() :
        std::runtime_error("Overflow.")
    {
    }
};

class FileModifiedBySomebodyElse : public std::runtime_error
{
public:
    inline FileModifiedBySomebodyElse() :
        std::runtime_error("File modified by somebody else.")
    {
    }
};

}

#endif
