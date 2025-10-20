#include <gui/screenipaddresses_screen/ScreenIPAddressesView.hpp>
#ifndef SIMULATOR
#include "main.h"
#endif

ScreenIPAddressesView::ScreenIPAddressesView()
{

}

void ScreenIPAddressesView::setupScreen()
{
    ScreenIPAddressesViewBase::setupScreen();

    // Initialize address byte values with saved values
    functionRevertAddresses();

}

void ScreenIPAddressesView::tearDownScreen()
{
    ScreenIPAddressesViewBase::tearDownScreen();
}

void ScreenIPAddressesView::actionWriteCounterValue(uint8_t value)
{
  uint16_t liNumberSelectorValue;

  // Get the value from the number picker
  liNumberSelectorValue = numberPICKERAddress.getValue();

  switch (value)
  {
  case 1: // Address byte 1
    lucAddress1 = liNumberSelectorValue;
    Unicode::snprintf(textAreaIPAddress1Buffer, TEXTAREAIPADDRESS1_SIZE, "%d", lucAddress1);
    textAreaIPAddress1.invalidate();
    break;
  case 2: // Address byte 2
    lucAddress2 = liNumberSelectorValue;
    Unicode::snprintf(textAreaIPAddress2Buffer, TEXTAREAIPADDRESS2_SIZE, "%d", lucAddress2);
    textAreaIPAddress2.invalidate();
    break;
  case 3: // Address byte 3
    lucAddress3 = liNumberSelectorValue;
    Unicode::snprintf(textAreaIPAddress3Buffer, TEXTAREAIPADDRESS3_SIZE, "%d", lucAddress3);
    textAreaIPAddress3.invalidate();
    break;
  case 4: // Address byte 4
    lucAddress4 = liNumberSelectorValue;
    Unicode::snprintf(textAreaIPAddress4Buffer, TEXTAREAIPADDRESS4_SIZE, "%d", lucAddress4);
    textAreaIPAddress4.invalidate();
    break;
  case 5: // DNS byte 1
    lucDNS1 = liNumberSelectorValue;
    Unicode::snprintf(textAreaDNS1Buffer, TEXTAREADNS1_SIZE, "%d", lucDNS1);
    textAreaDNS1.invalidate();
    break;
  case 6: // DNS byte 2
    lucDNS2 = liNumberSelectorValue;
    Unicode::snprintf(textAreaDNS2Buffer, TEXTAREADNS2_SIZE, "%d", lucDNS2);
    textAreaDNS2.invalidate();
    break;
  case 7: // DNS byte 3
    lucDNS3 = liNumberSelectorValue;
    Unicode::snprintf(textAreaDNS3Buffer, TEXTAREADNS3_SIZE, "%d", lucDNS3);
    textAreaDNS3.invalidate();
    break;
  case 8: // DNS byte 4
    lucDNS4 = liNumberSelectorValue;
    Unicode::snprintf(textAreaDNS4Buffer, TEXTAREADNS4_SIZE, "%d", lucDNS4);
    textAreaDNS4.invalidate();
    break;
  case 9: // Gateway byte 1
    lucGateway1 = liNumberSelectorValue;
    Unicode::snprintf(textAreaGateway1Buffer, TEXTAREAGATEWAY1_SIZE, "%d", lucGateway1);
    textAreaGateway1.invalidate();
    break;
  case 10: // Gateway byte 2
    lucGateway2 = liNumberSelectorValue;
    Unicode::snprintf(textAreaGateway2Buffer, TEXTAREAGATEWAY2_SIZE, "%d", lucGateway2);
    textAreaGateway2.invalidate();
    break;
  case 11: // Gateway byte 3
    lucGateway3 = liNumberSelectorValue;
    Unicode::snprintf(textAreaGateway3Buffer, TEXTAREAGATEWAY3_SIZE, "%d", lucGateway3);
    textAreaGateway3.invalidate();
    break;
  case 12: // Gateway byte 4
    lucGateway4 = liNumberSelectorValue;
    Unicode::snprintf(textAreaGateway4Buffer, TEXTAREAGATEWAY4_SIZE, "%d", lucGateway4);
    textAreaGateway4.invalidate();
    break;
  case 13: // Netmask byte 1
    lucNetmask1 = liNumberSelectorValue;
    Unicode::snprintf(textAreaNetmask1Buffer, TEXTAREANETMASK1_SIZE, "%d", lucNetmask1);
    textAreaNetmask1.invalidate();
    break;
  case 14: // Netmask byte 2
    lucNetmask2 = liNumberSelectorValue;
    Unicode::snprintf(textAreaNetmask2Buffer, TEXTAREANETMASK2_SIZE, "%d", lucNetmask2);
    textAreaNetmask2.invalidate();
    break;
  case 15: // Netmask byte 3
    lucNetmask3 = liNumberSelectorValue;
    Unicode::snprintf(textAreaNetmask3Buffer, TEXTAREANETMASK3_SIZE, "%d", lucNetmask3);
    textAreaNetmask3.invalidate();
    break;
  case 16: // Netmask byte 4
    lucNetmask4 = liNumberSelectorValue;
    Unicode::snprintf(textAreaNetmask4Buffer, TEXTAREANETMASK4_SIZE, "%d", lucNetmask4);
    textAreaNetmask4.invalidate();
    break;
  default:
    break;
  }
}

