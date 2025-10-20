#include <gui/screennew_screen/ScreenNewView.hpp>
#include <gui/model/Model.hpp>

ScreenNewView::ScreenNewView()
{

}

void ScreenNewView::setupScreen()
{
    ScreenNewViewBase::setupScreen();

    // Initialize display of current zone
    Unicode::snprintf(textAreaZoneNumberBuffer, TEXTAREAZONENUMBER_SIZE, "%d", presenter->getCurrentZone());
    textAreaZoneNumber.invalidate();
}

void ScreenNewView::tearDownScreen()
{
    ScreenNewViewBase::tearDownScreen();
}

void ScreenNewView::incrementZoneNumber()
{
  unsigned int luiCurrentZone;
  luiCurrentZone = presenter->incrementZoneNumber();
  Unicode::snprintf(textAreaZoneNumberBuffer, TEXTAREAZONENUMBER_SIZE, "%d", luiCurrentZone);
  textAreaZoneNumber.invalidate();
}

void ScreenNewView::decrementZoneNumber()
{
  unsigned int luiCurrentZone;
  luiCurrentZone = presenter->decrementZoneNumber();
  Unicode::snprintf(textAreaZoneNumberBuffer, TEXTAREAZONENUMBER_SIZE, "%d", luiCurrentZone);
  textAreaZoneNumber.invalidate();
}

void ScreenNewView::notifyZoneChanged(unsigned int newZone)
{
  Unicode::snprintf(textAreaZoneNumberBuffer, TEXTAREAZONENUMBER_SIZE, "%d", newZone);
  textAreaZoneNumber.invalidate();
}
