#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    gui.setup();
    gui.setPosition(0, 250);
    gui.add(front.setup("frontSlider",100,0,1000));
    gui.add(back.setup("backSlider",10,0,200));
    gui.add(pointSize.setup("pointSize",2,0,100));
    gui.add(meshResolution.setup("meshResolutionSlider",2,1,16));
    gui.add(cameraDistance.setup("cameraDistance",500,300,1000));
    gui.add(cameraZoom.setup("cameraZoom"));
    gui.add(cameraSpin.setup("cameraSpin"));
    gui.add(cubeMapSelector.setup("cubeMapSelector",1,1,4));
    cubeMapSelector.addListener(this, &ofApp::updateCubeMap);
    
  ofSetLogLevel(OF_LOG_VERBOSE);
  // disable vertical Sync is too bad with light sometimes
  ofSetVerticalSync(true);
  ofSetFrameRate(60);
  ofBackground(10, 10, 10);
  ofEnableAntiAliasing();
  

  // Point light = emit light in all directions
  pointLight.setDiffuseColor(ofColor::white);
  // specular color = the highlight/shininess color
  pointLight.setSpecularColor(ofColor::white);
  pointLight.setPointLight();
  pointLight.setPosition(100, 0, -150);
  pointLight.setAttenuation(0.0, 0.005);

  // spotLight = emit a cone of light
  spotLight.setSpotlight();
  spotLight.setDiffuseColor(ofColor::green);
  spotLight.setSpecularColor(ofColor::white);
  // size of the cone of emitted light, angle between light axis and side of
  // cone
  // angle range between 0 - 90 in degrees
  spotLight.setSpotlightCutOff(50);
  // rate of falloff, illumitation decreases as the angle from the cone axis
  // increases
  // range 0 - 128, zero is even illumination, 128 is max falloff
  spotLight.setSpotConcentration(45);
  spotLight.setAttenuation(0.0, 0.005);
  spotLight.setPosition(0, 100, -100);
  spotLight.setOrientation(ofVec3f(-90, 0, 0));

  // Directional Lights = emit light based on their orientation, regardless of
  // their position
  directionalLight.setDiffuseColor(ofColor::blue);
  directionalLight.setSpecularColor(ofColor::white);
  directionalLight.setDirectional();
  directionalLight.setPosition(-100, 0, -140);
  // set the direction of the light
  directionalLight.setOrientation(ofVec3f(0, 90, 0));

  // Activate all lights
  bPointLight = bSpotLight = bDirLight = true;
  bShowHelp = true;

  // High resolution sphere
  ofSetSphereResolution(128);
  sphere.setPosition(0, 0, -100);
  test.setPosition(0, 0, -100);

  // Shiny material
  mat.setSpecularColor(ofColor::white);
  mat.setShininess(120);
  ofSetGlobalAmbientColor(ofColor::black);

  tex.load("text1.jpg");

  phong.useLight(&spotLight);
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

  // Cube map setup
  textureSelector = 0;

  myCubeMap.loadImages(
      "ame_bluefreeze/bluefreeze_rt.tga", "ame_bluefreeze/bluefreeze_lf.tga",
      "ame_bluefreeze/bluefreeze_up.tga", "ame_bluefreeze/bluefreeze_dn.tga",
      "ame_bluefreeze/bluefreeze_ft.tga", "ame_bluefreeze/bluefreeze_bk.tga");

  cubeMapShader.load("shaders/CubeMap");

  pointCloudMode = true;
  delayMode=false;
    radius=500;
    camCurrentX=0;
    camCurrentY=0;
  
    
    glShadeModel(GL_SMOOTH);
}

