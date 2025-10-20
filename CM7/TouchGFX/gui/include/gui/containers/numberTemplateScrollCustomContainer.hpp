#ifndef NUMBERTEMPLATESCROLLCUSTOMCONTAINER_HPP
#define NUMBERTEMPLATESCROLLCUSTOMCONTAINER_HPP

#include <gui_generated/containers/numberTemplateScrollCustomContainerBase.hpp>
#include <touchgfx/Color.hpp>
class numberTemplateScrollCustomContainer : public numberTemplateScrollCustomContainerBase
{
public:
    numberTemplateScrollCustomContainer();
    virtual ~numberTemplateScrollCustomContainer() {}

    virtual void initialize();
    
    /*********************************************************************************/
    /**********                 PUT COLOR TO numberTextArea                ***********/
    /*********************************************************************************/
    virtual void setColorText(){ 
      numberTextArea.setColor(touchgfx::Color::getColorFromRGB(230, 230, 230));
      //// TEST MAB 2025.10.02 numberTextArea.setColor(touchgfx::Color::getColorFromRGB(0, 230, 0)); // green
    }    

    /*********************************************************************************/
    /**********              PUT VALUE TO numberTextAreabuffer             ***********/
    /*********************************************************************************/
    void setNumber(int value){ 
        Unicode::itoa(value, numberTextAreaBuffer, NUMBERTEXTAREA_SIZE, 10);
    }
protected:
};

#endif // NUMBERTEMPLATESCROLLCUSTOMCONTAINER_HPP
