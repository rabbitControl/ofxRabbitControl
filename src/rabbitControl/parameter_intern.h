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

#ifndef RCP_PARAMETER_INTERN_H
#define RCP_PARAMETER_INTERN_H

#include <cinttypes>
#include <iostream>
#include <istream>
#include <string>
#include <map>
#include <vector>

#include "stream_tools.h"
#include "iparameter.h"
#include "iparametermanager.h"

#include "type_noopt.h"
#include "type_default.h"
#include "type_number.h"
#include "type_enum.h"
#include "type_uri.h"
#include "type_range.h"
#include "type_string.h"
#include "type_custom.h"
#include "type_array.h"


namespace rcp {

    class ParameterManager;
    class GroupParameter;
    typedef std::shared_ptr<GroupParameter> GroupParameterPtr;
    class ParameterFactory;

    static const std::string language_string_any("any");

    //----------------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------------
    /*
        88888888ba      db         88888888ba          db         88b           d88  88888888888  888888888888  88888888888  88888888ba
        88      "8b    d88b        88      "8b        d88b        888b         d888  88                88       88           88      "8b
        88      ,8P   d8'`8b       88      ,8P       d8'`8b       88`8b       d8'88  88                88       88           88      ,8P
        88aaaaaa8P'  d8'  `8b      88aaaaaa8P'      d8'  `8b      88 `8b     d8' 88  88aaaaa           88       88aaaaa      88aaaaaa8P'
        88""""""'   d8YaaaaY8b     88""""88'       d8YaaaaY8b     88  `8b   d8'  88  88"""""           88       88"""""      88""""88'
        88         d8""""""""8b    88    `8b      d8""""""""8b    88   `8b d8'   88  88                88       88           88    `8b
        88        d8'        `8b   88     `8b    d8'        `8b   88    `888'    88  88                88       88           88     `8b
        88       d8'          `8b  88      `8b  d8'          `8b  88     `8'     88  88888888888       88       88888888888  88      `8b
     */
    //----------------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------------
    template <typename TD>
    class Parameter : public IParameter
    {
    public:
        static std::shared_ptr< Parameter<TD> > create(int16_t id) {
            return std::make_shared< Parameter<TD> >(id);
        }

        Parameter(const Parameter<TD>& v) :
            obj(v.obj)
        {}

        Parameter(int16_t id) :
            obj(std::make_shared<Value>(id, TD(*this)))
        {}

        virtual ParameterPtr newReference() {
            return std::make_shared<Parameter<TD> >(*this);
        }


        //------------------------------------
        // implement writeable
        virtual void write(Writer& out, bool all) {

            out.write(getId());
            getTypeDefinition().write(out, all);

            writeOptions(out, all);

            // terminator
            out.write(static_cast<char>(TERMINATOR));

        }

        void writeOptions(Writer& out, bool all) {
            // writing options
            obj->write(out, all);
        }


        //------------------------------------
        // implement optionparser
        virtual void parseOptions(std::istream& is) {

            while (!is.eof()) {

                // read option prefix
                parameter_options_t opt = static_cast<parameter_options_t>(is.get());

                // check stream
                CHECK_STREAM

                if (opt == TERMINATOR) {
                    break;
                }


                switch (opt) {
                case PARAMETER_OPTIONS_LABEL: {

                    // peek one
                    int peek = is.peek();
                    CHECK_STREAM

                    char code[4];
                    code[3] = 0;
                    while (peek > 0) {

                        is.read(code, 3);

                        CHECK_STREAM

                        std::string code_str(code);
                        std::string label = readTinyString(is);

                        CHECK_STREAM

                        if (code_str == language_string_any) {
                            setLabel(label);
                        } else {
                            setLanguageLabel(code_str, label);
                        }

                        // peek next byte
                        peek = is.peek();
                    }

                    CHECK_STREAM

                    // read 0-byte off the stream
                    is.get();
                    break;
                }
                case PARAMETER_OPTIONS_DESCRIPTION: {

                    // peek one
                    int peek = is.peek();
                    CHECK_STREAM

                    char code[4];
                    code[3] = 0;
                    while (peek > 0) {

                        is.read(code, 3);                       

                        CHECK_STREAM

                        std::string code_str(code);
                        std::string description = readShortString(is);

                        CHECK_STREAM

                        if (code_str == language_string_any) {
                            setDescription(description);
                        } else {
                            setDescriptionLanguage(code_str, description);
                        }

                        // peek next byte
                        peek = is.peek();
                    }

                    CHECK_STREAM
                    // read 0-byte off the stream
                    is.get();
                    break;
                }
                case PARAMETER_OPTIONS_TAGS: {
                    std::string st = readTinyString(is);
                    CHECK_STREAM

                    setTags(st);
                    break;
                }
                case PARAMETER_OPTIONS_ORDER: {

                    int32_t val = readFromStream(is, obj->order);
                    CHECK_STREAM

                    setOrder(val);
                    break;
                }
                case PARAMETER_OPTIONS_PARENTID: {

                    int16_t parent_id = readFromStream(is, parent_id);
                    CHECK_STREAM

                    if (auto p = obj->parameterManager.lock()) {
                        ParameterPtr parent = p->getParameter(parent_id);
                        if (parent->getDatatype() == DATATYPE_GROUP) {
                            // parsed parameter is is a proxy parameter
                            // _not_ set as child of parent!
                            // proxy-parameter is either updating exisitng parameter in cache
                            // or is added. if the proxy-parameter is added it gets added to the parent too
                            setParent(std::dynamic_pointer_cast<GroupParameter>(parent));
                        }
                    }

                    break;
                }

                case PARAMETER_OPTIONS_WIDGET:
                    // TODO: handle this
                    break;
                case PARAMETER_OPTIONS_USERDATA:
                    // TODO: handle this
                    break;

                case PARAMETER_OPTIONS_USERID: {

                    std::string st = readTinyString(is);
                    CHECK_STREAM

                    setUserid(st);
                    break;
                }

                // handle all other cases in "handleOption"
//                case PARAMETER_OPTIONS_VALUE:
                default:
                    if (!handleOption(opt, is)) {
                        // error!
                        return;
                    }
                    break;
                }

            } // while
        } // parseOptions


