cmake_minimum_required(VERSION 3.21)

project(Waiter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ---------- Qt ----------
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    PrintSupport
    Multimedia
    AxContainer
    WebSockets
    HttpServer
)

qt_standard_project_setup()

# ---------- DEFINES ----------
add_definitions(
    -D_ORGANIZATION_="Jazzve"
    -D_APPLICATION_="Cafe5"
    -D_MODULE_="waiter"
    -DWAITER
    -D_CRYPT_KEY_="shop2022!!!"
    -DBORDERLESSDIALOGS
    -DQT_DEPRECATED_WARNINGS
)

# ---------- INCLUDE PATHS ----------
include_directories(
    ../Cafe5
    ../Classes
    ../Cache
    ../Controls
    ../Editors
    ../Forms
    ../Service
    ../Configs
    ../Printing
    ../NTable
    ../Service5Working/raw
    ../Service5Working/socket
    ../Service5/worker
    ../DbData
    C:/Development/OpenSSL-Win64/include
    C:/Development/projects/desktop.newtax/Src
)

# ---------- SOURCES ----------
file(GLOB_RECURSE APP_SOURCES
    *.cpp
    *.h
    *.ui
    *.qrc
)

set(APP_RESOURCES
    res.rc
)

qt_add_executable(Waiter
    ${APP_SOURCES}
    ${APP_RESOURCES}
    ../Cafe5/c5double.cpp
    ../Cafe5/c5systempreference.cpp
    ../Cafe5/calendar.cpp
    ../Classes/QProgressIndicator.cpp
    ../Classes/amkbd.cpp
    ../Classes/c5gridgilter.cpp
    ../Classes/c5random.cpp
    ../Classes/c5replacecharacter.cpp
    ../Classes/c5tabledata.cpp

    ../Classes/c5threadobject.cpp
    ../Classes/calculator.cpp
    ../Classes/dict_payment_type.cpp
    ../Classes/dlgpassword.cpp
    ../Classes/idram.cpp
    ../Classes/logwriter.cpp
    ../Classes/notificationwidget.cpp
    ../Configs/breezeconfig.cpp
    ../Configs/configs.cpp
    ../Controls/c5guicontrols.cpp
    ../Controls/combosearchview.cpp
    ../Forms/barcode.cpp
    ../Forms/c5changepriceofgroup.cpp
    ../Forms/c5connectiondialog.cpp
    ../Forms/c5daterange.cpp
    ../Forms/c5dbresetoption.cpp
    ../Forms/c5filtervalues.cpp
    ../Forms/c5progressdialog.cpp
    ../Forms/c5storedocselectprinttemplate.cpp
    ../Forms/c5toolbarwidget.cpp
    ../Forms/dlgconfigtable.cpp
    ../Forms/dlgguestinfo.cpp
    ../Forms/dlglist2.cpp
    ../Forms/dlglistofdishcomments.cpp
    ../Forms/dlgqty.cpp
    ../Forms/dlgsplashscreen.cpp
    ../Forms/dlgviewcashreport.cpp
    ../Forms/httpquerydialog.cpp
    ../Forms/mfprocessproductpriceupdate.cpp
    ../Forms/touchdlgphonenumber.cpp
    ../Forms/touchentertaxreceiptnumber.cpp
    ../Forms/touchselecttaxreport.cpp
    ../NTable/ndataprovider.cpp
    ../NTable/nfilterdlg.cpp
    ../NTable/ninterface.cpp
    ../NTable/nloadingdlg.cpp
    ../NTable/nsearchdlg.cpp
    ../NTable/ntablemodel.cpp
    ../Printing/selectprinters.cpp
    ../Service5/worker/struct_workstationitem.cpp
    ../Service5Working/utils/configini.cpp
    c5waiterdialog.cpp
    dishitembutton.cpp
    dlgdashboard.cpp
    dlgeditphone.cpp
    dlgguests.cpp
    dlglist.cpp
    dlglistdishspecial.cpp
    dlgprecheckoptions.cpp
    dlgrealreports.cpp
    dlgreportparams.cpp
    dlgreportsfilter.cpp
    dlgscreen.cpp
    dlgsearchinmenu.cpp
    dlgsimleoptions.cpp
    dlgsplitorder.cpp
    dlgstafflist.cpp
    dlgstoplistoption.cpp
    dlgtables.cpp
    dlgtext.cpp
    dlgtimeorder.cpp
    dlgviewstoplist.cpp
    goodsgroupbutton.cpp
        main.cpp
        dlgface.cpp
    ../Cafe5/c5dialog.cpp
    ../Cafe5/c5utils.cpp
    ../Cafe5/c5message.cpp
    dlgorder.cpp
    ../Classes/c5user.cpp
    ../Classes/c5menu.cpp
    ../Classes/c5witerconf.cpp
    ../Classes/c5printing.cpp
    ../Classes/QRCodeGenerator.cpp
    ../Controls/c5lineedit.cpp
    ../Controls/c5checkbox.cpp
    ../Controls/c5dateedit.cpp
    ../Controls/c5graphicsview.cpp
    ../Classes/c5widget.cpp
    qdishbutton.cpp
    tablewidget.cpp
    waiterdishwidget.cpp
    waiterguestwidget.cpp
    waitermodificatorwidget.cpp
    waiterorderitemwidget.cpp
    waiterordermodel.cpp
    waitertablewidget.cpp
    dlgdishremovereason.cpp
    ../Classes/c5printjson.cpp
    dlgreports.cpp
    dlgguest.cpp
    ../Classes/rkeyboard.cpp
    dlgreceiptlanguage.cpp
    dlgcl.cpp
    ../Classes/c5permissions.cpp
    dlgexitwithmessage.cpp


    ../Classes/fileversion.cpp
    wplast30sessions.cpp
    wpwidget.cpp
    ../Cafe5/c5double.h
    ../Cafe5/c5systempreference.h
    ../Cafe5/calendar.h
    ../Classes/QProgressIndicator.h
    ../Classes/amkbd.h
    ../Classes/barcode5.h
    ../Classes/c5gridgilter.h
    ../Classes/c5random.h
    ../Classes/c5replacecharacter.h
    ../Classes/c5tabledata.h

    ../Classes/c5threadobject.h
    ../Classes/calculator.h
    ../Classes/chatmessage.h
    ../Classes/dict_currency.h
    ../Classes/dict_dish_state.h
    ../Classes/dict_goods_types.h
    ../Classes/dict_payment_type.h
    ../Classes/dict_workstation.h
    ../Classes/dlgpassword.h
    ../Classes/format_bytes.h
    ../Classes/goodsreserve.h
    ../Classes/idram.h
    ../Classes/logwriter.h
    ../Classes/notificationwidget.h
    ../Configs/breezeconfig.h
    ../Configs/configs.h
    ../Controls/c5guicontrols.h
    ../Controls/combosearchview.h
    ../Forms/barcode.h
    ../Forms/c5changepriceofgroup.h
    ../Forms/c5connectiondialog.h
    ../Forms/c5daterange.h
    ../Forms/c5dbresetoption.h
    ../Forms/c5filtervalues.h
    ../Forms/c5progressdialog.h
    ../Forms/c5storedocselectprinttemplate.h
    ../Forms/c5toolbarwidget.h
    ../Forms/dlgconfigtable.h
    ../Forms/dlgguestinfo.h
    ../Forms/dlglist2.h
    ../Forms/dlglistofdishcomments.h
    ../Forms/dlgqty.h
    ../Forms/dlgsplashscreen.h
    ../Forms/dlgviewcashreport.h
    ../Forms/httpquerydialog.h
    ../Forms/mfprocessproductpriceupdate.h
    ../Forms/touchdlgphonenumber.h
    ../Forms/touchentertaxreceiptnumber.h
    ../Forms/touchselecttaxreport.h
    ../NTable/ndataprovider.h
    ../NTable/nfilterdlg.h
    ../NTable/ninterface.h
    ../NTable/nloadingdlg.h
    ../NTable/nsearchdlg.h
    ../NTable/ntablemodel.h
    ../Printing/selectprinters.h
    ../Service5/worker/struct_dish.h
    ../Service5/worker/struct_goods_group.h
    ../Service5/worker/struct_hall.h
    ../Service5/worker/struct_parent.h
    ../Service5/worker/struct_table.h
    ../Service5/worker/struct_waiter_dish.h
    ../Service5/worker/struct_waiter_order.h
    ../Service5/worker/struct_workstationitem.h
    ../Service5Working/utils/configini.h
    ../Service5Working/utils/servicecommands.h
    c5waiterdialog.h
    dlgdashboard.h
    dlgeditphone.h
        dlgface.h
    ../Cafe5/c5dialog.h
    ../Cafe5/c5utils.h
    ../Cafe5/c5message.h
    dlgguests.h
    dlglist.h
    dlglistdishspecial.h
    dlgorder.h
    ../Classes/c5user.h
    ../Classes/c5menu.h
    ../Classes/c5witerconf.h
    ../Classes/c5printing.h
    ../Classes/QRCodeGenerator.h
    ../Controls/c5lineedit.h
    dlgprecheckoptions.h
    dlgrealreports.h
    dlgreportparams.h
    dlgreportsfilter.h
    dlgscreen.h
    dlgsearchinmenu.h
    dlgsimleoptions.h
    dlgsplitorder.h
    dlgstafflist.h
    dlgstoplistoption.h
    dlgtables.h
    dlgtext.h
    dlgtimeorder.h
    dlgviewstoplist.h
    goodsgroupbutton.h
    qdishbutton.h
    rc.h
    ../Controls/c5checkbox.h
    ../Controls/c5dateedit.h
    ../Controls/c5graphicsview.h
    ../Classes/c5widget.h
    tablewidget.h
    waiterdishwidget.h
    waiterguestwidget.h
    waitermodificatorwidget.h
    waiterorderitemwidget.h
    waiterordermodel.h
    waitertablewidget.h
    dlgdishremovereason.h
    ../Classes/c5printjson.h
    dlgreports.h
    dlgguest.h
    ../Classes/rkeyboard.h
    dlgreceiptlanguage.h
    dlgcl.h
    ../Classes/c5permissions.h
    dlgexitwithmessage.h


    ../Classes/fileversion.h
    wplast30sessions.h
    wpwidget.h
    ../Cafe5/calendar.ui
    ../Classes/c5gridgilter.ui
    ../Classes/calculator.ui
    ../Classes/dlgpassword.ui
    ../Classes/notificationwidget.ui
    ../Controls/combosearchview.ui
    ../Forms/c5changepriceofgroup.ui
    ../Forms/c5connectiondialog.ui
    ../Forms/c5daterange.ui
    ../Forms/c5dbresetoption.ui
    ../Forms/c5filtervalues.ui
    ../Forms/c5progressdialog.ui
    ../Forms/c5storedocselectprinttemplate.ui
    ../Forms/c5toolbarwidget.ui
    ../Forms/dlgconfigtable.ui
    ../Forms/dlgguestinfo.ui
    ../Forms/dlglist2.ui
    ../Forms/dlglistofdishcomments.ui
    ../Forms/dlgqty.ui
    ../Forms/dlgsplashscreen.ui
    ../Forms/dlgviewcashreport.ui
    ../Forms/httpquerydialog.ui
    ../Forms/mfprocessproductpriceupdate.ui
    ../Forms/touchdlgphonenumber.ui
    ../Forms/touchentertaxreceiptnumber.ui
    ../Forms/touchselecttaxreport.ui
    ../NTable/nfilterdlg.ui
    ../NTable/nloadingdlg.ui
    ../NTable/nsearchdlg.ui
    ../Printing/preorders.ui
    ../Printing/selectprinters.ui
    dlgdashboard.ui
    dlgeditphone.ui
        dlgface.ui
    ../Cafe5/c5message.ui
    dlgguests.ui
    dlglist.ui
    dlglistdishspecial.ui
    dlgorder.ui
    dlgprecheckoptions.ui
    dlgrealreports.ui
    dlgreportparams.ui
    dlgreportsfilter.ui
    dlgscreen.ui
    dlgsearchinmenu.ui
    dlgsimleoptions.ui
    dlgsplitorder.ui
    dlgstafflist.ui
    dlgstoplistoption.ui
    dlgtables.ui
    dlgtext.ui
    dlgtimeorder.ui
    dlgviewstoplist.ui
    tablewidget.ui
    waiterdishwidget.ui
    dlgdishremovereason.ui
    dlgreports.ui
    dlgguest.ui
    ../Classes/rkeyboard.ui
    dlgreceiptlanguage.ui
    dlgcl.ui
    dlgexitwithmessage.ui

    wplast30sessions.ui
    ../../desktop.newTax/Src/printtaxn.cpp
)

# ---------- Qt link ----------
target_link_libraries(Waiter PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::PrintSupport
    Qt6::Multimedia
    Qt6::AxContainer
    Qt6::WebSockets
    Qt6::HttpServer
)

# ---------- OpenSSL ----------
target_include_directories(${PROJECT_NAME} PRIVATE
    C:/Development/OpenSSL-Win64/include
    C:/Development/OpenSSL-Win64/include/openssl
)


target_link_directories(${PROJECT_NAME} PRIVATE
    C:/Development/OpenSSL-Win64/lib/VC/x64/MD
)

target_link_libraries(Waiter PRIVATE
    libssl
    libcrypto
    wsock32
    Version
)

# ---------- Windows flags ----------
if(MSVC)
    target_compile_options(Waiter PRIVATE
        /Zc:strictStrings-
    )
endif()

add_custom_target(version_inc
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass
            -File ${CMAKE_SOURCE_DIR}/increase_version.ps1
)

add_dependencies(Waiter version_inc)

