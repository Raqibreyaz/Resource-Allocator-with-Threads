#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include 
#include <signal.h>

/*
- std::atexit: run when process exits

- stack unwinding happens only when:
    - successfully run
    - custom error throw

- crash vs custom_error:
    - for crash(like seg fault, stack overflow), there's no stack unwinding
    - for custom_error it will have stack unwinding
    - controller will go back according to call stack until it founds try-catch, only for custom_error
    - controller directly get exits in case of crash(like seg fault, stack overflow)

- thread_local:
    - it's memory gets destroyed for every case regardless of crash or custom errors etc
    - unlike stack object
*/

class A{
public:
    A(){
        std::clog<<"constructor called"<<std::endl;
        std::clog<<"resources attached"<<std::endl;
    }
    
    ~A(){
        std::clog<<"destructor called"<<std::endl;
        std::clog<<"resources detached"<<std::endl;
    }
};

class File{
    std::fstream fstream;
public:
    File(){
        fstream.open("file.txt",std::ios::out | std::ios::in);

        std::clog<<"File opened!!"<<std::endl;
    }

    void close_file(){
        if(fstream.is_open()){
            fstream.close();
            std::clog<<"File closed!!"<<std::endl;
        }
        else
        std::clog<<"file was already closed!!\n";
    }

    ~File(){
        close_file();
    }
};

int fn(){
    std::atexit([]{std::clog<<"Hello world in atexit! from fn()"<<std::endl;});
    return 0;
}

// []
// port : 8080, binded to server, ctrl+c
// port already in use

File* fileptr = nullptr;

void cleanup_fn(int sig){
    std::clog<<"Cleanedup in SIGSEGV"<<std::endl;
    
    std::clog<<fileptr<<std::endl;
    fileptr->close_file();

    exit(EXIT_FAILURE);
}

void worker(){
    int val = 42; // stack storage, will not get destoryed on exit()

    thread_local int* x = &val; // thread-local-storage, will get destroyed in any case
    
    exit(EXIT_FAILURE);

    std::clog<<"printing local int: "<<*x<<std::endl;
}

int main(){

File * file = new File();
fileptr = file;

// try catch cant handle it ever
// we have to catch that exception using, OS specific signals

// std::variant<int,std::string,float> x = 42;
// decltype(x);

signal(SIGSEGV,cleanup_fn);

std::thread w(worker);

// try{
//     int *ptr = nullptr;
//     *ptr = 10;
    throw std::runtime_error("custom error!");
// }catch(const std::exception& e){
//     std::cerr<<e.what()<<std::endl;
// }

// delete file;
// fileptr = nullptr;

w.join();

return 0;
}
// [main, fn1, fn2]
//     .   <- <- .   <- <-crash
//              exit()