//
// Created by 18163 on 2021/1/27.
//
#include "book.h"
book::book(const string& ISBN, const string& NAME, const string& AUTHOR, const string& KEY, double PRICE) {
    for(int i = 0 ; i < sizeof(isbn) ; ++i)isbn[i] = ISBN[i];
    for(int i = 0 ; i < sizeof(name) ; ++i)name[i] = NAME[i];
    for(int i = 0 ; i < sizeof(author) ; ++i)author[i] = AUTHOR[i];
    for(int i = 0 ; i < sizeof(keyword) ; ++i)keyword[i] = KEY[i];
    price = PRICE;
}

book::book(const book &other) {
    for(int i = 0 ; i < sizeof(isbn) ; ++i)isbn[i] = other.isbn[i];
    for(int i = 0 ; i < sizeof(name) ; ++i)name[i] = other.name[i];
    for(int i = 0 ; i < sizeof(author) ; ++i)author[i] = other.author[i];
    for(int i = 0 ; i < sizeof(keyword) ; ++i)keyword[i] = other.keyword[i];
    price = other.price;
}

void book::show_message(int priority) {
    if (priority >= 1){
        cout<<isbn<<'\t'<<name<<'\t'<<author<<'\t'<<keyword<<'\t';
        if (price < 0 && price > -0.01){
            double Price = -price;
            cout<<fixed<<setprecision(2)<<Price<<'\t';
        } else{
            cout<<fixed<<setprecision(2)<<price<<'\t';
        }
        cout<<quantity<<endl;
        return;
    }
    if (priority < 1){
        cout<<isbn<<'\t'<<name<<'\t'<<author<<'\t'<<keyword<<'\t';
        if (price < 0 && price > -0.01){
            double Price = -price;
            cout<<fixed<<setprecision(2)<<Price<<'\t';
        } else{
            cout<<fixed<<setprecision(2)<<price<<'\t';
        }
        return;
    }
}


bool book::operator<(book other) {
    if (strlen(isbn) < strlen(other.isbn))return true;
    if (strlen(isbn) > strlen(other.isbn))return false;
    for(int i = 0 ; i < strlen(isbn) ; ++i){
        if (isbn[i] < other.isbn[i])return true;
        if (isbn[i] > other.isbn[i])return false;
    }
    return false;
}

bool book::operator==(const book &other) {
    if (this == &other)return true;
    for(int i = 0 ; i < sizeof(isbn) ; ++i){
        if (isbn[i] != other.isbn[i])return false;
    }
    for(int i = 0 ; i < sizeof(name) ; ++i){
        if (name[i] != other.name[i])return false;
    }
    for(int i = 0 ; i < sizeof(author) ; ++i){
        if (author[i] != other.author[i])return false;
    }
    for(int i = 0 ; i < sizeof(keyword) ; ++i){
        if (keyword[i] != other.keyword[i])return false;
    }
    if (price != other.price)return false;
    return true;
}

book_system::~book_system() {
    scanner.close();
}

int book_system::offset(const book &bk) {
    int pointer = sizeof(int);
    book a;
    while (pointer != getnumber()* sizeof(a) + sizeof(int)){
        scanner.seekg(pointer);
        scanner.read(reinterpret_cast<char *>(&a), sizeof(a));
        if (a == bk)return pointer;
        pointer += sizeof(a);
    }
    return -1;
}

void book_system::push_back(const book& ex) {
    reopen();
    book bk(ex);//拷贝初始化
    scanner.seekp(0,ios::end);
    scanner.write(reinterpret_cast<char *>(&bk), sizeof(bk));
    if (scanner.fail()){
        cerr<<"book_system fail"<<endl;
    }
    numberplus();
}

void book_system::show(const int &offset, int priority) {
    book a;
    scanner.seekg(offset);
    scanner.read(reinterpret_cast<char *>(&a), sizeof(a));
    a.show_message(priority);
}

