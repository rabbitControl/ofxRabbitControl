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

#ifndef WRITER_H
#define WRITER_H

#include <inttypes.h>

#include "color.h"
#include "range.h"
#include "ip.h"

namespace rcp {

    class Writer {
    public:
        virtual ~Writer();

        virtual void write(const bool& c) = 0;
        virtual void write(const char& c) = 0;
        virtual void write(const uint8_t& c) = 0;
        virtual void write(const int8_t& c) = 0;
        virtual void write(const uint16_t& c) = 0;
        virtual void write(const int16_t& c) = 0;
        virtual void write(const uint32_t& c) = 0;
        virtual void write(const int32_t& c) = 0;
        virtual void write(const uint64_t& c) = 0;
        virtual void write(const int64_t& c) = 0;
        virtual void write(const float& c) = 0;
        virtual void write(const double& c) = 0;
        virtual void write(const std::string& s, bool prefix = true) = 0;
        virtual void write(const rcp::Color& s) = 0;
        virtual void write(const rcp::IPv4& s) = 0;
        virtual void write(const rcp::IPv6& s) = 0;
        virtual void write(const char* data, uint32_t length) = 0;

        template<typename T>
        void write(const Range<T>& c) {
            write(c.value1());
            write(c.value2());
        }

        void writeTinyString(const std::string& s){
            if (s.length() >= UINT8_MAX) {
                write(UINT8_MAX);
                write(s.substr(0, UINT8_MAX), false); // write without length-prefix
            } else {
                write(static_cast<uint8_t>(s.length()));
                write(s, false); // write without length-prefix
            }
        }
        void writeShortString(const std::string& s){
            if (s.length() >= UINT16_MAX) {
                write(UINT16_MAX);
                write(s.substr(0, UINT16_MAX), false); // write without length-prefix
            } else {
                write(static_cast<uint16_t>(s.length()));
                write(s, false); // write without length-prefix
            }
        }
        void writeString(const std::string& s){
            if (s.length() >= UINT32_MAX) {
                write(UINT32_MAX);
                write(s.substr(0, UINT32_MAX), false); // write without length-prefix
            } else {
                write(static_cast<uint32_t>(s.length()));
                write(s, false); // write without length-prefix
            }
        }
    };


    template<typename T>
    Writer& operator<<(Writer& out, const T& c)
    {
        out.write(c);
        return out;
    }

    template<typename T>
    Writer& operator<<(Writer& out, const Range<T>& c)
    {
        out.write(c.value1());
        out.write(c.value2());
        return out;
    }

}

#endif // WRITER_H
