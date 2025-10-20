#ifndef SCREENSTANDBYPRESENTER_HPP
#define SCREENSTANDBYPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ScreenStandbyView;

class ScreenStandbyPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ScreenStandbyPresenter(ScreenStandbyView& v);

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

    virtual ~ScreenStandbyPresenter() {}

private:
    ScreenStandbyPresenter();

    ScreenStandbyView& view;
};

#endif // SCREENSTANDBYPRESENTER_HPP
