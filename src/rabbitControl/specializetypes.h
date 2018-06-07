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

#ifndef SPECIALIZETYPES_H
#define SPECIALIZETYPES_H

#include "types.h"
#include "color.h"
#include "ip.h"
#include "range.h"

enum td_types {
    td_default,
    td_string,
    td_num,
    td_enum,
    td_uri,
    td_array,
    td_custom
};


namespace rcp {

    class Writeable;

    // default
    template <typename T>
    struct isSpecialType
    { static const td_types value = std::is_arithmetic<T>::value ? td_num : td_default; };

    template <>
    struct isSpecialType<bool>
    { static const td_types value = td_default; };

    template <>
    struct isSpecialType<std::string>
    { static const td_types value = td_string; };

    template <typename T>
    struct isSpecialType<Range<T>>
    { static const td_types value = td_num; };


    // convert datatype
    template <typename T>
    struct convertDatatype
    { static const datatype_t value = DATATYPE_CUSTOMTYPE; };

    template <>
    struct convertDatatype<bool>
    { static const datatype_t value = DATATYPE_BOOLEAN; };

    template <>
    struct convertDatatype<int8_t>
    { static const datatype_t value = DATATYPE_INT8; };
    template <>
    struct convertDatatype<uint8_t>
    { static const datatype_t value = DATATYPE_UINT8; };

    template <>
    struct convertDatatype<int16_t>
    { static const datatype_t value = DATATYPE_INT16; };
    template <>
    struct convertDatatype<uint16_t>
    { static const datatype_t value = DATATYPE_UINT16; };

    template <>
    struct convertDatatype<int32_t>
    { static const datatype_t value = DATATYPE_INT32; };
    template <>
    struct convertDatatype<uint32_t>
    { static const datatype_t value = DATATYPE_UINT32; };

    template <>
    struct convertDatatype<int64_t>
    { static const datatype_t value = DATATYPE_INT64; };
    template <>
    struct convertDatatype<uint64_t>
    { static const datatype_t value = DATATYPE_UINT64; };

    template <>
    struct convertDatatype<float>
    { static const datatype_t value = DATATYPE_FLOAT32; };
    template <>
    struct convertDatatype<double>
    { static const datatype_t value = DATATYPE_FLOAT64; };

    template <>
    struct convertDatatype<std::string>
    { static const datatype_t value = DATATYPE_STRING; };

}

#endif // SPECIALIZETYPES_H
