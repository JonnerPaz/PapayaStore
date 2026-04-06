cmake_minimum_required(VERSION 3.16)

project(PapayaStore LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
    src/Bootstrapper.cpp
    src/main_oop.cpp
    src/domain/entities/cliente/Cliente.entity.cpp
    src/domain/entities/entidad.entity.cpp
    src/domain/entities/producto/producto.entity.cpp
    src/domain/entities/proveedor/Proveedor.entity.cpp
    src/domain/entities/tienda/tienda.entity.cpp
    src/domain/entities/transaccion/transaccion.entity.cpp
    src/infrastructure/datasource/admin/FSDatabaseAdmin.cpp
    src/infrastructure/datasource/cliente/FSClienteRepository.cpp
    src/infrastructure/datasource/producto/FSProductoRepository.cpp
    src/infrastructure/datasource/proveedor/FSProveedorRepository.cpp
    src/infrastructure/datasource/transaccion/FSTransaccionRepository.cpp
    src/presentation/CliUtils.cpp
    src/presentation/Menu/Menu.cpp
    src/presentation/Menu/MenuClientes/MenuClientes.cpp
    src/presentation/Menu/MenuProductos/MenuProductos.cpp
    src/presentation/Menu/MenuProveedores/MenuProveedores.cpp
    src/presentation/Menu/MenuTienda/MenuTienda.cpp
    src/presentation/Menu/MenuTransacciones/MenuTransacciones.cpp
    src/presentation/Menu/MenuReportes/MenuReportes.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
