//
// Created by 18163 on 2021/1/27.
//
#include "user_id_list.h"
id_element::id_element(const string &USER_ID, int OFFSET) {
    for(int i = 0 ; i < sizeof(user_id) ; ++i)user_id[i] = USER_ID[i];
    offset = OFFSET;
}

id_element &id_element::operator=(const id_element &other) {
    if (this == &other)return *this;
    for(int i = 0 ; i < sizeof(user_id) ; ++i)user_id[i] = other.user_id[i];
    offset = other.offset;
    return *this;
}

bool id_element::operator<(const id_element &other) {
    for(int i = 0 ; i < sizeof(user_id) ; ++i){
        if (user_id[i] < other.user_id[i])return true;
        if (user_id[i] > other.user_id[i])return false;
    }
    return false;
}

id_block::id_block(int PREV, int NEXT) {
    prev = PREV;
    next = NEXT;
}

void id_block::insert(const id_element &ele) {
    int cnt;
    for(cnt = 0 ; cnt < cur_size && array[cnt] < ele ; ++cnt) ;
    for(int i = cur_size-1 ; i >= cnt ; --i)
        array[i+1] = array[i];
    array[cnt] = ele;
    ++cur_size;
}

int id_block::find_user(const string &id) {
    for(int i = 0 ; i < cur_size ; ++i){
        if (array[i].user_id == id)return array[i].offset;
    }
    return -1;
}

void id_block::excise(const string &id) {
    int cnt ;
    for(cnt = 0 ; array[cnt].user_id != id; ++cnt);
    for(int i = cnt ; i < cur_size-1 ; ++i)
        array[i] = array[i+1];
    --cur_size;
}

id_linklist::~id_linklist() {
    scanner.close();
}

void id_linklist::add_user(const string &id, int user_offset) {
    id_element x(id,user_offset);
    id_block tmp;
    if (getnumber() == 0){
        numberplus();
        id_block A(-1 , -1);
        A.insert(x);
        scanner.seekp(sizeof(int));
        scanner.write(reinterpret_cast<char *>(&A),sizeof(A));
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
            }//插入到头部 (tmp.prev == -1)
            off = tmp.prev;
            scanner.seekg(off);//插入到上一个
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
    tmp.insert(x);//插入到尾部
    scanner.seekp(save);
    scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
    reopen();
    split_block();
}

int id_linklist::find_user(const string &id) {
    id_block tmp;
    if (getnumber() == 0)return -1;
    int off = sizeof(int);
    while (off != -1){
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        string userid = tmp.array[0].user_id;
            //cout<<"head_id::"<<userid<<endl;
        if (id < userid){
            if (off == sizeof(int))return -1;
            off = tmp.prev;
            scanner.seekg(off);
            scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
//                cout<<"find_this_block::"<<endl;
//                for(int i = 0 ; i < tmp.cur_size ; ++i){
//                    cout<<tmp.array[i].user_id<<"***"<<tmp.array[i].offset<<endl;
//                }
//                cout<<"answer::"<<endl;
//                cout<<tmp.find_user(id)<<endl;
            return tmp.find_user(id);
        }
        off = tmp.next;
    }
    return tmp.find_user(id);
}

void id_linklist::show_connect() {
    id_block tmp;
    if (getnumber() == 0){
        cout<<endl;return;
    }
    int off = sizeof(int);
    while (off != -1){
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        string userid = tmp.array[0].user_id;
            cout<<"head_id::"<<userid<<endl;
            cout<<"block_prev::"<<tmp.prev<<endl;
            cout<<"block_next::"<<tmp.next<<endl;
        off = tmp.next;
    }
}

void id_linklist::excise_user(const string &id) {
    id_block tmp;
    int save = -1;
    int off = sizeof(int);
    while (off != -1){
        save = off;
        scanner.seekg(off);
        scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        if ( id < tmp.array[0].user_id){
            off = tmp.prev;
            scanner.seekg(off);
            scanner.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            tmp.excise(id);
            scanner.seekp(off);
            scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            reopen();
            merge_block();
            return;
        }
        off = tmp.next;
    }
    tmp.excise(id);
    scanner.seekp(save);
    scanner.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
    reopen();
    merge_block();
}

void id_linklist::show_all() {
    id_block blk;
    fstream show("user");
    user use;
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
            show.read(reinterpret_cast<char *>(&use), sizeof(use));
            cout<<use.user_id<<'\t'<<use.passwd<<'\t'<<use.priority<<'\t'<<use.select<<endl;
        }
        next = blk.next;
    }
    show.close();
    if (!flag)cout<<endl;
}

void id_linklist::merge_block() {
    if (getnumber() == 0)return;
    int off = sizeof(int);
    id_block a, b, c;
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

void id_linklist::split_block() {
    if (getnumber() == 0)return;
    int number;
    int off = sizeof(int);
    id_block a, b, c;
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

void id_linklist::numberplus() {
    int number;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    ++number;
    scanner.seekp(0);
    scanner.write(reinterpret_cast<char *>(&number), sizeof(number));
    reopen();
}

int id_linklist::getnumber() {
    int number = 0;
    scanner.seekg(0);
    scanner.read(reinterpret_cast<char *>(&number), sizeof(number));
    return number;
}

void id_linklist::open() {
    scanner.open("refer_id");
}

void id_linklist::reopen() {
    scanner.close();
    scanner.open("refer_id");
}