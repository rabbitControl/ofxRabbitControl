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

#ifndef RCP_PARAMETER_CUSTOM_H
#define RCP_PARAMETER_CUSTOM_H

#include "parameter_intern.h"

namespace rcp {

    template <typename T>
    class CustomParameter :
            public ValueParameter<T, TypeDefinition<T, DATATYPE_CUSTOMTYPE, td_custom>, DATATYPE_CUSTOMTYPE>
    {
    public:
        typedef TypeDefinition<T, DATATYPE_CUSTOMTYPE, td_custom> _CustomType;
        typedef ValueParameter<T, TypeDefinition<T, DATATYPE_CUSTOMTYPE, td_custom>, DATATYPE_CUSTOMTYPE> _CustomParameter;

        static ParameterPtr create(int16_t id) {
            return std::make_shared<CustomParameter<T> >(id);
        }

        CustomParameter(int16_t id) :
            _CustomParameter(id)
        {}

        ~CustomParameter()
        {}

        // convenience
        void setDefault(const T& v) {
            _CustomParameter::getDefaultTypeDefinition().setDefault(v);
        }

        void setUuid(char* uuid, uint8_t length) {
            _CustomParameter::getDefaultTypeDefinition().setUuid(uuid, length);
        }

        void setConfig(const std::vector<int8_t>& config) {
            _CustomParameter::getDefaultTypeDefinition().setConfig(config);
        }
    };
}

#endif
