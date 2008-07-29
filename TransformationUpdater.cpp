// Update a transformation node in time intervals
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "TransformationUpdater.h"

namespace OpenEngine {
namespace Utils {


TransformationUpdater::TransformationUpdater(TransformationNode* tn, IUpdateStrategy* s): 
    tn(tn), s(s), active(true)
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
    if (active)
        s->Update(tn, deltaTime, percent);
}

void TransformationUpdater::SetActive(bool state) {
    active = state;
}

void TransformationUpdater::Deinitialize() {
}

bool TransformationUpdater::IsTypeOf(const std::type_info& inf) {
    return ((typeid(TransformationUpdater) == inf));
}

} // NS Utils
} // NS OpenEngine

