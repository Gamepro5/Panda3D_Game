#include "pandaFramework.h"
#include "pandaSystem.h"
#include "load_prc_file.h"
#include <iostream>

#include "genericAsyncTask.h"
#include "asyncTaskManager.h"

// The global task manager
PT(AsyncTaskManager) taskMgr = AsyncTaskManager::get_global_ptr();
// The global clock
PT(ClockObject) globalClock = ClockObject::get_global_clock();
// Here's what we'll store the camera in.
NodePath camera;

// This is our task - a global or static function that has to return DoneStatus.
// The task object is passed as argument, plus a void* pointer, containing custom data.
// For more advanced usage, we can subclass AsyncTask and override the do_task method.
AsyncTask::DoneStatus spinCameraTask(GenericAsyncTask* task, void* data) {
    // Calculate the new position and orientation (inefficient - change me!)
    double dt = globalClock->get_dt();
    double angledegrees = camera.get_h() + dt * 6.0;
    double angleradians = angledegrees * (3.14 / 180.0);
    //camera.set_pos(20 * sin(angleradians), -20.0 * cos(angleradians), 3);
    camera.set_h(angledegrees);

    std::cout << dt << std::endl;

    // Tell the task manager to continue this task the next frame.
    return AsyncTask::DS_cont;
}

int main(int argc, char* argv[]) {

    // Load the window and set its title.
    load_prc_file_data("", "fullscreen 1");
    load_prc_file_data("", "win-size 2560 1440");
    load_prc_file_data("", "sync-video false");

    PandaFramework framework;
    framework.open_framework(argc, argv);
    framework.set_window_title("My Panda3D Window");
    WindowFramework* window = framework.open_window();
    // Get the camera and store it in a variable.
    camera = window->get_camera_group();

    // Load the environment model.
    NodePath scene = window->load_model(framework.get_models(), "models/environment");
    // Reparent the model to render.
    scene.reparent_to(window->get_render());
    // Apply scale and position transforms to the model.
    scene.set_scale(0.25, 0.25, 0.25);
    scene.set_pos(-8, 42, 0);


    camera.set_pos(0, 0, 3);
    // Add our task.
    // If we specify custom data instead of NULL, it will be passed as the second argument
    // to the task function.
    taskMgr->add(new GenericAsyncTask("Spins the camera", &spinCameraTask, nullptr));

    // Run the engine.
    framework.main_loop();
    // Shut down the engine when done.
    framework.close_framework();
    return 0;
}