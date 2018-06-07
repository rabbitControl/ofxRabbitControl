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

#include <string>
#include <arpa/inet.h>

namespace rcp {

    class IPv4 {

    public:
        IPv4() {}
        IPv4(const std::string& ip) {
            setAddress(ip);
        }
        IPv4(const uint32_t& ip) {
            memcpy(&m_ip, &ip, sizeof (in_addr_t));
        }

        void setAddress(const std::string& ip) {
            m_ip = inet_addr(ip.c_str());
        }
        std::string getAddress() const {
            struct in_addr in = {m_ip};
            return std::string(inet_ntoa(in));
        }
        in_addr_t getIp() const { return m_ip; }

        bool operator==(const IPv4& other) {
            return m_ip == other.getIp();
        }

    private:
        in_addr_t m_ip{};
    };



    class IPv6 {

    public:
        IPv6() {}
        IPv6(const std::string& ip) {
            setAddress(ip);
        }
        IPv6(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4) {
            buf.__u6_addr.__u6_addr32[0] = v1;
            buf.__u6_addr.__u6_addr32[1] = v2;
            buf.__u6_addr.__u6_addr32[2] = v3;
            buf.__u6_addr.__u6_addr32[3] = v4;
        }

        void setAddress(const std::string& ip) {
            int res = inet_pton(AF_INET6, ip.c_str(), &buf);
        }
        std::string getAddress() const {
            char dst[INET6_ADDRSTRLEN];
            const char *adr = inet_ntop(AF_INET6, &buf, dst, INET6_ADDRSTRLEN);
            return std::string(adr);
        }
        const in6_addr& getIp() const { return buf; }

        bool operator==(const IPv6& other) {
            return &buf == &other.getIp();
        }

    private:
        in6_addr buf{};
    };


}

#endif // RCP_IP_H
