# Discrete Event System: Automated Palletizer

## Project Context
Industrial automation relies heavily on Discrete Event Systems (DES) to model and control sequential processes. In this project, I developed a **bare-metal C application** for an **STM32F072RB microcontroller** to automate a complex multi-stage palletizer in the Factory I/O environment. 

Unlike RTOS-based architectures, this approach relies on a deterministic, infinite polling loop evaluating strict mathematical state models to orchestrate the machinery without relying on an underlying operating system.

## Petri Net Translation & State Machines
The entire mechanical process was first modeled mathematically using **Petri nets**, breaking down the system into five autonomous mechanical entities: carton distribution, entry blocking, pushers, doors, and elevators.

To translate this abstract model into efficient C code, I utilized a multi-level switch-case architecture. Each entity maintains its own state variable (e.g., `State_entity_1`), which tracks its current operational phase. Transitions between states are triggered exclusively by physical sensor readings, processed rapidly via bitwise operations on an actuator buffer.

## Inter-Entity Synchronization
Because all entities operate within the same sequential loop, they must coordinate complex physical hand-offs safely. For example, the pusher must wait for exactly two cartons to accumulate before activating.

I implemented **lightweight synchronization flags** (`Synch_X_Y`) and **memory rings** to solve this. When one entity completes a task, it raises a synchronization flag. This acts as an event trigger, allowing a dependent entity to progress to its next state. This method ensures collision-free mechanical operations and perfect timing across the assembly line.

## Tech Stack
* **Microcontroller:** STM32F072RB (ARM Cortex-M0)
* **Language:** Bare-metal C
* **Theory & Modeling:** Discrete Event Systems (DES), Petri Nets, Finite State Machines (FSM)
* **Simulation Environment:** Factory I/O
