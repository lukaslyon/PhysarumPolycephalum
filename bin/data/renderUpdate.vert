#version 120

uniform sampler2DRect posTex;
uniform vec2 dimensions;
uniform vec2 screen;

void main(){
    vec2 verPos = gl_MultiTexCoord0.xy;
    vec4 pixPos = texture2DRect( posTex, verPos );
    pixPos.x *= screen.x;
    pixPos.y *= screen.y;
    gl_PointSize = 3.0f;
    gl_Position = gl_ModelViewProjectionMatrix * pixPos;
    gl_FrontColor =  vec4(1.0,1.0,1.0,1.0);
}