#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>    
#include <semaphore.h>  
#include <time.h>       
#include <unistd.h>     

#define FLOUR 0
#define SUGAR 1
#define YEAST 2
#define SODA 3
#define SALT 4
#define CINNAMON 5

#define EGG 6
#define MILK 7
#define BUTTER 8

char ingredients[9][8] = { "Flour", "Sugar", "Yeast", "Soda", "Salt", "Cinnamon", "Egg(s)", "Milk", "Butter" };

int cookies[] = { FLOUR, SUGAR, MILK, BUTTER };
int pancakes[] = { FLOUR, SUGAR, SODA, SALT, EGG, MILK, BUTTER };
int pizza[] = { YEAST, SODA, EGG };
int pretzels[] = { FLOUR, SUGAR, SALT, YEAST, SODA, EGG };
int rolls[] = { FLOUR, SUGAR, SALT, BUTTER, EGG, CINNAMON };

typedef struct {
    int mixer;
    int pantry;
    int refrigerator;
    int bowl;
    int spoon;
    int oven;
} Kitchen;

// Semaphores for each shared resource
sem_t mixer_sem;
sem_t pantry_sem;
sem_t refrigerator_sem;
sem_t bowl_sem;
sem_t spoon_sem;
sem_t oven_sem;

int ramsiedBaker = -1;

int findNum(int* lis, int num) {
    // returns the index of a recipe that hasn't been completed yet

    if (lis[num] == 0) {
        return num;
    }
    else {
        // Checks to see if everything has been done
        int count = 0;
        for (int i = 0; i < 5; i++) {
            if (lis[i] == 1) {
                count = count + 1;
            }
        }
        if (count == 5) {
            return -1;
        }

        // Finds the next recipe that hasn't been done yet
        int tmp = num;
        while (lis[tmp] != 0) {
            if (tmp < 4) {
                tmp = tmp + 1;
            }
            else if (tmp == 4) {
                tmp = 0;
            }
        }
        return tmp;
    }
}

void* baker_thread(void* arg) {
    int bakerId = *(int*)arg;
    char bakerLetter = 'A' + bakerId; // A B C D E ...

    int todoList[] = { 0, 0, 0, 0, 0 }; // which recipes this baker has done
    int nonpick, pick, grabbing, steps;
    char* finishTxt;
    int* recipe;

    srand((unsigned int)(time(NULL) + bakerId * 123));

    while (1) {
        nonpick = rand() % 5;
        pick = findNum(todoList, nonpick); // -1 if every recipe has been done

        if (pick == -1) {
            printf("Baker %c is done baking everything!\n", bakerLetter);
            break;
        }
        else if (pick == 0) { // Cookies
            todoList[0] = 1;
            finishTxt = "Cookies";
            recipe = cookies;
            steps = 4;
        }
        else if (pick == 1) { // Pancakes
            todoList[1] = 1;
            finishTxt = "Pancakes";
            recipe = pancakes;
            steps = 7;
        }
        else if (pick == 2) { // Pizza 
            todoList[2] = 1;
            finishTxt = "Pizza Dough";
            recipe = pizza;
            steps = 3;
        }
        else if (pick == 3) { // Pretzels
            todoList[3] = 1;
            finishTxt = "Soft Prezetls";
            recipe = pretzels;
            steps = 6;
        }
        else {              // Rolls
            todoList[4] = 1;
            finishTxt = "Cinnamon Rolls";
            recipe = rolls;
            steps = 4;
        }

        printf("Baker %c started making %s\n", bakerLetter, finishTxt);

        // ingerdients 
        for (int i = 0; i < steps; i++) {
            grabbing = recipe[i];

            if (grabbing < 6) {
                // Pantry ingredient
                sem_wait(&pantry_sem);  // only one baker in pantry at a time
                printf("Baker %c is in the pantry grabbing %s\n",
                       bakerLetter, ingredients[grabbing]);
                usleep(10000);
                sem_post(&pantry_sem);
            }
            else {
                // Refrigerator ingredient
                sem_wait(&refrigerator_sem); // at most 2 bakers in fridge area
                printf("Baker %c is at the refrigerator grabbing %s\n",
                       bakerLetter, ingredients[grabbing]);
                usleep(10000);
                sem_post(&refrigerator_sem);
            }
        }

        // grab mixer, bowl, spoon
        printf("Baker %c is grabbing a mixer.\n", bakerLetter);
        sem_wait(&mixer_sem);

        printf("Baker %c is grabbing a bowl.\n", bakerLetter);
        sem_wait(&bowl_sem);

        printf("Baker %c is grabbing a spoon.\n", bakerLetter);
        sem_wait(&spoon_sem);

        // mixing
        printf("Baker %c is mixing the batter/dough.\n", bakerLetter);
        usleep(20000);

        printf("Baker %c is putting back the mixer, bowl, and spoon.\n",
               bakerLetter);
        sem_post(&spoon_sem);
        sem_post(&bowl_sem);
        sem_post(&mixer_sem);

        // oven
        printf("Baker %c is putting the %s in the oven.\n",
               bakerLetter, finishTxt);
        sem_wait(&oven_sem);
        // baking
        usleep(30000);
        sem_post(&oven_sem);

        printf("Baker %c finished making %s\n", bakerLetter, finishTxt);

        if (bakerId == ramsiedBaker) {
            int chance = rand() % 100; 
            if (chance < 15) { 
                printf("Baker %c has been RAMSIED and must start over!\n",
                       bakerLetter);
                for (int i = 0; i < 5; i++) {
                    todoList[i] = 0;
                }
            }
        }
    }

    return NULL;
}

int main() {
    Kitchen kitchen;
    kitchen.mixer = 2;
    kitchen.pantry = 1;
    kitchen.refrigerator = 2;
    kitchen.bowl = 3;
    kitchen.spoon = 5;
    kitchen.oven = 1;

    int numBakers;
    printf("Enter number of bakers: ");
    scanf("%d", &numBakers);

    if (numBakers <= 0) {
        printf("Need at least 1 baker.\n");
        return 1;
    }

    // Ramsied baker selection
    srand((unsigned int)time(NULL));
    ramsiedBaker = rand() % numBakers;

    printf("Baker %c has been chosen as the potential RAMSIED baker.\n",
           'A' + ramsiedBaker);

    // Initialize semaphores
    sem_init(&mixer_sem,        0, kitchen.mixer);
    sem_init(&pantry_sem,       0, kitchen.pantry);
    sem_init(&refrigerator_sem, 0, kitchen.refrigerator);
    sem_init(&bowl_sem,         0, kitchen.bowl);
    sem_init(&spoon_sem,        0, kitchen.spoon);
    sem_init(&oven_sem,         0, kitchen.oven);

    // Create one thread per baker
    pthread_t* threads = malloc(sizeof(pthread_t) * numBakers);
    int* ids = malloc(sizeof(int) * numBakers);

    for (int i = 0; i < numBakers; i++) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, baker_thread, &ids[i]) != 0) {
            perror("pthread_create failed");
        }
    }

    // Wait for all bakers to finish
    for (int i = 0; i < numBakers; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    sem_destroy(&mixer_sem);
    sem_destroy(&pantry_sem);
    sem_destroy(&refrigerator_sem);
    sem_destroy(&bowl_sem);
    sem_destroy(&spoon_sem);
    sem_destroy(&oven_sem);

    free(threads);
    free(ids);

    return 0;
}
