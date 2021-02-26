#include "serializer.hpp"

#include "serializable.hpp"

namespace DC
{

void storeToFile(Chunkfile::Bytes& result_ref, Chunkfile* f, uint32_t* item)
{
    (void)f;
    uInt32ToBytes(result_ref, *item);
}

void storeToFile(Chunkfile::Bytes& result_ref, Chunkfile* f, Serializable* obj)
{
    // Do the actual storing to file
    uint64_t chunk_id = f->findFreeChunk();
    obj->setParentFile(f);
    obj->connectToFile(chunk_id);
    // Chunk ID will be the reference
    uInt64ToBytes(result_ref, chunk_id);
}

}
