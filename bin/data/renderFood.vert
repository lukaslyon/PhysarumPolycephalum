#version 120

uniform vec2 location;
uniform sampler2DRect scene;

void main(){
	vec4 pos = vec4(location.x, location.y, 1.0, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * pos;
    gl_FrontColor =  vec4(0.0,0.0,0.0,0.0);
}