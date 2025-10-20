#include <gui/screennew_screen/ScreenNewView.hpp>
#include <gui/screennew_screen/ScreenNewPresenter.hpp>

ScreenNewPresenter::ScreenNewPresenter(ScreenNewView& v)
    : view(v)
{

}

void ScreenNewPresenter::activate()
{

}

void ScreenNewPresenter::deactivate()
{

}

unsigned int ScreenNewPresenter::incrementZoneNumber()
{
  unsigned int luiCurrentZone;
  luiCurrentZone = model->getCurrentZone();
  if (luiCurrentZone+1 >= 1 && luiCurrentZone+1 <= 50)
  {
    ++luiCurrentZone;
    model->setCurrentZone(luiCurrentZone);
  }
  return luiCurrentZone;
}

unsigned int ScreenNewPresenter::decrementZoneNumber()
{
  unsigned int luiCurrentZone;
  luiCurrentZone = model->getCurrentZone();
  if (luiCurrentZone-1 >= 1 && luiCurrentZone-1 <= 50)
  {
    --luiCurrentZone;
    model->setCurrentZone(luiCurrentZone);
  }
  return luiCurrentZone;
}

unsigned int ScreenNewPresenter::getCurrentZone()
{
  return model->getCurrentZone();
}


void ScreenNewPresenter::notifyZoneChanged(unsigned int newZone)
{
  view.notifyZoneChanged(newZone);
}
