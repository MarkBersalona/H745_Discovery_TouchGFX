#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#ifndef SIMULATOR
#include "main.h"

extern "C"
{
extern osMessageQueueId_t MainQueueHandle;
extern osMessageQueueId_t TouchGFXQueueHandle;
extern UART_HandleTypeDef huart1;
}
#endif // SIMULATOR

Model::Model() : modelListener(0), uiCurrentZone (33)
{

}

void Model::tick()
{
  #ifndef SIMULATOR
  //////////////////////////////////////////////
  //
  // Pull messages from the queue and process
  //
  //////////////////////////////////////////////
  uint8_t lucTouchGFXQueueCount = osMessageQueueGetCount(TouchGFXQueueHandle);
  uint16_t luiTouchGFXQueueEventID;
  uint16_t luiTouchGFXQueuePayload;
  if (lucTouchGFXQueueCount > 0)
  {
    osStatus_t ltTouchGFXQueueStatus = osMessageQueueGet(TouchGFXQueueHandle, &luiTouchGFXQueueEventID, NULL, 0);
    if (ltTouchGFXQueueStatus  != osOK) LOG("%s: *** WARNING *** Message queue osMessageQueueGet() returned = %d\r\n", __FUNCTION__, ltTouchGFXQueueStatus);

    switch (luiTouchGFXQueueEventID)
    {
    case msgid_MAIN_TOUCHGFX_SET_CURRENT_ZONE:
      osMessageQueueGet(TouchGFXQueueHandle, &luiTouchGFXQueuePayload, NULL, 0);
      LOG("%s: Main sent TouchGFX new Z-Wave zone: %d\r\n", __FUNCTION__, luiTouchGFXQueuePayload);
      Model::uiCurrentZone = luiTouchGFXQueuePayload;
      modelListener->notifyZoneChanged(Model::uiCurrentZone);
      break;

    case msgid_MAIN_TOUCHGFX_STANDBY_UPDATE:
      LOG("%s: Main sent TouchGFX notification of Standby update...\r\n", __FUNCTION__);
      modelListener->notifyStandbyChanged();
      break;

    default:
      if (luiTouchGFXQueueEventID == msgid_NOP)
      {
        // do nothing
      }
      else
      {
        LOG("%s: ***ERROR*** Invalid command %d\r\n", __FUNCTION__, luiTouchGFXQueueEventID)
      }
      break;
    }
  }
  #endif // !SIMULATOR

}

unsigned int  Model::getCurrentZone()
{
  return Model::uiCurrentZone;
}

void Model::setCurrentZone(unsigned int auiCandidateNewZone)
{
  #ifndef SIMULATOR
  unsigned int luiMessageQueueBuffer;
  #endif // !SIMULATOR

  if (auiCandidateNewZone >= 1 && auiCandidateNewZone <= 50)
  {
    // New zone number is within zone limits
    Model::uiCurrentZone = auiCandidateNewZone;
    #ifndef SIMULATOR
    // Send the new zone number to Main task
    // (Let Main task worry about converting the zone slot number to the correct zone for Input and other tasks)
    luiMessageQueueBuffer = msgid_TOUCHGFX_MAIN_SET_CURRENT_ZONE;
    osMessageQueuePut(MainQueueHandle, &luiMessageQueueBuffer, 0, 0);
    luiMessageQueueBuffer = auiCandidateNewZone;
    osMessageQueuePut(MainQueueHandle, &luiMessageQueueBuffer, 0, 0);
    #endif // !SIMULATOR
  }
}
