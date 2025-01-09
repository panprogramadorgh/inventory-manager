## TODO LIST

- Give up Manager<T> derivated classes system and define Manager<T> custom interface methods making then independent of what kind of ManagerItem is handling

- Mejorar Database::connect() abriendo solo conexiones si son inexistentes

- Cambiar como se genera el archivo de base de datos al momento de construir el proyecto (y no en la configuracion) y crear target para inicializar la base de datos (el target es llamao automaticamente si se llama al target install)

- Actualizar linea de comandos

### To be implemented in next versions 

- Sistema de registro de transacciones en la interfaz de Manager<T>

- Sistema de plugins

- Auditoria y modificacion de productos automatica por el sistema (integrarlo con los plugins)

- Crear sistema de grupos de productos para facilitar la auditoria y manipulacion

- Agregar sistema de informes avanzados basados en datos de productos o grupos de productos (ver productos de un probedor, ver productos con poca cantidad)

- Sistema de analisis de datos basado en el historial (sistema de prediccion para agotamiento de un articulo teniendo en cuenta factores como el tiempo de reabastecimiento o sistema de agrupacion de pedidos para ahorrar costes de envio)

- Sistema de accion automatica con el proveedor (contactar en el momento estimado para lanzar una nueva orden de compra) (se puede crar otro software para proveeores que hagas las veces ed socket TCP y que reciba paquetes desde)
