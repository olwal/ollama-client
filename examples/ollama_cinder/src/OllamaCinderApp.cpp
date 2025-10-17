#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Text.h"
#include "cinder/gl/TextureFont.h"
#include <OllamaClient/OllamaClientCinder.h>

using namespace ci;
using namespace ci::app;
using namespace std;

class OllamaCinderApp : public App {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void mouseDrag(MouseEvent event) override;
    void keyDown(KeyEvent event) override;

private:
    // Ollama client
    OllamaClientCinder ollama;

    // Drawing canvas
    gl::FboRef drawingFbo;
    bool isDrawing;
    vec2 lastPoint;

    // Results
    string lastResult;
    bool analyzing;
    double lastAnalysisTime;

    // UI
    gl::TextureFontRef font;

    // Static callback
    static void onAnalysisComplete(const string& result, void* userData);
};

void OllamaCinderApp::setup() {
    setWindowSize(800, 650);
    setWindowPos(100, 100);
    getWindow()->setTitle("OllamaClient Cinder Example");

    // Initialize drawing FBO
    gl::Fbo::Format format;
    format.colorTexture();
    drawingFbo = gl::Fbo::create(512, 512, format);

    // Clear to white
    {
        gl::ScopedFramebuffer fbScp(drawingFbo);
        gl::clear(ColorA(1, 1, 1, 1));
    }

    isDrawing = false;
    analyzing = false;
    lastAnalysisTime = 0.0;
    lastResult = "Draw something and press SPACE to analyze!";

    // Load font
    TextLayout layout;
    layout.setFont(Font("Arial", 18));
    font = gl::TextureFont::create(Font("Arial", 18));

    CI_LOG_I("OllamaClient initialized");
    CI_LOG_I("Draw with mouse, press SPACE to analyze, C to clear");
}

void OllamaCinderApp::update() {
    // Nothing to update
}

void OllamaCinderApp::draw() {
    gl::clear(Color(0.16f, 0.16f, 0.16f));

    // Draw the canvas
    gl::color(1, 1, 1);
    float x = (getWindowWidth() - 512) / 2;
    float y = 50;
    gl::draw(drawingFbo->getColorTexture(), vec2(x, y));

    // Draw border
    gl::color(0.8f, 0.8f, 0.8f);
    gl::drawStrokedRect(Rectf(x - 1, y - 1, x + 513, y + 513));

    // Draw instructions
    gl::color(1, 1, 1);
    font->drawString("Draw with mouse | SPACE = Analyze | C = Clear",
        vec2(20, 30));

    // Draw status
    if (analyzing) {
        gl::color(1, 0.8f, 0);
        font->drawString("Analyzing...", vec2(20, getWindowHeight() - 60));
    }

    // Draw result
    gl::color(0, 1, 0.4f);
    font->drawString("Result: " + lastResult, vec2(20, getWindowHeight() - 30));
}

void OllamaCinderApp::mouseDrag(MouseEvent event) {
    float canvasX = (getWindowWidth() - 512) / 2;
    float canvasY = 50;

    vec2 pos = event.getPos();

    // Check if inside canvas
    if (pos.x >= canvasX && pos.x <= canvasX + 512 &&
        pos.y >= canvasY && pos.y <= canvasY + 512) {

        vec2 localPos = pos - vec2(canvasX, canvasY);

        // Draw to FBO
        {
            gl::ScopedFramebuffer fbScp(drawingFbo);
            gl::ScopedViewport viewportScp(ivec2(0), drawingFbo->getSize());
            gl::ScopedMatrices matScp;
            gl::setMatricesWindow(drawingFbo->getSize());

            gl::color(0, 0, 0);

            if (isDrawing) {
                gl::drawLine(lastPoint, localPos);
            }
            gl::drawSolidCircle(localPos, 5);

            lastPoint = localPos;
            isDrawing = true;
        }
    }
}

void OllamaCinderApp::keyDown(KeyEvent event) {
    if (event.getCode() == KeyEvent::KEY_SPACE) {
        // Analyze the drawing
        if (!analyzing) {
            analyzing = true;
            lastResult = "Analyzing...";

            // Get surface from FBO
            Surface8u surface(drawingFbo->getColorTexture()->createSource());

            // Send for analysis
            ollama.sendImageForInference(
                surface,
                "What do you see in this drawing? Describe it in one sentence.",
                onAnalysisComplete,
                this
            );

            lastAnalysisTime = app::getElapsedSeconds();
        }
    }
    else if (event.getCode() == KeyEvent::KEY_c) {
        // Clear canvas
        {
            gl::ScopedFramebuffer fbScp(drawingFbo);
            gl::clear(ColorA(1, 1, 1, 1));
        }

        lastResult = "Canvas cleared. Draw something!";
        isDrawing = false;
    }
}

void OllamaCinderApp::onAnalysisComplete(const string& result, void* userData) {
    OllamaCinderApp* app = static_cast<OllamaCinderApp*>(userData);
    if (app) {
        app->analyzing = false;
        app->lastResult = result;

        double duration = app::getElapsedSeconds() - app->lastAnalysisTime;
        CI_LOG_I("Analysis complete in " << duration << "s: " << result);
    }
}

CINDER_APP(OllamaCinderApp, RendererGl)
