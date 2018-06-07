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

#ifndef STRINGSTREAMWRITER_H
#define STRINGSTREAMWRITER_H

#include <sstream>

#include "writeable.h"

namespace rcp {

    /*
     * StringStreamWriter writes big-endian to the stream converting if needed
    */
    class StringStreamWriter : public Writer
    {
    public:
        StringStreamWriter() {}

        virtual void write(const bool& c);
        virtual void write(const char& c);
        virtual void write(const uint8_t& c);
        virtual void write(const int8_t& c);
        virtual void write(const uint16_t& c);
        virtual void write(const int16_t& c);
        virtual void write(const uint32_t& c);
        virtual void write(const int32_t& c);
        virtual void write(const uint64_t& c);
        virtual void write(const int64_t& c);
        virtual void write(const float& c);
        virtual void write(const double& c);
        virtual void write(const std::string& s, bool prefix = true);
        virtual void write(const rcp::Color& s);
        virtual void write(const rcp::IPv4& s);
        virtual void write(const rcp::IPv6& s);
        virtual void write(const char* data, uint32_t length);


        size_t getSize() {
            return buffer.str().size();
        }

        std::stringstream& getBuffer() {
            return buffer;
        }

        void dump() {
            size_t len = buffer.str().size();
            char data[len];

            buffer.seekg(0);
            buffer.get(data, len);

            for (size_t i=0; i<len; i++) {
                printf("0x%02X ", static_cast<uint8_t>(data[i]));
            }
            printf("\n");
        }

        void clear() { buffer.str(""); }

    private:
        std::stringstream buffer;
    };
}


#endif // STRINGSTREAMWRITER_H
