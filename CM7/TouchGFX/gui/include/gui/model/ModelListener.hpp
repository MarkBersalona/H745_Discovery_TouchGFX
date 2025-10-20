#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    // Notify the current Zone number has changed
    // Per default, use an empty implementation so that only those
    // Presenters interested in this specific event need to
    // override this function.
    virtual void notifyZoneChanged(unsigned int newZone) {}

    // Notify the Standby mode and/or countdown has changed
    // Per default, use an empty implementation so that only those
    // Presenters interested in this specific event need to
    // override this function.
    virtual void notifyStandbyChanged() {}

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
