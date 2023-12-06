# BrainRot
BrainRot - an external cheat library

## Namespaces
- `misc`: Random, miscellaneous functionality, including a `std::barrier` variant without comptime checks.
- `util`: Utilities, primarily string and PE-related functionalities.
- `globals`: Contains a handle; only one handle is necessary.

## Classes
- `import`, `export`, `module`: Self-explanatory.
- `process`: Singleton class that contains modules. Call `init(process_name)` at the start of your program to initialize it and `process.hijack_handle()` to hijack the process handle. Process ID is obtained in the `init` function. Access modules via the overloaded `[]` operator.

   ```cpp
   // Returns the CreateInterface export.
   process["client.dll"]->get("CreateInterface");
   ```
- `overlay`: Work in progress, designed to provide a thread-safe way of queuing render tasks into the render thread from anywhere. The main advantage is that the render thread won't handle non-render tasks like syscalls. 
   ```cpp
   //set up the overlay parameters
   overlay->set_parameters(params..);
   //initialize the overlay, create a custom render barrier
   overlay->init(number_of_rendering_threads);
   //create a thread with rendering tasks
   overlay->create_thread_with_tasks(thread, args..);
   //message loop, run in main before cleanup, careful as it BLOCKS MAIN. THIS IS INTENDED
   overlay->message_loop();
   //cleanup, run at the end of main.
   overlay->cleanup();
   ```
