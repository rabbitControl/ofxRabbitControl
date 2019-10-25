#include "color.h"

#include "stream_tools.h"


namespace rcp {

    Color& swap_endian(Color& u) {
        u.setValue(swap_endian(u.getValue()));
        return u;
    }

    Color readFromStream(std::istream& is, const Color& i) {

        uint32_t value;
        is.read(reinterpret_cast<char *>(&value), sizeof(uint32_t));

    #if BYTE_ORDER == LITTLE_ENDIAN
        value = swap_endian(value);
    #endif

        return Color(value);
    }

    std::ostream& operator<<(std::ostream& out, const Color& v) {
        out << v.getValue();
        return out;
    }
}
