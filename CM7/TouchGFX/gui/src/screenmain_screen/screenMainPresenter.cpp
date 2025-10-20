#include <gui/screenmain_screen/screenMainView.hpp>
#include <gui/screenmain_screen/screenMainPresenter.hpp>

screenMainPresenter::screenMainPresenter(screenMainView& v)
    : view(v)
{

}

void screenMainPresenter::activate()
{

}

void screenMainPresenter::deactivate()
{

}

unsigned int screenMainPresenter::getCurrentZone()
{
  return model->getCurrentZone();
}

void screenMainPresenter::notifyZoneChanged(unsigned int newZone)
{
  view.setupScreen();
}
