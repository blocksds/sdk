// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
// Copyright The Mbed TLS Contributors
// Copyright 2025 Antonio Niño Díaz

// SSL client demonstration program. This is a modified version of the following
// example provided by Mbed TLS:
//
// https://github.com/Mbed-TLS/mbedtls/blob/v3.6.4/programs/ssl/ssl_client1.c

#include <unistd.h>

#include <mbedtls/build_info.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/ssl.h>

#include <dswifi9.h>
#include <filesystem.h>
#include <nds.h>

// Default port for SSL
#define SERVER_PORT "443"

// Change this to get debug messages from Mbed TLS in the console
#define DEBUG_LEVEL 0

static PrintConsole topScreen;
static PrintConsole bottomScreen;

// This function will be called when Mbed TLS prints debug messages
static void my_debug(void *ctx, int level, const char *file, int line,
                     const char *str)
{
    (void)ctx;
    (void)level;

    printf("%s:%04d: %s", file, line, str);
}

static void wait_forever(void)
{
    printf("Press START to exit");

    while (1)
    {
        scanKeys();
        if (keysHeld() & KEY_START)
            exit(0);
        cothread_yield_irq(IRQ_VBLANK);
    }
}

static void getWebsiteSSL(const char *certs, const char *host, const char *path)
{
    int ret = 1, len;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    mbedtls_net_context server_fd;
    uint32_t flags;
    unsigned char buf[1024];
    const char *pers = "ssl_client1";

    printf("Mbed TLS init\n");

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

    // 0. Initialize the RNG and the session data

    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    printf("- Seeding RNG\n");

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *)pers, strlen(pers));
    if (ret != 0)
    {
        printf("  Error! ret = %d\n", ret);
        goto exit;
    }

    // 1. Initialize certificates

    printf("- Loading certs");

    // In Linux, for example, they are stored in /etc/ssl/certs/
    ret = mbedtls_x509_crt_parse_file(&cacert, certs);
    if (ret < 0)
    {
        printf("\n  Error! ret = -0x%x\n", (unsigned int)-ret);
        goto exit;
    }
    printf(" (%d skipped)\n", ret);

    // 2. Start the connection

    printf("- Connecting to tcp/%s/%s\n", host, SERVER_PORT);

    ret = mbedtls_net_connect(&server_fd, host, SERVER_PORT,
                              MBEDTLS_NET_PROTO_TCP);
    if (ret != 0)
    {
        printf("  Error! ret = %d\n", ret);
        goto exit;
    }

    // 3. Setup stuff

    printf("- Setting up SSL/TLS\n");

    ret = mbedtls_ssl_config_defaults(&conf,
                                      MBEDTLS_SSL_IS_CLIENT,
                                      MBEDTLS_SSL_TRANSPORT_STREAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0)
    {
        printf("  Config error! ret = %d\n", ret);
        goto exit;
    }

    // MBEDTLS_SSL_VERIFY_OPTIONAL means that the certificates are checked but
    // any failure is ignored, which is useful for debugging. You need to set
    // DEBUG_LEVEL to a non-zero value if you want to see the error messages.
    //
    // If you use MBEDTLS_SSL_VERIFY_REQUIRED Mbed TLS will refuse to connect if
    // the certificates aren't valid, which is the right setting for a finished
    // program.
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

    ret = mbedtls_ssl_setup(&ssl, &conf);
    if (ret != 0)
    {
        printf("  Setup error! ret = %d\n", ret);
        goto exit;
    }

    ret = mbedtls_ssl_set_hostname(&ssl, host);
    if (ret != 0)
    {
        printf("  Set hostname error! ret = %d\n", ret);
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    // 4. Handshake

    printf("- Doing SSL/TLS handshake\n");

    while (1)
    {
        ret = mbedtls_ssl_handshake(&ssl);
        if (ret == 0)
            break;

        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            printf("  Error! ret = -0x%x\n", (unsigned int)-ret);
            goto exit;
        }
    }

    // 5. Verify the server certificate

    printf("- Verifying peer X.509 cert\n");

    // In real life, we probably want to exit when ret != 0
    flags = mbedtls_ssl_get_verify_result(&ssl);
    if (flags != 0)
    {
        printf("  Error:\n");

        char vrfy_buf[512];
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        printf("%s\n", vrfy_buf);
    }

    // 6. Send the GET request

    printf("- Send GET request\n");

    len = snprintf((char *)buf, sizeof(buf),
                   "GET %s HTTP/1.1\r\n"
                   "Host: %s\r\n"
                   "User-Agent: Nintendo DS\r\n"
                   "\r\n", path, host);

    while (1)
    {
        ret = mbedtls_ssl_write(&ssl, buf, len);
        if (ret > 0)
            break;

        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            printf("  Error! ret = %d\n", ret);
            goto exit;
        }
    }

    len = ret;
    printf("  %d bytes written\n", len);

    // 7. Read the HTTP response

    printf("- Read response\n");

    while (1)
    {
        len = sizeof(buf) - 1;
        memset(buf, 0, sizeof(buf));
        ret = mbedtls_ssl_read(&ssl, buf, len);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            continue;
        }

        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
        {
            printf("The return value %d from mbedtls_ssl_read() means that the server\n"
                   "closed the connection first. We're ok with that.\n",
                   MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY);
            break;
        }

        if (ret < 0)
        {
            printf("  Error! ret = %d\n\n", ret);
            break;
        }

        if (ret == 0)
        {
            printf("  EOF\n");
            break;
        }

        len = ret;
        printf("  %d bytes read\n", len);

        // Print the result on the bottom screen
        consoleSelect(&bottomScreen);
        printf("%s", (char *) buf);
        consoleSelect(&topScreen);

        cothread_yield();
    }

    mbedtls_ssl_close_notify(&ssl);

    if (ret == 0 || ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
    {
        exit_code = MBEDTLS_EXIT_SUCCESS;
    }

exit:

    if (exit_code != MBEDTLS_EXIT_SUCCESS)
    {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, 100);
        printf("\n%s\n", error_buf);
    }

    mbedtls_net_free(&server_fd);
    mbedtls_x509_crt_free(&cacert);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

int main(int argc, char *argv[])
{
    defaultExceptionHandler();

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

    printf("NitroFS init\n");

    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit");
        wait_forever();
    }

    printf("WiFi init\n");

    if (!Wifi_InitDefault(WFC_CONNECT | WIFI_ATTEMPT_DSI_MODE))
    {
        printf("Can't connect to WiFi!\n");
        wait_forever();
    }

    printf("- Connected\n");

    consoleSelect(&bottomScreen);

    printf("Pick one option:\n");
    printf("\n");
    printf("A: Use correct certificates\n");
    printf("B: Use incorrect certificates\n");

    while (1)
    {
        scanKeys();
        u16 keys_down = keysDown();
        if (keys_down & KEY_A)
        {
            consoleClear();
            consoleSelect(&topScreen);

            getWebsiteSSL("/akkit-org-chain.pem",
                          "www.akkit.org", "/dswifi/example1.php");
            break;
        }
        if (keys_down & KEY_B)
        {
            consoleClear();
            consoleSelect(&topScreen);

            // The certificates used here are the ones required for
            // www.example.com, so they won't work for this website.
            getWebsiteSSL("/example-com-chain.pem",
                          "www.akkit.org", "/dswifi/example1.php");
            break;
        }
    }

    if (Wifi_DisconnectAP() != 0)
        printf("Error: Wifi_DisconnectAP()\n");

    Wifi_DisableWifi();

    printf("\nPress START to exit!\n");

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);
        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
