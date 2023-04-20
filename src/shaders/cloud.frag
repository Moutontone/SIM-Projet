#version 330

// input uniforms
uniform vec3 light;
uniform vec3 motion;

// in variables
in vec3  normalView;
in vec3  eyeView;
in vec3 p;
// out buffers
layout(location = 0) out vec4 outColor;

void main() {

    vec3 ambient  = vec3(0.5,0.5,0.5);
    vec3 cloud = vec3(128, 124, 122)/255;
    vec3 diffuse = cloud;
    const vec3 specular = vec3(128, 124, 122)/255;;
    const float et = 10.0;

    vec3 n = normalize(normalView);
    vec3 e = normalize(eyeView);
    vec3 l = normalize(light);

    float diff = dot(l,n);
    float spec = pow(max(dot(reflect(l,n),e),0.0),et);

    vec3 color = ambient + diff*diffuse + spec*specular;

    outColor = vec4(color,1.0);
}
