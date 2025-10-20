#ifndef SCREENABOUTVIEW_HPP
#define SCREENABOUTVIEW_HPP

#include <gui_generated/screenabout_screen/ScreenAboutViewBase.hpp>
#include <gui/screenabout_screen/ScreenAboutPresenter.hpp>

class ScreenAboutView : public ScreenAboutViewBase
{
public:
    ScreenAboutView();
    virtual ~ScreenAboutView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SCREENABOUTVIEW_HPP
