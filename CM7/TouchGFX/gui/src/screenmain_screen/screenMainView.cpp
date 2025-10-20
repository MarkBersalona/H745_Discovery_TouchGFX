#include <gui/screenmain_screen/screenMainView.hpp>

screenMainView::screenMainView()
{

}

void screenMainView::setupScreen()
{
    screenMainViewBase::setupScreen();

    // Initialize display of current zone
    Unicode::snprintf(textAreaCurrentZoneBuffer, TEXTAREACURRENTZONE_SIZE, "%d", presenter->getCurrentZone() );
    textAreaCurrentZone.invalidate();
}

void screenMainView::tearDownScreen()
{
    screenMainViewBase::tearDownScreen();
}
