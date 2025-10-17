#pragma once

#include "OllamaClientBase.h"
#include "ofMain.h"
#include "ofImage.h"
#include "ofTexture.h"
#include "ofPixels.h"

/*
    OpenFrameworks-specific implementation of OllamaClient
    Handles OF ofTexture and ofPixels types for image processing

    Usage:
    OllamaClientOF client;
    client.sendImageForInference(myTexture, "describe this image", callback, userData);

    Or with ofPixels:
    client.sendPixelsForInference(myPixels, "what do you see?", callback, userData);
*/

class OllamaClientOF : public OllamaClientBase {
public:
    OllamaClientOF(const string& host = "localhost", int port = 11434, const string& visionModel = "llava:7b", const string& chatModel = "llama3");

    // OpenFrameworks-specific image inference methods
    void sendPixelsForInference(const ofPixels& pixels, const string& prompt, InferenceCallback callback, void * userData);
    string sendPixelsForInferenceSync(const ofPixels& pixels, const string& prompt);

    // OpenFrameworks texture methods
    void sendTextureForInference(const ofTexture& texture, const string& prompt, InferenceCallback callback, void * userData);
    string sendTextureForInferenceSync(const ofTexture& texture, const string& prompt);

    // OpenFrameworks image methods (ofImage wrapper)
    void sendImageForInference(const ofImage& image, const string& prompt, InferenceCallback callback, void * userData);
    string sendImageForInferenceSync(const ofImage& image, const string& prompt);

    // Static utility methods for OpenFrameworks image conversion
    static string textureToBase64Jpeg(const ofTexture& texture, ofImageQualityType quality = OF_IMAGE_QUALITY_HIGH);
    static string pixelsToBase64Jpeg(const ofPixels& pixels, ofImageQualityType quality = OF_IMAGE_QUALITY_HIGH);
    static string imageToBase64Jpeg(const ofImage& image, ofImageQualityType quality = OF_IMAGE_QUALITY_HIGH);

protected:
    // Implementation of pure virtual methods from base class
    string convertImageToBase64Jpeg(const void* imageData, float jpegQuality = 0.8f) override;

    // Base class pure virtual implementations (using void* for generic interface)
    void sendImageForInference(const void* imageData, const string& prompt, InferenceCallback callback, void * userData) override;
    string sendImageForInferenceSync(const void* imageData, const string& prompt) override;

private:
    string sendPixelsForInferenceInternal(const ofPixels& pixels, const string& prompt);

    // Helper to convert OF quality enum to float
    static float qualityToFloat(ofImageQualityType quality);
};