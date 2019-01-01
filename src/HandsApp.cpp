#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/Json.h"
#include "AreaMap.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;


class HandsApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
    void keyDown( KeyEvent event ) override;

  private:
	void printDevices();
    void loadFromConfig();

    int mCaptureWidth;
    int mCaptureHeight;
    
    bool mPaused;

    CaptureRef			        mCapture;
    
    vector<AreaMap>         mAreaMaps;
};



void HandsApp::setup()
{
	printDevices();
    
    mPaused = true;
    
    
    
    


    loadFromConfig();


}

void HandsApp::loadFromConfig()
{
    
    try
    {
        JsonTree configDoc( app::loadAsset( "config.json" ) );
        CI_LOG_V( "successfully loaded config.json" );
        
        try
        {
            if(mCapture.get() != nullptr)
            {
                if(mCapture->isCapturing())mCapture->stop();
            }
            Capture::DeviceRef d = Capture::findDeviceByName(configDoc["camera"].getValue());
            mCaptureWidth = configDoc["width"].getValue<int>();
            mCaptureHeight = configDoc["height"].getValue<int>();
            mCapture = Capture::create(mCaptureWidth,mCaptureHeight, d);
            mCapture->start();
        }
        catch( ci::Exception &exc )
        {
            CI_LOG_EXCEPTION( "Failed to init capture ", exc );
        }
        
        string settingsPath = configDoc["settings"].getValue();
        
        JsonTree settingsDoc( app::loadAsset( settingsPath ) );
        CI_LOG_V( "successfully loaded " + settingsPath );
        
        mPaused = true;
        ci::sleep(20); //Make sure nothing is running
        mAreaMaps.clear(); //destroy any old Area maps
        
        for(int i = 0; i < settingsDoc.getNumChildren(); i++)
        {
            AreaMap t;
            t.mName = settingsDoc[i]["name"].getValue();
            t.mFlipH = settingsDoc[i]["flip_h"].getValue<bool>();
            t.mFlipV = settingsDoc[i]["flip_v"].getValue<bool>();
            
            for(int j = 0; j < 4; j++)
            {
                t.mSrcDims[j] = settingsDoc[i]["src_dims"][j].getValue<float>();
            }
            
            t.mDstCoord[0] = settingsDoc[i]["dst_coord"][0].getValue<float>();
            t.mDstCoord[1] = settingsDoc[i]["dst_coord"][1].getValue<float>();
            t.mScale = settingsDoc[i]["scale"].getValue<float>();
            t.mDelay = settingsDoc[i]["delay"].getValue<int>();
            
            mAreaMaps.push_back(t);
        }
        
        
    }
    catch( Exception &exc )
    {
        console() << exc.what() << endl;
    }
    
    for(int i = 0; i < mAreaMaps.size(); i++)
    {
        mAreaMaps[i].setup(mCaptureWidth, mCaptureHeight);
    }
    
    mPaused = false;
}

void HandsApp::update()
{

	if( mCapture && mCapture->checkNewFrame() && !mPaused )
    {
        SurfaceRef orig  = mCapture->getSurface();
        
        for(int i= 0; i < mAreaMaps.size(); i++)
        {
            mAreaMaps[i].update(orig);
        }
        
	}

}

void HandsApp::draw()
{

    //scale output according to screen and capture size
    ivec2 screenDims = getDisplay()->getSize();
    float scaleAmt = (float)screenDims.x/(float)mCaptureWidth;

	gl::clear();

    gl::ScopedModelMatrix modelScope;
    gl::scale(scaleAmt, scaleAmt);
    
    for(int i = 0; i < mAreaMaps.size(); i++)
    {
        mAreaMaps[i].draw();
    }


}

void HandsApp::keyDown( KeyEvent event )
{
    if( event.getChar() == 'f' ) {
        // Toggle full screen when the user presses the 'f' key.
        setFullScreen( ! isFullScreen() );
    }
    else if( event.getCode() == KeyEvent::KEY_SPACE ) {
        // Clear the list of points when the user presses the space bar.
        loadFromConfig(); // reload the config
    }
    else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
        // Exit full screen, or quit the application, when the user presses the ESC key.
        if( isFullScreen() )
            setFullScreen( false );
        else
            quit();
    }
}



void HandsApp::printDevices()
{
	for( const auto &device : Capture::getDevices() )
    {
		console() << "Device: " << device->getName() << " " << endl;
	}
}

void prepareSettings( HandsApp::Settings* settings )
{
    
    settings->setWindowSize( 1280, 720 );
}

CINDER_APP( HandsApp, RendererGl, prepareSettings )
