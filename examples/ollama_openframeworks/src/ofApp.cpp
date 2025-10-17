#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("OllamaClient OpenFrameworks Example");
    ofSetFrameRate(60);
    ofBackground(40);

    // Initialize drawing canvas
    drawingCanvas.allocate(512, 512, GL_RGBA);
    drawingCanvas.begin();
    ofClear(255, 255, 255, 255);
    drawingCanvas.end();

    isDrawing = false;
    analyzing = false;
    lastAnalysisTime = 0;
    lastResult = "Draw something and press SPACE to analyze!";

    //// Load font
    //font.load("arial.ttf", 14);

    // Initialize Ollama client (default: localhost:11434, llava:7b)
    ofLogNotice() << "OllamaClient initialized";
    ofLogNotice() << "Draw with mouse, press SPACE to analyze, C to clear";
}

void ofApp::update() {
    // Nothing to update
}

void ofApp::draw() {
    // Draw the canvas
    ofSetColor(255);
    float x = (ofGetWidth() - 512) / 2;
    float y = 50;
    drawingCanvas.draw(x, y);

    // Draw border
    ofNoFill();
    ofSetColor(200);
    ofDrawRectangle(x-1, y-1, 514, 514);

    // Draw instructions
    ofSetColor(255);
    ofDrawBitmapString("Draw with mouse | SPACE = Analyze | C = Clear", 20, 30);

    // Draw status
    if (analyzing) {
        ofSetColor(255, 200, 0);
        ofDrawBitmapString("Analyzing...", 20, ofGetHeight() - 60);
    }

    // Draw result
    ofSetColor(100, 255, 100);
    ofDrawBitmapString("Result: " + lastResult, 20, ofGetHeight() - 30);
}

void ofApp::mouseDragged(int x, int y, int button) {
    float canvasX = (ofGetWidth() - 512) / 2;
    float canvasY = 50;

    // Check if inside canvas
    if (x >= canvasX && x <= canvasX + 512 &&
        y >= canvasY && y <= canvasY + 512) {

        drawingCanvas.begin();
        ofFill();
        ofSetColor(0);
        ofDrawCircle(x - canvasX, y - canvasY, 5);
        drawingCanvas.end();
    }
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        // Analyze the drawing
        if (!analyzing) {
            analyzing = true;
            lastResult = "Analyzing...";

            // Get pixels from FBO
            ofPixels pixels;
            drawingCanvas.readToPixels(pixels);

            // Send for analysis
            ollama.sendPixelsForInference(
                pixels,
                "What do you see in this drawing? Describe it in one sentence.",
                onAnalysisComplete,
                this
            );

            lastAnalysisTime = ofGetElapsedTimef();
        }
    }
    else if (key == 'c' || key == 'C') {
        // Clear canvas
        drawingCanvas.begin();
        ofClear(255, 255, 255, 255);
        drawingCanvas.end();

        lastResult = "Canvas cleared. Draw something!";
    }
}

void ofApp::onAnalysisComplete(const string& result, void* userData) {
    ofApp* app = static_cast<ofApp*>(userData);
    if (app) {
        app->analyzing = false;
        app->lastResult = result;

        float duration = ofGetElapsedTimef() - app->lastAnalysisTime;
        ofLogNotice() << "Analysis complete in " << duration << "s: " << result;
    }
}
