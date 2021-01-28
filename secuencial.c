#include "image_proc.h"
#include <time.h>

int main(int argc, char **argv) {
	DIR *in_dir, *out_dir;
	int filt;

	struct dirent *de;

	int s,i = 0;
	char file[256];
	char end = '\0';

	struct timespec start, finish;
	double elapsed;

	//Error checking --------------------------------------------------
	if (argc != 4) {
		fprintf(stderr, "Usage %s input_dir output_dir filter\n", argv[0]);
		fprintf(stderr, "Filters:\n");
		fprintf(stderr, "\t0: blur\n");
		fprintf(stderr, "\t1: bottom sobel\n");
		fprintf(stderr, "\t2: emboss\n");
		fprintf(stderr, "\t3: identity\n");
		fprintf(stderr, "\t4: left sobel\n");
		fprintf(stderr, "\t5: outline\n");
		fprintf(stderr, "\t6: right sobel\n");
		fprintf(stderr, "\t7: sharpen\n");
		fprintf(stderr, "\t8: top sobel\n");
		exit(EXIT_FAILURE);
	}

	if (!(in_dir = opendir(argv[1]))) {
		perror("Error abriendo directorio de entrada: ");
		exit(EXIT_FAILURE);
	}

	if (!(out_dir = opendir(argv[2]))) {
		perror("Error abriendo directorio de salida: ");
		closedir(in_dir);
		exit(EXIT_FAILURE);
	}

	if (strlen(argv[3]) > 1 || argv[3][0] - '0' > 8 || argv[3][0] - '0' < 1) {
		fprintf(stderr, "Filtro no reconocido\n");
		exit(EXIT_FAILURE);
	}
	//End error checking --------------------------------------------------
	filt = argv[3][0] - '0';

	clock_gettime(CLOCK_MONOTONIC, &start);

	while ((de = readdir(in_dir))) {

		if (de->d_type != DT_REG || !strstr(de->d_name, ".bmp")) 
			continue;

		//building string with input and output path to send:
		strcpy(file, argv[1]);
		s = strlen(file);
		file[s] = '/';
		strcpy(file +s +1, de->d_name);
		strcpy(file + strlen(file) + 1, argv[2]);
		s = strlen(file) + strlen(argv[2]);
		file[s + 1] = '/';
		strcpy(file + s + 2, de->d_name);

		process(file, file + strlen(file) + 1, filt);

		memset(file, 0, 256);
	}

	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

	printf("El filtrado de imágenes toma %f segundos en ejecutarse\n", elapsed);
	
	exit(EXIT_SUCCESS);
}
