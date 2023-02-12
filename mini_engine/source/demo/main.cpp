#include "MEObject.h"
#include "MEJobSystem.h"

#include <iostream>

class A : public MEObject {
public:
    A() {
        std::cout << "AAA"<<std::endl;
    }
    A(int a) {
        std::cout << "AAA:" <<a <<std::endl;
    }
    A(int a, float b) {
        std::cout << "AAA:" <<a <<" " << b <<std::endl;
    }
    
    void p() {
        std::cout << "p" << std::endl;
    }
    
    ~A() {
        std::cout << "delete A" << std::endl;
    }
};
 
int main() {
    
    auto a=ME_ALLOC(A, 10, 5.0f);
    a->p();
    ME_SP_RETAIN(A, res, a);
    ME_SP_RELEASE(a);

    JobSystem js=JobSystem(2);
    
    return 0;
}
