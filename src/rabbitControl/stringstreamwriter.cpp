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

#include <iostream>

#include "stringstreamwriter.h"

namespace rcp {

    void StringStreamWriter::write(const bool& c) {
        buffer << static_cast<int8_t>(c);
    }

    void StringStreamWriter::write(const char& c) {
        buffer << static_cast<int8_t>(c);
    }

    void StringStreamWriter::write(const uint8_t& c) {
        buffer << c;
    }
    void StringStreamWriter::write(const int8_t& c) {
        buffer << c;
    }


    void StringStreamWriter::write(const uint16_t& v) {
        write(static_cast<int16_t>(v));
    }
    void StringStreamWriter::write(const int16_t& v) {
#if BYTE_ORDER == LITTLE_ENDIAN
        buffer << static_cast<unsigned char>(v >> 8);
        buffer << static_cast<unsigned char>(v);
#else
        buffer << static_cast<unsigned char>(v);
        buffer << static_cast<unsigned char>(v >> 8);
#endif
    }


    void StringStreamWriter::write(const uint32_t& v) {
        write(static_cast<int32_t>(v));
    }
    void StringStreamWriter::write(const int32_t& v) {
#if BYTE_ORDER == LITTLE_ENDIAN
        buffer << static_cast<unsigned char>(v >> 24);
        buffer << static_cast<unsigned char>(v >> 16);
        buffer << static_cast<unsigned char>(v >> 8);
        buffer << static_cast<unsigned char>(v);
#else
        buffer << static_cast<unsigned char>(v);
        buffer << static_cast<unsigned char>(v >> 8);
        buffer << static_cast<unsigned char>(v >> 16);
        buffer << static_cast<unsigned char>(v >> 24);
#endif

    }


    void StringStreamWriter::write(const uint64_t& v) {
        write(static_cast<int64_t>(v));
    }
    void StringStreamWriter::write(const int64_t& v) {
#if BYTE_ORDER == LITTLE_ENDIAN
        buffer << static_cast<unsigned char>(v >> 56);
        buffer << static_cast<unsigned char>(v >> 48);
        buffer << static_cast<unsigned char>(v >> 40);
        buffer << static_cast<unsigned char>(v >> 32);

        buffer << static_cast<unsigned char>(v >> 24);
        buffer << static_cast<unsigned char>(v >> 16);
        buffer << static_cast<unsigned char>(v >> 8);
        buffer << static_cast<unsigned char>(v);
#else
        buffer << static_cast<unsigned char>(v);
        buffer << static_cast<unsigned char>(v >> 8);
        buffer << static_cast<unsigned char>(v >> 16);
        buffer << static_cast<unsigned char>(v >> 24);

        buffer << static_cast<unsigned char>(v >> 32);
        buffer << static_cast<unsigned char>(v >> 40);
        buffer << static_cast<unsigned char>(v >> 48);
        buffer << static_cast<unsigned char>(v >> 56);
#endif
    }


    void StringStreamWriter::write(const float& value) {
        float fv = value;
        write(reinterpret_cast<uint32_t&>(fv));
    }


    void StringStreamWriter::write(const double& value) {
        double dv = value;
        write(reinterpret_cast<uint64_t&>(dv));
    }


    void StringStreamWriter::write(const std::string& s, bool prefix) {
        if (prefix) {
            write(static_cast<uint32_t>(s.length()));
        }
        buffer << s;
    }

    void StringStreamWriter::write(const rcp::Color& s) {
        write(s.getValue());
    }

    void StringStreamWriter::write(const rcp::IPv4& s) {
        write(s.getAddress());
    }

    void StringStreamWriter::write(const rcp::IPv6& s) {
        for (int i=0; i<4; i++) {
            write(static_cast<uint32_t>(s.getAddress(i)));
        }
    }

    void StringStreamWriter::write(const char* data, uint32_t length) {
        buffer.write(data, (std::streamsize)length);
    }
}

