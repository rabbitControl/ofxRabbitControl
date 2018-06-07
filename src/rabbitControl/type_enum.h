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

#ifndef ENUMTYPE_H
#define ENUMTYPE_H

#include <iostream>
#include <vector>

#include "typedefinition.h"
#include "iparameter.h"
#include "stream_tools.h"

namespace rcp {

    typedef TypeDefinition<std::string, DATATYPE_ENUM, td_enum > EnumTypeDefinition;

    template<>
    class TypeDefinition<std::string, DATATYPE_ENUM, td_enum> : public IDefaultDefinition<std::string>
    {
    public:
        TypeDefinition(EnumTypeDefinition& v)  :
            obj(v.obj)
        {}
        TypeDefinition(const EnumTypeDefinition& v)  :
            obj(v.obj)
        {}

        TypeDefinition(IParameter& param) :
            obj(std::make_shared<Value>(param))
        {}
        TypeDefinition(const std::string& d, IParameter& param) :
            obj(std::make_shared<Value>(d, param))
        {}

        ~TypeDefinition();

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

        // options
        std::vector<std::string>& getOptions() const { return obj->options; }
        const std::string& getOption(const std::string& selection) const {

            auto it = std::find(obj->options.begin(), obj->options.end(), selection);
            if (it != obj->options.end()) {
                return *it;
            }

            return empty_string;
        }
        void addOption(const std::string& option) {
            obj->options.push_back(option);
            obj->optionsChanged = true;
            setDirty();
        }
        void setOptions(const std::vector<std::string>& options) {
            obj->options = options;
            obj->optionsChanged = true;
            setDirty();
        }
        bool hasOptions() const { return obj->options.size() > 0; }
        void clearOptions() {
            obj->options.clear();
            obj->optionsChanged = true;
            setDirty();
        }


        // multiselect
        bool getMultiselect() const { return obj->multiselect; }
        bool hasMultiselect() const { return obj->hasMultiselect; }
        void setMultiselect(bool value) {

            obj->hasMultiselect = true;

            if (obj->multiselect == value) {
                return;
            }

            obj->multiselect = value;
            obj->multiselectChanged = true;
            setDirty();
        }
        void clearMultiselect() {
            obj->hasMultiselect = false;
            obj->multiselectChanged = true;
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
                enum_options_t opt = static_cast<enum_options_t>(is.get());

                if (is.eof()) {
                    break;
                }

                if (opt == TERMINATOR) {
                    break;
                }

                switch (opt) {
                case ENUM_OPTIONS_DEFAULT: {

                    std::string d = readFromStream(is, d);
                    CHECK_STREAM

                    setDefault(d);
                    break;
                }
                case ENUM_OPTIONS_ENTRIES: {

                    obj->options.clear();
                    std::string option = readTinyString(is);
                    CHECK_STREAM
                    while (option != "") {

                        addOption(option);

                        option = readTinyString(is);
                        CHECK_STREAM
                    }
                    break;
                }
                case ENUM_OPTIONS_MULTISELECT: {

                    bool multi = readFromStream(is, multi);
                    CHECK_STREAM

                    setMultiselect(multi);   
                    break;
                }
                }
            }
        }

        virtual std::string readValue(std::istream& is) {
            return readTinyString(is);
        }

        virtual void dump() {
            std::cout << "--- type enum ---\n";

            if (hasDefault()) {
                std::cout << "\tdefault: " << getDefault() << "\n";
            }

            if (hasMultiselect()) {
                std::cout << "\tmultiselect: " << getMultiselect() << "\n";
            }

            if (hasOptions()) {
                for (auto& o : obj->options) {
                    std::cout << "\toption: " << o << "\n";
                }
            }
        }

    private:
        void setDirty() {
            obj->parameter.setDirty();
        }

        class Value {
        public:
            Value(IParameter& param) : datatype(DATATYPE_ENUM)
              , defaultValue(empty_string)
              , hasDefaultValue(false)
              , defaultValueChanged(false)
              , optionsChanged(false)
              , hasMultiselect(false)
              , multiselectChanged(false)
              , parameter(param)
            {}

            Value(const std::string& defaultValue, IParameter& param) : datatype(DATATYPE_ENUM)
              , defaultValue(defaultValue)
              , hasDefaultValue(true)
              , defaultValueChanged(true)
              , optionsChanged(false)
              , hasMultiselect(false)
              , multiselectChanged(false)
              , parameter(param)
            {}

            void write(Writer& out, bool all) {

                out.write(static_cast<char>(datatype));

                // write default value
                if (hasDefaultValue) {

                    if (all || defaultValueChanged) {
                        out.write(static_cast<char>(ENUM_OPTIONS_DEFAULT));
                        out.writeTinyString(defaultValue);

                        if (!all) {
                            defaultValueChanged = false;
                        }
                    }
                } else if (defaultValueChanged) {

                    out.write(static_cast<char>(ENUM_OPTIONS_DEFAULT));
                    out.writeTinyString("");
                    defaultValueChanged = false;
                }

                // options
                if (options.size() > 0) {

                    if (all || optionsChanged) {
                        out.write(static_cast<char>(ENUM_OPTIONS_ENTRIES));
                        out.write(static_cast<uint16_t>(options.size()));
                        for (const std::string& entry : options) {
                            out.writeTinyString(entry);
                        }

                        if (!all) {
                            optionsChanged = false;
                        }
                    }

                } else if (optionsChanged) {

                    out.write(static_cast<char>(ENUM_OPTIONS_ENTRIES));
                    out.write(static_cast<uint16_t>(0));
                    optionsChanged = false;
                }


                // multiselect
                if (hasMultiselect) {

                    if (all || multiselectChanged) {
                        out.write(static_cast<char>(ENUM_OPTIONS_MULTISELECT));
                        out.write(multiselect);

                        if (!all) {
                            multiselectChanged = false;
                        }
                    }

                } else if (multiselectChanged) {

                    out.write(static_cast<char>(ENUM_OPTIONS_MULTISELECT));
                    out.write(false);
                    multiselectChanged = false;
                }
            }

            // mandatory
            datatype_t datatype;

            // options - base
            std::string defaultValue{};
            bool hasDefaultValue;
            bool defaultValueChanged;

            // options - enum
            std::vector<std::string> options;
            bool optionsChanged;

            bool multiselect{};
            bool hasMultiselect;
            bool multiselectChanged;

            IParameter& parameter;
        };
        std::shared_ptr<Value> obj;
        TypeDefinition(std::shared_ptr<Value> obj) :obj(obj) {}
    };

}
#endif // ENUMTYPE_H
