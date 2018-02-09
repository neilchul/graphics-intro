// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Co-Authors:
//			Jeremy Hart, University of Calgary
//			John Hall, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include "texture.h"

#define PI 3.14159265359
using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

float ratio = 0.0f;			//ratio of 1 GL unit to n pixel 

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
char filePaths[6][50] ={
	"./images/image1-mandrill.png",
	"./images/image2-uclogo.png",
	"./images/image3-aerial.jpg",
	"./images/image4-thirsk.jpg",
	"./images/image5-pattern.png",
	"./images/bard.jpg"
	
} ;

int windowWidth, windowHeight;
int picNumber = -1,  theta = 0, filterMode = 0;
float offsetX = .0f, offsetY = .0f, resize = 1.f; 
MyTexture myTex;
GLuint program;

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

// load, compile, and link shaders, returning true if successful
GLuint InitializeShaders()
{
	// load shader source from files
	string vertexSource = LoadSource("shaders/vertex.glsl");
	string fragmentSource = LoadSource("shaders/fragment.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// check for OpenGL errors and return false if error occurred
	return program;
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct Geometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	Geometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

bool InitializeVAO(Geometry *geometry){

	const GLuint VERTEX_INDEX = 0;
	const GLuint TEXTURE_INDEX = 1;

	//Generate Vertex Buffer Objects
	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->textureBuffer);

	//Set up Vertex Array Object
	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(
		VERTEX_INDEX,		//Attribute index 
		2, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec2),		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(VERTEX_INDEX);

	// associate the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glVertexAttribPointer(
		TEXTURE_INDEX,		//Attribute index 
		2, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec2), 		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(TEXTURE_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return !CheckGLErrors();
}

// create buffers and fill with geometry data, returning true if successful
bool LoadGeometry(Geometry *geometry, vec2 *vertices, vec2 *textures, int elementCount)
{
	geometry->elementCount = elementCount;

	// create an array buffer object for storing our vertices
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*geometry->elementCount, vertices, GL_STATIC_DRAW);

	// create another one for storing our colours
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*geometry->elementCount, textures, GL_STATIC_DRAW);

	//Unbind buffer to reset to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(Geometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(Geometry *geometry, GLuint program)
{
	// clear screen to a dark grey colour
	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);
	glBindVertexArray(geometry->vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}
