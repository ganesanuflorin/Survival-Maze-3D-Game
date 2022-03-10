#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"
#include "lab_m1/Tema2/tema_camera.h"
#include <random>

namespace m1
{
    class tema2 : public gfxc::SimpleScene
    {
    public:
        tema2();
        ~tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        std::vector<std::pair<int, int>> findNeighbors(int r, int c, int isWall);
        void generate();
        bool doExit(int r, int c);

    protected:
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        float mouseAngle;
        float playerX;
        float playerY;
        float playerZ;
        float enemiesX;
        float enemiesZ;
        int row;
        int column;
        int h;
        int w;
        int H;
        int W;
        float varE;
        int numberofEnemies;
        int count;
        int forcedExit;
        std::vector<std::vector<int>> grid;
        std::vector<std::pair<int, int>> exits;
        std::default_random_engine engie = std::default_random_engine();

    };

}   // namespace m1
