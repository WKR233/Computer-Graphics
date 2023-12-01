// Hand Example
// Author: Yi Kangrui <yikangrui@pku.edu.cn>

//#define DIFFUSE_TEXTURE_MAPPING

#include "gl_env.h"

#include <cstdlib>
#include <cstdio>
#include <config.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#define Lenth 2560.0
#define Width 1440.0

#define SENSI 0.001
#define MOVEPERIOD 3

#include <iostream>

#include "skeletal_mesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace SkeletalAnimation {
    const char *vertex_shader_330 =
            "#version 330 core\n"
            "const int MAX_BONES = 100;\n"
            "uniform mat4 u_bone_transf[MAX_BONES];\n"
            "uniform mat4 u_mvp;\n"
            "layout(location = 0) in vec3 in_position;\n"
            "layout(location = 1) in vec2 in_texcoord;\n"
            "layout(location = 2) in vec3 in_normal;\n"
            "layout(location = 3) in ivec4 in_bone_index;\n"
            "layout(location = 4) in vec4 in_bone_weight;\n"
            "out vec2 pass_texcoord;\n"
            "void main() {\n"
            "    float adjust_factor = 0.0;\n"
            "    for (int i = 0; i < 4; i++) adjust_factor += in_bone_weight[i] * 0.25;\n"
            "    mat4 bone_transform = mat4(1.0);\n"
            "    if (adjust_factor > 1e-3) {\n"
            "        bone_transform -= bone_transform;\n"
            "        for (int i = 0; i < 4; i++)\n"
            "            bone_transform += u_bone_transf[in_bone_index[i]] * in_bone_weight[i] / adjust_factor;\n"
            "     }\n"
            "    gl_Position = u_mvp * bone_transform * vec4(in_position, 1.0);\n"
            "    pass_texcoord = in_texcoord;\n"
            "}\n";

    const char *fragment_shader_330 =
            "#version 330 core\n"
            "uniform sampler2D u_diffuse;\n"
            "in vec2 pass_texcoord;\n"
            "out vec4 out_color;\n"
            "void main() {\n"
            #ifdef DIFFUSE_TEXTURE_MAPPING
            "    out_color = vec4(texture(u_diffuse, pass_texcoord).xyz, 1.0);\n"
            #else
            "    out_color = vec4(pass_texcoord, 0.0, 1.0);\n"
            #endif
            "}\n";
}

int input_status=0;

float passed_time_after_0=0,passed_time_after_1=0;
float passed_time_after_2=0,passed_time_after_3=0;
float lastX=Lenth/2.0f,lastY=Width/2.0f;
float yaw=M_PI/2.0,pitch=0.0f,roll=0.0f;
float fov=45;
float start_time_of_AtoB=0,start_time_of_BtoA=0;

bool camera_state=true;
bool AtoB=false,BtoA=false;

glm::vec3 cameraPos=glm::vec3(0.0f,10.0f,-30.0f);
glm::vec3 cameraFront=glm::vec3(0.0f,0.0f,1.0f);
glm::vec3 cameraUp=glm::vec3(0.0f,1.0f,0.0f);
glm::vec3 A_Position=glm::vec3(12.5f,14.318f,12.5f);
glm::vec3 B_Position=glm::vec3(9.545,6.818,-18.455f);
glm::vec3 A_EulerAngle=glm::vec3(-18.409f,0.0f,0.0f);
glm::vec3 B_EulerAngle=glm::vec3(6.818f,242.727f,0.0f);

glm::mat4 moveview;

static void error_callback(int error, const char *description);
static void cursor_callback(GLFWwindow *window,double xpos,double ypos);
static void scroll_callback(GLFWwindow *window,double xoffset,double yoffset);
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void cursor_callback(GLFWwindow *window,double xpos,double ypos)
{
    float offsetX=xpos-lastX;
    float offsetY=ypos-lastY;
    lastX=xpos;
    lastY=ypos;
    
    offsetX*=SENSI;
    offsetY*=SENSI;

    yaw-=offsetX;
    pitch+=offsetY;
    if(pitch>0.99*M_PI/2.0)
        pitch=0.99*M_PI/2.0;
    if(pitch<-0.99*M_PI/2.0)
        pitch=-0.99*M_PI/2.0;

    glm::vec3 tmpfront;
    tmpfront.x = cos(yaw)*cos(pitch);
    tmpfront.y = sin(pitch);
    tmpfront.z = sin(yaw)*cos(pitch);

    cameraFront = glm::normalize(tmpfront);
}

