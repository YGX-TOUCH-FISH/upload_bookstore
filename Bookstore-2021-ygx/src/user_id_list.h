//
// Created by 18163 on 2021/1/27.
//

#ifndef BOOKSTORE_2021_MAIN_USER_ID_LIST_H
#define BOOKSTORE_2021_MAIN_USER_ID_LIST_H
#include <iostream>
#include <fstream>
#include <string>
#include "user.h"
using namespace std;
class id_element{
public:
    char user_id[31]{'\0'};

    int offset = -1;
public:
    id_element() = default;

    id_element(const string &USER_ID, int OFFSET);

    id_element &operator=(const id_element &other);

    bool operator<(const id_element &other);

    friend class id_block;

};

class id_block{
    id_element array[2*320+4]{};
    int cur_size = 0;
    int prev = -1;
    int next = -1;
    void insert(const id_element &ele);

    int find_user(const string &id);

    void excise(const string &id);
public:
    id_block() = default;

    id_block(int PREV, int NEXT);

    friend class id_linklist;
};

class id_linklist{
    fstream scanner;
    //同时读写，一定要调seekp/seekg
    void merge_block();

    void split_block();

    void numberplus();

    int getnumber();

public:
    id_linklist() = default;

    ~id_linklist();

    void add_user(const string &id, int user_offset);

    int find_user(const string &id);

    void excise_user(const string &id);

    void show_all();

    void show_connect();

    void open();

    void reopen();
};
#endif //BOOKSTORE_2021_MAIN_USER_ID_LIST_H
