//
// Created by 18163 on 2021/1/27.
//

#ifndef BOOKSTORE_2021_MAIN_USER_H
#define BOOKSTORE_2021_MAIN_USER_H
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
using namespace std;
class user{
public:
    char user_id[31] = {'\0'};
    char name[31] = {'\0'};

public:
    char passwd[31] = {'\0'};
    int select = 0;
    int priority = 0;

    user();

    user(const string &USER_ID , const string &PASSWD , const int &PRIOR , const string &NAME , const int &SELECT);

    user(const user &other);

    user &operator=(const user &other);

    friend class user_system;
};

class user_system{
public:
    fstream scanner;

    void numberplus();

    int getnumber();

public:
    user_system() = default;

    ~user_system();

    int offset(const string &USER_ID);

    void push_back(const user& ex);

    user get_user(const int &offset);

    void change_passwd(const int &offset,const string &new_passwd);

    void open();

    void reopen();
};
#endif //BOOKSTORE_2021_MAIN_USER_H
