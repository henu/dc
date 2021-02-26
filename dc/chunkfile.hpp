#ifndef CHUNKFILE_HPP
#define CHUNKFILE_HPP

#include <cassert>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

// Two file library (only .cpp and .hpp files are needed) that represents file
// as a vector of Chunks. Chunks are arrays of bytes. They are identified by
// their index number. Index number can also point to chunk that does not exist.
class Chunkfile
{

public:

    class CorruptedFile : public std::runtime_error
    {
    public:
        inline CorruptedFile() : std::runtime_error("Corrupted file!") {}
    };

    class UnsupportedVersion : public std::runtime_error
    {
    public:
        inline UnsupportedVersion() : std::runtime_error("Unsupported version!") {}
    };

    class ChunkDoesNotExist : public std::runtime_error
    {
    public:
        inline ChunkDoesNotExist() : std::runtime_error("Chunk does not exist!") {}
    };

    typedef std::vector<uint8_t> Bytes;

    Chunkfile(std::string const& path);
    ~Chunkfile();

    void reserve(uint64_t chunks);

    bool exists(uint64_t chunk_id);

    void set(uint64_t chunk_id, uint8_t const* bytes, uint64_t size);

    inline void set(uint64_t chunk_id, std::string const& str)
    {
        set(chunk_id, (uint8_t const*)str.c_str(), str.size());
    }

    inline void set(uint64_t chunk_id, Bytes const& bytes)
    {
        set(chunk_id, &bytes[0], bytes.size());
    }

    uint64_t getChunkSize(uint64_t chunk_id);

    void get(uint8_t* result, uint64_t chunk_id);

    inline void get(Bytes& result, uint64_t chunk_id)
    {
        uint64_t chunk_size = getChunkSize(chunk_id);
        result.assign(chunk_size, 0);
        get((uint8_t*)&result[0], chunk_id);
    }

    inline Bytes getBytes(uint64_t chunk_id)
    {
        Bytes result;
        get(result, chunk_id);
        return result;
    }

    inline void get(std::string& result, uint64_t chunk_id)
    {
        uint64_t chunk_size = getChunkSize(chunk_id);
        result.assign(chunk_size, '\0');
        get((uint8_t*)&result[0], chunk_id);
    }

    inline std::string getString(uint64_t chunk_id)
    {
        std::string result;
        get(result, chunk_id);
        return result;
    }

    void del(uint64_t chunk_id);

    uint64_t findFreeChunk();

    void verify();

    void optimize();

private:

    // Chunk is divided to header and data parts. The header part
    // contains the following info:
    // 1) Absolute position of data part at data area, or 2^64-1 if not in use (64 bits)
    //
    // The data part contains the following info:
    // 1) Full size of data part (63 bits)
    // 2) Is in use, or is it free space (1 bit)
    // 3) Index number of chunk, if not free space (64 bits for actual data, 0 bits for free data)

    static unsigned const BUF_SIZE = 1024;
    static unsigned const HEADER_SIZE = 41;
    static unsigned const HEADER_MAGIC_AND_VERSION_SIZE = 17;
    static unsigned const HEADERPART_SIZE = 8;
    static unsigned const DATAPART_DATA_MIN_SIZE = 16;
    static unsigned const DATAPART_FREESPACE_MIN_SIZE = 8;

    static uint8_t const DATAPART_TYPE_FREESPACE = 0;
    static uint8_t const DATAPART_TYPE_DATA = 1;

    static uint64_t const MINUS_ONE = -1;

    static uint64_t const OPTIMIZE_THRESHOLD = 4;

    std::fstream file;

    uint64_t file_size;
    uint64_t chunks;
    uint64_t chunk_space_reserved;
    uint64_t total_data_part_empty_space;

    uint8_t* buf;

    void writeHeader();

    uint64_t findFreeSpace(uint64_t size, uint64_t min_limit = MINUS_ONE);

    uint64_t getDataPartPosition(uint64_t chunk_id);

    void moveDataPart(uint64_t datapart_pos, uint64_t new_datapart_pos);

    void optimizeHeaderParts();

    void optimizeDataParts();

    inline void readSeek(uint64_t seek)
    {
        file.seekg(seek);
    }

    inline void readBytes(uint8_t* result, uint64_t size)
    {
// TODO: Support caching and atomic operations!
        assert(!file.eof());
        file.read((char*)result, size);
        if (file.eof()) {
            throw CorruptedFile();
        }
    }

    inline void readString(std::string& result, uint64_t size)
    {
        result.assign(size, '\0');
        readBytes((uint8_t*)&result[0], size);
    }

    inline uint8_t readUInt8()
    {
        readBytes(buf, 1);
        return buf[0];
    }

    inline uint64_t readUInt64()
    {
        readBytes(buf, 8);
        uint64_t result = 0;
        result += uint64_t(buf[0]) << 0;
        result += uint64_t(buf[1]) << 8;
        result += uint64_t(buf[2]) << 16;
        result += uint64_t(buf[3]) << 24;
        result += uint64_t(buf[4]) << 32;
        result += uint64_t(buf[5]) << 40;
        result += uint64_t(buf[6]) << 48;
        result += uint64_t(buf[7]) << 56;
        return result;
    }

    inline void readUInt63AndUInt1(uint64_t& result1, uint8_t& result2)
    {
        uint64_t combined = readUInt64();
        result1 = combined & 0x7fffffffffffffff;
        result2 = combined >> 63;
    }

    inline void writeSeek(uint64_t seek)
    {
        file.seekp(seek);
    }

    inline void writeBytes(uint8_t const* bytes, uint64_t size)
    {
// TODO: Support caching and atomic operations!
        file.write((char const*)bytes, size);
    }

    inline void writeString(std::string const& str)
    {
        writeBytes((uint8_t const*)str.c_str(), str.size());
    }

    inline void writeUInt8(uint8_t i)
    {
        buf[0] = i;
        writeBytes(buf, 1);
    }

    inline void writeUInt64(uint64_t i)
    {
        buf[0] = (i >> 0) & 0xff;
        buf[1] = (i >> 8) & 0xff;
        buf[2] = (i >> 16) & 0xff;
        buf[3] = (i >> 24) & 0xff;
        buf[4] = (i >> 32) & 0xff;
        buf[5] = (i >> 40) & 0xff;
        buf[6] = (i >> 48) & 0xff;
        buf[7] = (i >> 56) & 0xff;
        writeBytes(buf, 8);
    }

    inline void writeUInt63AndUInt1(uint64_t i1, uint8_t i2)
    {
        writeUInt64((i1 & 0x7fffffffffffffff) + (uint64_t(i2) << 63));
    }

    inline void writeUnexpected(uint64_t size)
    {
// TODO: Maybe there is more optimal way of doing this? Some kind of system call that just increases the size?
        for (uint64_t i = 0; i < size; i += BUF_SIZE) {
            writeBytes(buf, std::min<uint64_t>(BUF_SIZE, size - i));
        }
    }

    inline uint64_t getFileSize()
    {
        file.seekg(0, std::ios::end);
        return file.tellg();
    }
};

#endif
