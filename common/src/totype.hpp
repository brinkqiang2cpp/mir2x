/*
 * =====================================================================================
 *
 *       Filename: totype.hpp
 *        Created: 03/07/2020 17:07:46
 *    Description: 
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#pragma once
#include <string>
#include <cstdint>
#include <stdexcept>

#define to_lld(x) static_cast<         long long>(x)
#define to_llu(x) static_cast<unsigned long long>(x)

#define to_u32(x) static_cast<uint32_t>(x)
#define to_u64(x) static_cast<uint64_t>(x)

#define to_cvptr(x) static_cast<const void *>(x)

inline const char * to_cstr(const char *s)
{
    if(s == nullptr){
        return "(null)";
    }
    else if(s[0] == '\0'){
        return "(empty)";
    }
    else{
        return s;
    }
}

inline const char *to_cstr(const unsigned char *s)
{
    return reinterpret_cast<const char *>(s);
}

inline const char *to_cstr(const char8_t *s)
{
    return reinterpret_cast<const char *>(s);
}

inline const char *to_cstr(const std::string &s)
{
    return to_cstr(s.c_str());
}

inline const char *to_cstr(const std::u8string &s)
{
    return to_cstr(s.c_str());
}

// cast char buf to char8_t buf
// this may break the strict-aliasing rule

inline const char8_t * to_u8cstr(const char8_t *s)
{
    if(s == nullptr){
        return u8"(null)";
    }
    else if(s[0] == '\0'){
        return u8"(empty)";
    }
    else{
        return s;
    }
}

inline const char8_t *to_u8cstr(const unsigned char *s)
{
    return reinterpret_cast<const char8_t *>(s);
}

inline const char8_t *to_u8cstr(const char *s)
{
    return reinterpret_cast<const char8_t *>(s);
}

inline const char8_t *to_u8cstr(const std::u8string &s)
{
    return to_u8cstr(s.c_str());
}

inline const char8_t *to_u8cstr(const std::string &s)
{
    return to_u8cstr(s.c_str());
}

inline const char *to_boolcstr(bool b)
{
    return b ? "true" : "false";
}

inline const char8_t *to_boolu8cstr(bool b)
{
    return b ? u8"true" : u8"false";
}

template<typename T, typename F> static T check_cast(F from)
{
    auto to = static_cast<T>(from);
    if(static_cast<F>(to) != from){
        throw std::runtime_error("cast fails to preserve original value");
    }
    return to;
}
