#ifndef SCREENNEWVIEW_HPP
#define SCREENNEWVIEW_HPP

#include <gui_generated/screennew_screen/ScreenNewViewBase.hpp>
#include <gui/screennew_screen/ScreenNewPresenter.hpp>

class ScreenNewView : public ScreenNewViewBase
{
public:
    ScreenNewView();
    virtual ~ScreenNewView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void incrementZoneNumber();
    virtual void decrementZoneNumber();
    virtual void notifyZoneChanged(unsigned int newZone);
protected:
};

#endif // SCREENNEWVIEW_HPP
