// Update a transformation node in time intervals
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _TRANSFORMATION_UPDATER_H_
#define _TRANSFORMATION_UPDATER_H_

#include <Core/IListener.h>
#include <Core/EngineEvents.h>
#include <Math/Vector.h>

namespace OpenEngine {
    // forward declarations
    namespace Scene {
        class TransformationNode;
    }

namespace Utils {

using OpenEngine::Core::IListener;
using OpenEngine::Core::ProcessEventArg;
using OpenEngine::Scene::TransformationNode;

class IUpdateStrategy {
public:
    virtual ~IUpdateStrategy() {};
    
    virtual void Update(TransformationNode* t, const float deltaTime) = 0;
};

class TransformationUpdater: public IListener<ProcessEventArg> {
private:
    TransformationNode* tn;
    IUpdateStrategy* s;
    bool active;
public:
    TransformationUpdater(TransformationNode* tn, IUpdateStrategy* s);
    virtual ~TransformationUpdater();

    void SetActive(bool state);

    void Handle(ProcessEventArg arg);
};

} // NS Utils
} // NS OpenEngine

#endif
