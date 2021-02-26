#ifndef DC_CAST_HPP
#define DC_CAST_HPP

#include "chunkfile.hpp"

#include <cstdint>

namespace DC
{

inline uint16_t bytesToUInt16(uint8_t const* bytes)
{
    return (bytes[0] << 0) +
           (bytes[1] << 8);
}

inline uint32_t bytesToUInt32(uint8_t const* bytes)
{
    return (bytes[0] << 0) +
           (bytes[1] << 8) +
           (bytes[2] << 16) +
           (bytes[3] << 24);
}

inline uint64_t bytesToUInt64(uint8_t const* bytes)
{
    return (bytes[0] << 0) +
           (bytes[1] << 8) +
           (bytes[2] << 16) +
           (bytes[3] << 24) +
           (uint64_t(bytes[4]) << 32) +
           (uint64_t(bytes[5]) << 40) +
           (uint64_t(bytes[6]) << 48) +
           (uint64_t(bytes[7]) << 56);
}

inline void uInt16ToBytes(Chunkfile::Bytes& result, uint16_t i)
{
    result.push_back((i >> 0) & 0xff);
    result.push_back((i >> 8) & 0xff);
}

inline void uInt32ToBytes(Chunkfile::Bytes& result, uint32_t i)
{
    result.push_back((i >> 0) & 0xff);
    result.push_back((i >> 8) & 0xff);
    result.push_back((i >> 16) & 0xff);
    result.push_back((i >> 24) & 0xff);
}

inline void uInt64ToBytes(Chunkfile::Bytes& result, uint64_t i)
{
    result.push_back((i >> 0) & 0xff);
    result.push_back((i >> 8) & 0xff);
    result.push_back((i >> 16) & 0xff);
    result.push_back((i >> 24) & 0xff);
    result.push_back((i >> 32) & 0xff);
    result.push_back((i >> 40) & 0xff);
    result.push_back((i >> 48) & 0xff);
    result.push_back((i >> 56) & 0xff);
}

}

#endif
