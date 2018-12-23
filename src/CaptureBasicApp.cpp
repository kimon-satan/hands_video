#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "glm/gtc/random.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;


class CaptureBasicApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

  private:
	void printDevices();

    int numCaptureAreas;
    int mCaptureWidth;
    int mCaptureHeight;
    int mSliverWidth;
    
	CaptureRef			mCapture;
	vector<gl::TextureRef>		mTextures;
    vector<Area>                mAreas;
};

void CaptureBasicApp::setup()
{
	//printDevices();

    numCaptureAreas = 3;
    mCaptureWidth = 1280;
    mCaptureHeight = 720;
    mSliverWidth = mCaptureWidth/numCaptureAreas;
    
	try
    {
		mCapture = Capture::create(mCaptureWidth,mCaptureHeight);
		mCapture->start();
	}
	catch( ci::Exception &exc )
    {
		CI_LOG_EXCEPTION( "Failed to init capture ", exc );
	}
    
    for(int i = 0; i < numCaptureAreas; i++)
    {
        int x = mSliverWidth * i;
        mAreas.push_back(Area(ivec2(x,0),ivec2(x + mSliverWidth, mCaptureHeight)));
        mTextures.push_back(nullptr);
    }
}

void CaptureBasicApp::update()
{

	if( mCapture && mCapture->checkNewFrame() )
    {
        Surface8uRef orig  = mCapture->getSurface();
        
        for(int i= 0; i < numCaptureAreas; i++)
        {
            Surface8u crop(mSliverWidth, mCaptureHeight, true, orig->getChannelOrder());
            crop.copyFrom( *orig, mAreas[i], ivec2(-mAreas[i].getUL().x, 0) );
            
            if(!mTextures[i])
            {
                // Capture images come back as top-down, and it's more efficient to keep them that way
                mTextures[i] = gl::Texture::create( crop , gl::Texture::Format().loadTopDown() );
            }
            else
            {
                mTextures[i]->update(crop);
            }
        }
        
	}


}

void CaptureBasicApp::draw()
{

	gl::clear();

    gl::ScopedModelMatrix modelScope;

    for(int i = 0; i < mTextures.size(); i++)
    {
        
        
        
        gl::pushModelView();
        
        if(i%2 == 0)
        {
            gl::translate(mSliverWidth, 0);
            gl::scale(-1, 1);
        }
        
        gl::draw( mTextures[i] );
        gl::popModelView();
        
        gl::translate(mSliverWidth, 0);
    }


}

void CaptureBasicApp::printDevices()
{
	for( const auto &device : Capture::getDevices() )
    {
		console() << "Device: " << device->getName() << " "
		<< endl;
	}
}

void prepareSettings( CaptureBasicApp::Settings* settings )
{

}

CINDER_APP( CaptureBasicApp, RendererGl, prepareSettings )
