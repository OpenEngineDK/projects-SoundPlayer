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

void TransformationUpdater::Handle(ProcessEventArg arg) {
    float deltaTime = arg.approx / 1000.0; //@todo: salomon!
    if (active)
        s->Update(tn, deltaTime);
}

void TransformationUpdater::SetActive(bool state) {
    active = state;
}

} // NS Utils
} // NS OpenEngine

