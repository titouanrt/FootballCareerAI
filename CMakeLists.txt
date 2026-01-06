cmake_minimum_required(VERSION 3.16)

project(FootballManagerGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Network REQUIRED)


add_executable(FootballManagerGame
    main.cpp
    mainwindow.cpp mainwindow.h
    DataManager.cpp DataManager.h  
    PlayerCard.cpp PlayerCard.h     
    GameUtils.h
    Player.cpp Player.h
    Club.cpp Club.h
    Championnat.cpp Championnat.h
    Match.cpp Match.h
    Saison.cpp Saison.h
    Profil.cpp Profil.h
    MercatoView.cpp MercatoView.h   
    SaveManager.cpp SaveManager.h
)


target_link_libraries(FootballManagerGame PRIVATE Qt6::Widgets Qt6::Network)

file(COPY ${CMAKE_SOURCE_DIR}/EAFC26-Men.csv DESTINATION ${CMAKE_BINARY_DIR})