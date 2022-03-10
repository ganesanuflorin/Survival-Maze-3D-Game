#include "lab_m1/tema2/tema2.h"
#include "lab_m1/Tema2/transform3D.h"


#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <time.h>
#include <stdlib.h>
#include <random>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


tema2::tema2()
{
}


tema2::~tema2()
{
}



void tema2::Init()
{
    renderCameraTarget = false;
    camera = new implemented::Camera();
    varE = 6.3f;
    w = 5;
    h = 5;
    H = 2 * h + 1;
    W = 2 * w + 1;
    grid = std::vector<std::vector<int>>(H, std::vector<int>(W, 1));
    forcedExit = 0;
    count = 2;
    numberofEnemies = 5;
    mouseAngle = 0;
    

    generate();

    //Am creat un vector de pozitii pentru locurile goale;
    std::vector<std::pair<int, int>> emptyPosition;
    int positionCounter = 0;
    for (int i = 0 ; i < H; i++) {
        for (int j = 0 ; j < W; j++) {
            if (grid[i][j] == 0) {
                emptyPosition.push_back({ i, j });
            }
        }
    }
    //Am luat o pozitie, random, libera din maze pentru erou;
    int varX;
    int varZ;
    if (emptyPosition.size() != 0) {
        int posIndex = rand() % emptyPosition.size();
        varX = emptyPosition[posIndex].first;
        varZ = emptyPosition[posIndex].second;
        emptyPosition.erase(emptyPosition.begin() + posIndex);
    }
    //Am luat 4 pozitii random pentru inamici;
    for (int i = 0; i < 4; i++) {
        int randEnemies = rand() % emptyPosition.size();
        grid[emptyPosition[randEnemies].first][emptyPosition[randEnemies].second] = 2;
    }

   
    playerX = 0;
    playerY = 0;
    playerZ = 0;
    float heroX = varX - 5.f;
    float heroY = 2.f;
    float heroZ = varZ - 5.f;
    camera->Set(glm::vec3(heroX, heroY + 0.5f, heroZ + camera->distanceToTarget), glm::vec3(heroX, heroY, heroZ), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(90), window->props.aspectRatio, 0.01f, 200.0f);

   /* for (int i = 0; i < H; i++) {
        cout << "" << endl;
        for (int j = 0; j < W; j++) {
            cout << grid[i][j] << " ";
        }
    }*/
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
  
    
}

std::vector<std::pair<int, int>> tema2::findNeighbors(int r, int c, int isWall)
{
    std::vector<std::pair<int, int>> ns;

    if (r > 1 && grid[r - 2][c] == isWall) {
        ns.push_back({ r - 2, c });
    }

    if (r < H - 2 && grid[r + 2][c] == isWall) {
        ns.push_back({ r + 2, c });
    }
    if (c > 1 && grid[r][c - 2] == isWall) {
        ns.push_back({ r, c - 2 });
    }
    if (c < W - 2 && grid[r][c + 2] == isWall) {
        ns.push_back({ r, c + 2 });
    }
   
    std::shuffle(std::begin(ns), std::end(ns), default_random_engine(engie));
    return ns;
}

//In functia aceasta imi creez iesiri din maze deoarece algoritmul de maze nu are iesiri;
bool tema2::doExit(int r, int c) 
{
    for (auto& exit : exits) {
        if (abs(exit.first - r) <= 1 || abs(exit.second) <= 1) {
            return true;
        }
    }
    return false;
}

void tema2::generate()
{
    srand((unsigned)time(NULL));

    int crow = 2 * (rand() % h) + 1;
    int ccol = 2 * (rand() % w) + 1;

    grid[crow][ccol] = 0;
    forcedExit++;

    while ((forcedExit < h * w)) {
        std::vector<std::pair<int, int>> neighbors = findNeighbors(crow, ccol, 1);

        if (neighbors.size() == 0) {
            std::vector<std::pair<int, int>> cell = findNeighbors(crow, ccol, 0);
           if (cell.size() != 0) {
                int rIndex = rand() % cell.size();
                crow = cell[rIndex].first;
                ccol = cell[rIndex].second;
            }
        }

        for (std::pair<int, int>& pairs : neighbors) {
            int nrow = pairs.first;
            int ncol = pairs.second;
            if (grid[nrow][ncol] == 1) {
                grid[abs((nrow + crow) / 2)][abs((ncol + ccol) / 2)] = 0;
                grid[nrow][ncol] = 0;
                if (count > 0) {
                    if (nrow == 1 && !doExit(0, ncol)) {
                        grid[0][ncol] = 0;
                        exits.push_back({ 0 , ncol });
                        count -= 1;
                    }
                    else if (nrow == H - 2 && !doExit(H - 1, ncol)) {
                        grid[H - 1][ncol] = 0;
                        exits.push_back({ H - 1, ncol });
                        count -= 1;
                    }
                    else if (ncol == 1 && !doExit(nrow, 0)) {
                        grid[nrow][0] = 0;
                        exits.push_back({ nrow, 0 });
                        count -= 1;
                    }
                    else if (ncol == W - 2 && !doExit(nrow, W - 1)) {
                        grid[nrow][W - 1] = 0;
                        exits.push_back({ nrow, W - 1 });
                        count -= 1;
                    }
                }
                forcedExit++;
                crow = nrow;
                ccol = ncol;
                break;
            }
            
        }
        

    }
}


void tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void tema2::Update(float deltaTimeSeconds)
{
    //Am setat coordonatele playerul la pozitia camerei;
    glm::vec3 aux = camera->GetTargetPosition();
    playerX = aux.x;
    playerY = aux.y - 1.f;
    playerZ = aux.z;
       
        //In if-ul de mai jos am creat playerul.
        if (!renderCameraTarget) {
            
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix *= transform3D::Translate(playerX,  playerY, playerZ);
                modelMatrix *= transform3D::RotateOY(mouseAngle);
                modelMatrix *= transform3D::Scale(0.4f, 0.5f, 0.2f);
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.f, 1.f, 0.53f));
                
                glm::mat4 auxMatrix = modelMatrix;
                auxMatrix *= transform3D::Translate(0.32f, - 0.71f, 0);
                auxMatrix *= transform3D::Scale(0.4f, 0.4f, 0.84f);
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], auxMatrix, glm::vec3(0.1f, 0.5f, 0.7f));

                auxMatrix = modelMatrix;
                auxMatrix *= transform3D::Translate(-0.32f, - 0.71f, 0);
                auxMatrix *= transform3D::Scale(0.4f, 0.4f, 0.84f);
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], auxMatrix, glm::vec3(0.1f, 0.5f, 0.7f));
            

                auxMatrix = modelMatrix;
                auxMatrix *= transform3D::Translate(0.68f, 0.3f, 0);
                auxMatrix *= transform3D::Scale(0.3f, 0.34f, 0.84f);
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], auxMatrix, glm::vec3(0.f, 1.f, 0.53f));

                auxMatrix = modelMatrix;
                auxMatrix *= transform3D::Translate(-0.68f, 0.3f, 0);
                auxMatrix *= transform3D::Scale(0.3f, 0.34f, 0.84f);
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], auxMatrix, glm::vec3(0.f, 1.f, 0.53f));
            
                auxMatrix = modelMatrix;
                auxMatrix *= transform3D::Translate(0.68f, - 0.07f, 0);
                auxMatrix *= transform3D::Scale(0.3f, 0.38f, 0.84f);
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], auxMatrix, glm::vec3(0.9f, 0.93f, 0.584f));
            
                auxMatrix = modelMatrix;
                auxMatrix *= transform3D::Translate(- 0.68f, - 0.07f, 0);
                auxMatrix *= transform3D::Scale(0.3f, 0.38f, 0.84f);
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], auxMatrix, glm::vec3(0.9f, 0.93f, 0.584f));

                auxMatrix = modelMatrix;
                auxMatrix *= transform3D::Translate(0, + 0.70f, 0);
                auxMatrix *= transform3D::Scale(0.45f, 0.3f, 0.9f);
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], auxMatrix, glm::vec3(0.9f, 0.93f, 0.584f));

        }
        //Am facut inamicii sa se miste in forma de 8;
        varE -= deltaTimeSeconds / 2;
        if (varE < -6.3f) {
            varE = 6.3f;
        }
        enemiesX = cos(varE);
        enemiesZ = sin(2 * varE) / 2;

        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {

                if (grid[i][j] == 1)
                {

                    {   
                        //Aici am creat maze -ul in functie de valoare 1 din matrice;
                        glm::mat4 modelMatrix = glm::mat4(1);
                        modelMatrix = glm::translate(modelMatrix, glm::vec3(-5.f + i, 0.f, -5.f  + j));
                        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 3.f, 1.f));
                        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0.5, 0));
                    }
                }
                
                if (grid[i][j] == 2) {
                    {
                        //Aici am creat inamicii la pozitia random gasita in init;
                        glm::mat4 modelMatrix = glm::mat4(1);
                        modelMatrix = glm::translate(modelMatrix, glm::vec3(-5.f + enemiesX + i, 0.3f,-5.f + enemiesZ + j));
                        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f, 0.5f, 0.75f));
                        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.7f, 0.3f, 0.3f));
                    }
                }
            }
        }
 
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.01f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.2f, 0.3f, 1.f));
    }

    
}


void tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);


    auto objectColorLocation = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(objectColorLocation, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    //// Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}



void tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 2;


        // Control light position using on W, A, S, D, E, Q
        if (window->KeyHold(GLFW_KEY_W))
        {
            camera->MoveForward(+deltaTime * cameraSpeed);
            playerX = +deltaTime * cameraSpeed;
        }
        if (window->KeyHold(GLFW_KEY_A))
        {
            camera->TranslateRight(-deltaTime * cameraSpeed);
            playerZ = -deltaTime * cameraSpeed;
        }
        if (window->KeyHold(GLFW_KEY_S))
        {
            camera->MoveForward(-deltaTime * cameraSpeed);
            playerX = -deltaTime * cameraSpeed;
        }
        if (window->KeyHold(GLFW_KEY_D))
        {
            camera->TranslateRight(deltaTime * cameraSpeed);
            playerZ = +deltaTime * cameraSpeed;
        }
        if (window->KeyHold(GLFW_KEY_E))
        {

            camera->TranslateUpward(deltaTime * cameraSpeed);
        }
        if (window->KeyHold(GLFW_KEY_Q))
        {
            camera->TranslateUpward(-deltaTime * cameraSpeed);

        }

}


void tema2::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
        if (!renderCameraTarget) {
            camera->MoveForward(-2.f);
        }
        else
        {
            camera->MoveForward(2.f);
        }
        
    }
}


void tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;
        mouseAngle += sensivityOY * deltaX;

        if (!renderCameraTarget) {
            
            camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * -deltaX);

        }

        if (renderCameraTarget) {
            
            camera->RotateThirdPerson_OX(sensivityOX * -deltaY);
            camera->RotateThirdPerson_OY(sensivityOY * -deltaX);

        }
    }
}


void tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void tema2::OnWindowResize(int width, int height)
{
}
