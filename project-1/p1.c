#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_NODES 10
#define MSG_LEN 256

typedef struct {
    int senderNodeId;
    int destinationNodeId;
    int isEmpty;
    char message[MSG_LEN];
} Apple;

int totalNodes;
int pipeEnds[MAX_NODES][2];
pid_t childProcessIds[MAX_NODES];

void handleSigInt(int sig) {
    printf("\nParent detected Ctrl+C, shutting down all nodes...\n");
    for (int i = 1; i < totalNodes; i++) {
        if (childProcessIds[i] > 0) kill(childProcessIds[i], SIGTERM);
    }
    while (wait(NULL) > 0) {}
    printf("All processes closed. Exiting program.\n");
    exit(0);
}

int main() {
    signal(SIGINT, handleSigInt);

    printf("Enter number of nodes (max %d): ", MAX_NODES);
    scanf("%d", &totalNodes);
    if (totalNodes < 2 || totalNodes > MAX_NODES) {
        printf("Invalid number of nodes.\n");
        exit(1);
    }

    for (int i = 0; i < totalNodes; i++) {
        if (pipe(pipeEnds[i]) == -1) {
            perror("Pipe failed");
            exit(1);
        }
        int next = (i == totalNodes - 1) ? 0 : i + 1;
        printf("Created pipe from Node %d to Node %d.\n", i, next);
    }

    for (int i = 1; i < totalNodes; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }
        if (pid == 0) {
            int myId = i;
            printf("Node %d started (PID %d).\n", myId, getpid());

            int next = (myId == totalNodes - 1) ? 0 : myId + 1;
            for (int j = 0; j < totalNodes; j++) {
                if (j != myId) close(pipeEnds[j][0]);
                if (j != next) close(pipeEnds[j][1]);
            }

            Apple apple;
            while (1) {
                if (read(pipeEnds[myId][0], &apple, sizeof(Apple)) <= 0) exit(0);
                printf("Node %d received apple from Node %d.\n", myId, apple.senderNodeId);

                if (!apple.isEmpty && apple.destinationNodeId == myId) {
                    printf("Node %d received message: \"%s\"\n", myId, apple.message);
                    apple.isEmpty = 1;
                }

                apple.senderNodeId = myId;
                if (write(pipeEnds[next][1], &apple, sizeof(Apple)) <= 0) exit(0);
                printf("Node %d passed apple to Node %d.\n", myId, next);
            }
            exit(0);
        } else childProcessIds[i] = pid;
    }

    int myId = 0;
    printf("Parent (Node %d) running with PID %d.\n", myId, getpid());
    int nextNode = 1;

    for (int i = 0; i < totalNodes; i++) {
        if (i != myId) close(pipeEnds[i][0]);
        if (i != nextNode) close(pipeEnds[i][1]);
    }

    Apple apple;
    apple.senderNodeId = myId;
    apple.isEmpty = 1;

    int dest;
    printf("Enter destination node (1-%d): ", totalNodes - 1);
    scanf("%d", &dest);
    getchar();
    printf("Enter message: ");
    fgets(apple.message, MSG_LEN, stdin);
    apple.message[strcspn(apple.message, "\n")] = 0;
    apple.destinationNodeId = dest;
    apple.isEmpty = 0;

    write(pipeEnds[nextNode][1], &apple, sizeof(Apple));
    printf("Parent sent first message to Node %d.\n", nextNode);

    while (1) {
        if (read(pipeEnds[myId][0], &apple, sizeof(Apple)) <= 0) break;
        printf("Parent received apple back from Node %d.\n", apple.senderNodeId);

        if (apple.isEmpty) {
            printf("\nEnter destination node (1-%d): ", totalNodes - 1);
            scanf("%d", &dest);
            getchar();
            printf("Enter message: ");
            fgets(apple.message, MSG_LEN, stdin);
            apple.message[strcspn(apple.message, "\n")] = 0;
            apple.destinationNodeId = dest;
            apple.isEmpty = 0;
        }

        apple.senderNodeId = myId;
        write(pipeEnds[nextNode][1], &apple, sizeof(Apple));
        printf("Parent sent apple to Node %d.\n", nextNode);
    }

    handleSigInt(0);
    return 0;
}

