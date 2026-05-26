# Operating Systems Project - Traffic Simulation in a Graph

## Building and Running

```bash
make milestone4   # Build Multi-Process Traffic Simulation

# Note: The following milestones are not supported in this version
make milestone1   # Build Dijkstra (CLI)
make milestone2   # Build Graph Visualization
make milestone3   # Build Traffic Animation

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
