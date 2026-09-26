### Diálogo de Configuración del Análisis Estático

Este cuadro de dialogo permite configurar diferentes opciones del análisis estático (argumentos para cppcheck) relacionadas al tipo errores que el análisis busca y los errores particulares o archivos fuentes que debe omitir. Se accede a él desde el ítem *Configurar...* del submenú *Análisis Estático* del menú *Herramientas*. Para una mejor descripción de este tipo de análisis y cómo se utiliza en *ZinjaI* vea [Análisis Estático](cppcheck.html).


El cuadro de diálogo muestra dos pestañas. La pestaña *General* permite definir opciones varias que se describen a continuación:

*  **Copiar configuración de las opciones del proyecto**: Puede haber bloques de código que se compilen condicionalmente (#ifdef...). En estos casos, se puede suministrar a cppcheck un conjunto de constantes de preprocesador ya definidas para determinar si analizar o no estos bloques, o esperar que analice todas las posibles combinaciones. Para lo primero, se puede tomar la lista de constantes definidas en las [Opciones de Compilación y Ejecución de Proyecto](project_config.html) (incluyendo las definidas en el campo específico para macros y las definidas en el campo de argumentos adicionales para la compilación), o especificar una lista diferente. Si elije suministrar una lista difernte (es decir, no copiar del perfil de compilación) debe introducir la lista de constantes a definir en el campo *Configuraciones a verificar* y la lista de constantes a considerar como definidas en el campo *Configuraciones a saltear*. Si no hay constantes definidas (ya sea en la lista específica, o en el perfil de compilación si se elige copiar), CppCheck intentará verificar todas las combinaciones, mientras no sean más que una cierta cantidad predefinida (12).

*  **Verificaciones adicionales**: Hay un conjunto de verificaciones básicas que CppCheck realiza siempre, y otros conjuntos de verificaciones adicionales que se pueden activar. Se deben listar en este campo el nombre de algunos de estos o la palabra "all" para activarlos a todos. Las opciones posibles son *style*, *performance*, *portability*, *information*, *unusedFunction* y *missingInclude*.

*  **Verificaciones específicas de una plataforma**: Para que cppcheck considere los tipos de datos y tamaños específicos de una plataforma, debe ingresarla en este campo. Las posibles opciones son: *unix32*, *unix64*, *win32A*, *win32W* y *win64*. La diferencia entre las dos versiones de win32 es que la primera utiliza codificación ASCII mientras que la segunda UNICODE.

*  **Verificaciones específicas de un estándar**: Habilita algunas verificaciones adicionales relacionadas específicamente a un estándar. Las opciones válidas son *posix*, *c99* y *c++11*.

*  **Supresiones**: Las supresiones permiten omitir en la salida determinados errores o conjuntos de ellos. Para especificar una supressión puede especificar simplemente el id del error (omite todos los errores de ese tipo), el id y el nombre de un archivo separados por dos puntos (ej: uninitVars:mi_clase.cpp, omite todos los errores de ese tipo generados por el archivo mi_clase.cpp), o agregando al final un número de línea también separado por dos puntos (ej: uninitVars:mi_clase.cpp:57, omite el error de id uninitVars de la linea 57 del archivo mi_clase.cpp). El id de un error se observa en *ZinjaI* en el panel de resultados; el id aparece entre paréntesis luego del nombre del archivo donde se produce (más aún, puede agregar el id a la lista de referencias directamente mediante click derecho sobre el mismo en dicho panel).

*  **Archivo con listas de supresiones**: Alternativamente, en lugar de ingresar las supresiones en el campo *Supresiones* se pueden ingresar en un archivo de texto y especificar en este campo el nombre de dicho archivo.

*  **Habilitar supresiones inline**: Las supresiones inline son supresiones que se especifican en el mismo código fuente como comentarios. Para que cppcheck omita un error hay que agregar un comentario con el texto "cppcheck-suppress" y el id del error separado por un espacio (por ejemplo, "// cppcheck-suppress memleakOnRealloc") en la linea anterior a la que produce el error.

*  **Habilitar paralelizar ejecución**: Esta opción habilita el uso del argumento "-j" al ejecutar cpp-check. Esto hará que cpp-check lanze más de un proceso, para analizar varios fuentes en simultáneo. La cantidad de procesos se tomará de la cantidad definida para el proceso de compilación en la pestaña [Proyecto](preferences.html#programa_simple) del cuadro de [Preferencias](preferences.html).

*  **Guardar esta configuración con el proyecto**: Si activa esta opción las configuraciones definidas en esta ventana se guardan dentro del archivo de proyecto. En caso contrarior, se pierden al cerrar el proyecto.


La pestaña archivos permite seleccionar qué archivos serán analizados y qué archivos serán excluidos del análisis. La lista incluye sólamente a los fuentes (.c/.cpp) y no a las cabeceras (.h/.hpp) porque éstas se procesan cuando se procesan los fuentes que las incluyen.