        //------------------------------------
        // implement IParameter
        virtual int16_t getId() const { return obj->parameter_id; }
        virtual ITypeDefinition& getTypeDefinition() const { return obj->typeDefinition; }

        virtual datatype_t getDatatype() const {
            return getTypeDefinition().getDatatype();
        }

        // optional
        virtual const std::string getLabel() const { return obj->label; }
        virtual void setLabel(const std::string& label) {

            obj->hasLabel = true;

            if (obj->label == label) {
                return;
            }

            obj->label = label;
            obj->labelChanged = true;

            setDirty();
        }
        virtual bool hasLabel() const { return obj->hasLabel; }
        virtual void clearLabel() { obj->hasLabel = false; setDirty(); }

        // label languages
        virtual std::vector<std::string> getLabelLanguages() const {
            std::vector<std::string> keys;
            for(auto it = obj->languageLabel.begin(); it != obj->languageLabel.end(); ++it) {
              keys.push_back(it->first);
            }

            return keys;
        }

        virtual std::string getLanguageLabel(const std::string& code) const {

            std::map<std::string, std::string>::iterator it = obj->languageLabel.find(code);
            if (it != obj->languageLabel.end()) {
                return it->second;
            }

            return empty_string;
        }

        virtual void clearLanguageLabel() {

            obj->languageLabel.clear();
            obj->labelChanged = true;
            setDirty();
        }

        virtual void setLanguageLabel(const std::string& code, const std::string& label) {

            obj->languageLabel[code] = label;
            obj->labelChanged = true;
            setDirty();
        }

        virtual void removeLanguageLabel(const std::string& code) {

            auto it = obj->languageLabel.find(code);
            if (it != obj->languageLabel.end()) {
                obj->languageLabel.erase(it);

                obj->labelChanged = true;
                setDirty();
            }
        }

        virtual const std::string& getDescription() const { return obj->description; }
        virtual void setDescription(const std::string& description) {

            obj->hasDescription = true;

            if (obj->description == description) {
                return;
            }

            obj->description = description;
            obj->descriptionChanged = true;
            setDirty();
        }
        virtual bool hasDescription() const { return obj->hasDescription; }
        virtual void clearDescription() { obj->hasDescription = false; setDirty(); }

        // description languages
        virtual std::vector<std::string> getDescriptionLanguages() const {
            std::vector<std::string> keys;
            for(auto it = obj->languageDescription.begin(); it != obj->languageDescription.end(); ++it) {
              keys.push_back(it->first);
            }

            return keys;
        }

        virtual std::string getDescriptionLanguage(const std::string& code) const {

            std::map<std::string, std::string>::iterator it = obj->languageDescription.find(code);
            if (it != obj->languageDescription.end()) {
                return it->second;
            }

            return empty_string;
        }

        virtual void clearDescriptionLanguage() {

            obj->languageDescription.clear();
            obj->descriptionChanged = true;
            setDirty();
        }

        virtual void setDescriptionLanguage(const std::string& code, const std::string& description) {

            obj->languageDescription[code] = description;
            obj->descriptionChanged = true;
            setDirty();
        }

        virtual void removeDescriptionLanguage(const std::string& code) {

            auto it = obj->languageDescription.find(code);
            if (it != obj->languageDescription.end()) {
                obj->languageDescription.erase(it);

                obj->descriptionChanged = true;
                setDirty();
            }
        }

