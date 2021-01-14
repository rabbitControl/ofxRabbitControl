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

#ifndef TYPE_STRING_H
#define TYPE_STRING_H

#include <iostream>

#include "typedefinition.h"
#include "iparameter.h"
#include "stream_tools.h"

namespace rcp {

    typedef TypeDefinition<std::string, DATATYPE_STRING, td_string > StringTypeDefinition;

    template<>
    class TypeDefinition<std::string, DATATYPE_STRING, td_string> : public IDefaultDefinition<std::string>
    {
    public:
        TypeDefinition(StringTypeDefinition& v)  :
            obj(v.obj)
        {}

        TypeDefinition(const StringTypeDefinition& v) :
            obj(v.obj)
        {}

        TypeDefinition(IParameter& param) :
            obj(std::make_shared<Value>(param))
        {}

        TypeDefinition(const std::string& d, IParameter& param) :
            obj(std::make_shared<Value>(d, param))
        {}

        //------------------------------------
        // implement writeable
        void write(Writer& out, bool all) {

            obj->write(out, all);

            // terminator
            out.write(static_cast<char>(TERMINATOR));
        }

        virtual void writeMandatory(Writer& out) const {
            obj->writeMandatory(out);
        }

        //------------------------------------
        // implement optionparser
        void parseOptions(std::istream& is) {

            while (!is.eof()) {

                // read option prefix
                string_options_t opt = static_cast<string_options_t>(is.get());

                if (is.eof()) {
                    break;
                }

                if (opt == TERMINATOR) {
                    break;
                }

                switch (opt) {
                case STRING_OPTIONS_DEFAULT: {

                    std::string d = readLongString(is);
                    CHECK_STREAM

                    obj->hasDefaultValue = true;
                    obj->defaultValue = d;
                    break;
                }
                case STRING_OPTIONS_REGULAR_EXPRESSION: {

                    std::string d = readLongString(is);
                    CHECK_STREAM

                    obj->hasRegex = true;
                    obj->regex = d;
                    break;
                }
                }
            }
        }

        virtual bool anyOptionChanged() const {
            return obj->defaultValueChanged
                    || obj->regexChanged;
        }

        virtual std::string readValue(std::istream& is) {
            return readLongString(is);
        }

        //------------------------------------
        // implement ITypeDefinition
        virtual datatype_t getDatatype() const { return obj->datatype; }

        virtual const std::string& getDefault() const { return obj->defaultValue; }
        virtual void setDefault(const std::string& defaultValue) {

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

        // reges
        virtual const std::string& getRegex() const { return obj->regex; }
        virtual void setRegex(const std::string& value) {

            obj->hasRegex = true;

            if (obj->regex == value) {
                return;
            }

            obj->regex = value;
            obj->regexChanged = true;
            setDirty();
        }
        virtual bool hasRegex() const { return obj->hasRegex; }
        virtual void clearRegex() {
            obj->hasRegex= false;
            obj->regexChanged = true;
            setDirty();
        }


        virtual void dump() {
            std::cout << "--- type string ---\n";

            if (hasDefault()) {
                std::cout << "\tdefault: " << getDefault() << "\n";
            }

            if (hasRegex()) {
                std::cout << "\tdefault: " << getRegex() << "\n";
            }
        }

    private:
        void setDirty() {
            obj->parameter.setDirty();
        }

        class Value {
        public:
            Value(IParameter& param) : datatype(DATATYPE_STRING)
              , defaultValue("")
              , hasDefaultValue(false)
              , defaultValueChanged(false)
              , hasRegex(false)
              , regexChanged(false)
              , parameter(param)
            {}

            Value(const std::string& defaultValue, IParameter& param) : datatype(DATATYPE_STRING)
              , defaultValue(defaultValue)
              , hasDefaultValue(true)
              , defaultValueChanged(true)
              , hasRegex(false)
              , regexChanged(false)
              , parameter(param)
            {}

            void writeMandatory(Writer& out) {
                out.write(static_cast<char>(datatype));
            }

            void write(Writer& out, bool all) {

                writeMandatory(out);

                // write default value
                if (hasDefaultValue) {

                    if (all || defaultValueChanged) {
                        out.write(static_cast<char>(STRING_OPTIONS_DEFAULT));
                        out.writeString(defaultValue);

                        if (!all) {
                            defaultValueChanged = false;
                        }
                    }
                } else if (defaultValueChanged) {

                    out.write(static_cast<char>(STRING_OPTIONS_DEFAULT));
                    out.writeString("");
                    defaultValueChanged = false;
                }

                // regex
                if (hasRegex) {

                    if (all || regexChanged) {
                        out.write(static_cast<char>(STRING_OPTIONS_REGULAR_EXPRESSION));
                        out.writeString(regex);

                        if (!all) {
                            regexChanged = false;
                        }
                    }

                } else if (regexChanged) {

                    out.write(static_cast<char>(STRING_OPTIONS_REGULAR_EXPRESSION));
                    out.writeString("");
                    regexChanged = false;
                }

            }

            // mandatory
            datatype_t datatype;

            // options - base
            std::string defaultValue{""};
            bool hasDefaultValue;
            bool defaultValueChanged;

            // options - regex
            std::string regex{""};
            bool hasRegex;
            bool regexChanged;

            IParameter& parameter;
        };
        std::shared_ptr<Value> obj;
        TypeDefinition(std::shared_ptr<Value> obj) :obj(obj) {}
    };


}
#endif // TYPE_STRING_H
