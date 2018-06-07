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

#ifndef DEFAULTTYPE_H
#define DEFAULTTYPE_H

#include <iostream>

#include "typedefinition.h"
#include "iparameter.h"
#include "stream_tools.h"

namespace rcp {

    template<
        typename T,
        datatype_t type_id
    >
    class TypeDefinition<T, type_id, td_default> : public IDefaultDefinition<T>
    {
    public:
        TypeDefinition(TypeDefinition<T, type_id, td_default>& v) :
            obj(v.obj)
        {}

        TypeDefinition(const TypeDefinition<T, type_id, td_default>& v) :
            obj(v.obj)
        {}

        TypeDefinition(IParameter& param) :
            obj(std::make_shared<Value>(param))
        {}

        TypeDefinition(const T& defaultValue, IParameter& param) :
            obj(std::make_shared<Value>(defaultValue, param))
        {}

        ~TypeDefinition()
        {}

        //------------------------------------
        // implement IDefaultDefinition<T>
        virtual datatype_t getDatatype() const { return obj->datatype; }

        virtual const T& getDefault() const { return obj->defaultValue; }
        virtual void setDefault(const T& defaultValue) {

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

            while (!is.eof()) {

                // read option prefix
                boolean_options_t opt = static_cast<boolean_options_t>(is.get());

                if (opt == TERMINATOR) {
                    break;
                }

                // check stream
                CHECK_STREAM_MSG("typedefinition - could not read from stream")


                switch (opt) {
                case OPTIONS_DEFAULT: {

                    T def = readFromStream(is, def);
                    CHECK_STREAM   

                    setDefault(def);
                    break;
                }
                }

            }
        } // parseOptions


        virtual T readValue(std::istream& is) {
            T value = readFromStream(is, value);
            return value;
        }

        virtual void dump() {
            std::cout << "--- type default ---\n";

            if (hasDefault()) {
                std::cout << "\tdefault: " << getDefault() << "\n";
            }
        }

    private:
        void setDirty() {
            obj->parameter.setDirty();
        }

        class Value {
        public:
            Value(IParameter& param) :
                datatype(type_id)
              , hasDefaultValue(false)
              , defaultValueChanged(false)
              , parameter(param)
            {
            }

            Value(const T& defaultValue, IParameter& param) :
                datatype(type_id)
              , defaultValue(defaultValue)
              , hasDefaultValue(true)
              , defaultValueChanged(true)
              , parameter(param)
            {}

            void write(Writer& out, bool all) {

                out.write(static_cast<char>(datatype));

                if (hasDefaultValue) {

                    if (all || defaultValueChanged) {
                        out.write(static_cast<char>(OPTIONS_DEFAULT));
                        out.write(defaultValue);

                        if (!all) {
                            defaultValueChanged = false;
                        }
                    }
                } else if (defaultValueChanged) {

                    out.write(static_cast<char>(OPTIONS_DEFAULT));

                    T v{};
                    out.write(v);

                    defaultValueChanged = false;
                }
            }

            // mandatory
            datatype_t datatype;

            // options - default
            T defaultValue{};
            bool hasDefaultValue;
            bool defaultValueChanged;

            IParameter& parameter;
        };
        std::shared_ptr<Value> obj;
        TypeDefinition(std::shared_ptr<Value> obj) :obj(obj) {}
    };


    //
    typedef TypeDefinition<bool, DATATYPE_BOOLEAN, td_default> BooleanTypeDefinition;
    typedef TypeDefinition<Color, DATATYPE_RGBA, td_default > RGBATypeDefinition;
    typedef TypeDefinition<Color, DATATYPE_RGB, td_default > RGBTypeDefinition;
    typedef TypeDefinition<IPv4, DATATYPE_IPV4, td_default> IPv4TypeDefinition;
    typedef TypeDefinition<IPv6, DATATYPE_IPV6, td_default> IPv6TypeDefinition;
}

#endif // DEFAULTTYPE_H
