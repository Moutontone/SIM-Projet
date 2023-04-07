#version 330

// input attributes 
layout(location = 0) in vec3 position; 

// input uniforms
uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform mat3 normalMat;   // normal matrix
uniform vec3 light;
uniform vec3 motion;
uniform float _y;

// out variables 
out vec3 normalView;
out vec3 eyeView;
out float px;

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

float riverFLow(float t){
//  return .5*sin(t*3);
  return .5*sin(t*3) + .2*sin(t*8) + 2*sin(t*0.2);
}

float computeHeight(in vec2 p) {
  float height;
  float height1;
  float height2;
  float height3;
  float height_river;
  // version plan
  //  float height_left = left_height();
//  float height_right = right_height();
  if (p.x < -1./3.) {
    //rive gauche
    height = 0;
    // variation
    vec2 point = vec2(p.x, p.y + _y);
    height += pnoise(point,.05,.8,.5,2);
    height1 = height;
    height1 = .051;
  } else if (p.x > 1./3.) {
    //rive droite
    height = -.02;
    height3 = height;
  }
  // lit de la rivière
  float offset = -(3.1415)/2.;
  float periode = 10;
  // variation de la largeur
  periode + 3*(sin((_y+ p.y)*3) + 0.3*sin((_y+ p.y)*10));
  float max_height = 0;
  float sin_height = .2;
  // calculation
  float sin_val = sin_height*sin(offset + p.x * periode);
  height = sin_val;
  height = min(0., height);
  height = max(-.12, height);
  height_river = height;

  if (p.x < 0) {
    float frontiere = -1./3.;
    float v = .1;
    float s = smoothstep(frontiere-v, frontiere+v, p.x);
    height = mix(height1, height_river, s);
    return height;
  } if (p.x > 0){
    float frontiere = 1./3.;
    float v = .1;
    float s = smoothstep(frontiere-v, frontiere+v, p.x);
    height = mix(height_river,height3, s);
    return height;
  }
  return height_river;
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
  px = position.x;
  float h = computeHeight(position.xy);
  vec3  n = computeNormal(position.xy);

//  float x = position.x + .5*sin(motion.x*10 +(_y + position.y)*3);
  float x = position.x + riverFLow(_y + position.y);
  vec3 p = vec3(x, position.y,h);

  
  gl_Position =  projMat*mdvMat*vec4(p,1);
  normalView  = normalize(normalMat*n);
  eyeView     = normalize((mdvMat*vec4(p,1.0)).xyz);
}
