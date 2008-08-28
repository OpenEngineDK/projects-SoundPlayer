#ifndef _MUSIC_PLAYER_KEY_HANDLER_
#define _MUSIC_PLAYER_KEY_HANDLER_

// from core
#include <Core/IListener.h>
#include <Core/EngineEvents.h>
#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>
#include <Sound/MusicPlayer.h>

using OpenEngine::Core::IListener;
using OpenEngine::Core::ProcessEventArg;
using OpenEngine::Sound::MusicPlayer;

using namespace OpenEngine::Devices;

class MusicPlayerKeyHandler : public IListener<KeyboardEventArg> {
private:
    MusicPlayer& mplayer;
public:
 MusicPlayerKeyHandler(MusicPlayer& mplayer): mplayer(mplayer) {}
    virtual ~MusicPlayerKeyHandler() {}
    void Handle(KeyboardEventArg arg) {
        if (arg.type == KeyboardEventArg::PRESS) {
            switch (arg.sym) {
            case KEY_g: 
                mplayer.Play();
                break;
            case KEY_h: 
                mplayer.Stop(); 
                break;
            case KEY_j:
                mplayer.Pause(); 
                break;
            case KEY_l:
                mplayer.Next();
                break;
            case KEY_k:
                mplayer.Previous(); 
                break;
            default: 
                break;
            }
        }
    }
};

#endif // _MUSIC_PLAYER_KEY_HANDLER_
