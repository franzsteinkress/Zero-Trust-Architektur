cmake_minimum_required(VERSION 3.20)
project(ZeroTrustApp LANGUAGES CXX)

# Dieser Block unterdrückt die Warnung unter Windows,
# ohne den Docker-Build unter Linux zu gefährden.
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

# 1. Abhängigkeiten (Thread hinzugefügt für Linux-Stabilität)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

# 2. Quellen
file(GLOB SOURCES "src/*.cpp")

# 3. Ausgabe-Ordner (Damit deine .exe wieder in /bin landet)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 4. Das Programm erstellen
add_executable(mtls_app ${SOURCES})

# 5. System-spezifische Konfiguration
if (WIN32)
    # Erforderlich für Netzwerk-Schnittstellen unter Windows
    target_compile_definitions(mtls_app PRIVATE _WIN32_WINNT=0x0A00)
    target_link_libraries(mtls_app PRIVATE ws2_32 mswsock)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(mtls_app PRIVATE -Wall -Wextra -pedantic)
endif()

# 6. Bibliotheken verknüpfen
target_link_libraries(mtls_app 
    PRIVATE 
    Boost::system 
    Boost::thread
    OpenSSL::SSL 
    OpenSSL::Crypto
)

# 7. Include-Pfade
target_include_directories(mtls_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
install(TARGETS mtls_app DESTINATION bin)

# --- 8. Zertifikats-Generierung ---
find_program(SHELL_PATH NAMES bash sh REQUIRED)

add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/bin/certs/ca.pem"
           "${CMAKE_SOURCE_DIR}/bin/certs/server_cert.pem"
           "${CMAKE_SOURCE_DIR}/bin/certs/client_cert.pem"
    COMMAND ${CMAKE_COMMAND} -E env MSYS_NO_PATHCONV=1 
            ${SHELL_PATH} "${CMAKE_SOURCE_DIR}/generate_san_certs.sh"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Erzeuge mTLS-Zertifikate (SAN) für Windows/Docker..."
    VERBATIM
)

# Target, das die Zertifikate erzwingt
add_custom_target(generate_san_certs DEPENDS "${CMAKE_SOURCE_DIR}/bin/certs/ca.pem")
add_dependencies(mtls_app generate_san_certs)

# Sicherstellen, dass die Zertifikate beim Installieren mitgenommen werden
install(DIRECTORY "${CMAKE_SOURCE_DIR}/bin/certs/" DESTINATION bin/certs)