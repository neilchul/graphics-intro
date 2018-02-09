// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 Colour;
in vec2 Texcoord;

// first output is mapped to the framebuffer's colour index by default
//out vec4 FragmentColour;
out vec4 outColor;

uniform sampler2DRect s; 

uniform float red;
uniform float green; 
uniform float blue; 

/*
modes : 0       - default 
        1,2,3   - luminance
*/
uniform int mode;


//luminance
vec4  luminance(float r, float g, float b){
    vec4 currentColor = texture(s, Texcoord);

    float L = (currentColor[0] * r)
                +   (currentColor[1] * g)     
                +   (currentColor[2] * b);

    return vec4(
        L,L,L,currentColor[3]
    );
}

void main(void)
{
    // write colour output without modification
    //FragmentColour = vec4(Colour, 0);

    if (mode == 0)
        outColor = texture(s, Texcoord); 
    else if (mode == 1)
        outColor = luminance(0.333, 0.333, 0.333);
    else if (mode == 2)
        outColor = luminance(0.299, 0.587, 0.114);    
    else if (mode == 3)
        outColor = luminance(0.213, 0.715, 0.072);
 

    //outColor = vec4 (Texcoord/1530, 0,1);
}




//sobel filter returns a vec3 colors


