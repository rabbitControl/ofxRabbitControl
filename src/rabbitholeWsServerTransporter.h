/*
********************************************************************
* ofxRabbitControl
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

#ifndef RABBITHOLEWSSERVERTRANSPORTER_H
#define RABBITHOLEWSSERVERTRANSPORTER_H

#include "rabbitControl/servertransporter.h"

#include "websocketClient.h"

#include <boost/interprocess/streams/bufferstream.hpp>

#include <ofLog.h>
#include <ofThread.h>

class rabbitholeWsServerTransporter
        : public rcp::ServerTransporter
        , public rcp::websocketClient
        , public ofThread
{
public:
    rabbitholeWsServerTransporter(const std::string& uri = "");
    ~rabbitholeWsServerTransporter();

    void setUri(const std::string& uri);

public:
    // ofThread
    void threadedFunction() override;

public:
    // webserverClient
    virtual void connected() override;
    virtual void disconnected() override;
	virtual void received(char* data, size_t size) override;

public:
    // rcp::ServerTransporter
    virtual void bind(int port) override;
    virtual void unbind() override;
    virtual void sendToOne(std::istream& data, void* id) override;
    virtual void sendToAll(std::istream& data, void* excludeId) override;
    virtual int getConnectionCount() override;

private:
    void tryConnect();

    std::string m_uri;
    std::atomic<bool> m_doTryConnect{false};

};

#endif // RABBITHOLEWSSERVERTRANSPORTER_H
