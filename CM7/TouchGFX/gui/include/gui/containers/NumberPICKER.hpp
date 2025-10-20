#ifndef NUMBERPICKER_HPP
#define NUMBERPICKER_HPP

#include <gui_generated/containers/NumberPICKERBase.hpp>

class NumberPICKER : public NumberPICKERBase
{
public:
    NumberPICKER();
    virtual ~NumberPICKER() {}

    virtual void initialize();

    /*********************************************************************************/
    /**********     UPDATE THE CONTAINED ITEMS OF numberDateScrollWheel     **********/
    /*********************************************************************************/
    virtual void numberScrollWheelUpdateItem(numberTemplateScrollCustomContainer& item, int16_t itemIndex)
    {
        item.setNumber(itemIndex);
        item.setColorText();
    }
    /*********************************************************************************/
    /**********    UPDATE THE ITEM BASED ON THE SELECTED STYLE TEMPLATE     **********/
    /**********               OF THE numberDateScrollWheel                  **********/
    /*********************************************************************************/
    virtual void numberScrollWheelUpdateCenterItem(numberTemplateScrollCustomContainer& item, int16_t itemIndex)
    {
        item.setNumber(itemIndex);
    }
  
    /*********************************************************************************/
    /**********       RETURN THE SELECT ITEM OF numberDateScrollWheel      ***********/
    /*********************************************************************************/
    int16_t getValue (){
        return numberScrollWheel.getSelectedItem();
    }
    
    // Set the number scroll wheel to a given index
    virtual void setValue(uint16_t auiSetValue)
    {
      numberScrollWheel.animateToItem(auiSetValue);
    }

protected:
};

#endif // NUMBERPICKER_HPP