void drawHalfPic(Geometry *geometry, GLuint program){

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);
	glBindVertexArray(geometry->vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

void drawFullPic(GLuint program, float factor, MyTexture mtex, float theta, float offsetX, float offsetY){

	glClearColor(0.0f, 0.f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float width = mtex.width;
	float height = mtex.height;
	float x = 0.0f, y = 0.0f; 			
	
	float windowRatio = 0.0f;

	vec2 rotation[] = {
		vec2(cos(theta*PI/180), -1*sin(theta*PI/180)),
		vec2(sin(theta*PI/180), cos(theta*PI/180))
	};

/*
	//unlimited rotation works
	glm::mat4 rotation = glm::mat4 (1.0f);
	rotation = glm::rotate(rotation, glm::radians(theta), glm::vec3(0.f,0.f,1.f));

	glUseProgram(program);
	GLint rot = glGetUniformLocation(program, "rotationMatrix");
	glUniformMatrix4fv(rot, 1, GL_FALSE, glm::value_ptr(rotation));
	glUseProgram(0);
*/

	if (height > width){

		ratio = 1/(height/2);
		x = (width/2)*ratio;
		y = 1.0f;
	}
	else if (height < width){
		ratio = 1/(width/2);
		y = (height/2)*ratio;
		x = 1.0f;
	}
	else{
		y = 1.0f;
		x = 1.0f;

	}

	//Scale with window
	if (windowWidth > windowHeight){
		windowRatio = windowWidth/windowHeight;
		
		//scalingMatrix = glm::scale(windowRatio, 1.0f ,1.0f);
		
	}	
	else  if (windowHeight > windowWidth){
		windowRatio = windowHeight/windowWidth;
		
		//scalingMatrix = glm::scale(1.0f, windowRatio ,1.0f);
	}


	

	


	// three vertex positions and assocated colours of a triangle
	vec2 vertices[] = {
		vec2( (-1.0f*x)/factor, y/factor ),
		vec2( (-1.0f*x)/factor,  (-1.0f*y)/factor ),
		vec2( x/factor, (-1.0f*y)/factor ),

		vec2( (-1.0f*x)/factor, y/factor ),
		vec2( x/factor,  y/factor ),
		vec2( x/factor, (-1.0f*y)/factor )
	};

	//	need vec2 texture
	vec2 texCord[] = {
		vec2( 0.0f, height ),
		vec2( 0.0f, 0.0f ),
		vec2( width, 0.0f ),

		vec2( 0.0f, height ),
		vec2( width, height ),
		vec2( width, 0.0f )
	};




	//transformation
	for (int i = 0; i < 6; i++){
		
		
		//rotation
		float newX = (vertices[i][0] * rotation[0][0]) + (vertices[i][1] * rotation[0][1]);
		float newY = (vertices[i][0] * rotation[1][0]) + (vertices[i][1] * rotation[1][1]);
		vertices[i] = vec2(newX, newY);
		

		
		//scale with window
		if (windowHeight > windowWidth)
			vertices[i].y /= windowRatio;
		if (windowHeight < windowWidth)
			vertices[i].x /= windowRatio;
		

		//translation
		newX = vertices[i][0] + offsetX;
		newY = vertices[i][1] + offsetY;
		vertices[i] = vec2(newX, newY);
	}



	// call function to create and fill buffers with geometry data
	Geometry geometry;
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if(!LoadGeometry(&geometry, vertices, texCord, 6))
		cout << "Failed to load geometry" << endl;

	drawHalfPic(&geometry, program);
/*
	//new vertex and texcoord for snd triangle
	vertices[1] = vec2( x/scale,  y/scale );
	texCord[1] = vec2( width, height );

	//apply rotation to the new vertex
	/*
	float newX = (vertices[1][0] * rotation[0][0]) + (vertices[1][1] * rotation[0][1]);
	float newY = (vertices[1][0] * rotation[1][0]) + (vertices[1][1] * rotation[1][1]);
	vertices[1] = vec2( newX,  newY );
	

	if (windowHeight > windowWidth)
		vertices[1].y /= windowRatio;
	if (windowHeight < windowWidth)
		vertices[1].x /= windowRatio;

	vertices[1].x += offsetX;
	vertices[1].y += offsetY;




	if(!LoadGeometry(&geometry, vertices, texCord, 3))
		cout << "Failed to load geometry" << endl;

	drawHalfPic(&geometry, program);

*/




}
// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//GLuint program = InitializeShaders();

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		resize = 1; theta = 0; offsetX = 0.0f; offsetY = 0.0f, filterMode = 0; 
	
		if (picNumber <= 0)
			picNumber = 6;

		

		//MyTexture myTex;

		if (InitializeTexture(&myTex, filePaths[--picNumber], GL_TEXTURE_RECTANGLE)){
			cout << "hi" << endl;
		}
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, myTex.textureID);
		GLint sample = glGetUniformLocation(program, "s");
		GLint mode = glGetUniformLocation(program, "mode");
		glUseProgram(program);
		glUniform1i(sample, 0);
		glUniform1i(mode , 0);
		glUseProgram(0);

		CheckGLErrors();

		//drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	
		
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		resize = 1; theta = 0; offsetX = 0.0f; offsetY = 0.0f, filterMode = 0; 
	

		if(picNumber == 5)
			picNumber = -1;
		
		picNumber;

		//MyTexture myTex;

		if (InitializeTexture(&myTex, filePaths[++picNumber], GL_TEXTURE_RECTANGLE)){
			cout << "hi" << endl;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, myTex.textureID);
		GLint sample = glGetUniformLocation(program, "s");
		GLint mode = glGetUniformLocation(program, "mode");
		glUseProgram(program);
		glUniform1i(sample, 0);
		glUniform1i(mode , 0);
		glUseProgram(0);

		CheckGLErrors();

		//drawFullPic(program,resize, myTex, theta , offsetX, offsetY);

		
	}
	if (key == GLFW_KEY_0 && action == GLFW_PRESS){
		filterMode = 0;
	//	drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS){

		filterMode = 1;
	//	drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS){
		filterMode = 2;
	//	drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS){
		filterMode = 3;
	//	drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	}
	if(key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || action == GLFW_REPEAT)){

		theta+=5;
		//drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	}
	if(key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || action == GLFW_REPEAT)){

	
		theta-=5;

		//cout << theta << endl;
		//drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	}


}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{		
		
	resize-=(yoffset/25.0f);
		
	if (resize <= .1f)
		resize = .1f; 
		

	//GLuint program = InitializeShaders();

	//drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	static vec2 LastPostion(0, 0);
	/*
	GLuint program = InitializeShaders();
	offsetX = xpos/256;
	offsetY = ypos/-256;
	//cout << offsetX <<endl;
	//cout << offsetY<<endl;
	drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	*/
	vec2 CurrentPosition((xpos-(windowWidth/2))/(windowWidth/2), 
							(ypos-(windowHeight/2))/(windowHeight/2)*-1.f);
	
	GLuint program = InitializeShaders();
	

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS){
		offsetX += CurrentPosition.x - LastPostion.x;
		offsetY += CurrentPosition.y - LastPostion.y;
		
		//offsetX = xpos/256*resize/10;
		//offsetY = ypos/-256*resize/10;

		//drawFullPic(program,resize, myTex, theta , offsetX, offsetY);
	}
    
	LastPostion= CurrentPosition;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0,0,width, height);	
	windowWidth = width;
	windowHeight = height;
}

