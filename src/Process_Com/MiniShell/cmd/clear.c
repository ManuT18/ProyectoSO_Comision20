#include <stdio.h>

int main(int argc, char **args) {
    // https://stackoverflow.com/questions/37774983/clearing-the-screen-by-printing-a-character
    printf("\033[H\033[J"); // código de escape ANSI para limpiar la pantalla
    fflush(stdout);

    return 0;
}