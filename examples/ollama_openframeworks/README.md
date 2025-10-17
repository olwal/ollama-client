# OpenFrameworks Example - ollama_openframeworks

A simple drawing app that uses OllamaClient to analyze your sketches with AI.

This is a complete Visual Studio 2022 project ready to build and run.

## Setup

### Quick Start
1. Open `ollama_openframeworks.sln` in Visual Studio 2022
2. Make sure Ollama is running (see below)
3. Build and run (F5)

The project is already configured with:
- OllamaClient include path: `$(SolutionDir)..\..\include`
- OllamaClient source files: `OllamaClientBase.cpp` and `OllamaClientOF.cpp`

## Usage

1. Make sure Ollama is running locally with a vision model:
   ```bash
   ollama pull llava:7b
   ollama serve
   ```

2. Run the app
3. Draw something with your mouse
4. Press SPACE to analyze the drawing
5. Press C to clear the canvas

## Controls

- **Mouse Drag**: Draw
- **SPACE**: Analyze drawing with AI
- **C**: Clear canvas

## Requirements

- OpenFrameworks 0.11.0+
- Ollama running locally
- A vision model (llava:7b, bakllava, etc.)
