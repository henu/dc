#ifndef DC_SERIALIZABLE_HPP
#define DC_SERIALIZABLE_HPP

#include "cast.hpp"
#include "chunkfile.hpp"

namespace DC
{

class Serializable
{

public:

    inline Serializable() :
        my_owned_file(NULL),
        parent_file(NULL)
    {
    }

    inline Serializable(Chunkfile* file) :
        my_owned_file(NULL),
        parent_file(file)
    {
    }

    inline Serializable(std::string const& path) :
        my_owned_file(new Chunkfile(path)),
        parent_file(NULL)
    {
    }

    inline ~Serializable()
    {
        delete my_owned_file;
    }

    inline void setParentFile(Chunkfile* file)
    {
        assert(!parent_file);
        parent_file = file;
    }

    virtual void connectToFile(uint64_t chunk_id) = 0;

protected:

    inline Chunkfile* getFile()
    {
        if (my_owned_file) {
            return my_owned_file;
        }
        return parent_file;
    }

private:

    Chunkfile* my_owned_file;
    Chunkfile* parent_file;
};

}

#endif
