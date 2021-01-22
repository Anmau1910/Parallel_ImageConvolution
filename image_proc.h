
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <mpi.h>
#include <string.h>

extern float filters[9][3][3];
enum filter { BLUR, BSOBEL, EMBOSS, IDENT, LSOBEL, OUTL, RSOBEL, SHARP, TSOBEL };

void process(const char *filepath, const char *outfile);

