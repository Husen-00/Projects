#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

#define MAX_QUESTION_LEN 200
#define MAX_OPTION_LEN 50
#define NUM_QUESTIONS 5

int check_answer(char user_answer, char correct_answer) {
    if(user_answer == correct_answer) {
        printf("Correct!\n");
        return 1;
    } else {
        printf("Wrong!,,, Correct answer is %c\n", correct_answer);
        return 0;
    }
}
int update_score(int score, int result) {
    if(result == 1) score++;
    return score;
}
void initialize_and_shuffle(char questions[][MAX_QUESTION_LEN],
                            char options[][4][MAX_OPTION_LEN],
                            char answers[],
                            int *order, int n) {

    strcpy(questions[0], "What is the output of: printf(\"%d\", 5 + 3 * 2);");
    strcpy(options[0][0], "A) 11");
    strcpy(options[0][1], "B) 16");
    strcpy(options[0][2], "C) 10");
    strcpy(options[0][3], "D) 13");
    answers[0] = 'A';

    strcpy(questions[1], "What does the '++' operator do in C?");
    strcpy(options[1][0], "A) Adds 2");
    strcpy(options[1][1], "B) Increments by 2");
    strcpy(options[1][2], "C) Increments by 1");
    strcpy(options[1][3], "D) Squares value");
    answers[1] = 'C';

    strcpy(questions[2], "Which is used for decision making in C?");
    strcpy(options[2][0], "A) if");
    strcpy(options[2][1], "B) switch");
    strcpy(options[2][2], "C) while");
    strcpy(options[2][3], "D) Both A and B");
    answers[2] = 'D';

    strcpy(questions[3], "Which function compares two strings?");
    strcpy(options[3][0], "A) strcomp()");
    strcpy(options[3][1], "B) strcmp()");
    strcpy(options[3][2], "C) compare()");
    strcpy(options[3][3], "D) stringcomp()");
    answers[3] = 'B';

    strcpy(questions[4], "Which is safest for string input?");
    strcpy(options[4][0], "A) scanf");
    strcpy(options[4][1], "B) gets (unsafe)");
    strcpy(options[4][2], "C) fgets");
    strcpy(options[4][3], "D) All");
    answers[4] = 'C';

    srand(time(NULL));

    for(int i = 0; i < n - 1; i++) {
        int j = i + rand() % (n - i);
        int temp = order[i];
        order[i] = order[j];
        order[j] = temp;
    }
}
void countdown_timer() {
    printf("\n You have 10 seconds...\n");
    for(int i = 10; i > 0; i--) {
        printf("\rTime left: %d ", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
}
void display_question(char *question, char options[][MAX_OPTION_LEN], int qno) {
    printf("\n====================\n");
    printf("Q%d: %s\n", qno, question);
    for(int i = 0; i < 4; i++) {
        printf("%s\n", options[i]);
    }
}
char get_user_answer() {
    char ans;
    printf("Enter answer (A/B/C/D): ");
    scanf(" %c", &ans);

    ans = toupper(ans);

    if(ans=='A'||ans=='B'||ans=='C'||ans=='D')
        return ans;
    else
        return '\0';
}
void save_score(int score) {
    FILE *fp = fopen("score.txt", "a");
    if(fp != NULL) {
        fprintf(fp, "Score: %d/%d\n", score, NUM_QUESTIONS);
        fclose(fp);
    }
}
void play_quiz() {
    char questions[NUM_QUESTIONS][MAX_QUESTION_LEN];
    char options[NUM_QUESTIONS][4][MAX_OPTION_LEN];
    char answers[NUM_QUESTIONS];
    int order[NUM_QUESTIONS];

    for(int i=0;i<NUM_QUESTIONS;i++)
        order[i]=i;

    initialize_and_shuffle(questions, options, answers, order, NUM_QUESTIONS);

    int score = 0;

    for(int i = 0; i < NUM_QUESTIONS; i++) {
        int idx = order[i];

        display_question(questions[idx], options[idx], i+1);
        countdown_timer();

        char user = get_user_answer();

        if(user == '\0') {
            printf("⚠ Invalid input!\n");
            continue;
        }

        int result = check_answer(user, answers[idx]);
        score = update_score(score, result);
    }

    printf("\n Final Score: %d/%d\n", score, NUM_QUESTIONS);
    save_score(score);
}
int main() {
    char choice;

    do {
        play_quiz();

        printf("\nPlay again? (Y / N): ");
        scanf(" %c", &choice);
        choice = toupper(choice);

    } while(choice == 'Y'); 

    printf("Thanks for playing!\n");
    return 0;
}