        virtual const std::string& getTags() const { return obj->tags; }
        virtual void setTags(const std::string& tags) {

            obj->hasTags = true;

            if (obj->tags == tags) {
                return;
            }

            obj->tags = tags;
            obj->tagsChanged = true;
            setDirty();
        }
        virtual bool hasTags() const { return obj->hasTags; }
        virtual void clearTags() {
            obj->hasTags = false;
            obj->tagsChanged = true;
            setDirty();
        }


        virtual const int32_t& getOrder() const { return obj->order; }
        virtual void setOrder(const int32_t& order) {

            obj->hasOrder = true;

            if (obj->order == order) {
                return;
            }

            obj->order = order;
            obj->orderChanged = true;
            setDirty();
        }
        virtual bool hasOrder() const { return obj->hasOrder; }
        virtual void clearOrder() {
            obj->hasOrder = false;
            obj->orderChanged = true;
            setDirty();
        }


        virtual std::weak_ptr<GroupParameter>& getParent() const;
        virtual bool hasParent() const { return obj->parent.lock() != nullptr; }


        virtual const std::vector<char> getUserdata() const { return obj->userdata; }
        virtual void setUserdata(const std::vector<char> userdata) {
            obj->userdata = userdata;
            obj->userdataChanged = true;
            setDirty();
        }
        virtual bool hasUserdata() const { return obj->userdata.size() > 0; }
        virtual void clearUserdata() {
            obj->userdata.clear();
            obj->userdataChanged = true;
            setDirty();
        }

        virtual const std::string& getUserid() const { return obj->userid; }
        virtual void setUserid(const std::string& userid) {

            obj->hasUserid = true;

            if (obj->userid == userid) {
                return;
            }

            obj->userid = userid;
            obj->useridChanged = true;
            setDirty();
        }
        virtual bool hasUserid() const { return obj->hasUserid; }
        virtual void clearUserid() {
            obj->hasUserid = false;
            obj->useridChanged = true;
            setDirty();
        }

        //
        virtual void dump();
        virtual void update(const ParameterPtr& other);

        // update callbacks
//        const std::function< void() >& addChangedCb(std::function< void() > const& func) {

//            for(auto it = obj->updatedCallbacks.begin(); it != obj->updatedCallbacks.end(); it++ )    {
//                if (&func == &(*it).get()) {
//                    // allready contained
//                    return func;
//                }
//            }

//            obj->updatedCallbacks.push_back(const_cast<std::function< void() >& >(func));
//            return func;
//        }

//        void removeChangedCb(const std::function< void() >& func) {
//            for(auto it = obj->updatedCallbacks.begin(); it != obj->updatedCallbacks.end(); it++ )    {
//                if (&func == &(*it).get()) {
//                    obj->updatedCallbacks.erase(it);
//                    break;
//                }
//            }
//        }

        void setUpdatedCb(std::function< void() >&& func) {
            obj->updatedCb = std::move(func);
        }

        void clearUpdatedCb() {
            obj->updatedCallbacks.clear();
            obj->updatedCb = nullptr;
        }

        virtual void dispose() {
            clearUpdatedCb();
        }

        friend class ParameterManager;
        friend class GroupParameter;
        friend class ParameterFactory;
        template<typename, typename, datatype_t> friend class ValueParameter;

    protected:
        virtual bool handleOption(const parameter_options_t& opt, std::istream& is) {
            return false;
        }

        TD& getRealTypeDef() { return obj->typeDefinition; }
        const TD& getRealTypeDef() const { return obj->typeDefinition; }

        void setDirty() {
            if (auto p = obj->parameterManager.lock()) {
                p->setParameterDirty(*this);
            }
        }

    private:

        virtual void setParent(GroupParameter& parent);
        virtual void setParent(GroupParameterPtr parent);
        virtual void clearParent() {
            obj->parent.reset();
            obj->parentChanged = true;
            setDirty();
        }

        virtual void setManager(std::shared_ptr<IParameterManager> manager) {
            obj->parameterManager = manager;
            setDirty();
        }

        class Value {
        public:
            Value(int16_t id, const TD& td) :
                parameter_id(id)
              , typeDefinition(td)
            {
                // init vars
                label = empty_string;
                hasLabel = false;
                labelChanged = false;

                description = empty_string;
                hasDescription = false;
                descriptionChanged = false;

                tags = empty_string;
                hasTags = false;
                tagsChanged = false;

                order = 0;
                hasOrder = false;
                orderChanged = false;

                parentChanged = false;

                userdata.clear();
                userdataChanged = false;

                userid = empty_string;
                hasUserid = false;
                useridChanged = false;
            }

            // write label and all language labels
            void writeLabel(Writer& out) {

                out.write(static_cast<char>(PARAMETER_OPTIONS_LABEL));

                if (hasLabel) {

                    out.write(language_string_any, false);
                    out.writeTinyString(label);
                }

                for (auto& l : languageLabel) {

                    if (l.first.length() < 3) {
                        continue;
                    }

                    out.write(l.first.substr(0, 3), false);
                    out.writeTinyString(l.second);
                }

                out.write(static_cast<char>(TERMINATOR));
            }

