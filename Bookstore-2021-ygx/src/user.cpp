//
// Created by 18163 on 2021/1/27.
//
#include "user.h"
user::user() {
    memset(user_id,'\0', sizeof(user_id));
    memset(name,'\0', sizeof(name));
    memset(passwd,'\0', sizeof(passwd));
    select = 0;
    priority = 0;
}

user::user(const string &USER_ID, const string &PASSWD, const int &PRIOR, const string &NAME, const int &SELECT) {
    for(int i = 0 ; i < sizeof(user_id) ; ++i)user_id[i] = USER_ID[i];
    for(int i = 0 ; i < sizeof(passwd) ;  ++i)passwd[i] = PASSWD[i];
    priority = PRIOR;
    for(int i = 0 ; i < sizeof(name) ; ++i)name[i] = NAME[i];
    select = SELECT;
}

user::user(const user &other) {
    if (this == &other)return;
    for(int i = 0 ; i < sizeof(user_id); ++i)user_id[i] = other.user_id[i];
    for(int i = 0 ; i < sizeof(passwd) ; ++i)passwd[i] = other.passwd[i];
    priority = other.priority;
    for(int i = 0 ; i < sizeof(name) ; ++i)name[i] = other.name[i];
    select = other.select;
}

user &user::operator=(const user &other) {
    if (this == &other)return *this;
    for(int i = 0 ; i < sizeof(user_id); ++i)user_id[i] = other.user_id[i];
    for(int i = 0 ; i < sizeof(passwd) ; ++i)passwd[i] = other.passwd[i];
    priority = other.priority;
    for(int i = 0 ; i < sizeof(name) ; ++i)name[i] = other.name[i];
    select = other.select;
    return *this;
}

user_system::~user_system(){
    scanner.close();
}

int user_system::offset(const string &USER_ID) {
    int pointer = sizeof(int);
    user a;
    scanner.seekg(pointer);
    while (pointer != getnumber()* sizeof(a)+ sizeof(int)){
        scanner.seekg(pointer);
        scanner.read(reinterpret_cast<char *>(&a), sizeof(a));
        if (a.user_id == USER_ID) {
            return pointer;
        }
        pointer += sizeof(a);
    }
    return -1;
}

void user_system::push_back(const user& ex) {
    reopen();
    user use(ex);//拷贝初始化
    scanner.seekp(0,ios::end);
    scanner.write(reinterpret_cast<char *>(&use), sizeof(use));
    numberplus();
}

void user_system::numberplus() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    ++number;
    scanner.seekp(0);
    scanner.write(reinterpret_cast<char *>(&number), sizeof(number));
    reopen();
}

int user_system::getnumber() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    return number;
}

user user_system::get_user(const int &offset) {
    user answer;
    scanner.seekg(offset);
    scanner.read(reinterpret_cast<char *>(&answer), sizeof(answer));
    return answer;
}

void user_system::change_passwd(const int &offset, const string &new_passwd) {
    user a;
    scanner.seekg(offset);
    scanner.read(reinterpret_cast<char *>(&a), sizeof(a));
    for(int i = 0 ; i < sizeof(a.passwd) ; ++i)a.passwd[i] = new_passwd[i];
    scanner.seekp(offset);
    scanner.write(reinterpret_cast<char *>(&a), sizeof(a));
    reopen();
}

void user_system::open() {
    scanner.open("user");
}

void user_system::reopen() {
    scanner.close();
    scanner.open("user");
}