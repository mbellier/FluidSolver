#include "touchpoints.h"

#include "Leap.h"
#include "LeapMath.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TouchPointsApp : public AppNative {
  public:
        void setup();
        void draw();
  private:
    int windowWidth = 800;
    int windowHeight = 800;
    Leap::Controller leap;
};

void TouchPointsApp::setup()
{
    this->setWindowSize(windowWidth, windowHeight);
    this->setFrameRate(120);
    gl::enableAlphaBlending();
}

void TouchPointsApp::draw()
{
        gl::clear( Color( .97, .93, .79 ) );
    Leap::PointableList pointables = leap.frame().pointables();
    Leap::InteractionBox iBox = leap.frame().interactionBox();

    for( int p = 0; p < pointables.count(); p++ )
    {
        Leap::Pointable pointable = pointables[p];
        Leap::Vector normalizedPosition = iBox.normalizePoint(pointable.stabilizedTipPosition());
        float x = normalizedPosition.x * windowWidth;
        float y = windowHeight - normalizedPosition.y * windowHeight;

        if(pointable.touchDistance() > 0 && pointable.touchZone() != Leap::Pointable::Zone::ZONE_NONE)
        {
            gl::color(0, 1, 0, 1 - pointable.touchDistance());
        }
        else if(pointable.touchDistance() <= 0)
        {
            gl::color(1, 0, 0, -pointable.touchDistance());
        }
        else
        {
            gl::color(0, 0, 1, .05);
        }

        gl::drawSolidCircle(Vec2f(x,y), 40);
    }
}
