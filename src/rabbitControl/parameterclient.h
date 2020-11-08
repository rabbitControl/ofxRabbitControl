#ifndef PARAMETERCLIENT_H
#define PARAMETERCLIENT_H

#include "packet.h"
#include "clienttransporter.h"
#include "parametermanager.h"
#include "rcp_error_listener.h"

namespace rcp {

    class ParameterClientListener
    {
    public:
        virtual void parameterAdded(ParameterPtr parameter) = 0;
        virtual void parameterRemoved(ParameterPtr parameter) = 0;
    };


    class ParameterClient : public ClientTransporterListener
    {
    public:
        ParameterClient(ClientTransporter& transporter);
        virtual ~ParameterClient();

        void dispose();
        void connect(const std::string& host, const int port);
        void disconnect();

        void initialize(); // tries to send an init-command
        void update(); // update all changes

        // connect to events
        void addParameterAddedCb(ParameterClientListener* c, void(ParameterClientListener::* func)(ParameterPtr parameter)) {
            parameter_added_cb[c] = func;
        }
        void removeParameterAddedCb(ParameterClientListener* c) {
            parameter_added_cb.erase(c);
        }

        void addParameterRemovedCb(ParameterClientListener* c, void(ParameterClientListener::* func)(ParameterPtr parameter)) {
            parameter_removed_cb[c] = func;
        }
        void removeParameterRemovedCb(ParameterClientListener* c) {
            parameter_removed_cb.erase(c);
        }

        void addParsingErrorCb(ParsingErrorListener* c, void(ParsingErrorListener::* func)()) {
            parsing_error_cb[c] = func;
        }
        void removeParsingErrorCb(ParsingErrorListener* c) {
            parsing_error_cb.erase(c);
        }


        // interface ClientTransporterListener
        virtual void connected();
        virtual void disconnected();
        virtual void received(std::istream& data);

        void setApplicationId(const std::string& appid) {
            m_applicationId = appid;
        }
        std::string getApplicationId() {
            return m_applicationId;
        }

    private:
        void _update(Packet& packet);
        void _remove(Packet& packet);
        void _version(Packet& packet);

        std::shared_ptr<ParameterManager> m_parameterManager;
        ClientTransporter& m_transporter;

        // Events:
        std::map<ParameterClientListener*, void(ParameterClientListener::*)(ParameterPtr parameter)> parameter_added_cb;
        std::map<ParameterClientListener*, void(ParameterClientListener::*)(ParameterPtr parameter)> parameter_removed_cb;
        std::map<ParsingErrorListener*, void(ParsingErrorListener::*)()> parsing_error_cb;
//        onError(Exception ex);
//        statusChanged(Status status, String message);

        std::string m_applicationId;
    };

}


#endif // PARAMETERCLIENT_H
