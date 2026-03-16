# 🛡️ Zero-Trust-Architektur: mTLS C++ Microservices

Dieses Projekt demonstriert eine hochsichere **Client-Server-Architektur** auf Basis von **Zero-Trust-Prinzipien**. Die Kommunikation erfolgt über gegenseitig authentifiziertes TLS (mTLS).

## 🚀 Features

* **Mutual TLS (mTLS):** Beidseitige Zertifikatsprüfung (Server prüft Client, Client prüft Server).
* **C++20 Standard:** Nutzung von `std::format`, `std::chrono` und moderner Speicherverwaltung.
* **Zero-Trust-Prinzip:** Kein Vertrauen ohne explizite Identitätsbestätigung.
* **Docker-Native:** Vollständig isolierte Umgebung für reproduzierbare Tests.

---

## 🛠️ Voraussetzungen

Das Projekt ist für die Nutzung mit **VS Code DevContainers** optimiert, was die Installation von Abhängigkeiten (OpenSSL, Boost, CMake) auf deinem lokalen System überflüssig macht.

* **Windows:** Docker Desktop + WSL2.
* **Linux/Mac:** Docker + Docker Compose.
* **IDE:** VS Code mit der Extension *Dev Containers*.

---

## 🏗️ Tech-Stack & Spezifikationen

* **Architektur:** Microservices (C++20)
* **Betriebssystem:** Debian Trixie (Slim Image) / Windows (via MSYS2 UCRT64)
* **Compiler:** GCC 15.2.0 (UCRT64 unter Windows)
* **Bibliotheken:**
* **Boost.Asio:** Für asynchrone Netzwerkkommunikation.
* **OpenSSL:** Für die kryptografische Absicherung und Zertifikatsverwaltung.


* **Protokoll:** mTLS über TCP/IP auf **Port 8443**.

---

## 🔐 Sicherheits-Infrastruktur (Zertifikate)

Die Identitätsprüfung basiert auf einer internen Root-CA. Zertifikate werden mit `Subject Alternative Names` (SAN) generiert.

* **Generierung:** `./generate_san_certs.sh` (Erstellt CA, Server- & Client-Keys).
* **Struktur:** Alle Zertifikate befinden sich in `./bin/certs/`.

---

## 🚀 Startanleitung

### Option A: Docker Compose (VS Code DevContainer)

1. Klone das Repository: `git clone https://github.com/franzsteinkress/Zero-Trust-Architektur.git`
2. Öffne den Ordner in VS Code.
3. Bestätige die Meldung **"Reopen in Container"** (unten rechts).
4. Starte die Umgebung:
```bash
docker-compose up --build

```



**Beispiel für die Konsolen-Ausgabe:**

```text
zt_server  | [20:45:36.083] [SERVER ] [INFO ] Server gestartet auf Port 8443...
zt_client  | [20:45:36.342] [SYSTEM ] [INFO ] Starte Client-Verbindung zu: zt_server
zt_server  | [20:45:36.363] [SERVER ] [INFO ] Neue Verbindung von: 172.19.0.3
zt_client  | [20:45:36.363] [CLIENT ] [INFO ] Starte mTLS Handshake...
zt_server  | [20:45:36.373] [SERVER ] [INFO ] mTLS Handshake erfolgreich!
zt_client  | [20:45:36.372] [CLIENT ] [INFO ] ERFOLG: Verbindung ist sicher.
zt_server  | [20:45:36.373] [SERVER ] [INFO ] VERIFIZIERT: Client: /CN=client-app
zt_client  | [20:45:36.373] [CLIENT ] [INFO ] --- Server Antwort Start ---
zt_client  | HTTP/1.1 200 OK
zt_client  | Identität bestätigt. Zugriff gewährt.
zt_server  | [20:45:36.373] [SERVER ] [INFO ] BINÄRE DATEN VERIFIZIERT: [ID: 42 | Temp: 22.5°C]
zt_client exited with code 0

```

### Option B: Natives Testen (MSYS2 UCRT64)

1. **Terminal 1 (Server):** `./bin/mtls_app server`
2. **Terminal 2 (Client):** `./bin/mtls_app client localhost`

---

## ⌨️ VS Code "Cheat Sheet" (Windows / MSYS2)

Nutze `Strg + Shift + P` für folgende Aktionen:

* `Terminal: Select Default Profile` -> **MSYS2 UCRT64**
* `CMake: Select a Kit` -> **GCC 15.2.0 UCRT64**
* `CMake: Configure` / `CMake: Build`
* `CMake: Delete Cache and Reconfigure` (bei Build-Problemen)

---

## 🐳 Docker "Cheat Sheet"

* `docker-compose up` / `down`: Start/Stopp der Services.
* `docker-compose logs -f`: Live-Logs verfolgen.
* `docker system prune`: Aufräumen ungenutzter Docker-Ressourcen.

---

## 🔍 Debugging

* **Handshake-Fehler:** Systemzeit prüfen (Zertifikatsgültigkeit!).
* **Verbindung abgelehnt:** Prüfen, ob Port **8443** frei ist.
* **Docker-Host:** In Docker-Netzwerken immer den Servicenamen `zt_server` statt `localhost` verwenden.

---

## 📊 System-Architektur

1. **Handshake:** Gegenseitiger Zertifikatsaustausch.
2. **Verifikation:** Validierung gegen die Root-CA.
3. **Verschlüsselung:** Aufbau des gesicherten Tunnels.
4. **Datentransfer:** Übertragung der binären Sensor-Payload.

---

*Erstellt für die mTLS Zero-Trust Demonstration 2026.*

---

