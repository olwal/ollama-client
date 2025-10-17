# Cinder Example - ollama_cinder

A simple drawing app that uses OllamaClient to analyze your sketches with AI.

This is a complete Visual Studio 2022 project ready to build and run.

## Setup

### Quick Start
1. Open `ollama_cinder.sln` in Visual Studio 2022
2. Make sure Ollama is running (see below)
3. Build and run (F5)

The project is already configured with:
- OllamaClient include path: `$(SolutionDir)..\..\include`
- OllamaClient source files: `OllamaClientBase.cpp` and `OllamaClientCinder.cpp`
- Required libraries: `WinHttp.lib`

## Usage

1. Make sure Ollama is running locally with a vision model:
   ```bash
   ollama pull llava:7b
   ollama serve
   ```

2. Build and run the app
3. Draw something with your mouse
4. Press SPACE to analyze the drawing
5. Press C to clear the canvas

## Controls

- **Mouse Drag**: Draw
- **SPACE**: Analyze drawing with AI
- **C**: Clear canvas

## Requirements

- Cinder 0.9.0+
- Ollama running locally
- A vision model (llava:7b, granite3.2-vision, etc.)
- Windows (uses WinHTTP for network communication)
