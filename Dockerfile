# --- STAGE 1: Build ---
FROM debian:trixie AS builder
RUN apt-get update && apt-get install -y openssl bash \
    g++ cmake libboost-system-dev libboost-thread-dev libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Build und Installation in einen sauberen Ordner (/app/dist)
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release . && \
    cmake --build build && \
    cmake --install build --prefix /app/dist

# --- STAGE 2: Runtime ---
FROM debian:trixie-slim
# Nur die notwendigen Shared Libraries installieren (kein -dev)
RUN apt-get update && apt-get install -y \
    libssl3 \
    libboost-system1.83.0 \
    libboost-thread1.83.0 \
    libstdc++6 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Wir kopieren NUR die fertige Binary aus dem Installations-Verzeichnis
# CMake install legt sie nach /app/dist/bin/mtls_app
COPY --from=builder /app/dist/bin/mtls_app ./

# Zertifikate kopieren
COPY bin/certs/*.pem ./

# Ausführrechte sicherstellen
RUN chmod +x ./mtls_app

# Erstelle einen dedizierten User für die App
RUN useradd -m zerotrustuser
USER zerotrustuser

ENTRYPOINT ["./mtls_app"]
CMD ["server"]