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
uniform float clock;
uniform float _t;

// out variables 
out vec3 normalView;
out vec3 eyeView;
out float px;
out float py;

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
  float l = .2;
  return .5*sin(t*3*l) + .2*sin(t*8*l) + 2*sin(t*0.2*l);
}

float computeHeight(in vec2 p) {
  float larg = 0.16;
  float base = -.06;
  if (p.x < -larg){
    base = -0.5;
  }
  if (p.x > larg){
    base = -0.5;
  }
  vec2 point = vec2(p.x, p.y + _y + _t * 0.1);
  point = vec2(p.x , p.y + _y + _t * 0.001);
  float ampl = 0.05;
  float noiseA = pnoise(point, ampl, 40,.005,2);
  float noiseB = pnoise(point + vec2(0.5, 0.5), ampl, 40,.005,2);
  float w = _t *20.;
  float pi = 4.1315;
  float noise = sin(w)*noiseA + sin(-w-pi/2)*noiseB;
  point = vec2(p.x , p.y + _y + _t *  2.);
//  float noise = pnoise(point,.25,10,1.005,2);
  float waves = gnoise(point*2. );
//  waves = 0;
  return base + noise*0.1 + waves*0.03 ;
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
  py = position.y;
  float h =  computeHeight(position.xy);
  vec3  n = computeNormal(position.xy);

  //  float x = position.x + .5*sin(motion.x*10 +(_y + position.y)*3);
  float x = position.x + riverFLow(_y + position.y);
  vec3 p = vec3(x, position.y,h);
//  vec3 p = vec3(x, position.y,h) + _t * 0.0001;


  gl_Position =  projMat*mdvMat*vec4(p,1);
  normalView  = normalize(normalMat*n);
  eyeView     = normalize((mdvMat*vec4(p,1.0)).xyz);
}
