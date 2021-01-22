# Parallel_ImageConvolution

# Filtrado de imágenes

Compile y ejecute:
```
$ mpicc mpi.c image_proc.c -o filtrado -lm -fopenmp

$ mpirun filtrado <input directory> <output directory> <kernel>
```
## Implementación
El proceso maestro itera sobre el directorio de entrada enviando a los esclavos tanto la ruta de imagenes de entrada bmp, que cada uno debe procesar, como la ruta de imágenes de salida.

Una vez el esclavo recibe la ruta del archivo a procesar y la ruta de destino, procede a cargar la imágen con las herramientas de la librería stb para lectura de imágenes bmp. La imagen será procesada como un arreglo de bytes, para luego invocar un for paralelizado que, por pixel, extrae las tres filas que conforman la submatriz centrada en este, calcula el filtro sobre las mismas y retorna el valor sobre la nueva imágen que posteriormente será escrita.

