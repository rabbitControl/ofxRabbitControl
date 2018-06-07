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

#ifndef RCP_TYPEDEFINITION_NOOPT_H
#define RCP_TYPEDEFINITION_NOOPT_H

#include <iostream>

#include "typedefinition.h"
#include "iparameter.h"

namespace rcp {

    template <datatype_t type>
    class TypeDefinitionNoOpt : public ITypeDefinition
    {
    public:
        TypeDefinitionNoOpt() : datatype(type)
        {}

        TypeDefinitionNoOpt(IParameter& param) :
            datatype(type)
        {}

        virtual datatype_t getDatatype() const { return datatype; }

        //------------------------------------
        // implement writeable
        void write(Writer& out, bool all) {

            out.write(static_cast<char>(datatype));

            // terminator
            out.write(static_cast<char>(TERMINATOR));
        }

        //------------------------------------
        // implement optionparser
        void parseOptions(std::istream& is) {
            // no options - expect terminator

            // read one byte
            int did = is.get();

            if (did != TERMINATOR) {
                std::cerr << "error - no-option typedefinition has no terminator!";
            }
        }

        virtual void dump() {
            std::cout << "--- type with no options ---\n";
        }

    private:
        // mandatory
        datatype_t datatype;
    };

    //
    typedef TypeDefinitionNoOpt<DATATYPE_BANG> BangTypeDefinition;
    typedef TypeDefinitionNoOpt<DATATYPE_GROUP> GroupTypeDefinition;

    typedef TypeDefinitionNoOpt<DATATYPE_MAX_> InvalidTypeDefinition;

}

#endif // RCP_TYPEDEFINITION_NOOPT_H
