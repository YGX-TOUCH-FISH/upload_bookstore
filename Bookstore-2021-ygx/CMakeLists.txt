cmake_minimum_required(VERSION 3.10.2)
project(code)

set(CMAKE_CXX_STANDARD 14)

add_executable(code
        src/main.cpp src/book.h src/book.cpp src/user.h src/user.cpp src/user_id_list.h src/user_id_list.cpp src/book_isbn_list.h src/book_isbn_list.cpp src/book_name_list.h src/book_name_list.cpp src/book_author_list.h src/book_author_list.cpp src/book_key_list.h src/book_key_list.cpp)
