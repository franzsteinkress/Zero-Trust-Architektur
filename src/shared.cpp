#include "shared.hpp"
#include <filesystem>
#include <cstdlib>

// Hilfsfunktion zur Pfad-Auflösung
std::string get_cert_path(const std::string& filename) {
    // Falls CERT_PATH gesetzt ist (Docker), nutze diesen, sonst lokal bin/certs
    const char* env_p = std::getenv("CERT_PATH");
    std::filesystem::path base = env_p ? env_p : "./bin/certs";
    return (base / filename).string();
}
