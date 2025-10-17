#pragma once

#include "OllamaClientBase.h"

#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/Json.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"
#include "cinder/Buffer.h"
#include "cinder/DataTarget.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace std;
using namespace gl;

/*
    Cinder-specific implementation of OllamaClient
    Handles Cinder Surface and Texture types for image processing

    https://discourse.libcinder.org/t/get-image-file-data-without-saving-to-filesystem/161/2
    https://ollama.com/library/llava
    https://ollama.com/blog/vision-models
*/

class OllamaClientCinder : public OllamaClientBase {
public:
    OllamaClientCinder(const string& host = "localhost", int port = 11434, const string& visionModel = "granite3.2-vision", const string& chatModel = "llama3");

    // Cinder-specific image inference methods
    void sendImageForInference(const Surface& surface, const string& prompt, InferenceCallback callback, void * userData);
    string sendImageForInferenceSync(const Surface& surface, const string& prompt);

    // Cinder texture methods
    void sendTextureForInference(const Texture2dRef& texture, const string& prompt, InferenceCallback callback, void * userData);
    string sendTextureForInferenceSync(const Texture2dRef& texture, const string& prompt);

    // Static utility methods for Cinder image conversion
    static string textureToBase64Jpeg(const Texture2dRef& texture, float jpegQuality = 0.8f);
    static string textureToRawBase64Jpeg(const Texture2dRef& texture, float jpegQuality = 0.8f);
    static string surfaceToRawBase64Jpeg(const Surface& surface, float jpegQuality = 0.8f);

protected:
    // Implementation of pure virtual methods from base class
    string convertImageToBase64Jpeg(const void* imageData, float jpegQuality = 0.8f) override;

    // Base class pure virtual implementations (using void* for generic interface)
    void sendImageForInference(const void* imageData, const string& prompt, InferenceCallback callback, void * userData) override;
    string sendImageForInferenceSync(const void* imageData, const string& prompt) override;

private:
    string sendImageForInferenceInternal(const Surface& surface, const string& prompt);
};