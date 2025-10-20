#ifndef SCREENNEWPRESENTER_HPP
#define SCREENNEWPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ScreenNewView;

class ScreenNewPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ScreenNewPresenter(ScreenNewView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~ScreenNewPresenter() {}

    virtual unsigned int incrementZoneNumber();
    virtual unsigned int decrementZoneNumber();
    virtual unsigned int getCurrentZone();
    virtual void notifyZoneChanged(unsigned int newZone);

private:
    ScreenNewPresenter();

    ScreenNewView& view;
};

#endif // SCREENNEWPRESENTER_HPP
