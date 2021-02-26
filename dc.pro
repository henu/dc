TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    dc/chunkfile.cpp \
    dc/serializer.cpp

HEADERS += \
    dc/cast.hpp \
    dc/serializable.hpp \
    dc/serializer.hpp \
    dc/vector.hpp \
    dc/string.hpp \
    dc/errors.hpp \
    dc/chunkfile.hpp

