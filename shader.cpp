#include "include/glad/glad.h"
#include "glfw3.h"

char vShaderStr[] =
        "#version 300 es                            \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec2 a_texCoord;   \n"
        "out vec2 v_texCoord;                       \n"
        "void main()                                \n"
        "{                                          \n"
        "   gl_Position = a_position;               \n"
        "   v_texCoord = a_texCoord;                \n"
        "}                                          \n";

char fShaderStr[] =
        "#version 300 es                                     \n"
        "precision mediump float;                            \n"
        "in vec2 v_texCoord;                                 \n"
        "layout(location = 0) out vec4 outColor;             \n"
        "uniform sampler2D y_texture;                        \n"
        "uniform sampler2D uv_texture;                        \n"
        "void main()                                         \n"
        "{                                                   \n"
        "	vec3 yuv;										\n"
        "   yuv.x = texture(y_texture, v_texCoord).r;  	\n"
        "   yuv.y = texture(uv_texture, v_texCoord).a-0.5;	\n"
        "   yuv.z = texture(uv_texture, v_texCoord).r-0.5;	\n"
        "	highp vec3 rgb = mat3( 1,       1,       	1,					\n"
        "               0, 		-0.344, 	1.770,					\n"
        "               1.403,  -0.714,       0) * yuv; 			\n"
        "	outColor = vec4(rgb, 1);						\n"
        "}                                                  \n";

/*顶点着色器代码*/
static const char g_pGLVS[] =              ///<普通纹理渲染顶点着色器
{
    "precision mediump float;"
    "attribute vec4 position; "
    "attribute vec4 texCoord; "
    "varying vec4 pp; "
 
    "void main() "
    "{ "
    "    gl_Position = position; "
    "    pp = texCoord; "
    "} "
};
 
/*像素着色器代码*/
const char* g_pGLFS =              ///<YV12片段着色器
{
    "precision mediump float;"
    "varying vec4 pp; "
    "uniform sampler2D Ytexture; "
    "uniform sampler2D Utexture; "
    "uniform sampler2D Vtexture; "
    "void main() "
    "{ "
    "    float r,g,b,y,u,v; "
 
    "    y=texture2D(Ytexture, pp.st).r; "
    "    u=texture2D(Utexture, pp.st).r; "
    "    v=texture2D(Vtexture, pp.st).r; "
 
    "    y=1.1643*(y-0.0625); "
    "    u=u-0.5; "
    "    v=v-0.5; "
 
    "    r=y+1.5958*v; "
    "    g=y-0.39173*u-0.81290*v; "
    "    b=y+2.017*u; "
    "    gl_FragColor=vec4(r,g,b,1.0); "
    "} "
};

GLbyte vShaderStr[] =  
    "attribute vec4 vPosition;    \n"
    "attribute vec2 a_texCoord;	\n"
    "varying vec2 tc;		\n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = vPosition;  \n"
    "	  tc = a_texCoord;	\n"
    "}                            \n";
   
GLbyte fShaderStr[] =  
    "precision mediump float;\n"
    "uniform sampler2D tex_y;					\n"
    "uniform sampler2D tex_u;					\n"
    "uniform sampler2D tex_v;					\n"
    "varying vec2 tc;							\n"
    "void main()                                  \n"
    "{                                            \n"
    "  vec4 c = vec4((texture2D(tex_y, tc).r - 16./255.) * 1.164);\n"
    "  vec4 U = vec4(texture2D(tex_u, tc).r - 128./255.);\n"
    "  vec4 V = vec4(texture2D(tex_v, tc).r - 128./255.);\n"
    "  c += V * vec4(1.596, -0.813, 0, 0);\n"
    "  c += U * vec4(0, -0.392, 2.017, 0);\n"
    "  c.a = 1.0;\n"
    "  gl_FragColor = c;\n"
    "}                                            \n";


const GLchar *strVertexShaderRgb2Yuv =
    "attribute vec4 position;\n"
    "attribute vec4 inputTextureCoordinate;\n"
    "\n"
    "varying vec2 textureCoordinate;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = position;\n"
    "    textureCoordinate = inputTextureCoordinate.xy;\n"
    "}\n";

