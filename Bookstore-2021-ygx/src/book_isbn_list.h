//
// Created by 18163 on 2021/1/28.
//

#ifndef BOOKSTORE_2021_MAIN_BOOK_ISBN_LIST_H
#define BOOKSTORE_2021_MAIN_BOOK_ISBN_LIST_H
#include <string>
#include <fstream>
#include "book.h"
using namespace std;
class isbn_element{
public:
    int offset = -1;

    char isbn[21]{'\0'};

    isbn_element() = default;

    isbn_element(const string &ISBN, const int &OFFSET);

    isbn_element &operator=(const isbn_element &other);

    bool operator<(const isbn_element &other);

    friend class isbn_block;
};

class isbn_block{
    isbn_element array[2*320+4];
    int cur_size = 0;
    int prev = -1;
    int next = -1;

    void insert(const isbn_element &ele);

    int find_book(const string &isbn);

    void excise(const string &isbn);

public:
    isbn_block() = default;

    isbn_block(int PREV, int NEXT);

    friend class isbn_linklist;
};

class isbn_linklist{
    fstream scanner;

    void merge_block();

    void split_block();

    void numberplus();

    int getnumber();

public:
    isbn_linklist() = default;

    ~isbn_linklist();

    void add_book(const string &ISBN, int book_offset);

    int find_book(const string &ISBN);

    void excise_book(const string &ISBN);

    void show_all(int priority);

    void open();

    void reopen();
};
#endif //BOOKSTORE_2021_MAIN_BOOK_ISBN_LIST_H
