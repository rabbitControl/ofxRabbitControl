#ifndef SEMVER_H
#define SEMVER_H

#include <iostream>
#include <regex>
#include <string>

namespace rcp {

    class semver {

    public:
        semver(const std::string& s) {

            std::regex regex("\\-");

            std::vector<std::string> out(
                            std::sregex_token_iterator(s.begin(), s.end(), regex, -1),
                            std::sregex_token_iterator()
                            );

            for (auto &s: out) {
                std::cout << s << '\n';
            }

        }

        bool operator<(const semver& other) {
            return (m_major < other.m_major &&
                    m_minor < other.m_minor &&
                    m_patch < other.m_patch);
        }

        bool operator>(const semver& other) {
            return (m_major > other.m_major &&
                    m_minor > other.m_minor &&
                    m_patch > other.m_patch);
        }

    private:
        int m_major{0};
        int m_minor{0};
        int m_patch{0};
        std::string m_rest;
    };
}

#endif // SEMVER_H
