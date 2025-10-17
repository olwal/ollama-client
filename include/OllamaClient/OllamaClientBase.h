#pragma once

#include <string>
#include <functional>
#include <thread>
#include <algorithm>

// Forward declarations to avoid including Windows headers in the header file
typedef void* HINTERNET;

using namespace std;

/*
    Generic Ollama client base class
    Framework-independent HTTP communication with Ollama API

    Subclasses should implement image conversion methods for their specific framework
*/

class OllamaClientBase {
public:
    // Callback type for inference results
    using InferenceCallback = function<void(const string& result, void * userData)>;

    OllamaClientBase(const string& host = "localhost", int port = 11434, const string& visionModel = "granite3.2-vision", const string& chatModel = "llama3");
    virtual ~OllamaClientBase() = default;

    // Text-only prompts (framework independent)
    void sendPrompt(const string& prompt, InferenceCallback callback, void * userData);
    string sendPromptSync(const string& prompt);

    // Model management
    void setVisionModel(const string& visionModel);
    string getVisionModel();

    // Simple base64 encoder
    static string base64_encode(const unsigned char* data, size_t input_length);

protected:
    // Connection parameters
    string mHost;
    int mPort;
    string mEndpoint;
    string mVisionModel;
    string mChatModel;

    // Core HTTP functionality
    string sendJSONPayload(const string payload);
    string sendPromptInternal(const string& prompt);

    // Pure virtual methods that subclasses must implement for image handling
    virtual string convertImageToBase64Jpeg(const void* imageData, float jpegQuality = 0.8f) = 0;

public:
    // Framework-specific image methods (to be implemented by subclasses)
    // These are public but pure virtual, forcing subclass implementation
    virtual void sendImageForInference(const void* imageData, const string& prompt, InferenceCallback callback, void * userData) = 0;
    virtual string sendImageForInferenceSync(const void* imageData, const string& prompt) = 0;

protected:
    // Helper for image inference that subclasses can use
    string sendImageForInferenceInternal(const string& base64Image, const string& prompt);

};