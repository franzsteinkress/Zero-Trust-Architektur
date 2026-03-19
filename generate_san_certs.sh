## @file generate_san_certs.sh
## @brief PKI-Automatisierungsskript.
## @details Generiert die Root-CA und erstellt X.509-Zertifikate mit 
## Subject Alternative Names (SAN) für die Docker-DNS-Auflösung. 
## Dies ist das Herzstück der Identitätsprüfung.

#!/bin/bash
# Verhindert Pfad-Konvertierungsfehler in MSYS2 (Windows)
export MSYS_NO_PATHCONV=1

# --- Konfiguration ---
TARGET_DIR="./bin/certs"
DAYS=3650
SUBJ_BASE="/C=DE/ST=Bavaria/L=Munich/O=ZeroTrustProject"

mkdir -p "$TARGET_DIR"
cd "$TARGET_DIR" || exit 1

echo "--- Generiere Zero-Trust PKI (SAN & mTLS) ---"

# 1. Root-CA erstellen
openssl genrsa -out ca.key 4096
openssl req -x509 -new -nodes -key ca.key -sha256 -days $DAYS -out ca.pem \
    -subj "$SUBJ_BASE/CN=ZeroTrust Root CA"

# 2. Server-Zertifikat (Rolle: serverAuth)
openssl genrsa -out server_key.pem 2048
cat > server.cnf <<EOF
[req]
distinguished_name = dn
req_extensions = v3_req
prompt = no
[dn]
CN = zt_server
[v3_req]
keyUsage = critical, digitalSignature, keyEncipherment
extendedKeyUsage = serverAuth
subjectAltName = @alt_names
[alt_names]
DNS.1 = localhost
DNS.2 = server.local
DNS.3 = zt_server
IP.1 = 127.0.0.1
EOF

openssl req -new -key server_key.pem -out server.csr -config server.cnf
openssl x509 -req -in server.csr -CA ca.pem -CAkey ca.key -CAcreateserial \
    -out server_cert.pem -days $DAYS -sha256 -extfile server.cnf -extensions v3_req

# 3. Client-Zertifikat (Rolle: clientAuth)
openssl genrsa -out client_key.pem 2048
cat > client.cnf <<EOF
[req]
distinguished_name = dn
req_extensions = v3_client
prompt = no
[dn]
CN = client-app
[v3_client]
keyUsage = critical, digitalSignature, keyEncipherment
extendedKeyUsage = clientAuth
subjectAltName = @alt_names
[alt_names]
DNS.1 = client.local
DNS.2 = zt_client
IP.1 = 127.0.0.1
EOF

openssl req -new -key client_key.pem -out client.csr -config client.cnf
openssl x509 -req -in client.csr -CA ca.pem -CAkey ca.key -CAcreateserial \
    -out client_cert.pem -days $DAYS -sha256 -extfile client.cnf -extensions v3_client

# 4. Aufräumen
rm *.csr *.cnf ca.srl 2>/dev/null || true
chmod 600 *_key.pem ca.key 2>/dev/null || true

echo "--- Fertig: Zertifikate liegen in $TARGET_DIR ---"