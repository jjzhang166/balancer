#pragma once

#include <util/system/defaults.h>
#include <util/generic/static_assert.h>

#define FNV32INIT  2166136261U
#define FNV32PRIME 16777619U
#define FNV64INIT  ULL(14695981039346656037)
#define FNV64PRIME ULL(1099511628211)

namespace NFnvPrivate {
    template <class It>
    inline ui32 FnvHash32(It b, It e, ui32 init) {
        while (b != e) {
            init = (init * FNV32PRIME) ^ (unsigned char)*b++;
        }

        return init;
    }

    template <class It>
    inline ui64 FnvHash64(It b, It e, ui64 init) {
        while (b != e) {
            init = (init * FNV64PRIME) ^ (unsigned char)*b++;
        }

        return init;
    }

    template <unsigned N>
    struct TFnvHelper;

#define DEF_FNV(t)\
    template <>\
    struct TFnvHelper<t> {\
        static const ui ## t Init = FNV ## t ## INIT;\
        template <class It>\
        static inline ui ## t FnvHash(It b, It e, ui ## t init) {\
            return FnvHash ## t (b, e, init);\
        }\
    };

    DEF_FNV(32)
    DEF_FNV(64)

#undef DEF_FNV
}

template <class T, class It>
static inline T FnvHash(It b, It e, T init) {
    STATIC_ASSERT(sizeof(*b) == 1);
    return (T)NFnvPrivate::TFnvHelper<8 * sizeof(T)>::FnvHash(b, e, init);
}

template <class T, class It>
static inline T FnvHash(It b, It e) {
    return FnvHash<T>(b, e, (T)NFnvPrivate::TFnvHelper<8 * sizeof(T)>::Init);
}

template <class T>
static inline T FnvHash(const void* buf, size_t len, T init) {
    return FnvHash<T>((const unsigned char*)buf, (const unsigned char*)buf + len, init);
}

template <class T>
static inline T FnvHash(const void* buf, size_t len) {
    return FnvHash<T>((const unsigned char*)buf, (const unsigned char*)buf + len);
}
