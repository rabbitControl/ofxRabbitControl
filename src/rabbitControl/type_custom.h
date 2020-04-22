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

#ifndef TYPE_CUSTOM_H
#define TYPE_CUSTOM_H

#include <iostream>
#include <vector>
#include <string.h>

#include "typedefinition.h"
#include "iparameter.h"
#include "stream_tools.h"

namespace rcp {

    template<typename T>
    class TypeDefinition<T, DATATYPE_CUSTOMTYPE, td_custom> : public IDefaultDefinition<T>
    {
    public:
        TypeDefinition(TypeDefinition<std::string, DATATYPE_CUSTOMTYPE, td_custom>& v) :
            obj(v.obj)
        {}

        TypeDefinition(const TypeDefinition<std::string, DATATYPE_CUSTOMTYPE, td_custom>& v) :
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

            while (!is.eof()) {

                // read option prefix
                customtype_options_t opt = static_cast<customtype_options_t>(is.get());

                if (is.eof()) {
                    break;
                }

                if (opt == TERMINATOR) {
                    break;
                }

                switch (opt) {
                case CUSTOMTYPE_OPTIONS_DEFAULT: {

                    T def = readFromStream(is, def);
                    CHECK_STREAM

                    setDefault(def);
                    break;
                }
                case CUSTOMTYPE_OPTIONS_UUID: {

                    char uuid[16];
                    is.get(uuid, 16);
                    CHECK_STREAM

                    setUuid(uuid, 16);
                    break;
                }
                case CUSTOMTYPE_OPTIONS_CONFIG: {

                    uint32_t data_length = readFromStream(is, data_length);
                    CHECK_STREAM

                    char data[data_length];
                    is.get(data, static_cast<std::streamsize>(data_length));
                    CHECK_STREAM

                    //set
                    obj->config.clear();
                    obj->config.resize(data_length);
                    memcpy(&obj->config[0], &data[0], data_length);
                    break;
                }
                }
            }
        }


        virtual T readValue(std::istream& is) {
            T value = readFromStream(is, value);
            return value;
        }

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

        // uuid
        bool hasUuid() const { return obj->hasUuid; }
        void setUuid(const char* uuid, uint8_t length) {
            if (length != 16) {
                return;
            }

            obj->hasUuid = true;
            if (memcmp(obj->uuid, uuid, 16) == 0) {
                return;
            }

            memcpy(obj->uuid, uuid, 16);
            obj->uuidChanged = true;
            setDirty();
        }
        const char* getUuid() const {
            return obj->uuid;
        }
        void clearUuid() {
            memset(obj->uuid, 0, 16);
            obj->hasUuid = false;
            obj->uuidChanged = true;
            setDirty();
        }


        // config
        bool hasConfig() const { return obj->config.size() > 0; }
        std::vector<int8_t>& getConfig() const {
            return obj->config;
        }
        void setConfig(const std::vector<int8_t>& config) {
            obj->config = config;
            obj->configChanged = true;
            setDirty();
        }
        void clearConfig() {
            obj->config.clear();
            obj->configChanged = true;
            setDirty();
        }


        virtual void dump() {
            std::cout << "--- type custom ---\n";

            if (hasDefault()) {
                std::cout << "\tdefault: " << getDefault() << "\n";
            }

            if (hasUuid()) {
                std::cout << "\tuuid: " << getUuid() << "\n";
            }

            if (hasConfig()) {
                std::cout << "\tconfig bytes: " << getConfig().size() << "\n";
            }
        }

    private:
        void setDirty() {
            obj->parameter.setDirty();
        }

        class Value {
        public:
            Value(IParameter& param) : datatype(DATATYPE_CUSTOMTYPE)
              , hasDefaultValue(false)
              , defaultValueChanged(false)
              , hasUuid(false)
              , uuidChanged(false)
              , configChanged(false)
              , parameter(param)
            {
                memset(uuid, 0, 16);
            }

            Value(const std::string& defaultValue, IParameter& param) : datatype(DATATYPE_CUSTOMTYPE)
              , defaultValue(defaultValue)
              , hasDefaultValue(true)
              , defaultValueChanged(true)
              , hasUuid(false)
              , uuidChanged(false)
              , configChanged(false)
              , parameter(param)
            {
                memset(uuid, 0, 16);
            }

            void write(Writer& out, bool all) {

                out.write(static_cast<char>(datatype));
                out.write(static_cast<int32_t>(sizeof(T)));

                // write default value
                if (hasDefaultValue) {

                    if (all || defaultValueChanged) {
                        out.write(static_cast<char>(CUSTOMTYPE_OPTIONS_DEFAULT));
                        out.write((char*)(&defaultValue), sizeof(T));

                        if (!all) {
                            defaultValueChanged = false;
                        }
                    }
                } else if (defaultValueChanged) {

                    out.write(static_cast<char>(CUSTOMTYPE_OPTIONS_DEFAULT));

                    T v;
                    memset(&v, 0, sizeof(T));
                    out.write((char*)(&v), sizeof(T));

                    defaultValueChanged = false;
                }

                // uuid
                if (hasUuid) {

                    if (all || uuidChanged) {
                        out.write(static_cast<char>(CUSTOMTYPE_OPTIONS_UUID));

                        // TODO
                        out.write(uuid, 16);

                        if (!all) {
                            uuidChanged = false;
                        }
                    }

                } else if (uuidChanged) {

                    out.write(static_cast<char>(CUSTOMTYPE_OPTIONS_UUID));

                    // if uuid was cleared we write 0-bytes
                    out.write(uuid, 16);
                    uuidChanged = false;
                }


                // config
                if (config.size() > 0) {

                    if (all || configChanged) {
                        out.write(static_cast<char>(CUSTOMTYPE_OPTIONS_CONFIG));
                        out.write(static_cast<uint32_t>(config.size()));

                        // write
                        out.write((char*)config.data(), config.size());

                        if (!all) {
                            configChanged = false;
                        }
                    }

                } else if (configChanged) {

                    out.write(static_cast<char>(CUSTOMTYPE_OPTIONS_CONFIG));
                    out.write(static_cast<uint32_t>(0));
                    configChanged = false;
                }
            }

            // mandatory
            datatype_t datatype;

            // options - base
            T defaultValue{};
            bool hasDefaultValue;
            bool defaultValueChanged;

            char uuid[16];
            bool hasUuid;
            bool uuidChanged;

            std::vector<int8_t> config;
            bool configChanged;

            IParameter& parameter;
        };
        std::shared_ptr<Value> obj;
        TypeDefinition(std::shared_ptr<Value> obj) :obj(obj) {}

    };

}

#endif // TYPE_CUSTOM_H
