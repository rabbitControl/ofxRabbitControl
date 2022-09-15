/*
********************************************************************
* rabbitholeWsServerTransporter cpp
*
* written by: Ingo Randolf - 2021
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

#include "rabbitholeWsServerTransporter.h"

#define DELAYED_CONNECT_SLEEP_TIME 50
#define DELAYED_CONNECT_SLEEP_TIME_TIMEOUT 2000

rabbitholeWsServerTransporter::rabbitholeWsServerTransporter(const std::string& uri)
    : rcp::websocketClient()
{
    setUri(uri);
}

rabbitholeWsServerTransporter::~rabbitholeWsServerTransporter()
{
    unbind();
}


void rabbitholeWsServerTransporter::setUri(const std::string& uri)
{
    m_uri = uri;

    if (m_uri.find("https", 0) == 0)
    {
        m_uri = m_uri.replace(0, 5, "wss");
    }
    else if (m_uri.find("http", 0) == 0)
    {
        m_uri = m_uri.replace(0, 4, "ws");
    }
}

void rabbitholeWsServerTransporter::tryConnect()
{
    startThread();
}

//----------------------------------------
// ofThread
void rabbitholeWsServerTransporter::threadedFunction()
{
    int slept = 0;
    while(slept < DELAYED_CONNECT_SLEEP_TIME_TIMEOUT)
    {
        sleep(DELAYED_CONNECT_SLEEP_TIME);
        if (!isThreadRunning())
        {
            return;
        }

        slept += DELAYED_CONNECT_SLEEP_TIME;
    }

    websocketClient::connect(m_uri);
}


//----------------------------------------
// webserverClient
void rabbitholeWsServerTransporter::connected()
{
}

void rabbitholeWsServerTransporter::disconnected()
{
    if (m_doTryConnect)
    {
        tryConnect();
    }
}

void rabbitholeWsServerTransporter::received(char* data, size_t size)
{
	std::istringstream input_stream(std::string(const_cast<char*>(data), size));

    // call receive callbacks
    for (const auto& kv : receive_cb) {
        (kv.first->*kv.second)(input_stream, *this, this);
    }
}

//----------------------------------------
// rcp::ServerTransporter
void rabbitholeWsServerTransporter::bind(int port)
{
    // todo - try connect - start timer
    m_doTryConnect = true;
    tryConnect();
}

void rabbitholeWsServerTransporter::unbind()
{
    m_doTryConnect = false;

    // stop timer
    stopThread();
    waitForThread();

    // close current connection
    close();
}

void rabbitholeWsServerTransporter::sendToOne(std::istream& data, void* id)
{
    data.seekg (0, data.end);
    size_t length = data.tellg();
    data.seekg (0, data.beg);

    char d[length];
    data.read(d, length);

    rcp::websocketClient::send(d, length);
}

void rabbitholeWsServerTransporter::sendToAll(std::istream& data, void* excludeId)
{
    sendToOne(data, nullptr);
}

int rabbitholeWsServerTransporter::getConnectionCount()
{
    return isOpen();
}
