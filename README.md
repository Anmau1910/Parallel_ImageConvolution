# Filtrado de imágenes

> Andrea Rodríguez y Miguel Bastidas

Compile y ejecute:
```
$ mpicc mpi.c image_proc.c -o filtrado -lm -fopenmp

$ mpirun filtrado <input directory> <output directory> <filtro>
```
Secuencial:
```
$ gcc mpi.c image_proc.c -o filtrado -lm

$ ./filtrado <input directory> <output directory> <filtro>
```

### Filtros:
* 0: blur
* 1: bottom sobel
* 2: emboss
* 3: identity
* 4: left sobel
* 5: outline
* 6: right sobel
* 7: sharpen
* 8: top sobel

## Visión General
Dado un directorio de entrada, un directorio de salida y un filtro, se aplicará el mismo sobre las imágenes de formato bmp encontradas en el directorio de entrada y serán escritas (con el mismo identificador) en el directorio de salida. Todo esto aprovechando los recursos de un sistema de cómputo a través de la paralelización.

## Implementación
El proceso maestro itera sobre el directorio de entrada enviando a los esclavos tanto la ruta de imagenes de entrada bmp, que cada uno debe procesar, como la ruta de imágenes de salida. Esté envío ocurre sobre memoria distribuída mediante pase de mensajes.

Una vez el esclavo recibe la ruta del archivo a procesar y la ruta de destino, procede a cargar la imágen con las herramientas de la librería stb para lectura de imágenes bmp. La imagen será procesada como un arreglo de bytes, para luego invocar un for paralelizado que, por pixel, extrae las tres filas que conforman la submatriz centrada en este, calcula el filtro sobre las mismas y retorna el valor sobre la nueva imágen que posteriormente será escrita en el directorio especificado por el usuario. El for paralelizado ocurre bajo memoria compartida con el uso de las herramientas de paralelización proporcionadas por Open MP.

## Desempeño

> Nota: Los siguientes cálculos se basan en la ejecución del programa sobre directorios con 6, 50 y 100 imagenes.
> La dimensión de la imágen utilizada es de 1717x1370 (2.2 MB).
> El número de threads establecido para OpenMP es 6.

Al estudiar el desempeño de las múltiples implementaciones y configuraciones de unidades de cómputo, se utiliza la métrica de aceleración, comparando con una implementación secuencial. Esto nos permite evaluar de manera directa qué tan tangible es la mejora de tiempo de cómputo, y comparar estos resultados entre sí con facilidad. Luego, se hace una comparativa entre la aceleración alcanzada con cada implementación y sus respectivas eficiencias y efectividades, para así encontrar no solo la implementación más veloz, sino la que resulta en una mejor utilización de recursos.

Los resultados de la medición de tiempo (dadas por el promedio de tres ejecuciones para cada cronometraje), así como el análisis de desempeño, se encuentran a continuación:

### Tiempo secuencial

| 6 imágenes | 50 imágenes | 100 imágenes |
| ---------- | ----------- | ------------ |
| 2.361242s  | 19.206631s  | 37.557241s   |

### Tiempo secuencial (sin el uso de MPI) usando OpenMP

| 6 imágenes | 50 imágenes | 100 imágenes |
| ---------- | ----------- | ------------ |
| 1.679716s  | 14.340627s  | 25.558062s   |

#### Aceleración

* Con 6 imágenes = 2.361242s / 1.679716s = 1.4057389
* Con 50 imágenes = 19.206631s / 14.340627s = 1.339315
* Con 100 imágenes =  37.557241s / 25.558062s  = 1.469487

Se aprecia la mejora con respecto al puramente secuencial con una aceleración promedio de 1.4.

#### Eficiencia

* Con 6 imágenes = 1.4057389 / 6 = 0.234289
* Con 50 imágenes = 1.339315 / 6 = 0.223219
* Con 100 imágenes = 1.469487 / 6 = 0.2449145

Eficiencia promedio 0.23

#### Costo

* Con 6 imágenes = 6* 1.679716s = 10.07
* Con 50 imágenes = 6 * 14.340627s = 86.04
* Con 100 imágenes = 6 * 25.558062s = 153.35

#### Efectividad

* Con 6 imágenes = 1.4057389 / 10.07 = 0.1394818
* Con 50 imágenes = 1.339315 / 86.04 = 0.0155655
* Con 100 imágenes = 1.469487 / 153.35 = 0.0095826

