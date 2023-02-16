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
#include <model.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);  
void mouse_callback(GLFWwindow*, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
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
     

    //read model

 



    //create buffer and assign vertices --------------------------------------

    

    //ligthVAO
    /*unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    */

    //texture 1 ---------------------------------------------

    
  
  
 
  


    //Shader Program ------------------------------------------

    Shader ourShader("Shaders/vShader.vs", "Shaders/fShader.fs");

    Shader lightShader("Shaders/vShader.vs", "Shaders/unlitShader.fs");

    
    
   
    // -------------------------------------------------------------------------------------------


    //ourShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    //ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5)));

    //light calcs 
    //Directional light

    ourShader.use();
    ourShader.setVec3("dirLight.direction", 0.3f, -0.5f, 0.0f);
    ourShader.setVec3("dirLight.ambient", ambientCol);
    ourShader.setVec3("dirLight.diffuse",glm::vec3(0.8f)); 
    ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    

    

    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(0.0f,0.0f,0.8f),
        glm::vec3(1.0f,1.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(0.0f,0.7f,0.0f)
    };

    for (int i = 0; i < 4; i++) {
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


    stbi_set_flip_vertically_on_load(true);
    Model backpack("Resources/Models/backpack/backpack.obj");

  


    //enable depth test

    glEnable(GL_DEPTH_TEST);

    //60fps = 16.67 ms

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //fps
        std::chrono::high_resolution_clock timer;
        auto start = timer.now();
        //input
        ProcessInput(window); 

        

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);

        //set constants
        ourShader.setFloat("material.shininess" ,32.0f);


        backpack.Draw(ourShader);
   
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





