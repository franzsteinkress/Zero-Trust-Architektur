/**
 * @file shared.hpp
 * @brief Gemeinsame Ressourcen, Datenstrukturen und Hilfsfunktionen für das PKI-System.
 */
#ifndef SHARED_HPP
#define SHARED_HPP

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

/**
 * @brief Ermittelt den absoluten Pfad zu den X.509-Zertifikaten.
 * @param filename Name der Zertifikatsdatei (z.B. "ca.pem").
 * @return std::string Der validierte Dateipfad.
 */
std::string get_cert_path(const std::string& filename);

/**
 * @brief Datenstruktur für den sicheren Sensordatenaustausch.
 * * Dieses Paket wird im mTLS-Tunnel übertragen und entspricht den Integritätsanforderungen des DSA.
 */
struct SensorPayload {
    uint32_t message_id;
    float temperature;
    uint64_t timestamp;
};

/**
 * @brief Thread-sichere Logging-Funktion mit Zeitstempel-Generierung.
 * * Erzeugt die Log-Einträge, die später vom sort_logs.ps1 Skript verarbeitet werden.
 * @param component Der Name des Dienstes (zt_client/zt_server).
 * @param level Log-Level (INFO, ERROR, WARN).
 * @param msg Die zu loggende Nachricht.
 */
inline void log_msg(const std::string& component, const std::string& level, const std::string& msg) {
    // Filtert Nachrichten, die nur aus Steuerzeichen oder Leerzeichen bestehen
    if (msg.empty() || msg.find_first_not_of(" \t\n\r") == std::string::npos) {
        return;
    }

    // Präziser C++20 Zeitstempel
    auto now = std::chrono::system_clock::now();
    
    // Formatierung: [HH:MM:SS] [COMP] [LEVEL] Message
    // %T liefert HH:MM:SS
    std::cout << std::format("[{:%T}] [{:<7}] [{:<5}] {}", now, component, level, msg) << std::endl;
}
#endif