            // write description and language description
            void writeDescription(Writer& out) {

                out.write(static_cast<char>(PARAMETER_OPTIONS_DESCRIPTION));

                if (hasDescription) {

                    out.write(language_string_any, false);
                    out.writeShortString(description);
                }

                for (auto& l : languageDescription) {

                    if (l.first.length() < 3) {
                        continue;
                    }

                    out.write(l.first.substr(0, 3), false);
                    out.writeShortString(l.second);
                }

                out.write(static_cast<char>(TERMINATOR));
            }


            // write all options
            void write(Writer& out, bool all) {

                // label
                if (hasLabel || languageLabel.size() > 0) {

                    if (all || labelChanged) {

                        writeLabel(out);

                        if (!all) {
                            labelChanged = false;
                        }
                    }
                } else if(labelChanged) {

                    out.write(static_cast<char>(PARAMETER_OPTIONS_LABEL));
                    out.write(static_cast<char>(0));
                    labelChanged = false;
                }


                // description
                if (hasDescription || languageDescription.size() > 0) {

                    if (all || descriptionChanged) {

                        writeDescription(out);

                        if (!all) {
                            descriptionChanged = false;
                        }
                    }
                } else if (descriptionChanged) {

                    out.write(static_cast<char>(PARAMETER_OPTIONS_DESCRIPTION));
                    out.write(static_cast<char>(0));
                    descriptionChanged = false;
                }


                // tags
                if (hasTags) {

                    if (all || tagsChanged) {
                        out.write(static_cast<char>(PARAMETER_OPTIONS_TAGS));
                        out.writeTinyString(tags);

                        if (!all) {
                            tagsChanged = false;
                        }
                    }
                } else if (tagsChanged) {

                    out.write(static_cast<char>(PARAMETER_OPTIONS_TAGS));
                    out.writeTinyString("");
                    tagsChanged = false;
                }

                // order
                if (hasOrder) {

                    if (all || orderChanged) {
                        out.write(static_cast<char>(PARAMETER_OPTIONS_ORDER));
                        out.write(order);

                        if (!all) {
                            orderChanged = false;
                        }
                    }
                } else if (orderChanged) {

                    out.write(static_cast<char>(PARAMETER_OPTIONS_ORDER));
                    out.write(static_cast<int32_t>(0));
                    orderChanged = false;
                }


                // parent id
                if (auto p = parent.lock()) {

                    if (all || parentChanged) {
                        out.write(static_cast<char>(PARAMETER_OPTIONS_PARENTID));
                        out.write(p->getId());

                        if (!all) {
                            parentChanged = false;
                        }
                    }
                } else if (parentChanged) {

                    out.write(static_cast<char>(PARAMETER_OPTIONS_PARENTID));
                    out.write(static_cast<int16_t>(0));
                    parentChanged = false;
                }


                // TODO: widget

                // userdata
                if (userdata.size() > 0) {

                    if (all || userdataChanged) {
                        out.write(static_cast<char>(PARAMETER_OPTIONS_USERDATA));
                        out.write(&userdata[0], userdata.size());

                        if (!all) {
                            userdataChanged = false;
                        }
                    }
                } else if (userdataChanged) {

                    out.write(static_cast<char>(PARAMETER_OPTIONS_USERDATA));
                    out.write(static_cast<uint32_t>(0));
                    userdataChanged = false;
                }


                // userid
                if (hasUserid) {

                    if (all || useridChanged) {
                        out.write(static_cast<char>(PARAMETER_OPTIONS_USERID));
                        out.writeTinyString(userid);

                        if (!all) {
                            useridChanged = false;
                        }
                    }
                } else if (useridChanged) {

                    out.write(static_cast<char>(PARAMETER_OPTIONS_USERID));
                    out.writeTinyString("");
                    useridChanged = false;
                }
            }


            void callUpdatedCb() {

                updatedCb();

                for (auto& function : updatedCallbacks) {
                    function();
                }
            }

            // mandatory
            int16_t parameter_id;
            TD typeDefinition;

            // optional fields
            std::string label{};
            std::map<std::string, std::string> languageLabel;
            bool hasLabel;
            bool labelChanged;

            std::string description{};
            std::map<std::string, std::string> languageDescription;
            bool hasDescription;
            bool descriptionChanged;

            std::string tags{};
            bool hasTags;
            bool tagsChanged;

            int32_t order{};
            bool hasOrder;
            bool orderChanged;

            std::weak_ptr<GroupParameter> parent;
            bool parentChanged;

            //TODO: PARAMETER_WIDGET

            std::vector<char> userdata;
            bool userdataChanged;

