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

#ifndef TYPE_RANGE_H
#define TYPE_RANGE_H

#include <iostream>

#include "typedefinition.h"
#include "iparameter.h"
#include "stream_tools.h"

#include "range.h"

namespace rcp {


    template<typename ElementType>
    class TypeDefinition<Range<ElementType>, DATATYPE_RANGE, td_num> : public IDefaultDefinition<Range<ElementType>>
    {
    public:
        TypeDefinition(TypeDefinition<Range<ElementType>, DATATYPE_RANGE, td_default>& v) :
            obj(v.obj)
        {}

        TypeDefinition(const TypeDefinition<Range<ElementType>, DATATYPE_RANGE, td_default>& v) :
            obj(v.obj)
        {}

        TypeDefinition(IParameter& param) :
            obj(std::make_shared<Value>(param))
        {}

        TypeDefinition(const std::string& d, IParameter& param) :
            obj(std::make_shared<Value>(d, param))
        {}

        ~TypeDefinition()
        {}

        //------------------------------------
        // implement writeable
        void write(Writer& out, bool all) {

            obj->write(out, all);

            // terminator
            out.write(static_cast<char>(TERMINATOR));
        }


        //------------------------------------
        // implement optionparser
        void parseOptions(std::istream& is) {

            // parse element type options first
            obj->element_type.parseOptions(is);


            while (!is.eof()) {

                // get option prefix
                range_options_t opt = static_cast<range_options_t>(is.get());

                if (opt == TERMINATOR) {
                    break;
                }

                // check stream
                CHECK_STREAM_MSG("range - could not read from stream")


                switch (opt) {
                case RANGE_OPTIONS_DEFAULT:

                    // read 2 values of elementtype
                    ElementType v1 = readFromStream(is, v1);
                    CHECK_STREAM
                    ElementType v2 = readFromStream(is, v2);
                    CHECK_STREAM

                    setDefault(Range<ElementType>(v1, v2));
                    break;
                }

            }
        } // parseOptions


        virtual Range<ElementType> readValue(std::istream& is) {
            ElementType v1 = readFromStream(is, v1);
            ElementType v2 = readFromStream(is, v2);
            return Range<ElementType>(v1, v2);
        }

        //------------------------------------
        // implement IDefaultDefinition<T>
        virtual datatype_t getDatatype() const { return obj->datatype; }

        TypeDefinition<ElementType, convertDatatype<ElementType>::value, td_num>& getElementType() {
            return obj->element_type;
        }

        void setMinimum(const ElementType& v) {
            obj->element_type.setMinimum(v);
        }
        void setMaximum(const ElementType& v) {
            obj->element_type.setMaximum(v);
        }
        void setMultipleof(const ElementType& v) {
            obj->element_type.setMultipleof(v);
        }
        void setScale(const number_scale_t& v) {
            obj->element_type.setScale(v);
        }
        void setUnit(const std::string& v) {
            obj->element_type.setUnit(v);
        }

        virtual const Range<ElementType>& getDefault() const { return obj->defaultValue; }
        virtual void setDefault(const Range<ElementType>& defaultValue) {

            obj->hasDefaultValue = true;

            if (obj->defaultValue == defaultValue) {
                return;
            }

            obj->defaultValue = defaultValue;
            obj->defaultValueChanged = true;

            setDirty();
        }
        virtual bool hasDefault() const { return obj->hasDefaultValue; }
        virtual void clearDefault() {
            obj->hasDefaultValue = false;
            obj->defaultValueChanged = true;

            setDirty();
        }

        virtual void dump() {
            std::cout << "--- type range ---\n";

            if (hasDefault()) {
                std::cout << "\tdefault: " << getDefault().value1() << ":" << getDefault().value2() << "\n";
            }

            obj->element_type.dump();
        }


    private:
        void setDirty() {
            obj->parameter.setDirty();
        }

        class Value {
        public:
            Value(IParameter& param) :
                datatype(DATATYPE_RANGE)
              , element_type(TypeDefinition<ElementType, convertDatatype<ElementType>::value, td_num>(param))
              , hasDefaultValue(false)
              , defaultValueChanged(false)
              , parameter(param)
            {}

            Value(const Range<ElementType>& defaultValue, IParameter& param) :
                datatype(DATATYPE_URI)
              , element_type(TypeDefinition<ElementType, convertDatatype<ElementType>::value, td_num>(param))
              , defaultValue(defaultValue)
              , hasDefaultValue(true)
              , defaultValueChanged(true)
              , parameter(param)
            {}

            void write(Writer& out, bool all) {

                out.write(static_cast<char>(datatype));
                element_type.write(out, all);

                // write default value
                if (hasDefaultValue) {

                    if (all || defaultValueChanged) {
                        out.write(static_cast<char>(RANGE_OPTIONS_DEFAULT));

                        out.write(defaultValue.value1());
                        out.write(defaultValue.value2());

                        if (!all) {
                            defaultValueChanged = false;
                        }
                    }
                } else if (defaultValueChanged) {

                    out.write(static_cast<char>(RANGE_OPTIONS_DEFAULT));

                    ElementType v{};
                    if (element_type.hasDefault()) {
                         v = element_type.getDefault();
                    }
                    out.write(v);
                    out.write(v);

                    defaultValueChanged = false;
                }
            }

            // mandatory
            datatype_t datatype;
            TypeDefinition<ElementType, convertDatatype<ElementType>::value, td_num> element_type;


            // options - default
            Range<ElementType> defaultValue{};
            bool hasDefaultValue;
            bool defaultValueChanged;

            IParameter& parameter;
        };
        std::shared_ptr<Value> obj;
        TypeDefinition(std::shared_ptr<Value> obj) :obj(obj) {}
    };

}
#endif // TYPE_RANGE_H
