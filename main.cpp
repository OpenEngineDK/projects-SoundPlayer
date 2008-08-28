// Main setup for the OpenEngine SoundPlayer project.
// -------------------------------------------------------------------
// Copyright (C) 2008 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine stuff
#include <Meta/Config.h>

// Core structures
#include <Core/Engine.h>

// Display structures
#include <Display/Camera.h>
#include <Display/Frustum.h>
#include <Display/ViewingVolume.h>
// SDL implementation
#include <Display/SDLFrame.h>
#include <Devices/SDLInput.h>

// OpenGL rendering implementation
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Renderers/OpenGL/TextureLoader.h>

// Resources
#include <Resources/IModelResource.h>
#include <Resources/DirectoryManager.h>
#include <Resources/ResourceManager.h>
// Sound plugins
#include <Resources/VorbisResource.h>

// Scene structures
#include <Scene/SceneNode.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Scene/PointLightNode.h>

// Utilities and logger
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Utils/MoveHandler.h>
#include <Utils/QuitHandler.h>
#include <Utils/Statistics.h>

// Sound things
#include <Sound/OpenALSoundSystem.h>
#include <Sound/ISoundSystem.h>
#include <Sound/ISound.h>
#include <Scene/SoundNode.h>
#include <Sound/SoundRenderer.h>
#include <Sound/MusicPlayer.h>
#include <Scene/SphereNode.h>

#include "CircleUpdate.h"
#include "TransformationUpdater.h"
#include "MusicPlayerKeyHandler.h"
#include "SoundKeyHandler.h"

// Additional namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Sound;
using namespace OpenEngine::Utils;

// Configuration structure to pass around to the setup methods
struct Config {
    IEngine&              engine;
    IFrame*               frame;
    Viewport*             viewport;
    IViewingVolume*       viewingvolume;
    Camera*               camera;
    Frustum*              frustum;
    IRenderer*            renderer;
    IMouse*               mouse;
    IKeyboard*            keyboard;
    ISceneNode*           scene;
    bool                  resourcesLoaded;
    Config(IEngine& engine)
        : engine(engine)
        , frame(NULL)
        , viewport(NULL)
        , viewingvolume(NULL)
        , camera(NULL)
        , frustum(NULL)
        , renderer(NULL)
        , mouse(NULL)
        , keyboard(NULL)
        , scene(NULL)
        , resourcesLoaded(false)
    {}
};

// Forward declaration of the setup methods
void SetupResources(Config&);
void SetupDisplay(Config&);
void SetupScene(Config&);
void SetupRendering(Config&);
void SetupDebugging(Config&);

int main(int argc, char** argv) {
    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    // Print usage info.
    logger.info << "========= Running The OpenEngine SoundPlayer Project =========" << logger.end;
    logger.info << "This project is a simple testing project for OpenEngine." << logger.end;
    logger.info << logger.end;
    logger.info << "Camera controls:" << logger.end;
    logger.info << "  move forwards:   w" << logger.end;
    logger.info << "  move backwards:  s" << logger.end;
    logger.info << "  move left:       a" << logger.end;
    logger.info << "  move right:      d" << logger.end;
    logger.info << "  rotate:          mouse" << logger.end;
    logger.info << logger.end;

    // Create an engine and config object
    Engine* engine = new Engine();
    Config config(*engine);

    // Setup the engine
    SetupResources(config);
    SetupDisplay(config);
    SetupScene(config);
    SetupRendering(config);
    
    // Possibly add some debugging stuff
    // SetupDebugging(config);

    // Start up the engine.
    engine->Start();

    // Return when the engine stops.
    delete engine;
    return EXIT_SUCCESS;
}

void SetupResources(Config& config) {
    // set the resources directory
    // @todo we should check that this path exists
    DirectoryManager::AppendPath("projects/SoundPlayer/data/");
    DirectoryManager::AppendPath("");
    DirectoryManager::AppendPath("/");

    // load resource plug-ins
    ResourceManager<ISoundResource>::AddPlugin(new VorbisResourcePlugin());

    config.resourcesLoaded = true;
}

void SetupDisplay(Config& config) {
    if (config.frame         != NULL ||
        config.viewingvolume != NULL ||
        config.camera        != NULL ||
        config.frustum       != NULL ||
        config.viewport      != NULL)
        throw Exception("Setup display dependencies are not satisfied.");

    config.frame         = new SDLFrame(800, 600, 32);
    config.viewingvolume = new ViewingVolume();
    config.camera        = new Camera( *config.viewingvolume );
    config.frustum       = new Frustum(*config.camera, 20, 3000);
    config.viewport      = new Viewport(*config.frame);
    config.viewport->SetViewingVolume(config.frustum);

    config.engine.InitializeEvent().Attach(*config.frame);
    config.engine.ProcessEvent().Attach(*config.frame);
    config.engine.DeinitializeEvent().Attach(*config.frame);
}