            std::string userid{};
            bool hasUserid;
            bool useridChanged;

            std::function< void() > updatedCb;
            std::vector< std::reference_wrapper< std::function< void() > > > updatedCallbacks;
            std::weak_ptr<IParameterManager> parameterManager;
        };

        std::shared_ptr<Value> obj;

        //
        Parameter(std::shared_ptr<Value> obj) :
            obj(obj)
        {}
    };



    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
    /*   
        8b           d8   db         88          88        88  88888888888
        `8b         d8'  d88b        88          88        88  88
         `8b       d8'  d8'`8b       88          88        88  88
          `8b     d8'  d8'  `8b      88          88        88  88aaaaa
           `8b   d8'  d8YaaaaY8b     88          88        88  88"""""
            `8b d8'  d8""""""""8b    88          88        88  88
             `888'  d8'        `8b   88          Y8a.    .a8P  88
              `8'  d8'          `8b  88888888888  `"Y8888Y"'   88888888888
     */
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
    template<
        typename T,
        typename TD,
        datatype_t type_id
    >
    class ValueParameter : public Parameter<TD>, public IValueParameter<T, TD>
    {
    public:
        static std::shared_ptr< ValueParameter<T, TD, type_id> > create(int16_t id) {
            return std::make_shared< ValueParameter<T, TD, type_id> >(id);
        }


        ValueParameter(const ValueParameter<T, TD, type_id>& v) :
            Parameter<TD>(v)
          , obj(v.obj)
        {}

        ValueParameter(int16_t id) :
            Parameter<TD>(id)
          , obj(std::make_shared<Value>())
        {}

        ValueParameter(int16_t id, T& init) :
            Parameter<TD>(id)
          , obj(std::make_shared<Value>())
        {
            setValue(init);
        }

        ValueParameter(int16_t id, const T& init) :
            Parameter<TD>(id)
          , obj(std::make_shared<Value>())
        {
            setValue(init);
        }

        ~ValueParameter()
        {}


        virtual ParameterPtr newReference() {
            return std::make_shared<ValueParameter<T, TD, type_id> >(*this);
        }

        //------------------------------------
        // implement writeable
        virtual void write(Writer& out, bool all) {

            out.write(Parameter<TD>::getId());
            getDefaultTypeDefinition().write(out, all);

            obj->write(out, all);

            Parameter<TD>::writeOptions(out, all);

            // terminator
            out.write(static_cast<char>(TERMINATOR));
        }

        virtual void dump() {
            Parameter<TD>::dump();

            if (hasValue()) {
                std::cout << "value: " << obj->value << "\n";
            }
        }

        virtual bool handleOption(const parameter_options_t& opt, std::istream& is) {

            if (opt == PARAMETER_OPTIONS_VALUE) {

                // read options
                T val = getDefaultTypeDefinition().readValue(is);
                CHECK_STREAM_RETURN(false)

                setValue(val);
                return true;
            }

            // could not parse... error
            return false;
        }

        // implement IValueParameter
        const T& getValue() const { return obj->value; }
        void setValue(const T& value) {

            obj->hasValue = true;

            // todo: compare??
            if (obj->value == value) {
                return;
            }

            obj->value = value;
            obj->valueChanged = true;
            setDirty();
        }
        virtual bool hasValue() const { return obj->hasValue; }
        virtual void clearValue() {
            obj->hasValue = false;
            obj->valueChanged = true;
            setDirty();
        }

        virtual TD& getDefaultTypeDefinition() {
            return Parameter<TD>::getRealTypeDef();
        }
        virtual const TD& getDefaultTypeDefinition() const {
            return Parameter<TD>::getRealTypeDef();
        }



        //--------------------------------------------
        // convenience - default
        // do that in subclass?

        void setDefault(const T& value) {
            Parameter<TD>::getRealTypeDef().setDefault(value);
        }
        const T& getDefault() const {
            return Parameter<TD>::getRealTypeDef().getDefault();
        }


        //--------------------------------------------
        // convenience - number
        // do that in subclass?

        template<class Q = T>
        typename std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value, T>::type
        getMinimum() const
        {
            const TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            return d.getMinimum();
        }

        template<class Q = T>
        void setMinimum(const typename std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value, T>::type& value)
        {
            TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            d.setMinimum(value);
        }

        template<class Q = T>
        typename std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value, T>::type
        getMaximum() const
        {
            const TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            return d.getMaximum();
        }

        template<class Q = T>
        void setMaximum(const typename std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value, T>::type& value)
        {
            TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            d.setMaximum(value);
        }

        template<class Q = T>
        typename std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value, T>::type
        getMultipleof() const
        {
            const TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            return d.getMultipleof();
        }

        template<class Q = T>
        void setMultipleof(const typename std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value, T>::type& value)
        {
            TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            d.setMultipleof(value);
        }


