#version 330

// input uniforms 
uniform vec3 light;
uniform vec3 motion;

// in variables 
in vec3  normalView;
in vec3  eyeView;
in float px;
// out buffers 
layout(location = 0) out vec4 outColor;

void main() {
  vec3 ambient  = vec3(0.1,0.1,0.05);
  vec3 diffuse  = vec3(0.3,0.5,0.8);
  const vec3 specular = vec3(0.8,0.2,0.2);
  const float et = 500.0;

  vec3 n = normalize(normalView);
  vec3 e = normalize(eyeView);
  vec3 l = normalize(light);

  vec3 diffuseG = vec3(96., 153., 54.) /255.;
  vec3 diffuseM = vec3(53., 90., 21.) /255.;
  vec3 diffuseD = vec3(96.,153.,54.) /255.;

  // smooth step for colors
  float v = .03;
  if (px < 0) {
    float frontiere = -1./7.;
    float s = smoothstep(frontiere-v, frontiere+v, px);
    diffuse = mix(diffuseG, diffuseM, s);
  } else {
    float frontiere = 1./7.;
    float s = smoothstep(frontiere-v, frontiere+v, px);
    diffuse = mix(diffuseM,diffuseD, s);

  }


    float diff = dot(l,n);
  float spec = pow(max(dot(reflect(l,n),e),0.0),et);

  vec3 color = ambient + diff*diffuse + spec*specular;

  outColor = vec4(color,1.0);
}
