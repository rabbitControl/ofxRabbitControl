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

#ifndef RCP_OPTION_H
#define RCP_OPTION_H

#include "specializetypes.h"

namespace rcp {

    template<typename T>
    class Option {
    public:
        Option() :
            m_hasValue(false)
        {}

        Option(const T& value) :
            m_value(value)
          , m_hasValue(true)
        {}

        ~Option()
        {}

        void setValue(const T& value) {
            m_value = value;
            m_hasValue = true;
        }

        bool hasValue() const {
            return m_hasValue;
        }

        T& getValue() {
            return m_value;
        }

        void clearValue() const {
            m_hasValue = false;
        }


        Option& operator=(T& value)
        {
            m_value = value;
            m_hasValue = true;
            return *this;
        }

        const Option& operator=(const T& value)
        {
            m_value = value;
            m_hasValue = true;
            return *this;
        }

    private:
        T m_value;
        bool m_hasValue;
    };

}

#endif
