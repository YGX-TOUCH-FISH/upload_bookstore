#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include "book.h"
#include "user.h"
#include "user_id_list.h"
#include "book_isbn_list.h"
#include "book_name_list.h"
#include "book_author_list.h"
#include "book_key_list.h"
//初始化
void build();

//嵌套登录系统
class user log_stack[300];
class user current_user;
int log_status = 0;
void add_user(const string& userid, const string& password, int prior, const string& username,const int &select);

//命令行系统
string command ;
void process(const string& speak);//按照优先级处理语句
void process_su(string speak);
void process_useradd(string speak);
void process_register(string speak);
void process_delete(string speak);
void process_passwd(string speak);
void process_select(string speak);
void process_modify(string speak);
void process_import(string speak);
void process_show(string speak);
void process_buy(string speak);
bool isLegal(const string &speak);
bool isNumber(const string &speak);
//倒排文档：
//用户id的倒排文档
id_linklist idLinklist;
//书籍的isbn倒排
isbn_linklist isbnLinklist;
//书籍的名称-isbn倒排
name_linklist nameLinklist;
//书籍的作者-isbn倒排
author_linklist authorLinklist;
//书籍的关键词-isbn倒排
key_linklist keyLinklist;
//用户原始记录的接口
user_system userSystem;
//书籍原始记录的接口
book_system bookSystem;
//收支记录
cash_log cashLog;

int main() {
    build();
    while (getline(cin,command)){
        if (command.empty())continue;
        if (command == "quit" || command== "exit")break;
        process(command);
    }
    return 0;
}

void build(){
    log_stack[log_status].priority = 0;
    current_user = log_stack[log_status];

    int cnt = 0;
    int number = 0;
    fstream book("book");                   if (!book)          cnt++;
    fstream user("user");                   if (!user)          cnt++;
    fstream refer_isbn("refer_isbn");       if (!refer_isbn)    cnt++;
    fstream refer_id("refer_id");           if (!refer_id)      cnt++;
    fstream refer_name("refer_name");       if (!refer_name)    cnt++;
    fstream refer_author("refer_author");   if (!refer_author)  cnt++;
    fstream refer_keyword("refer_keyword"); if (!refer_keyword) cnt++;
    fstream financial_log("financial_log"); if (!financial_log) cnt++;
    if (cnt != 0){
        ofstream books("book");
        ofstream users("user");
        ofstream refer_isbns("refer_isbn");
        ofstream refer_ids("refer_id");
        ofstream refer_names("refer_name");
        ofstream refer_authors("refer_author");
        ofstream refer_keywords("refer_keyword");
        ofstream financial_logs("financial_log");

        books.write(reinterpret_cast<char *>(&number), sizeof(number));
        users.write(reinterpret_cast<char *>(&number), sizeof(number));
        refer_isbns.write(reinterpret_cast<char *>(&number), sizeof(number));
        refer_ids.write(reinterpret_cast<char *>(&number), sizeof(number));
        refer_names.write(reinterpret_cast<char *>(&number), sizeof(number));
        refer_authors.write(reinterpret_cast<char *>(&number), sizeof(number));
        refer_keywords.write(reinterpret_cast<char *>(&number), sizeof(number));
        financial_logs.write(reinterpret_cast<char *>(&number), sizeof(number));

        book.close();
        user.close();
        refer_isbn.close();
        refer_id.close();
        refer_name.close();
        refer_author.close();
        refer_keyword.close();
        financial_log.close();

        books.close();
        users.close();
        refer_isbns.close();
        refer_ids.close();
        refer_names.close();
        refer_authors.close();
        refer_keywords.close();
        financial_logs.close();

        bookSystem.open();
        userSystem.open();
        isbnLinklist.open();
        idLinklist.open();
        nameLinklist.open();
        authorLinklist.open();
        keyLinklist.open();
        cashLog.open();

        add_user("root","sjtu",7,"root",0);
        return;
    }
    book.close();
    user.close();
    refer_isbn.close();
    refer_id.close();
    refer_name.close();
    refer_author.close();
    refer_keyword.close();
    financial_log.close();

    bookSystem.open();
    userSystem.open();
    isbnLinklist.open();
    idLinklist.open();
    nameLinklist.open();
    authorLinklist.open();
    keyLinklist.open();
    cashLog.open();
}

