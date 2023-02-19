#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include<chrono>
#include<thread>


#include <Includes/stb_image.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    //display size

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;



//camera init

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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

    
    
   
    // -------------------------------------------------------------------------------------------


    //ourShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    //ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5)));

    //light calcs 
    //Make 3 point Lighting

    ourShader.use();

    ourShader.setVec3("dirLight.direction", 0.3f, -0.5f, 0.0f);
    ourShader.setVec3("dirLight.ambient", ambientCol);
    ourShader.setVec3("dirLight.diffuse",glm::vec3(0.0f)); 
    ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    

    

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


    //read models

    stbi_set_flip_vertically_on_load(true);
    Model backpack("Resources/Models/backpack/backpack.obj");

    //Model shadowCatcher("Resources/Models/shadowcatcher/Shadowcatcher.obj");

    //Create primitives
    Primitive plane(Plane);
    Primitive box(Box);


    //enable depth test

    glEnable(GL_DEPTH_TEST);


    // stencil testing

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

   

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

        

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));	

        //set constants
        ourShader.setFloat("material.shininess" ,6.0f);
        ourShader.setMat4("model", model);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing the floor
        plane.Draw(ourShader);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 3.0f));
        model = glm::scale(model, glm::vec3(0.5f));

        ourShader.setMat4("model", model);

        box.Draw(ourShader);


        model = glm::mat4(1.0f);


        ourShader.setFloat("material.shininess", 32.0f);
        ourShader.setMat4("model", model);
        backpack.Draw(ourShader);
       //drawObjectWithOutline(backpack, ourShader, unlitShader, model, view, projection);

        
   
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

    ///xxxx Ulla
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




