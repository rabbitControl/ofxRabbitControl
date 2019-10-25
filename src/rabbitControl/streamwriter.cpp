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

#include "streamwriter.h"

namespace rcp {

    void StreamWriter::write(const bool& c) {
        os << c;
    }

    void StreamWriter::write(const char& c) {
        os << c;
    }

    void StreamWriter::write(const uint8_t& c) {
        os << c;
    }
    void StreamWriter::write(const int8_t& c) {
        os << c;
    }


    void StreamWriter::write(const uint16_t& c) {
        os << c;
    }
    void StreamWriter::write(const int16_t& c) {
        os << c;
    }


    void StreamWriter::write(const uint32_t& c) {
        os << c;
    }
    void StreamWriter::write(const int32_t& c) {
        os << c;
    }


    void StreamWriter::write(const uint64_t& c) {
        os << c;
    }
    void StreamWriter::write(const int64_t& c) {
        os << c;
    }


    void StreamWriter::write(const float& c) {
        os << c;
    }
    void StreamWriter::write(const double& c) {
        os << c;
    }


    void StreamWriter::write(const std::string& s, bool prefix) {
        if (prefix) {
            write(static_cast<uint32_t>(s.length()));
        }
        os << s;
    }


    void StreamWriter::write(const rcp::Color& s) {
        os << s.getValue();
    }


    void StreamWriter::write(const rcp::IPv4& s) {
        os << s.getAddress();
    }

    void StreamWriter::write(const rcp::IPv6& s) {
        for (int i=0; i<4; i++) {
            os << static_cast<uint32_t>(s.getAddress(i));
        }
    }


    void StreamWriter::write(const char* data, uint32_t length) {
        os.write(data, length);
    }

} // namespace