void ScreenIPAddressesView::actionSelectAddressByte(uint8_t value)
{
  // Make the number picker visible
  numberPICKERAddress.setVisible(true);
  numberPICKERAddress.invalidate();

  // Reset all the address byte flex button borders to normal
  resetAddressButtonBorders();

  // Highlight the selected address byte flex button border
  switch (value)
  {
  case 1: // Address byte 1
    flexButtonIPAddress1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonIPAddress1.invalidate();
    numberPICKERAddress.setValue(lucAddress1);
    break;
  case 2: // Address byte 2
    flexButtonIPAddress2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonIPAddress2.invalidate();
    numberPICKERAddress.setValue(lucAddress2);
    break;
  case 3: // Address byte 3
    flexButtonIPAddress3.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonIPAddress3.invalidate();
    numberPICKERAddress.setValue(lucAddress3);
    break;
  case 4: // Address byte 4
    flexButtonIPAddress4.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonIPAddress4.invalidate();
    numberPICKERAddress.setValue(lucAddress4);
    break;
  case 5: // DNS byte 1
    flexButtonDNS1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonDNS1.invalidate();
    numberPICKERAddress.setValue(lucDNS1);
    break;
  case 6: // DNS byte 2
    flexButtonDNS2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonDNS2.invalidate();
    numberPICKERAddress.setValue(lucDNS2);
    break;
  case 7: // DNS byte 3
    flexButtonDNS3.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonDNS3.invalidate();
    numberPICKERAddress.setValue(lucDNS3);
    break;
  case 8: // DNS byte 4
    flexButtonDNS4.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonDNS4.invalidate();
    numberPICKERAddress.setValue(lucDNS4);
    break;
  case 9: // Gateway byte 1
    flexButtonGateway1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonGateway1.invalidate();
    numberPICKERAddress.setValue(lucGateway1);
    break;
  case 10: // Gateway byte 2
    flexButtonGateway2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonGateway2.invalidate();
    numberPICKERAddress.setValue(lucGateway2);
    break;
  case 11: // Gateway byte 3
    flexButtonGateway3.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonGateway3.invalidate();
    numberPICKERAddress.setValue(lucGateway3);
    break;
  case 12: // Gateway byte 4
    flexButtonGateway4.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonGateway4.invalidate();
    numberPICKERAddress.setValue(lucGateway4);
    break;
  case 13: // Netmask byte 1
    flexButtonNetmask1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonNetmask1.invalidate();
    numberPICKERAddress.setValue(lucNetmask1);
    break;
  case 14: // Netmask byte 2
    flexButtonNetmask2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonNetmask2.invalidate();
    numberPICKERAddress.setValue(lucNetmask2);
    break;
  case 15: // Netmask byte 3
    flexButtonNetmask3.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonNetmask3.invalidate();
    numberPICKERAddress.setValue(lucNetmask3);
    break;
  case 16: // Netmask byte 4
    flexButtonNetmask4.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(255, 255, 255), touchgfx::Color::getColorFromRGB(255, 255, 255));
    flexButtonNetmask4.invalidate();
    numberPICKERAddress.setValue(lucNetmask4);
    break;
  default:
    break;
  }

  // Save the selected address byte index
  lucSelectedAddressByte = value;
}

