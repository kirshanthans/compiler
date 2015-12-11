//
//  main.cpp
//  Hi
//
//  Created by lsakka on 12/1/15.
//  Copyright © 2015 lsakka. All rights reserved.
//

#include <iostream>
#include "astnode.h"
#include <set>
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    set<string>s;
    s.insert("1");
    s.insert("1");
    s.erase("1");
    cout<<s.size(); 
    
    return 0;
}
