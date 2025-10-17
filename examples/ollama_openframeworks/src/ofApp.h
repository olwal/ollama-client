#pragma once

#include "ofMain.h"
#include <OllamaClient/OllamaClientOF.h>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mouseDragged(int x, int y, int button);

private:
    // Ollama client
    OllamaClientOF ollama;

    // Drawing canvas
    ofFbo drawingCanvas;
    bool isDrawing;

    // Results
    string lastResult;
    bool analyzing;
    float lastAnalysisTime;

    // UI
    ofTrueTypeFont font;

    // Static callback
    static void onAnalysisComplete(const string& result, void* userData);
};
