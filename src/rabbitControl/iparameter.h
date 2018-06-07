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

#ifndef IPARAMETER_H
#define IPARAMETER_H

#include "types.h"
#include "writeable.h"
#include "optionparser.h"
#include "typedefinition.h"

namespace rcp {

    class GroupParameter;
    class ITypeDefinition;
    class IParameter;
    class IParameterManager;

    typedef std::shared_ptr<IParameter> ParameterPtr;

    class IParameter : public Writeable, public IOptionparser {
    public:
        virtual ~IParameter();

        //--------------------------------
        // mandatory
        virtual int16_t getId() const = 0;
        virtual ITypeDefinition& getTypeDefinition() const = 0;
        virtual datatype_t getDatatype() const = 0;

        //--------------------------------
        // optional
        virtual const std::string getLabel() const = 0;
        virtual void setLabel(const std::string& label) = 0;
        virtual bool hasLabel() const = 0;
        virtual void clearLabel() = 0;

        virtual std::vector<std::string> getLabelLanguages() const = 0;
        virtual std::string getLanguageLabel(const std::string& code) const = 0;
        virtual void clearLanguageLabel() = 0;
        virtual void setLanguageLabel(const std::string& code, const std::string& label) = 0;
        virtual void removeLanguageLabel(const std::string& code) = 0;


        virtual const std::string& getDescription() const = 0;
        virtual void setDescription(const std::string& description) = 0;
        virtual bool hasDescription() const = 0;
        virtual void clearDescription() = 0;

        virtual std::vector<std::string> getDescriptionLanguages() const = 0;
        virtual std::string getDescriptionLanguage(const std::string& code) const = 0;
        virtual void clearDescriptionLanguage() = 0;
        virtual void setDescriptionLanguage(const std::string& code, const std::string& label) = 0;
        virtual void removeDescriptionLanguage(const std::string& code) = 0;


        virtual const std::string& getTags() const = 0;
        virtual void setTags(const std::string& tags) = 0;
        virtual bool hasTags() const = 0;
        virtual void clearTags() = 0;

        virtual const int32_t& getOrder() const = 0;
        virtual void setOrder(const int32_t& order) = 0;
        virtual bool hasOrder() const = 0;
        virtual void clearOrder() = 0;

        virtual std::weak_ptr<GroupParameter>& getParent() const = 0;
        virtual bool hasParent() const = 0;

        virtual const std::vector<char> getUserdata() const = 0;
        virtual void setUserdata(const std::vector<char> userdata) = 0;
        virtual bool hasUserdata() const = 0;
        virtual void clearUserdata() = 0;

        virtual const std::string& getUserid() const = 0;
        virtual void setUserid(const std::string& userid) = 0;
        virtual bool hasUserid() const = 0;
        virtual void clearUserid() = 0;

        //
        virtual void update(const ParameterPtr& other) = 0;
        virtual ParameterPtr newReference() = 0;

        //
        virtual void dump() = 0;
        virtual void dispose() = 0;

        template<typename, datatype_t, td_types> friend class TypeDefinition;
        friend class GroupParameter;
        friend class ParameterManager;

    protected:
        virtual void setDirty() = 0;

    private:
        virtual void setParent(GroupParameter& parent) = 0;
        virtual void clearParent() = 0;
        virtual void setManager(std::shared_ptr<IParameterManager> manager) = 0;

    };


    template <typename T,
              typename TD>
    class IValueParameter
    {
    public:
        // mandatory
        virtual TD& getDefaultTypeDefinition() = 0;
        virtual const TD& getDefaultTypeDefinition() const = 0;

        // optional
        virtual const T& getValue() const = 0;
        virtual void setValue(const T& value) = 0;
        virtual bool hasValue() const = 0;
        virtual void clearValue() = 0;
    };   

    class IElementParameter
    {
    public:
        virtual datatype_t getElementType() = 0;
    };
}

#endif // IPARAMETER_H
