/*
********************************************************************
* websocketClient h
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

#ifndef RABBITCONTROL_WEBSOCKET_CLIENT_H
#define RABBITCONTROL_WEBSOCKET_CLIENT_H

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::client<websocketpp::config::asio_tls_client> ssl_client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

using websocketpp::lib::thread;

namespace rcp {

class websocketClient
{

public:

    websocketClient();
	virtual ~websocketClient();
	
public:
	//
	virtual void connected() = 0;
	virtual void disconnected() = 0;
	virtual void received(char* data, size_t size) = 0;

public:
    void on_open(websocketpp::connection_hdl /*hdl*/)
    {
        connected();
    }

    void on_fail(websocketpp::connection_hdl /*hdl*/)
    {
        disconnected();
    }

    void on_close(websocketpp::connection_hdl /*hdl*/)
    {
        disconnected();
    }

    bool isOpen() const;

	virtual void connect(const std::string& uri, const std::string& subprotocol = "");
    void close();

    void on_message(connection_hdl hdl, ssl_client::message_ptr msg);
    void send(char* data, size_t size);

    // SSL
    context_ptr on_tls_init(websocketpp::connection_hdl);
    bool verify_subject_alternative_name(X509 * cert);
    bool verify_common_name(X509 * cert);
    bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);

private:
    std::string m_hostname = "rabbithole.rabbitcontrol.cc";

    client m_client;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    client::connection_ptr m_con;

    // ssl
    ssl_client m_sslClient;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_sslThread;
    ssl_client::connection_ptr m_sslCon;
};

}

#endif // RABBITCONTROL_WEBSOCKET_SERVER_TRANSPORTER_H