const GLchar *strFragmentShaderRgb2Yuv =
    "precision highp float;\n"
    "\n"
    "varying vec2 textureCoordinate;\n"
    "\n"
    "uniform sampler2D inputImageTexture;\n"
    "uniform float uWidth;\n"
    "uniform float uHeight;\n"
    "\n"
    "float cY(float x, float y)\n"
    "{\n"
    "    vec4 c = texture2D(inputImageTexture, vec2(x, y));\n"
    "    return c.r * 0.257 + c.g * 0.504 + c.b * 0.098 + 0.0625;\n"
    "}\n"
    "\n"
    "vec4 cC(float x, float y, float dx, float dy)\n"
    "{\n"
    "    vec4 c0 = texture2D(inputImageTexture, vec2(x, y));\n"
    "    vec4 c1 = texture2D(inputImageTexture, vec2(x + dx, y));\n"
    "    vec4 c2 = texture2D(inputImageTexture, vec2(x, y + dy));\n"
    "    vec4 c3 = texture2D(inputImageTexture, vec2(x + dx, y + dy));\n"
    "    return (c0 + c1 + c2 + c3) / 4.0;\n"
    "}\n"
    "\n"
    "float cU(float x, float y, float dx, float dy)\n"
    "{\n"
    "    vec4 c = cC(x, y, dx, dy);\n"
    "    return -0.148 * c.r - 0.291 * c.g + 0.439 * c.b + 0.5000;\n"
    "}\n"
    "\n"
    "float cV(float x, float y, float dx, float dy)\n"
    "{\n"
    "    vec4 c = cC(x, y, dx, dy);\n"
    "    return 0.439 * c.r - 0.368 * c.g - 0.071 * c.b + 0.5000;\n"
    "}\n"
    "\n"
    "vec4 calculateY(float x, float y, float dx, float dy)\n"
    "{\n"
    "    vec4 c = vec4(0.0);\n"
    "    c[0] = cY(x, y);\n"
    "    c[1] = cY(x + 1.0 * dx, y);\n"
    "    c[2] = cY(x + 2.0 * dx, y);\n"
    "    c[3] = cY(x + 3.0 * dx, y);\n"
    "    return c;\n"
    "}\n"
    "\n"
    "vec4 calculateU(float x, float y, float dx, float dy)\n"
    "{\n"
    "    vec4 c = vec4(0.0);\n"
    "    c[0] = cU(x, y, dx, dy);\n"
    "    c[1] = cU(x + 2.0 * dx, y, dx, dy);\n"
    "    c[2] = cU(x + 4.0 * dx, y, dx, dy);\n"
    "    c[3] = cU(x + 6.0 * dx, y, dx, dy);\n"
    "    return c;\n"
    "}\n"
    "\n"
    "vec4 calculateV(float x, float y, float dx, float dy)\n"
    "{\n"
    "    vec4 c = vec4(0.0);\n"
    "    c[0] = cV(x, y, dx, dy);\n"
    "    c[1] = cV(x + 2.0 * dx, y, dx, dy);\n"
    "    c[2] = cV(x + 4.0 * dx, y, dx, dy);\n"
    "    c[3] = cV(x + 6.0 * dx, y, dx, dy);\n"
    "    return c;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    if (textureCoordinate.y > 0.375) {\n"
    "       return;\n"
    "    }\n"
    "\n"
    "    float y4xSum = uWidth * uHeight / 4.0;\n"
    "    float u4xSum = uWidth * uHeight / 16.0;\n"
    "    float v4xSum = uWidth * uHeight / 16.0;\n"
    "    float yuv4xOffset = floor(textureCoordinate.y * uHeight) * uWidth + floor(textureCoordinate.x * uWidth);\n"
    "\n"
    "    if (yuv4xOffset < y4xSum) {\n"
    "        float yOffset = yuv4xOffset * 4.0 + 1.0;\n"
    "        float yX = floor(mod(yOffset, uWidth));\n"
    "        float yY = floor(yOffset / uWidth);\n"
    "        yY = uHeight - yY;\n"
    "        gl_FragColor = calculateY(yX / uWidth, yY / uHeight, 1.0 / uWidth, 1.0 / uHeight);\n"
    "    } else if (yuv4xOffset < (y4xSum + u4xSum)) {\n"
    "        float uOffset = (yuv4xOffset - y4xSum) * 4.0 + 1.0;\n"
    "        float uX = floor(mod(uOffset, uWidth / 2.0)) * 2.0;\n"
    "        float uY = floor(uOffset / (uWidth / 2.0)) * 2.0;\n"
    "        uY = uHeight - uY - 1.0;\n"
    "        gl_FragColor = calculateU(uX / uWidth, uY / uHeight, 1.0 / uWidth, 1.0 / uHeight);\n"
    "    } else if (yuv4xOffset < (y4xSum + u4xSum + v4xSum)) {\n"
    "        float vOffset = (yuv4xOffset - y4xSum - u4xSum) * 4.0 + 1.0;\n"
    "        float vX = floor(mod(vOffset, uWidth / 2.0)) * 2.0;\n"
    "        float vY = floor(vOffset / (uWidth / 2.0)) * 2.0;\n"
    "        vY = uHeight - vY - 1.0;\n"
    "        gl_FragColor = calculateV(vX / uWidth, vY / uHeight, 1.0 / uWidth, 1.0 / uHeight);\n"
    "    }\n"
    "}\n";
