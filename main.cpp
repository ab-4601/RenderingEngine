// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "Application.h"

uint Mesh::meshCount = 0;

int main() {
    srand((uint)time(0));

    Application app{};
    app.start();

    return 0;
}