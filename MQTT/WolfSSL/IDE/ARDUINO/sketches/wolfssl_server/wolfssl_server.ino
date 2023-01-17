/* wolfssl_server.ino
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */


#include <wolfssl.h>
#include <wolfssl/ssl.h>
#include <Ethernet.h>

#define USE_CERT_BUFFERS_256
#include <wolfssl/certs_test.h>

#ifdef NO_WOLFSSL_SERVER
  #error Please undefine NO_WOLFSSL_SERVER for this example
#endif

const int port = 11111; /* port to listen on */

int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx);
int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx);

EthernetServer server(port);
EthernetClient client;

WOLFSSL_CTX* ctx = NULL;
WOLFSSL* ssl = NULL;

void setup() {
  int err;
  WOLFSSL_METHOD* method;

  Serial.begin(9600);

  method = wolfTLSv1_2_server_method();
  if (method == NULL) {
    Serial.println("unable to get method");
    return;
  }
  ctx = wolfSSL_CTX_new(method);
  if (ctx == NULL) {
    Serial.println("unable to get ctx");
    return;
  }

  /* initialize wolfSSL using callback functions */
  wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
  wolfSSL_SetIOSend(ctx, EthernetSend);
  wolfSSL_SetIORecv(ctx, EthernetReceive);

  /* setup the private key and certificate */
  err = wolfSSL_CTX_use_PrivateKey_buffer(ctx, ecc_key_der_256, 
    sizeof_ecc_key_der_256, WOLFSSL_FILETYPE_ASN1);
  if (err != WOLFSSL_SUCCESS) {
    Serial.println("error setting key");
    return;
  }
  err = wolfSSL_CTX_use_certificate_buffer(ctx, serv_ecc_der_256, 
    sizeof_serv_ecc_der_256, WOLFSSL_FILETYPE_ASN1);
  if (err != WOLFSSL_SUCCESS) {
    Serial.println("error setting certificate");
    return;
  }

  /* Start the server */
  server.begin();
  
  return;
}

int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx) {
  int sent = 0;

  sent = client.write((byte*)msg, sz);

  return sent;
}

int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx) {
  int ret = 0;

  while (client.available() > 0 && ret < sz) {
    reply[ret++] = client.read();
  }

  return ret;
}

void loop() {
  int err = 0;
  int input = 0;
  char errBuf[80];
  char reply[80];
  int replySz = 0;
  const char* cipherName;

  /* Listen for incoming client requests. */
  client = server.available();
  if (!client) {
    return;
  }

  if (client.connected()) {

    Serial.println("Client connected");

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
      Serial.println("Unable to allocate SSL object");
      return;
    }

    err = wolfSSL_accept(ssl);
    if (err != WOLFSSL_SUCCESS) {
      err = wolfSSL_get_error(ssl, 0);
      wolfSSL_ERR_error_string(err, errBuf);
      Serial.print("TLS Accept Error: ");
      Serial.println(errBuf);
    }

    Serial.print("SSL version is ");
    Serial.println(wolfSSL_get_version(ssl));
    
    cipherName = wolfSSL_get_cipher(ssl);
    Serial.print("SSL cipher suite is ");
    Serial.println(cipherName);

    Serial.print("Server Read: ");
    /* wait for data */
    while (!client.available()) {}
    /* read data */
    while (wolfSSL_pending(ssl)) {
      input = wolfSSL_read(ssl, reply, sizeof(reply) - 1);
      if (input < 0) {
        err = wolfSSL_get_error(ssl, 0);
        wolfSSL_ERR_error_string(err, errBuf);
        Serial.print("TLS Read Error: ");
        Serial.println(errBuf);
        break;
      } else if (input > 0) {
        replySz = input;
        reply[input] = '\0';
        Serial.print(reply);
      } else {
        Serial.println();
      }
    }

    /* echo data */
    if ((wolfSSL_write(ssl, reply, replySz)) != replySz) {
      err = wolfSSL_get_error(ssl, 0);
      wolfSSL_ERR_error_string(err, errBuf);
      Serial.print("TLS Write Error: ");
      Serial.println(errBuf);
    }
    
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
  }

  client.stop();
  Serial.println("Connection complete");
}
