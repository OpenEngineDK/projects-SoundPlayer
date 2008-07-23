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

#include <Core/IModule.h>
#include <Math/Vector.h>

namespace OpenEngine {
    // forward declarations
    namespace Scene {
        class TransformationNode;
    }

namespace Utils {

using OpenEngine::Core::IModule;
using OpenEngine::Math::Vector;
using OpenEngine::Scene::TransformationNode;

class TransformationUpdater: public IModule {
private:
    TransformationNode* tn;
    Vector<3,float> center;
    float radius;
    float angle;
    float speed;
public:
    TransformationUpdater(TransformationNode* tn, 
                          Vector<3,float> center,
                          float radius,
                          float speed);

    virtual ~TransformationUpdater();

    void Initialize();
    void Process(const float deltaTime, const float percent);
    void Deinitialize();
    bool IsTypeOf(const std::type_info& inf);
};

} // NS Utils
} // NS OpenEngine

#endif
