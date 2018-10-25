/* Copyright MovingModels Inc, 1999

Module:
----------------------------------------------------------
	mmDisplay

Function:
----------------------------------------------------------
	Handles DirectDraw and Direct3D

Revision History:
----------------------------------------------------------
	07/31/99 - jrb


*/



#include "stdafx.h"
#include "math.h"

#include "global.h"
#include "ki.h"
#include "mmDisplay.h"

#include <switch.h> 
#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions
#include <glad/glad.h>  // glad library (OpenGL loader)

#define GL_MATRIX_MODE				0x0BA0
#define GL_MODELVIEW				0x1700
#define GL_PROJECTION				0x1701
#define GL_TEXTURE					0x1702

#define AR 90
#define AB 90
#define AG 90

#define CULLMODE D3DCULL_CCW
#define NEAR_CLIP_VALUE 0.000001f


#define WIDTH 640
#define HEIGHT 480
#define WIDTH_2 320.f
#define HEIGHT_2 240.f
 

static WORD*	IntermediateBuffer;
WORD   ColorSwap[0x8000] = {0};
void MakeColorSwap16BG();

//-----------------------------------------------------------------------------
// EGL initialization
//-----------------------------------------------------------------------------

static EGLDisplay s_display;
static EGLContext s_context;
static EGLSurface s_surface;
 
bool initEgl()
{
    // Connect to the EGL default display
	
    s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!s_display)
    {
        //TRACE("Could not connect to display! error: %d", eglGetError());
        goto _fail0;
    }

    // Initialize the EGL display connection
    eglInitialize(s_display, NULL, NULL);

    // Select OpenGL (Core) as the desired graphics API
    if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
    {
        //TRACE("Could not set API! error: %d", eglGetError());
        goto _fail1;
    }

    // Get an appropriate EGL framebuffer configuration
    EGLConfig config;
    EGLint numConfigs;
    static const EGLint framebufferAttributeList[] =
    {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_NONE
    };
    eglChooseConfig(s_display, framebufferAttributeList, &config, 1, &numConfigs);
    if (numConfigs == 0)
    {
        //TRACE("No config found! error: %d", eglGetError());
        goto _fail1;
    }

    // Create an EGL window surface
    s_surface = eglCreateWindowSurface(s_display, config, (char*)"", NULL);
    if (!s_surface)
    {
        //TRACE("Surface creation failed! error: %d", eglGetError());
        goto _fail1;
    }

    // Create an EGL rendering context
    static const EGLint contextAttributeList[] =
    {
        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
        EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
        EGL_CONTEXT_MINOR_VERSION_KHR, 3,
        EGL_NONE
    };
    s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, contextAttributeList);
    if (!s_context)
    {
        //TRACE("Context creation failed! error: %d", eglGetError());
        goto _fail2;
    }

    // Connect the context to the surface
    eglMakeCurrent(s_display, s_surface, s_surface, s_context);
    return true;

_fail2:
    eglDestroySurface(s_display, s_surface);
    s_surface = NULL;
_fail1:
    eglTerminate(s_display);
    s_display = NULL;
_fail0:
    return false;
}

void deinitEgl()
{
    if (s_display)
    {
		eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		  
        if (s_context)
        {
            eglDestroyContext(s_display, s_context);
            s_context = NULL;
        }
        if (s_surface)
        {
            eglDestroySurface(s_display, s_surface);
            s_surface = NULL;
        }
        
		eglTerminate(s_display);
        
		s_display = NULL;
    }
}

//-----------------------------------------------------------------------------
// Main program
//-----------------------------------------------------------------------------

static const char* const vertexShaderSource = R"text(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)text";

static const char* const fragmentShaderSource = R"text(
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, TexCoord);
}
)text";

static GLuint createAndCompileShader(GLenum type, const char* source)
{
    GLint success;
    GLchar msg[512];

    GLuint handle = glCreateShader(type);
    if (!handle)
    {
        //TRACE("%u: cannot create shader", type);
        return 0;
    }
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(handle, sizeof(msg), NULL, msg);
        //TRACE("%u: %s\n", type, msg);
        glDeleteShader(handle);
        return 0;
    }

    return handle;
}
 
float vertices[] = {
    // positions          // colors           // texture coords
     0.76f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   // top right
     0.76f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // bottom right
    -0.76f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   // bottom left
    -0.76f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f    // top left 
};
 
unsigned int indices[] = {  
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};
	
static GLuint s_program;
static unsigned int VBO, VAO, EBO;
static GLuint s_tex;
 
DWORD CurTime;
DWORD StartTime=1;
extern DWORD dynaVCount;
extern bool iCpuResetVSYNC; 

mmDisplay::mmDisplay()
{
	IntermediateBuffer	= (WORD *)malloc((320 * 242 + 2) * sizeof(WORD));
	IntermediateBuffer += 321;
	
	gfxInitResolution(1280, 720);
}

mmDisplay::~mmDisplay()
{
	 
}

 
HRESULT mmDisplay::Open(
	WORD Width,
	WORD Height)
{
	initEgl();
	gladLoadGL();
	
	GLint vsh = createAndCompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLint fsh = createAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    s_program = glCreateProgram();
    glAttachShader(s_program, vsh);
    glAttachShader(s_program, fsh);
    glLinkProgram(s_program);

    GLint success;
    glGetProgramiv(s_program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        char buf[512];
        glGetProgramInfoLog(s_program, sizeof(buf), NULL, buf);
        return EXIT_FAILURE;
    }
    glDeleteShader(vsh);
    glDeleteShader(fsh);
 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
 
    // Textures
    glGenTextures(1, &s_tex);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, s_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 	 
	glUseProgram(s_program);
	
	MakeColorSwap16BG();
	MakeScreenBuffer();
	return(0);
 
}
 
void mmDisplay::Close()
{
 
}
 
BOOL mmDisplay::RenderScene()
{
 	return true;
}

void mmDisplay::ClearScreen()
{

}
 
void mmDisplay::BeginScene()
{
 
}

void mmDisplay::EndScene()
{
	 
}

 
void MakeColorSwap16BG()
{
	WORD r, b, g;
 
	for (WORD i = 0; i < 0x8000; i++) {
		r = i & 0x001f;
		g = i & 0x03e0;
		b = i & 0x7c00;

		if (theApp.m_ScanLines) {
			r += 3 <<  0; if (r > 0x001f) r = 0x001f;
			g += 3 <<  5; if (g > 0x03e0) g = 0x03e0;
			b += 3 << 10; if (b > 0x7c00) b = 0x7c00;
		}

		ColorSwap[i] = r << 11 | b >> 10 | g << 1;
	}
}

void mmDisplay::UpdateScreenBuffer(unsigned char *source)
{
 
	//svcOutputDebugString("UpdateScreenBuffer",20);
	
	m_FrameCount++; 
	 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DWORD *src=(DWORD *)source;
	WORD  *dst = IntermediateBuffer;	 
	DWORD p1, p2;
 
	for (int y = 240; y > 0; y--) {
		for (int x = 160; x > 0; x--) {
			p2 = *(src++);
			p1 = p2 & 0x7fff;
			p2 >>= 16;
			p2 &= 0x7fff;
			*(dst++) = ColorSwap[p1];
			*(dst++) = ColorSwap[p2];
		}
	}
	 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, 320, 240, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, IntermediateBuffer);	
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	eglSwapBuffers(s_display, s_surface);
}
 
void mmDisplay::MakeScreenBuffer()
{
 
}
 
