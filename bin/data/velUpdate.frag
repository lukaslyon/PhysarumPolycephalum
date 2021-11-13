#version 120

uniform sampler2DRect backbuffer;   // recive the previus velocity texture
uniform sampler2DRect posData;      // recive the position texture
uniform sampler2DRect scene;

uniform vec2 screen;

uniform float timestep;
uniform float sensorDistance;
uniform float sensorAngle;
uniform float rotationAngle;
uniform float sensorWidth;

vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}
    
void main(void){
    vec2 st = gl_TexCoord[0].st;    // gets the position of the pixel that it´s dealing with...
    
    vec2 pos = texture2DRect( posData, st).xy;      // ... for getting the position data 
    vec2 vel = texture2DRect( backbuffer, st ).xy;  // and the velocity
    vec2 oldVel = vel;

    float width = screen.x;
    float height = screen.y;

    float FL = 0.0;
	float F = 0.0;
	float FR = 0.0;

	pos.x *= width;
	pos.y *= height;

	vec2 FLcornerRight = pos + sensorDistance * rotate(vel, sensorAngle);

	float x = 0.0;
	float y = 0.0;

	for (float i = FLcornerRight.x - sensorWidth; i <= FLcornerRight.x + sensorWidth; i++) {
		for (float j = FLcornerRight.y - sensorWidth; j <= FLcornerRight.y + sensorWidth; j++) {
			if (i > width) {
				x = i - width;
			}
			else if (i < 0) {
				x = width + i;
			}
			else {
				x = i;
			}
			if (j > height) {
				y = j - height;
			}
			else if (j < 0) {
				y = height + j;
			}
			else if (j == height) {
				y = j - 1;
			}
			else {
				y = j;
			}
			FL += texture2DRect(scene, vec2(x,y)).x;
		}
	}

	vec2 FcornerRight = (pos + sensorDistance * vel);

		for (float i = FcornerRight.x - sensorWidth; i <= FcornerRight.x + sensorWidth; i++) {
			for (float j = FcornerRight.y - sensorWidth; j <= FcornerRight.y + sensorWidth; j++) {
			if (i > width) {
				x = i - width;
			}
			else if (i < 0) {
				x = width + i;
			}
			else {
				x = i;
			}
			if (j > height) {
				y = j - height;
			}
			else if (j < 0) {
				y = height + j;
			}
			else if (j == height) {
				y = j - 1;
			}
			else {
				y = j;
			}
			F += texture2DRect(scene, vec2(x,y)).x;
		}
	}

	vec2 FRcornerLeft = pos + sensorDistance * rotate(vel, -sensorAngle);

	for (float i = FRcornerLeft.x - sensorWidth; i <= FRcornerLeft.x + sensorWidth; i++) {
		for (float j = FRcornerLeft.y - sensorWidth; j <= FRcornerLeft.y + sensorWidth; j++) {
			if (i > width) {
				x = i - width;
			}
			else if (i < 0) {
				x = width + i;
			}
			else {
				x = i;
			}
			if (j > height) {
				y = j - height;
			}
			else if (j < 0) {
				y = height + j;
			}
			else if (j == height) {
				y = j - 1;
			}
			else {
				y = j;
			}
			FR += texture2DRect(scene, vec2(x,y)).x;
		}
	}

	if ((F <= FL) && (F <= FR)) {
		float rand = fract(sin(pos.x)*1000000.0);
		if (rand <= 0.5) {
			vel = rotate(vel, rotationAngle);
		}
		else {
			vel = rotate(vel, -rotationAngle);
		}
	}
	if ((FL < FR)) {
		vel = rotate(vel, -rotationAngle);
	}
	else if (FR < FL) {
		vel = rotate(vel, rotationAngle);
	}

	if (texture2DRect(scene, vec2(pos.x + vel.x, pos.y + vel.y)).r == 1.0){
		float rand = fract(sin(pos.x)*1000000.0);
		if (rand <= 0.5) {
			vel = rotate(vel, rotationAngle);
		}
		else {
			vel = rotate(vel, -rotationAngle);
		}
	}

    gl_FragColor = vec4(vel.x,vel.y,0.0,1.0);   // Then save the vel data into the velocity FBO
}