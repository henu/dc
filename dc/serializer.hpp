#ifndef DC_SERIALIZER_HPP
#define DC_SERIALIZER_HPP

#include "cast.hpp"
#include "chunkfile.hpp"

#include <cstdint>

namespace DC
{

class Serializable;

template<uint8_t> uint8_t getRefSize() { return 1; }
template<uint16_t> uint8_t getRefSize() { return 2; }
template<uint32_t> uint8_t getRefSize() { return 4; }
template<uint64_t> uint8_t getRefSize() { return 8; }
template<typename T> uint8_t getRefSize() { return 8; }

void storeToFile(Chunkfile::Bytes& result_ref, Chunkfile* f, uint32_t* item);
void storeToFile(Chunkfile::Bytes& result_ref, Chunkfile* f, Serializable* obj);

template<uint8_t> uint8_t getFromFile(uint8_t* buf, Chunkfile* f) { (void)f; return *buf; }
template<uint16_t> uint16_t getFromFile(uint8_t* buf, Chunkfile* f) { (void)f; return bytesToUInt16(buf); }
template<uint32_t> uint32_t getFromFile(uint8_t* buf, Chunkfile* f) { (void)f; return bytesToUInt32(buf); }
template<uint64_t> uint64_t getFromFile(uint8_t* buf, Chunkfile* f) { (void)f; return bytesToUInt64(buf); }
template<typename T> T getFromFile(uint8_t* buf, Chunkfile* f) { uint64_t chunk_id = bytesToUInt64(buf); return T(f, chunk_id); }

}

#endif