void book_system::numberplus() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    ++number;
    scanner.seekp(0);
    scanner.write(reinterpret_cast<char *>(&number), sizeof(number));
    reopen();
}

int book_system::getnumber() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    return number;
}

void book_system::im(int offset, int quantity) {
    book bk;
    scanner.seekg(offset);
    scanner.read(reinterpret_cast<char *>(&bk), sizeof(bk));
    bk.quantity += quantity;
    scanner.seekp(offset);
    scanner.write(reinterpret_cast<char *>(&bk), sizeof(bk));
    if (scanner.fail()){
        cerr<<"im fail"<<endl;
    }
    reopen();
}

void book_system::ex(int offset, int quantity) {
    book bk;
    scanner.seekg(offset);
    scanner.read(reinterpret_cast<char *>(&bk), sizeof(bk));
    bk.quantity -= quantity;
    scanner.seekp(offset);
    scanner.write(reinterpret_cast<char *>(&bk), sizeof(bk));
    if (scanner.fail()){
        cerr<<"ex fail"<<endl;
    }
    reopen();
}

int book_system::get_quantity(int offset) {
    book bk;
    scanner.seekg(offset);
    scanner.read(reinterpret_cast<char *>(&bk), sizeof(bk));
    return bk.quantity;
}

double book_system::get_price(int offset) {
    book bk;
    scanner.seekg(offset);
    scanner.read(reinterpret_cast<char *>(&bk), sizeof(bk));
    return bk.price;
}

void book_system::open() {
    scanner.open("book");
}

void book_system::reopen() {
    scanner.close();
    scanner.open("book");
}

cash_msg::cash_msg() {
    exim = '+';
    budget = 0;
}

cash_msg::cash_msg(char ex1, double ex2) {
    exim = ex1;
    budget = ex2;
}

void cash_msg::show() const {
    cout<<exim<<' '<<fixed<<setprecision(2)<<budget;
}

cash_log::~cash_log() {
    scanner.close();
}

void cash_log::numberplus() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    ++number;
    scanner.seekp(0);
    scanner.write(reinterpret_cast<char *>(&number), sizeof(number));
    reopen();
}

int cash_log::getnumber() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    return number;
}

void cash_log::push_back(cash_msg ex) {
    reopen();
    cash_msg a(ex);
    scanner.seekp(0,ios::end);
    scanner.write(reinterpret_cast<char *>(&a), sizeof(a));
    if (scanner.fail()){
        cout<<"cash_log fail"<<endl;
    }
    numberplus();
}

void cash_log::show(int time) {
    double positive = 0, negative = 0;
    cash_msg r;
    int pointer;
    if (time == -1){
        pointer = sizeof(int);
        while (pointer != getnumber()*sizeof(cash_msg)+sizeof(int)){
            scanner.seekg(pointer);
            scanner.read(reinterpret_cast<char *>(&r), sizeof(r));
            pointer += sizeof(cash_msg);
            if (r.exim == '+'){
                positive += r.budget;
                continue;
            }
            if (r.exim == '-'){
                negative += r.budget;
                continue;
            }
        }
    }
    else{
        pointer = (getnumber()-time)*sizeof(cash_msg)+sizeof(int);
        while (pointer != getnumber()*sizeof(cash_msg)+sizeof(int)){
            scanner.seekg(pointer);
            scanner.read(reinterpret_cast<char *>(&r), sizeof(r));
            pointer += sizeof(cash_msg);
            if (r.exim == '+'){
                positive += r.budget;
                continue;
            }
            if (r.exim == '-'){
                negative += r.budget;
                continue;
            }
        }
    }
    cash_msg p('+',positive);
    cash_msg n('-',negative);
    p.show();
    cout<<" ";
    n.show();
    cout<<endl;
}//time == 0 :show all message

void cash_log::open() {
    scanner.open("financial_log");
}

void cash_log::reopen() {
    scanner.close();
    scanner.open("financial_log");
}