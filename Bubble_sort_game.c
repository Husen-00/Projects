#include <stdio.h>

#define MAX_BOTTLES 20
#define MAX_H       10
#define MAX_MOVES   2000   // maximum depth we will search

/*
   bottles[i][0]   = BOTTOM of bottle i
   bottles[i][H-1] = TOP of bottle i

   One move = pour a WHOLE BLOCK of same colour from the top of one bottle
              to another (like the real game).

   Rules:
   - Can pour only if destination not full.
   - Can pour only into empty bottle OR onto same colour.
   - NEW: if both bottles are "pure" (only one colour) and same colour,
          then we allow only pour from SMALLER quantity -> BIGGER quantity.

   Goal (solved):
   - Each bottle is either:
        * completely empty (all 0), OR
        * completely full and all slots have the same colour.
*/

/* ---------- Helper functions ---------- */

int top_index(int bottle[], int H) {
    for (int i = H - 1; i >= 0; i--) {
        if (bottle[i] != 0) return i;
    }
    return -1;
}

void normalize_bottle(int bottle[], int H) {
    int temp[MAX_H];
    int idx = 0;

    for (int i = 0; i < H; i++) {
        if (bottle[i] != 0) temp[idx++] = bottle[i];
    }
    while (idx < H) temp[idx++] = 0;

    for (int i = 0; i < H; i++) bottle[i] = temp[i];
}

// solved: each bottle empty OR full & single colour
int is_solved(int bottles[MAX_BOTTLES][MAX_H], int N, int H) {
    for (int i = 0; i < N; i++) {
        int top = top_index(bottles[i], H);

        if (top == -1) continue; // empty OK

        if (top != H - 1) return 0;  // must be full

        int color = bottles[i][0];
        if (color == 0) return 0;

        for (int j = 1; j < H; j++) {
            if (bottles[i][j] != color) return 0;
        }
    }
    return 1;
}

/*
   Get info about a bottle:
   - returns colour if bottle has only one colour (ignoring zeros)
     else returns -1
   - writes count of non-zero slots into *count
*/
int bottle_single_color_info(int bottle[], int H, int *count) {
    *count = 0;
    int color = -1;
    for (int i = 0; i < H; i++) {
        int v = bottle[i];
        if (v == 0) continue;
        if (color == -1) color = v;
        else if (v != color) return -1;
        (*count)++;
    }
    return color;
}

/*
   Pour a WHOLE BLOCK from 'from' to 'to':
   block = consecutive same-colour balls at top of 'from'
*/
int pour(int from[], int to[], int H) {
    int from_top = top_index(from, H);
    int to_top   = top_index(to, H);

    if (from_top == -1) return 0;      // source empty
    if (to_top == H - 1) return 0;     // dest full

    int color = from[from_top];

    if (to_top != -1 && to[to_top] != color)
        return 0;                      // different colour on top

    // how many same-colour balls at source top?
    int same_count = 1;
    for (int i = from_top - 1; i >= 0; i--) {
        if (from[i] == color) same_count++;
        else break;
    }

    int free_space = H - (to_top + 1);
    int move_amount = same_count;
    if (move_amount > free_space) move_amount = free_space;
    if (move_amount <= 0) return 0;

    // perform the pour
    for (int k = 0; k < move_amount; k++) {
        to_top = top_index(to, H);
        if (to_top == -1) to[0] = color;
        else              to[to_top + 1] = color;

        from_top = top_index(from, H);
        from[from_top] = 0;
    }

    return 1;
}

/* ---------- DFS with depth limit (for IDDFS) ---------- */

int dfs_limited(int bottles[MAX_BOTTLES][MAX_H],
                int N, int H,
                int depth, int limit,
                int last_from, int last_to,
                int moves_from[], int moves_to[],
                int *solution_depth) {

    if (is_solved(bottles, N, H)) {
        *solution_depth = depth;
        return 1;
    }
    if (depth >= limit) return 0;

    // Precompute single-colour info for current state
    int color_info[MAX_BOTTLES];
    int count_info[MAX_BOTTLES];
    for (int i = 0; i < N; i++) {
        color_info[i] = bottle_single_color_info(bottles[i], H, &count_info[i]);
    }

    for (int i = 0; i < N; i++) {
        int from_top = top_index(bottles[i], H);
        if (from_top == -1) continue;

        for (int j = 0; j < N; j++) {
            if (i == j) continue;

            // don't immediately undo last move (j -> i)
            if (last_from == j && last_to == i) continue;

            // NEW RULE: if i and j are pure same-colour bottles,
            // allow move only from smaller count to bigger count
            if (color_info[i] != -1 &&
                color_info[j] != -1 &&
                color_info[i] == color_info[j]) {

                if (count_info[i] > count_info[j]) {
                    // this is big -> small, skip
                    continue;
                }
                // if equal count, we allow both directions;
                // you could restrict further if you want.
            }

            int temp[MAX_BOTTLES][MAX_H];

            // copy state
            for (int bi = 0; bi < N; bi++)
                for (int bj = 0; bj < H; bj++)
                    temp[bi][bj] = bottles[bi][bj];

            if (!pour(temp[i], temp[j], H)) continue;

            moves_from[depth] = i;
            moves_to[depth]   = j;

            if (dfs_limited(temp, N, H, depth + 1, limit,
                            i, j, moves_from, moves_to, solution_depth)) {
                return 1;
            }
        }
    }

    return 0;
}

/* ---------- main ---------- */

int main(void) {
    int N, H;
    int bottles[MAX_BOTTLES][MAX_H];

    printf("Enter number of bottles: ");
    scanf("%d", &N);

    printf("Enter capacity (colors per bottle): ");
    scanf("%d", &H);

    if (N > MAX_BOTTLES || H > MAX_H) {
        printf("Too big! Max bottles = %d, max height = %d\n",
               MAX_BOTTLES, MAX_H);
        return 0;
    }

    printf("\nEnter colors for each bottle.\n");
    printf("Use 0 for empty slot.\n");
    printf("Enter from BOTTOM to TOP.\n\n");

    for (int i = 0; i < N; i++) {
        printf("Bottle %d:\n", i + 1);
        for (int j = 0; j < H; j++) {
            printf("  Slot %d color (slot 1 = BOTTOM): ", j + 1);
            scanf("%d", &bottles[i][j]);
        }
    }

    // pack colours to bottom
    for (int i = 0; i < N; i++)
        normalize_bottle(bottles[i], H);

    int moves_from[MAX_MOVES];
    int moves_to[MAX_MOVES];
    int solution_depth = -1;
    int found = 0;

    // Iterative deepening -> minimum number of steps
    for (int limit = 0; limit <= MAX_MOVES; limit++) {
        if (dfs_limited(bottles, N, H, 0, limit,
                        -1, -1, moves_from, moves_to, &solution_depth)) {
            found = 1;
            break;
        }
    }

    if (found) {
        printf("\nPuzzle solved in %d moves:\n", solution_depth);
        for (int i = 0; i < solution_depth; i++) {
            printf("Step %2d: %d -> %d\n",
                   i + 1, moves_from[i] + 1, moves_to[i] + 1);
        }
    } else {
        printf("\nNo solution found within %d moves.\n", MAX_MOVES);
    }

    return 0;
}
