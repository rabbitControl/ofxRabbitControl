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

#ifndef NUMBERTYPE_H
#define NUMBERTYPE_H

#include <iostream>
#include <limits>

#include "typedefinition.h"
#include "iparameter.h"
#include "stream_tools.h"

namespace rcp {

    template<
        typename T,
        datatype_t type_id
    >
    class TypeDefinition<T, type_id, td_num > : public INumberDefinition<T>
    {
    public:
        TypeDefinition(TypeDefinition<T, type_id, td_num >& v) :
            obj(v.obj)
        {}

        TypeDefinition(const TypeDefinition<T, type_id, td_num >& v) :
            obj(v.obj)
        {}

        TypeDefinition(IParameter& param) :
            obj(std::make_shared<Value>(param))
        {}

        TypeDefinition(const T& dv, IParameter& param) :
            obj(std::make_shared<Value>(dv, param))
        {}

        TypeDefinition(const T& dv, const T& min, const T& max, IParameter& param) :
            obj(std::make_shared<Value>(dv, min, max, param))
        {}

        //------------------------------------
        // implement writeable
        virtual void write(Writer& out, bool all) {

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
                number_options_t opt = static_cast<number_options_t>(is.get());

                CHECK_STREAM_MSG("typedefinition - could not read from stream")

                if (opt == TERMINATOR) {   
                    break;
                }

                switch (opt) {
                case NUMBER_OPTIONS_DEFAULT: {

                    // read options
                    T def = readFromStream(is, def);
                    CHECK_STREAM

                    obj->hasDefaultValue = true;
                    obj->defaultValue = def;
                    break;
                }
                case NUMBER_OPTIONS_MINIMUM: {

                    T min = readFromStream(is, min);
                    CHECK_STREAM

                    obj->hasMinimum = true;
                    obj->minimum = min;
                    break;
                }
                case NUMBER_OPTIONS_MAXIMUM: {

                    T max = readFromStream(is, max);
                    CHECK_STREAM

                    obj->hasMaximum = true;
                    obj->maximum = max;
                    break;
                }
                case NUMBER_OPTIONS_MULTIPLEOF: {

                    T mult = readFromStream(is, mult);
                    CHECK_STREAM

                    obj->hasMultipleof = true;
                    obj->multipleof = mult;
                    break;
                }
                case NUMBER_OPTIONS_SCALE: {

                    number_scale_t scale = static_cast<number_scale_t>(is.get());
                    CHECK_STREAM

                    obj->hasScale = true;
                    obj->scale = scale;
                    break;
                }
                case NUMBER_OPTIONS_UNIT: {

                    std::string unit = readTinyString(is);
                    CHECK_STREAM

                    obj->hasUnit = true;
                    obj->unit = unit;
                    break;
                }
                }

            }
        } // parseOptions


        virtual T readValue(std::istream& is) {
            T val = readFromStream(is, val);
            return val;
        }

        //------------------------------------
        // implement IDefaultDefinition
        virtual datatype_t getDatatype() const { return obj->datatype; }

        virtual const T& getDefault() const {
            return obj->defaultValue;
        }
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
        // implement INumberDefinition
        virtual T getMinimum() const {
            if (obj->hasMinimum)
                return obj->minimum;
            return 0;
        }
        virtual void setMinimum(const T& val) {

            obj->hasMinimum = true;

            if (obj->minimum == val) {
                return;
            }

            obj->minimum = val;
            obj->minimumChanged = true;
            setDirty();
        }
        virtual bool hasMinimum() const { return obj->hasMinimum; }
        virtual void clearMinimum() {
            obj->hasMinimum = false;
            obj->minimumChanged = true;
            setDirty();
        }

        virtual T getMaximum() const {
            if (obj->hasMaximum)
                return obj->maximum;
            return 0;
        }
        virtual void setMaximum(const T& val) {

            obj->hasMaximum = true;

            if (obj->maximum == val) {
                return;
            }

            obj->maximum = val;
            obj->maximumChanged = true;
            setDirty();
        }
        virtual bool hasMaximum() const { return obj->hasMaximum; }
        virtual void clearMaximum() {
            obj->hasMaximum = false;
            obj->maximumChanged = true;
            setDirty();
        }

        virtual T getMultipleof() const {
            if (obj->hasMultipleof)
                return obj->multipleof;
            return 0;
        }
        virtual void setMultipleof(const T& val) {

            obj->hasMultipleof = true;

            if (obj->multipleof == val) {
                return;
            }

            obj->multipleof = val;
            obj->multipleofChanged = true;
            setDirty();
        }
        virtual bool hasMultipleof() const { return obj->hasMultipleof; }
        virtual void clearMultipleof() {
            obj->hasMultipleof = false;
            obj->multipleofChanged = true;
            setDirty();
        }