void SetupRendering(Config& config) {
    if (config.viewport == NULL ||
        config.renderer != NULL ||
        config.scene == NULL)
        throw Exception("Setup renderer dependencies are not satisfied.");

    // Create a renderer
    config.renderer = new Renderer();

    // Setup a rendering view
    RenderingView* rv = new RenderingView(*config.viewport);
    config.renderer->ProcessEvent().Attach(*rv);

    // Add rendering initialization tasks
    TextureLoader* tl = new TextureLoader();
    config.renderer->InitializeEvent().Attach(*tl);

    // Supply the scene to the renderer
    config.renderer->SetSceneRoot(config.scene);

    config.engine.InitializeEvent().Attach(*config.renderer);
    config.engine.ProcessEvent().Attach(*config.renderer);
    config.engine.DeinitializeEvent().Attach(*config.renderer);
}

void SetupScene(Config& config) {
    if (config.scene    != NULL ||
        config.resourcesLoaded == false)
        throw Exception("Setup scene dependencies are not satisfied.");

    // Create scene nodes
    config.scene = new SceneNode();
    ISceneNode* root = config.scene;

    // visualize sound
    TransformationNode* tn = new TransformationNode();
    TransformationNode* ltn = new TransformationNode();
    TransformationNode* rtn = new TransformationNode();
    TransformationNode* tn2 = new TransformationNode();

    tn2->Move(0,0,-100);
    PointLightNode* dln = new PointLightNode();
    dln->linearAtt = 0.001;
    dln->quadAtt = 0.0001;
    dln->constAtt = 0.5;

    root->AddNode(tn);
    root->AddNode(tn2);
    tn2->AddNode(dln);
    
    // move the transformation node in a circle
    CircleUpdate* cu = 
      new CircleUpdate(Vector<3,float>(0,0,0), 50, 0.2, -PI*0.5 - PI*0.25);
    TransformationUpdater* tu  = 
      new TransformationUpdater(tn, cu);
    config.engine.ProcessEvent().Attach(*tu);
    
    //SoundRenderer* sr = new SoundRenderer();
    //@todo config.renderer->preProcess.Attach(*sr);
    //sr->AddSoundNode(soundnode,Vector<3,float>(10.0,10.0,10.0));
    //   sr->AddSoundNode(rightnode,Vector<3,float>(10.0,10.0,10.0));
    /*
    tn->AddNode(ltn);
    tn->AddNode(rtn);
    ltn->AddNode(leftnode);
    ltn->Move(-5.0, 0.0, 0.0);
    rtn->AddNode(rightnode);
    rtn->Move(5.0, 0.0, 0.0);*/

    SphereNode* sphere2 = new SphereNode();        
    dln->AddNode(sphere2);
    ISoundSystem* openalsmgr = new OpenALSoundSystem(root, config.camera);
    config.engine.ProcessEvent().Attach(*openalsmgr);


    //backgroundsound test
    MusicPlayer* player = new MusicPlayer(config.camera, openalsmgr);
    //player->AddMonoBackGroundSound("batmanfo.ogg");
    //player->AddMonoBackGroundSound("Atmosphere01.ogg");

    std::list<string> files;

    // load playlist.txt file
    ifstream* playlistFile = File
        ::Open("projects/SoundPlayer/playlist.txt");
    while (!playlistFile->eof()) {
        string line;
        getline(*playlistFile, line);

        // Check the string
        if (line[0] == '#' || line == "") continue;
        else files.push_back(line);
    }

    // load and decompress each files from the list
    list<string>::iterator itr;
    for (itr=files.begin(); itr != files.end(); ++itr) {
        string file = *itr;
        logger.info << "loading file: " << file << " ... "; //@todo logger.flush
        try {
            player->AddSound(file);
            logger.info << "done" << logger.end;
        } catch (Exception e) {
            logger.info << "failed" << logger.end;
            logger.warning << e.what() << logger.end;
        }
    }

    config.engine.ProcessEvent().Attach(*player);
    
    // Create the mouse and keyboard input modules
    SDLInput* input = new SDLInput();
    config.engine.InitializeEvent().Attach(*input);
    config.engine.ProcessEvent().Attach(*input);
    config.engine.DeinitializeEvent().Attach(*input);
    config.keyboard = input;
    config.mouse    = input;

    // Bind the quit handler
    QuitHandler* quit_h = new QuitHandler(config.engine);
    config.keyboard->KeyEvent().Attach(*quit_h);

    // Register movement handler to be able to move the camera
    MoveHandler* move_h = new MoveHandler(*config.camera, *config.mouse);
    config.keyboard->KeyEvent().Attach(*move_h);

    config.engine.InitializeEvent().Attach(*move_h);
    config.engine.ProcessEvent().Attach(*move_h);
    config.engine.DeinitializeEvent().Attach(*move_h);

    MusicPlayerKeyHandler* mph = new MusicPlayerKeyHandler(*player);
    config.keyboard->KeyEvent().Attach(*mph);

    //SoundKeyHandler* ph = new SoundKeyHandler(soundnode);
    //config.keyboard->KeyEvent().Attach(*ph);
    //SoundKeyHandler* ph1 = new SoundKeyHandler(rightnode);
    //config.keyboard->KeyEvent().Attach(*ph1);
    
    player->Play();
}

void SetupDebugging(Config& config) {

    // Visualization of the frustum
    if (config.frustum != NULL) {
        config.frustum->VisualizeClipping(true);
        config.scene->AddNode(config.frustum->GetFrustumNode());
    }

    config.engine.ProcessEvent()
        .Attach(*(new OpenEngine::Utils::Statistics(1000)));
}
