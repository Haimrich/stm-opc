#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

extern "C" 
{
#include "opcua.h"
}

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void updateTime(Tempo tempo) {}
    virtual void updateMemoryGauge(double availMemory) {}

    virtual void updateMainTankTemp(double value) {}
    virtual void updateMainTankLevel(double value) {}
    virtual void updateBoilerTemp(double value) {}
    virtual void updateBoilerLevel(double value) {}
    virtual void updateBoilerPressure(double value) {}
    virtual void updateTurbinePower(double value) {}
    virtual void updateValveStatus(bool value) {}
    virtual void updatePumpStatus(bool value) {}
    virtual void updateFireStatus(bool value) {}

    virtual void updateCurrentTime(Tempo tempo) {}
    virtual void consolePrint() {}

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