void process(const string& speak){
    string token ;
    for(int i = 0 ; i < speak.length() && !isspace(command[i]) ; ++i)token += command[i];
    if (token == "su"){
        process_su(speak);
        return;
    }
    if (token == "logout"){
        if (current_user.priority < 1){
            cout<<"Invalid\n";return;
        }
        --log_status;
        current_user = log_stack[log_status];
        return;
    }
    if (token == "useradd"){
        if (current_user.priority < 3){
            cout<<"Invalid\n";return;
        }
        process_useradd(speak);
        return;
    }
    if (token == "register"){
        process_register(speak);
        return;
    }
    if (token == "delete"){
        if (current_user.priority < 7){
            cout<<"Invalid\n";return;
        }
        process_delete(speak);
        return;
    }
    if (token == "passwd"){
        if (current_user.priority < 1){
            cout<<"Invalid\n";return;
        }
        process_passwd(speak);
        return;
    }
    if (token == "select"){
        if (current_user.priority < 3){
            cout<<"Invalid\n";return;
        }
        process_select(speak);
        return;
    }
    if (token == "modify"){
        if (current_user.priority < 3){
            cout<<"Invalid\n";return;
        }
        if (current_user.select == 0){
            cout<<"Invalid\n";return;
        }
        process_modify(speak);
        return;
    }
    if (token == "import"){
        if (current_user.priority < 3){
            cout<<"Invalid\n";return;
        }
        if (current_user.select == 0){
            cout<<"Invalid\n";return;
        }
        process_import(speak);
        return;
    }
    if (token == "show"){
        if (current_user.priority < 1){
            cout<<"Invalid\n";return;
        }
        process_show(speak);
        return;
    }//show book or show finance
    if (token == "buy"){
        if (current_user.priority < 1){
            cout<<"Invalid\n";return;
        }
        process_buy(speak);
        return;
    }
//    if (token == "show_user"){
//        idLinklist.show_all();
//        return;
//    }
//    if (token == "show_status"){
//        cout<<"status::"<<log_status<<endl;
//        cout<<"prior::"<<current_user.priority<<endl;
//        return;
//    }
//    if (token == "show_connect"){
//        idLinklist.show_connect();
//        return;
//    }
    cout<<"Invalid\n";
}

void process_su(string speak){
    int i = 0;
    string userid, password , other;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'su'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)userid += speak[i];
        if (userid.empty()){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)password += speak[i];
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
            cout<<"Invalid\n";return;
        }
    int off = idLinklist.find_user(userid);
        if (off == -1){
            cout<<"Invalid\n";return;
        }
    class user a = userSystem.get_user(off);
    string a_passwd = a.passwd;
    if (current_user.priority > a.priority){
        if (password.empty() || password == a.passwd){
            ++log_status;
            log_stack[log_status] = a;
            current_user = log_stack[log_status];
            return;
        }
        if (password != a_passwd){
            cout<<"Invalid\n";return;
        }
    }
    if (password ==  a_passwd){
        ++log_status;
        log_stack[log_status] = a;
        current_user = log_stack[log_status];
        return;
    }
    if (password != a_passwd){
        cout<<"Invalid\n";return;
    }
}

void process_useradd(string speak){
    int i = 0;
    string userid, password, username , other;
    int prior;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'useradd'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)userid += speak[i];
        if (!isLegal(userid) || idLinklist.find_user(userid) != -1){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)password += speak[i];
        if(!isLegal(password)){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    prior = speak[i] - '0';++i;
        if (current_user.priority <= prior) {
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)username += speak[i];
        if (username.empty()){
        cout<<"Invalid\n";return;
    }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
                cout<<"Invalid\n";return;
        }
    add_user(userid,password,prior,username,0);
}

void process_register(string speak){
    int i = 0;
    string userid, password, username , other;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'register'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)userid += speak[i];
        if (!isLegal(userid)){
            cout<<"Invalid\n";return;
        }
        if (idLinklist.find_user(userid) != -1){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)password += speak[i];
        if(!isLegal(password)){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)username += speak[i];
        if (username.empty()){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
            cout<<"Invalid\n";return;
        }
    add_user(userid,password,1,username,0);
}

void process_delete(string speak){
    int i = 0;
    string userid , other;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'delete'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)userid += speak[i];
        if (userid.empty() || idLinklist.find_user(userid) == -1) {
            cout<<"Invalid\n";return;
        }
    for(int k = 0 ; k <= log_status ; ++k){
        string stack_id = log_stack[k].user_id;
        if (stack_id == userid){
                cout<<"Invalid\n";return;
            }
    }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
            cout<<"Invalid\n";return;
        }
    idLinklist.excise_user(userid);
}

