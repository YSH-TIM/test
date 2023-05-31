#include "include/glad/glad.h"
#include "glfw3.h"

#include <iostream>
#include <thread>
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH=1920, SCR_HEIGHT=1080;

unsigned int texYId, texUId, texVId;
unsigned char *YUV_pt;

int width = 1920, height = 1080;
const char *yuv_path = "C:/Learning/CPlusPlus/OpenGL/Video/dolby-vision-slider-R3.yuv";
int nFrame;

void delay(int timeout_ms)
{
  auto start = std::chrono::system_clock::now();
  while (true)
  {
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    if (duration > timeout_ms)  break; 
  }
}

unsigned char * readYUV(const int &frame){
	FILE *fp=fopen(yuv_path,"rb");
    unsigned char * buffer;
	long size = width * height * 3 / 2 ;

    if(fp==NULL)
	{
	   printf("can't open the file");
	   exit(0);
	}

	fseek(fp, 0, SEEK_END);   //将文件指针移到文件末尾，偏移0个字节；fseek(fp,50L,0)；或fseek(fp,50L,SEEK_SET);解释：其作用是将位置指针移到离文件头50个字节处
	int Sum_pix = ftell(fp);     //得到文件尾相对于文件首的位移，即文件的总字节数
	rewind(fp);       //重置文件指针指向文件头部
	nFrame = Sum_pix / size; //得到视频文件的总帧数
 
	buffer = new unsigned char[size];
	// memset(buffer,'\0',size);

    fseek(fp,size*(frame%nFrame),SEEK_SET);
    fread(buffer,size,1,fp);
	fclose(fp);
	return buffer;
}

void reload_texture(unsigned char * buffer){
    glActiveTexture(GL_TEXTURE0);   
    glBindTexture ( GL_TEXTURE_2D, texYId );
    //采用替换性能能够有所优化
    glTexSubImage2D ( GL_TEXTURE_2D, 0, 0, 0, width, height,  GL_RED, GL_UNSIGNED_BYTE, buffer);
    // glTexImage2D ( GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture ( GL_TEXTURE_2D, texUId );
    glTexSubImage2D ( GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, buffer + width * height);
    // glTexImage2D ( GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, buffer + width * height);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture ( GL_TEXTURE_2D, texVId );
    glTexSubImage2D ( GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, buffer + width * height * 5 / 4);
    // glTexImage2D ( GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, buffer + width * height * 5 / 4 );

    delete buffer;
    buffer = NULL;
}

void loadYUV_texture(unsigned char * buffer){
    // buffer = readYUV(yuv_path);
    glGenTextures ( 1, &texYId );
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture ( GL_TEXTURE_2D, texYId );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);
    // glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures ( 1, &texUId );
    glActiveTexture(GL_TEXTURE1);
    glBindTexture ( GL_TEXTURE_2D, texUId );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, buffer + width * height);
    // glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures ( 1, &texVId );
    glActiveTexture(GL_TEXTURE2);
    glBindTexture ( GL_TEXTURE_2D, texVId );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, buffer + width * height * 5 / 4 );
    // glGenerateMipmap(GL_TEXTURE_2D);

    delete buffer;
    buffer = NULL;
}

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
        "gl_Position = vec4(aPos, 1.0f);\n"
        "ourColor = aColor;\n"
        "TexCoord = aTexCoord;\n"
    "}\n\0";

const char *fragmentShaderSource = 
        "#version 330 core                                     \n"
        "in vec3 ourColor;                                  \n"
        "in vec2 TexCoord;                                 \n"
        "out vec4 FragColor;                                   \n"
        "uniform sampler2D tex1;                        \n"
        "uniform sampler2D tex2;                        \n"
        "uniform sampler2D tex3;                        \n"
        "void main()                                         \n"
        "{                                                   \n"
        "	vec3 yuv;										\n"
        "   yuv.x = (texture(tex1, TexCoord).r - 16./255.)* 1.164;  	        \n"
        "   yuv.y = texture(tex2, TexCoord).r - 128./255.;	    \n"
        "   yuv.z = texture(tex3, TexCoord).r - 128./255.;	    \n"
        "	highp vec3 rgb = mat3( 1,          1,        1,					\n"
        "                           0,     -0.392, 	 2.017,					\n"
        "                           1.596, -0.813,        0 ) * yuv; 		\n"
        "	FragColor = vec4(rgb, 1.0);						\n"
        "}                                                  \n";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //顶点数据
    float vertices[] = {
        // ---- 位置 ----       ---- 颜色 ----      -- 纹理坐标 -
         1.0f,  1.0f, 0.0f,     1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 右上
         1.0f, -1.0f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // 右下
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // 左下
        -1.0f,  1.0f, 0.0f,     1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // 左上
    };

    // 索引数据
    unsigned int indices[] = {
        0, 1, 3, // 三角形一
        1, 2, 3  // 三角形二
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    // glVertexAttribPointer/
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 设置顶点颜色属性指针
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 设置顶点纹理坐标属性指针
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    loadYUV_texture(readYUV(0));

    glUseProgram(shaderProgram); 
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        int i=1000;
        // input
        processInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        while(i-- && !glfwWindowShouldClose(window)){
            reload_texture(readYUV(1000-i));

            // glGetUniformLocation 中的第二个参数指定GLSL中对应的纹理
            glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 0);
            glUniform1i(glGetUniformLocation(shaderProgram, "tex2"), 1);
            glUniform1i(glGetUniformLocation(shaderProgram, "tex3"), 2);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            glfwSwapBuffers(window);
            glfwPollEvents();
            delay(1000/24);
        }
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}