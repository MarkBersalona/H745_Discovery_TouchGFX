#ifndef SCREENSETTINGS1VIEW_HPP
#define SCREENSETTINGS1VIEW_HPP

#include <gui_generated/screensettings1_screen/ScreenSettings1ViewBase.hpp>
#include <gui/screensettings1_screen/ScreenSettings1Presenter.hpp>

class ScreenSettings1View : public ScreenSettings1ViewBase
{
public:
    ScreenSettings1View();
    virtual ~ScreenSettings1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void setLanguageEnglish();
    virtual void setLanguageDeutsch();
    virtual void functionEnableDHCP();
    virtual void functionDisableDHCP();

protected:

#ifdef SIMULATOR
    uint8_t gucIsDHCPEnabled;
#endif

};

#endif // SCREENSETTINGS1VIEW_HPP
