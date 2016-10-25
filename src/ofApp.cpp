#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup() {
    
    gui.setup();
    gui.setPosition(0, 40);
    gui.add(front.setup("frontSlider",10,0,150));
    gui.add(back.setup("backSlider",100,0,1000));
    gui.add(pointSize.setup("pointSize",2,0,100));  // Increase-decrease point size use it with meshMode = 1 (GL_POINTS)
    gui.add(meshMode.setup("meshMode",1,1,4));  // It change mesh mode POINTS, LINES ,TRIANGLES = activates delanuay, LINES_LOOP
    gui.add(meshType.setup("meshType",1,1,3));// Changes between standard pointCloud , CubeMap and Texture mode
    gui.add(meshResolution.setup("meshResolutionSlider",2,1,16)); //Increase-decrease resolution, use always pair values
    gui.add(displacement.setup("displacement",6,2,8)); // adjust kinect points Z-postion
    gui.add(cubeMapSelector.setup("cubeMapSelector",1,1,4));  // Change cube map images use with meshType = 3
    gui.add(cameraDistance.setup("cameraDistance",500,100,2000));
    gui.add(cameraZoom.setup("cameraZoom",0,25,25)); // Zoom in-out cam.
    //gui.add(cameraSpin.setup("cameraSpin",0,25,25));
    gui.add(activateParticles.setup("activateParticles",0,25,25)); // test with particles to future simulate delays , Atention! Drops FPS if not set higher values of meshResolution
    cubeMapSelector.addListener(this, &ofApp::updateCubeMap);
    meshMode.addListener(this,&ofApp::changeMeshMode);
    meshType.addListener(this,&ofApp::changeMeshType);
    
  ofSetLogLevel(OF_LOG_VERBOSE);
  // disable vertical Sync is too bad with light sometimes
  ofSetVerticalSync(true);
  ofSetFrameRate(60);
  ofBackground(10, 10, 10);
  ofEnableAntiAliasing();
    

  // High resolution sphere
  ofSetSphereResolution(128);
    sphere.setRadius(80);
  sphere.setPosition(0, 0, -100);
  test.setPosition(0, 0, -100);

  // Shiny material
  mat.setSpecularColor(ofColor::white);
  mat.setShininess(120);
  ofSetGlobalAmbientColor(ofColor::black);
  tex.load("text1.jpg");

    setupLights();
    
  phong.useLight(&spotLight);
    phong.useLight(&spotLight90);
    phong.useLight(&spotLight180);
    phong.useLight(&spotLight270);
    phong.useLight(&directionalLight);
    phong.useLight(&pointLight);
  phong.useMaterial(&mat);
  phong.useCamera(&cam);
  sphere.mapTexCoordsFromTexture(tex.getTexture());

  // kinect setup

  kinect0.open(false, true, 0);
  // Note :
  // Default OpenCL device might not be optimal.
  // e.g. Intel HD Graphics will be chosen instead of GeForce.
  // To avoid it, specify OpenCL device index manually like following.
  // kinect1.open(true, true, 0, 2); // GeForce on MacBookPro Retina

  kinect0.start();
  kinectFrameLimiter = 2;
  particleFrameLimiter= 0;
    int mode = 1;
    changeMeshMode(mode);
  // Cube map setup
  textureSelector = 0;

  myCubeMap.loadImages(
      "ame_bluefreeze/bluefreeze_rt.tga", "ame_bluefreeze/bluefreeze_lf.tga",
      "ame_bluefreeze/bluefreeze_up.tga", "ame_bluefreeze/bluefreeze_dn.tga",
      "ame_bluefreeze/bluefreeze_ft.tga", "ame_bluefreeze/bluefreeze_bk.tga");

  cubeMapShader.load("shaders/CubeMap");

  delayMode=false;
    radius=500;
    camCurrentX=0;
    camCurrentY=0;
    
    positionLights();
  
    
    glShadeModel(GL_SMOOTH);
}

