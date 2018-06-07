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

#include "stream_tools.h"

namespace rcp {

    Color& swap_endian(Color& u) {
        u.setValue(swap_endian(u.getValue()));
        return u;
    }

    std::string& swap_endian(std::string &u) {
        return u;
    }

    IPv4& swap_endian(IPv4 &u) {
        return  u;
    }

    IPv6& swap_endian(IPv6 &u) {
        return u;
    }



    //---------------------------------------------------
    // read from stream

    Color readFromStream(std::istream& is, const Color& i) {

        uint32_t value;
        is.read(reinterpret_cast<char *>(&value), sizeof(uint32_t));

#if BYTE_ORDER == LITTLE_ENDIAN
        value = swap_endian(value);
#endif

        return Color(value);
    }

    std::string readFromStream(std::istream& is, const std::string& i) {
        return readLongString(is);
    }

    IPv4 readFromStream(std::istream& is, const IPv4& i) {

        uint32_t value;
        is.read(reinterpret_cast<char *>(&value), sizeof(uint32_t));

#if BYTE_ORDER == LITTLE_ENDIAN
        value = swap_endian(value);
#endif

        return IPv4(value);
    }

    IPv6 readFromStream(std::istream& is, const IPv6& i) {

        uint32_t val1;
        uint32_t val2;
        uint32_t val3;
        uint32_t val4;

        is.read(reinterpret_cast<char *>(&val1), sizeof(uint32_t));
        is.read(reinterpret_cast<char *>(&val2), sizeof(uint32_t));
        is.read(reinterpret_cast<char *>(&val3), sizeof(uint32_t));
        is.read(reinterpret_cast<char *>(&val4), sizeof(uint32_t));

#if BYTE_ORDER == LITTLE_ENDIAN
        val1 = swap_endian(val1);
        val2 = swap_endian(val2);
        val3 = swap_endian(val3);
        val4 = swap_endian(val4);
#endif
        return IPv6(val1, val2, val3, val4);
    }


    // read strings from stream
    std::string readTinyString(std::istream& is) {

        char size;
        return readStringFromStream(is, size);
    }

    std::string readShortString(std::istream& is) {

        uint16_t size;
        return readStringFromStream(is, size);
    }

    std::string readLongString(std::istream& is) {

        uint32_t size;
        return readStringFromStream(is, size);
    }



    //---------------------------------------------------
    // << operator used in dump-method...
    std::ostream& operator<<(std::ostream& out, const Color& v) {
        out << v.getValue();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const IPv4& v) {
        struct in_addr a;
        a.s_addr = v.getIp();
        out << std::string(inet_ntoa(a));
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const IPv6& v) {

        char str[INET6_ADDRSTRLEN];

        // now get it back and print it
        inet_ntop(AF_INET6, (v.getIp().__u6_addr.__u6_addr32), str, INET6_ADDRSTRLEN);

        // hmm??
        out << std::string(str);
        return out;
    }

}
