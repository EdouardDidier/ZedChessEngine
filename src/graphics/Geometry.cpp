#include "Geometry.h"

void Geometry::SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int r, SDL_Color c)
{
    vector<SDL_Vertex> vertices;
    vector<int> indices;

    // Input center vertex
    vertices.push_back({ {(float)x, (float)y}, c,  { 0, 0 } });

    // Calculating vercies for the cicle and indexing vertices to prepare renderering
    int i = 1;
    for (float theta = 0; theta < 2.0 * M_PI; theta += CIRCLE_STEP) {
        float pX = x + r * cos(theta);
        float pY = y + r * sin(theta);
        vertices.push_back({ {pX, pY}, c,  { 0, 0 } });

        // Storing vertex as triangles to prepare renderering
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
     
        i++;
    } 

    indices[indices.size() - 1] = 1; // Looping back last triangle to first vertex

    // Render circle
    SDL_RenderGeometry(renderer, NULL, vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size());
}

void Geometry::SDL_RenderHollowCircle(SDL_Renderer* renderer, int x, int y, int innerRadius, int outerRadius, SDL_Color c) {
    vector<SDL_Vertex> vertices;
    vector<int> indices;

    // Calculating vercies for the cicle and indexing vertices to prepare renderering
    int i = 0;
    for (float theta = 0; theta < 2.0 * M_PI; theta += CIRCLE_STEP) {
        float pInnerX = x + innerRadius * cos(theta);
        float pInnnerY = y + innerRadius * sin(theta);

        float pOuterX = x + outerRadius * cos(theta);
        float pOuterY = y + outerRadius * sin(theta);

        vertices.push_back({ {pInnerX, pInnnerY}, c,  { 0, 0 } });
        vertices.push_back({ {pOuterX, pOuterY}, c,  { 0, 0 } });

        // Storing vertex as triangles to prepare renderering
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 3);

        indices.push_back(i + 0);
        indices.push_back(i + 3);
        indices.push_back(i + 2);

        i += 2;
    }

    indices[indices.size() - 4] = 1; // Looping back last triangle to first vertex

    indices[indices.size() - 2] = 1; // Looping back last triangle to first vertex
    indices[indices.size() - 1] = 0; // Looping back last triangle to first vertex

    // Render circle
    SDL_RenderGeometry(renderer, NULL, vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size());
}