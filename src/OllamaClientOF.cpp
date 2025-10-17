#include <OllamaClient/OllamaClientOF.h>

OllamaClientOF::OllamaClientOF(const string& host, int port, const string& visionModel, const string& chatModel)
    : OllamaClientBase(host, port, visionModel, chatModel)
{
}

// OpenFrameworks ofPixels methods
void OllamaClientOF::sendPixelsForInference(const ofPixels& pixels, const string& prompt, InferenceCallback callback, void * userData) {
    // Create a thread to handle the HTTP request
    thread worker([this, pixels, prompt, callback, userData]() {
        string result = sendPixelsForInferenceInternal(pixels, prompt);
        callback(result, userData);
        });

    // Detach the thread so it can continue running
    worker.detach();
}

string OllamaClientOF::sendPixelsForInferenceSync(const ofPixels& pixels, const string& prompt) {
    return sendPixelsForInferenceInternal(pixels, prompt);
}

// OpenFrameworks ofTexture methods
void OllamaClientOF::sendTextureForInference(const ofTexture& texture, const string& prompt, InferenceCallback callback, void * userData) {
    if (!texture.isAllocated()) {
        callback("Error: Texture is not allocated", userData);
        return;
    }

    // Read texture to pixels and use pixels method
    ofPixels pixels;
    texture.readToPixels(pixels);
    sendPixelsForInference(pixels, prompt, callback, userData);
}

string OllamaClientOF::sendTextureForInferenceSync(const ofTexture& texture, const string& prompt) {
    if (!texture.isAllocated()) {
        return "Error: Texture is not allocated";
    }

    ofPixels pixels;
    texture.readToPixels(pixels);
    return sendPixelsForInferenceSync(pixels, prompt);
}

// OpenFrameworks ofImage methods
void OllamaClientOF::sendImageForInference(const ofImage& image, const string& prompt, InferenceCallback callback, void * userData) {
    if (!image.isAllocated()) {
        callback("Error: Image is not allocated", userData);
        return;
    }

    sendPixelsForInference(image.getPixels(), prompt, callback, userData);
}

string OllamaClientOF::sendImageForInferenceSync(const ofImage& image, const string& prompt) {
    if (!image.isAllocated()) {
        return "Error: Image is not allocated";
    }

    return sendPixelsForInferenceSync(image.getPixels(), prompt);
}

// Implementation of pure virtual methods from base class
string OllamaClientOF::convertImageToBase64Jpeg(const void* imageData, float jpegQuality) {
    // For OpenFrameworks implementation, we expect imageData to be an ofPixels*
    const ofPixels* pixels = static_cast<const ofPixels*>(imageData);
    if (!pixels || !pixels->isAllocated()) {
        return "";
    }

    ofImageQualityType quality = (jpegQuality > 0.9f) ? OF_IMAGE_QUALITY_BEST :
                                (jpegQuality > 0.7f) ? OF_IMAGE_QUALITY_HIGH :
                                (jpegQuality > 0.5f) ? OF_IMAGE_QUALITY_MEDIUM :
                                OF_IMAGE_QUALITY_LOW;

    return pixelsToBase64Jpeg(*pixels, quality);
}

void OllamaClientOF::sendImageForInference(const void* imageData, const string& prompt, InferenceCallback callback, void * userData) {
    const ofPixels* pixels = static_cast<const ofPixels*>(imageData);
    if (!pixels || !pixels->isAllocated()) {
        callback("Error: Invalid pixels data", userData);
        return;
    }
    sendPixelsForInference(*pixels, prompt, callback, userData);
}

string OllamaClientOF::sendImageForInferenceSync(const void* imageData, const string& prompt) {
    const ofPixels* pixels = static_cast<const ofPixels*>(imageData);
    if (!pixels || !pixels->isAllocated()) {
        return "Error: Invalid pixels data";
    }
    return sendPixelsForInferenceSync(*pixels, prompt);
}

string OllamaClientOF::sendPixelsForInferenceInternal(const ofPixels& pixels, const string& prompt) {
    try {
        string base64Image = pixelsToBase64Jpeg(pixels);
        ofLogNotice("OllamaClientOF") << "Sending request to: " << mHost << ":" << mPort << mEndpoint;
        ofLogNotice("OllamaClientOF") << "Base64 image size: " << base64Image.size() << " bytes";

        return OllamaClientBase::sendImageForInferenceInternal(base64Image, prompt);
    }
    catch (const exception& e) {
        return "Error: " + string(e.what());
    }
}

// Static utility methods for OpenFrameworks image conversion
string OllamaClientOF::textureToBase64Jpeg(const ofTexture& texture, ofImageQualityType quality) {
    if (!texture.isAllocated()) {
        return "";
    }

    ofPixels pixels;
    texture.readToPixels(pixels);
    return pixelsToBase64Jpeg(pixels, quality);
}

string OllamaClientOF::pixelsToBase64Jpeg(const ofPixels& pixels, ofImageQualityType quality) {
    if (!pixels.isAllocated()) {
        return "";
    }

    // Create an ofImage from pixels and save to buffer
    ofImage tempImage;
    tempImage.setFromPixels(pixels);

    // Use ofSaveImage to encode as JPEG to an ofBuffer
    ofBuffer jpegBuffer;
    
    //if (!ofSaveImage(tempImage.getPixels(), tempImage.getWidth(), tempImage.getHeight(),
    //                 OF_IMAGE_CHANNELS_RGB, jpegBuffer, OF_IMAGE_FORMAT_JPEG, quality)) {
    if (!ofSaveImage(tempImage.getPixels(), jpegBuffer, OF_IMAGE_FORMAT_JPEG, quality)) {
        ofLogError("OllamaClientOF") << "Failed to encode image as JPEG";
        return "";
    }

    // Convert buffer to base64
    string base64Encoded = base64_encode(
        reinterpret_cast<const unsigned char*>(jpegBuffer.getData()),
        jpegBuffer.size()
    );

    return base64Encoded;
}

string OllamaClientOF::imageToBase64Jpeg(const ofImage& image, ofImageQualityType quality) {
    if (!image.isAllocated()) {
        return "";
    }

    return pixelsToBase64Jpeg(image.getPixels(), quality);
}

float OllamaClientOF::qualityToFloat(ofImageQualityType quality) {
    switch (quality) {
        case OF_IMAGE_QUALITY_BEST: return 1.0f;
        case OF_IMAGE_QUALITY_HIGH: return 0.8f;
        case OF_IMAGE_QUALITY_MEDIUM: return 0.6f;
        case OF_IMAGE_QUALITY_LOW: return 0.4f;
        case OF_IMAGE_QUALITY_WORST: return 0.2f;
        default: return 0.8f;
    }
}