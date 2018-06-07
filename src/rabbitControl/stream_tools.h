/*
********************************************************************
* rabbitcontrol cpp
*
* written by: Ingo Randolf - 2018
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*********************************************************************
*/

#ifndef STREAMTOOLS_H
#define STREAMTOOLS_H

#include <istream>
#include <ostream>
#include <string>

#include "color.h"
#include "ip.h"
#include "range.h"


#define CHECK_STREAM if (is.eof()) { /* std::cerr << "could not read from stream\n";*/ break;}
#define CHECK_STREAM_RETURN(ret_val) if (is.eof()) { /* std::cerr << "could not read from stream\n"; */return ret_val;}
#define CHECK_STREAM_MSG(msg) if (is.eof()) { /* std::cerr << msg << "\n"; */  break;}
#define CHECK_STREAM_MSG_RETURN(msg) if (is.eof()) { /* std::cerr << msg << "\n"; */ return;}
#define CHECK_STREAM_MSG_RETURN_VAL(msg, ret_val) if (is.eof()) { /* std::cerr << msg << "\n"; */ return ret_val;}


namespace rcp {

    template <typename T>
    T swap_endian(const T& u)
    {
#ifdef CHAR_BIT
        static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
#endif

        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source, dest;

        source.u = u;

        for (size_t k = 0; k < sizeof(T); k++) {
            dest.u8[k] = source.u8[sizeof(T) - k - 1];
        }

        return dest.u;
    }

    // TODO do this with templates...?
    Color& swap_endian(const Color& u);
    std::string& swap_endian(const std::string &u);
    IPv4& swap_endian(const IPv4 &u);
    IPv6& swap_endian(const IPv6 &u);

    template <typename T>
    Range<T>& swap_endian(Range<T>& u) {
        u.setValue1(swap_endian(u.value1()));
        u.setValue2(swap_endian(u.value2()));
        return u;
    }



    //---------------------------------------------------
    // read from stream

    template <typename T>
    T readFromStream(std::istream& is, const T& i) {

        T value;
        is.read(reinterpret_cast<char *>(&value), sizeof(T));

#if BYTE_ORDER == LITTLE_ENDIAN
        value = swap_endian(value);
#endif

        return value;
    }

    Color readFromStream(std::istream& is, const Color& i);
    std::string readFromStream(std::istream& is, const std::string& i);
    IPv4 readFromStream(std::istream& is, const IPv4& i);
    IPv6 readFromStream(std::istream& is, const IPv6& i);



    template <typename T,
              typename = std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, T>>
    std::string readStringFromStream(std::istream& is, T s) {
        T size;
        is.read(reinterpret_cast<char *>(&size), sizeof(size));

#if BYTE_ORDER == LITTLE_ENDIAN
        size = swap_endian(size);
#endif

        // read to buffer
        char buffer[size];
        is.read(buffer, size);

        return std::string(buffer, size);
    }


    // read strings from stream
    std::string readTinyString(std::istream& is);
    std::string readShortString(std::istream& is);
    std::string readLongString(std::istream& is);


    //---------------------------------------------------
    // << operator used in dump-method...
    std::ostream& operator<<(std::ostream& out, const Color& v);
    std::ostream& operator<<(std::ostream& out, const IPv4& v);
    std::ostream& operator<<(std::ostream& out, const IPv6& v);

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const Range<T>& v) {
        out << v.value1();
        out << v.value2();
        return out;
    }

} // namespace rcp

#endif
