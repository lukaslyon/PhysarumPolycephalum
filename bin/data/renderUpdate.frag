#version 120

uniform sampler2DRect scene;
uniform float dR;
uniform float diffuseRate;
uniform float timestep;

precision highp float;

void main(){

	vec2 pos = gl_FragCoord.xy;
	vec4 col;
	vec4 oldCol;

	float decayRate = dR;

	//DIFFUSE
	col = texture2DRect(scene, pos);

	oldCol = col;
	col += texture2DRect(scene, pos + vec2(-1,-1));
	col += texture2DRect(scene, pos + vec2(0,-1));
	col += texture2DRect(scene, pos + vec2(1,-1));
	col += texture2DRect(scene, pos + vec2(-1,0));
	col += texture2DRect(scene, pos + vec2(1,0));
	col += texture2DRect(scene, pos + vec2(-1,1));
	col += texture2DRect(scene, pos + vec2(0,1));
	col += texture2DRect(scene, pos + vec2(1,1));

	col.x /= 9.0;
	col.y /= 9.0;
	col.z /= 9.0;

	col.x = (diffuseRate*col.x) + (1.0 - diffuseRate)*oldCol.x;
	col.y = (diffuseRate*col.y) + (1.0 - diffuseRate)*oldCol.y;
	col.z = (diffuseRate*col.z) + (1.0 - diffuseRate)*oldCol.z;

	col.x = max(0, col.x - decayRate*timestep);
	col.y = max(0, col.y - decayRate*timestep);
	col.z = max(0, col.z - decayRate*timestep);

	col.w = 1.0;

	gl_FragColor = col;
}