        virtual number_scale_t getScale() const {
            if (obj->hasScale)
                return obj->scale;
            return NUMBER_SCALE_LINEAR;
        }
        virtual void setScale(const number_scale_t& val) {

            obj->hasScale = true;

            if (obj->scale == val) {
                return;
            }

            obj->scale = val;
            obj->scaleChanged = true;
            setDirty();
        }
        virtual bool hasScale() const { return obj->hasScale; }
        virtual void clearScale() {
            obj->hasScale = false;
            obj->scaleChanged = true;
            setDirty();
        }

        virtual std::string getUnit() const { return obj->unit; }
        virtual void setUnit(const std::string& val) {

            obj->hasUnit = true;

            if (obj->unit == val) {
                return;
            }

            obj->unit = val;
            obj->unitChanged = true;
            setDirty();
        }
        virtual bool hasUnit() const { return obj->hasUnit; }
        virtual void clearUnit() {
            obj->hasUnit = false;
            obj->unitChanged = true;

            setDirty();
        }


        virtual bool anyOptionChanged() const {
            return obj->defaultValueChanged
                    || obj->minimumChanged
                    || obj->maximumChanged
                    || obj->multipleofChanged
                    || obj->scaleChanged
                    || obj->unitChanged;
        }

        virtual void dump() {
            std::cout << "--- type number ---\n";

            if (hasDefault()) {
                std::cout << "\tdefault: " << (int)getDefault() << "\n";
            }

            if (hasMinimum()) {
                std::cout << "\tminimum: " << (int)getMinimum() << "\n";
            }

            if (hasMaximum()) {
                std::cout << "\tmaximum: " << (int)getMaximum() << "\n";
            }

            if (hasMultipleof()) {
                std::cout << "\tmultipleof: " << (int)getMultipleof() << "\n";
            }

            if (hasScale()) {
                std::cout << "\tscale: " << getScale() << "\n";
            }

            if (hasUnit()) {
                std::cout << "\tunit: " << getUnit() << "\n";
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
              , minimum(std::numeric_limits<T>::min())
              , hasMinimum(false)
              , minimumChanged(false)
              , maximum(std::numeric_limits<T>::max())
              , hasMaximum(false)
              , maximumChanged(false)
              , hasMultipleof(false)
              , multipleofChanged(false)
              , hasScale(false)
              , scaleChanged(false)
              , hasUnit(false)
              , unitChanged(false)
              , parameter(param)
            {}

            Value(const T& defaultValue, IParameter& param) :
                datatype(type_id)
              , defaultValue(defaultValue)
              , hasDefaultValue(true)
              , defaultValueChanged(true)
              , minimum(std::numeric_limits<T>::min())
              , hasMinimum(false)
              , minimumChanged(false)
              , maximum(std::numeric_limits<T>::max())
              , hasMaximum(false)
              , maximumChanged(false)
              , hasMultipleof(false)
              , multipleofChanged(false)
              , hasScale(false)
              , scaleChanged(false)
              , hasUnit(false)
              , unitChanged(false)
              , parameter(param)
            {}

            Value(const T& defaultValue, const T& min, const T& max, IParameter& param) :
                datatype(type_id)
              , defaultValue(defaultValue)
              , hasDefaultValue(true)
              , defaultValueChanged(true)
              , minimum(min)
              , hasMinimum(true)
              , minimumChanged(true)
              , maximum(max)
              , hasMaximum(true)
              , maximumChanged(true)
              , hasMultipleof(false)
              , multipleofChanged(false)
              , hasScale(false)
              , scaleChanged(false)
              , hasUnit(false)
              , unitChanged(false)
              , parameter(param)
            {}

            void writeMandatory(Writer& out) {
                out.write(static_cast<char>(datatype));
            }

            virtual void write(Writer& out, bool all) {

                writeMandatory(out);

                // write default value
                if (hasDefaultValue) {

                    if (all || defaultValueChanged) {
                        out.write(static_cast<char>(NUMBER_OPTIONS_DEFAULT));
                        out.write(defaultValue);

                        if (!all) {
                            defaultValueChanged = false;
                        }
                    }
                } else if (defaultValueChanged) {
                    out.write(static_cast<char>(NUMBER_OPTIONS_DEFAULT));
                    out.write(static_cast<T>(0));
                    defaultValueChanged = false;
                }


                // minimum
                if (hasMinimum) {

                    if (all || minimumChanged) {
                        out.write(static_cast<char>(NUMBER_OPTIONS_MINIMUM));
                        out.write(minimum);

                        if (!all) {
                            minimumChanged = false;
                        }
                    }
                } else if (minimumChanged) {

                    out.write(static_cast<char>(NUMBER_OPTIONS_MINIMUM));
                    out.write(std::numeric_limits<T>::min());
                    minimumChanged = false;
                }


                // maximum
                if (hasMaximum) {

                    if (all || maximumChanged) {
                        out.write(static_cast<char>(NUMBER_OPTIONS_MAXIMUM));
                        out.write(maximum);

                        if (!all) {
                            maximumChanged = false;
                        }
                    }
                } else if (maximumChanged) {

                    out.write(static_cast<char>(NUMBER_OPTIONS_MAXIMUM));
                    out.write(std::numeric_limits<T>::max());
                    maximumChanged = false;
                }


                // multipleof
                if (hasMultipleof) {

                    if (all || multipleofChanged) {
                        out.write(static_cast<char>(NUMBER_OPTIONS_MULTIPLEOF));
                        out.write(multipleof);

                        if (!all) {
                            multipleofChanged = false;
                        }
                    }
                } else if (multipleofChanged) {

                    out.write(static_cast<char>(NUMBER_OPTIONS_MULTIPLEOF));
                    out.write(static_cast<T>(0));
                    multipleofChanged = false;
                }


                // scale
                if (hasScale) {

                    if (all || scaleChanged) {
                        out.write(static_cast<char>(NUMBER_OPTIONS_SCALE));
                        out.write(static_cast<char>(scale));

                        if (!all) {
                            scaleChanged = false;
                        }
                    }
                } else if (scaleChanged) {

                    out.write(static_cast<char>(NUMBER_OPTIONS_SCALE));
                    out.write(static_cast<char>(NUMBER_SCALE_LINEAR));
                    scaleChanged = false;
                }


                // unit
                if (hasUnit) {

                    if (all || unitChanged) {
                        out.write(static_cast<char>(NUMBER_OPTIONS_UNIT));
                        out.writeTinyString(unit);

                        if (!all) {
                            unitChanged = false;
                        }
                    }
                } else if (unitChanged) {

                    out.write(static_cast<char>(NUMBER_OPTIONS_UNIT));
                    out.writeTinyString("");
                    unitChanged = false;
                }
            }

            // mandatory
            datatype_t datatype;

            // options - base
            T defaultValue{0};
            bool hasDefaultValue;
            bool defaultValueChanged;

            // options - number
            T minimum{0};
            bool hasMinimum;
            bool minimumChanged;

            T maximum{0};
            bool hasMaximum;
            bool maximumChanged;

            T multipleof{0};
            bool hasMultipleof;
            bool multipleofChanged;

            number_scale_t scale{NUMBER_SCALE_LINEAR};
            bool hasScale;
            bool scaleChanged;

            std::string unit{""};
            bool hasUnit;
            bool unitChanged;

            IParameter& parameter;
        };
        std::shared_ptr<Value> obj;
        TypeDefinition(std::shared_ptr<Value> obj) :obj(obj) {}
    };

    //
    typedef TypeDefinition<int8_t, DATATYPE_INT8, td_num > Int8TypeDefinition;
    typedef TypeDefinition<uint8_t, DATATYPE_UINT8, td_num > UInt8TypeDefinition;
    typedef TypeDefinition<int16_t, DATATYPE_INT16, td_num > Int16TypeDefinition;
    typedef TypeDefinition<uint16_t, DATATYPE_UINT16, td_num > UInt16TypeDefinition;
    typedef TypeDefinition<int32_t, DATATYPE_INT32, td_num > Int32TypeDefinition;
    typedef TypeDefinition<uint32_t, DATATYPE_UINT32, td_num > UInt32TypeDefinition;
    typedef TypeDefinition<int64_t, DATATYPE_INT64, td_num > Int64TypeDefinition;
    typedef TypeDefinition<uint64_t, DATATYPE_UINT64, td_num > UInt64TypeDefinition;
    typedef TypeDefinition<float, DATATYPE_FLOAT32, td_num > Float32TypeDefinition;
    typedef TypeDefinition<double, DATATYPE_FLOAT64, td_num > Float64TypeDefinition;
}

#endif // NUMBERTYPE_H
