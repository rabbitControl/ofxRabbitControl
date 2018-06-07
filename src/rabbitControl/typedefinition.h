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

#ifndef RCP_TYPEDEFINITION_H
#define RCP_TYPEDEFINITION_H

#include <string>

#include "types.h"
#include "writeable.h"
#include "optionparser.h"

namespace rcp {

    static const std::string empty_string = "";

    class ITypeDefinition : public virtual Writeable, public IOptionparser
    {
    public:
        virtual ~ITypeDefinition();
        virtual datatype_t getDatatype() const = 0;
        virtual void dump() = 0;
    };


    template<typename T>
    class IDefaultDefinition : public virtual ITypeDefinition
    {
    public:
        virtual const T& getDefault() const = 0;
        virtual void setDefault(const T& defaultValue) = 0;
        virtual bool hasDefault() const = 0;
        virtual void clearDefault() = 0;

        virtual T readValue(std::istream& is) = 0;
    };


    template<typename T>
    class INumberDefinition : public virtual IDefaultDefinition<T>
    {
    public:
        virtual T getMinimum() const = 0;
        virtual void setMinimum(const T& min) = 0;
        virtual bool hasMinimum() const = 0;
        virtual void clearMinimum() = 0;

        virtual T getMaximum() const = 0;
        virtual void setMaximum(const T& min) = 0;
        virtual bool hasMaximum() const = 0;
        virtual void clearMaximum() = 0;

        virtual T getMultipleof() const = 0;
        virtual void setMultipleof(const T& min) = 0;
        virtual bool hasMultipleof() const = 0;
        virtual void clearMultipleof() = 0;

        virtual number_scale_t getScale() const = 0;
        virtual void setScale(const number_scale_t& scale) = 0;
        virtual bool hasScale() const = 0;
        virtual void clearScale() = 0;

        virtual std::string getUnit() const = 0;
        virtual void setUnit(const std::string& unit) = 0;
        virtual bool hasUnit() const = 0;
        virtual void clearUnit() = 0;
    };


    /*
    Value Typedefinition class
    with partial type specialisation
    */
    template<
            typename T,
            datatype_t type_id,
            td_types t
    >
    class TypeDefinition;


    //
    template<typename T,
             datatype_t type_id,
             td_types t
    >
    std::ostream& operator<<(std::ostream& out, rcp::TypeDefinition<T, type_id, t>& type);
}

#endif // RCP_TYPEDEFINITION_H
