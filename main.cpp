#include <bits/stdc++.h>
#include <omp.h>
#pragma GCC optimize ("O3")

using namespace std;

double dotProduct(const vector<double>& v1, const vector<double>& v2) {
    double cur_val=0;
//    #pragma omp parallel default(none) reduction(+:cur_val) shared(v1,v2)
    #pragma omp for
    for(unsigned int i=0; i < v1.size(); i++) {
        cur_val += v1[i]*v2[i];
    }
    return cur_val;
}

double cosineSimilarity(const vector<double>& v1, const vector<double>& v2) {
    double res=dotProduct(v1, v2);
    double size1=0;
    double size2=0;
    for(double i : v1) {
        size1+=i*i;
    }
    for(double i : v2) {
        size2+=i*i;
    }
//    cout<<"res: "<<res<<" size1: "<<size1<<" size2: "<<size2<<endl;
    return res/sqrt(size1*size2);
}

template<class HashKeyType,int HashSize, int Dimension=1> // HashKeyType is either int/ ll
class HashTable {
private:
    unordered_map<HashKeyType, vector<string> > hash_table;
    vector< vector<double> > projections;
    HashKeyType generate_hash(const vector<double>& v) {
        HashKeyType cur=0;
        #pragma omp for
        for(int i=0; i<HashSize; i++) {
            if(dotProduct(projections[i],v)>0) {
                cur=cur|1;
            }
            cur=cur<<1;
        }
        return cur;
    }


public:
    HashTable() {
        // create an instance of an engine.
        random_device rnd_device;
        // Specify the engine and distribution.
        mt19937 mersenne_engine {rnd_device()};  // Generates random double
//        uniform_real_distribution<double> dist(-10, 10);
        normal_distribution<double> dist;
        auto gen = [&dist, &mersenne_engine](){
            return dist(mersenne_engine);
        };
        for(int i=0;i<HashSize; i++) {
            vector<double> vec(Dimension);
            generate(begin(vec), end(vec), gen);
            projections.push_back(vec);
        }
    }
    void set_item(vector<double>& v, string label) {
        HashKeyType hash_value = generate_hash(v);
        //cout<<"hash value: "<<hash_value<<endl;
        if(hash_table.count(hash_value)==0) {
            // New value
            vector<string> tmp;
            tmp.push_back(label);
            hash_table[hash_value]=tmp;
        }
        else {
            hash_table[hash_value].push_back(label);
        }
    }

    vector<string> get_item(vector<double>& v) {
        return hash_table[generate_hash(v)];
    }
};

template<class HashKeyType,int NumTables, int HashSize, int Dimension=1> // HashKeyType is either int/ ll
class LSH {
private:
    vector<HashTable<HashKeyType, HashSize, Dimension> > hash_tables;
public:
    LSH() {
        for(int i=0;i<NumTables;i++) {
            HashTable<HashKeyType, HashSize, Dimension> hash_table;
            hash_tables.push_back(hash_table);
        }
    }
    void set_item(vector<double>& v, string label) {
        for(auto& table:hash_tables) {
            table.set_item(v, label);
        }
    }
    unordered_set<string> get_item(vector<double>& v) {
        unordered_set<string> results;
        results.clear();
        for(auto& table:hash_tables) {
            vector<string> tmp = table.get_item(v);
            for(const auto& label:tmp) {
                results.insert(label);
            }
        }
        return results;
    }
};

unordered_map<string, vector<double> > source;
//#pragma omp parallel
int main() {
    const int dimensions =50;
    double temp;
    LSH<int, 50, 10, dimensions> lsh;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    FILE * pFile;
    pFile = fopen ("../glove.6B.50d.txt","r");
    for(int i=0;i<40000;i++) {
        char label[100];
        vector<double> v;
        fscanf(pFile,"%s", label);
        #pragma omp for
        for(int j=1;j<=dimensions;j++) {
            fscanf(pFile, "%lf",&temp);
            v.push_back(temp);
        }
        source[label]=v;
        lsh.set_item(v, label);
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Time difference = " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    cout<<"max threads: "<<omp_get_max_threads()<<endl;
    char labelq[100] = "cat";
    vector<double> vq;
    vector<pair<double, string> > sortedres;
//    while(true) {
//        // Test Query
//        scanf("%s",labelq);
//        vq= source[labelq];
//
//        if(strcmp(labelq,"break!")==0) {
//            break;
//        }
//        begin = chrono::steady_clock::now();
//        // Process Query
//        auto res = lsh.get_item(vq);
//        cout<<"Word that is similar to "<<labelq<<" are: \n";
//        sortedres.clear();
//        for (const auto& ans:res) {
//            sortedres.emplace_back(1-cosineSimilarity(vq, source[ans]), ans);
//        }
//        sort(sortedres.begin(), sortedres.end());
//        cout<<"Number of possible matches: "<<res.size()<<endl;
//        for(int i=0;i< min(10,(int)sortedres.size());i++) {
//            cout<<sortedres[i].second<<" cosine distance="<<sortedres[i].first<<endl;
//        }
//        end = chrono::steady_clock::now();
//        cout << "Time difference = " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
//
//        cout<<"Manual test"<<endl;
//        sortedres.clear();
//        begin = chrono::steady_clock::now();
//        for(auto a:source) {
//            sortedres.emplace_back(1-cosineSimilarity(vq, a.second), a.first);
//        }
//        sort(sortedres.begin(),sortedres.end());
//        for(int i=0;i< min(10,(int)sortedres.size());i++) {
//            cout<<sortedres[i].second<<" cosine distance="<<sortedres[i].first<<endl;
//        }
//        end = chrono::steady_clock::now();
//        cout << "Time difference = " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
//    }

    return 0;
}