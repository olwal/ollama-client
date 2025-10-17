#include <OllamaClient/OllamaClientCinder.h>

OllamaClientCinder::OllamaClientCinder(const string& host, int port, const string& visionModel, const string& chatModel)
    : OllamaClientBase(host, port, visionModel, chatModel)
{
}

// Cinder Surface methods
void OllamaClientCinder::sendImageForInference(const Surface& surface, const string& prompt, InferenceCallback callback, void * userData) {
    // Create a thread to handle the HTTP request
    thread worker([this, surface, prompt, callback, userData]() {
        string result = sendImageForInferenceInternal(surface, prompt);
        callback(result, userData);
        });

    // Detach the thread so it can continue running
    worker.detach();
}

string OllamaClientCinder::sendImageForInferenceSync(const Surface& surface, const string& prompt) {
    return sendImageForInferenceInternal(surface, prompt);
}

// Cinder Texture methods
void OllamaClientCinder::sendTextureForInference(const Texture2dRef& texture, const string& prompt, InferenceCallback callback, void * userData) {
    if (!texture) {
        callback("Error: Invalid texture", userData);
        return;
    }

    // Convert texture to surface and use surface method
    Surface8u surface(texture->createSource());
    sendImageForInference(surface, prompt, callback, userData);
}

string OllamaClientCinder::sendTextureForInferenceSync(const Texture2dRef& texture, const string& prompt) {
    if (!texture) {
        return "Error: Invalid texture";
    }

    Surface8u surface(texture->createSource());
    return sendImageForInferenceSync(surface, prompt);
}

// Implementation of pure virtual methods from base class
string OllamaClientCinder::convertImageToBase64Jpeg(const void* imageData, float jpegQuality) {
    // For Cinder implementation, we expect imageData to be a Surface*
    const Surface* surface = static_cast<const Surface*>(imageData);
    if (!surface) {
        return "";
    }
    return surfaceToRawBase64Jpeg(*surface, jpegQuality);
}

void OllamaClientCinder::sendImageForInference(const void* imageData, const string& prompt, InferenceCallback callback, void * userData) {
    const Surface* surface = static_cast<const Surface*>(imageData);
    if (!surface) {
        callback("Error: Invalid surface data", userData);
        return;
    }
    sendImageForInference(*surface, prompt, callback, userData);
}

string OllamaClientCinder::sendImageForInferenceSync(const void* imageData, const string& prompt) {
    const Surface* surface = static_cast<const Surface*>(imageData);
    if (!surface) {
        return "Error: Invalid surface data";
    }
    return sendImageForInferenceSync(*surface, prompt);
}

string OllamaClientCinder::sendImageForInferenceInternal(const Surface& surface, const string& prompt) {
    try {
        string base64Image = surfaceToRawBase64Jpeg(surface);
        CI_LOG_I("Sending request to: " << mHost << ":" << mPort << mEndpoint);
        CI_LOG_I("Base64 image size: " << base64Image.size() << " bytes");

        return OllamaClientBase::sendImageForInferenceInternal(base64Image, prompt);
    }
    catch (const exception& e) {
        return "Error: " + string(e.what());
    }
}

// Static utility methods for Cinder image conversion
string OllamaClientCinder::textureToBase64Jpeg(const Texture2dRef& texture, float jpegQuality) {
    if (!texture) {
        return "";
    }

    // Create a Surface from the texture
    Surface8u surface(texture->createSource());

    // Create a memory stream
    OStreamMemRef stream = OStreamMem::create();

    // Create a DataTarget that uses the stream
    DataTargetRef target = DataTargetStream::createRef(stream);

    // Set JPEG compression options
    ImageTarget::Options options;
    options.quality(jpegQuality);

    // Write the surface as JPEG to the target
    writeImage(target, surface, options, "jpg");

    // Get the buffer from the stream
    void* bufferData = stream->getBuffer();
    size_t bufferSize = stream->tell();

    // Encode to base64
    string base64Encoded = base64_encode(
        reinterpret_cast<const unsigned char*>(bufferData),
        bufferSize
    );

    return "data:image/jpeg;base64," + base64Encoded;
}

string OllamaClientCinder::textureToRawBase64Jpeg(const Texture2dRef& texture, float jpegQuality) {
    if (!texture) {
        return "";
    }

    Surface8u surface(texture->createSource());
    return surfaceToRawBase64Jpeg(surface, jpegQuality);
}

string OllamaClientCinder::surfaceToRawBase64Jpeg(const Surface& surface, float jpegQuality) {
    OStreamMemRef stream = OStreamMem::create();
    DataTargetRef target = DataTargetStream::createRef(stream);

    ImageTarget::Options options;
    options.quality(jpegQuality);

    writeImage(target, surface, options, "jpg");

    void* bufferData = stream->getBuffer();
    size_t bufferSize = stream->tell();

    return base64_encode(
        reinterpret_cast<const unsigned char*>(bufferData),
        bufferSize
    );
}