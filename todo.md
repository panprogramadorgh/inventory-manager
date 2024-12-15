## TODO LIST

- Mejorar Database::connect() abriendo solo conexiones si son inexistentes

- Metodo de obtencion de productos por patron de nombre (retorna varios productos con una query)

- Crear modo "console" donde se procesan varias entradas en una misma "sesion" (llamar a la anterior funcion)

- Print new product calculated ID at screen (metodo de productos rem)

- Cambiar como se genera el archivo de base de datos al momento de construir el proyecto (y no en la configuracion) y crear target para inicializar la base de datos (el target es llamao automaticamente si se llama al target install)

- Agregar metodo seguro de obtencion de prouctos (y su respectiva version no segura que lanza excepcion) (mismo concepto para todos los metodos de interaccion con la base de datos)

- Agregar / decrementar cantidad de productos

- Actualizacion de informacion de productos

Ideas y conceptos a implementar

- Mejorar los proveeores, añadir campo de contacto (posiblemente alguno mas)

- Agregar interfaz de gestion de proveedores (VendorManager)

- Actualizacion de informacion de proveedores

- Sistema de notificaciones

- Mejorar ciclo de vida de los productos agregando sistema de directivas para los mismos (ejemplo: cantidad minima de productos como para generar una alerta en el sistema de notificacones)

- Crear sistema de grupos de productos para facilitar la administracion de directivas

- Agregar sistema de informes avanzados basados en datos de productos o grupos de productos (ver productos de un probedor, ver productos con poca cantidad)

- Historial de cambios donde se registra cada operacion y hay una fecha y hora (tiempo de reabastecimiento desde que se lanzo la orden de compra hasta que llego el material)

- Sistema de analisis de datos basado en el historial (sistema de prediccion para agotamiento de un articulo teniendo en cuenta factores como el tiempo de reabastecimiento o sistema de agrupacion de pedidos para ahorrar costes de envio)

- Sistema de accion automatica con el proveedor (contactar en el momento estimado para lanzar una nueva orden de compra)
