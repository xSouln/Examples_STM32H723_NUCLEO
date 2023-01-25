#include "Components.h"

#include <stdbool.h>
#include <stdint.h>
#include "HTTP_Helper.h"
#include "common\timer_platform.h"
#include "network_interface.h"

#include "aws_iot_error.h"
#include "aws_iot_log.h"
#include "tls\network_platform.h"
#include "wolfssl\wolfcrypt\memory.h"

#include "timer_interface.h"

#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/pkcs12.h"
/*
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"
*/
#include "lwip.h"
#include "sockets.h"
#include "netdb.h"

#define AWS_YIELD_EXTENSION		50
#define SOCKET_RX_BLOCK_TIME	500
#define SOCKET_TX_BLOCK_TIME	3000

IoT_Error_t iot_tls_init(	Network* pNetwork, char* pRootCALocation, char* pDeviceCertLocation,
							char* pDevicePrivateKeyLocation, char* pDestinationURL,
							uint16_t DestinationPort, uint32_t timeout_ms, bool ServerVerificationFlag )
{
	// Let's initialise these mothers.
	pNetwork->tlsConnectParams.DestinationPort = DestinationPort;
	pNetwork->tlsConnectParams.pDestinationURL = pDestinationURL;
	pNetwork->tlsConnectParams.pDeviceCertLocation = pDeviceCertLocation;
	pNetwork->tlsConnectParams.pDevicePrivateKeyLocation = pDevicePrivateKeyLocation;
	pNetwork->tlsConnectParams.pRootCALocation = pRootCALocation;
	pNetwork->tlsConnectParams.timeout_ms = timeout_ms;
	pNetwork->tlsConnectParams.ServerVerificationFlag = ServerVerificationFlag;

	pNetwork->connect = iot_tls_connect;
	pNetwork->read = iot_tls_read;
	pNetwork->write = iot_tls_write;
	pNetwork->disconnect = iot_tls_disconnect;
	pNetwork->isConnected = iot_tls_is_connected;
	pNetwork->destroy = iot_tls_destroy;

	pNetwork->tlsDataParams.connected = false;
	pNetwork->tlsDataParams.credentials = (SUREFLAP_CREDENTIALS*)pDeviceCertLocation;

	pNetwork->tlsDataParams.socket = -1;

	int result = SSL_SUCCESS;

	if(wolfSSL_Init() != SSL_SUCCESS)
	{
		return NETWORK_SSL_INIT_ERROR;
	}

	if(pNetwork->tlsDataParams.ssl_ctx == NULL)
	{
		pNetwork->tlsDataParams.ssl_ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());

		if(pNetwork->tlsDataParams.ssl_ctx == NULL)
		{
			return NETWORK_SSL_INIT_ERROR;
		}

		wolfSSL_CTX_SetIORecv(pNetwork->tlsDataParams.ssl_ctx, Wrapper_Receive);
		wolfSSL_CTX_SetIOSend(pNetwork->tlsDataParams.ssl_ctx, Wrapper_Send);

		wolfSSL_CTX_set_verify(pNetwork->tlsDataParams.ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
		// CA Certificate:
		WOLFSSL_MSG("\tLoad Root CA");
		result = wolfSSL_CTX_load_verify_buffer(pNetwork->tlsDataParams.ssl_ctx,
				(unsigned char*)pRootCALocation,
				strlen(pRootCALocation), SSL_FILETYPE_PEM );

		if(result != SSL_SUCCESS)
		{
			return NETWORK_SSL_CERT_ERROR;
		}

		if( (NULL == pNetwork->tlsDataParams.credentials->decoded_cert) ||
		    (NULL == pNetwork->tlsDataParams.credentials->decoded_key) )
		{
			return NETWORK_SSL_CERT_ERROR;
		}

		result = wolfSSL_CTX_use_certificate_buffer( pNetwork->tlsDataParams.ssl_ctx,
				(const unsigned char*)pNetwork->tlsDataParams.credentials->decoded_cert,
				pNetwork->tlsDataParams.credentials->decoded_cert_size,
				SSL_FILETYPE_ASN1);

		if(result != SSL_SUCCESS)
		{
			return NETWORK_SSL_CERT_ERROR;
		}

		result = wolfSSL_CTX_use_PrivateKey_buffer(pNetwork->tlsDataParams.ssl_ctx,
				(const unsigned char*)pNetwork->tlsDataParams.credentials->decoded_key,
				pNetwork->tlsDataParams.credentials->decoded_key_size,
				SSL_FILETYPE_ASN1 );

		if(result != SSL_SUCCESS)
		{
			return NETWORK_SSL_CERT_ERROR;
		}
	}

	pNetwork->tlsDataParams.ssl_obj = wolfSSL_new(pNetwork->tlsDataParams.ssl_ctx);
	if(pNetwork->tlsDataParams.ssl_obj == NULL)
	{
		return NETWORK_SSL_INIT_ERROR;
	}

	result = wolfSSL_check_domain_name(pNetwork->tlsDataParams.ssl_obj, pDestinationURL);
	if(result != SSL_SUCCESS)
	{
		return NETWORK_SSL_INIT_ERROR;
	}

	wolfSSL_set_using_nonblock(pNetwork->tlsDataParams.ssl_obj, 1);

	/* create a TCP socket */
	pNetwork->tlsDataParams.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pNetwork->tlsDataParams.socket < 0)
	{
		return TCP_SETUP_ERROR;
	}

	wolfSSL_set_fd(pNetwork->tlsDataParams.ssl_obj, (int)pNetwork->tlsDataParams.socket);
	/*
	uint32_t timeout = SOCKET_RX_BLOCK_TIME;
	lwip_setsockopt(pNetwork->tlsDataParams.socket, 0, SO_RCVTIMEO, &timeout, sizeof(timeout));
	timeout = SOCKET_TX_BLOCK_TIME;
	lwip_setsockopt(pNetwork->tlsDataParams.socket, 0, SO_SNDTIMEO, &timeout, sizeof(timeout));
	*/
	struct timeval timeout = { 0 };

	timeout.tv_sec = SOCKET_RX_BLOCK_TIME;
	setsockopt(pNetwork->tlsDataParams.socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	timeout.tv_sec = SOCKET_TX_BLOCK_TIME;
	setsockopt(pNetwork->tlsDataParams.socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	return SUCCESS;
}

IoT_Error_t iot_tls_connect(Network* pNetwork, TLSConnectParams* TLSParams)
{
	struct hostent* hostent_result;

	if(pNetwork->tlsDataParams.socket < 0)
	{
		wrapper_printf("\r\n\t--- TLS Connect Failed: No Socket.\r\n");
		return NETWORK_ERR_NET_SOCKET_FAILED;
	}

	hostent_result = gethostbyname(pNetwork->tlsConnectParams.pDestinationURL);

	if (!hostent_result)
	{
		return NETWORK_ERR_NET_SOCKET_FAILED;
	}

	struct sockaddr_in address;
	address.sin_family = AF_UNSPEC;
	address.sin_addr.s_addr = ((ip_addr_t*)(hostent_result->h_addr_list[0]))[0].addr;
	address.sin_port = htons(pNetwork->tlsConnectParams.DestinationPort);

	if(address.sin_addr.s_addr == 0)
	{
		wrapper_printf("\r\n\t--- TLS Connect Failed: Unable to resolve host.\r\n");
		return NETWORK_ERR_NET_UNKNOWN_HOST;
	}

	//int32_t result = FreeRTOS_connect(pNetwork->tlsDataParams.socket, &address, sizeof(address));
	int32_t result = connect(pNetwork->tlsDataParams.socket, (const struct sockaddr*)&address, sizeof(address));
	if( pdFREERTOS_ERRNO_NONE != result )
	{
		wrapper_printf("\r\n\t--- Socket Connect Failed: %d\r\n", result);
		return TCP_CONNECTION_ERROR;
	}

	int ret = wolfSSL_connect(pNetwork->tlsDataParams.ssl_obj);
	int wolf_state = SSL_SUCCESS;

	if( 0 >= ret )
	{
		wolf_state = wolfSSL_get_error(pNetwork->tlsDataParams.ssl_obj, ret);
	}

	switch( wolf_state )
	{
		case WOLFSSL_ERROR_WANT_READ:
			return NETWORK_SSL_NOTHING_TO_READ;
		case WOLFSSL_ERROR_WANT_WRITE:
			return NETWORK_SSL_WRITE_ERROR;
		case WOLFSSL_SUCCESS:
			wrapper_printf("SSL Connect Succeeded.\r\n");
			return SUCCESS;
		default:
			wrapper_printf("Unknown SSL Error: %d\r\n", wolf_state);
			wolfSSL_UnloadCertsKeys(pNetwork->tlsDataParams.ssl_obj);
			return SSL_CONNECTION_ERROR;
	}
}

IoT_Error_t iot_tls_write( Network* pNetwork, unsigned char* buf, size_t count, Timer* write_timer, size_t* written )
{
	int ret = wolfSSL_write(pNetwork->tlsDataParams.ssl_obj, buf, count);
	int wolf_state = SSL_SUCCESS;
	if( 0 >= ret )
	{
		wolf_state = wolfSSL_get_error( pNetwork->tlsDataParams.ssl_obj, ret );
	}

	if( SSL_SUCCESS == wolf_state )
	{
		*written = ret;
		return SUCCESS;
	}

	return NETWORK_SSL_WRITE_ERROR;
}

IoT_Error_t iot_tls_read( Network* pNetwork, unsigned char* buf, size_t to_read, Timer* read_timer, size_t* consumed )
{
	int ret = wolfSSL_read( pNetwork->tlsDataParams.ssl_obj, buf, to_read );
	int wolf_state = SSL_SUCCESS;
	if( 0 >= ret )
	{
		wolf_state = wolfSSL_get_error( pNetwork->tlsDataParams.ssl_obj, ret );
	}

	switch( wolf_state )
	{
		case SSL_SUCCESS:
			*consumed = ret;
			extend_countdown_ms(read_timer, AWS_YIELD_EXTENSION);
			return SUCCESS;

		case SSL_ERROR_WANT_READ:
			return NETWORK_SSL_NOTHING_TO_READ;
	}
	return NETWORK_SSL_READ_ERROR;
}

IoT_Error_t iot_tls_disconnect( Network* pNetwork )
{
	int ret;
	uint32_t attempts = 0;

	pNetwork->tlsDataParams.connected = false;
	if( NULL == pNetwork->tlsDataParams.ssl_obj ){ return SUCCESS; }
	do
	{
		attempts++;
		ret = wolfSSL_shutdown( pNetwork->tlsDataParams.ssl_obj ); // Note this may return WOLFSSL_SHUTDOWN_NOT_DONE for ever!
		if( WOLFSSL_SHUTDOWN_NOT_DONE == ret)
		{
			vTaskDelay(100);	// release to scheduler for 100ms
		}
	} while( (SSL_SUCCESS != ret ) && (WOLFSSL_FATAL_ERROR != ret) && (attempts < 10) );

	if( WOLFSSL_SHUTDOWN_NOT_DONE == ret ) {zprintf(LOW_IMPORTANCE,"wolfSSL_shutdown() never succeeded\r\n");}

	shutdown(pNetwork->tlsDataParams.socket, SHUT_RDWR);

	wrapper_printf("\r\n\tShutting down TLS Socket: %d\r\n", shutdown_ret);

	close(pNetwork->tlsDataParams.socket);
	pNetwork->tlsDataParams.socket = -1;

	return SUCCESS;
}

IoT_Error_t iot_tls_destroy( Network* pNetwork )
{
	wolfSSL_free( pNetwork->tlsDataParams.ssl_obj );
	wolfSSL_Cleanup();

	pNetwork->tlsDataParams.ssl_obj = NULL;
	return SUCCESS;
}

IoT_Error_t iot_tls_is_connected(Network* pNetwork)
{
	/*
	if( pdTRUE == FreeRTOS_issocketconnected(pNetwork->tlsDataParams.socket) )
	{
		return NETWORK_PHYSICAL_LAYER_CONNECTED;
	}
	*/
	return NETWORK_PHYSICAL_LAYER_DISCONNECTED;
}

/*** Wolf Callbacks! ***/
int Wrapper_Receive(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
	int32_t result;
	/*
	int32_t result = lwip_recv((socket_t)*(int32_t*)ctx, buf, sz, ssl->rflags);
	if( 0 < result )
	{
		return result;
	}
	*/
	switch( result )
	{
		case 0: // Nothing to read right now.
			return WOLFSSL_CBIO_ERR_WANT_READ;
		case -pdFREERTOS_ERRNO_ENOTCONN:	// Not connected.
		case -pdFREERTOS_ERRNO_ENOMEM:	// Memory allocation error.
		case -pdFREERTOS_ERRNO_EINVAL:	// Socket invalid.
			return WOLFSSL_CBIO_ERR_CONN_CLOSE;
		default:
			return WOLFSSL_CBIO_ERR_GENERAL;
	}
}

int Wrapper_Send(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
	int32_t sent;
	/*
	int32_t sent = FreeRTOS_send((Socket_t)*(int32_t*)ctx, buf, sz, ssl->wflags);
	if( 0 < sent )
	{
		return sent;
	}
	*/
	switch( sent )
	{
		case 0:	// Could not transmit.
			return WOLFSSL_CBIO_ERR_WANT_WRITE;
		case -pdFREERTOS_ERRNO_ENOTCONN:	// Not connected.
		case -pdFREERTOS_ERRNO_ENOSPC:	// No space left?
		case -pdFREERTOS_ERRNO_ENOMEM:	// Memory allocation error.
		case -pdFREERTOS_ERRNO_EINVAL:	// Invalid socket.
			return WOLFSSL_CBIO_ERR_CONN_CLOSE;
		default:
			return WOLFSSL_CBIO_ERR_GENERAL;
	}
}

