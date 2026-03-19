/**
 * @file mtls-server.cpp
 * @brief Zero-Trust Server-Logik mit gegenseitiger Zertifikatsprüfung.
 */
#include <utility>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <iostream>
#include <string>
#include "shared.hpp"

namespace asio = boost::asio;
namespace ssl  = boost::asio::ssl;
using tcp      = asio::ip::tcp;

/**
 * @brief Verwaltet eine einzelne verschlüsselte Client-Sitzung.
 * * Implementiert die strikte Identitätsprüfung gemäß mTLS-Standard.
 * @param stream Der aktive SSL/TLS-Datenstrom zum Client.
 * @return asio::awaitable<void>
 */
asio::awaitable<void> handle_session(ssl::stream<tcp::socket> stream) {
    try {
        // 1. Handshake durchführen
        co_await stream.async_handshake(ssl::stream_base::server, asio::use_awaitable);
        log_msg("SERVER", "INFO", "mTLS Handshake erfolgreich!");

        // 2. Client-Zertifikat extrahieren
        X509* cert = SSL_get_peer_certificate(stream.native_handle());
        if (cert) {
            char name[256];
            // Den "Subject Name" (Identität) auslesen
            X509_NAME_oneline(X509_get_subject_name(cert), name, sizeof(name));
            log_msg("SERVER", "INFO", std::format("VERIFIZIERT: Client-Identität: {} ", name));
            X509_free(cert); // Wichtig: Speicher freigeben!
        } else {
            log_msg("SERVER", "INFO", "WARNUNG: Kein Client-Zertifikat erhalten!");
        }

        // 3. Daten lesen (Request des Clients)
        char text_data[1024];
        size_t n = co_await stream.async_read_some(asio::buffer(text_data), asio::use_awaitable);
        log_msg("SERVER", "INFO", std::format("Anfrage erhalten: {}", std::string_view(text_data, n)));

        // 4. Binäre Sensor-Payload gezielt empfangen
        SensorPayload incoming_sensor_data;
        co_await asio::async_read(stream, asio::buffer(&incoming_sensor_data, sizeof(incoming_sensor_data)), asio::use_awaitable);
        log_msg("SERVER", "INFO", std::format("BINÄRE DATEN VERIFIZIERT: [ID: {} | Temp: {}°C]", incoming_sensor_data.message_id, incoming_sensor_data.temperature));

        // 5. Antwort senden
        std::string response = "HTTP/1.1 200 OK\r\n\r\nIdentität bestätigt. Zugriff gewährt.";
        co_await asio::async_write(stream, asio::buffer(response), asio::use_awaitable);

    } catch (std::exception& e) {
        log_msg("SERVER", "ERROR", std::format("Server Session-Fehler: {}", e.what()));
    }
}

/**
 * @brief Akzeptiert eingehende TCP-Verbindungen und initiiert die Sessions.
 * @return asio::awaitable<void>
 */
asio::awaitable<void> listener() {
    auto executor = co_await asio::this_coro::executor;
    
    // 1. SSL Kontext für mTLS (Server-Perspektive)
    ssl::context ctx(ssl::context::tls_server);
    ctx.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 | ssl::context::single_dh_use);
    
    ctx.use_certificate_chain_file(get_cert_path("server_cert.pem"));
    ctx.use_private_key_file(get_cert_path("server_key.pem"), ssl::context::pem);

    // mTLS: Client-Zertifikat erzwingen
    ctx.load_verify_file(get_cert_path("ca.pem")); // CA, die Client-Zertifikate signiert hat
    ctx.set_verify_mode(ssl::verify_peer | ssl::verify_fail_if_no_peer_cert);

    // 2. Acceptor-Loop
    tcp::acceptor acceptor(executor, {tcp::v4(), 8443});

    log_msg("SERVER", "INFO", "Server gestartet auf Port 8443. Warte auf Verbindungen...");

    for (;;) {
        auto socket = co_await acceptor.async_accept(asio::use_awaitable);
        // Wir fügen ein kurzes Log hinzu, um zu sehen, wenn jemand anklopft
        log_msg("SERVER", "INFO", std::format("Neue Verbindung von: {}:{}", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port()));
        asio::co_spawn(executor, handle_session(ssl::stream<tcp::socket>(std::move(socket), ctx)), asio::detached);
    }
}
