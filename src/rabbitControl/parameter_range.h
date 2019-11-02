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

#ifndef RCP_PARAMETER_RANGE_H
#define RCP_PARAMETER_RANGE_H

#include <vector>

#include "parameter_intern.h"
#include "type_range.h"

namespace rcp {

    template <typename ElementType>
    class RangeParameter :
            public ValueParameter<Range<ElementType>, TypeDefinition<Range<ElementType>, DATATYPE_RANGE, td_num>, DATATYPE_RANGE>
            , public IElementParameter
    {
    public:

        typedef TypeDefinition<Range<ElementType>, DATATYPE_RANGE, td_num> _RangeType;
        typedef ValueParameter<Range<ElementType>, TypeDefinition<Range<ElementType>, DATATYPE_RANGE, td_num>, DATATYPE_RANGE> _RangeParameter;

        static std::shared_ptr< _RangeParameter > create(int16_t id) {
            return std::make_shared<RangeParameter<ElementType> >(id);
        }

        RangeParameter(int16_t id) :
            _RangeParameter(id)
        {}

        ~RangeParameter()
        {}

        // IElementParameter
        virtual datatype_t getElementType() {
            return _RangeParameter::getDefaultTypeDefinition().getElementType().getDatatype();
        }

        // convenience
        void setDefault(const Range<ElementType>& v) {
            _RangeParameter::getDefaultTypeDefinition().setDefault(v);
        }
        void setElementDefault(const ElementType& v) {
            _RangeParameter::getDefaultTypeDefinition().getElementType().setDefault(v);
        }
        void setMinimum(const ElementType& v) {
            _RangeParameter::getDefaultTypeDefinition().setMinimum(v);
        }
        void setMaximum(const ElementType& v) {
            _RangeParameter::getDefaultTypeDefinition().setMaximum(v);
        }
        void setMultipleof(const ElementType& v) {
            _RangeParameter::getDefaultTypeDefinition().setMultipleof(v);
        }
        void setScale(const number_scale_t& v) {
            _RangeParameter::getDefaultTypeDefinition().setScale(v);
        }
        void setUnit(const std::string& v) {
            _RangeParameter::getDefaultTypeDefinition().setUnit(v);
        }

        virtual void dump() {
            Parameter<_RangeType>::dump();

            if (_RangeParameter::hasValue()) {
                Range<ElementType> r = _RangeParameter::getValue();
                std::cout << "value: " << r.value1() << ":" << r.value2() << "\n";
            }
        }
    };
}

#endif
