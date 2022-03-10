#pragma once

#include "utils/glm_utils.h"


namespace transform3D
{
    // Translate matrix
    inline glm::mat4 Translate(float translateX, float translateY, float translateZ)
    {
        // TODO(student): Implement the translation matrix
        return glm::mat4(
            1.f, 0.f, 0.f, 0.f,     // coloana 1 in memorie 
            0.f, 1.f, 0.f, 0.f,     // coloana 2 in memorie 
            0.f, 0.f, 1.f, 0.f,     // coloana 3 in memorie 
            translateX, translateY, translateZ, 1.f);    // coloana 4 in memorie 

    }

    // Scale matrix
    inline glm::mat4 Scale(float scaleX, float scaleY, float scaleZ)
    {
        // TODO(student): Implement the scaling matrix
        return glm::mat4(
            scaleX, 0.f, 0.f, 0.f,
            0.f, scaleY, 0.f, 0.f,
            0.f, 0.f, scaleZ, 0.f,
            0.f, 0.f, 0.f, 1.f);
    }

    // Rotate matrix relative to the OZ axis
    inline glm::mat4 RotateOZ(float radians)
    {
        auto sine = glm::sin(radians);
        auto cosine = glm::cos(radians);
        // TODO(student): Implement the rotation matrix

        return glm::mat4(
            cosine, -sine, 0.f, 0.f,
            sine, cosine, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f);
    }

    // Rotate matrix relative to the OY axis
    inline glm::mat4 RotateOY(float radians)
    {   
        auto sine = glm::sin(radians);
        auto cosine = glm::cos(radians);
        // TODO(student): Implement the rotation matrix
        return glm::mat4(
            cosine, 0.f, sine, 0.f,
            0.f, 1.f, 0.f, 0.f,
            -sine, 0.f, cosine, 0.f,
            0.f, 0.f, 0.f, 1.f);

    }

    // Rotate matrix relative to the OX axis
    inline glm::mat4 RotateOX(float radians)
    {
        auto sine = glm::sin(radians);
        auto cosine = glm::cos(radians);
        // TODO(student): Implement the rotation matrix
        return glm::mat4(
            1.f, 0.f, 0.f, 0.f,
            0.f, cosine, -sine, 0.f,
            0.f, sine, cosine, 0.f,
            0.f, 0.f, 0.f, 1.f);

    }
}   // namespace transform3D
