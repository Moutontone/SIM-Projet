#version 330

// input attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// input uniforms
uniform mat4 mdvMat;      // modelview matrix
uniform mat4 projMat;     // projection matrix
uniform mat3 normalMat;   // normal matrix
uniform float _y;

// out variables
out vec3 normalView;
out vec3 eyeView;
out vec3 p;

void main() {

    p   = position;
    p.x -= 25*_y;
    p.y += 10*sin(_y);

    gl_Position = projMat*mdvMat*vec4(p,1);
    normalView  = normalize(normalMat*normal);
    eyeView     = normalize((mdvMat*vec4(p,1.0)).xyz);

}
