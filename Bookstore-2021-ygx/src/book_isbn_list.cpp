//
// Created by 18163 on 2021/1/28.
//
#include "book_isbn_list.h"
isbn_element::isbn_element(const string &ISBN, const int &OFFSET) {
    for(int i = 0 ; i < sizeof(isbn) ; ++i)isbn[i] = ISBN[i];
    offset = OFFSET;
}

isbn_element &isbn_element::operator=(const isbn_element &other) {
    if (this == &other)return *this;
    for(int i = 0 ; i < sizeof(isbn) ; ++i)isbn[i] = other.isbn[i];
    offset = other.offset;
    return *this;
}

bool isbn_element::operator<(const isbn_element &other) {
    for(int i = 0 ; i < sizeof(isbn) ; ++i){
        if (isbn[i] < other.isbn[i])return true;
        if (isbn[i] > other.isbn[i])return false;
    }
    return false;
}

isbn_block::isbn_block(int PREV, int NEXT) {
    prev = PREV;
    next = NEXT;
}

void isbn_block::insert(const isbn_element &ele) {
    int cnt;
    for(cnt = 0 ; cnt < cur_size && array[cnt] < ele ; ++cnt);
    for(int i = cur_size-1 ; i >= cnt ; --i)
        array[i+1] = array[i];
    array[cnt] = ele;
    ++cur_size;
}

int isbn_block::find_book(const string &isbn) {
    for(int i = 0 ; i < cur_size ; ++i){
        if (array[i].isbn == isbn)return array[i].offset;
    }
    return -1;
}

void isbn_block::excise(const string &isbn) {
    int cnt;
    for(cnt = 0 ; array[cnt].isbn != isbn ; ++cnt);
    for(int i = cnt ; i < cur_size-1 ; ++i)
        array[i] = array[i+1];
    --cur_size;
}

isbn_linklist::~isbn_linklist() {
    scanner.close();
}

void isbn_linklist::add_book(const string &ISBN, int book_offset) {
    isbn_element x(ISBN,book_offset);
    isbn_block tmp;
    if (getnumber() == 0){
        numberplus();
        isbn_block a(-1,-1);
        a.insert(x);
        scanner.seekp(sizeof(int));
        scanner.write(reinterpret_cast<char *>(&a), sizeof(a));
        reopen();
        return;
    }
    int save = -1;
    int off = sizeof(int);
    while (off != -1){
        save = off;
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        if (ISBN < tmp.array[0].isbn){
            if (off == sizeof(int)){
                tmp.insert(x);
                scanner.seekp(off);
                scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
                reopen();
                split_block();
                return;
            }
            off = tmp.prev;
            scanner.seekg(off);
            scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            tmp.insert(x);
            scanner.seekp(off);
            scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            reopen();
            split_block();
            return;
        }
        off = tmp.next;
    }
    tmp.insert(x);
    scanner.seekp(save);
    scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
    reopen();
    split_block();
}

int isbn_linklist::find_book(const string &ISBN) {
    isbn_block tmp;
    if (getnumber() == 0)return -1;
    int off = sizeof(int);
    while (off != -1){
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        if (ISBN < tmp.array[0].isbn){
            if (off == sizeof(int))return -1;
            off = tmp.prev;
            scanner.seekg(off);
            scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            return tmp.find_book(ISBN);
        }
        off = tmp.next;
    }
    return tmp.find_book(ISBN);
}

void isbn_linklist::excise_book(const string &ISBN) {
    isbn_block tmp;
    int save = -1;
    int off = sizeof(int);
    while (off != -1){
        save = off;
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        if (ISBN < tmp.array[0].isbn){
            off = tmp.prev;
            scanner.seekg(off);
            scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            tmp.excise(ISBN);
            scanner.seekp(off);
            scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            reopen();
            merge_block();
            return;
        }
        off = tmp.next;
    }//外层特判：找不到这本书，不会excise
    tmp.excise(ISBN);
    scanner.seekp(save);
    scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
    reopen();
    merge_block();
}

void isbn_linklist::show_all(int priority) {
    isbn_block blk;
    fstream show("book");
    book bok;
    int offset;
    bool flag = false;
    int next = sizeof(int);
    while (next != -1){
        scanner.seekg(next);
        scanner.read(reinterpret_cast<char *>(&blk), sizeof(blk));
        for(int i = 0 ; i < blk.cur_size ; ++i){
            flag = true;
            offset = blk.array[i].offset;
            show.seekg(offset);
            show.read(reinterpret_cast<char *>(&bok), sizeof(bok));
            bok.show_message(priority);
        }
        next = blk.next;
    }
    show.close();
    if (!flag)cout<<endl;
}

void isbn_linklist::merge_block() {
    if (getnumber() == 0)return;
    int off = sizeof(int);
    isbn_block a, b, c;
    while (true){
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&a), sizeof(a));
        off = a.next;
        if (off == -1)break;
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&b), sizeof(b));
        if (a.cur_size + b.cur_size < 320){
            a.next = b.next;
            for(int i = a.cur_size ; i < a.cur_size + b.cur_size ; ++i)
                a.array[i] = b.array[i-a.cur_size];
            a.cur_size += b.cur_size;
            off = b.prev;
            scanner.seekp(off);
            scanner.write(reinterpret_cast<char *>(&a), sizeof(a));
            reopen();
            if (b.next != -1){
                scanner.seekg(b.next);
                scanner.read(reinterpret_cast<char *>(&c), sizeof(c));
                c.prev = b.prev;
                scanner.seekp(b.next);
                scanner.write(reinterpret_cast<char *>(&c), sizeof(c));
                reopen();
            }
            continue;
        }
    }
}

void isbn_linklist::split_block() {
    if (getnumber() == 0)return;
    int number;
    int off = sizeof(int);
    isbn_block a, b, c;
    while (off != -1){
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&a), sizeof(a));
        if (a.cur_size >= 2*320){
            int i;
            for(i = a.cur_size-1 ; i >= a.cur_size-320 ; --i)
                b.array[i-a.cur_size+320] = a.array[i];
            b.prev = off;
            b.cur_size = 320;
            b.next = a.next;
            scanner.seekp(0,ios::end);
            scanner.write(reinterpret_cast<char *>(&b), sizeof(b));
            reopen();
            numberplus();
            a.cur_size -=320;
            number = getnumber();
            a.next = sizeof(number) + (number-1)* sizeof(a);// b的地址
            scanner.seekp(b.prev);
            scanner.write(reinterpret_cast<char *>(&a), sizeof(a));
            reopen();
            if (b.next != -1){
                scanner.seekg(b.next);
                scanner.read(reinterpret_cast<char *>(&c), sizeof(c));
                c.prev = a.next;
                scanner.seekp(b.next);
                scanner.write(reinterpret_cast<char *>(&c), sizeof(c));
                reopen();
            }//修改后一个block的prev地址为新增block的地址
            off = b.next;//跳过分裂出的新block
            continue;
        }
        off = a.next;
    }
}

void isbn_linklist::numberplus() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    ++number;
    scanner.seekp(0);
    scanner.write(reinterpret_cast<char *>(&number), sizeof(number));
    reopen();
}

int isbn_linklist::getnumber() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    return number;
}

void isbn_linklist::open() {
    scanner.open("refer_isbn");
}

void isbn_linklist::reopen() {
    scanner.close();
    scanner.open("refer_isbn");
}