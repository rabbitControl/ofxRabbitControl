#ifndef IDDATA_H
#define IDDATA_H

#include "writeable.h"
#include "stream_tools.h"

namespace rcp {

    class IdData;
    typedef std::shared_ptr<IdData> IdDataPtr;

    class IdData : public Writeable {
    public:
        //----------------------------------------
        // parser
        static IdDataPtr parse(std::istream& is) {

            // read mandatory
            int16_t parameter_id = readFromStream(is, parameter_id);
            return std::make_shared<IdData>(parameter_id);
        }

        static inline IdDataPtr create(const int16_t& id) {
            return std::make_shared<IdData>(id);
        }

        IdData(const int16_t& id) :
            m_id(id)
        {}

        int16_t getId() { return m_id; }


        //----------------------------------------
        // interface Writeable
        virtual void write(Writer& out, bool /*all*/) {

            // write mandatory
            out.write(m_id);
        }

    private:
        int16_t m_id;
    };
}
#endif // IDDATA_H
