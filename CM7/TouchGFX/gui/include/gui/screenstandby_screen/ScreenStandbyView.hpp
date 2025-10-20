#ifndef SCREENSTANDBYVIEW_HPP
#define SCREENSTANDBYVIEW_HPP

#include <gui_generated/screenstandby_screen/ScreenStandbyViewBase.hpp>
#include <gui/screenstandby_screen/ScreenStandbyPresenter.hpp>

class ScreenStandbyView : public ScreenStandbyViewBase
{
public:
    ScreenStandbyView();
    virtual ~ScreenStandbyView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SCREENSTANDBYVIEW_HPP
