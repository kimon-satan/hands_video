//
//  AreaMap.hpp
//  HandsApp
//
//  Created by Simon on 01/01/2019.
//

#ifndef AreaMap_hpp
#define AreaMap_hpp

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"

using namespace ci;
using namespace std;

class AreaMap {
    
public:
    string mName;
    float mSrcDims [4];
    float mDstCoord [2];
    float mScale;
    bool mFlipH;
    bool mFlipV;
    int mDelay;
    
    void setup(int captureWidth, int captureHeight);
    void update(SurfaceRef orig);
    void draw();
    
private:
    
    Surface8u mCrop;
    vector<Surface8u>   mHist;
    gl::TextureRef      mTextureRef;
    Area mSrcArea;
    ivec2 mDstPixels;
    
};



#endif /* AreaMap_hpp */
