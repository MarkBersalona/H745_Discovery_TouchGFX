#ifndef SCREENMAINPRESENTER_HPP
#define SCREENMAINPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class screenMainView;

class screenMainPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    screenMainPresenter(screenMainView& v);

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

    virtual ~screenMainPresenter() {}

    virtual unsigned int getCurrentZone();

    virtual void notifyZoneChanged(unsigned int newZone);

private:
    screenMainPresenter();

    screenMainView& view;
};

#endif // SCREENMAINPRESENTER_HPP
