#version 330

// input attributes 
layout(location = 0) in vec3 position; 

// input uniforms
uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform mat3 normalMat;   // normal matrix
uniform vec3 light;
uniform vec3 motion;

// out variables 
out vec3 normalView;
out vec3 eyeView;

// fonctions utiles pour créer des terrains en général
vec2 hash(vec2 p) {
  p = vec2( dot(p,vec2(127.1,311.7)),
	    dot(p,vec2(269.5,183.3)) );  
  return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float gnoise(in vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);
	
  vec2 u = f*f*(3.0-2.0*f);
  
  return mix(mix(dot(hash(i+vec2(0.0,0.0)),f-vec2(0.0,0.0)), 
		 dot(hash(i+vec2(1.0,0.0)),f-vec2(1.0,0.0)),u.x),
	     mix(dot(hash(i+vec2(0.0,1.0)),f-vec2(0.0,1.0)), 
		 dot(hash(i+vec2(1.0,1.0)),f-vec2(1.0,1.0)),u.x),u.y);
}

float pnoise(in vec2 p,in float amplitude,in float frequency,in float persistence, in int nboctaves) {
  float a = amplitude;
  float f = frequency;
  float n = 0.0;
  
  for(int i=0;i<nboctaves;++i) {
    n = n+a*gnoise(p*f);
    f = f*2.;
    a = a*persistence;
  }
  
  return n;
}


float computeHeight(in vec2 p) {
  
  // version plan
  return 0.0;
  
  // version sinus statique
  //return 0.5*sin(p.x*10);

  // version sinus animé 
  //return 0.2*sin((p.x+motion.x)*30);
}


vec3 computeNormal(in vec2 p) {
  const float EPS = 0.01;
  const float SCALE = 1.;
  
  vec2 g = vec2(computeHeight(p+vec2(EPS,0.))-computeHeight(p-vec2(EPS,0.)),
		computeHeight(p+vec2(0.,EPS))-computeHeight(p-vec2(0.,EPS)))/(2.*EPS);
  
  vec3 n1 = vec3(1.,0.,g.x*SCALE);
  vec3 n2 = vec3(0.,1.,-g.y*SCALE);
  vec3 n = normalize(cross(n1,n2));

  return n;
}

void main() {

  float h = computeHeight(position.xy);
  vec3  n = computeNormal(position.xy);
  
  vec3 p = vec3(position.xy,h);
  
  gl_Position =  projMat*mdvMat*vec4(p,1);
  normalView  = normalize(normalMat*n);
  eyeView     = normalize((mdvMat*vec4(p,1.0)).xyz);
}
