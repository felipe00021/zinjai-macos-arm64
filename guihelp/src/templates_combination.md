### Combinación de Plantillas

Esta funcionalidad permite fusionar o reemplazar los [perfiles](projects.html#config) de configuración de un proyecto con los otro o de una plantilla. Está pensada para actualizar proyectos creados a partir de plantillas cuando se actualizan las plantillas, y para generar combinaciones que utilicen configuraciones de más de una plantilla a la vez.

Para actualizar un proyecto, o combinarlo con otra plantilla, debe abrir el proyecto en cuestión y acceder a esta opción mediante el botón *Combinar/Actualizar Plantilla* de la pestaña [Más](project_general_config.html#mas) del cuadro de [Configuración de Proyecto](project_general_config.html).

En primer lugar, *ZinjaI* le mostrará la lista de plantillas de proyecto disponibles para que seleccione una. También dispondrá de una opción adicional al final de la lista que le permitirá abrir un archivo *.zpr* de otro proyecto, para combinar así los perfiles de dos proyectos existentes  en lugar de un proyecto y una plantilla. Una vez seleccionada la plantilla, o el segundo proyecto, aparecerá el cuadro de diálogo "Combinar Plantillas".

![](templates_combination.png)

En este cuadro, a la izquierda se presentan 3 columnas: perfiles del proyecto actual, acción a realizar, y perfiles del segundo proyecto. Allí puede determinar para cada peril del proyecto actual, si desea combinarlo o reemplazarlo con un perfil del otro, o no modificarlo. La acción "combinar" suma los campos de ambos perfiles. Por ejemplo, en la lista de directorios adicionales para cabeceras, quedarán concatenadas las listas de ambos proyectos. La opción reemplazar reemplaza el valor del perfil del proyecto actual con el del segundo proyecto (esto implica que se perderá el valor actual).

Por último, a la derecha del cuadro de diálogo se listan los campos de los perfiles y de la configuración general que pueden combinarse, junto a casillas de verificación para que puede filtrar (desactivando) los que desee que no se actualicen.
