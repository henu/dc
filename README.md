DiskContainers
==============

C++ library that implements an STL like containers that live on disk instead of memory.


Example
=======

```
DC::Vector<DC::String> vec("/tmp/string_vector.dc");

// Print current items
for (unsigned i = 0; i < vec.size(); ++ i) {
    std::cout << vec[i].toStdString() << std::endl;
}

// Add new item
vec.push(DC::String("TEST"));
```