        template<class Q = T,
                 typename = std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value > >
        void setScale(const number_scale_t& scale)
        {
            TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            d.setScale(scale);
        }

        template<class Q = T,
                 typename = std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value > >
        number_scale_t getScale() const
        {
            const TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            return d.getScale();
        }

        template<class Q = T,
                 typename = std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value > >
        void setUnit(const std::string& unit)
        {
            TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            d.setUnit(unit);
        }

        template<class Q = T,
                 typename = std::enable_if<std::is_arithmetic<Q>::value && !std::is_same<Q, bool>::value > >
        std::string getUnit() const
        {
            const TypeDefinition<T, type_id, td_num >& d = Parameter<TD>::getRealTypeDef();
            return d.getUnit();
        }



        //--------------------------------------------
        // convenience - other... string, enum, etc
        // do that in subclasses?

        using Parameter<TD>::getId;
        using Parameter<TD>::getTypeDefinition;

        virtual void update(const ParameterPtr& other) {

            if (other.get() == this) {
                //c'mon, it's me!
                return;
            }

            if (other->getId() != getId()) {
                std::cerr << "trying to update value-parameter from parameter with wrong id!\n";
                return;
            }

            // update value
            bool updated = false;

            auto v_other = std::dynamic_pointer_cast<ValueParameter<T, TD, type_id> >(other);
            if (v_other) {
                if (v_other->hasValue()) {
                    setValue(v_other->getValue());
                    updated = true;
                }
            }

            if (updated) {
                obj->callValueChangedCb();
            }

            // update base
            Parameter<TD>::update(other);
        }

//        const std::function< void ( T& )>& addValueUpdatedCb(std::function< void(T&) >&& func) {

//            for(auto it = obj->valueUpdatedCallbacks.begin(); it != obj->valueUpdatedCallbacks.end(); it++ )    {
//                if (&func == &(*it).get()) {
//                    // allready contained
//                    break;
//                }
//            }

//            obj->valueUpdatedCallbacks.push_back(const_cast<std::function< void ( T& )>&>(func));
//            return func;
//        }

//        void removeValueUpdatedCb(const std::function< void ( T& )> & func) {
//            for(auto it = obj->valueUpdatedCallbacks.begin(); it != obj->valueUpdatedCallbacks.end(); it++ )    {
//                if (&func == &(*it).get()) {
//                    obj->valueUpdatedCallbacks.erase(it);
//                    break;
//                }
//            }
//        }

        void setValueUpdatedCb(std::function< void(T&) > const&& func) {
            obj->vcb = std::move(func);
        }


        void clearValueUpdatedCb() {
            obj->valueUpdatedCallbacks.clear();
            obj->vcb = nullptr;
        }

        virtual void dispose() {
            clearValueUpdatedCb();
            Parameter<TD>::dispose();
        }

        friend class ParameterManager;
        friend class GroupParameter;
        friend class ParameterFactory;

    protected:
        using Parameter<TD>::setDirty;

    private:
        class Value {
        public:
            Value() :
                hasValue(false)
              , valueChanged(false)
            {}

            // writing options
            void write(Writer& out, bool all) {

                // value
                if (hasValue) {

                    if (all || valueChanged) {
                        out.write(static_cast<char>(PARAMETER_OPTIONS_VALUE));
                        out.write(value);

                        if (!all) {
                            valueChanged = false;
                        }
                    }
                } else if (valueChanged) {
                    out.write(static_cast<char>(PARAMETER_OPTIONS_VALUE));

                    T v{};
                    out.write(v);

                    valueChanged = false;
                }
            }

            void callValueChangedCb() {

                if (vcb) {
                    vcb(value);
                }

                for (auto& f : valueUpdatedCallbacks) {
                    f(value);
                }
            }

            T value{};
            bool hasValue;
            bool valueChanged;

            std::function< void(T&) > vcb;
            std::vector< std::reference_wrapper< std::function< void(T&) > > > valueUpdatedCallbacks;
        };
        std::shared_ptr<Value> obj;

        //
        ValueParameter(std::shared_ptr<Value> obj) :
            obj(obj)
        {}
    };




    /*
      ,ad8888ba,   88888888ba     ,ad8888ba,    88        88  88888888ba
     d8"'    `"8b  88      "8b   d8"'    `"8b   88        88  88      "8b
    d8'            88      ,8P  d8'        `8b  88        88  88      ,8P
    88             88aaaaaa8P'  88          88  88        88  88aaaaaa8P'
    88      88888  88""""88'    88          88  88        88  88""""""'
    Y8,        88  88    `8b    Y8,        ,8P  88        88  88
     Y8a.    .a88  88     `8b    Y8a.    .a8P   Y8a.    .a8P  88
      `"Y88888P"   88      `8b    `"Y8888Y"'     `"Y8888Y"'   88
    */
    class GroupParameter : public Parameter<GroupTypeDefinition>, public std::enable_shared_from_this<GroupParameter>
    {
    public:
        static GroupParameterPtr create(int16_t id) {
            return std::make_shared<GroupParameter>(id);
        }

