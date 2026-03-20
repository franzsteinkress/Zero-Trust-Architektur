## @file Dockerfile
## @brief Build-Rezept für die mTLS-Komponenten.
## @details Nutzt ein Multi-Stage-Build (Ubuntu 24.04), installiert Boost.Asio 
## und OpenSSL 3.x. Optimiert für minimale Image-Größe und DSA-Sicherheitsvorgaben.

# --- STAGE 1: Build & Cert Generation ---
FROM debian:trixie AS builder
RUN apt-get update && apt-get install -y \
    g++ cmake libboost-system-dev libboost-thread-dev libssl-dev openssl bash \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# 1. Zertifikate generieren (Erzeugt /app/bin/certs/*.pem)
RUN chmod +x generate_san_certs.sh && ./generate_san_certs.sh

# 2. App bauen (Erzeugt /app/bin/mtls_app laut deiner CMakeLists.txt)
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release . && \
    cmake --build build

# --- STAGE 2: Runtime ---
FROM debian:trixie-slim
RUN apt-get update && apt-get install -y \
    libssl3 libboost-system-dev libboost-thread-dev ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Kopiere die fertige Binary DIREKT aus dem bin-Ordner der Stage 1
#COPY --from=builder /app/bin/mtls_app ./

# Kopiere die generierten Zertifikate DIREKT aus dem certs-Ordner
#COPY --from=builder /app/bin/certs/*.pem ./

# Ausführrechte sicherstellen
#RUN chmod +x ./mtls_app

#ENTRYPOINT ["./mtls_app"]

# Kopiere die fertige Binary für das Image
COPY --from=builder /app/bin/mtls_app ./bin/

# Kopiere die generierten Zertifikate für das Image
COPY --from=builder /app/bin/certs/ /app/bin/certs/

# Ausführrechte sicherstellen
RUN chmod +x /app/bin/mtls_app

ENTRYPOINT ["/app/bin/mtls_app"]

CMD ["server"]