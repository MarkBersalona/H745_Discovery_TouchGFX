#include <gui/screensettings1_screen/ScreenSettings1View.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#ifndef SIMULATOR
#include "main.h"
#endif

ScreenSettings1View::ScreenSettings1View()
{

}

void ScreenSettings1View::setupScreen()
{
    ScreenSettings1ViewBase::setupScreen();

    // Detect the current language, and set the language radio buttons accordingly
    LanguageId ltCurrentLanguage = Texts::getLanguage();
    switch (ltCurrentLanguage)
    {
    case GB:
      radioButtonSettings1LanguageEnglish.setSelected(true);
      radioButtonSettings1LanguageDeutsch.setSelected(false);
      break;
    case DE:
      radioButtonSettings1LanguageEnglish.setSelected(false);
      radioButtonSettings1LanguageDeutsch.setSelected(true);
      break;
    default:
      radioButtonSettings1LanguageEnglish.setSelected(false);
      radioButtonSettings1LanguageDeutsch.setSelected(false);
      break;
    }

    // Set radio buttons for DHCP/Static depending on if DHCP is enabled
    if (gucIsDHCPEnabled)
    {
      radioButtonSettings1IPDHCP.setSelected(true);
      radioButtonSettings1IPStatic.setSelected(false);
    }
    else
    {
      radioButtonSettings1IPDHCP.setSelected(false);
      radioButtonSettings1IPStatic.setSelected(true);
    }

}

void ScreenSettings1View::tearDownScreen()
{
    ScreenSettings1ViewBase::tearDownScreen();
}

void ScreenSettings1View::setLanguageEnglish()
{
  Texts::setLanguage(GB);
  invalidate(); // invalidate the entire screen, so texts will update to new language
}

void ScreenSettings1View::setLanguageDeutsch()
{
  Texts::setLanguage(DE);
  invalidate(); // invalidate the entire screen, so texts will update to new language
}

void ScreenSettings1View::functionEnableDHCP()
{
  gucIsDHCPEnabled = true;
}
void ScreenSettings1View::functionDisableDHCP()
{
  gucIsDHCPEnabled = false;
}
