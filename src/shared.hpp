#ifndef SHARED_HPP
#define SHARED_HPP

//#pragma once
#include <utility>
#include <boost/asio.hpp>
#include <cstdint>
#include <iostream>
#include <string>
#include <format>
#include <chrono>
#include <string_view>
#include <cstdint>

// Deklarationen der Coroutines
boost::asio::awaitable<void> listener();
boost::asio::awaitable<void> run_mtls_client(std::string target_host);

// Deklaration: Nur der "Fingerabdruck" der Funktion
std::string get_cert_path(const std::string& filename);

// Ein kompaktes Datenpaket für Sensordaten
struct SensorPayload {
    uint32_t message_id;
    float temperature;
    uint64_t timestamp;
};

/*
inline void log_msg(const std::string& component, const std::string& msg) {
    std::cout << "[" << component << "] " << msg << std::endl;
}
*/

/*
inline void log_msg(const std::string& component, const std::string& level, const std::string& msg) {
    // Format: [COMPONENT] [LEVEL] Message
    std::cout << std::format("[{:<6}] [{:<5}] {}", component, level, msg) << std::endl;
}
*/

inline void log_msg(const std::string& component, const std::string& level, const std::string& msg) {
    // Filtert Nachrichten, die nur aus Steuerzeichen oder Leerzeichen bestehen
    if (msg.empty() || msg.find_first_not_of(" \t\n\r") == std::string::npos) {
        return;
    }
/*
    // Kopiert die Nachricht und ersetzen Umbrüche
    std::string sanitized = msg;
    for (char& c : sanitized) {
        if (c == '\n' || c == '\r' || c == '\t') {
            c = ' '; // Ersetze Steuerzeichen durch ein Leerzeichen
        }
    }
*/
    // Präziser C++20 Zeitstempel
    auto now = std::chrono::system_clock::now();
    
    // Formatierung: [HH:MM:SS] [COMP] [LEVEL] Message
    // %T liefert HH:MM:SS
    std::cout << std::format("[{:%T}] [{:<7}] [{:<5}] {}", now, component, level, msg) << std::endl;
}
#endif