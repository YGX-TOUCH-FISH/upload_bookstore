//
// Created by 18163 on 2021/2/1.
//

#ifndef BOOKSTORE_2021_MAIN_BOOK_KEY_LIST_H
#define BOOKSTORE_2021_MAIN_BOOK_KEY_LIST_H
#include <string>
#include <fstream>
#include "book.h"
using namespace std;
class key_element{
public:
    int offset = -1;

    char keywd[61]{'\0'};

    char isbn[21]{'\0'};

    key_element() = default;

    key_element(const string &KEYWD , const string &ISBN , const int &OFFSET);

    friend class key_block;

    bool operator<(key_element other);

    key_element &operator=(const key_element &other);
};

class key_block{
    key_element array[2*320+4];
    int cur_size = 0;
    int prev = -1;
    int next = -1;

    void insert(const key_element &ele);

    int find_book(const string &keywd, const string &isbn);

    void excise(const string &KEYWD, const string &ISBN);

public:
    key_block() = default;

    key_block(int PREV,int NEXT);

    friend class key_linklist;
};

class key_linklist{
    fstream scanner;

    void merge_block();

    void split_block();

    void numberplus();

    int getnumber();

    void real_show(const string &keywd, int offset , int priority);

public:
    key_linklist() = default;

    ~key_linklist();

    void add_book(const string &keywd,const string &isbn , int book_offset);

    int find_book(const string &keywd, const string &isbn);

    void excise_book(const string &keywd,const string &isbn);

    void show(const string &keywd,int priority);

    void open();

    void reopen();
};
#endif //BOOKSTORE_2021_MAIN_BOOK_KEY_LIST_H
