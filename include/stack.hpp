#pragma once

#define STACK_SIZE 4096 * 4096

class Stack
{
    public:
    Stack(float incFactor = 1.5f);
    ~Stack();
    void push(float value);
    void push(double value);
    void push(int value);
    void push(char value);
    void push(char* value);
    void push(unsigned int value);
    void push(long long value);
    void push(unsigned long long value);
    void push(char* value, int size);

    float pop_float();
    double pop_double();
    int pop_int();
    char pop_char();
    char* pop_str();
    char* pop(int size = 1);
    unsigned int pop_uint();
    long long pop_llong();
    unsigned long long pop_ullong();    

    void print(const char* end = "\n");

    private:
    void validate(int size);

    float incFactor;
    char* top;
    char* data;
    int size;
    int capacity;
};