void process_passwd(string speak){
    int i = 0 ;
    string userid, old_passwd, new_passwd , other;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'passwd'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)userid += speak[i];
        if(userid.empty()){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)old_passwd += speak[i];
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)new_passwd += speak[i];
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
            cout<<"Invalid\n";return;
        }
    string current_id = current_user.user_id;
        if (old_passwd.empty() && new_passwd.empty()){
            cout<<"Invalid";return;
        }
    int off = idLinklist.find_user(userid);
        if (off == -1){
            cout<<"Invalid\n";return;
        }
    if (current_id == "root"){
        if (new_passwd.empty()){
            userSystem.change_passwd(off,old_passwd);
            return;
        }
        if (!new_passwd.empty()){
            user a = userSystem.get_user(off);
            if (old_passwd != a.passwd){
                cout<<"Invalid\n";return;
            }
            userSystem.change_passwd(off,new_passwd);
            return;
        }
    }
    else{
        user a = userSystem.get_user(off);
        if (old_passwd != a.passwd){
            cout<<"Invalid\n";return;
        }
        if (new_passwd.empty()){
            cout<<"Invalid\n";return;
        }
        userSystem.change_passwd(off,new_passwd);
        return;
    }
}

void process_select(string speak){
    int i = 0;
    string isbn , other;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'select'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)isbn += speak[i];
        if (isbn.empty()){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
            cout<<"Invalid\n";return;
        }
    int offset = isbnLinklist.find_book(isbn);
    if (offset == -1){
        book tmp(isbn,"","","",0);
        bookSystem.push_back(tmp);
        int num = bookSystem.getnumber();
        offset = sizeof(int) + (num-1)*sizeof(book);
        isbnLinklist.add_book(isbn,offset);
        nameLinklist.add_book("",isbn,offset);
        authorLinklist.add_book("",isbn,offset);
        keyLinklist.add_book("",isbn,offset);
    }//若该书不存在，添加该书
    log_stack[log_status].select = offset;
    current_user = log_stack[log_status];
}

