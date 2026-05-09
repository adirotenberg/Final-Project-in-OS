# Operating Systems Project - Traffic Simulation in a Graph

## Building and Running

```bash
make milestone1   # Build Dijkstra (CLI)
make milestone2    # Build Graph Visualization
make milestone3   # Build Traffic Animation
make clean        # Remove compiled files
```

### Running

```bash
./dijkstra <file_name>   # Milestone 1
./sim <file_name>        # Milestones 2-3
```

### Input File Format

```
<num_vertices> <num_edges>
<src> <dst> <weight>
...
<source> <destination>
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
0 5
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
