#include "Factory.h"
#include "SoundKeyHandler.h"
#include "TransformationUpdater.h"


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

#include <Sound/OpenALSoundManager.h>
#include <Sound/ISound.h>
#include <Sound/OpenALSound.h>
#include <Scene/SoundNode.h>


#include <Resources/OpenALSoundResource.h>
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
using namespace OpenEngine::Sound;

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
        
        // load the resource plug-ins
        ResourceManager<ISoundResource>::AddPlugin(new OpenALSoundPlugin());
        //ResourceManager<IMovieResource>::AddPlugin(new FFMPEGPlugin());

        // visualize sound
        TransformationNode* tn = new TransformationNode();
        TransformationNode* tn2 = new TransformationNode();
        tn->Scale(5,5,5);
        tn2->Move(0,0,-100);
        PointLightNode* dln = new PointLightNode();
        dln->linearAtt = 0.001;
        dln->quadAtt = 0.0001;
        dln->constAtt = 0.5;

        MaterialPtr m = MaterialPtr(new Material());
//         m->ambient = Vector<4,float>(0.0,1.0,0.0,1.0);
        m->specular = Vector<4,float>(0.0,1.0,0.0,1.0);
        m->diffuse = Vector<4,float>(0.0,0.0,0.4,1.0);
        m->shininess = 0;
        m->emission = Vector<4,float>(0.1,0.0,0.0,1.0);

        root->AddNode(tn);
        root->AddNode(tn2);
        tn2->AddNode(dln);

        SphereNode* sphere = new SphereNode(m,30,30);
        SphereNode* sphere2 = new SphereNode();        
        dln->AddNode(sphere2);
        tn->AddNode(sphere);
	OpenALSoundManager* openalsmgr = new OpenALSoundManager(root, camera);
        engine.AddModule(*openalsmgr);


        ISoundResourcePtr soundres = 
            ResourceManager<ISoundResource>::Create(filename);

	ISound* sound = openalsmgr->CreateSound(soundres);
        SoundNode* soundNode = new SoundNode(sound);
        tn->AddNode(soundNode);

        // move the transformation node in a circle
        TransformationUpdater* tu  = 
            new TransformationUpdater(tn, Vector<3,float>(0,0,0), 100, 0.1);
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
