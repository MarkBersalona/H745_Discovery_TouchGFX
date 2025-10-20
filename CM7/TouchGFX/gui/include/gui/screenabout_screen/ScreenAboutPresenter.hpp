#ifndef SCREENABOUTPRESENTER_HPP
#define SCREENABOUTPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ScreenAboutView;

class ScreenAboutPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ScreenAboutPresenter(ScreenAboutView& v);

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

    virtual ~ScreenAboutPresenter() {}

private:
    ScreenAboutPresenter();

    ScreenAboutView& view;
};

#endif // SCREENABOUTPRESENTER_HPP
