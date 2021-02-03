QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    UserBiddingWidget.cpp \
    board.cpp \
    boardeditor.cpp \
    goalwidget.cpp \
    leaderboardwidget.cpp \
    boardview.cpp \
    client.cpp \
    connectiontoclient.cpp \
    gamecontroll.cpp \
    keyinput.cpp \
    keymapping.cpp \
    keymappingview.cpp \
    main.cpp \
    mainwidget.cpp \
    mainwindow.cpp \
    offlineleaderboardwidget.cpp \
    pawnwidget.cpp \
    networkview.cpp \
    playerwidget.cpp \
    server.cpp \
    settingsdialog.cpp \
    tile.cpp \
    user.cpp \
    usercreationwidget.cpp \
    useronlinewidget.cpp

HEADERS += \
    Direction.h \
    LeaderBoardWidget.h \
    UserBiddingWidget.h \
    UserCreationWidget.h \
    board.h \
    boardeditor.h \
    boardview.h \
    client.h \
    connectiontoclient.h \
    direction.h \
    gamecontroll.h \
    goalwidget.h \
    keyinput.h \
    keymapping.h \
    keymappingview.h \
    mainwidget.h \
    mainwindow.h \
    networkview.h \
    offlineleaderboardwidget.h \
    pawnwidget.h \
    playerwidget.h \
    server.h \
    settingsdialog.h \
    tile.h \
    boardview.h \
    direction.h \
    user.h \
    useronlinewidget.h

TRANSLATIONS += \
Robospiel_de_DE.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
