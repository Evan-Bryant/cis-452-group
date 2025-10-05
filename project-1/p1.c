#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_NODES 10

typedef struct {
    int tokenValue;
    int senderNodeId;
} Apple;

int main() {
    int totalNodes;
    int pipeEnds[MAX_NODES][2];
    pid_t childProcessIds[MAX_NODES];

    printf("Enter number of nodes (max %d): ", MAX_NODES);
    scanf("%d", &totalNodes);

    if (totalNodes < 2 || totalNodes > MAX_NODES) {
        printf("Invalid node count.\n");
        exit(1);
    }

    for (int nodeIndex = 0; nodeIndex < totalNodes; nodeIndex++) {
        if (pipe(pipeEnds[nodeIndex]) == -1) {
            perror("pipe creation failed");
            exit(1);
        }

        int nextNodeIndex;
        if (nodeIndex == totalNodes - 1) {
            nextNodeIndex = 0;
        } else {
            nextNodeIndex = nodeIndex + 1;
        }

        printf("Pipe from Node %d to Node %d created.\n", nodeIndex, nextNodeIndex);
    }

    for (int nodeIndex = 1; nodeIndex < totalNodes; nodeIndex++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0) {
            int myId = nodeIndex;

            printf("[Node %d] Process started (PID: %d)\n", myId, getpid());

            Apple apple;

            while (1) {
                read(pipeEnds[myId][0], &apple, sizeof(Apple));

                printf("[Node %d] Received token from Node %d (token value: %d)\n",
                       myId, apple.senderNodeId, apple.tokenValue);

                apple.senderNodeId = myId;

                int nextNodeId;
                if (myId == totalNodes - 1) {
                    nextNodeId = 0;
                } else {
                    nextNodeId = myId + 1;
                }

                write(pipeEnds[nextNodeId][1], &apple, sizeof(Apple));
                printf("[Node %d] Sent token to Node %d\n", myId, nextNodeId);
            }

            exit(0);
        } else {
            childProcessIds[nodeIndex] = pid;
        }
    }

    int myId = 0;
    printf("[Node %d] Parent process running. PID: %d\n", myId, getpid());

    Apple apple;
    apple.tokenValue = 1;
    apple.senderNodeId = myId;

    write(pipeEnds[1][1], &apple, sizeof(Apple));
    printf("[Node %d] Sent initial token to Node 1.\n", myId);

    while (1) {
        read(pipeEnds[myId][0], &apple, sizeof(Apple));
        printf("[Node %d] Received token from Node %d (token value: %d)\n",
               myId, apple.senderNodeId, apple.tokenValue);

        apple.senderNodeId = myId;

        int nextNodeId;
        if (myId == totalNodes - 1) {
            nextNodeId = 0;
        } else {
            nextNodeId = myId + 1;
        }

        write(pipeEnds[nextNodeId][1], &apple, sizeof(Apple));
        printf("[Node %d] Sent token to Node %d\n", myId, nextNodeId);
    }

    return 0;
}
