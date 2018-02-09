// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec2 VertexPosition;
//layout(location = 1) in vec3 VertexColour;

layout(location = 1) in vec2 texcoord;



// output to be interpolated between vertices and passed to the fragment stage
out vec3 Colour;
out vec2 Texcoord;

uniform mat4 rotationMatrix;



uniform float time;

vec2 circleOffset (float t){
	return vec2(cos(t), sin(t));
}

void main()
{
    // assign vertex position without modification
    




    gl_Position = vec4(VertexPosition , 0.0, 1.0);

    //rotate
    //gl_Position = rotationMatrix * vec4(VertexPosition , 0.0, 1.0);
    
    


    // assign output colour to be interpolated
   // Colour = VertexColour;

    Texcoord = texcoord;
}