void process_modify(string speak){
    int i = 0;
    string token[6] {"","","","","",""};
    string ISBN ,NAME , AUTHOR , KEYWORD , PRICE;
    double pric = 0;
    int is = 0 ,na= 0, au= 0, ke = 0 ,pr = 0;
    //记录ISBN= ; NAME= ; AUTHOR= ; KEYWORD= ; PRICE= 分别出现的次数
    int count = 0;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'modify'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)token[0] += speak[i];
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)token[1] += speak[i];
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)token[2] += speak[i];
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)token[3] += speak[i];
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)token[4] += speak[i];
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)token[5] += speak[i];
        if (!token[5].empty()){
            cout<<"Invalid\n";return;
        }
    for(auto & k : token){
        if (!k.empty()){
            ++count;
        }
    }
        if (count == 0){
            cout<<"Invalid\n";return;
        }
    for(int k = 0 ; k < 5 ; ++k){
        if (token[k].find("-ISBN=") != string::npos){
            string check;
            for(int r = 0 ; r < 6 ; ++r) check += token[k][r];
            if (check != "-ISBN="){
                cout<<"Invalid\n";return;
            }
            if (is == 0){
                ++is;
                int j = 0;
                for( ; j < token[k].length() && token[k][j] != '=' ; ++j);
                ++j;
                for( ; j < token[k].length() ; ++j)ISBN += token[k][j];
                continue;
            }
            else{
                cout<<"Invalid\n";return;
            }//重复。语法错误
        }
        if (token[k].find("-name=") != string::npos){
            string check;
            for(int r = 0 ; r < 6 ; ++r) check += token[k][r];
            if (check != "-name="){
                cout<<"Invalid\n";return;
            }
            if (na == 0){
                ++na;
                int j = 0;
                for( ; token[k][j] != '"' ; ++j);
                ++j;
                for( ; token[k][j] != '"' ; ++j)NAME += token[k][j];
                continue;
            }
            else{
                cout<<"Invalid\n";return;
            }
        }
        if (token[k].find("-author=") != string::npos){
            string check;
            for(int r = 0 ; r < 8 ; ++r) check += token[k][r];
            if (check != "-author="){
                cout<<"Invalid\n";return;
            }
            if (au == 0){
                ++au;
                int j = 0;
                for( ; token[k][j] != '"' ; ++j);
                ++j;
                for( ; token[k][j] != '"' ; ++j)AUTHOR += token[k][j];
                continue;
            }
            else{
                cout<<"Invalid\n";return;
            }
        }
        if (token[k].find("-keyword=") != string::npos){
            string check;
            for(int r = 0 ; r < 9 ; ++r) check += token[k][r];
            if (check != "-keyword="){
                cout<<"Invalid\n";return;
            }
            if (ke == 0){
                ++ke;
                int j = 0;
                for( ; token[k][j] != '"' ; ++j);
                ++j;
                for( ; token[k][j] != '"' ; ++j)KEYWORD += token[k][j];
                continue;
            }
            else{
                cout<<"Invalid\n";return;
            }
        }
        if (token[k].find("-price=") != string::npos){
            string check;
            for(int r = 0 ; r < 7 ; ++r) check += token[k][r];
            if (check != "-price="){
                cout<<"Invalid\n";return;
            }
            if (pr == 0){
                ++pr;
                int j = 0 ;
                for( ; token[k][j] != '=' ; ++j);
                ++j;
                for( ; j < token[k].length() ; ++j){
                    PRICE += token[k][j];
                }
            }
            else{
                cout<<"Invalid\n";return;
            }
        }
    }
    //获取各变量并计数
    if (pr != 0 && !PRICE.empty())pric = stod(PRICE);
    else pric = 0;
        if (pric < 0){
            cout<<"Invalid\n";return;
        }
    int address = current_user.select;//地址不变
    fstream get_book("book");
    get_book.seekg(address);
    book old_book;
    get_book.read(reinterpret_cast<char *>(&old_book),sizeof(old_book));

    //变量缺省
    if (is == 1 && isbnLinklist.find_book(ISBN) != -1){
        cout<<"Invalid\n";return;
    }
    if (is == 0)ISBN = old_book.isbn;
    if (na == 0)NAME = old_book.name;
    if (au == 0)AUTHOR = old_book.author;
    if (ke == 0)KEYWORD = old_book.keyword;
    if (pr == 0)pric = old_book.price;

    book new_book(ISBN,NAME,AUTHOR,KEYWORD,pric);
    new_book.quantity = old_book.quantity;
    if (old_book == new_book)return;
    if (ke == 1){
        for(int k = 0 ; k < KEYWORD.length() ; ++k){
            if (KEYWORD[k] == '|' && ( k == 0 || k == KEYWORD.length()-1 )){
                cout<<"Invalid\n";return;
            }
        }
    }//keyword语法检查
    int old_cnt = 0;
    int new_cnt = 0;
    string old_keywd[61]={""};
    string new_keywd[61]={""};
    for(int k = 0 ; k < strlen(old_book.keyword) ; ++k){
        if (old_book.keyword[k] == '|'){
            ++old_cnt;
            continue;
        }
        old_keywd[old_cnt] += old_book.keyword[k];
    }//收集旧的keywd
    for(int k = 0 ; k < KEYWORD.length()         ; ++k){
        if (KEYWORD[k] == '|'){
            ++new_cnt;
            continue;
        }
        new_keywd[new_cnt] += KEYWORD[k];
    }//收集新的keywd
    for(int k = 0 ; k <= new_cnt                 ; ++k){
        for(int j = k+1 ; j <= new_cnt ; ++j){
            if (new_keywd[k] == new_keywd[j]){
                cout<<"Invalid\n";
                return;
            }
        }
    }//检测新的keywd是否有重复元素

    isbnLinklist.excise_book(old_book.isbn);
    nameLinklist.excise_book(old_book.name,old_book.isbn);
    authorLinklist.excise_book(old_book.author,old_book.isbn);
    for(int k = 0 ; k <= old_cnt ; ++k){
        keyLinklist.excise_book(old_keywd[k],old_book.isbn);
    }//删除旧的keywd
    get_book.seekp(address);//地址不变
    get_book.write(reinterpret_cast<char *>(&new_book), sizeof(new_book));
    get_book.close();
    isbnLinklist.add_book(ISBN,address);
    nameLinklist.add_book(NAME,ISBN,address);
    authorLinklist.add_book(AUTHOR,ISBN,address);
    for(int k = 0 ; k <= new_cnt; ++k){
        keyLinklist.add_book(new_keywd[k],ISBN,address);
    }//添加新的keywd
}

void process_import(string speak){
    int i = 0;
    string quantity , total_price , other;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'import'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)quantity += speak[i];
        if (!isNumber(quantity)){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)total_price += speak[i];
        if (!isNumber(total_price)){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
        cout<<"Invalid\n";return;
        }
    int quanti = stoi(quantity);
    double total = stod(total_price);
        if (quanti <= 0 || total <= 0){
            cout<<"Invalid\n";return;
        }
    int offset = current_user.select;
    bookSystem.im(offset,quanti);
    cash_msg x('-',total);
    cashLog.push_back(x);
}

