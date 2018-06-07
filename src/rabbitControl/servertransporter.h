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

#ifndef SERVERTRANSPORTER_H
#define SERVERTRANSPORTER_H

#include <map>

namespace rcp {

    class ServerTransporter;

    class ServerTransporterReceiver
    {
    public:
        virtual void received(std::istream& data, ServerTransporter& transporter, void* id) = 0;
    };


    class ServerTransporter
    {
    public:
        virtual void bind(int port) = 0;
        virtual void unbind() = 0;

        virtual void sendToOne(std::istream& data, void* id) = 0;
        virtual void sendToAll(std::istream& data, void* excludeId) = 0;

        virtual int getConnectionCount() = 0;

        //
        void addReceivedCb(ServerTransporterReceiver* c, void(ServerTransporterReceiver::* func)(std::istream&, ServerTransporter&, void*)) {
            receive_cb[c] = func;
        }
        void removeReceivedCb(ServerTransporterReceiver* c) {
            receive_cb.erase(c);
        }

    protected:
        std::map<ServerTransporterReceiver*, void(ServerTransporterReceiver::*)(std::istream&, ServerTransporter&, void*)> receive_cb;
    };
}


#endif // SERVERTRANSPORTER_H
