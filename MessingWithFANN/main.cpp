#include <iostream>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <cassert>

#include <raylib.h>

constexpr const int WINDOW_WIDTH = 600;
constexpr const int WINDOW_HEIGHT = 600;
constexpr const char* WINDOW_TITLE = "Messing with graphs.";

struct Polynomial4
{
    float SampleFunctionAt(const float x) const
    {
        return (c3 * x * x * x) + (c2 * x * x) + (c1 * x) + c0;
    }

    Polynomial4 Derivative()
    {
        return {0, 3.0f * c3, 2.0f * c2, c1};
    }

    float c3 = 0;
    float c2 = 0;
    float c1 = 0;
    float c0 = 0;
};

void SamplePolynomial4AtInterval(const Polynomial4 p, const float intervalBegin, const float intervalEnd, const float stepSize, std::vector<float>& outSamples)
{
    assert(outSamples.size() == size_t(std::floorf((intervalEnd - intervalBegin) / stepSize)) && "outSamples vector has an unexpected size.");
    size_t idx = 0;
    for (float x = intervalBegin; x < intervalEnd - stepSize; x += stepSize)
    {
        outSamples[idx++] = p.SampleFunctionAt(x);
    }
}

Vector2 CartesianToScreenSpace(const Vector2 in)
{
    return
    {
        (in.x * (WINDOW_HEIGHT / 2) + (WINDOW_HEIGHT / 2)),
        (-in.y * (WINDOW_WIDTH / 2) + (WINDOW_WIDTH / 2))
    };
}

void PrintVector2(const Vector2 v)
{
    std::cout << std::to_string(v.x) << ";" << std::to_string(v.y) << "\n";
}

void PrintFloat(const float f)
{
    std::cout << std::to_string(f) << "\n";
}

void DrawSamples(const std::vector<float> y, const Color c, const float intervalBegin, const float intervalEnd, const float stepSize, const Vector2 normalizedOffset = {0,0})
{
    size_t idx = 0;
    for (float x = intervalBegin; x < intervalEnd - stepSize; x += stepSize)
    {
        const auto screenSpacePos = CartesianToScreenSpace({ x + normalizedOffset.x, y[idx++] + normalizedOffset.y });
        if (screenSpacePos.x < 0 || screenSpacePos.x >= WINDOW_WIDTH ||
            screenSpacePos.y < 0 || screenSpacePos.y >= WINDOW_HEIGHT)
        {
            continue;
        }
        DrawPixelV(
            screenSpacePos,
            c
        );
    }
}

constexpr const float PARAMETERS_ADJUSTEMENT_MULTIPLIER = 0.001f;
constexpr const float PARAMETERS_ADJUSTEMENT_MULTIPLIER_C2 = PARAMETERS_ADJUSTEMENT_MULTIPLIER * 1.0f;
constexpr const float PARAMETERS_ADJUSTEMENT_MULTIPLIER_C1 = PARAMETERS_ADJUSTEMENT_MULTIPLIER * 1.0f;
constexpr const float PARAMETERS_ADJUSTEMENT_MULTIPLIER_C0 = PARAMETERS_ADJUSTEMENT_MULTIPLIER * 1.0f;

int main()
{
    Polynomial4 p0 = {1, 1, 0, 0};
    Polynomial4 d0 = {0, 0, 0, 0};
    Polynomial4 tangent = {0, 0, 0, 0};
    constexpr const float xStart = -1.0f;
    constexpr const float xEnd = +1.0f;
    constexpr const float xDelta = (xEnd - xStart) / WINDOW_WIDTH;
    std::vector<float> p0Samples((size_t)std::floor((xEnd - xStart) / xDelta), 0.0f);
    float seek = 0;

    // Initialize window
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    
    // Main game loop
    while (!WindowShouldClose())
    {
        bool increaseC2 = IsKeyDown(KEY_KP_4);
        bool decreaseC2 = IsKeyDown(KEY_KP_1);
        bool increaseC1 = IsKeyDown(KEY_KP_5);
        bool decreaseC1 = IsKeyDown(KEY_KP_2);
        bool increaseC0 = IsKeyDown(KEY_KP_6);
        bool decreaseC0 = IsKeyDown(KEY_KP_3);
        bool seekLeft = IsKeyDown(KEY_KP_7);
        bool seekRight = IsKeyDown(KEY_KP_8);
        bool reset = IsKeyDown(KEY_R);

        p0.c2 += (float)increaseC2 * PARAMETERS_ADJUSTEMENT_MULTIPLIER_C2;
        p0.c2 -= (float)decreaseC2 * PARAMETERS_ADJUSTEMENT_MULTIPLIER_C2;
        p0.c1 += (float)increaseC1 * PARAMETERS_ADJUSTEMENT_MULTIPLIER_C1;
        p0.c1 -= (float)decreaseC1 * PARAMETERS_ADJUSTEMENT_MULTIPLIER_C1;
        p0.c0 += (float)increaseC0 * PARAMETERS_ADJUSTEMENT_MULTIPLIER_C0;
        p0.c0 -= (float)decreaseC0 * PARAMETERS_ADJUSTEMENT_MULTIPLIER_C0;
        seek += (float)seekRight * PARAMETERS_ADJUSTEMENT_MULTIPLIER;
        seek -= (float)seekLeft * PARAMETERS_ADJUSTEMENT_MULTIPLIER;
        if (reset)
        {
            p0 = { 0, 1, 0, 0 };
            d0 = { 0, 0, 0, 0 };
            seek = 0;
        }
        
        d0 = p0.Derivative();

        // Clear background to blue
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Axes.
        DrawLineV(
            { WINDOW_WIDTH * 0.5f, 0.0f},
            { WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT },
            BLACK);
        DrawLineV(
            {0.0f, WINDOW_HEIGHT * 0.5f},
            {WINDOW_WIDTH, WINDOW_HEIGHT * 0.5f},
            BLACK);

        SamplePolynomial4AtInterval(p0, xStart, xEnd, xDelta, p0Samples);
        DrawSamples(p0Samples, RED, xStart, xEnd, xDelta);

        SamplePolynomial4AtInterval(d0, xStart, xEnd, xDelta, p0Samples);
        DrawSamples(p0Samples, SKYBLUE, xStart, xEnd, xDelta);

        tangent = {0,0,d0.SampleFunctionAt(seek), 0};
        SamplePolynomial4AtInterval(tangent, xStart, xEnd, xDelta, p0Samples);
        DrawSamples(p0Samples, DARKBLUE, xStart, xEnd, xDelta, { seek, p0.SampleFunctionAt(seek) });

        EndDrawing();
    }
    
    // Close window and OpenGL context
    CloseWindow();

    return 0;
}