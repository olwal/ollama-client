# OllamaClient Examples

This directory contains complete, ready-to-run example projects demonstrating how to use the OllamaClient library.

## Available Examples

### 1. ollama_openframeworks
A simple drawing app for OpenFrameworks that analyzes sketches with AI.

**Features:**
- Draw with your mouse
- AI analysis of your drawings using vision models
- Simple, clean code demonstrating OllamaClient integration

**To run:**
```bash
cd ollama_openframeworks
# Open ollama_openframeworks.sln in Visual Studio 2022
# Build and run (F5)
```

### 2. ollama_cinder
A simple drawing app for Cinder that analyzes sketches with AI.

**Features:**
- Draw with your mouse
- AI analysis of your drawings using vision models
- Demonstrates Cinder-specific OllamaClient API

**To run:**
```bash
cd ollama_cinder
# Open ollama_cinder.sln in Visual Studio 2022
# Build and run (F5)
```

## Prerequisites

All examples require:
1. **Ollama** running locally on port 11434
2. A **vision model** pulled (e.g., `llava:7b` or `granite3.2-vision`)

### Setting up Ollama

```bash
# Download and install Ollama from https://ollama.ai/

# Pull a vision model
ollama pull llava:7b

# Start the server (usually starts automatically)
ollama serve
```

## Project Structure

Both examples are complete Visual Studio 2022 projects with:
- Pre-configured include paths to `../../include`
- OllamaClient source files already added
- All necessary dependencies configured

Simply open the `.sln` file and build!

## Learning Path

If you're new to OllamaClient:
1. Start with **ollama_openframeworks** if you're familiar with OpenFrameworks
2. Start with **ollama_cinder** if you're familiar with Cinder
3. Read the source code in `src/ofApp.cpp` or `src/CinderApp.cpp`
4. Experiment with different prompts and models
5. Try modifying the drawing or analysis features

## Need Help?

- Check the main [OllamaClient README](../README.md)
- Review the API documentation in the header files
- Open an issue on GitHub
