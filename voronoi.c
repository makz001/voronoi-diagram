#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define COLS 300
#define ROWS 300

#define BG_COLOR    0X181818FF
#define RED_COLOR   0XFF0000FF
#define WHITE_COLOR 0XFFFFFFFF

#define OUTPUT_FILE "output.ppm"

#define POINT_COUNT 4
#define POINT_RADIUS 5
#define POINT_COLOR RED_COLOR

typedef uint32_t Color;

Color image[ROWS][COLS];

#define PALETTE_COLOR_COUNT 4

#define PALETTE 2

Color palettes[][PALETTE_COLOR_COUNT] = {
    [0] = {
        0X352f44FF,
        0X5c5470FF,
        0Xb9b4c7FF,
        0Xfaf0e6FF,
    },
    [1] = {
        0X793fdfFF,
        0X7091f5FF,
        0X97fff4FF,
        0Xfffd8cFF,
    },
    [2] = {
        0X6d3580FF,
        0Xcc4165FF,
        0Xe4734fFF,
        0Xffe26fFF,
    },
};

typedef struct {
    int x, y;
} Point;

Point points[POINT_COUNT];

void fill_image(Color color)
{
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            image[i][j] = color;
        }
    }
}

void save_to_file(char *file)
{
    FILE *f = fopen(file, "wb");
    if (f == NULL) {
        fprintf(stderr, "could not open file: %s\n", file);
        exit(1);
    }

    fprintf(f, "P6\n%d %d 255\n", COLS, ROWS);

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            /* 0XRRGGBBAA */
            char pixel[3] = {
                (image[i][j] & 0XFF000000) >> 8 * 3,
                (image[i][j] & 0X00FF0000) >> 8 * 2,
                (image[i][j] & 0X0000FF00) >> 8 * 1
            };

            fwrite(pixel, sizeof(pixel), 1, f);
        }
    }

    fclose(f);
}

void generate_points()
{
    for (int i = 0; i < POINT_COUNT; ++i) {
        points[i].x = rand() % COLS;
        points[i].y = rand() % ROWS;
    }
}

int distance(int x1, int x2, int y1, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;

    return dx * dx + dy * dy;
}

void voronoi(int palette)
{
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            int closest = 0;

            for (int y = 1; y < POINT_COUNT; ++y) {
                if (distance(points[closest].x, j, points[closest].y, i) > distance(points[y].x, j, points[y].y, i)) {
                    closest = y;
                }

            }

            image[i][j] = palettes[palette][closest % PALETTE_COLOR_COUNT];
        }
    }
}

void draw_point(Point point, Color color)
{
    int x0 = point.x - POINT_RADIUS;
    int x1 = point.x + POINT_RADIUS;
    int y0 = point.y - POINT_RADIUS;
    int y1 = point.y + POINT_RADIUS;

    for (int i = x0; i <= x1; ++i) {
        if (i >= 0 && i < COLS) {
            for (int j = y0; j < y1; ++j) {
                if (j >= 0 && j < ROWS) {
                    if (distance(point.x, i, point.y, j) <= POINT_RADIUS * POINT_RADIUS) {
                        image[j][i] = color;
                    }
                }
            }
        }
    }
}

void draw_points()
{
    for (int i = 0; i < POINT_COUNT; ++i) {
        draw_point(points[i], POINT_COLOR);
    }
}

int main(void)
{
    srand(time(NULL));
    generate_points();
    voronoi(PALETTE);
    save_to_file(OUTPUT_FILE);

    return 0;
}