//--------------------------------------------------------------
void ofApp::update() {
    
    if(!cameraZoom){radius=cameraDistance;};
    if(cameraZoom){zoomInOutCamera();};
    if(cameraSpin){spinCamera();};
    if(!cameraSpin){updateCamera();};
    
    updateKinectMesh();
    
    for(unsigned int i = 0; i < p.size(); i++){
        p[i].update();
    }
    
    ofVec3f meshPosition = mesh.getCentroid();
    
    pointLight.setPosition(100, 0, meshPosition.z+600);
    //spotLight.setPosition(0, 100, meshPosition.z+600);
   // spotLight90.setPosition(0, 100, meshPosition.z+600);
    spotLight.setOrientation(ofVec3f(-45, 90, 0));
    spotLight90.setOrientation(ofVec3f(225, 0, 0));
    spotLight180.setOrientation(ofVec3f(-45, -90, 0));
    spotLight270.setOrientation(ofVec3f(-45, 0, 0));
    directionalLight.setPosition(-100, 0, meshPosition.z+600);
    directionalLight.setOrientation(ofVec3f(-90, 0, 0));
    
  }

void ofApp::calcNormals(ofMesh &mesh) {
  for (int i = 0; i < mesh.getVertices().size(); i++)
    mesh.addNormal(ofPoint(0, 0, 0));

  for (int i = 0; i < mesh.getIndices().size(); i += 3) {
    const int ia = mesh.getIndices()[i];
    const int ib = mesh.getIndices()[i + 1];
    const int ic = mesh.getIndices()[i + 2];
    //  cout<<"Index:"<<i<<"\n";
    // cout<<ia <<ib<<ic<<"\n";
    if (ia >= mesh.getVertices().size() || ib >= mesh.getVertices().size() ||
        ic >= mesh.getVertices().size()) {
      return;
    }
    //     cout<<mesh.getVertices()[ia] - mesh.getVertices()[ib]
    //     <<mesh.getVertices()[ic] - mesh.getVertices()[ib]<<"\n";
    ofVec3f e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
    ofVec3f e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
    ofVec3f no = e2.cross(e1);

    mesh.getNormals()[ia] += no;
    mesh.getNormals()[ib] += no;
    mesh.getNormals()[ic] += no;

    // depending on your clockwise / winding order, you might want to reverse
    // the e2 / e1 above if your normals are flipped.
  }
}

void ofApp::updateCubeMap(int &cubeMapSelector){
    changeCubeMapImages(cubeMapSelector, myCubeMap);
}
void ofApp::changeMeshMode(int &meshSelector){
    switch (meshSelector) {
        case 1:{
            mesh.setMode(OF_PRIMITIVE_POINTS);
        }
            break;
        case 2:{
            mesh.setMode(OF_PRIMITIVE_LINES);
        }
            break;
        case 3:{
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        }
            break;
        case 4:{
            mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
        }
            break;
            
        default:
            break;
    }
}

void ofApp::changeMeshType(int &meshTypeSelector){


}

void ofApp::changeCubeMapImages(int textureSelector, ofxCubeMap &myCubeMap) {

  switch (textureSelector) {
  case 0:
    break;
  case 1:
    myCubeMap.loadImages(
        "ame_bluefreeze/bluefreeze_rt.tga", "ame_bluefreeze/bluefreeze_lf.tga",
        "ame_bluefreeze/bluefreeze_up.tga", "ame_bluefreeze/bluefreeze_dn.tga",
        "ame_bluefreeze/bluefreeze_ft.tga", "ame_bluefreeze/bluefreeze_bk.tga");
    break;
  case 2:
    myCubeMap.loadImages("mp_ss/ss_rt.tga", "mp_ss/ss_lf.tga",
                         "mp_ss/ss_up.tga", "mp_ss/ss_dn.tga",
                         "mp_ss/ss_ft.tga", "mp_ss/ss_bk.tga");
    break;
  case 3:
    myCubeMap.loadImages(
        "sb_iceflow/iceflow_rt.tga", "sb_iceflow/iceflow_lf.tga",
        "sb_iceflow/iceflow_up.tga", "sb_iceflow/iceflow_dn.tga",
        "sb_iceflow/iceflow_ft.tga", "sb_iceflow/iceflow_bk.tga");
    break;
  case 4:
    myCubeMap.loadImages(
        "sb_strato/stratosphere_rt.tga", "sb_strato/stratosphere_lf.tga",
        "sb_strato/stratosphere_up.tga", "sb_strato/stratosphere_dn.tga",
        "sb_strato/stratosphere_ft.tga", "sb_strato/stratosphere_bk.tga");
    break;

  default:
    break;
  }
}

