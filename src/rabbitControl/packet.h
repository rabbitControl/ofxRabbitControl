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

#ifndef RCP_PACKET_H
#define RCP_PACKET_H

#include "parameter_intern.h"
#include "parameter_range.h"
#include "parameter_custom.h"
#include "parameter_parser.h"
#include "infodata.h"
#include "iddata.h"

namespace rcp {

    class Packet : public Writeable
    {
    public:
        static Option<Packet> parse(std::istream& is) {

            // read command
            command_t command = static_cast<command_t>(is.get());

            if (command == TERMINATOR) {
                // return empty option
                return Option<Packet>();
            }

            // check for valid command
            if (is.eof() ||
                command <= COMMAND_INVALID ||
                command >= COMMAND_MAX_)
            {
                // return empty option
                return Option<Packet>();
            }

            // create proper packet
            Option<Packet> packet_option = Packet(command);

            //------------------------------------
            // handle update value
            if (packet_option.getValue().getCommand() == COMMAND_UPDATEVALUE) {

                WriteablePtr param = ParameterParser::parseUpdateValue(is);

                if (param != nullptr) {
                    packet_option.getValue().setData(param);
                }

                if (!is.eof()) {
                    // should be eof
                    // throw error?
                }

                return packet_option;
            }


            //------------------------------------
            // read options
            while(!is.eof()) {

                // read option prefix
                packet_options_t packet_prefix = static_cast<packet_options_t>(is.get());

                if (packet_prefix == TERMINATOR) {
                    return packet_option;
                }

                if (is.eof() || is.fail()) {
                    break;
                }

                if (packet_prefix < PACKET_OPTIONS_TIMESTAMP || packet_prefix > PACKET_OPTIONS_DATA) {
                    std::cout << "error: unknown packet option: " << packet_prefix << "\n";
                    break;
                }

                switch (packet_prefix) {

                    case PACKET_OPTIONS_TIMESTAMP: {

                        uint64_t v = readFromStream(is, v);
                        CHECK_STREAM

                        packet_option.getValue().setTimestamp(v);
                        break;
                    }
                    // packet data
                    case PACKET_OPTIONS_DATA:

                        // already have data?
                        if (packet_option.getValue().hasData()) {
                            // error - data already set
                            // throw exception here?
                            break;
                        }

                        switch (command) {
                        case COMMAND_INVALID:
                        case COMMAND_MAX_:
                            // ERROR
                            break;

                        case COMMAND_INFO: {
                            // get infodata
                            const InfoDataPtr& info_data = InfoData::parse(is);
                            packet_option.getValue().setData(info_data);
                        }
                            break;

                        case COMMAND_INITIALIZE:
                        {
                            // exect ID-data or null
                            const IdDataPtr& id_data = IdData::parse(is);

                            if (id_data) {
                                packet_option.getValue().setData(id_data);
                            }

                            break;
                        }

                        case COMMAND_DISCOVER:
                            // TODO
                            // exect ID-data or null
                            // parameter-id followed by terminator
                            break;

                        case COMMAND_UPDATE:
                        {
                            // we expect a Parameter
                            WriteablePtr param = ParameterParser::parse(is);

                            if (param != nullptr) {
                                packet_option.getValue().setData(param);
                            }

                            break;
                        }

                        case COMMAND_REMOVE:
                        {
                            // we expect ID Data
                            const IdDataPtr& id_data = IdData::parse(is);

                            if (id_data) {
                                packet_option.getValue().setData(id_data);
                            }

                            break;
                        }

                        case COMMAND_UPDATEVALUE:
                            // handled above
                            break;
                        } // switch

                    break;
                } // switch
            } // while

            return packet_option;
        }

        Packet() :
            m_command(COMMAND_INVALID)
          , m_timestamp(0)
          , m_hasTimestamp(false)
          , m_data(nullptr)
          , m_hasData(false)
        {}

        Packet(enum command_t cmd) :
            m_command(cmd)
          , m_timestamp(0)
          , m_hasTimestamp(false)
          , m_data(nullptr)
          , m_hasData(false)
        {}

        Packet(enum command_t cmd, ParameterPtr& data) :
            m_command(cmd)
          , m_timestamp(0)
          , m_hasTimestamp(false)
        {
            setData(data);
        }

        Packet(enum command_t cmd, WriteablePtr& data) :
            m_command(cmd)
          , m_timestamp(0)
          , m_hasTimestamp(false)
        {
            setData(data);
        }

        Packet(enum command_t cmd, uint64_t timestamp, ParameterPtr& data) :
            m_command(cmd)
          , m_timestamp(timestamp)
          , m_hasTimestamp(true)
        {
            setData(data);
        }

        Packet(const Packet& other) :
            m_hasTimestamp(false)
          , m_data(nullptr)
          , m_hasData(false)
        {
            m_command = other.getCommand();

            if (other.hasTimestamp()) {
                setTimestamp(other.getTimestamp());
            }

            if (other.hasData()) {
                setData(other.getData());
            }
        }

        ~Packet();



        Packet& operator=(const Packet& other) {

            m_command = other.getCommand();

            if (other.hasTimestamp()) {
                setTimestamp(other.getTimestamp());
            } else {
                clearTimestamp();
            }

            if (other.hasData()) {
                setData(other.getData());
            } else {
                clearData();
            }

            return *this;
        }


        // writeable interface
        virtual void write(Writer& out, bool all) {

            out.write(static_cast<char>(m_command));

            if (m_hasTimestamp) {
                out.write(static_cast<char>(PACKET_OPTIONS_TIMESTAMP));
                out.write(m_timestamp);
            }

            if (m_hasData) {
                out.write(static_cast<char>(PACKET_OPTIONS_DATA));               
                m_data->write(out, all);
            }

            // terminator
            out.write(static_cast<char>(TERMINATOR));
        }


        // public methods

        void setCommand(enum command_t cmd) {
            m_command = cmd;
        }

        enum command_t getCommand() const {
            return m_command;
        }


        // option - timestamp
        bool hasTimestamp() const {
            return m_hasTimestamp;
        }

        void setTimestamp(uint64_t timestamp) {
            m_timestamp = timestamp;
            m_hasTimestamp = true;
        }

        uint64_t getTimestamp() const {
            return m_timestamp;
        }

        void clearTimestamp() {
            m_hasTimestamp = false;
        }


        // option - data
        bool hasData() const {
            return m_hasData;
        }

        void setData(IParameter& data) {
            setData(data.newReference());
        }

        void setData(const WriteablePtr& data) {
            m_data = data;
            m_hasData = data != nullptr;
        }

        WriteablePtr getData() const {
            return m_data;
        }

        void clearData() {
            m_data = nullptr;
            m_hasData = false;
        }


    private:

        // mandatory
        enum command_t m_command;

        // options
        uint64_t m_timestamp;
        bool m_hasTimestamp;

        WriteablePtr m_data;
        bool m_hasData;
    };


    std::ostream& operator<<(std::ostream& out, Packet& Packet);
}

#endif
