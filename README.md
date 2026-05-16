<div align="center">
  <h1>Papaya Store</h1>
  <p><b>Sistema de gestión de tienda</b></p>
  <p>Desarrollado por <b>Jonner Paz</b></p>
</div>

> ![IMPORTANT]
> Esta rama pertenece a la reescritura del proyecto en rust, 
> la cual hace cambios tanto en la estructura del proyecto, como del lenguaje,
> así como de otros servicios. **No es compatible con la rama principal**.

---

## ¿Por qué el rewrite?

La idea principal de esta reescritura es **mejorar la experiencia de uso** y **mejorar la performance**, así como escalar la aplicación a algo mucho más grande y complejo que sólo una simple aplicación de terminal. Ya que la aplicación original se planeó como una CLI, se mantendrá esa filosofía, pero conectarán más servicios y la CLI cambiará completamente para que luzca más moderna. Para este trabajo, he prefiero realizar algunos cambios en la arquitectura:

- **El lenguaje principal a utilizar será Rust**. Es un lenguaje de programación **seguro** y **concurrente**. Permite escalar de manera eficiente y eficaz, al menos mejor que su contraparte en c++ con todos los problemas relacionados con no saber utilizar bien la memoria. Además, con su sintaxis moderna permite utilizar herramientas de terceros en un ecosistema más agradable para todo aquel desarrollador que sepa utilizar rust.
- **Ya no será una simple CLI, ahora será una TUI**. Terminal User Interface, o por sus siglas, TUI, son aplicaciones de terminal que permiten utilizar interfaces más agradables y vienen a ser la evolución de simples CLIs. Teniendo en cuenta el tipo de proyecto, este aprovecharía mucho más una interfaz basada en TUI que una CLI
- **Tendrá GUI**. Gracias a los diferentes frameworks de la comunidad, rust tiene librerías donde se podría crear una app web al mismo que se permite aprovechar las características de rust.
- **¿Cuál deberías usar? ¡La que tú quieras!**. Gracias a la separación de responsabilidades, el plan es que cualquier cosa que desees hacer con tu lógica de negocio, sea posible en cualquier vista que estés utilizando.

Este proyecto sirve como aprendizaje personal para aprender sobre el lenguaje rust, programación a bajo nivel, diseño de arquitectura y sobre su ecosistema.


## librerías utilizadas para las vistas

- Para la TUI: [ratatui](https://ratatui.rs/)
- Para la GUI: [tauri](https://v2.tauri.app/)

---

# Correr localmente

Aunque todavía no está preparado para compilar, para correr el proyecto sólo necesitas:

```rust
cargo run 
```

Si quieres estar seguro de que todo funciona:

```rust
cargo check
```

---

## Descripcion general

Papaya Store es una aplicacion de consola para administrar una tienda con modulos de:

- Productos
- Proveedores
- Clientes
- Transacciones (compras y ventas)
- Reportes operativos y de seguridad
- Resumen global de tienda

## Funcionalidades principales

### Productos
<details>
<summary>Ver detalles</summary>
    <ul>
        <li>CRUD completo con borrado logico.</li>
        <li>Relacion obligatoria con proveedor.</li>
        <li>Control de stock, stock minimo y total vendido.</li>
        <li>Restriccion de eliminacion cuando existe en transacciones activas.</li>
    </ul>
</details>

### Proveedores

<details>
<summary>Ver detalles</summary>
    <ul>
        <li>CRUD completo con validaciones de formato.</li>
        <li>Relacion con productos.</li>
        <li>Restriccion de eliminacion con integridad referencial.</li>
    </ul>
</details>

### Clientes

<details>
<summary>Ver detalles</summary>
    <ul>
        <li>CRUD completo con validaciones de formato.</li>
        <li>Relacion con transacciones.</li>
        <li>Restriccion de eliminacion con integridad referencial.</li>
    </ul>
</details>

### Transacciones

<details>
<summary>Ver detalles</summary>
    <ul>
        <li>CRUD completo con validaciones de formato.</li>
        <li>Relacion con clientes y productos.</li>
        <li>Restriccion de eliminacion con integridad referencial.</li>
    </ul>
</details>

### Reportes y administracion

<details>
<summary>Ver detalles</summary>
    <ul>
        <li>Estadisticas de tienda.</li>
        <li>Verificacion de integridad referencial.</li>
        <li>Reporte de stock critico.</li>
        <li>Backup de archivos `.bin`.</li>
        <li>Sincronizacion y resumen de contadores globales en `tienda.bin`.</li>
    </ul>
</details>
