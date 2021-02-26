#ifndef DC_VECTOR_HPP
#define DC_VECTOR_HPP

#include "chunkfile.hpp"
#include "errors.hpp"
#include "serializable.hpp"
#include "serializer.hpp"

#include <vector>

namespace DC
{

template<class T>
class Vector : public Serializable
{

public:

    inline Vector() :
        chunk_id(-1),
        items(0)
    {
    }

    inline Vector(std::string const& path) :
        Serializable(path),
        items(0)
    {
        connectToFile(0);
    }

    inline void push(T item)
    {
        // If file is open, then connect the item to it immediately
        Chunkfile* f = getFile();
        if (f) {
            // Store content of item to file and get its reference
            Chunkfile::Bytes ref;
            storeToFile(ref, f, &item);
            assert(ref.size() == getRefSize<T>());
            // Read current vector in byte format
            Chunkfile::Bytes v_bytes;
            getBytes(v_bytes);
            // Add reference to the vector
            v_bytes.insert(v_bytes.end(), ref.begin(), ref.end());
            getFile()->set(chunk_id, v_bytes);
        } else {
            pending_items.push_back(item);
        }
        ++ items;
    }

    inline uint64_t size()
    {
        return items;
    }

    inline T operator[](int64_t index)
    {
        if (index >= int64_t(items)) {
            throw Overflow();
        }
        if (index < 0) {
            index += items;
            if (index < 0) {
                throw Overflow();
            }
        }

        Chunkfile* f = getFile();
        if (f) {
            Chunkfile::Bytes v_bytes;
            getBytes(v_bytes, true);
            index = 2 + index * getRefSize<T>();
            return getFromFile<T>(&v_bytes[index], getFile());
        } else {
            return pending_items[index];
        }
    }

    inline void connectToFile(uint64_t chunk_id)
    {
// TODO: Handle pending items!
assert(pending_items.empty());

        this->chunk_id = chunk_id;

        Chunkfile* file = getFile();

        // First try to get existing chunk
        if (file->exists(chunk_id)) {
            Chunkfile::Bytes v_bytes = file->getBytes(chunk_id);
            // First read version
            if (v_bytes.size() < 2) {
                throw CorruptedFile();
            }
            uint16_t version = bytesToUInt16(&v_bytes[0]);
            if (version != VERSION_0_FIRST_VERSION) {
                throw UnsupportedVersion();
            }
            // Read number of items
            if ((v_bytes.size() - 2) % getRefSize<T>() != 0) {
                throw CorruptedFile();
            }
            items = (v_bytes.size() - 2) / getRefSize<T>();
        }
        // If it does not exist, then initialize a new one
        else {
            Chunkfile::Bytes version_bytes;
            uInt16ToBytes(version_bytes, VERSION_0_FIRST_VERSION);
            file->set(chunk_id, version_bytes);
        }
    }

private:

    static uint16_t const VERSION_0_FIRST_VERSION = 0;

    typedef std::vector<T> Vec;

    // The chunk that is reserved for references of this vector
    uint64_t chunk_id;

    uint64_t items;

    // Items that are not added to file, because file has not been defined yet
    Vec pending_items;

    inline void getBytes(Chunkfile::Bytes& result, bool verify_size = false)
    {
        result.clear();
        getFile()->get(result, chunk_id);
        if (result.size() < 2) {
            throw CorruptedFile();
        }
        if (bytesToUInt16(&result[0]) != VERSION_0_FIRST_VERSION) {
            throw UnsupportedVersion();
        }
        if ((result.size() - 2) % getRefSize<T>() != 0) {
            throw CorruptedFile();
        }
        if (verify_size && result.size() != 2 + items * getRefSize<T>()) {
            throw FileModifiedBySomebodyElse();
        }
    }
};

}

#endif