static void scroll_callback(GLFWwindow *window,double xoffset,double yoffset)
{
    roll+=yoffset*0.01;
    cameraUp=glm::fvec3(sin(roll),cos(roll),0.0);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if(glfwGetKey(window,GLFW_KEY_0)==GLFW_PRESS)
    {
        passed_time_after_0=(float)glfwGetTime();
        input_status=0;
    }
    if(glfwGetKey(window,GLFW_KEY_1)==GLFW_PRESS)
    {
        passed_time_after_1=(float)glfwGetTime();
        input_status=1;
    }
    if(glfwGetKey(window,GLFW_KEY_2)==GLFW_PRESS)
    {
        passed_time_after_2=(float)glfwGetTime();
        input_status=2;
    }
    if(glfwGetKey(window,GLFW_KEY_3)==GLFW_PRESS)
    {
        passed_time_after_3=(float)glfwGetTime();
        input_status=3;
    }
    float cameraSpeed=0.1f;
    if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS)
        cameraPos+=cameraSpeed*cameraFront;
    if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS)
        cameraPos-=cameraSpeed*cameraFront;
    if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS)
        cameraPos-=glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
    if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS)
        cameraPos+=glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
    if(glfwGetKey(window,GLFW_KEY_T)==GLFW_PRESS)
        cameraPos+=cameraSpeed*cameraUp;
    if(glfwGetKey(window,GLFW_KEY_G)==GLFW_PRESS)
        cameraPos-=cameraSpeed*cameraUp;
    if(glfwGetKey(window,GLFW_KEY_R)==GLFW_PRESS)
    {
        cameraUp=glm::vec3(0.0f,1.0f,0.0f);
        cameraPos=glm::vec3(0.0f,10.0f,-30.0f);
        roll=0;
    }
    if(glfwGetKey(window,GLFW_KEY_Q)==GLFW_PRESS)
    {
        camera_state=!camera_state;
        if(camera_state)
        {
            glfwSetCursorPos(window,Lenth/2,Width/2);
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
            glfwSetCursorPosCallback(window,cursor_callback);
        }
        else
        {
            glfwSetCursorPosCallback(window,nullptr);
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
        }
    }
    if(((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS)
        ||(glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL)==GLFW_PRESS))
        &&(glfwGetKey(window,GLFW_KEY_MINUS)==GLFW_PRESS))
        fov=(fov+1>60.0)?60.0:(fov+1);
    if(((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS)
        ||(glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL)==GLFW_PRESS))
        &&(glfwGetKey(window,GLFW_KEY_EQUAL)==GLFW_PRESS))
        fov=(fov-1<1.0)?1.0:(fov-1);
}

void draw_ui()
{
    ImGui::Text("A Position");
    ImGui::SliderFloat("A Position x",&A_Position.x,-30.0f,30.0f);
    ImGui::SliderFloat("A Position y",&A_Position.y,-30.0f,30.0f);
    ImGui::SliderFloat("A Position z",&A_Position.z,-30.0f,30.0f);
    ImGui::Text("A EulerAngle");
    ImGui::SliderFloat("A yaw",&A_EulerAngle.x,-90.0f,90.0f);
    ImGui::SliderFloat("A pitch",&A_EulerAngle.y,0.0f,360.0f);
    ImGui::Text("B Position");
    ImGui::SliderFloat("B Position x",&B_Position.x,-30.0f,30.0f);
    ImGui::SliderFloat("B Position y",&B_Position.y,-30.0f,30.0f);
    ImGui::SliderFloat("B Position z",&B_Position.z,-30.0f,30.0f);
    ImGui::Text("B EulerAngle");
    ImGui::SliderFloat("B yaw",&B_EulerAngle.x,-90.0f,90.0f);
    ImGui::SliderFloat("B pitch",&B_EulerAngle.y,0.0f,360.0f);
    if(ImGui::Button("A->B"))
    {
        AtoB=true;
        start_time_of_AtoB=glfwGetTime();
    }
    if(ImGui::Button("B->A"))
    {
        BtoA=true;
        start_time_of_BtoA=glfwGetTime();
    }
}

