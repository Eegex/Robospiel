QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SUBDIRS += \
    Sounds

SOURCES += \
    UserBiddingWidget.cpp \
    board.cpp \
    boardeditor.cpp \
    buttondelegate.cpp \
    goalwidget.cpp \
    boardview.cpp \
    client.cpp \
    connectiontoclient.cpp \
    gamecontroll.cpp \
    keyinputblock.cpp \
    keyinputrecorder.cpp \
    keymapping.cpp \
    keymappingview.cpp \
    main.cpp \
    mainwidget.cpp \
    mainwindow.cpp \
    networkdebugger.cpp \
    networkmodel.cpp \
    onlineleaderboardwidget.cpp \
    offlineleaderboardwidget.cpp \
    pawnwidget.cpp \
    playerwidget.cpp \
    posknoten.cpp \
    powerbuttondelegate.cpp \
    server.cpp \
    settingsdialog.cpp \
    solver.cpp \
    spinbox.cpp \
    stackwidget.cpp \
    tablemodel.cpp \
    tile.cpp \
    user.cpp \
    usercreationwidget.cpp \
    votekickdelegate.cpp \
    zugknoten.cpp

HEADERS += \
    Direction.h \
    LeaderBoardWidget.h \
    UserBiddingWidget.h \
    UserCreationWidget.h \
    board.h \
    boardeditor.h \
    boardview.h \
    buttondelegate.h \
    client.h \
    connectiontoclient.h \
    direction.h \
    gamecontroll.h \
    goalwidget.h \
    keyinputblock.h \
    keyinputrecorder.h \
    keymapping.h \
    keymappingview.h \
    mainwidget.h \
    mainwindow.h \
    networkdebugger.h \
    networkmodel.h \
    onlineleaderboardwidget.h \
    offlineleaderboardwidget.h \
    pawnwidget.h \
    playerwidget.h \
    posknoten.h \
    powerbuttondelegate.h \
    server.h \
    settingsdialog.h \
    solver.h \
    spinbox.h \
    stackwidget.h \
    tablemodel.h \
    tile.h \
    boardview.h \
    direction.h \
    user.h \
    votekickdelegate.h \
    zugknoten.h

TRANSLATIONS += \
Robospiel_de_DE.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	Sounds/count.mp3 \
	Sounds/rick.mp3
