// Update a transformation node in time intervals
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _CIRCLE_UPDATE_H_
#define _CIRCLE_UPDATE_H_


#include "TransformationUpdater.h"
#include <Math/Vector.h>

namespace OpenEngine {
    // forward declarations
    namespace Scene {
        class TransformationNode;
    }

namespace Utils {

using OpenEngine::Math::Vector;
using OpenEngine::Scene::TransformationNode;

class CircleUpdate: public IUpdateStrategy {
private:
    TransformationNode* tn;
    Vector<3,float> center;
    float radius;
    float angle;
    float speed;
public:
    CircleUpdate(Vector<3,float> center,
                 float radius,
                 float speed,
                 float angle);

    virtual ~CircleUpdate();
    virtual void Update(TransformationNode* t, const float deltaTime, const float percent);
};

} // NS Utils
} // NS OpenEngine

#endif
