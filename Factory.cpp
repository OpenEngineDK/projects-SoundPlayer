#include "Factory.h"
#include "SoundKeyHandler.h"
#include "TransformationUpdater.h"
#include "CircleUpdate.h"

// from OpenEngine
#include <Logging/Logger.h>
#include <Display/Frustum.h>
#include <Display/Viewport.h>
#include <Display/ViewingVolume.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Resources/ResourceManager.h>
#include <Utils/Statistics.h>

#include <Display/SDLFrame.h>
#include <Devices/SDLInput.h>

#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>

#include <Sound/OpenALSoundSystem.h>
#include <Sound/ISoundSystem.h>
#include <Sound/ISound.h>
#include <Scene/SoundNode.h>
#include <Sound/SoundRenderer.h>
#include <Sound/MusicPlayer.h>

#include <Resources/VorbisResource.h>
#include <Resources/ISoundResource.h>

#include <Scene/SphereNode.h>

#include <Scene/DirectionalLightNode.h>
#include <Scene/PointLightNode.h>

#include <Utils/MoveHandler.h>
#include <Utils/QuitHandler.h>

#include <string>

using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Sound;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

string Factory::filename = "";



class PlayHandler : public IListener<KeyboardEventArg> {

private:
    SoundNode* sn;
public:

    PlayHandler(SoundNode* sn): sn(sn){}
    ~PlayHandler() {}
    
    void Handle(KeyboardEventArg arg) {
        if (arg.type == KeyboardEventArg::PRESS) {
            switch (arg.sym) {
            case KEY_p: sn->GetSound()->Play(); break;
            case KEY_o: sn->GetSound()->Stop(); break;
            case KEY_i: sn->GetSound()->Pause(); break;
            case KEY_UP: sn->GetSound()->SetMaxDistance(sn->GetSound()->GetMaxDistance()+1); break;
            case KEY_DOWN: sn->GetSound()->SetMaxDistance(sn->GetSound()->GetMaxDistance()-1); break;
            default: 
                break;
            }
        }
    }
    
    void BindToEventSystem() {
        IKeyboard::keyEvent.Attach(*this);
    }
};


Factory::Factory() {
    frame    = new SDLFrame(800, 600, 32);
    viewport = new Viewport(*frame);
        
    camera = new Camera(*(new ViewingVolume()));
    camera->SetPosition(Vector<3,float>(0,0,0));
    //viewport->SetViewingVolume(camera);

    // frustum hack
    Frustum* frustum = new Frustum(*camera);
    frustum->SetFar(1000);
    frustum->SetNear(0.0001);
    viewport->SetViewingVolume(frustum);

    renderer = new Renderer();
    renderer->process.Attach(*(new RenderingView(*viewport)));
}

Factory::~Factory() {
    delete frame;
    delete viewport;
    delete camera;
    delete renderer;
}

bool Factory::SetupEngine(IGameEngine& engine) {
    try {
        // Setup input handling
        SDLInput* input = new SDLInput();
        engine.AddModule(*input);


        // Register the handler as a listener on up and down keyboard events.
        MoveHandler* move_h = new MoveHandler(*camera);
        engine.AddModule(*move_h);
        move_h->BindToEventSystem();
        QuitHandler* quit_h = new QuitHandler();
        quit_h->BindToEventSystem();

        // Create scene root
        SceneNode* root = new SceneNode();
        this->renderer->SetSceneRoot(root);

        // Add models from models.txt to the scene
        // First we set the resources directory
        DirectoryManager::AppendPath("/");
        DirectoryManager::AppendPath(""); //current directory
		DirectoryManager::AppendPath("/projects/SoundPlayer/data");
        
        // load the resource plug-ins
        ResourceManager<ISoundResource>::AddPlugin(new VorbisResourcePlugin());
        //ResourceManager<IMovieResource>::AddPlugin(new FFMPEGPlugin());

        // visualize sound
        TransformationNode* tn = new TransformationNode();
        TransformationNode* tn2 = new TransformationNode();

        tn2->Move(0,0,-100);
        PointLightNode* dln = new PointLightNode();
        dln->linearAtt = 0.001;
        dln->quadAtt = 0.0001;
        dln->constAtt = 0.5;

        root->AddNode(tn);
        root->AddNode(tn2);
        tn2->AddNode(dln);

        SphereNode* sphere2 = new SphereNode();        
        dln->AddNode(sphere2);
        ISoundSystem* openalsmgr = new OpenALSoundSystem(root, camera);
        engine.AddModule(*openalsmgr);

        ISoundResourcePtr soundres = 
            ResourceManager<ISoundResource>::Create(filename);

        ISound* sound = openalsmgr->CreateSound(soundres);
        sound->SetMaxDistance(10);
        sound->SetLooping(true);
        SoundNode* soundNode = new SoundNode(sound);

        SoundRenderer* sr = new SoundRenderer();
        renderer->preProcess.Attach(*sr);
        sr->AddSoundNode(soundNode);
        tn->AddNode(soundNode);

		//backgroundsound test
		MusicPlayer* player = new MusicPlayer(camera, openalsmgr);
		int id1 = player->AddBackGroundSound("batmanfo.ogg");
		int id2 = player->AddBackGroundSound("Beastie Boys - Now Get Busy.ogg");
		(player->GetBackGroundSound(id2))->SetGain(0.1f);
		engine.AddModule(*player);		

        // move the transformation node in a circle
        CircleUpdate* cu = new CircleUpdate(Vector<3,float>(0,0,0), 50, 0.2, -PI*0.5 - PI*0.25);
        TransformationUpdater* tu  = 
            new TransformationUpdater(tn, cu);
        engine.AddModule(*tu);
        
        PlayHandler* ph = new PlayHandler(soundNode);
        ph->BindToEventSystem();

        engine.AddModule(*(new Statistics(1000)));

    } catch (const Exception& ex) {
        logger.error << "An exception occurred: " << ex.what() << logger.end;
        throw ex;
    }
    // Return true to signal success.
    return true;
}

// Get methods for the factory.
IFrame*      Factory::GetFrame()      { return frame; }
IRenderer*   Factory::GetRenderer()   { return renderer; }
