// Update a transformation node in time intervals
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "TransformationUpdater.h"

#include <Logging/Logger.h>
#include <Scene/TransformationNode.h>
#include <Math/Math.h>

namespace OpenEngine {
namespace Utils {

using namespace OpenEngine::Math;

TransformationUpdater::TransformationUpdater(TransformationNode* tn, 
                                             Vector<3,float> center,
                                             float radius,
                                             float speed) : 
    tn(tn), 
    center(center),
    radius(radius),
    angle(0.0),
    speed(speed)
{

}

/**
 * TransformationUpdater destructor.
 * Performs no clean up.
 */
TransformationUpdater::~TransformationUpdater() {

}

void TransformationUpdater::Initialize() {
}

void TransformationUpdater::Process(const float deltaTime, const float percent) {
    Vector<3,float> pos(center[0]+radius*cos(angle), center[0], center[2]+radius*sin(angle));
    tn->SetPosition(pos);
    angle += deltaTime * speed/1000;
    if (angle > 2*PI)
        angle = angle-2*PI;
    //    logger.info << "Sphere position: " << pos << logger.end;
}

void TransformationUpdater::Deinitialize() {
}

bool TransformationUpdater::IsTypeOf(const std::type_info& inf) {
    return ((typeid(TransformationUpdater) == inf));
}

} // NS Utils
} // NS OpenEngine

