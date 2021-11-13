#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	timeStep = 0.001f;
	numParticles = 100000;
	numFood = 5;
	diffuseRate = 1.0;
	width = ofGetWindowWidth();
	height = ofGetWindowHeight();

	gui.setup();
	gui.add(decayRate.setup("Decay Rate", 10.0, 5.0, 20.0));
	gui.add(sensorDistance.setup("Sensor Distance", 27.0, 9.0, 36.0));
	gui.add(sensorAngle.setup("Sensor Angle", glm::pi<float>() / 4.0, glm::pi<float>()/16.0, glm::pi<float>()/2.0));
	gui.add(rotationAngle.setup("Rotation Angle", glm::pi<float>() / 8.0, glm::pi<float>() / 16.0, glm::pi<float>() / 2.0));
	gui.add(sensorWidth.setup("Sensor Width", 1.0, 0.5, 2.0));

	int foodWidth = 10.0;
	// load fragment shaders;
	updatePos.load("", "posUpdate.frag");
	updateVel.load("", "velUpdate.frag");
	//updateRender.load("renderUpdate.vert", "renderUpdate.frag");
	updateRender.load("renderUpdate.vert", "");
	updateDiffuse.load("", "renderUpdate.frag");
	updateFood.load("renderFood.vert", "");

	// tweak number of particles to nearest square;
	textureRes = (int)sqrt((float)numParticles);
	
	numParticles = textureRes * textureRes;

	// populate velocity vector;
	vector<float> vel(numParticles * 3);
	for (int i = 0; i < numParticles; i++) {
		float vx = ofRandom(-1.0, 1.0);
		float vy = ofRandom(-1.0, 1.0);
		float length = (float)sqrt(vx*vx + vy * vy);
		vel[i * 3 + 0] = vx / length;
		vel[i * 3 + 1] = vy / length;
		vel[i * 3 + 2] = ofRandom(-1.0, 1.0);
	}

	// allocate and populate velocity texture in velocity ping-pong buffer;
	velPingPong.allocate(textureRes, textureRes, GL_RGB32F);
	velPingPong.src->getTexture().loadData(vel.data(), textureRes, textureRes, GL_RGB);
	velPingPong.dst->getTexture().loadData(vel.data(), textureRes, textureRes, GL_RGB);

	// populate positions vector;
	vector<float> pos(numParticles * 3);
	for (int x = 0; x < textureRes; x++) {
		for (int y = 0; y < textureRes; y++) {
			int i = textureRes * y + x;

			pos[i * 3 + 0] = ofRandom(1.0);
			pos[i * 3 + 1] = ofRandom(1.0);
			pos[i * 3 + 2] = 0.0;
		}

		// allocate and populate position texture in position ping-pong buffer;
		posPingPong.allocate(textureRes, textureRes, GL_RGB32F);
		posPingPong.src->getTexture().loadData(pos.data(), textureRes, textureRes, GL_RGB);
		posPingPong.dst->getTexture().loadData(pos.data(), textureRes, textureRes, GL_RGB);
	}

	// allocate render FBO;
	renderFBO.allocate(width, height, GL_RGB);
	renderFBO.begin();
	ofClear(0.0, 0.0, 0.0);
	renderFBO.end();
	renderFBO.createAndAttachTexture(GL_RGB, 0);

	finalFBO.allocate(width, height, GL_RGB);
	finalFBO.begin();
	ofClear(0.0, 0.0, 0.0);
	finalFBO.end();
	finalFBO.createAndAttachTexture(GL_RGB, 0);

	foodFBO.allocate(width, height, GL_RGB);
	foodFBO.begin();
	ofClear(0.0, 0.0, 0.0);
	foodFBO.end();
	foodFBO.createAndAttachTexture(GL_RGB, 0);

	if (numFood != 0) {
		foodFBO.readToPixels(food);
		for (int i = 0; i < numFood; i++) {
			float pixX = ofRandomWidth();
			float pixY = ofRandomHeight();
			for (int j = 0; j < (int)foodWidth; j++) {
				for (int k = 0; k < (int)foodWidth; k++) {
					float xx = pixX + j;
					float yy = pixY + k;
					if (xx > width) xx -= width;
					if (xx < 0.0) xx += width;
					if (yy > height) yy -= height;
					if (yy < 0.0) yy += height;
					food.setColor(xx, yy, ofColor(255, 0, 255, 255));
				}
			}
		}
		foodFBO.getTexture().loadData(food);
	}

	mesh.setMode(OF_PRIMITIVE_POINTS);
	for (int x = 0; x < textureRes; x++) {
		for (int y = 0; y < textureRes; y++) {
			mesh.addVertex({ x,y,0 });
			mesh.addTexCoord({ x, y });
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){



	// apply shaders to velocity textures;
	velPingPong.dst->begin();
	ofClear(0);
	updateVel.begin();
	updateVel.setUniformTexture("backbuffer", velPingPong.src->getTexture(), 0);
	updateVel.setUniformTexture("posData", posPingPong.src->getTexture(), 1);
	updateVel.setUniformTexture("scene", finalFBO.getTexture(), 2);

	updateVel.setUniform1i("resolution", (int)textureRes);
	updateVel.setUniform2f("screen", (float)width, (float)height);
	updateVel.setUniform1f("timestep", (float)timeStep);
	updateVel.setUniform1f("sensorDistance", (float)sensorDistance);
	updateVel.setUniform1f("sensorAngle", (float)sensorAngle);
	updateVel.setUniform1f("rotationAngle", (float)rotationAngle);
	updateVel.setUniform1f("sensorWidth", (float)sensorWidth);

	velPingPong.src->draw(0,0);
	updateVel.end();
	velPingPong.dst->end();
	velPingPong.swap();

	// apply shaders to position texture;

	posPingPong.dst->begin();
	ofClear(0);
	updatePos.begin();
	updatePos.setUniformTexture("prevPosData", posPingPong.src->getTexture(), 0);
	updatePos.setUniformTexture("velData", velPingPong.src->getTexture(), 1);
	updatePos.setUniform1f("timestep", (float)timeStep);
	updatePos.setUniform2f("screen", (float)width, (float)height);

	posPingPong.src->draw(0,0);
	updatePos.end();
	posPingPong.dst->end();
	posPingPong.swap();


	// render scene;

	renderFBO.begin();
	ofClear(0.0);
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	finalFBO.getTexture().draw(0,0);
	foodFBO.getTexture().draw(0, 0);
	ofDisableBlendMode();
	ofPopStyle();
	renderFBO.end();
	renderFBO.begin();

	updateRender.begin();

	updateRender.setUniformTexture("posTex", posPingPong.dst->getTexture(), 1);
	updateRender.setUniform2f("dimensions", numParticles, numParticles);
	updateRender.setUniform2f("screen", (float)width, (float)height);
	updateRender.setUniformTexture("scene", renderFBO.getTexture(), 0);
	updateRender.setUniform1f("dR", decayRate);
	ofEnablePointSprites();
	mesh.draw();
	glEnd();
	updateRender.end();
	renderFBO.end();

	finalFBO.begin();
	updateDiffuse.begin();
	updateDiffuse.setUniformTexture("scene", renderFBO.getTexture(), 0);
	updateDiffuse.setUniform1f("dR", (float)decayRate);
	updateDiffuse.setUniform1f("diffuseRate", (float)diffuseRate);
	updateDiffuse.setUniform1f("timestep", (float)timeStep);
	renderFBO.getTexture().draw(0, 0);
	updateDiffuse.end();
	finalFBO.end();	
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	finalFBO.draw(0, 0);
	gui.draw();
}