void process_show(string speak){
    int i = 0;
    string token;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'show'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)token += speak[i];
    if (token.empty()){
        isbnLinklist.show_all(current_user.priority);
        return;
    }
    if (token == "finance"){
            if (current_user.priority < 7){
                cout<<"Invalid\n";return;
            }
        string time; int tim;
        for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
        for( ; i < speak.length() && !isspace(speak[i]) ; ++i)time += speak[i];
            if (time.empty()){
                cashLog.show(-1);return;
            }
            if (!isNumber(time)){
                cout<<"Invalid\n";return;
            }
        tim = stoi(time);
            if (tim < 0 || tim > cashLog.getnumber()){
                cout<<"Invalid\n";return;
            }
        cashLog.show(tim);
        return;
    }
    string other;
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
        cout<<"Invalid\n";return;
    }
    int count = 0;
    string isbn , name , author , keyword ;
    if (token.find("-ISBN=") != string::npos){
        count++;
        int k = 0;
        for( ; token[k] != '=' && k < token.length() ; ++k);
        ++k;
        for( ; k < token.length() ; ++k)isbn += token[k];
    }
    if (token.find("-name=") != string ::npos){
        count++;
        int k = 0;
        for( ; token[k] != '"' && k < token.length() ; ++k);
        ++k;
        for( ; token[k] != '"' && k < token.length() ; ++k)name += token[k];
    }
    if (token.find("-author=") != string::npos){
        count++;
        int k = 0;
        for( ; token[k] != '"' && k < token.length() ; ++k);
        ++k;
        for( ; token[k] != '"' && k < token.length() ; ++k)author += token[k];
    }
    if (token.find("-keyword=") != string::npos){
        count++;
        int k = 0;
        for( ; token[k] != '"' && k < token.length() ; ++k);
        ++k;
        for( ; token[k] != '"' && k < token.length() ; ++k)keyword += token[k];
    }
    if (count != 1){
        cout<<"Invalid\n";return;
    }
    if (!isbn.empty()){
        int offset = isbnLinklist.find_book(isbn);
        if (offset == -1){
            cout<<endl;return;
        }
        bookSystem.show(offset,current_user.priority);
        return;
    }
    if (!name.empty()){
        nameLinklist.show(name,current_user.priority);
        return;
    }
    if (!author.empty()){
        authorLinklist.show(author,current_user.priority);
        return;
    }
    if (!keyword.empty()){
        if (token.find('|') != string::npos){
            cout<<"Invalid\n";return;
        }
        keyLinklist.show(keyword,current_user.priority);
        return;
    }
    cout<<"Invalid\n";
}

void process_buy(string speak){
    int i = 0; int quanti = -1; int offset = -1;
    double money; double solo_price;
    string isbn , quantity , other;
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i);//skip 'buy'
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)isbn += speak[i];
        if (isbn.empty()){
            cout<<"Invalid\n";return;
        }
    offset = isbnLinklist.find_book(isbn);
        if (offset == -1){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)quantity += speak[i];
        if (!isNumber(quantity)){
            cout<<"Invalid\n";return;
        }
    quanti = stoi(quantity);
        if (quanti < 0){
            cout<<"Invalid\n";return;
        }
        if (bookSystem.get_quantity(offset) < quanti){
            cout<<"Invalid\n";return;
        }
    for( ; i < speak.length() &&  isspace(speak[i]) ; ++i);//skip space
    for( ; i < speak.length() && !isspace(speak[i]) ; ++i)other += speak[i];
        if (!other.empty()){
            cout<<"Invalid\n";return;
        }
    bookSystem.ex(offset,quanti);
    solo_price = bookSystem.get_price(offset);
    money = quanti*solo_price;
    cash_msg buy('+',money);
    cashLog.push_back(buy);
    cout<<fixed<<setprecision(2)<<money<<endl;
}

bool isLegal(const string &speak){
    if (speak.empty())return false;
    for(char i : speak){
        if (i >= '0' && i <= '9')continue;
        if (i >= 'A' && i <= 'Z')continue;
        if (i >= 'a' && i <= 'z')continue;
        if (i == '_')continue;
        return false;
    }
    return true;
}

bool isNumber(const string &speak){
    istringstream sin(speak);
    double test;
    return sin>>test && sin.eof();
}

void add_user(const string& userid, const string& password, int prior, const string& username, const int &select){
    int offset = idLinklist.find_user(userid);
    if (offset != -1){
        cout<<"Invalid\n";return;
    }//重复添加账户
    class user x(userid,password,prior,username,select);
    userSystem.push_back(x);
    int num = userSystem.getnumber();
    offset = sizeof(int) + (num-1)*sizeof(user);
    idLinklist.add_user(userid,offset);
}
