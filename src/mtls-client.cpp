#include <utility>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include "shared.hpp"

namespace asio = boost::asio;
namespace ssl  = boost::asio::ssl;
using tcp      = asio::ip::tcp;

asio::awaitable<void> run_mtls_client(std::string target_host) {
    auto executor = co_await asio::this_coro::executor;
    
    try {
        // 1. SSL Kontext für mTLS konfigurieren
        ssl::context ctx(ssl::context::tls_client);
        ctx.set_verify_mode(ssl::verify_peer);
        ctx.load_verify_file(get_cert_path("ca.pem"));
        ctx.use_certificate_chain_file(get_cert_path("client_cert.pem"));
        ctx.use_private_key_file(get_cert_path("client_key.pem"), ssl::context::pem);

        // 2. Verbindung aufbauen
        ssl::stream<tcp::socket> stream(executor, ctx);
        tcp::resolver resolver(executor);
        log_msg("CLIENT", "INFO", std::format("Verbinde zu {} auf Port 8443...", target_host));
        auto endpoints = co_await resolver.async_resolve(target_host, "8443", asio::use_awaitable);
        co_await asio::async_connect(stream.next_layer(), endpoints, asio::use_awaitable);

        // 3. TLS Handshake
        log_msg("CLIENT", "INFO", "Starte mTLS Handshake...");
        co_await stream.async_handshake(ssl::stream_base::client, asio::use_awaitable);
        log_msg("CLIENT", "INFO", "ERFOLG: Handshake abgeschlossen. Verbindung ist sicher.");

        // 4. Sicherer Datenaustausch
        // 4.1 Text-Anfrage senden (wie bisher)
        std::string request = "POST /sensor_update HTTP/1.1\r\n"
                                "Host: " + target_host + "\r\n"
                                "Content-Type: application/octet-stream\r\n"
                                "Connection: close\r\n\r\n";
        co_await asio::async_write(stream, asio::buffer(request), asio::use_awaitable);

        // 4.2 Binäre Sensor-Payload senden (Automotive-Demo)
        SensorPayload sensor_data{ 42, 22.5f, static_cast<uint64_t>(std::time(nullptr)) };
        co_await asio::async_write(stream, asio::buffer(&sensor_data, sizeof(sensor_data)), asio::use_awaitable);
        log_msg("CLIENT", "INFO", "HTTP-Anfrage und binäre Sensor-Payload gesendet.");

        // 5. Antwort empfangen
        char data[1024];
        size_t n = co_await stream.async_read_some(asio::buffer(data), asio::use_awaitable);
        log_msg("CLIENT", "INFO", std::format("--- Server Antwort Start ---"));
        log_msg("CLIENT", "INFO", std::format("{}", std::string_view(data, n)));
        log_msg("CLIENT", "INFO", std::format("--- Server Antwort Ende  ---"));    
    } catch (const std::exception& e) {
        log_msg("CLIENT", "ERROR", std::format("CLIENT-FEHLER: {}", e.what()));
    }
}