Se aprecia una pequeña mejora con respecto a rendimiento en una implementación que incluye procesamiento con openMP.

### Tiempo Paralelo - 2 procesos sin OpenMP

| 6 imágenes | 50 imágenes | 100 imágenes |
| --------   | ----------- | ------------ |
| 2.214579s  | 17.985999s  | 34.912283s   |

#### Aceleración

* Con 6 imágenes = 2.361242s / 2.214579s = 1.066226
* Con 50 imágenes = 19.206631s / 17.985999s = 1.0678657
* Con 100 imágenes =  37.557241s / 34.912283s = 1.0757601

#### Eficiencia

* Con 6 imágenes = 1.066226 / 2 = 0.533113
* Con 50 imágenes = 1.0678657 / 2 = 0.5339329
* Con 100 imágenes = 1.0757601 / 2 = 0.537880

Eficiencia promedio 0.53.

#### Costo

* Con 6 imágenes = 2* 2.214579s = 4.4292
* Con 50 imágenes = 2 * 17.985999s = 35.97
* Con 100 imágenes = 2 * 34.912283s = 69.825

#### Efectividad

* Con 6 imágenes = 0.533113 / 4.4292 = 0.12036
* Con 50 imágenes = 0.5339329 / 35.97 = 0.015
* Con 100 imágenes = 0.537880 / 69.825 = 0.0077

Con una aceleración promedio de 1.06, se percibe una ligera mejora con respecto al tiempo puramente secuencial, sin embargo es notable la latencia de las comunicaciones con pase de mensajes, como puede apreciarse al comparar las diferencias en la aceleración de la implementación que usa paralelización exclusivamente de grano fino con esta implementación.

### Tiempo Paralelo - 2 procesos con OpenMP

| 6 imágenes | 50 imágenes | 100 imágenes |
| --------   | ----------- | ------------ |
| 2.101408s  | 17.796060s  | 35.480506s   |

#### Aceleración

* Con 6 imágenes = 2.361242s / 2.101408s = 1.123647
* Con 50 imágenes = 19.206631s / 17.796060s = 1.079263
* Con 100 imágenes =  37.557241s / 35.480506s  = 1.058531

Al introducir paralelización de grano fino, se evidencia una mejora promedio de tres centésimas, aumentando la aceleración a 1.09.
El tiempo de ejecución pudo haberse encontrado afectado, adicionalmente, por la arquitectura maestro/esclavo de la implementación. Dados solo dos procesos, uno es el maestro y otro, el esclavo, lo que esencialmente convierte nuestro sistema en una implementación secuencial con paralelización de grano fino, pero añadiendo la sobrecarga de la comunicación entre procesos.

### Tiempo Paralelo - 4 procesos sin OpenMP

| 6 imágenes | 50 imágenes | 100 imágenes |
| --------   | ----------- | ------------ |
| 1.120988s  | 8.150082s   | 16.870632s   |

#### Aceleración

* Con 6 imágenes = 2.361242s / 1.120988s = 2.106393
* Con 50 imágenes = 19.206631s / 8.150082s = 2.356618
* Con 100 imágenes =  37.557241s / 16.870632s = 2.226191


#### Eficiencia

* Con 6 imágenes = 2.106393 / 4 = 0.5265983
* Con 50 imágenes = 2.356618 / 4 = 0.5891545
* Con 100 imágenes = 2.226191 / 4 = 0.5565478

Eficiencia promedio 0.55

#### Costo

* Con 6 imágenes = 4* 1.120988s = 4.48
* Con 50 imágenes = 4 * 8.150082s = 32.600
* Con 100 imágenes = 4 * 16.870632s = 67.48

#### Efectividad

* Con 6 imágenes = 0.5265983 / 4.48 = 0.118
* Con 50 imágenes = 0.5891545 / 32.600 = 0.01
* Con 100 imágenes = 0.5565478 / 67.48 = 0.008

En este punto se empieza a apreciar una mejora importante, en comparación a los resultados obtenidos anteriormente, con una aceleración promedio de 2.13. Esto es debido a que la cantidad de procesos esclavos es aumentada de 1 a 3.

### Tiempo Paralelo - 4 procesos con OpenMP

| 6 imágenes | 50 imágenes | 100 imágenes |
| --------   | ----------- | ------------ |
| 0.942981s  | 7.505590s   | 14.550652s   |

Existe una mejoría con respecto a los valores puramente secuenciales, la ejecución se acorta a la mitad del tiempo, en algunos casos más, sobre todo con el uso de OpenMP.

