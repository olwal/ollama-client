# OllamaClient

A cross-framework C++ client library for [Ollama](https://ollama.ai/) API with support for both text and vision models. Works with OpenFrameworks, Cinder, and can be extended to other C++ frameworks.

## Features

- **Framework-agnostic base**: Core HTTP communication works with any C++ project
- **OpenFrameworks support**: Native integration with `ofPixels`, `ofTexture`, and `ofImage`
- **Cinder support**: Native integration with Cinder's `Surface` and `Texture`
- **Async & Sync APIs**: Choose between callback-based or blocking calls
- **Vision models**: Send images for inference with multimodal models
- **Text models**: Send text prompts to chat models
- **Windows support**: Uses WinHTTP for reliable HTTP communication
- **No external dependencies**: Base64 encoding and JSON building included

## Quick Start

### OpenFrameworks Example

```cpp
#include <OllamaClient/OllamaClientOF.h>

class ofApp : public ofBaseApp {
    OllamaClientOF ollama;
    ofVideoGrabber camera;

    void setup() {
        camera.setup(640, 480);
        // Default: localhost:11434, llava:7b vision model
    }

    void analyzeFrame() {
        ollama.sendPixelsForInference(
            camera.getPixels(),
            "Describe what you see in this image",
            [](const string& result, void* userData) {
                ofLogNotice() << "AI says: " << result;
            },
            this
        );
    }
};
```

### Cinder Example

```cpp
#include <OllamaClient/OllamaClientCinder.h>

class MyApp : public ci::app::App {
    OllamaClientCinder ollama;

    void analyzeSurface(const ci::Surface& surf) {
        string result = ollama.sendSurfaceForInferenceSync(
            surf,
            "What colors are in this image?"
        );
        CI_LOG_I("Result: " << result);
    }
};
```

## Installation

### Option 1: Clone Next to Your Project (Recommended)

```bash
cd your-workspace/
git clone https://github.com/yourusername/OllamaClient.git
cd your-project/
```

Add `../OllamaClient/include` to your project's include paths, then use:
```cpp
#include <OllamaClient/OllamaClientOF.h>
```

### Option 2: Git Submodule

```bash
cd your-project/
git submodule add https://github.com/yourusername/OllamaClient.git lib/OllamaClient
git submodule update --init --recursive
```

Add `lib/OllamaClient/include` to your project's include paths.

### Option 3: Copy Files

Copy `include/OllamaClient/` headers and `src/` implementation files into your project.

## API Reference

### Base Class (OllamaClientBase)

All framework-specific clients inherit from this base class.

#### Constructor
```cpp
OllamaClientBase(
    const string& host = "localhost",
    int port = 11434,
    const string& visionModel = "llava:7b",
    const string& chatModel = "llama3"
);
```

#### Text Prompts
```cpp
// Async
void sendPrompt(const string& prompt, InferenceCallback callback, void* userData);

// Sync
string sendPromptSync(const string& prompt);
```

#### Model Management
```cpp
void setVisionModel(const string& visionModel);
string getVisionModel();
```

### OpenFrameworks Client (OllamaClientOF)

#### Image Inference Methods

```cpp
// With ofPixels
void sendPixelsForInference(const ofPixels& pixels, const string& prompt,
                           InferenceCallback callback, void* userData);
string sendPixelsForInferenceSync(const ofPixels& pixels, const string& prompt);

// With ofTexture
void sendTextureForInference(const ofTexture& texture, const string& prompt,
                            InferenceCallback callback, void* userData);
string sendTextureForInferenceSync(const ofTexture& texture, const string& prompt);

// With ofImage
void sendImageForInference(const ofImage& image, const string& prompt,
                          InferenceCallback callback, void* userData);
string sendImageForInferenceSync(const ofImage& image, const string& prompt);
```

#### Static Utility Methods
```cpp
static string textureToBase64Jpeg(const ofTexture& texture,
                                  ofImageQualityType quality = OF_IMAGE_QUALITY_HIGH);
static string pixelsToBase64Jpeg(const ofPixels& pixels,
                                ofImageQualityType quality = OF_IMAGE_QUALITY_HIGH);
```

### Cinder Client (OllamaClientCinder)

#### Image Inference Methods

```cpp
// With ci::Surface
void sendSurfaceForInference(const ci::Surface& surface, const string& prompt,
                            InferenceCallback callback, void* userData);
string sendSurfaceForInferenceSync(const ci::Surface& surface, const string& prompt);

// With ci::gl::Texture
void sendTextureForInference(const ci::gl::Texture& texture, const string& prompt,
                            InferenceCallback callback, void* userData);
string sendTextureForInferenceSync(const ci::gl::Texture& texture, const string& prompt);
```

## Usage Examples

### Video Analysis with OpenFrameworks

```cpp
void analyzeVideo() {
    ofVideoPlayer video;
    video.load("myvideo.mp4");
    video.play();

    OllamaClientOF ollama("localhost", 11434, "llava:7b");

    // Analyze every 10 seconds
    if (ofGetElapsedTimef() - lastAnalysisTime > 10.0f) {
        ollama.sendPixelsForInference(
            video.getPixels(),
            "Describe what is happening in this video frame",
            [](const string& result, void* userData) {
                ofLogNotice() << "Frame description: " << result;
            },
            nullptr
        );
        lastAnalysisTime = ofGetElapsedTimef();
    }
}
```

### Image Classification

```cpp
ofImage img;
img.load("photo.jpg");

OllamaClientOF ollama;
ollama.setVisionModel("llava:7b");

string result = ollama.sendImageForInferenceSync(
    img,
    "What objects are visible in this image? List them."
);
cout << "Objects found: " << result << endl;
```

### Text Chat

```cpp
OllamaClientOF ollama("localhost", 11434, "llava:7b", "llama3");

string answer = ollama.sendPromptSync(
    "Explain quantum computing in simple terms"
);
cout << answer << endl;
```

### Custom Models

```cpp
// Use a different vision model
ollama.setVisionModel("granite3.2-vision");

// Or specify in constructor
OllamaClientOF ollama("localhost", 11434, "bakllava", "codellama");
```

## Requirements

- **C++11 or later**
- **Windows**: WinHTTP (included with Windows SDK)
- **OpenFrameworks**: 0.11.0 or later (for OF client)
- **Cinder**: 0.9.0 or later (for Cinder client)
- **Ollama**: Running locally or on a network server

## Setting Up Ollama

1. Download and install [Ollama](https://ollama.ai/)
2. Pull a vision model:
   ```bash
   ollama pull llava:7b
   # or
   ollama pull bakllava
   # or
   ollama pull granite3.2-vision
   ```
3. The Ollama server runs automatically on `localhost:11434`

## Architecture

```
OllamaClientBase (Framework-agnostic)
├── HTTP communication via WinHTTP
├── JSON payload building
├── Base64 encoding
└── Threading for async operations

OllamaClientOF (OpenFrameworks)
├── Inherits from OllamaClientBase
├── ofPixels, ofTexture, ofImage support
└── JPEG encoding via ofSaveImage

OllamaClientCinder (Cinder)
├── Inherits from OllamaClientBase
├── ci::Surface, ci::gl::Texture support
└── JPEG encoding via Cinder's writeImage
```

## Extending to Other Frameworks

To add support for another framework, inherit from `OllamaClientBase` and implement:

```cpp
class OllamaClientYourFramework : public OllamaClientBase {
protected:
    // Convert your framework's image type to base64 JPEG
    string convertImageToBase64Jpeg(const void* imageData,
                                    float jpegQuality = 0.8f) override;

    // Implement the pure virtual methods
    void sendImageForInference(const void* imageData, const string& prompt,
                              InferenceCallback callback, void* userData) override;
    string sendImageForInferenceSync(const void* imageData,
                                     const string& prompt) override;
};
```

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

## Author

Created for creative coding and AI experimentation with C++ frameworks.

## See Also

- [Ollama Documentation](https://github.com/ollama/ollama)
- [OpenFrameworks](https://openframeworks.cc/)
- [Cinder](https://libcinder.org/)
