# BrainRot

BrainRot, an external cheat library.

## Namespaces
- `misc`: Random, miscellaneous functionality, including a `std::barrier` variant without comptime checks.
- `util`: Utilities, primarily string and PE-related functionalities.
- `globals`: Contains a handle; only one handle is necessary.

## Classes
- `_import`, `_export`, `_module`: Self-explanatory.
- `process`: Singleton class that contains modules. Call `init(process_name)` at the start of your program to initialize it and `process.hijack_handle()` to hijack the process handle. Process ID is obtained in the `init` function. Access modules via the overloaded `[]` operator.

   ```cpp
   // Returns the CreateInterface export.
   process["client.dll"]->get("CreateInterface");
   ```

- `overlay`: Work in progress, designed to provide a thread-safe way of queuing render tasks into the render thread from anywhere. The main advantage is that the render thread won't handle non-render tasks like syscalls. 

   ```cpp
   // Set up the custom barrier.
   overlay->create_barrier(number_of_rendering_threads);

   // Create a rendering task. Each call of this adds 1 to the number of rendering tasks.
   overlay->create_thread_with_tasks(func, args);

   // Set overlay parameters; plans are to make it automatic.
   set_parameters(parameters...);

   // Call at the start in main.
   init(); 

   // Gets called last in main before cleanup().
   message_loop();
   ```