#### Aceleración

* Con 6 imágenes = 2.361242s / 0.942981s = 2.5040187
* Con 50 imágenes = 19.206631s / 7.505590s = 2.5589768
* Con 100 imágenes = 37.557241s / 14.550652s = 2.5811380

Al analizar la aceleración, se encuentra que el promedio aumenta hasta situarse en 2.55. En perspectiva, se puede observar que la aceleración obtenida al usar OpenMP en este caso es mayor a la aceleración obtenida al usar OpenMP utilizando solo dos procesos. Esto se explica de la misma manera en la que se explica el aumento en la aceleración en el punto anterior: con una mayor cantidad de procesos esclavos.

### Tiempo Paralelo - 6 procesos sin OpenMP

| 6 imágenes | 50 imágenes | 100 imágenes |
| --------   | ----------- | ------------ |
| 1.003396s  | 6.128263s   | 12.664929s   |

#### Aceleración

* Con 6 imágenes = 2.361242s / 1.003396s = 2.3532504
* Con 50 imágenes = 19.206631s / 6.128263s = 3.1341068
* Con 100 imágenes =  37.557241s / 12.664929s = 2.9654522


#### Eficiencia

* Con 6 imágenes = 2.3532504 / 6 = 0.3922084
* Con 50 imágenes = 3.1341068 / 6 = 0.522351
* Con 100 imágenes = 2.9654522 / 6 = 0.494242

Eficiencia promedio 0.47.

#### Costo

* Con 6 imágenes = 6 * 1.003396s = 6.018
* Con 50 imágenes = 6 * 6.128263s = 36.7695
* Con 100 imágenes = 6 * 12.664929s = 75.989

#### Efectividad

* Con 6 imágenes = 0.3922084 / 6.018 = 0.0652
* Con 50 imágenes = 0.522351 / 36.7695 = 0.01
* Con 100 imágenes = 0.494242 / 75.989 = 0.007

Aunque evidentemente la aceleración aumenta al agregar dos procesos trabajadores, con una aceleración promedio de 2.82, podemos observar que el aumento, en general, no es tan dramático como uno podría esperar de agregar 6 unidades de procesamiento a este problema. Esto puede ser debido al tamaño de la data de prueba aunado al *overhead* de la comunicación entre estas unidades de procesamiento. En el análisis de eficiencia y efectividad, se medirá qué tan beneficioso es el aumento de procesos esclavos para problemas de este tamaño.

### Tiempo Paralelo - 6 procesos con OpenMP

| 6 imágenes | 50 imágenes | 100 imágenes |
| --------   | ----------- | ------------ |
| 0.921052s  | 5.813593s   | 11.858716s   |

#### Aceleración

* Con 6 imágenes = 2.361242s / 0.921052s = 2.563636
* Con 50 imágenes = 19.206631s / 5.813593s = 3.303745
* Con 100 imágenes =  37.557241s / 11.858716s = 3.167058

Finalmente, los valores con la reducción de tiempo más significativa (y, por por consiguiente, la mayor aceleración promedio, de 3.01) se obtienen con 6 procesos trabajando en conjunto, añadiendo a esto paralelización de grano fino al procesar cada imagen. De querer finalizar el trabajo con la mayor rapidez posible a cualquier costo, esta sería la opción acertada.

## Conclusión

Dados los resultados de desempeño se percibe cómo a medida que se incrementa el número de procesadores en conjunto con multithreading la aceleración también aumenta. Cabe destacar que dicha ganancia por el uso de hilos solo afecta a n - 1 procesos de los establecidos pues el maestro no ejecuta cómputo intensivo, esto se evidencia al comprobar que el factor de mejora entre los tiempos de ejecución obtenidos sin OpenMP y con OpenMP no alcanza el factor de mejora base obtenido al utilizar paralelización de grano fino únicamente.

La eficiencia superó el 50% en la mitad de las evaluaciones y la aceleración alcanzó 3.3 cuando se combinó MPI y OpenMP con el máximo número de procesos trabajando en conjunto, aunque al llegar a esa aceleración, la eficiencia comienza a disminuir. Esto indica que, dado el tamaño de las pruebas ejecutadas, añadir más unidades de cómputo podría disminuir el tiempo necesario para procesar el conjunto de archivos, pero se utilizarían con menor eficiencia los recursos, impulsando un aumento en costo.