void ofApp::setupLights(){
    // Point light = emit light in all directions
    pointLight.setDiffuseColor(ofColor::blue);
    // specular color = the highlight/shininess color
    pointLight.setSpecularColor(ofColor::white);
    pointLight.setPointLight();
    pointLight.setPosition(100, 0, -150);
    pointLight.setAttenuation(0.0, 0.005);
    
    // spotLight = emit a cone of light
    spotLight.setSpotlight();
    spotLight.setDiffuseColor(ofColor::red);
    spotLight.setSpecularColor(ofColor::white);
    // size of the cone of emitted light, angle between light axis and side of
    // cone
    // angle range between 0 - 90 in degrees
    spotLight.setSpotlightCutOff(90);
    // rate of falloff, illumitation decreases as the angle from the cone axis
    // increases
    // range 0 - 128, zero is even illumination, 128 is max falloff
    spotLight.setSpotConcentration(128);
    spotLight.setAttenuation(0.0, 0.005);
    spotLight.setPosition(0, 200, -100);
    spotLight.setOrientation(ofVec3f(0, 90, 0));
    
    spotLight90.setSpotlight();
    spotLight90.setDiffuseColor(ofColor::yellow);
    spotLight90.setSpecularColor(ofColor::white);
    spotLight90.setSpotlightCutOff(50);
    spotLight90.setSpotConcentration(45);
    spotLight90.setAttenuation(0.0, 0.005);
    spotLight90.setOrientation(ofVec3f(-45, 0, 0));
    
    spotLight180.setSpotlight();
    spotLight180.setDiffuseColor(ofColor::yellow);
    spotLight180.setSpecularColor(ofColor::white);
    spotLight180.setSpotlightCutOff(50);
    spotLight180.setSpotConcentration(45);
    spotLight180.setAttenuation(0.0, 0.005);
    spotLight180.setOrientation(ofVec3f(-45, 0, 0));
    
    spotLight270.setSpotlight();
    spotLight270.setDiffuseColor(ofColor::yellow);
    spotLight270.setSpecularColor(ofColor::white);
    spotLight270.setSpotlightCutOff(50);
    spotLight270.setSpotConcentration(45);
    spotLight270.setAttenuation(0.0, 0.005);
    spotLight270.setOrientation(ofVec3f(-45, 0, 0));
    
    // Directional Lights = emit light based on their orientation, regardless of
    // their position
    directionalLight.setDiffuseColor(ofColor::blue);
    directionalLight.setSpecularColor(ofColor::white);
    directionalLight.setDirectional();
    directionalLight.setPosition(-100, 0, -140);
    // set the direction of the light
    directionalLight.setOrientation(ofVec3f(0, 90, 0));
    
    // Activate all lights
    bPointLight=false;
    bSpotLight = true;
    bDirLight = false;
    bShowHelp = true;
    
}

void ofApp::positionLights(){
    
    float xorig = 0;
    float zorig = 0;
    float radius= 200;
    float x;
    float z;
    
    x = xorig + radius * cos(0 * PI / 180.0);
    z = zorig + radius * -sin(0 * PI / 180.0);
    spotLight.setPosition(x, 100, z);
    x = xorig + radius * cos(45 * PI / 180.0);
    z = zorig + radius * -sin(45 * PI / 180.0);
    spotLight45.setPosition(x, 100, z);
    x = xorig + radius * cos(90 * PI / 180.0);
    z = zorig + radius * -sin(90 * PI / 180.0);
    spotLight90.setPosition(x, 100, z);
    x = xorig + radius * cos(135 * PI / 180.0);
    z = zorig + radius * -sin(135 * PI / 180.0);
    spotLight135.setPosition(x, 100, z);
    x = xorig + radius * cos(180 * PI / 180.0);
    z = zorig + radius * -sin(180 * PI / 180.0);
    spotLight180.setPosition(x, 100, z);
    x = xorig + radius * cos(270 * PI / 180.0);
    z = zorig + radius * -sin(270 * PI / 180.0);
    spotLight270.setPosition(x, 100, z);
    
}
void ofApp::zoomInOutCamera(){
    bool upDirection;
    if(radius>=1000){
        upDirection=false;
    }else if(radius<=500){
        upDirection=true;
    }
    if (upDirection) {
        radius= radius +0.1;
    }else{
        radius= radius -0.1;
    }
}

