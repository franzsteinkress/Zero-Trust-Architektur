#include <utility>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "shared.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @brief Einstiegspunkt des Programms.
 * * Initialisiert den Boost.Asio I/O-Kontext und startet den Coroutine-Scheduler.
 */
int main(int argc, char* argv[]) {
    #ifdef _WIN32
    // Setzt die Konsole auf UTF-8 (Code Page 65001)
    SetConsoleOutputCP(CP_UTF8);
    // Optional: Auch die Eingabe auf UTF-8 setzen
    SetConsoleCP(CP_UTF8);
    #endif

    if (argc < 2) {
        log_msg("SYSTEM", "ERROR", std::format("Usage: mtls_app <server|client> [target_host]\n"));
        return 1;
    }

    boost::asio::io_context ioc;
    std::string mode = argv[1];

    if (mode == "server") {
        boost::asio::co_spawn(ioc, listener(), boost::asio::detached);
    } else if (mode == "client" && argc >= 3) {
        log_msg("SYSTEM", "INFO", std::format("Starte Client-Verbindung zu: {}", argv[2])); // Debug-Zeile
        boost::asio::co_spawn(ioc, run_mtls_client(argv[2]), boost::asio::detached);
    } else {
        log_msg("SYSTEM", "ERROR", std::format("Fehler: Client benötigt einen Ziel-Host (z.B. localhost)"));
        return 1;
    }

    ioc.run();
    return 0;
}