#include "Window.h"

Window::Window()
    : window{ nullptr }, windowWidth{ 0 }, windowHeight{ 0 }, bufferWidth { 0 }, bufferHeight{ 0 },
    lastX{ 0.f }, lastY{ 0.f }, XChange{ 0.f }, YChange{ 0.f }, mouseFirstMoved{ true }, scrollChange{ 0.f }, 
    LMBPressed{ false }, RMBPressed{ false }, viewportX{0}, viewportY{0} {

    for (size_t i = 0; i < 1024; i++)
        this->keys[i] = false;

    setupWindow();
}

void Window::handleInput(GLFWwindow* window, int key, int code, int action, int mode) {
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            currWindow->keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            currWindow->keys[key] = false;
        }
    }
}

void Window::setMousePosition(GLFWwindow* window, double xPos, double yPos) {
    GLFWwindow* temp = window;
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    currWindow->viewportX = xPos;
    currWindow->viewportY = yPos;

    if (currWindow->getRMBPressed() == true) {
        glfwSetInputMode(temp, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (currWindow->mouseFirstMoved) {
            currWindow->lastX = (GLfloat)xPos;
            currWindow->lastY = (GLfloat)yPos;
            currWindow->mouseFirstMoved = false;
        }

        currWindow->XChange = static_cast<GLfloat>((GLfloat)xPos - currWindow->lastX);
        currWindow->YChange = static_cast<GLfloat>(currWindow->lastY - (GLfloat)yPos);

        currWindow->lastX = (GLfloat)xPos;
        currWindow->lastY = (GLfloat)yPos;
    }
    else {
        glfwSetInputMode(temp, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        currWindow->mouseFirstMoved = true;
    }
}

void Window::setMouseScroll(GLFWwindow* window, double xOffset, double yOffset) {
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if(currWindow->scrollChange < 15 || currWindow->scrollChange > -15)
        currWindow->scrollChange += (float)yOffset;
}

void Window::getMouseButton(GLFWwindow* window, int button, int action, int mods) {
    Window* currWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        currWindow->setLMBPressed(true);

    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        currWindow->setLMBPressed(false);

    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        currWindow->setRMBPressed(true);

    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        currWindow->setRMBPressed(false);
}

void Window::setupWindow() {
    if (!glfwInit()) {
        std::cerr << "Unable to initialize glfw" << std::endl;
        exit(1);
    }

    // Setup GLFW window properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, CONTEXT_VERSION::MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, CONTEXT_VERSION::MINOR);

    // Core profile = no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, ::samples);

    // Allow forward compatibility
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    this->windowWidth = mode->width;
    this->windowHeight = mode->height;

    this->window = glfwCreateWindow(mode->width, mode->height, "Curr Window", glfwGetPrimaryMonitor(), NULL);

    if (!this->window) {
        std::cerr << "GLFW Window creation failed" << std::endl;
        glfwTerminate();
        exit(1);
    }

    // Get buffer size information
    glfwGetFramebufferSize(this->window, &this->bufferWidth, &this->bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(this->window);

    // Handle key and mouse input
    this->createCallbacks();

    // Lock cursor to window
    //glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        GLenum result{};
        glewGetErrorString(result);
        std::cerr << "GLEW initialization failed\n" << result << std::endl;
        glfwDestroyWindow(this->window);
        glfwTerminate();
        exit(1);
    }

    // Enable depth buffer to avoid drawing over faces
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);

    // Create viewport
    glViewport(0, 0, this->bufferWidth, this->bufferHeight);

    glfwSetWindowUserPointer(this->window, this);
}