void ofApp::spinCamera(){
    float xorig = 0;
    float yorig = 0;
    float angle = ofGetElapsedTimef()*0.5;
    float x = xorig + radius * cos(angle);
    float y = yorig + radius * -sin(angle);
    camCurrentAngle=angle;
    camCurrentX= x;
    camCurrentY=y;
    cam.lookAt(ofVec3f(0,0,0));
    cam.setPosition(x, 0, y);
}

void ofApp::updateCamera(){
    cam.lookAt(ofVec3f(0,0,0));
    cam.setDistance(cameraDistance);
    
}

void ofApp::updateKinectMesh(){
    
    kinect0.update();
    
    int step = meshResolution;
    int total = 0;
    
    int h = kinect0.getDepthPixelsRef().getHeight();
    int w = kinect0.getDepthPixelsRef().getWidth();
    
    
    if (kinect0.isFrameNew()) {
        
        if (delayMode) {
            if (kinectFrameLimiter > 20) {
                kinectFrameLimiter = 0;
                mesh.clear();
            }
        }
        
        if (kinectFrameLimiter >= 0) {
            
            if(!delayMode)
                mesh.clear();
            
            points.clear();
            colors.clear();
            indexs.clear();
            
            {
                vector<demoParticle> tempParticles;
                for (int j = 0; j < h; j += step) {
                    vector<ofVec3f> temppoints;
                    vector<ofColor> tempcolors;
                    
                    points.push_back(temppoints);
                    colors.push_back(tempcolors);
                    
                    for (int i = 0; i < w; i += step) {
                        float distance = kinect0.getDistanceAt(i, j);
                        if (distance > front && distance < back) {
                            ofVec3f tempPoint;
                            ofColor tempColor;
                            demoParticle particle;
                        
                            tempPoint = ofVec3f(i, j, distance * -1 *displacement);
                            ofColor c;
                            float h = ofMap(distance, 50, 200, 0, 255, true);
                            c.setHsb(h, 255, 255);
                            points[j / step].push_back(tempPoint);
                            colors[j / step].push_back(ofColor::white);
                            particle.setPosition(tempPoint);
                            particle.setMode(PARTICLE_MODE_NOISE);
                            particle.reset();
                            particle.addColor(c);
                            tempParticles.push_back(particle);
                            total++;
                        } else {
                            ofVec3f tempPoint2;
                            ofColor tempColor2;
                            tempPoint2 = ofVec3f(i, j, 0);
                            tempColor2 = ofColor(ofColor::yellow);
                            points[j / step].push_back(tempPoint2);
                            colors[j / step].push_back(tempColor2);
                        }
                    }
                }
                
                if(particleFrameLimiter>2 && activateParticles){
                    particleFrameLimiter=0;
                    p.reserve( p.size() + tempParticles.size() );                // preallocate memory without erase
                    p.insert( p.end(), tempParticles.begin(), tempParticles.end() );
                    tempParticles.clear();

                }else if(!activateParticles){
                    p.clear();
                }
                
                int ind = 0;
                for (int m = 0; m < h; m += step) {
                    vector<int> tempindexs;
                    indexs.push_back(tempindexs);
                    
                    for (int n = 0; n < w; n += step) {
                        if (points[m / step][n / step].z != 0) {
                            //   cout << points[m][n] << endl;
                            mesh.addColor(colors[m / step][n / step]);
                            ofVec3f ptTemp= points[m / step][n / step];
                            //   ofVec3f pt = kinect0.getWorldCoordinateAt(ptTemp.x, ptTemp.y, ptTemp.z);
                            mesh.addVertex(ptTemp);
                            indexs[m / step].push_back(ind);
                            ind++;
                        } else {
                            indexs[m / step].push_back(-1);
                        }
                    }
                }
                
                // Drops 4 fps
                if (!pointCloudMode) {
                    int W = int(w / step);
                    for (int b = 0; b < h - step; b += step) {
                        for (int a = 0; a < w - 1; a += step) {
                            if ((indexs[int(b / step)][int(a / step)] != -1 &&
                                 indexs[int(b / step)][int(a / step + 1)] != -1) &&
                                (indexs[int(b / step + 1)][int(a / step + 1)] != -1 &&
                                 indexs[int(b / step + 1)][int(a / step)] != -1)) {
                                    
                                    mesh.addTriangle(indexs[int(b / step)][int(a / step)],
                                                     indexs[int(b / step)][int(a / step + 1)],
                                                     indexs[int(b / step + 1)][int(a / step + 1)]);
                                    mesh.addTriangle(indexs[int(b / step)][int(a / step)],
                                                     indexs[int(b / step + 1)][int(a / step + 1)],
                                                     indexs[int(b / step + 1)][int(a / step)]);
                                }
                        }
                    }
                }
                calcNormals(mesh);
            }
        }
        kinectFrameLimiter++;
        particleFrameLimiter++;
    }

    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofBackgroundGradient(ofColor(64), ofColor(0));
    // this makes everything look glowy :)
    //ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnableDepthTest();
    
    glPointSize(pointSize);
    glLineWidth(int(1));
    
    if(meshType == pointCloudMesh){drawPointCloudMode();};
    if(meshType == cubeMapMesh){drawCubeMapMode();};
    if(meshType == texturedMesh){drawTexturedMode();};
    
    ofDisableDepthTest();
    drawGui();
    
}


