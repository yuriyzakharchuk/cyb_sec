#include <iostream>
#include <vector>
#include <map>
#include <cstring>

long
gcd(long a, long b) 
{ 
 	if (a == 0) 
 		return b; 
 	return gcd(b % a, a); 
} 

std::vector<long>
get_all_coprime_ints(long m) {
    std::vector<long> a;
    for(int i = 1; i < m; ++i) {
        if(gcd(i, m) == 1) {
            a.push_back(i);
        }
    }
    return a;
}

int
main(int argc, char *argv[]) {
    if(argc != 4) {
        std::cerr << "Usage m freq_in_source freq_in_dest " << std::endl;
        std::exit(1);
    }
    std::map<long, long> coeff_pairs;

    long m { std::atol(argv[1]) };
    long x { std::atol(argv[2]) };
    long y { std::atol(argv[3]) };
    for(auto a : get_all_coprime_ints(m)) {
        for(int b = 0; b < m; ++b) {
            if((x * a + b) % m == y) {
                coeff_pairs.insert(std::pair<int, int>(a, b));
            }
        }
    }
    for(auto &x : coeff_pairs) {
        std::cout << "a = " << x.first << " b = " << x.second << std::endl;
    }
    return 0;
}

