// ... existing code ...

// Add this near other global variables or state management
bool isCharging = false;

// ... existing code ...

// In the keyboard callback function or your input handling code
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // ... existing code ...
    
    // Handle C key press/release
    if (key == GLFW_KEY_C) {
        if (action == GLFW_PRESS) {
            isCharging = true;
        } else if (action == GLFW_RELEASE) {
            isCharging = false;
        }
    }
    
    // ... existing code ...
}

// ... existing code ...

// In your render loop, before drawing
void render() {
    // ... existing code ...
    
    // Set the charging state uniform
    shaderProgram.Use();
    shaderProgram.SetUniform1i("isCharging", isCharging ? 1 : 0);
    
    // ... continue with rendering code ...
}

// ... existing code ... 