void ofApp::drawPointCloudMode(){
    mesh.setUsage(GL_DYNAMIC_DRAW);
    if (mesh.getVertices().size()) {
        ofPushStyle();
        glPointSize(pointSize);
        cam.begin();
        ofDrawAxis(200);
        ofPushMatrix();
        ofRotateZ(-180);
        ofTranslate(-kinect0.getDepthPixelsRef().getWidth()/2, -kinect0.getDepthPixelsRef().getHeight()/2, +600);
        mesh.draw();
        ofPopMatrix();
        
        if(activateParticles){
            ofPushMatrix();
            ofRotateZ(-180);
            ofTranslate(-kinect0.getDepthPixelsRef().getWidth()/2, -kinect0.getDepthPixelsRef().getHeight()/2, +600);
            for(unsigned int i = 0; i < p.size(); i++){
                //  printf("%f /n",p[i].pos.z);
                if(p[i].pos.y > 400){
                    p.erase(p.begin() + i);
                }else{
                    p[i].draw();
                }
            }
            ofPopMatrix();
        }
        cam.end();
        ofPopStyle();
    }
}

void ofApp::drawCubeMapMode(){
    
    cam.begin();
    myCubeMap.bind();
    cubeMapShader.begin();
    cubeMapShader.setUniform1i("envMap", 0);
    cubeMapShader.setUniform1f("reflectivity", 1.0);
    ofPushMatrix();
    ofRotateZ(-180);
    ofTranslate(-kinect0.getDepthPixelsRef().getWidth()/2, -kinect0.getDepthPixelsRef().getHeight()/2, +600);
    mesh.drawFaces();
    ofPopMatrix();
    cubeMapShader.end();
    myCubeMap.unbind();
    cam.end();
    
    
};
void ofApp::drawTexturedMode(){
    
    cam.begin();
    phong.begin();
    ofPushMatrix();
     ofRotateZ(-180);
      ofTranslate(-kinect0.getDepthPixelsRef().getWidth()/2, -kinect0.getDepthPixelsRef().getHeight()/2, +600);
    mesh.drawFaces();
    ofDrawAxis(100);
    //sphere.draw();
    ofPopMatrix();
    phong.end();
    
    ofSetColor(ofColor::black);
    if (bDirLight) {
        ofSetColor(directionalLight.getDiffuseColor());
    }
    //directionalLight.draw();
    ofSetColor(ofColor::black);
    if (bPointLight) {
        ofSetColor(pointLight.getDiffuseColor());
    }
    //  pointLight.draw();
    ofSetColor(ofColor::black);
    if (bSpotLight) {
        ofSetColor(spotLight.getDiffuseColor());
    }
    spotLight.draw();
    ofSetColor(spotLight90.getDiffuseColor());
    spotLight90.draw();
    ofSetColor(spotLight180.getDiffuseColor());
    spotLight180.draw();
    ofSetColor(spotLight270.getDiffuseColor());
    spotLight270.draw();
    cam.end();
    
    
    
};

