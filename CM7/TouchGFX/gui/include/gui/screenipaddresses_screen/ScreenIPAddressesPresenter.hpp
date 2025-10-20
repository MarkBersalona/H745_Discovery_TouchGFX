#ifndef SCREENIPADDRESSESPRESENTER_HPP
#define SCREENIPADDRESSESPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ScreenIPAddressesView;

class ScreenIPAddressesPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ScreenIPAddressesPresenter(ScreenIPAddressesView& v);

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

    virtual ~ScreenIPAddressesPresenter() {}

private:
    ScreenIPAddressesPresenter();

    ScreenIPAddressesView& view;
};

#endif // SCREENIPADDRESSESPRESENTER_HPP
