#include "ip.h"
#include "stream_tools.h"

namespace rcp {

    std::ostream& operator<<(std::ostream& out, const IPv4& v) {
        out << v.getAddress();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const IPv6& v) {

        // hmm??
        out << v.getAddress(0);
        out << v.getAddress(1);
        out << v.getAddress(2);
        out << v.getAddress(3);
        return out;
    }

    IPv4& swap_endian(IPv4 &u) {
        return  u;
    }

    IPv6& swap_endian(IPv6 &u) {
        return u;
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

}
