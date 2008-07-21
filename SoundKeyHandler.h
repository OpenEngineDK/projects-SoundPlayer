#ifndef _SOUND_KEY_HANDLER_
#define _SOUND_KEY_HANDLER_

// from core
#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>

using namespace OpenEngine::Devices;

class SoundKeyHandler : public IListener<KeyboardEventArg> {
private:
    //IMovieResourcePtr movie;
    bool pause;
public:
    //MovieKeyHandler(IMovieResourcePtr movie) : movie(movie), pause(false) {}
    void Handle(KeyboardEventArg arg) {
        if (arg.type == KeyboardEventArg::PRESS
	    && arg.sym == KEY_SPACE) {
            pause = !pause;
            //if(movie!=NULL)
	        //movie->Pause(pause);
        }
    }

    void RegisterWithEngine(IGameEngine& engine) {
        IKeyboard::keyEvent.Attach(*this);
    }
};

#endif
