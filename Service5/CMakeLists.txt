cmake_minimum_required(VERSION 3.20)
project(service5 VERSION 1.4.6.55 LANGUAGES CXX)

# Используем C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOUIC ON)       # Автоматическая генерация ui_*.h
set(CMAKE_AUTOMOC ON)       # Автоматическая генерация moc_*.cpp
set(CMAKE_AUTORCC ON)       # Автоматическая генерация из .qrc


# Определяем Qt модули
find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Sql WebSockets Widgets)
# Если используешь Qt5, замени Qt6 на Qt5 и Widgets убери/добавь через if

# Источники
set(SOURCES
    ../Classes/fileversion.cpp
    ../Classes/logwriter.cpp
    armsoft.cpp
    c5searchengine.cpp
    utils/configini.cpp
    utils/database.cpp
    datadriver/datadriver.cpp
    dlglicenses.cpp
    firebase.cpp
    handlers/httpheader.cpp
    serverthread.cpp
    utils/sqlqueries.cpp
    waiter.cpp
    worker/socketstruct.cpp
)

# Заголовки
set(HEADERS
    ../Classes/fileversion.h
    ../Classes/logwriter.h
    armsoft.h
    c5searchengine.h
    worker/struct_storage_item.h
    utils/configini.h
    utils/database.h
    datadriver/datadriver.h
    dlglicenses.h
    firebase.h
    handlers/httpheader.h
    rc.h
    serverthread.h
    utils/sqlqueries.h
    waiter.h
    worker/socketstruct.h
    worker/struct_goods_item.h
    worker/struct_hall.h
    worker/struct_partner.h
    worker/struct_storage_item.h
)

# Генерация кода Qt (moc, uic)
qt_wrap_ui(UI_HEADERS ${FORMS})
qt_wrap_cpp(MOC_HEADERS ${HEADERS})

# Основной исполняемый файл
if(WIN32)
    list(APPEND SOURCES main.cpp)
elseif(UNIX)
    list(APPEND SOURCES main_linux.cpp)
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_HEADERS} ${MOC_HEADERS} dlglicenses.ui)

# Добавляем include пути
target_include_directories(${PROJECT_NAME} PRIVATE
    handlers
    headers
    store
    datadriver
    utils
    worker
    ../Service5Working/raw
    ../Service5Working/socket
    ../Classes
)

# Связываем Qt библиотеки
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Sql
    Qt6::WebSockets
    Qt6::Widgets
)

# Платформенные зависимости
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Version wsock32 advapi32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator'\""
    )
elseif(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-Wl,--build-id"
    )
endif()

# Определяем макросы
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _DBDRIVER_=\"QMYSQL\"
    _ORGANIZATION_=\"BreezeDevs\"
    _APPLICATION_=\"Cafe5\"
    _MODULE_=\"Service5\"
    QSSLSOCKET_DEBUG
    APP_VERSION=\"${PROJECT_VERSION}\"
)
