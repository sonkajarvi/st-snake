#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cglm/types.h>
#include <glad/gl.h>

#include <st/instance.h>
#include <st/window.h>
#include <st/graphics/renderer.h>
#include <st/graphics/draw_text.h>
#include <st/input/keyboard.h>

#define SIZE 32
#define ROWS 16
#define COLS 16

#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

ivec2 snake, apple;
int board[ROWS][COLS];
unsigned int direction, score;
float counter;
bool lost;

void reset(void)
{
    snake[0] = ROWS / 2;
    snake[1] = COLS / 2;
    apple[0] = rand() % ROWS;
    apple[1] = rand() % COLS;
    memset(board, 0, sizeof(board));
    direction = NORTH;
    score = 2;
    lost = false;
    counter = 0.0f;
}

int main(void)
{
    st_hello();

    StWindow *window = st_window_create("st snake", SIZE * ROWS, SIZE * COLS);
    st_window_set_vsync(window, true);

    // todo: Don't call OpenGL directly
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    srand(time(NULL));
    reset();

    st_window_show(window);
    while (!st_window_should_close(window)) {
        st_window_poll_events(window);

        if (key_press(ST_KEY_R))
            reset();

        if (lost)
            goto skip;

        if (key_press(ST_KEY_W) && direction != SOUTH) direction = NORTH;
        if (key_press(ST_KEY_D) && direction != WEST ) direction = EAST;
        if (key_press(ST_KEY_S) && direction != NORTH) direction = SOUTH;
        if (key_press(ST_KEY_A) && direction != EAST ) direction = WEST;

        counter += st_window_deltatime();
        if (counter > 0.1f) {
            counter = 0.0f;

            snake[0] += direction == EAST  ? 1 : -(direction == WEST);
            snake[1] += direction == NORTH ? 1 : -(direction == SOUTH);

            if (board[snake[0]][snake[1]] > 0)
                lost = true;
            
            board[snake[0]][snake[1]] = score;

            for (int x = 0; x < ROWS; x++) {
                for (int y = 0; y < COLS; y++)
                    board[x][y] -= (board[x][y] > 0);
            }
        }

        if (snake[0] < 0 || snake[0] >= ROWS || snake[1] < 0 || snake[1] >= COLS)
            lost = true;

        if (snake[0] == apple[0] && snake[1] == apple[1]) {
            while (board[apple[0]][apple[1]] > 0) {
                apple[0] = rand() % ROWS;
                apple[1] = rand() % COLS;
            }

            score++;
        }

skip:
        st_draw_begin(window);

        const int h = SIZE / 2;
        const int n = h - 1;
        
        for (int x = 0; x < ROWS; x++) {
            for (int y = 0; y < COLS; y++) {
                if (board[x][y] > 0) {
                    st_draw_quad(window,
                        (vec3){SIZE * x + h, SIZE * y + h, 0.0f},
                        (vec3){0.0f, 0.0f, 0.0f},
                        (vec3){n, n, 0.0f},
                        (vec4){0.2f, 0.8f - ((float)board[x][y] / score / 2.0f), 0.5f, 1.0f});
                    
                    continue;
                }

                if ((x & 1 && y & 1) || (~x & 1 && ~y & 1)) {
                    st_draw_quad(window,
                        (vec3){SIZE * x + h, SIZE * y + h, 0.0f},
                        (vec3){0.0f, 0.0f, 0.0f},
                        (vec3){h, h, 0.0f},
                        (vec4){0.11f, 0.11f, 0.11f, 1.0f});
                }
            }
        }
        
        st_draw_quad(window,
            (vec3){SIZE * apple[0] + h, SIZE * apple[1] + h, 0.0f},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){n, n, 0.0f},
            (vec4){0.8f, 0.2f, 0.2f, 1.0f});

        static char score_buf[16];
        snprintf(score_buf, 16, "Score: %d", score - 2);
        st_draw_string(window,
            (vec3){10.0f, SIZE * COLS - 10.0f, 0.0f},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){8.0f, 8.0f, 0.0f},
            (vec4){1.0f, 1.0f, 1.0f, 1.0f},
            score_buf);
        
        if (lost) {
            st_draw_string(window,
                (vec3){SIZE * ROWS / 2 - 32.0f, SIZE * COLS / 2 + 64.0f, 0.0f},
                (vec3){0.0f, 0.0f, 0.0f},
                (vec3){8.0f, 8.0f, 0.0f},
                (vec4){1.0f, 1.0f, 1.0f, 1.0f},
                "You lost!");

            st_draw_string(window,
                (vec3){SIZE * ROWS / 2 - 64.0f, SIZE * COLS / 2, 0.0f},
                (vec3){0.0f, 0.0f, 0.0f},
                (vec3){8.0f, 8.0f, 0.0f},
                (vec4){1.0f, 1.0f, 1.0f, 1.0f},
                "Press 'R' to reset");
        }

        st_draw_end(window);

        st_window_swap_buffers(window);
    }

    st_window_destroy(window);
    st_goodbye();

    return 0;
}
