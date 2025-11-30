#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Using a struct to represent vertices is convenient for later as it translates 1-to-1 with OpenGL
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};


class Mesh {

    private:
        // Create vectors to hold vertex data and index data, respectively
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;

        // Creating buffers and vertex array
        unsigned int _VBO, _EBO, _VAO, _instanceVBO, _colorsVBO;

        void setupMesh() {

            // Generate IDs for buffers and vertex arrays
            glGenBuffers(1, &_VBO);
            glGenBuffers(1, &_EBO);
            glGenVertexArrays(1, &_VAO);

            // Bind a vertex array; all subsequent VBOs and attributes will be bound to this object
            glBindVertexArray(_VAO);

            // Bind array buffer type and then vertex data
            glBindBuffer(GL_ARRAY_BUFFER, _VBO);
            glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

            // Bind element buffer type and then index data
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(0); // Enable position attribute at position 0

            // Normal Attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1); // Enable normal attribute at position 1

            // Unbind current VAO
            glBindVertexArray(0);

        }

    public:

        // Default constructor
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) : _vertices(vertices), _indices(indices) {

            // Load data onto GPU
            setupMesh();

        };

        void draw() {

            // Bind VAO and draw mesh
            glBindVertexArray(_VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(_indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

        }

};

#endif