void ScreenIPAddressesView::functionUpdateSelectedAddressWithCounter()
{
  // If an address byte has been selected, update its value with the counter value
  if (lucSelectedAddressByte >= 1 && lucSelectedAddressByte <= 16)
  {
    actionWriteCounterValue(lucSelectedAddressByte);
  }
}

void ScreenIPAddressesView::functionSaveAddresses()
{
  // Deselect address byte
  lucSelectedAddressByte = 0;
  // Reset all the address byte flex button borders to normal
  resetAddressButtonBorders();

  // Make the number picker invisible
  numberPICKERAddress.setVisible(false);
  numberPICKERAddress.invalidate();



  // Save the current values of address bytes
  gucAddress1Saved = lucAddress1;
  gucAddress2Saved = lucAddress2;
  gucAddress3Saved = lucAddress3;
  gucAddress4Saved = lucAddress4;
  gucDNS1Saved = lucDNS1;
  gucDNS2Saved = lucDNS2;
  gucDNS3Saved = lucDNS3;
  gucDNS4Saved = lucDNS4;
  gucGateway1Saved = lucGateway1;
  gucGateway2Saved = lucGateway2;
  gucGateway3Saved = lucGateway3;
  gucGateway4Saved = lucGateway4;
  gucNetmask1Saved = lucNetmask1;
  gucNetmask2Saved = lucNetmask2;
  gucNetmask3Saved = lucNetmask3;
  gucNetmask4Saved = lucNetmask4;

  // Refresh the screen
  refreshDisplaySavedAddresses();
}

void ScreenIPAddressesView::functionRevertAddresses()
{
  // Deselect address byte
  lucSelectedAddressByte = 0;
  // Reset all the address byte flex button borders to normal
  resetAddressButtonBorders();

  // Make the number picker invisible
  numberPICKERAddress.setVisible(false);
  numberPICKERAddress.invalidate();

  // Restore the saved address bytes
  lucAddress1 = gucAddress1Saved;
  lucAddress2 = gucAddress2Saved;
  lucAddress3 = gucAddress3Saved;
  lucAddress4 = gucAddress4Saved;
  lucDNS1 = gucDNS1Saved;
  lucDNS2 = gucDNS2Saved;
  lucDNS3 = gucDNS3Saved;
  lucDNS4 = gucDNS4Saved;
  lucGateway1 = gucGateway1Saved;
  lucGateway2 = gucGateway2Saved;
  lucGateway3 = gucGateway3Saved;
  lucGateway4 = gucGateway4Saved;
  lucNetmask1 = gucNetmask1Saved;
  lucNetmask2 = gucNetmask2Saved;
  lucNetmask3 = gucNetmask3Saved;
  lucNetmask4 = gucNetmask4Saved;

  // Refresh the screen
  refreshDisplaySavedAddresses();
}

