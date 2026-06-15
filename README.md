# Operating Systems Project - Traffic Simulation in a Graph

## Building and Running

```bash
make milestone6   # Build Node Access Synchronization Simulation

# Note: The following milestones are not supported in this version
make milestone1   # Build Dijkstra (CLI)
make milestone2   # Build Graph Visualization
make milestone3   # Build Traffic Animation
make milestone4   # Build Multi-Process Traffic Simulation
make milestone5   # Build IPC Multi-Process Traffic Simulation

make clean        # Remove compiled files
```

### Running

```bash
./sim <input_file>
```

### Input File Format

```
<num_vertices> <num_edges>
<src> <dst> <weight>
...
(repeat for num_edges)
<num_travelers>
<dijkSrc> <dijkDst>
...
(repeat for num_travelers)
```

Example (input.txt):
```
6 10
0 1 4
0 2 2
1 2 1
1 3 5
2 1 1
2 3 4
2 4 10
3 4 3
3 5 2
4 5 3
2
0 5
1 4
```

---

## Milestone Descriptions

### Milestone 1: Dijkstra's Algorithm
Implementation of Dijkstra's shortest path algorithm using adjacency lists and a Min-Heap priority queue. Output is printed to console.

### Milestone 2: Graph Visualization
Visual display using raylib with circular vertex layout, curved edges for bidirectional connections, and edge weights shown on graph.

### Milestone 3: Traffic Animation
Interactive simulation that animates an entity (plane) moving along the shortest path. Features include:
- **Play/Pause Control:** Interactive button to start or stop the animation.
- **Dynamic Movement:** Plane speed and travel time are determined by the weights of the edges in the graph.
- **Node Interaction:** Simulated waiting times at intermediate nodes during the journey.
- **Visual Feedback:** Status messages indicating current progress and arrival at the destination.

### Milestone 4: Multi-Process Traffic Simulation
Extended the simulation to support multiple simultaneous passengers using OS process management. The parent process orchestrates the simulation while child processes represent individual travelers.
- **Parent Process:** Computes Dijkstra paths, manages the `raylib` GUI loop, assigns unique colors to passengers, and terminates children via signals upon arrival.
- **Child Processes:** Each traveler runs in a separate process, prints `[PID] started` on creation, and remains active until signaled by the parent.
- **Parallel Simulation:** All passengers move simultaneously on the screen, with the parent waiting for all processes to terminate before exiting.
- **Extended Input:** Support for multiple travelers with individual source and destination nodes.

### Milestone 5: Inter-Process Communication (IPC)

Extended the simulation by making each traveler process autonomous. Instead of receiving a precomputed route from the parent process, every child process computes its own shortest path using Dijkstra's algorithm and reports its progress to the parent through IPC.

- **IPC Mechanism:** Implemented using **pipes**, providing a simple and efficient communication channel between child and parent processes.
- **Child Processes:** Independently compute their shortest path and send route progress updates whenever they arrive at a new node.
- **Parent Process:** Receives updates from all children, prints execution logs to the terminal, and updates the GUI according to the received messages.
- **Event Logging:** The parent reports traveler progress in the format: [PID=XXXX] arrived at node X | next node: Y , and announces destination arrival and process completion.
- **Decoupled Architecture:** Route computation is no longer performed by the parent process. Children are responsible for path calculation and status reporting, while the parent focuses solely on visualization and coordination.

#### Why Pipes?

Pipes were selected because communication in this milestone is naturally **one-directional** (child → parent) and **message-based**. 
Each traveler only needs to send progress notifications to the parent process. Using pipes avoids the additional synchronization complexity required by shared memory solutions (such as mutexes or semaphores) while providing a clean and reliable IPC mechanism.

### Milestone 6: Node Access Synchronization

Implemented a robust, process-level synchronization mechanism to ensure that no more than one plane occupies a node at any given time.

- **Synchronization Mechanism:** Utilizes **POSIX Named Semaphores** (`sem_open`, `sem_wait`, `sem_post`). Each node in the graph is associated with a unique semaphore (e.g., `/node_0`, `/node_1`).
- **Critical Section:** The logic for "staying in a node" is defined as a critical section within the child processes. A traveler process must successfully call `sem_wait` on the target node's semaphore before entering.
- **Process-Level Sleep:** Once a process enters the critical section (acquires the semaphore), it calls `sleep(1)` to simulate the mandatory dwell time. This ensures that the traveler process is genuinely occupied and blocking others from the resource.
- **Event-Driven IPC:** The child processes report their synchronization state to the parent via pipes using a structured event system:
    - `EVENT_WAITING`: Child is blocked at `sem_wait` (displayed as **Yellow** "WAITING" in GUI).
    - `EVENT_ENTERING`: Child has acquired the lock and is inside the node.
    - `EVENT_LEAVING`: Child has released the lock and is moving to the next edge.
- **No Starvation:** The use of standard POSIX semaphores ensures that waiting processes are handled fairly by the OS scheduler, preventing starvation.
- **Visual Feedback:** The GUI accurately reflects these OS-level states, providing a real-time visualization of process synchronization and critical section management.
