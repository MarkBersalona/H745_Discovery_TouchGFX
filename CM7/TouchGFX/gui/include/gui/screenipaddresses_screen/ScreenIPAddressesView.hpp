#ifndef SCREENIPADDRESSESVIEW_HPP
#define SCREENIPADDRESSESVIEW_HPP

#include <gui_generated/screenipaddresses_screen/ScreenIPAddressesViewBase.hpp>
#include <gui/screenipaddresses_screen/ScreenIPAddressesPresenter.hpp>

class ScreenIPAddressesView : public ScreenIPAddressesViewBase
{
public:
    ScreenIPAddressesView();
    virtual ~ScreenIPAddressesView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void actionWriteCounterValue(uint8_t value);
    virtual void actionSelectAddressByte(uint8_t value);
    virtual void functionUpdateSelectedAddressWithCounter();
    virtual void functionSaveAddresses();
    virtual void functionRevertAddresses();
    virtual void resetAddressButtonBorders();
    virtual void refreshDisplaySavedAddresses();

//    typedef enum Address_buttons
//    {
//      addrButton_ADDRESS_1,
//      addrButton_ADDRESS_2,
//      addrButton_ADDRESS_3,
//      addrButton_ADDRESS_4,
//
//      addrButton_DNS_1,
//      addrButton_DNS_2,
//      addrButton_DNS_3,
//      addrButton_DNS_4,
//
//      addrButton_GATEWAY_1,
//      addrButton_GATEWAY_2,
//      addrButton_GATEWAY_3,
//      addrButton_GATEWAY_4,
//
//      addrButton_NETMASK_1,
//      addrButton_NETMASK_2,
//      addrButton_NETMASK_3,
//      addrButton_NETMASK_4,
//
//    } Address_button_ID;

protected:

#ifdef SIMULATOR
    // Temporary storage for address bytes
    // (until the true IP address storage is implemented)
    uint8_t gucAddress1Saved = 10;
    uint8_t gucAddress2Saved = 1;
    uint8_t gucAddress3Saved = 2;
    uint8_t gucAddress4Saved = 33;
    uint8_t gucDNS1Saved = 10;
    uint8_t gucDNS2Saved = 1;
    uint8_t gucDNS3Saved = 2;
    uint8_t gucDNS4Saved = 81;
    uint8_t gucGateway1Saved = 10;
    uint8_t gucGateway2Saved = 1;
    uint8_t gucGateway3Saved = 2;
    uint8_t gucGateway4Saved = 1;
    uint8_t gucNetmask1Saved = 255;
    uint8_t gucNetmask2Saved = 255;
    uint8_t gucNetmask3Saved = 255;
    uint8_t gucNetmask4Saved = 0;
#endif

    // Working values for address bytes
    uint8_t lucAddress1;
    uint8_t lucAddress2;
    uint8_t lucAddress3;
    uint8_t lucAddress4;
    uint8_t lucDNS1;
    uint8_t lucDNS2;
    uint8_t lucDNS3;
    uint8_t lucDNS4;
    uint8_t lucGateway1;
    uint8_t lucGateway2;
    uint8_t lucGateway3;
    uint8_t lucGateway4;
    uint8_t lucNetmask1;
    uint8_t lucNetmask2;
    uint8_t lucNetmask3;
    uint8_t lucNetmask4;

    // Selected address byte
    // 1-4   Address byte
    // 5-8   DNS byte
    // 9-12  Gateway byte
    // 13-16 Netmask byte
    // else  no address byte selected
    uint8_t lucSelectedAddressByte = 0;
};

#endif // SCREENIPADDRESSESVIEW_HPP
