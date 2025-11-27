#include <stdlib.h>
#include <stdio.h>

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
}Kitchen; // Temporary? Might need to change this to work with semaphores idk

int findNum(int* lis, int num) {
    // returns the index of a recipe that hasn't been completed yet

    // If the current selection hasn't been done yet
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

int main() {
    // Temporary? Might need to change this to work with semaphores idk 
    // Maybe a binary semaphore for pantry and oven
    // and counting semaphores for the rest? i dont understand semaphores
    Kitchen kitchen;
    kitchen.mixer = 2;
    kitchen.pantry = 1;
    kitchen.refrigerator = 2;
    kitchen.bowl = 3;
    kitchen.spoon = 5;
    kitchen.oven = 1;

    // TODO: Setup shared memory


    // user inputs number of bakers
    int numBakers;
    printf("Enter number of bakers: ");
    scanf("%d", &numBakers);

    // TOCULLEN: fork off processes, if odd the parent forks off the extra

    // TOCULLEN: assign each baker a letter

    // TOCULLEN: give each baker a different color text in terminal

    // TOCULLEN: assign one baker a chance to have to start over

    // TOCULLEN: if chance met for restart, reset todoList


    // actual baker
    int todoList[] = { 0, 0, 0, 0, 0 }; // a binary list of which recipes the chef has/has not done
    int nonpick, // a temporary variable containing the raw random value generated
        pick, // the sanitised value after being put through findNum()
        grabbing, // the ingredient the baker currently needs
        steps; // how many steps the current recipe has
    char* finishTxt; // The text containing what recipe is currently being cooked
    int* recipe; // a pointer to the list containing the current recipe

    while (1) {
        srand(numBakers); // seeds random function with the number of bakers to get a consistent result while testing
        nonpick = rand() % 5;
        pick = findNum(todoList, nonpick); // sanitises the random number, returns -1 if every recipe has been done
        if (pick == -1) {
            printf("Baker _ is done baking everything!\n");
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
        else if (pick == 4) { // Rolls
            todoList[4] = 1;
            finishTxt = "Cinnamon Rolls";
            recipe = rolls;
            steps = 4;
        }
        printf("Baker _ started making %s\n", finishTxt);
        for (int i = 0; i < steps; i++) {
            grabbing = recipe[i];
            printf("Baker _ is grabbing %s\n", ingredients[grabbing]);
            if (grabbing < 6) {
                // TODO: Check to see if pantry is available

                // TODO: Use pantry if available

            }
            else {
                // TODO: Check to see if fridge is available

                // TODO: Use fridge if available
            }
        }
        // Grab the mixer, bowl, and spoon (in that order)
        printf("Baker _ is grabbing a mixer.\n");
        // TODO: Check if a mixer is available
        // TODO: Use mixer if available

        printf("Baker _ is grabbing a bowl.\n");
        // TODO: Check if a bowl is available
        // TODO: Use bowl if available

        printf("Baker _ is grabbing a spoon.\n");
        // TODO: Check if a spoon is available
        // TODO: Use spoon if available

        // Put mixer, bowl, and spoon back
        printf("Baker _ is putting back the mixer, bowl, and spoon.\n");
        // TODO: release the mixer, spoon, and bowl

        // Put in Oven
        printf("Baker _ is putting the %s in the oven.\n", finishTxt);
        // TODO: Check if oven is available 
        // TODO: Use oven if available


        printf("Baker _ finished making %s\n", finishTxt);
    }
}