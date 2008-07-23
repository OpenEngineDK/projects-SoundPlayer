#include "Factory.h"
#include "SoundKeyHandler.h"

// from OpenEngine
#include <Logging/Logger.h>
#include <Display/Frustum.h>
#include <Display/Viewport.h>
#include <Display/ViewingVolume.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Resources/ResourceManager.h>
#include <Utils/Statistics.h>

// from extensions
#include <Display/SDLFrame.h>
#include <Devices/SDLInput.h>

#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>

#include <Sound/OpenALSoundManager.h>
#include <Resources/OpenALSoundResource.h>
#include <Resources/ISoundResource.h>

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

Factory::Factory() {
    frame    = new SDLFrame(800, 600, 32);
    viewport = new Viewport(*frame);
        
    camera = new Camera(*(new ViewingVolume()));
    camera->SetPosition(Vector<3,float>(0,20,80));
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

	ISoundResourcePtr sound = 
	  ResourceManager<ISoundResource>::Create(filename);
	SoundNode* soundNode = new SoundNode(sound);
	root->AddNode(soundNode);

        engine.AddModule(*(new OpenALSoundManager(root)));
	soundNode->Play();

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
