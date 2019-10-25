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

#ifndef RCP_IP_H
#define RCP_IP_H

#include <ostream>
#include <inttypes.h>

namespace rcp {

    class IPv4 {

    public:
        IPv4() {}
        IPv4(const uint32_t& ip) {
            m_ip = ip;
        }

        void setAddress(const uint32_t& ip) {
            m_ip = ip;
        }
        uint32_t getAddress() const { return m_ip; }

        bool operator==(const IPv4& other) {
            return m_ip == other.getAddress();
        }

    private:
        uint32_t m_ip;
    };



    class IPv6 {

    public:
        IPv6() {}
        IPv6(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4) {
            m_ip[0] = v1;
            m_ip[1] = v2;
            m_ip[2] = v3;
            m_ip[3] = v4;
        }

        void setAddress(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4) {
            m_ip[0] = v1;
            m_ip[1] = v2;
            m_ip[2] = v3;
            m_ip[3] = v4;
        }
        uint32_t getAddress(const int index) const {
            if (index < 0 || index > 3) {
                return 0;
            }
            return m_ip[index];
        }

        bool operator==(const IPv6& other) {
            return m_ip[0] == other.getAddress(0) &&
                    m_ip[1] == other.getAddress(1) &&
                    m_ip[2] == other.getAddress(2) &&
                    m_ip[3] == other.getAddress(3);
        }

    private:
        uint32_t m_ip[4];
    };


    std::ostream& operator<<(std::ostream& out, const IPv4& v);
    std::ostream& operator<<(std::ostream& out, const IPv6& v);

    IPv4& swap_endian(const IPv4 &u);
    IPv6& swap_endian(const IPv6 &u);

    IPv4 readFromStream(std::istream& is, const IPv4& i);
    IPv6 readFromStream(std::istream& is, const IPv6& i);

}

#endif // RCP_IP_H
