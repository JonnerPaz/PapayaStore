cmake_minimum_required(VERSION 3.16)

project(PapayaStore LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 1. Core Library (Dominio e Infraestructura)
add_library(papaya_core STATIC
    src/domain/entities/cliente/Cliente.entity.cpp
    src/domain/entities/entidad.entity.cpp
    src/domain/entities/producto/producto.entity.cpp
    src/domain/entities/proveedor/Proveedor.entity.cpp
    src/domain/entities/tienda/tienda.entity.cpp
    src/domain/entities/transaccion/transaccion.entity.cpp
    src/domain/utils/utils.cpp
    src/infrastructure/datasource/admin/FSDatabaseAdmin.cpp
    src/infrastructure/datasource/cliente/FSClienteRepository.cpp
    src/infrastructure/datasource/producto/FSProductoRepository.cpp
    src/infrastructure/datasource/proveedor/FSProveedorRepository.cpp
    src/infrastructure/datasource/transaccion/FSTransaccionRepository.cpp
)

target_include_directories(papaya_core PUBLIC ${CMAKE_SOURCE_DIR}/src)

# 2. Executable (CLI Actual)
add_executable(${PROJECT_NAME}
    src/Bootstrapper.cpp
    src/main.cpp
    src/presentation/cli/CliUtils.cpp
    src/presentation/cli/Menu/Menu.cpp
    src/presentation/cli/Menu/MenuClientes/MenuClientes.cpp
    src/presentation/cli/Menu/MainMenu/MainMenu.cpp
    src/presentation/cli/Menu/MenuProductos/MenuProductos.cpp
    src/presentation/cli/Menu/MenuProveedores/MenuProveedores.cpp
    src/presentation/cli/Menu/MenuTransacciones/MenuTransacciones.cpp
    src/presentation/cli/Menu/MenuReportes/MenuReportes.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE papaya_core)

# 3. Python Bindings (Pybind11)
if (EXISTS "${CMAKE_SOURCE_DIR}/.venv/bin/python")
    set(Python3_EXECUTABLE "${CMAKE_SOURCE_DIR}/.venv/bin/python" CACHE FILEPATH "Python executable" FORCE)

    execute_process(
        COMMAND "${Python3_EXECUTABLE}" -c "import sys; print(sys.base_prefix)"
        OUTPUT_VARIABLE UV_PYTHON_ROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (UV_PYTHON_ROOT)
        set(Python3_ROOT_DIR "${UV_PYTHON_ROOT}" CACHE PATH "Python root directory" FORCE)
    endif()
endif()

find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT PYBIND11_CMAKE_DIR)
    message(FATAL_ERROR "No se pudo localizar pybind11 via Python. Instala pybind11 en el entorno uv.")
endif()

list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_CMAKE_DIR}")
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

set(PAPAYA_BINDINGS_SOURCES
    src/bindings/module.cpp
    src/bindings/database/init_database_binding.cpp
    src/bindings/admin/database_admin_binding.cpp
    src/bindings/context/papaya_context.cpp
    src/bindings/context/papaya_context_binding.cpp
    src/bindings/common/header_file_binding.cpp
    src/bindings/common/entidad_base_binding.cpp
    src/bindings/entities/producto_binding.cpp
    src/bindings/entities/proveedor_binding.cpp
    src/bindings/entities/cliente_binding.cpp
    src/bindings/entities/transaccion_binding.cpp
    src/bindings/entities/tienda_binding.cpp
    src/bindings/repositories/producto_repository_binding.cpp
    src/bindings/repositories/proveedor_repository_binding.cpp
    src/bindings/repositories/cliente_repository_binding.cpp
    src/bindings/repositories/transaccion_repository_binding.cpp
)

pybind11_add_module(papaya_backend ${PAPAYA_BINDINGS_SOURCES})
target_link_libraries(papaya_backend PRIVATE papaya_core)
