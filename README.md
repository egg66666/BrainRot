# BrainRot
BrainRot - an external WIP cheat lib

## Namespaces
misc - random, miscellaneous shit. also a std::barrier variant that wont fuck you with comptime checks.
util - utilities, mainly string and PE related utilities.
globals - only a handle, cause why would you need to make more than 1 handle?

## classes 
```cpp
_import, _export, _module.
```
all self explanatory.
class process - contains modules, singleton. call init(process_name) at the start of your program to
initialize it and process.hijack_handle() to hijack the process handle. process id is obtained in the init function, unlike the handle.
access modules via the overloaded [] operator. 
```cpp
process["client.dll"]->get("CreateInterface"); returns create interface export.
```
overlay - WIP, will contain a thread safe way of queueing render tasks into the render thread from anywhere.
main advantage is that the render thread won't have to fuck around with non-render tasks like syscalls etc..
when people shove entire ESP functions into the render thread.

call 
```cpp
overlay->create_barrier(number_of_rendering_threads) to setup the custom barrier.
overlay->create_thread_with_tasks(func, args) to create a rendering task. each call of this is one
rendering_thread so it adds 1 to number_of_rendering_tasks.
making this dynamic so you dont have to call create_barrier would be too much of a hassle.
set_parameters sets overlay parameters, plans are to make it automatic.
init() is self explanatory, call at the start in main.
message_loop gets called last in main.
```
