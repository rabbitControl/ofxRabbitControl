/*
********************************************************************
* rabbitcontrol cpp
*
* written by: Ingo Randolf - 2020
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

#ifndef OFXRABBITCONTROL_WEBSOCKET_SERVER_TRANSPORTER_H
#define OFXRABBITCONTROL_WEBSOCKET_SERVER_TRANSPORTER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

#include <boost/interprocess/streams/bufferstream.hpp>
#include <iostream>
#include <set>


#include "rabbitControl/servertransporter.h"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

enum action_type {
    SUBSCRIBE,
    UNSUBSCRIBE,
    MESSAGE
};

struct action {
    action(action_type t, connection_hdl h) : type(t), hdl(h) {}
    action(action_type t, connection_hdl h, server::message_ptr m)
      : type(t), hdl(h), msg(m) {}

    action_type type;
    websocketpp::connection_hdl hdl;
    server::message_ptr msg;
};


#include <ofThread.h>
#include <ofLog.h>

class websocketServerTransporter : public ofThread, public rcp::ServerTransporter
{
public:
    websocketServerTransporter()
    {
        // set log level
        m_server.clear_access_channels(websocketpp::log::alevel::all);
//        m_server.set_access_channels(websocketpp::log::alevel::frame_payload);
//        m_server.set_access_channels(websocketpp::log::alevel::connect);
//        m_server.set_access_channels(websocketpp::log::alevel::disconnect);
//        m_server.set_access_channels(websocketpp::log::alevel::app);

        // Initialize Asio Transport
        m_server.init_asio();
        m_server.set_reuse_addr(true);

        // Register handler callbacks
        m_server.set_open_handler(std::bind(&websocketServerTransporter::on_open,this,::_1));
        m_server.set_close_handler(std::bind(&websocketServerTransporter::on_close,this,::_1));
        m_server.set_message_handler(std::bind(&websocketServerTransporter::on_message,this,::_1,::_2));

        // start service thread
        ws_thread = new websocketpp::lib::thread(std::bind(&websocketServerTransporter::process_messages, this));
    }

    ~websocketServerTransporter()
    {
        unbind();
        waitForThread();

        if (ws_thread)
        {
            {
                lock_guard<websocketpp::lib::mutex> guard(m_action_lock);
                m_run = false;
            }
            m_action_cond.notify_one();
            ws_thread->join();
        }
    }


    //----------------------------------------
    // ofThread
    // the thread function
    void threadedFunction()
    {
        try
        {
            // Listen on port
            m_server.listen(uint16_t(m_port));

            // Start the server accept loop
            m_server.start_accept();

            // Start the ASIO io_service run loop
            m_server.run();
        }
        catch (const std::exception & e)
        {
            ofLogNotice() << e.what();
        }
    }

    //----------------------------------------
    // transporter interface
    virtual void bind(int port)
    {
        unbind();

        m_port = port;

        startThread();
    }


    virtual void unbind()
    {
        if (!m_server.is_listening()) {
            return;
        }

        m_server.stop_listening();
        m_server.stop();
        m_connections.clear();
    }


    virtual void sendToOne(std::istream& data, void* id)
    {
        if (!m_server.is_listening()) {
            return;
        }

        if (id == nullptr) {
            return;
        }

        data.seekg (0, data.end);
        size_t length = data.tellg();
        data.seekg (0, data.beg);

        char d[length];
        data.read(d, length);

        for (auto& conn : m_connections)
        {
            if (auto p = conn.lock())
            {
                if (id == p.get()) {
                    m_server.send(conn, d, length, websocketpp::frame::opcode::value::binary);
                }
            }
            else
            {
                // TODO: handle this?
            }
        }
    }

    virtual void sendToAll(std::istream& data, void* excludeId) {

        if (!m_server.is_listening()) {
            return;
        }

        data.seekg (0, data.end);
        size_t length = data.tellg();
        data.seekg (0, data.beg);

        char d[length];
        data.read(d, length);

        for (auto& conn : m_connections)
        {
            if (auto p = conn.lock())
            {
                if (excludeId == p.get())
                {
                    continue;
                }
                m_server.send(conn, d, length, websocketpp::frame::opcode::value::binary);
            }
        }
    }

    virtual int getConnectionCount()
    {
        return int(m_connections.size());
    }

    // websocket methods
    void on_open(connection_hdl hdl)
    {
        {
            lock_guard<websocketpp::lib::mutex> guard(m_action_lock);
            m_actions.push(action(SUBSCRIBE,hdl));
        }
        m_action_cond.notify_one();
    }

    void on_close(connection_hdl hdl)
    {
        {
            lock_guard<websocketpp::lib::mutex> guard(m_action_lock);
            m_actions.push(action(UNSUBSCRIBE,hdl));
        }
        m_action_cond.notify_one();
    }

    void on_message(connection_hdl hdl, server::message_ptr msg)
    {
        // queue message up for sending by processing thread
        {
            lock_guard<websocketpp::lib::mutex> guard(m_action_lock);
            m_actions.push(action(MESSAGE,hdl,msg));
        }
        m_action_cond.notify_one();
    }


    void process_messages()
    {
        m_run = true;

        while (m_run)
        {
            unique_lock<websocketpp::lib::mutex> lock(m_action_lock);

            while (m_run && m_actions.empty())
            {
                m_action_cond.wait(lock);
            }

            if (!m_run) break;

            action a = m_actions.front();
            m_actions.pop();

            lock.unlock();

            //
            if (a.type == SUBSCRIBE)
            {
                lock_guard<websocketpp::lib::mutex> guard(m_connection_lock);
                m_connections.insert(a.hdl);
            }
            else if (a.type == UNSUBSCRIBE)
            {
                lock_guard<websocketpp::lib::mutex> guard(m_connection_lock);
                m_connections.erase(a.hdl);
            }
            else if (a.type == MESSAGE)
            {
                lock_guard<websocketpp::lib::mutex> guard(m_connection_lock);

                if (a.msg->get_opcode() == websocketpp::frame::opcode::value::binary)
                {
                    auto data = a.msg->get_raw_payload();

                    boost::interprocess::bufferstream input_stream(const_cast<char*>(data.data()), data.size());

                    if (auto ptr = a.hdl.lock())
                    {
                        // call receive callbacks
                        for (const auto& kv : receive_cb) {
                            (kv.first->*kv.second)(input_stream, *this, ptr.get());
                        }
                    }
                }
                else
                {
                    // got text message
                    ofLogNotice() << "got text message: " << a.msg->get_payload();
                }
            }
            else
            {
                // undefined.
            }
        }
    }

private:
    typedef std::set<connection_hdl,std::owner_less<connection_hdl> > con_list;

    server m_server;
    con_list m_connections;
    std::queue<action> m_actions;

    websocketpp::lib::mutex m_action_lock;
    websocketpp::lib::mutex m_connection_lock;
    condition_variable m_action_cond;

    // service thread
    websocketpp::lib::thread *ws_thread{nullptr};
    std::atomic_bool m_run{false};
    uint16_t m_port{0};
};

#endif // OFXRABBITCONTROL_WEBSOCKET_SERVER_TRANSPORTER_H