        GroupParameter(const GroupParameter& v) :
            Parameter<GroupTypeDefinition>(v)
          , obj(v.obj)
        {}

        GroupParameter() :
            Parameter<GroupTypeDefinition>(static_cast<int16_t>(0)),
            obj(std::make_shared<Value>())
        {}

        GroupParameter(int16_t id) :
            Parameter<GroupTypeDefinition>(id),
            obj(std::make_shared<Value>())
        {}


        virtual ParameterPtr newReference() {
            return std::make_shared<GroupParameter>(*this);
        }

        //
        void dumpChildren(int indent);

        template<typename> friend class Parameter;
        friend class ParameterManager;
        friend class ParameterServer;
        friend class ParameterFactory;

    private:
        void addChild(IParameter& child);
        void addChild(ParameterPtr& child);
        void removeChild(IParameter& child);
        void removeChild(ParameterPtr& child);

        GroupParameterPtr getShared() {
            return shared_from_this();
        }

        std::map<short, ParameterPtr >& children() {
            return obj->children;
        }

        class Value {
        public:
            Value()
            {}

            std::map<short, ParameterPtr > children;
        };

        std::shared_ptr<Value> obj;

        GroupParameter(std::shared_ptr<Value> obj) :
            Parameter<GroupTypeDefinition>(static_cast<int16_t>(0))
          , obj(obj)
        {}
    };




    template <typename TD>
    void Parameter<TD>::update(const ParameterPtr& other) {

        if (other.get() == this) {
            //c'mon, it's me!
            return;
        }

        if (other->getId() != getId()) {
            std::cerr << "trying to update from parameter with wrong id: " << getId() << " != " << other->getId() << "\n";
            return;
        }

        bool updated = false;

        if (other->hasLabel()) {
            setLabel(other->getLabel());
            updated = true;
        }

        if (other->hasDescription()) {
            setDescription(other->getDescription());
            updated = true;
        }

        if (other->hasTags()) {
            setTags(other->getTags());
            updated = true;
        }

        if (other->hasOrder()) {
            setOrder(other->getOrder());
            updated = true;
        }

        if (other->hasParent()) {
            std::shared_ptr<GroupParameter> p = other->getParent().lock();
            p->addChild(*this);
            updated = true;
        }

        if (other->hasUserdata()) {
            setUserdata(other->getUserdata());
            updated = true;
        }

        if (other->hasUserid()) {
            setUserid(other->getUserid());
            updated = true;
        }

        if (updated) {
            obj->callUpdatedCb();
        }
    }


    template <typename TD>
    std::weak_ptr<GroupParameter>& Parameter<TD>::getParent() const {
        return obj->parent;
    }

    template <typename TD>
    void Parameter<TD>::setParent(GroupParameterPtr parent) {

        if (auto p = obj->parent.lock()) {
            if (parent->getId() == p->getId()) {
                // parent already set
                return;
            } else {
                // remove from parent...
                p->removeChild(*this);
            }
        }

        obj->parent = parent;
        obj->parentChanged = true;
        setDirty();
    }

    template <typename TD>
    void Parameter<TD>::setParent(GroupParameter& parent) {
        setParent(parent.getShared());
    }




    template <typename TD>
    void Parameter<TD>::dump() {

        std::cout << "parameter id: " << getId() << "\n";
        getTypeDefinition().dump();

        if (hasLabel()) {
            std::cout << "label: " << getLabel() << "\n";
        }
        auto langs = getLabelLanguages();
        if (langs.size() < 0) {
            for (auto& code : langs) {
                std::cout << "label " << code << ": " << getLanguageLabel(code) << "\n";
            }
        }

        if (hasDescription()) {
            std::cout << "description: " << getDescription() << "\n";
        }
        auto descs = getDescriptionLanguages();
        if (descs.size() < 0) {
            for (auto& code : descs) {
                std::cout << "description " << code << ": " << getDescriptionLanguage(code) << "\n";
            }
        }

        if (hasTags()) {
            std::cout << "Tags: " << getTags() << "\n";
        }

        if (hasOrder()) {
            std::cout << "order: " << getOrder() << "\n";
        }

        if (hasParent()) {
            if (auto p = obj->parent.lock()) {
                std::cout << "parent id: " << p->getId() << "\n";
            } else {
                std::cout << "parent id: -\n";
            }
        }

        if (hasUserdata()) {
            std::cout << "has userdata\n";
        }

        if (hasUserid()) {
            std::cout << "userid: " << getUserid() << "\n";
        }
    }


    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    /*
                                                                88                 ad88
          ,d                                                    88                d8"
          88                                                    88                88
        MM88MMM  8b       d8  8b,dPPYba,    ,adPPYba,   ,adPPYb,88   ,adPPYba,  MM88MMM
          88     `8b     d8'  88P'    "8a  a8P_____88  a8"    `Y88  a8P_____88    88
          88      `8b   d8'   88       d8  8PP"""""""  8b       88  8PP"""""""    88
          88,      `8b,d8'    88b,   ,a8"  "8b,   ,aa  "8a,   ,d88  "8b,   ,aa    88
          "Y888      Y88'     88`YbbdP"'    `"Ybbd8"'   `"8bbdP"Y8   `"Ybbd8"'    88
                     d8'      88
                    d8'       88
    */
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    typedef ValueParameter<bool, BooleanTypeDefinition, DATATYPE_BOOLEAN > BooleanParameter;
    typedef std::shared_ptr<BooleanParameter> BooleanParameterPtr;

