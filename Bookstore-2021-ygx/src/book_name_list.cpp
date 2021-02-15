//
// Created by 18163 on 2021/1/29.
//
#include "book_name_list.h"
name_element::name_element(const string &NAME, const string &ISBN, const int &OFFSET) {
    for(int i = 0 ; i < sizeof(name) ; ++i)name[i] = NAME[i];
    for(int i = 0 ; i < sizeof(isbn) ; ++i)isbn[i] = ISBN[i];
    offset = OFFSET;
}

bool name_element::operator<(name_element other) {
    string nam = name;
    string onam = other.name;
    string isb = isbn;
    string oisb = other.isbn;
    if (nam < onam)return true;
    if (nam > onam)return false;
    if (isb < oisb)return true;
    return false;
}

name_element &name_element::operator=(const name_element &other) {
    if (this == &other)return *this;
    for(int i = 0 ; i < sizeof(name) ; ++i)name[i] = other.name[i];
    for(int i = 0 ; i < sizeof(isbn) ; ++i)isbn[i] = other.isbn[i];
    offset = other.offset;
    return *this;
}

name_block::name_block(int PREV, int NEXT) {
    prev = PREV;
    next = NEXT;
}

void name_block::insert(const name_element &ele) {
    int cnt;
    for(cnt = 0 ; cnt < cur_size && array[cnt] < ele ; ++cnt);
    for(int i = cur_size-1; i >= cnt ; --i)
        array[i+1] = array[i];
    array[cnt] = ele;
    ++cur_size;
}

int name_block::find_book(const string &name, const string &isbn) {
    for(int i = 0 ; i < cur_size ; ++i){
        if (array[i].isbn == isbn && array[i].name == name)return array[i].offset;
    }
    return -1;
}

void name_block::excise(const string &ISBN) {
    int cnt;
    for(cnt = 0 ; array[cnt].isbn != ISBN ; ++cnt);
    for(int i = cnt ; i < cur_size-1 ; ++i)
        array[i] = array[i+1];
    --cur_size;
}

name_linklist::~name_linklist() {
    scanner.close();
}

void name_linklist::add_book(const string &name,const string &isbn, int book_offset) {
    name_element x(name,isbn,book_offset);
    name_block tmp;
    if (getnumber() == 0){
        numberplus();
        name_block a(-1,-1);
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
        if (x < tmp.array[0]){
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
            scanner.write(reinterpret_cast<char *>(&tmp),sizeof(tmp));
            reopen();
            split_block();
            return;
        }
        off = tmp.next;
    }
    tmp.insert(x);
    scanner.seekp(save);
    scanner.write(reinterpret_cast<char *>(&tmp),sizeof(tmp));
    reopen();
    split_block();
}

int name_linklist::find_book(const string &name, const string &isbn) {
    name_block tmp;
    name_element x(name,isbn,-1);
    if (getnumber() == 0)return -1;
    int off = sizeof(int);
    while (off != -1){
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp),sizeof(tmp));
        if (x < tmp.array[0]){
            if (off == sizeof(int))return -1;
            off = tmp.prev;
            scanner.seekg(off);
            scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            return tmp.find_book(name,isbn);
        }
        off = tmp.next;
    }
    return tmp.find_book(name,isbn);
}

void name_linklist::excise_book(const string &name , const string &isbn) {
    name_element x(name,isbn,-1);
    name_block tmp;
    int save = -1;
    int off = sizeof(int);
    while (off != -1){
        save = off;
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        if (x < tmp.array[0]){
            off = tmp.prev;
            scanner.seekg(off);
            scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            tmp.excise(isbn);
            scanner.seekp(off);
            scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            reopen();
            merge_block();
            return;
        }
        off = tmp.next;
    }
    tmp.excise(isbn);
    scanner.seekp(save);
    scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
    reopen();
    merge_block();
}

void name_linklist::merge_block() {
    if (getnumber() == 0)return;
    int off = sizeof(int);
    name_block a, b, c;
    while (true){
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&a), sizeof(a));
        off = a.next;
        if (off == -1)break;//没有可以merge的block了
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&b), sizeof(b));
        if (a.cur_size + b.cur_size < 320){
            a.next = b.next;
            for(int i = a.cur_size ; i < a.cur_size + b .cur_size ; ++i)
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

void name_linklist::split_block() {
    if (getnumber() == 0)return;
    int number;
    int off = sizeof(int);
    name_block a, b, c;
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
            a.cur_size -= 320;
            scanner.seekp(0,ios::end);
            scanner.write(reinterpret_cast<char *>(&b), sizeof(b));
            reopen();
            numberplus();
            number = getnumber();
            a.next = sizeof(int) + (number-1)*sizeof(name_block);
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
            off = b.next;//跳过新分裂的block b
            continue;
        }
        off = a.next;
    }
}

void name_linklist::show(const string &name,int priority) {
    name_block blk;
    int offset = sizeof(int);
    int save;
    while (offset != -1){
        save = offset;
        scanner.seekg(offset);
        scanner.read(reinterpret_cast<char *>(&blk), sizeof(blk));
        if (name <= blk.array[0].name){
            if (name <  blk.array[0].name){
                if (offset == sizeof(int)){
                    cout<<endl;
                    return;
                }
                save = blk.prev;
                real_show(name,save,priority);
                return;
            }
            if (name == blk.array[0].name){
                if (offset == sizeof(int)){
                    save = offset;
                    real_show(name,save,priority);
                    return;
                }
                save = blk.prev;
                real_show(name,save,priority);
                return;
            }
        }
        offset = blk.next;
    }
    //直到最后一块block，始终name>head，在最后一块block中搜索
    real_show(name,save,priority);
}

void name_linklist::real_show(const string &name, int offset, int priority) {
    bool flag = false;
    name_block blk;
    fstream bookshow("book");
    book bk;
    int off;
    int pointer = offset;
    while (pointer != -1){
        scanner.seekg(pointer);
        scanner.read(reinterpret_cast<char *>(&blk), sizeof(blk));
        if (name < blk.array[0].name){
            bookshow.close();
            break;
        }
        for(int i = 0 ; i < blk.cur_size ; ++i){
            if (name == blk.array[i].name){
                flag = true;
                off = blk.array[i].offset;
                bookshow.seekg(off);
                bookshow.read(reinterpret_cast<char *>(&bk), sizeof(bk));
                bk.show_message(priority);
            }
        }
        pointer = blk.next;
    }
    if (!flag)cout<<endl;
    bookshow.close();
}

void name_linklist::numberplus() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    ++number;
    scanner.seekp(0);
    scanner.write(reinterpret_cast<char *>(&number), sizeof(number));
    reopen();
}

int name_linklist::getnumber() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    return number;
}

void name_linklist::open() {
    scanner.open("refer_name");
}

void name_linklist::reopen() {
    scanner.close();
    scanner.open("refer_name");
}