// ==========================================================================
// PROGRAM ENTRY POINT


int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 512, height = 512;
	windowWidth = width; windowHeight = height;
	window = glfwCreateWindow(width, height, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwMakeContextCurrent(window);
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	//Intialize GLAD
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	GLuint program = InitializeShaders();
	if (program == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}
/*
	// three vertex positions and assocated colours of a triangle
	vec2 vertices[] = {
		vec2( -.653f, 1.0f ),
		vec2( -.653f,  -1.0f ),
		vec2( .653f, -1.0f )
	};
*/
/*
	MyTexture myTex;

	if (InitializeTexture(&myTex, filePaths[4], GL_TEXTURE_RECTANGLE)){
		cout << "hi" << endl;
	}
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, myTex.textureID);
	GLint sample = glGetUniformLocation(program, "s");
	glUseProgram(program);
	glUniform1i(sample, 0);
	glUseProgram(0);

	CheckGLErrors();


//	need vec2 texture
	vec2 texCord[] = {
		
		
		vec2( 0.0f, 1530.0f ),
		vec2( 0.0f, 0.0f ),
		vec2( 1000.0f, 0.0f )
	};


	// call function to create and fill buffers with geometry data
	Geometry geometry;
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if(!LoadGeometry(&geometry, vertices, texCord, 3))
		cout << "Failed to load geometry" << endl;


	float timeElapsed = 0.f;
	GLint timeLocation = glGetUniformLocation(program, "time");
/*
	GLint fragmentRed = glGetUniformLocation(program, "red");
	GLint fragmentGreen = glGetUniformLocation(program, "green");
	GLint fragmentBlue = glGetUniformLocation(program, "blue");
*/
	
/*
	glUseProgram(program);
	glUniform1f( fragmentRed, 0.333f);
	glUniform1f( fragmentGreen, 0.333f);
	glUniform1f( fragmentBlue, 0.333f);

*/
	


		if (InitializeTexture(&myTex, filePaths[picNumber], GL_TEXTURE_RECTANGLE)){
			cout << "hi" << endl;
		}
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, myTex.textureID);
		GLint sample = glGetUniformLocation(program, "s");
		GLint mode = glGetUniformLocation(program, "mode");
		glUseProgram(program);
		glUniform1i(sample, 0);
		glUniform1i(mode , 0);
		glUseProgram(0);

		CheckGLErrors();



	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		glUseProgram(program);
	
		
		GLint fragMode = glGetUniformLocation(program, "mode");
		glUniform1i(fragMode , filterMode);
		drawFullPic(program,resize, myTex, theta , offsetX, offsetY);

		glUseProgram(0);

		//timeElapsed += 0.01f;
		glfwSwapBuffers(window);

		glfwPollEvents();
		
	}

	// clean up allocated resources before exit
	//DestroyGeometry(&geometry);
	glUseProgram(0);
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}
