#pragma once

#include "ofMain.h"
#include "ofxGui.h"

struct pingPongBuffer {
public:
	void allocate(int _width, int _height, int _internalFormat = GL_RGBA) {
		for (int i = 0; i < 2; i++) {
			FBOs[i].allocate(_width, _height, _internalFormat);
			FBOs[i].getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
		}

		src = &FBOs[0];
		dst = &FBOs[1];

		clear();
	}

	void swap() {
		std::swap(src, dst);
	}

	void clear() {
		for (int i = 0; i < 2; i++) {
			FBOs[i].begin();
			ofClear(0, 255);
			FBOs[i].end();
		}
	}

	ofFbo& operator[](int n){
		return FBOs[n];
	}
	ofFbo *src;
	ofFbo *dst;

private:
	ofFbo FBOs[2];
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		//void mousePressed(int x, int y, int button);

		ofShader updatePos;
		ofShader updateVel;
		ofShader updateRender;
		ofShader updateDiffuse;
		ofShader updateFood;

		pingPongBuffer posPingPong;
		pingPongBuffer velPingPong;

		ofFbo renderFBO;
		ofFbo finalFBO;
		ofFbo foodFBO;

		ofPixels food;

		float timeStep;
		ofxFloatSlider decayRate;
		float diffuseRate;

		int width, height;
		int imgWidth, imgHeight;
		int numParticles;
		int textureRes;
		int numFood;

		float foodWidth;
		ofxFloatSlider sensorDistance;
		ofxFloatSlider sensorAngle;
		ofxFloatSlider rotationAngle;
		ofxFloatSlider sensorWidth;

		ofxPanel gui;

		ofVboMesh mesh;

};
