//
// Created by 18163 on 2021/2/1.
//
#include "book_key_list.h"
key_element::key_element(const string &KEYWD, const string &ISBN, const int &OFFSET) {
    for(int i = 0 ; i < sizeof(keywd) ; ++i)keywd[i] = KEYWD[i];
    for(int i = 0 ; i < sizeof(isbn) ; ++i)isbn[i] = ISBN[i];
    offset = OFFSET;
}

bool key_element::operator<(key_element other) {
    string keyw = keywd;
    string okeyw = other.keywd;
    string isb = isbn;
    string oisb= other.isbn;
    if (keyw < okeyw)return true;
    if (keyw > okeyw)return false;
    if (isb  < oisb)return true;
    return false;
}

key_element &key_element::operator=(const key_element &other) {
    if (this == &other)return *this;
    for(int i = 0 ; i < sizeof(keywd) ; ++i)keywd[i] = other.keywd[i];
    for(int i = 0 ; i < sizeof(isbn) ; ++i)isbn[i] = other.isbn[i];
    offset = other.offset;
    return *this;
}

key_block::key_block(int PREV, int NEXT) {
    prev = PREV;
    next = NEXT;
}

void key_block::insert(const key_element &ele) {
    int cnt;
    for(cnt = 0 ; cnt < cur_size && array[cnt] < ele ; ++cnt);
    for(int i = cur_size-1; i >= cnt ; --i)
        array[i+1] = array[i];
    array[cnt] = ele;
    ++cur_size;
}

int key_block::find_book(const string &keywd, const string &isbn) {
    for(int i = 0 ; i < cur_size ; ++i){
        if (array[i].keywd == keywd && array[i].isbn == isbn)return array[i].offset;
    }
    return -1;
}

void key_block::excise(const string &KEYWD, const string &ISBN) {
    int cnt ;
    for(cnt = 0 ; array[cnt].keywd != KEYWD || array[cnt].isbn != ISBN ; ++cnt);
    for(int i = cnt;  i < cur_size-1 ; ++i)
        array[i] = array[i+1];
    --cur_size;
}

key_linklist::~key_linklist() {
    scanner.close();
}

void key_linklist::add_book(const string &keywd, const string &isbn, int book_offset) {
    key_element x(keywd,isbn,book_offset);
    key_block tmp;
    if (getnumber() == 0){
        numberplus();
        key_block a(-1,-1);
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

int key_linklist::find_book(const string &keywd, const string &isbn) {
    key_block tmp;
    key_element x(keywd,isbn,-1);
    if (getnumber() == 0)return -1;
    int off = sizeof(int);
    while (off != -1){
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        if (x < tmp.array[0]){
            if (off == sizeof(int))return -1;
            off = tmp.prev;
            scanner.seekg(off);
            scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            return tmp.find_book(keywd,isbn);
        }
        off = tmp.next;
    }
    return tmp.find_book(keywd,isbn);
}

void key_linklist::excise_book(const string &keywd, const string &isbn) {
    key_element x(keywd,isbn,-1);
    key_block tmp;
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
            tmp.excise(keywd,isbn);
            scanner.seekp(off);
            scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            reopen();
            merge_block();
            return;
        }
        off = tmp.next;
    }
    tmp.excise(keywd,isbn);
    scanner.seekp(save);
    scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
    reopen();
    merge_block();
}

void key_linklist::merge_block() {
    if(getnumber() == 0)return;
    int off = sizeof(int);
    key_block a, b ,c;
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

void key_linklist::split_block() {
    if (getnumber() == 0)return;
    int number;
    int off = sizeof(int);
    key_block a, b, c;
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

void key_linklist::show(const string &keywd, int priority) {
    key_block blk;
    int save = -1;
    int offset = sizeof(int);
    while (offset != -1){
        save = offset;
        scanner.seekg(offset);
        scanner.read(reinterpret_cast<char *>(&blk), sizeof(blk));
        if (keywd <= blk.array[0].keywd){
            if (keywd <  blk.array[0].keywd){
                if (offset == sizeof(int)){
                    cout<<endl;
                    return;
                }
                save = blk.prev;
                real_show(keywd,save,priority);
                return;
            }
            if (keywd == blk.array[0].keywd){
                if (offset == sizeof(int)){
                    save = offset;
                    real_show(keywd,save,priority);
                    return;
                }
                save = blk.prev;
                real_show(keywd,save,priority);
                return;
            }
        }
        offset = blk.next;
    }
    real_show(keywd,save,priority);
}

void key_linklist::real_show(const string &keywd, int offset, int priority) {
    bool flag = false;
    key_block blk;
    fstream bookshow("book");
    book bk;
    int off;
    int pointer = offset;
    while (pointer != -1){
        scanner.seekg(pointer);
        scanner.read(reinterpret_cast<char *>(&blk), sizeof(blk));
        if (keywd < blk.array[0].keywd){
            bookshow.close();
            break;
        }
        for(int i = 0 ; i < blk.cur_size ; ++i){
            if (keywd == blk.array[i].keywd){
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

void key_linklist::numberplus() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    ++number;
    scanner.seekp(0);
    scanner.write(reinterpret_cast<char *>(&number), sizeof(number));
    reopen();
}

int key_linklist::getnumber() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    return number;
}

void key_linklist::open() {
    scanner.open("refer_keyword");
}

void key_linklist::reopen() {
    scanner.close();
    scanner.open("refer_keyword");
}