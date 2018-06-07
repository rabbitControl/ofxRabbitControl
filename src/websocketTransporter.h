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

#ifndef OFXRABBITCONTROL_WEBSOCKET_TRANSPORTER_H
#define OFXRABBITCONTROL_WEBSOCKET_TRANSPORTER_H

#include "ofxLibwebsockets.h"
#include "rabbitControl/servertransporter.h"

class websocketServerTransporter : public rcp::ServerTransporter
{
public:
    websocketServerTransporter() {}
    ~websocketServerTransporter() {
        if (connected) {
            unbind();
        }
    }

    //----------------------------------------
    // transporter interface
    virtual void bind(int port) {
        if (connected) {
            unbind();
        }

        ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
        options.port = port;
        options.bUseSSL = false;
//        options.protocol = "rcp";

        server.setWaitMillis(5);

        connected = server.setup( options );

        // this adds your app as a listener for the server
        server.addListener(this);
    }
    virtual void unbind() {
        server.close();
        connected = false;
        server.removeListener(this);
    }

    virtual void sendToOne(std::istream& data, void* id) {

        if (!connected) {
            return;
        }

        if (id == nullptr) {
            return;
        }

        data.seekg (0, data.end);
        int length = data.tellg();
        data.seekg (0, data.beg);

        char d[length];
        data.read(d, length);

        for (ofxLibwebsockets::Connection* conn : server.getConnections()) {
            if (id == conn) {
                conn->sendBinary(d, length);
            }
        }
    }

    virtual void sendToAll(std::istream& data, void* excludeId) {

        if (!connected) {
            return;
        }

        data.seekg (0, data.end);
        int length = data.tellg();
        data.seekg (0, data.beg);

        char d[length];
        data.read(d, length);

        for (ofxLibwebsockets::Connection* conn : server.getConnections()) {
            if (excludeId == conn) {
                continue;
            }
            conn->sendBinary(d, length);
        }
    }

    virtual int getConnectionCount() {
        return server.getConnections().size();
    }

    // websocket methods
    void onConnect( ofxLibwebsockets::Event& args );
    void onOpen( ofxLibwebsockets::Event& args );
    void onClose( ofxLibwebsockets::Event& args );
    void onIdle( ofxLibwebsockets::Event& args );
    void onMessage( ofxLibwebsockets::Event& args );
    void onBroadcast( ofxLibwebsockets::Event& args );

private:

    ofxLibwebsockets::Server server;
    bool connected;
};

#endif // OFXRABBITCONTROL_WEBSOCKET_TRANSPORTER_H
