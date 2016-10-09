#pragma once

#include "ofMain.h"
#include "ofxShadersFX.h"
#include "ofxMultiKinectV2.h"
#include "ofxCubeMap.h"
#include "ofxGui.h"
#include "MSAFluid.h"
//#include "MSATimer.h"
#include "ParticleSystem.h"

class ofApp : public ofBaseApp{
    
public:
    
    ofSpherePrimitive sphere;
    ofSpherePrimitive test;
    ofLight pointLight;
    ofLight directionalLight;
    ofLight spotLight;
    ofEasyCam cam;
    ofxShadersFX::Lighting::LightingShader phong;
    ofMaterial mat;
    bool bPointLight;
    bool bSpotLight;
    bool bDirLight;
    bool bShowHelp;
    bool cubeMapReflection;
    bool pointCloudMode;
    bool delayMode;
    float radius;
    float maxX;
    float minX;
    float minY;
    float maxY;
    float camCurrentX;
    float camCurrentY;
    float camCurrentAngle;
    
    ofImage tex;
    
    ofxMultiKinectV2 kinect0;
    ofVboMesh mesh;
    
    vector< vector<ofVec3f> > points;
    vector< vector<ofColor> > colors;
    vector< vector<int> > indexs;
    
    ofxCubeMap myCubeMap;
    ofShader cubeMapShader;
    
    int textureSelector;
    int kinectFrameLimiter;
    
    ofxPanel gui;
    ofxIntSlider  front;
    ofxIntSlider  back;
    ofxIntSlider  meshResolution;
    ofxIntSlider  pointSize;
    ofxFloatSlider cameraDistance;
    ofxToggle cameraZoom;
    ofxToggle cameraSpin;
    ofxToggle showSolver;
    ofxIntSlider  cubeMapSelector;
    
    
    //solver
    
    float                   colorMult;
    float                   velocityMult;
    int                     fluidCellsX;
    bool                    resizeFluid;
    bool                    drawFluid;
    bool                    drawParticles;
    
    msa::fluid::Solver      fluidSolver;
    msa::fluid::DrawerGl	fluidDrawer;
    
    ParticleSystem          particleSystem;
    
    ofVec2f                 pMouse;
//    ofxPanel gui;
//    ofxIntSlider fluidCellsX2;
//    ofxButton resizeFluid2;

    
    
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void calcNormals(ofMesh &mesh);
    void drawGui();
    void drawPointCloudMode();
    void zoomInOutCamera();
    void spinCamera();
    void updateCamera();
    void updateKinectMesh();
    void updateCubeMap(int &cubeMapSelector);
    void changeCubeMapImages(int textureSelector, ofxCubeMap &myCubeMap);
    void strobeLighting();
    void fadeToColor(float r, float g, float b, float speed);
    void addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce);
    
};