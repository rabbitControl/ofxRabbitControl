#ifndef VERSIONDATA_H
#define VERSIONDATA_H

#include "writeable.h"

namespace rcp {

    class VersionData : public Writeable {

    public:
        VersionData(const std::string& data) : m_data(data) {}
        ~VersionData() {}

        virtual void write(Writer& out, bool all) {
            out.writeTinyString(m_data);
        }

    private:
        std::string m_data;
    };

    typedef std::shared_ptr<VersionData> VersionDataPtr;
}

#endif // VERSIONDATA_H