void ScreenIPAddressesView::resetAddressButtonBorders()
{
  // Reset all the address byte flex button borders to normal

  flexButtonIPAddress1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonIPAddress1.invalidate();
  flexButtonIPAddress2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonIPAddress2.invalidate();
  flexButtonIPAddress3.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonIPAddress3.invalidate();
  flexButtonIPAddress4.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonIPAddress4.invalidate();

  flexButtonDNS1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonDNS1.invalidate();
  flexButtonDNS2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonDNS2.invalidate();
  flexButtonDNS3.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonDNS3.invalidate();
  flexButtonDNS4.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonDNS4.invalidate();

  flexButtonGateway1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonGateway1.invalidate();
  flexButtonGateway2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonGateway2.invalidate();
  flexButtonGateway3.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonGateway3.invalidate();
  flexButtonGateway4.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonGateway4.invalidate();

  flexButtonNetmask1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonNetmask1.invalidate();
  flexButtonNetmask2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonNetmask2.invalidate();
  flexButtonNetmask3.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonNetmask3.invalidate();
  flexButtonNetmask4.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(255, 238, 0), touchgfx::Color::getColorFromRGB(0, 153, 204), touchgfx::Color::getColorFromRGB(0, 0, 0), touchgfx::Color::getColorFromRGB(255, 255, 255));
  flexButtonNetmask4.invalidate();
}

void ScreenIPAddressesView::refreshDisplaySavedAddresses()
{
  // Refresh the display of address bytes from the saved values

  Unicode::snprintf(textAreaIPAddress1Buffer, TEXTAREAIPADDRESS1_SIZE, "%d", gucAddress1Saved);
  textAreaIPAddress1.invalidate();
  Unicode::snprintf(textAreaIPAddress2Buffer, TEXTAREAIPADDRESS2_SIZE, "%d", gucAddress2Saved);
  textAreaIPAddress2.invalidate();
  Unicode::snprintf(textAreaIPAddress3Buffer, TEXTAREAIPADDRESS3_SIZE, "%d", gucAddress3Saved);
  textAreaIPAddress3.invalidate();
  Unicode::snprintf(textAreaIPAddress4Buffer, TEXTAREAIPADDRESS4_SIZE, "%d", gucAddress4Saved);
  textAreaIPAddress4.invalidate();

  Unicode::snprintf(textAreaDNS1Buffer, TEXTAREADNS1_SIZE, "%d", gucDNS1Saved);
  textAreaDNS1.invalidate();
  Unicode::snprintf(textAreaDNS2Buffer, TEXTAREADNS2_SIZE, "%d", gucDNS2Saved);
  textAreaDNS2.invalidate();
  Unicode::snprintf(textAreaDNS3Buffer, TEXTAREADNS3_SIZE, "%d", gucDNS3Saved);
  textAreaDNS3.invalidate();
  Unicode::snprintf(textAreaDNS4Buffer, TEXTAREADNS4_SIZE, "%d", gucDNS4Saved);
  textAreaDNS4.invalidate();

  Unicode::snprintf(textAreaGateway1Buffer, TEXTAREAGATEWAY1_SIZE, "%d", gucGateway1Saved);
  textAreaGateway1.invalidate();
  Unicode::snprintf(textAreaGateway2Buffer, TEXTAREAGATEWAY2_SIZE, "%d", gucGateway2Saved);
  textAreaGateway2.invalidate();
  Unicode::snprintf(textAreaGateway3Buffer, TEXTAREAGATEWAY3_SIZE, "%d", gucGateway3Saved);
  textAreaGateway3.invalidate();
  Unicode::snprintf(textAreaGateway4Buffer, TEXTAREAGATEWAY4_SIZE, "%d", gucGateway4Saved);
  textAreaGateway4.invalidate();

  Unicode::snprintf(textAreaNetmask1Buffer, TEXTAREANETMASK1_SIZE, "%d", gucNetmask1Saved);
  textAreaNetmask1.invalidate();
  Unicode::snprintf(textAreaNetmask2Buffer, TEXTAREANETMASK2_SIZE, "%d", gucNetmask2Saved);
  textAreaNetmask2.invalidate();
  Unicode::snprintf(textAreaNetmask3Buffer, TEXTAREANETMASK3_SIZE, "%d", gucNetmask3Saved);
  textAreaNetmask3.invalidate();
  Unicode::snprintf(textAreaNetmask4Buffer, TEXTAREANETMASK4_SIZE, "%d", gucNetmask4Saved);
  textAreaNetmask4.invalidate();
}
