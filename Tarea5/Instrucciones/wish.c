#include <stdio.h>
#include <unistd.h>

void init_w(int maxx, int maxy)
{
    setlinebuf(stdout);
    printf("canvas .jo -relief raised -width %d -height %d\n", maxx, maxy);
    printf("pack .jo\n");
}


void close_w(char *msg, int x, int y)
{
    printf(".jo create text %d %d -text \"%s\"\n", x, y, msg);
    sleep(10);
    printf("exit\n");
    fclose(stdout);
}

int create_obj_w(int x, int y, int gordo, char *type, char *color)
{
static int item = 0;

    if(strcmp(type, "text") == 0)
    printf(".jo create %s %d %d -text \"%s\"\n", type, x, y, color);
    else
    printf(".jo create %s %d %d %d %d -fill %s\n", type, x, y, x+gordo, y+gordo, color);
    return(++item);
}

void delete_w(int item) {
    printf(".jo delete %d\n", item);
}

void move_w(int item, int dx, int dy)
{
    printf(".jo move %d %d %d\n", item, dx, dy);
}

void coords_w(int item, int x, int y, int gordo)
{
    printf(".jo coords %d %d %d %d %d\n", item, x, y, x+gordo, y+gordo);
}
