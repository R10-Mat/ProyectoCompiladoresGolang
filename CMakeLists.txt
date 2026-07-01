cmake_minimum_required(VERSION 3.31)
project(ProyectoCompiladoresC_)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProyectoCompiladoresC_ main.cpp
        parser.cpp
        TypeCheker.cpp
        TypeCheker.h
        GenCode.cpp
        GenCode.h
        Enviroment.h
        token.cpp
        token.h
        Semantic_types.cpp
        ast.cpp
        ast.h
        scanner.cpp
        scanner.h
        Semantic_types.h
        visitor.h
        visitor.cpp)
