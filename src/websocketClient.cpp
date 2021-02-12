/*
********************************************************************
* websocketClient cpp
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

#include "websocketClient.h"

// make sure these are built
#include <boost/asio/ssl/impl/error.hpp>
#include <boost/asio/ssl/impl/contexti.hpp>
#include <boost/asio/ssl/detail/impl/engine.hpp>
#include <boost/asio/ssl/detail/impl/openssl_init.hpp>

namespace rcp
{

websocketClient::websocketClient()
{
    // Set logging to be pretty verbose (everything except message payloads)
    m_client.clear_access_channels(websocketpp::log::alevel::all);
//        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    m_sslClient.clear_access_channels(websocketpp::log::alevel::all);
//        m_sslClient.clear_access_channels(websocketpp::log::alevel::frame_payload);
    m_sslClient.set_tls_init_handler(bind(&websocketClient::on_tls_init, this, ::_1));

    // Initialize Asio Transport

    m_client.init_asio();
    m_client.start_perpetual();
    m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_client);

    m_sslClient.init_asio();
    m_sslClient.start_perpetual();
    m_sslThread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&ssl_client::run, &m_sslClient);
}

websocketClient::~websocketClient()
	{
        m_sslClient.stop_perpetual();
        m_client.stop_perpetual();

        close();

        m_sslThread->join();
        m_thread->join();
	}


void websocketClient::close()
{
    if (m_sslCon)
    {
        m_sslCon->set_open_handler(nullptr);
        m_sslCon->set_fail_handler(nullptr);
        m_sslCon->set_close_handler(nullptr);
        m_sslCon->set_message_handler(nullptr);

        if (m_sslCon->get_state() == websocketpp::session::state::open)
        {
            websocketpp::lib::error_code ec;
            m_sslCon->close(websocketpp::close::status::normal, "close", ec);
            if (ec) {
                std::cout << "closing failed: " << ec.message() << std::endl << std::endl;
            }
        }

        m_sslCon.reset();
    }

    if (m_con)
    {
        m_con->set_open_handler(nullptr);
        m_con->set_fail_handler(nullptr);
        m_con->set_close_handler(nullptr);
        m_con->set_message_handler(nullptr);

        if (m_con->get_state() == websocketpp::session::state::open)
        {
            websocketpp::lib::error_code ec;
            m_con->close(websocketpp::close::status::normal, "close", ec);
            if (ec) {
                std::cout << "closing failed: " << ec.message() << std::endl << std::endl;
            }
        }

        m_con.reset();
    }
}

void websocketClient::connect(const std::string& uri, const std::string& subprotocol)
{

    // close first?
    close();

    if (uri.empty())
    {
        return;
    }


    websocketpp::lib::error_code ec;
    if (uri.find("wss", 0) == 0)
    {
        m_sslCon = m_sslClient.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection: " << ec.message() << std::endl;
            m_sslCon.reset();
            return;
        }

        m_sslCon->set_open_handler(bind(&websocketClient::on_open, this, ::_1));
        m_sslCon->set_fail_handler(bind(&websocketClient::on_fail, this, ::_1));
        m_sslCon->set_close_handler(bind(&websocketClient::on_close, this, ::_1));
        m_sslCon->set_message_handler(bind(&websocketClient::on_message, this, ::_1, ::_2));

        if (!subprotocol.empty())
        {
            m_sslCon->add_subprotocol(subprotocol);
        }

        try {
            m_sslClient.connect(m_sslCon);
        }
        catch (const std::exception & e) {
            std::cout << "connect error: " << e.what() << std::endl;
        }
    }
    else
    {
        m_con = m_client.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection: " << ec.message() << std::endl;
            m_con.reset();
            return;
        }

        m_con->set_open_handler(bind(&websocketClient::on_open, this, ::_1));
        m_con->set_fail_handler(bind(&websocketClient::on_fail, this, ::_1));
        m_con->set_close_handler(bind(&websocketClient::on_close, this, ::_1));
        m_con->set_message_handler(bind(&websocketClient::on_message, this, ::_1, ::_2));

        if (!subprotocol.empty())
        {
            m_con->add_subprotocol(subprotocol);
        }

        try {
            m_client.connect(m_con);
        }
        catch (const std::exception & e) {
            std::cout << "connect error: " << e.what() << std::endl;
        }
    }
}

bool websocketClient::isOpen() const
{
    if (m_sslCon)
    {
        return m_sslCon->get_state() == websocketpp::session::state::open;
    }

    if (m_con)
    {
        return m_con->get_state() == websocketpp::session::state::open;
    }

    return false;
}


void websocketClient::on_message(connection_hdl hdl, ssl_client::message_ptr msg)
{
    {
        if (msg->get_opcode() == websocketpp::frame::opcode::value::binary)
        {
            const std::string & data = msg->get_raw_payload();
            received(const_cast<char*>(data.data()), data.size());
        }
        else
        {
            // got text message
            std::cout << "websocket: got text message: " << msg->get_payload() << std::endl;
        }
    }
}

void websocketClient::send(char* data, size_t size)
{
    if (m_sslCon)
    {
        websocketpp::lib::error_code ec;
        m_sslClient.send(m_sslCon->get_handle(), data, size, websocketpp::frame::opcode::binary, ec);
        if (ec) {
            std::cout << "sending failed: " << ec.message() << std::endl << std::endl;
        }
    }

    if (m_con)
    {
        websocketpp::lib::error_code ec;
        m_client.send(m_con->get_handle(), data, size, websocketpp::frame::opcode::binary, ec);
        if (ec) {
            std::cout << "sending failed: " << ec.message() << std::endl << std::endl;
        }
    }
}

// ssl

context_ptr websocketClient::on_tls_init(websocketpp::connection_hdl)
{
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try
    {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);


        ctx->set_verify_mode(boost::asio::ssl::verify_none);

//            ctx->set_verify_mode(boost::asio::ssl::verify_peer);
//            ctx->set_verify_callback(bind(&websocketClient::verify_certificate, this, ::_1, ::_2));

        // Here we load the CA certificates of all CA's that this client trusts.
//            ctx->load_verify_file("ca-chain.cert.pem");

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return ctx;
}

/// Verify that one of the subject alternative names matches the given hostname
bool websocketClient::verify_subject_alternative_name(X509 * cert)
{
    STACK_OF(GENERAL_NAME) * san_names = NULL;

    san_names = (STACK_OF(GENERAL_NAME) *) X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);
    if (san_names == NULL) {
        return false;
    }

    int san_names_count = sk_GENERAL_NAME_num(san_names);

    bool result = false;

    for (int i = 0; i < san_names_count; i++) {
        const GENERAL_NAME * current_name = sk_GENERAL_NAME_value(san_names, i);

        if (current_name->type != GEN_DNS) {
            continue;
        }

        char const * dns_name = (char const *) ASN1_STRING_get0_data(current_name->d.dNSName);

        // Make sure there isn't an embedded NUL character in the DNS name
        if (ASN1_STRING_length(current_name->d.dNSName) != strlen(dns_name)) {
            break;
        }
        // Compare expected hostname with the CN
        std::cout << "dns_name: " << dns_name << std::endl;
        result = (strcasecmp(m_hostname.c_str(), dns_name) == 0);
    }
    sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);

    return result;
}

/// Verify that the certificate common name matches the given hostname
bool websocketClient::verify_common_name(X509 * cert)
{
    // Find the position of the CN field in the Subject field of the certificate
    int common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name(cert), NID_commonName, -1);
    if (common_name_loc < 0) {
        return false;
    }

    // Extract the CN field
    X509_NAME_ENTRY * common_name_entry = X509_NAME_get_entry(X509_get_subject_name(cert), common_name_loc);
    if (common_name_entry == NULL) {
        return false;
    }

    // Convert the CN field to a C string
    ASN1_STRING * common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
    if (common_name_asn1 == NULL) {
        return false;
    }

    char const * common_name_str = (char const *) ASN1_STRING_get0_data(common_name_asn1);

    // Make sure there isn't an embedded NUL character in the CN
    if (ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
        return false;
    }

    // Compare expected hostname with the CN
    std::cout << "common_name_str: " << common_name_str << std::endl;
    return (strcasecmp(m_hostname.c_str(), common_name_str) == 0);
}

bool websocketClient::verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // Retrieve the depth of the current cert in the chain. 0 indicates the
    // actual server cert, upon which we will perform extra validation
    // (specifically, ensuring that the hostname matches. For other certs we
    // will use the 'preverified' flag from Asio, which incorporates a number of
    // non-implementation specific OpenSSL checking, such as the formatting of
    // certs and the trusted status based on the CA certs we imported earlier.
    int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());

    // if we are on the final cert and everything else checks out, ensure that
    // the hostname is present on the list of SANs or the common name (CN).
    if (depth == 0 && preverified) {
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

        if (verify_subject_alternative_name(cert)) {
            return true;
        } else if (verify_common_name(cert)) {
            return true;
        } else {
            std::cout << "NOOOO: " << std::endl;
            return false;
        }
    }

    std::cout << "preverified: " << preverified << std::endl;

    return preverified;
}

}