int main(int argc, char *argv[]) {
    GLFWwindow *window;
    GLuint vertex_shader, fragment_shader, program;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ // for macos
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(Lenth, Width, "OpenGL output", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 150");

    glfwSetCursorPosCallback(window,cursor_callback);
    glfwSetKeyCallback(window,key_callback);
    glfwSetScrollCallback(window,scroll_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit()!=GLEW_OK)
        exit(EXIT_FAILURE);

    vertex_shader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader,1,&SkeletalAnimation::vertex_shader_330,NULL);
    glCompileShader(vertex_shader);

    fragment_shader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader,1,&SkeletalAnimation::fragment_shader_330,NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int linkStatus;
    if (glGetProgramiv(program, GL_LINK_STATUS, &linkStatus), linkStatus == GL_FALSE)
        std::cout << "Error occured in glLinkProgram()" << std::endl;

    SkeletalMesh::Scene &sr = SkeletalMesh::Scene::loadScene("Hand", DATA_DIR"/Hand.fbx");
    if (&sr == &SkeletalMesh::Scene::error)
        std::cout << "Error occured in loadMesh()" << std::endl;

    sr.setShaderInput(program, "in_position", "in_texcoord", "in_normal", "in_bone_index", "in_bone_weight");

    float passed_time;
    SkeletalMesh::SkeletonModifier modifier;

    glEnable(GL_DEPTH_TEST);
    glfwSetCursorPos(window,Lenth/2,Width/2);
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        passed_time = (float) glfwGetTime();
        // --- You may edit below ---

        // Example: Rotate the hand
        // * turn around every 4 seconds
        // * target = metacarpals
        // * rotation axis = (1, 0, 0)
        /**********************************************************************************\
        *
        * To animate fingers, modify modifier["HAND_SECTION"] each frame,
        * where HAND_SECTION can only be one of the bone names in the Hand's Hierarchy.
        *
        * A virtual hand's structure is like this: (slightly DIFFERENT from the real world)
        *    5432 1
        *    ....        1 = thumb           . = fingertip
        *    |||| .      2 = index finger    | = distal phalange
        *    $$$$ |      3 = middle finger   $ = intermediate phalange
        *    #### $      4 = ring finger     # = proximal phalange
        *    OOOO#       5 = pinky           O = metacarpals
        *     OOO
        * (Hand in the real world -> https://en.wikipedia.org/wiki/Hand)
        *
        * From the structure we can infer the Hand's Hierarchy:
        *    - metacarpals
        *        - thumb_proximal_phalange
        *            - thumb_intermediate_phalange
        *                - thumb_distal_phalange
        *                    - thumb_fingertip
        *        - index_proximal_phalange
        *            - index_intermediate_phalange
        *                - index_distal_phalange
        *                    - index_fingertip
        *        - middle_proximal_phalange
        *            - middle_intermediate_phalange
        *                - middle_distal_phalange
        *                    - middle_fingertip
        *        - ring_proximal_phalange
        *            - ring_intermediate_phalange
        *                - ring_distal_phalange
        *                    - ring_fingertip
        *        - pinky_proximal_phalange
        *            - pinky_intermediate_phalange
        *                - pinky_distal_phalange
        *                    - pinky_fingertip
        *
        * Notice that modifier["HAND_SECTION"] is a local transformation matrix,
        * where (1, 0, 0) is the bone's direction, and apparently (0, 1, 0) / (0, 0, 1)
        * is perpendicular to the bone.
        * Particularly, (0, 0, 1) is the rotation axis of the nearer joint.
        *
        \**********************************************************************************/

        // Example: Animate the index finger
        // * period = 2.4 seconds
        float period = 2.4f;
        float time_in_period = fmin(passed_time, period);
        // * angle: 0 -> PI/3 -> 0
        float finger_angle = abs(time_in_period / period) * (M_PI / 2.0);
        float thumb_angle_1=finger_angle/3.0;
        float thumb_angle_2=finger_angle/2.0;
        float finger_angle_3 = finger_angle/4.0;
        float metacarpals_angle = time_in_period * (M_PI / 4.8f);
        float metacarpals_angle_3 = time_in_period * (M_PI / 2.4f);
        // * target = proximal phalange of the index
        // * rotation axis = (0, 0, 1)
        /*
        float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view;
        if(camZ>0)
            view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
        else
            view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0));
        modifier["metacarpals"] = view;
        */
        if(input_status==0)
        {
            modifier["metacarpals"] = glm::mat4(1.0f);
            modifier["thumb_proximal_phalange"] = modifier["metacarpals"];
            modifier["thumb_intermediate_phalange"] = modifier["metacarpals"];
            modifier["thumb_distal_phalange"] = modifier["metacarpals"];
            modifier["thumb_fingertip"] = modifier["metacarpals"];
            modifier["index_proximal_phalange"] = modifier["metacarpals"];
            modifier["index_intermediate_phalange"] = modifier["metacarpals"];
            modifier["index_distal_phalange"] = modifier["metacarpals"];
            modifier["index_fingertip"] = modifier["metacarpals"];
            modifier["middle_proximal_phalange"] = modifier["metacarpals"];
            modifier["middle_intermediate_phalange"] = modifier["metacarpals"];
            modifier["middle_distal_phalange"] = modifier["metacarpals"];
            modifier["middle_fingertip"] = modifier["metacarpals"];
            modifier["ring_proximal_phalange"] = modifier["metacarpals"];
            modifier["ring_intermediate_phalange"] = modifier["metacarpals"];
            modifier["ring_distal_phalange"] = modifier["metacarpals"];
            modifier["ring_fingertip"] = modifier["metacarpals"];
            modifier["pinky_proximal_phalange"] = modifier["metacarpals"];
            modifier["pinky_intermediate_phalange"] = modifier["metacarpals"];
            modifier["pinky_distal_phalange"] = modifier["metacarpals"];
            modifier["pinky_fingertip"] = modifier["metacarpals"];
        }
        if(input_status==1)
        {
            time_in_period=fmin(passed_time-passed_time_after_1,period);
            finger_angle = abs(time_in_period / period) * (M_PI / 2.0);
            metacarpals_angle = time_in_period * (M_PI / 4.8f);
            thumb_angle_1=finger_angle/3.0;
            modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), metacarpals_angle, glm::fvec3(0.0, 1.0, 0.0));
            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle_1,glm::fvec3(0.0, 0.0, -1.0));
        }
        if(input_status==2)
        {
            time_in_period=fmin(passed_time-passed_time_after_2,period);
            finger_angle = abs(time_in_period / period) * (M_PI / 2.0);
            thumb_angle_2=finger_angle/2.0;
            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle_2,glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle_2,glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle_2,glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle_2,glm::fvec3(0.0, 0.0, 1.0));
        }
        if(input_status==3)
        {
            time_in_period=fmin(passed_time-passed_time_after_3,period);
            finger_angle = abs(time_in_period / period) * (M_PI / 2.0);
            metacarpals_angle_3 = time_in_period * (M_PI / 2.4f);
            finger_angle_3 = finger_angle/4.0;
            modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), metacarpals_angle_3, glm::fvec3(1.0, 0.0, 0.0));
            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle,glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), finger_angle_3,glm::fvec3(0.0,1.0,0.0));
        }
        if(AtoB)
        {
            float timeratio=0.5f-cos((passed_time-start_time_of_AtoB)*M_PI/MOVEPERIOD)/2;
            glm::quat Iniquat=glm::quat(A_EulerAngle);
            glm::quat Endquat=glm::quat(B_EulerAngle);
            glm::quat mixed=glm::slerp(Iniquat,Endquat,timeratio);
            glm::vec3 curpos=glm::mix(A_Position,B_Position,timeratio);
            moveview=glm::mat4_cast(mixed)*glm::translate(glm::fmat4(1.0f),-curpos);
            if(passed_time-start_time_of_AtoB>=MOVEPERIOD)
                AtoB=false;
        }
        if(BtoA)
        {
            float timeratio=0.5f-cos((passed_time-start_time_of_BtoA)*M_PI/MOVEPERIOD)/2;
            glm::quat Iniquat=glm::quat(B_EulerAngle);
            glm::quat Endquat=glm::quat(A_EulerAngle);
            glm::quat mixed=glm::slerp(Iniquat,Endquat,timeratio);
            glm::vec3 curpos=glm::mix(B_Position,A_Position,timeratio);
            moveview=glm::mat4_cast(mixed)*glm::translate(glm::fmat4(1.0f),-curpos);
            if(passed_time-start_time_of_BtoA>=MOVEPERIOD)
                BtoA=false;
        }

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glClearColor(0.5, 0.5, 0.5, 1.0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glm::fmat4 mvp = glm::perspective(glm::radians(fov),float(Lenth/Width), 0.1f, 100.0f)
                         *
                         glm::lookAt(cameraPos,cameraFront+cameraPos,cameraUp);
        if(AtoB||BtoA)
            mvp=glm::perspective(glm::radians(80.0f),float(Lenth/Width), 0.1f, 100.0f)*moveview;
        glUniformMatrix4fv(glGetUniformLocation(program, "u_mvp"), 1, GL_FALSE, (const GLfloat *) &mvp);
        glUniform1i(glGetUniformLocation(program, "u_diffuse"), SCENE_RESOURCE_SHADER_DIFFUSE_CHANNEL);
        SkeletalMesh::Scene::SkeletonTransf bonesTransf;
        sr.getSkeletonTransform(bonesTransf, modifier);
        if (!bonesTransf.empty())
            glUniformMatrix4fv(glGetUniformLocation(program, "u_bone_transf"), bonesTransf.size(), GL_FALSE,
                               (float *) bonesTransf.data());
        sr.render();

        draw_ui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    SkeletalMesh::Scene::unloadScene("Hand");

    ImGui::DestroyContext();
    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}