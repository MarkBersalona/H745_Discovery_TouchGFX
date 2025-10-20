#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    // Get current Zone number (1-50) corresponding to Z-Wave sensors
    unsigned int getCurrentZone ();

    // Set current Zone number (1-50)
    void setCurrentZone(unsigned int auiCandidateNewZone);

    void tick();
protected:
    ModelListener* modelListener;
    unsigned int uiCurrentZone;
};

#endif // MODEL_HPP
