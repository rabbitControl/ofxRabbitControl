/*
********************************************************************
* ofxRabbitControl
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

#include <boost/interprocess/streams/bufferstream.hpp>

#include "ofLog.h"

#include "websockettransporter.h"
#include "ofxrabbitcontrol.h"

#include "rabbitControl/option.h"
#include "rabbitControl/packet.h"


void websocketServerTransporter::onConnect( ofxLibwebsockets::Event& args ) {
}

void websocketServerTransporter::onOpen( ofxLibwebsockets::Event& args ) {
    ofLogNotice() <<"client connected: "<< args.conn.getClientIP();
}

void websocketServerTransporter::onClose( ofxLibwebsockets::Event& args ) {
    ofLogNotice() << "closed: " << args.conn.getClientIP();
}

void websocketServerTransporter::onIdle( ofxLibwebsockets::Event& args ) {
}

void websocketServerTransporter::onMessage( ofxLibwebsockets::Event& args ) {

    if (args.isBinary) {

        boost::interprocess::bufferstream input_stream(args.data.getData(), args.data.size());

        for (const auto& kv : receive_cb) {
            (kv.first->*kv.second)(input_stream, *this, &args.conn);
        }

    } else {
        ofLogNotice() << "got message " << args.message;
    }

    std::flush(std::cout);
}

void websocketServerTransporter::onBroadcast( ofxLibwebsockets::Event& args ) {
    ofLogNotice() << "got broadcast " << args.message;
}

