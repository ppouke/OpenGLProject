#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include<chrono>
#include<thread>


#include <Includes/stb_image.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <shader_s.h>
#include <camera.h>
#include <Model/model.h>

#include <Primitive/primitive.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);  
void mouse_callback(GLFWwindow*, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
void drawObjectWithOutline(Model& object, Shader& objectShader, Shader& outlineShader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
unsigned int loadCubemap(vector<std::string> faces);


    //display size

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;



//camera init

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));

float lastX = 400;
float lastY = 300;
bool firstMouse = true;




//light stuff

glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 ambientCol = glm::vec3(0.1f, 0.1f, 0.1f);


//timing

float deltaTime = 0.0f;
float lastFrame = 0.0f;



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    //glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //mouse input init
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
     

    

 

    //Shader Program ------------------------------------------

    Shader ourShader("Shaders/vShader.vs", "Shaders/fShader.fs");

    Shader unlitShader("Shaders/vShader.vs", "Shaders/unlitShader.fs");

    Shader depthShader("Shaders/vShader.vs", "Shaders/depthShader.fs");

    Shader grassShader("Shaders/vShader.vs", "Shaders/cutoffShader.fs");

    Shader screenShader("Shaders/bufferVertex.vs", "Shaders/bufferFrag.fs");

    Shader skyboxShader("Shaders/cubeVert.vs", "Shaders/cubeFrag.fs");


    //---------------------------Objects------------------------------------
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    

    screenShader.use();
    screenShader.setInt("screenTexture", 0);
   
    // -------------------------------------------------------------------------------------------


    //ourShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    //ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5)));

    //light calcs 
    //Make 3 point Lighting

    ourShader.use();

    ourShader.setVec3("dirLight.direction", 0.3f, -0.5f, 0.0f);
    ourShader.setVec3("dirLight.ambient", ambientCol);
    ourShader.setVec3("dirLight.diffuse",glm::vec3(1.0f)); 
    ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

    grassShader.use();
    grassShader.setVec3("dirLight.direction", 0.3f, -0.5f, 0.0f);
    grassShader.setVec3("dirLight.ambient", ambientCol);
    grassShader.setVec3("dirLight.diffuse", glm::vec3(1.0f));
    grassShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    

    //Enable to use pointlights
    ourShader.use();
    

    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f,  5.0f, -3.0f),
    glm::vec3(-3.0f, 5.0f, 3.0f),
    glm::vec3(3.0f, 5.0f, 3.0f),
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f),
        glm::vec3(0.5f),
        glm::vec3(0.2f),

    };

    for (int i = 0; i < 3; i++) {
        char buffer[64];
        sprintf_s(buffer, "pointLights[%i].position", i);
        ourShader.setVec3(buffer, pointLightPositions[i]);
        sprintf_s(buffer, "pointLights[%i].constant", i);
        ourShader.setFloat(buffer, 1.0f);
        sprintf_s(buffer, "pointLights[%i].linear", i);
        ourShader.setFloat(buffer, 0.09f);
        sprintf_s(buffer, "pointLights[%i].quadratic", i);
        ourShader.setFloat(buffer, 0.032f);

        sprintf_s(buffer, "pointLights[%i].ambient", i);
        ourShader.setVec3(buffer, ambientCol);
        sprintf_s(buffer, "pointLights[%i].diffuse", i);
        ourShader.setVec3(buffer, pointLightColors[i]);
        sprintf_s(buffer, "pointLights[%i].specular", i);
        ourShader.setVec3(buffer, 1.0f,1.0f,1.0f);

    }

    ourShader.setFloat("material.shininess", 6.0f);



    //---------------------Cube Map---------------------------


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    vector<std::string> faces{
        "Resources/Images/skybox/right.jpg",
        "Resources/Images/skybox/left.jpg",
        "Resources/Images/skybox/top.jpg",
        "Resources/Images/skybox/bottom.jpg",
        "Resources/Images/skybox/front.jpg",
        "Resources/Images/skybox/back.jpg"
    };

   
    

    unsigned int cubeVAO, cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    unsigned int cubeMap = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);




    //read models

    stbi_set_flip_vertically_on_load(true);
    Model backpack("Resources/Models/backpack/backpack.obj");

    //Model shadowCatcher("Resources/Models/shadowcatcher/Shadowcatcher.obj");

    //Create primitives
    Primitive plane(Plane);
    Primitive box(Box, "Resources/Images/container2.png", "Resources/Images/container2_specular.png");

    //Primitive grass(Plane, "Resources/Images/grass.png");
    //Primitive transparentWindow(Plane, "Resources/Images/blending_transparent_window.png");


    //create grass clump
    srand(static_cast <unsigned> (time(0))); //seed random
    glm::vec3 objPositions[5];
    float HI = 3.0f;
    float LO = -3.0f;
    
    for (int i = 0; i < 5; i++)
    {
        float x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
        float y = 0.25f;
        float z = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
        objPositions[i] = glm::vec3(x, y, z);
    }

    float objRotations[10];
    for (int i = 0; i < 10; i++)
    {
        objRotations[i] = static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / 360));

    }

    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < 5; i++)
    {
        float distance = glm::length(camera.Position - objPositions[i]);
        sorted[distance] = objPositions[i];
    }



    //enable depth test

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);


    // stencil testing

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //enable blending

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   //enable culling

    //glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);


    //_________________________________ FRAME BUFFER

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // gl buffer object -----------------------------------
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2* sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    //set projection matrix once (no zoom functionaloty) 
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
   

    //60fps = 16.67 ms

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //fps counters
        std::chrono::high_resolution_clock timer;
        auto start = timer.now();


        //input
        ProcessInput(window); 

        //first Pass-----------------------------------------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        // view transformations
        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


       
       
        // don't forget to enable shader before setting uniforms
        ourShader.use();


        // place ground 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));	
        //set model
        ourShader.setMat4("model", model);

        

        glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing the floor
        plane.Draw(ourShader);


        for (int i = 0; i < 5; i++)
        {
         
            model = glm::mat4(1.0f);
            model = glm::translate(model, objPositions[i]);
            model = glm::rotate(model, glm::radians(objRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f));

            ourShader.setMat4("model", model);
            box.Draw(ourShader);

        }


        //Draw skybox

         //cube view without transform
        glm::mat4 cubeView = glm::mat4(glm::mat3(view));
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", cubeView);
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        //second pass ----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
 


   
        glfwSwapBuffers(window);
        glfwPollEvents();



        //frame limiting
        auto stop = timer.now();
        auto delta = stop - start;
        auto totMs = chrono::milliseconds(8);
        std::this_thread::sleep_for(totMs - delta);
        auto secondStop = timer.now();
        typedef std::chrono::milliseconds ms;
        typedef std::chrono::duration<float> fsec;
        fsec fs =  secondStop - start;
        ms d = std::chrono::duration_cast<ms>(fs);
        

        //cout << d.count() << endl;
        
    }

    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

  
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }


}

void mouse_callback(GLFWwindow*, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);

   
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void drawObjectWithOutline(Model &object,Shader &objectShader, Shader &outlineShader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection) {

    //Stencil test

    outlineShader.setMat4("model", model);
    outlineShader.setMat4("view", view);
    outlineShader.setMat4("projection", projection);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    object.Draw(objectShader);


    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); //disable writing
    glDisable(GL_DEPTH_TEST);


    outlineShader.use();
    outlineShader.setVec3("col", 0.2f, 0.2f, 1.0f);

    
    model = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));	
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

    outlineShader.setMat4("model", model);
    outlineShader.setMat4("view", view);
    outlineShader.setMat4("projection", projection);

    object.Draw(outlineShader);

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);

}

unsigned int loadCubemap(vector<std::string> faces) {


    unsigned int textureCube;
    glGenTextures(1, &textureCube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap tex failed to load at path " << faces[i] << endl;
            stbi_image_free(data);
        }

    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureCube;
}






