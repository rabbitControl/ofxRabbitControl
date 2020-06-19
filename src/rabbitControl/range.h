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

#ifndef RCP_RANGE_H
#define RCP_RANGE_H

#include <type_traits>

namespace rcp {

    template <class T,
              typename = std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value > >
    class Range {
    public:
        Range() :
            m_value1(0)
          , m_value2(0)
        {}

        Range(T v1, T v2) :
            m_value1(v1)
          , m_value2(v2)
        {}

        void set(T v1, T v2) {
            m_value1 = v1;
            m_value2 = v2;
        }

        void setValue1(T v) {
            m_value1 = v;
        }

        void setValue2(T v) {
            m_value2 = v;
        }

        T value1() const {
            return m_value1;
        }

        T value2() const {
            return m_value2;
        }


        bool operator==(const Range<T>& other) {
            return m_value1 == other.m_value1 && m_value2 == other.m_value2;
        }

        Range& operator=(const Range& other) {
            m_value1 = other.m_value1;
            m_value2 = other.m_value2;
            return *this;
        }

    private:
        T m_value1{0};
        T m_value2{0};
    };

}

#endif // RCP_RANGE_H