    typedef ValueParameter<int8_t, Int8TypeDefinition, DATATYPE_INT8 > Int8Parameter;
    typedef ValueParameter<uint8_t, UInt8TypeDefinition, DATATYPE_UINT8 > UInt8Parameter;
    typedef std::shared_ptr<Int8Parameter> Int8ParameterPtr;
    typedef std::shared_ptr<UInt8Parameter> UInt8ParameterPtr;

    typedef ValueParameter<int16_t, Int16TypeDefinition, DATATYPE_INT16 > Int16Parameter;
    typedef ValueParameter<uint16_t, UInt16TypeDefinition, DATATYPE_UINT16 > UInt16Parameter;
    typedef std::shared_ptr<Int16Parameter> Int16ParameterPtr;
    typedef std::shared_ptr<UInt16Parameter> UInt16ParameterPtr;

    typedef ValueParameter<int32_t, Int32TypeDefinition, DATATYPE_INT32 > Int32Parameter;
    typedef ValueParameter<uint32_t, UInt32TypeDefinition, DATATYPE_UINT32 > UInt32Parameter;
    typedef std::shared_ptr<Int32Parameter> Int32ParameterPtr;
    typedef std::shared_ptr<UInt32Parameter> UInt32ParameterPtr;

    typedef ValueParameter<int64_t, Int64TypeDefinition, DATATYPE_INT64 > Int64Parameter;
    typedef ValueParameter<uint64_t, UInt64TypeDefinition, DATATYPE_UINT64 > UInt64Parameter;
    typedef std::shared_ptr<Int64Parameter> Int64ParameterPtr;
    typedef std::shared_ptr<UInt64Parameter> UInt64ParameterPtr;

    typedef ValueParameter<float, Float32TypeDefinition, DATATYPE_FLOAT32 > Float32Parameter;
    typedef std::shared_ptr<Float32Parameter> Float32ParameterPtr;

    typedef ValueParameter<double, Float64TypeDefinition, DATATYPE_FLOAT64 > Float64Parameter;
    typedef std::shared_ptr<Float64Parameter> Float64ParameterPtr;

    typedef ValueParameter<std::string, StringTypeDefinition, DATATYPE_STRING > StringParameter;
    typedef std::shared_ptr<StringParameter> StringParameterPtr;

    typedef ValueParameter<std::string, EnumTypeDefinition, DATATYPE_ENUM > EnumParameter;
    typedef std::shared_ptr<EnumParameter> EnumParameterPtr;

    typedef ValueParameter<Color, RGBTypeDefinition, DATATYPE_RGB > RGBParameter;
    typedef ValueParameter<Color, RGBATypeDefinition, DATATYPE_RGBA > RGBAParameter;
    typedef std::shared_ptr<RGBParameter> RGBParameterPtr;
    typedef std::shared_ptr<RGBAParameter> RGBAParameterPtr;

    typedef ValueParameter<std::string, UriTypeDefinition, DATATYPE_URI > URIParameter;
    typedef std::shared_ptr<URIParameter> URIParameterPtr;

    typedef ValueParameter<IPv4, IPv4TypeDefinition, DATATYPE_IPV4 > IPv4Parameter;
    typedef ValueParameter<IPv6, IPv6TypeDefinition, DATATYPE_IPV6 > IPv6Parameter;
    typedef std::shared_ptr<IPv4Parameter> IPv4ParameterPtr;
    typedef std::shared_ptr<IPv6Parameter> IPv6ParameterPtr;

    typedef Parameter<BangTypeDefinition> BangParameter;
    typedef std::shared_ptr<BangParameter> BangParameterPtr;

    typedef Parameter<InvalidTypeDefinition> InvalidParameter;
    typedef std::shared_ptr<InvalidParameter> InvalidParameterPtr;


    //
    template<
        typename T,
        typename TD,
        datatype_t type_id
    >
    std::ostream& operator<<(std::ostream& out, ValueParameter<T, TD, type_id>& param);
}



#endif
