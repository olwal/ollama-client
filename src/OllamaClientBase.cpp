#include <OllamaClient/OllamaClientBase.h>

// Note: We'll need a lightweight JSON library for the base class
// For now, we'll use a simple JSON string building approach
#include <sstream>

// Windows specific includes - prevent winsock.h inclusion
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>  // Include winsock2.h before Windows.h
#include <Windows.h>
#include <WinHttp.h>
#pragma comment(lib, "WinHttp.lib")
#pragma comment(lib, "ws2_32.lib")

// Internal helper function
static wstring utf8ToWide(const string& str) {
    if (str.empty()) return wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

OllamaClientBase::OllamaClientBase(const string& host, int port, const string& visionModel, const string& chatModel)
    : mHost(host), mPort(port), mEndpoint("/api/chat"), mVisionModel(visionModel), mChatModel(chatModel)
{
}

void OllamaClientBase::setVisionModel(const string& visionModel)
{
    mVisionModel = visionModel;
}

string OllamaClientBase::getVisionModel()
{
    return mVisionModel;
}

void OllamaClientBase::sendPrompt(const string& prompt, InferenceCallback callback, void * userData) {
    // Create a thread to handle the HTTP request
    thread worker([this, prompt, callback, userData]() {
        string result = sendPromptInternal(prompt);
        callback(result, userData);
        });

    // Detach the thread so it can continue running
    worker.detach();
}

string OllamaClientBase::sendPromptSync(const string& prompt) {
    return sendPromptInternal(prompt);
}

// Simple base64 encoder
string OllamaClientBase::base64_encode(const unsigned char* data, size_t input_length) {
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    string result;
    result.reserve(((input_length + 2) / 3) * 4);

    unsigned char b;
    int i = 0;

    while (input_length >= 3) {
        b = (data[i] & 0xFC) >> 2;
        result += charset[b];
        b = ((data[i] & 0x03) << 4) | ((data[i + 1] & 0xF0) >> 4);
        result += charset[b];
        b = ((data[i + 1] & 0x0F) << 2) | ((data[i + 2] & 0xC0) >> 6);
        result += charset[b];
        b = data[i + 2] & 0x3F;
        result += charset[b];

        i += 3;
        input_length -= 3;
    }

    if (input_length == 2) {
        b = (data[i] & 0xFC) >> 2;
        result += charset[b];
        b = ((data[i] & 0x03) << 4) | ((data[i + 1] & 0xF0) >> 4);
        result += charset[b];
        b = ((data[i + 1] & 0x0F) << 2);
        result += charset[b];
        result += '=';
    }
    else if (input_length == 1) {
        b = (data[i] & 0xFC) >> 2;
        result += charset[b];
        b = ((data[i] & 0x03) << 4);
        result += charset[b];
        result += "==";
    }

    return result;
}


string OllamaClientBase::sendPromptInternal(const string& prompt) {
    try {
        // Create JSON payload using string building (lightweight approach)
        ostringstream json;
        json << "{\"messages\":[{\"role\":\"user\",\"content\":\"" << prompt << "\"}],\"stream\":false,\"model\":\"" << mChatModel << "\"}";

        string payload = json.str();
        return sendJSONPayload(payload);
    }
    catch (const exception& e) {
        return "Error: " + string(e.what());
    }
}

string OllamaClientBase::sendImageForInferenceInternal(const string& base64Image, const string& prompt) {
    try {
        // Create JSON payload using string building
        ostringstream json;
        json << "{\"messages\":[{\"role\":\"user\",\"images\":[\"" << base64Image << "\"],\"content\":\"" << prompt << "\"}],\"stream\":false,\"model\":\"" << mVisionModel << "\"}";

        string payload = json.str();
        return sendJSONPayload(payload);
    }
    catch (const exception& e) {
        return "Error: " + string(e.what());
    }
}

string OllamaClientBase::sendJSONPayload(const string payload) {
    try {
        // Initialize WinHTTP
        HINTERNET hSession = WinHttpOpen(L"OllamaClient/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) {
            return "Error: Failed to initialize WinHTTP";
        }

        // Convert host to wide string
        wstring wideHost = utf8ToWide(mHost);

        // Connect to server
        HINTERNET hConnect = WinHttpConnect(hSession, wideHost.c_str(), static_cast<INTERNET_PORT>(mPort), 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "Error: Failed to connect to server";
        }

        // Create request
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", utf8ToWide(mEndpoint).c_str(),
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            0);
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "Error: Failed to create HTTP request";
        }

        // Add headers
        BOOL result = WinHttpAddRequestHeaders(hRequest,
            L"Content-Type: application/json\r\n",
            -1, WINHTTP_ADDREQ_FLAG_ADD);
        if (!result) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "Error: Failed to add headers";
        }

        // Send request
        result = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            (LPVOID)payload.c_str(),
            static_cast<DWORD>(payload.size()),
            static_cast<DWORD>(payload.size()), 0);
        if (!result) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "Error: Failed to send request";
        }

        // Receive response
        result = WinHttpReceiveResponse(hRequest, NULL);
        if (!result) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "Error: Failed to receive response";
        }

        // Read response data
        string response;
        DWORD bytesAvailable = 0;
        DWORD bytesRead = 0;
        char responseBuffer[4096];

        while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
            ZeroMemory(responseBuffer, sizeof(responseBuffer));

            DWORD bytesToRead = static_cast<DWORD>(min(sizeof(responseBuffer) - 1, static_cast<size_t>(bytesAvailable)));

            if (!WinHttpReadData(hRequest, responseBuffer, bytesToRead, &bytesRead)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return "Error: Failed to read response data";
            }

            response.append(responseBuffer, bytesRead);
        }

        // Clean up
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        // Parse response JSON (simple approach - look for "content" field)
        try {
            size_t contentStart = response.find("\"content\":\"");
            if (contentStart != string::npos) {
                contentStart += 11; // Length of "content":"
                size_t contentEnd = response.find("\"", contentStart);
                if (contentEnd != string::npos) {
                    return response.substr(contentStart, contentEnd - contentStart);
                }
            }
            return "Error: Could not parse response content\nRaw response: " + response;
        }
        catch (const exception& e) {
            return "Error parsing response: " + string(e.what()) + "\nRaw response: " + response;
        }
    }
    catch (const exception& e) {
        return "Error: " + string(e.what());
    }
}