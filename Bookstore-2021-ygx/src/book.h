//
// Created by 18163 on 2021/1/27.
//

#ifndef BOOKSTORE_2021_MAIN_BOOK_H
#define BOOKSTORE_2021_MAIN_BOOK_H
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
using namespace std;
class book{
public:
    char isbn[21] = "";
    char name[61] = "";
    char author[61] = "";
    char keyword[61] = "";
    double price = -1;

public:
    int quantity = 0;

    book() = default;

    explicit book(const string& ISBN , const string& NAME  = "", const string& AUTHOR = "", const string& KEY ="", double PRICE = 0);

    book(const book &other);

    void show_message(int priority) ;

    bool operator < (book other);

    bool operator ==(const book& other);
};

class book_system{
public:
    fstream scanner;

    void numberplus();

    int getnumber();
public:
    book_system() = default;

    ~book_system();

    int offset(const book &bk);

    void push_back(const book& ex);

    void show(const int &offset,int priority);

    void im(int offset,int quantity);

    void ex(int offset, int quantity);

    int get_quantity(int offset);

    double get_price(int offset);

    void open();

    void reopen();
};

class cash_msg{
public:
    char exim;//'+' or '-'
    double budget;

    cash_msg();

    cash_msg(char ex1, double ex2);

    void show() const;

    friend class cash_log;
};

class cash_log{
    fstream scanner;
public:
    void numberplus();

    int getnumber();

    cash_log() = default;

    ~cash_log();

    void push_back(cash_msg ex);

    void show(int time);

    void open();

    void reopen();
};

#endif //BOOKSTORE_2021_MAIN_BOOK_H
