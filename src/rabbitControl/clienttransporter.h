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

#ifndef CLIENTTRANSPORTER_H
#define CLIENTTRANSPORTER_H

#include <map>

namespace rcp {


    class ClientTransporterListener
    {
    public:
        virtual void connected() = 0;
        virtual void disconnected() = 0;
        virtual void received(std::istream& data) = 0;
    };


    class ClientTransporter
    {
    public:

        virtual void connect(std::string host, int port, bool secure = false) = 0;
        virtual void disconnect() = 0;
        virtual bool isConnected() = 0;

        virtual void send(std::istream& data) = 0;
        virtual void send(char* data, int size) = 0;

        void addConnectedCb(ClientTransporterListener* c, void(ClientTransporterListener::* func)()) {
            connected_cb[c] = func;
        }
        void removeConnectedCb(ClientTransporterListener* c) {
            connected_cb.erase(c);
        }

        void addDisconnectedCb(ClientTransporterListener* c, void(ClientTransporterListener::* func)()) {
            disconnected_cb[c] = func;
        }
        void removeDisconnectedCb(ClientTransporterListener* c) {
            disconnected_cb.erase(c);
        }

        void addReceivedCb(ClientTransporterListener* c, void(ClientTransporterListener::* func)(std::istream&)) {
            receive_cb[c] = func;
        }
        void removeReceivedCb(ClientTransporterListener* c) {
            receive_cb.erase(c);
        }

    protected:
        void _connected() {
            for (const auto& kv : connected_cb) {
                (kv.first->*kv.second)();
            }
        }
        void _disconnected() {
            for (const auto& kv : disconnected_cb) {
                (kv.first->*kv.second)();
            }
        }
        void _received(std::istream& in) {
            for (const auto& kv : receive_cb) {
                (kv.first->*kv.second)(in);
            }
        }


        std::map<ClientTransporterListener*, void(ClientTransporterListener::*)()> connected_cb;
        std::map<ClientTransporterListener*, void(ClientTransporterListener::*)()> disconnected_cb;
        std::map<ClientTransporterListener*, void(ClientTransporterListener::*)(std::istream&)> receive_cb;
    };

}

#endif // CLIENTTRANSPORTER_H
