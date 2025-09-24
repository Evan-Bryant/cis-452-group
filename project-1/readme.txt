# Project One - "One Bad Apple"

## Course
CS 452 - Operating Systems

## Overview
In this project, you will design and implement a **circular communication system** where `k` processes form a ring. Each node communicates only with its direct neighbor. The system uses a special token called the **apple** to synchronize message passing.

- Node 1 writes to Node 2, Node 2 writes to Node 3, ..., Node k writes to Node 0.
- Node 0 reads from Node k, Node k reads from Node (k-1), etc.
- A node can only send or receive messages when it holds the apple.
- Messages are forwarded around the ring until the correct destination receives them.

Verbose diagnostic messages should clearly show system activity (process creation/closure, message send/receive, apple passing, etc.).

## Requirements
- Use process management and IPC system calls covered in class (`fork`, `signal`, `pipe`, etc.).
- The **parent process** (Node 0):
  - Requests the value of `k` (number of processes).
  - Spawns `k` processes (including itself).
  - Prompts the user for a string message and a destination node.
  - Sends messages into the ring.
- Each node:
  - Receives the apple.
  - Checks if a message is intended for it:
    - If yes: copies the message and clears the header.
    - If no: forwards the message to the next node.

### Additional Notes
1. Must handle multi-word messages.
2. Must not use `sleep()` or no-op loops.
3. After the apple returns to the parent, prompt for the next message.
4. Gracefully shut down on `Ctrl+C` (signal handling).

### Coding Style
- Use descriptive camelCase variable names.
- Self-documenting code with minimal inline comments.

## Deliverables
1. **Design document** describing the project and implementation.
2. **C source code** (no zip files).
3. **Execution screenshot(s)**:
   - Show apple passing and message send/receive.
   - At least **two different messages** sent to different nodes from the parent.

## Grading
- Deductions based on missing features.
- More important features have higher weight.

### Extra Credit (+10%)
- Submit **two weeks early**.

### C Grade Option (-25 points)
- Implement with a fixed **3-node ring** instead of `k`.
- Standard deductions still apply.

## Appendix A
Diagram of nodes and communication directions (example for 4 nodes):

```
0 → 1 → 2 → 3 → 0
```

Each arrow represents a one-way communication channel.
