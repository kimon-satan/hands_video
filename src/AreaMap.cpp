//
//  AreaMap.cpp
//  HandsApp
//
//  Created by Simon on 01/01/2019.
//

#include "AreaMap.hpp"


void AreaMap::setup(int captureWidth, int captureHeight)
{
    ivec2 tls (captureWidth * mSrcDims[0], captureHeight * mSrcDims[1]);
    ivec2 trs (captureWidth * mSrcDims[2], captureHeight * mSrcDims[3]);
    mSrcArea = Area(tls,trs);
    mDstPixels = ivec2(captureWidth * mDstCoord[0], captureHeight * mDstCoord[1]);
    
    //Create the texture ref and crop surface
    mTextureRef = gl::Texture::create(mSrcArea.getWidth(), mSrcArea.getHeight(), gl::Texture::Format().loadTopDown());
    mCrop = Surface8u(mSrcArea.getWidth(), mSrcArea.getHeight(), true /*, orig->getChannelOrder() */); //TODO
}

void AreaMap::update(SurfaceRef orig)
{
    mCrop.copyFrom( *orig, mSrcArea, -mSrcArea.getUL()); //DOES THIS WORK
    
    if(mDelay > 0)
    {
        if(mHist.size() > mDelay)
        {
            mHist.erase(mHist.begin());
        }
        
        mHist.push_back(mCrop);
        
        if(mTextureRef.get() != nullptr)mTextureRef->update(mHist[0]);
    }
    else
    {
        if(mTextureRef.get() != nullptr)mTextureRef->update(mCrop);
    }
}

void AreaMap::draw()
{
    gl::pushModelView();

    gl::translate(mDstPixels);
    
    ivec2 trans(0,0);
    ivec2 scale(mScale,mScale);
    
    if(mFlipH)
    {
        trans.x += mSrcArea.getWidth() * mScale;
        scale.x *= -1;
    }
    
    if(mFlipV)
    {
        trans.y += mSrcArea.getHeight() * mScale;
        scale.y *= -1;
    }
    
    gl::translate(trans);
    gl::scale(scale);
    
    gl::draw(mTextureRef);
    
    
    gl::popModelView();
}