void ofApp::drawGui(){
    ofSetColor(255, 255, 255);
    if (bShowHelp) {
        ofDrawBitmapString( "FrameRate (Fr) : " + ofToString(ofGetFrameRate()) + "\n" +
                           + "Delay (d) : " + ofToString(delayMode) + "\n" ,
                           20, 20);
    }
 
    gui.draw();
    
};

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  ofVec3f pos;

  switch (key) {
  case OF_KEY_UP:
    spotLight.setSpotlightCutOff(spotLight.getSpotlightCutOff() + 1);
    break;
  case OF_KEY_DOWN:
    spotLight.setSpotlightCutOff(spotLight.getSpotlightCutOff() - 1);
    break;
  case OF_KEY_RIGHT:
    spotLight.setSpotConcentration(spotLight.getSpotConcentration() + 1);
    break;
  case OF_KEY_LEFT:
    spotLight.setSpotConcentration(spotLight.getSpotConcentration() - 1);
    break;
  case '1':
    bPointLight = !bPointLight;
    if (bPointLight) {
      phong.useLight(&pointLight);
    } else {
      phong.removeLight(&pointLight);
    }
    break;
  case '2':
    bSpotLight = !bSpotLight;
    if (bSpotLight) {
      phong.useLight(&spotLight);
    } else {
      phong.removeLight(&spotLight);
    }
    break;
  case '3':
    bDirLight = !bDirLight;
    if (bDirLight) {
      phong.useLight(&directionalLight);
    } else {
      phong.removeLight(&directionalLight);
    }
    break;
  case '4':
    textureSelector = 1;
    changeCubeMapImages(textureSelector, myCubeMap);
    break;
  case '5':
    textureSelector = 2;
    changeCubeMapImages(textureSelector, myCubeMap);
    break;
  case '6':
    textureSelector = 3;
    changeCubeMapImages(textureSelector, myCubeMap);
    break;

  case 'e':
    bShowHelp = !bShowHelp;
    break;
  case 'h':
    if (mat.getSpecularColor() == ofFloatColor(1., 1., 1.)) {
      mat.setSpecularColor(ofFloatColor(0., 0., 0.));
    } else {
      mat.setSpecularColor(ofFloatColor(1., 1., 1.));
    }
    break;
  case 's':
    if (phong.lightingMethod() == ofxShadersFX::Lighting::PHONG_LIGHTING) {
      phong.setLightingMethod(ofxShadersFX::Lighting::BLINN_PHONG_LIGHTING);
    } else {
      phong.setLightingMethod(ofxShadersFX::Lighting::PHONG_LIGHTING);
    }
    break;
  case 'y':
    if (phong.shadingMethod() == ofxShadersFX::Lighting::VERTEX_SHADING) {
      phong.setShadingMethod(ofxShadersFX::Lighting::PIXEL_SHADING);
    } else {
      phong.setShadingMethod(ofxShadersFX::Lighting::VERTEX_SHADING);
    }
    break;
  case 'm':
    if (cubeMapReflection == true) {
      cubeMapReflection = false;
    } else {
      cubeMapReflection = true;
    }
    break;
    case 'd':
          if (delayMode == true) {
              delayMode = false;
          } else {
              delayMode = true;
              pointCloudMode=true;
          }
          break;
  case 't':
    if (phong.texture() == NULL) {
      phong.useTexture(&tex);
    } else {
      phong.removeTexture();
    }
    break;
    
  case 'c':
    pos = sphere.getPosition();
    pos[2] += 5;
    sphere.setPosition(pos);
    break;
  case 'p':
    if (pointCloudMode == true) {
      pointCloudMode = false;
    } else {
      pointCloudMode = true;
    }
    break;
  case 'z':
    pos = sphere.getPosition();
    pos[2] -= 5;
    sphere.setPosition(pos);
    break;
  }
}

#pragma mark - camera tweens



#pragma mark - key events

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}
