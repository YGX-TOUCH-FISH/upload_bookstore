//
// Created by 18163 on 2021/1/30.
//

#ifndef BOOKSTORE_2021_MAIN_BOOK_AUTHOR_LIST_H
#define BOOKSTORE_2021_MAIN_BOOK_AUTHOR_LIST_H
#include <string>
#include <fstream>
#include "book.h"
using namespace std;
class author_element{
public:
    int offset = -1;

    char author[61]{'\0'};

    char isbn[21]{'\0'};

    author_element() = default;

    author_element(const string &AUTHOR, const string &ISBN , const int &OFFSET);

    friend class author_block;

    bool operator <(author_element other);

    author_element &operator=(const author_element &other);
};

class author_block{
    author_element array[2*320+4];
    int cur_size = 0;
    int prev = -1;
    int next = -1;

    void insert(const author_element &ele);

    int find_book(const string &author, const string &isbn);

    void excise(const string &AUTHOR, const string &ISBN);

public:
    author_block() = default;

    author_block(int PREV,int NEXT);

    friend class author_linklist;
};

class author_linklist{
    fstream scanner;

    void merge_block();

    void split_block();

    void numberplus();

    int getnumber();

    void real_show(const string &author,int offset , int priority);

public:
    author_linklist() = default;

    ~author_linklist();

    void add_book(const string &author,const string &isbn,int book_offset);

    int find_book(const string &author, const string &isbn);

    void excise_book(const string &author,const string &isbn);

    void show(const string &author , int priority);

    void open();

    void reopen();
};

#endif //BOOKSTORE_2021_MAIN_BOOK_AUTHOR_LIST_H
