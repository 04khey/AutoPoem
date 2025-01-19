#include <gtest/gtest.h>
#include <random>
#include <iostream>
#include <vector>

using namespace std;


vector<int> bruteSqueeze(vector<int> toSqueeze){
    vector<int> out;
    return out;
}

vector<int> naiveSqueeze(vector<int> toSqueeze){
    vector<int> out;
    return out;
}

vector<int> genRandomVec(int maxWidth, int length){
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> rand(0,maxWidth); // distribution in range [0, maxWidth]

    vector<int> out;

    for(int i=0;i<length;i++){
        out.push_back(rand(rng));
    }
    return out;
}

int main(){
    for(int i=0;i<1;i++){
        vector<int> testData = genRandomVec(9,20);
        vector<int> brute = bruteSqueeze(testData);
        vector<int> naive = naiveSqueeze(testData);

        for (int j = 0; j < brute.size(); ++j) {
            EXPECT_EQ(brute[j], naive[j]) << "Vectors x and y differ at index " << j;
        }
        
    }
}