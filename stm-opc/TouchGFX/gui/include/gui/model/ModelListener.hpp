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
    virtual void updateGauge(double availMemory) {}

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
