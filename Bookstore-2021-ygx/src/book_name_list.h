//
// Created by 18163 on 2021/1/29.
//

#ifndef BOOKSTORE_2021_MAIN_BOOK_NAME_LIST_H
#define BOOKSTORE_2021_MAIN_BOOK_NAME_LIST_H
#include <string>
#include <fstream>
#include "book.h"
using namespace std;
class name_element{
public:
    int offset = -1;

    char name[61]{'\0'};

    char isbn[21]{'\0'};

    name_element() = default;

    name_element(const string &NAME, const string &ISBN , const int &OFFSET);

    friend class name_block;

    bool operator<(name_element other);

    name_element &operator=(const name_element &other);
};

class name_block{
    name_element array[2*320+4];
    int cur_size = 0;
    int prev = -1;
    int next = -1;

    void insert(const name_element &ele);

    int find_book(const string &name, const string &isbn);

    void excise(const string &ISBN);

public:
    name_block() = default;

    name_block(int PREV, int NEXT);

    friend class name_linklist;
};

class name_linklist{
    fstream scanner;

    void merge_block();

    void split_block();

    void numberplus();

    int getnumber();

    void real_show(const string &name, int offset , int priority);

public:
    name_linklist() = default;

    ~name_linklist();

    void add_book(const string &name,const string &isbn, int book_offset);

    int find_book(const string &name, const string &isbn);

    void excise_book(const string &name , const string &isbn);

    void show(const string &name,int priority);

    void open();

    void reopen();
};
#endif //BOOKSTORE_2021_MAIN_BOOK_NAME_LIST_H
