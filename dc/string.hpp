#ifndef DC_STRING_HPP
#define DC_STRING_HPP

#include "errors.hpp"
#include "serializable.hpp"

#include <cstdlib>
#include <string>

namespace DC
{

class String : public Serializable
{

public:

    inline String(std::string const& str) :
        pending_str(str)
    {
    }

    inline String(Chunkfile* f, uint64_t chunk_id) :
        Serializable(f)
    {
        connectToFile(chunk_id);
    }

    inline std::string toStdString()
    {
        if (!getFile()) {
            return pending_str;
        }
        std::string result;
        getString(result, true);
        return result;
    }

    inline void connectToFile(uint64_t chunk_id) override
    {
        this->chunk_id = chunk_id;

        Chunkfile* file = getFile();
        if (!file->exists(chunk_id)) {
            Chunkfile::Bytes bytes;
            uInt16ToBytes(bytes, VERSION_0_FIRST_VERSION);
            bytes.insert(bytes.end(), pending_str.begin(), pending_str.end());
            file->set(chunk_id, bytes);
            pending_str.clear();
        } else {
            if (!pending_str.empty()) {
                throw CorruptedFile();
            }
            std::string str;
            getString(str);
            str_size = str.size();
        }
    }

private:

    static uint16_t const VERSION_0_FIRST_VERSION = 0;

    // The chunk that is reserved for references of this string
    uint64_t chunk_id;

    uint64_t str_size;

    std::string pending_str;

    inline void getString(std::string& result, bool verify_size = false)
    {
        Chunkfile::Bytes str_bytes = getFile()->getBytes(chunk_id);
        if (str_bytes.size() < 2) {
            throw CorruptedFile();
        }
        if (bytesToUInt16(&str_bytes[0]) != VERSION_0_FIRST_VERSION) {
            throw UnsupportedVersion();
        }
        if (verify_size && str_bytes.size() != 2 + str_size) {
            throw FileModifiedBySomebodyElse();
        }
        result = std::string((char const*)&str_bytes[2], str_bytes.size() - 2);
    }
};

}

#endif
