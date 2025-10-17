// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <string>

#include <curl/curl.h>

#include <dswifi9.h>
#include <nds.h>

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

static std::string buffer;

static size_t writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
    if (writerData == NULL)
        return 0;

    writerData->append(data, size*nmemb);

    return size * nmemb;
}

static int get_website_libcurl(const char *url)
{
    CURL *curl;

    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res)
        return (int)res;

    curl = curl_easy_init();
    if (curl == NULL)
        goto exit;

    // Enable verbose output. Set this to 0 to make libcurl quiet
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Use HTTP/3 but fallback to earlier HTTP if necessary
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_3);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    // Perform the request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    // Always cleanup
    curl_easy_cleanup(curl);

exit:
    curl_global_cleanup();
    return 0;
}

int main(int argc, char *argv[])
{
    PrintConsole topScreen;
    PrintConsole bottomScreen;

    defaultExceptionHandler();

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

    printf("WiFi init\n");

    if (!Wifi_InitDefault(WFC_CONNECT | WIFI_ATTEMPT_DSI_MODE))
    {
        printf("Can't connect to WiFi!\n");
        wait_forever();
    }

    printf("Connected!\n");

    const char *url = "https://example.com";

    get_website_libcurl(url);

    consoleSelect(&bottomScreen);

    printf("=====\n");
    printf("%s", buffer.c_str());
    printf("\n===== %s", url);

    consoleSelect(&topScreen);

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
