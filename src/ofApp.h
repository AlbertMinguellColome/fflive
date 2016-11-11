#pragma once

#include "ofMain.h"
#include "ofxShadersFX.h"
#include "ofxMultiKinectV2.h"
#include "ofxCubeMap.h"
#include "ofxGui.h"
#include "MSAFluid.h"
//#include "MSATimer.h"
#include "ParticleSystem.h"
#include "demoParticle.h"
//#include "AbletonManager.h"

enum meshType
{
    pointCloudMesh = 1,
    cubeMapMesh=2,
    texturedMesh=3
};

class ofApp : public ofBaseApp{
    
public:
    
    ofSpherePrimitive sphere;
    ofSpherePrimitive test;
    ofLight pointLight;
    ofLight directionalLight;
    ofLight spotLight;
    ofLight spotLight45;
    ofLight spotLight90;
    ofLight spotLight135;
    ofLight spotLight180;
    ofLight spotLight125;
    ofLight spotLight270;
    ofLight spotLight315;
    ofLight spotLight360;
    
    ofEasyCam cam;
    ofxShadersFX::Lighting::LightingShader phong;
    ofMaterial mat;
    bool bPointLight;
    bool bSpotLight;
    bool bSpotLight90;
    bool bSpotLight180;
    bool bSpotLight270;
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
    enum meshType meshtype;
    
    vector< vector<ofVec3f> > points;
    vector< vector<ofColor> > colors;
    vector< vector<int> > indexs;
    
    ofxCubeMap myCubeMap;
    ofShader cubeMapShader;
    
    int textureSelector;
    int kinectFrameLimiter;
    int particleFrameLimiter;
    
    ofxPanel gui;
    ofxIntSlider  front;
    ofxIntSlider  back;
    ofxIntSlider  pointSize;
    ofxIntSlider  meshMode;
    ofxIntSlider  meshResolution;
    ofxIntSlider  displacement;
    ofxIntSlider  meshType;
    ofxIntSlider  cutoff;
    ofxIntSlider  lightStrobeFrequency;
    ofxFloatSlider cameraDistance;
    ofxFloatSlider ZFilterMesh;
    ofxToggle showSolvers;
    ofxToggle cameraZoom;
    ofxToggle cameraSpin;
    ofxToggle showSolver;
    ofxToggle activateParticles;
    ofxToggle activatePointCloud;
    ofxToggle activateLightStrobe;
    ofxIntSlider  cubeMapSelector;
    
    
    //solver
    
    float                   colorMult;
    float                   velocityMult;
    int                     fluidCellsX;
    bool                    resizeFluid;
    bool                    drawFluid;
    bool                    drawParticles;
    ofVec2f                 pMouse;
    msa::fluid::Solver      fluidSolver;
    msa::fluid::DrawerGl	fluidDrawer;
    
    
    ParticleSystem          particleSystem;
    


    particleMode currentMode;
    string currentModeStr;
    
    vector <demoParticle> p;
    vector <ofPoint> attractPoints;
    vector <ofPoint> attractPointsWithMovement;

    
    
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
    void drawCubeMapMode();
    void drawTexturedMode();
    void zoomInOutCamera();
    void spinCamera();
    void updateCamera();
    void updateKinectMesh();
    void updateCubeMap(int &cubeMapSelector);
    void changeMeshMode(int &meshSelector);
    void changeMeshType(int &meshTypeSelector);
    void changeCubeMapImages(int textureSelector, ofxCubeMap &myCubeMap);
    void strobeLighting();
   
    void positionLights();
    void setupLights();
    void updateLights();
    void strobeLights();
    
    //Solver
    void setupSolver();
    void fadeToColor(float r, float g, float b, float speed);
    void addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce);
    void drawSolvers();
};
