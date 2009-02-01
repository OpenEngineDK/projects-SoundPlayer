#ifndef _SOUND_KEY_HANDLER_
#define _SOUND_KEY_HANDLER_

// from core
#include <Core/IListener.h>
#include <Core/EngineEvents.h>
#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>
#include <Scene/SoundNode.h>

using OpenEngine::Core::IListener;
using OpenEngine::Core::ProcessEventArg;
using OpenEngine::Scene::SoundNode;

using namespace OpenEngine::Devices;

class SoundKeyHandler : public IListener<KeyboardEventArg> {
private:
    SoundNode* sn;
public:
    SoundKeyHandler(SoundNode* sn): sn(sn){}
    virtual ~SoundKeyHandler() {}
    void Handle(KeyboardEventArg arg) {
        if (arg.type == EVENT_PRESS) {
            switch (arg.sym) {
            case KEY_p: 
	      sn->GetSound()->Play();
	      break;
            case KEY_o: 
	      sn->GetSound()->Stop(); 
	      break;
            case KEY_i:
	      sn->GetSound()->Pause(); 
	      break;
            case KEY_UP: 
	      sn->GetSound()->SetMaxDistance(sn->GetSound()->GetMaxDistance()+1); 
	      break;
            case KEY_DOWN: 
	      sn->GetSound()->SetMaxDistance(sn->GetSound()->GetMaxDistance()-1); 
	      break;
            default: 
                break;
            }
        }
    }
};

#endif
