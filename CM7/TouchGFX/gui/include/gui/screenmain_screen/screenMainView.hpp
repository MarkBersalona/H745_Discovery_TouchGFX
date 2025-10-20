#ifndef SCREENMAINVIEW_HPP
#define SCREENMAINVIEW_HPP

#include <gui_generated/screenmain_screen/screenMainViewBase.hpp>
#include <gui/screenmain_screen/screenMainPresenter.hpp>

class screenMainView : public screenMainViewBase
{
public:
    screenMainView();
    virtual ~screenMainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SCREENMAINVIEW_HPP