//--------------------------------------------------------------
void ofApp::update() {
    
    if(!cameraZoom){radius=cameraDistance;};
    if(cameraZoom){zoomInOutCamera();};
    if(cameraSpin){spinCamera();};
    if(!cameraSpin){updateCamera();};
    
    updateKinectMesh();

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
                for (int j = 0; j < h; j += step) {
                    vector<ofVec3f> temppoints;
                    vector<ofColor> tempcolors;
                    
                    points.push_back(temppoints);
                    colors.push_back(tempcolors);
                    
                    for (int i = 0; i < w; i += step) {
                        float distance = kinect0.getDistanceAt(i, j);
                        if (distance > back && distance < front) {
                            ofVec3f tempPoint;
                            ofColor tempColor;
                            
                            tempPoint = ofVec3f(i, j, distance *-8);
                            ofColor c;
                            float h = ofMap(distance, 50, 200, 0, 255, true);
                            c.setHsb(h, 255, 255);
                            points[j / step].push_back(tempPoint);
                            colors[j / step].push_back(c);
                            
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
    }

    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofBackgroundGradient(ofColor(64), ofColor(0));
    // this makes everything look glowy :)
    //ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnableDepthTest();
    
  if (pointCloudMode) {
    
      drawPointCloudMode();

  } else {
    cam.begin();
    if (cubeMapReflection) {
      myCubeMap.bind();
     //   myCubeMap.drawSkybox( 800 );
      cubeMapShader.begin();

      cubeMapShader.setUniform1i("envMap", 0);
      cubeMapShader.setUniform1f("reflectivity", 1.0);

      ofPushMatrix();
        ofRotateZ(-180);
        ofTranslate(-kinect0.getDepthPixelsRef().getWidth()/2, -kinect0.getDepthPixelsRef().getHeight()/2, +600);
      //      material.begin();
      // colorShader.begin();
      mesh.setMode(OF_PRIMITIVE_TRIANGLES);

      glLineWidth(int(1));
      mesh.drawFaces();
      //          material.end();
      ofPopMatrix();

      cubeMapShader.end();
      myCubeMap.unbind();

    } else {
      phong.begin();

      // Point cloud mesh
      ofPushMatrix();
      ofRotateZ(-180);
      ofTranslate(-kinect0.getDepthPixelsRef().getWidth()/2, -kinect0.getDepthPixelsRef().getHeight()/2, +600);
      //      material.begin();
      // colorShader.begin();
      mesh.setMode(OF_PRIMITIVE_TRIANGLES);
      glLineWidth(int(1));
      mesh.drawFaces();
      //          material.end();
      ofPopMatrix();

      // sphere.draw();
      phong.end();
        
        ofSetColor(ofColor::black);
        if (bDirLight) {
            ofSetColor(directionalLight.getDiffuseColor());
        }
        directionalLight.draw();
        ofSetColor(ofColor::black);
        if (bPointLight) {
            ofSetColor(pointLight.getDiffuseColor());
        }
        pointLight.draw();
        ofSetColor(ofColor::black);
        if (bSpotLight) {
            ofSetColor(spotLight.getDiffuseColor());
        }
        spotLight.draw();
    }
    

    cam.end();
  }
    
    ofDisableDepthTest();
    
    drawGui();

}


void ofApp::drawPointCloudMode(){
    mesh.setUsage(GL_DYNAMIC_DRAW);
    mesh.setMode(OF_PRIMITIVE_POINTS);
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
        cam.end();
        ofPopStyle();
    }
}

void ofApp::drawGui(){
    ofSetColor(255, 255, 255);
    if (bShowHelp) {
        ofDrawBitmapString(
                           string("Shiny (H) : ") +
                           (mat.getSpecularColor() == ofFloatColor(1., 1., 1.) ? "yes"
                            : "no") +
                           "\n" + "Point Light (1) : " + (bPointLight ? "on" : "off") + "\n" +
                           "Spot Light On (2) : " + (bSpotLight ? "on" : "off") + "\n" +
                           "Directional Light On (3) : " + (bDirLight ? "on" : "off") + "\n" +
                           "Spot Light Cutoff (up/down) : " +
                           ofToString(spotLight.getSpotlightCutOff(), 0) + "\n" +
                           "Spot Light Concentration (right/left) : " +
                           ofToString(spotLight.getSpotConcentration(), 0) + "\n" +
                           "Shader (S) : " +
                           (phong.lightingMethod() == ofxShadersFX::Lighting::PHONG_LIGHTING
                            ? "Phong"
                            : "Blinn-Phong") +
                           "\n" + "Shader type (Y) : " +
                           (phong.shadingMethod() == ofxShadersFX::Lighting::VERTEX_SHADING
                            ? "Vertex "
                            : "Pixel ") +
                           "shader\n" + "Texture (T) : " +
                           (phong.texture() != NULL ? "on" : "off") + "\n" +
                           "Sphere Position (Z: z/c) : " + ofToString(sphere.getPosition()) +
                           "\n" + "FrameRate (Fr) : " + ofToString(ofGetFrameRate()) + "\n" +
                           + "PointCloud (p) : " + ofToString(pointCloudMode) + "\n" +
                           + "Delay (d) : " + ofToString(delayMode) + "\n" +
                           + "reflection map (m) : " + ofToString(cubeMapReflection) + "\n" +
                           "Show/Hide help (E)",
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
