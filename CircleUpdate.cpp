// Update a transformation node in time intervals
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "CircleUpdate.h"

#include <Logging/Logger.h>
#include <Scene/TransformationNode.h>
#include <Math/Math.h>

namespace OpenEngine {
namespace Utils {

using namespace OpenEngine::Math;

CircleUpdate::CircleUpdate(Vector<3,float> center,
                           float radius,
                           float speed,
                           float angle=0) : 
    center(center),
    radius(radius),
    speed(speed),
    angle(angle)
{
  
}

/**
 * CircleUpdate destructor.
 * Performs no clean up.
 */
CircleUpdate::~CircleUpdate() {

}

void CircleUpdate::Update(TransformationNode* t, const float deltaTime, const float percent) {
    Vector<3,float> pos(center[0]+radius*cos(angle), center[0], center[2]+radius*sin(angle));
    t->SetPosition(pos);
    angle += deltaTime * speed/1000;
    if (angle > 2*PI)
        angle = angle-2*PI;
    //logger.info << "Sphere position: " << pos << logger.end;
}

} // NS Utils
} // NS OpenEngine

