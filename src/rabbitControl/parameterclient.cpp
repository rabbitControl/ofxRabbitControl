#include "parameterclient.h"

#include "stringstreamwriter.h"
#include "streamwriter.h"

namespace rcp {

    ParameterClient::ParameterClient(ClientTransporter& transporter)
        : m_parameterManager(std::make_shared<rcp::ParameterManager>())
        , m_transporter(transporter)
    {
        transporter.addConnectedCb(this, &ClientTransporterListener::connected);
        transporter.addDisconnectedCb(this, &ClientTransporterListener::disconnected);
        transporter.addReceivedCb(this, &ClientTransporterListener::received);
    }

    ParameterClient::~ParameterClient() {
        dispose();
    }


    void ParameterClient::dispose() {
        disconnect();
    }

    void ParameterClient::connect(const std::string& host, const int port) {
        m_transporter.connect(host, port);
    }

    void ParameterClient::disconnect() {
        m_transporter.disconnect();
    }

    void ParameterClient::initialize() {

        char data[2];
        data[0] = 0x02;
        data[1] = 0x00;
        m_transporter.send(data);
    }

    // sending all dirty parameter to server!
    void ParameterClient::update() {

        if (!m_transporter.isConnected()) {
            return;
        }

        // send updates
        for (auto& p : m_parameterManager->dirtyParameter) {

            Packet packet(COMMAND_UPDATE, p.second);

            // serialize
            StringStreamWriter writer;
            packet.write(writer, false);

            std::istream& data = writer.getBuffer();

            data.seekg (0, data.end);
            long long length = data.tellg();
            data.seekg (0, data.beg);

            char* d = new char[length];
            data.read(d, length);

            m_transporter.send(d);
            delete []d;
        }
        m_parameterManager->dirtyParameter.clear();

    }

    // interface ClientTransporterListener
    void ParameterClient::connected() {

        // request version
        char data[2];
        data[0] = 0x01;
        data[1] = 0x00;
        m_transporter.send(data);

        // send initialize command
        initialize();
    }

    void ParameterClient::disconnected() {

        // TODO: erase all parameter!
    }

    void ParameterClient::received(std::istream& data) {

        auto packet = rcp::Packet::parse(data);
        if (packet.hasValue()) {

            rcp::Packet& the_packet = packet.getValue();
            switch (the_packet.getCommand()) {

            case COMMAND_INITIALIZE:
                // error
                std::cerr << "got initialize command: ???\n";
                break;

            case COMMAND_UPDATE:
                _update(the_packet);
                break;

            case COMMAND_INFO:
                std::cerr << "got version command: ???\n";
                break;

            case COMMAND_UPDATEVALUE:
                std::cerr << "COMMAND_UPDATEVALUE not implemented!\n";
                break;

            case COMMAND_DISCOVER:
                // not implemented
                std::cerr << "command not implemented!\n";
                break;

            case COMMAND_REMOVE:
                _remove(the_packet);
                break;

            case COMMAND_INVALID:
            case COMMAND_MAX_:
                std::cerr << "got invalid command!\n";
                break;
            }
        }
    }


    // private functions
    // receiving dirty parameter from server!
    void ParameterClient::_update(rcp::Packet& packet) {

        if (!packet.hasData()) {
            std::cerr << "_update: packet has no data\n";
            return;
        }

        // assume this is a parameter!!
        rcp::ParameterPtr param = std::dynamic_pointer_cast<rcp::IParameter>(packet.getData());
        if (param) {

            std::cout << "update for param: " << param->getId() << "\n";

            rcp::ParameterPtr chached_param = m_parameterManager->getParameter(param->getId());
            if (rcp::ParameterManager::isValid(chached_param)) {
                // got it... update it
                std::cout << "update exisiting parameter: " << param->getId() << "\n";
                chached_param->update(param);
            } else {
                std::cout << "todo: add parameter: " << param->getId() << "\n";
                std::flush(std::cout);

                // parameter not in cache, add it
                m_parameterManager->_addParameter(param);

                // call disconnected callbacks
                for (const auto& kv : parameter_added_cb) {
                    (kv.first->*kv.second)(param);
                }
            }
            return;

        } else {
            std::cerr << "data not a parameter\n";
        }

    }

    void ParameterClient::_remove(Packet& packet) {

        if (!packet.hasData()) {
            std::cerr << "_remove: packet has no data\n";
            return;
        }

        // assume this is id-data
        IdDataPtr id_data = std::dynamic_pointer_cast<IdData>(packet.getData());
        if (id_data) {

            std::cout << "remove param: " << id_data->getId() << "\n";

            rcp::ParameterPtr chached_param = m_parameterManager->getParameter(id_data->getId());
            if (rcp::ParameterManager::isValid(chached_param)) {

                // parameter is in list, remove it
                std::cout << "removing exisiting parameter: " << id_data->getId() << "\n";

                // call disconnected callbacks
                for (const auto& kv : parameter_removed_cb) {
                    (kv.first->*kv.second)(chached_param);
                }

                // remove it (direct)
                m_parameterManager->removeParameterDirect(chached_param);

            } else {
                std::cout << "parameter not in list!: " << id_data->getId() << "\n";
            }
            return;

        } else {
            std::cerr << "data not a parameter\n";
        }
    }


}

