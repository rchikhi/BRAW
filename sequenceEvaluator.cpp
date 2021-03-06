#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_set>



using namespace std;



char revCompChar(char c) {
	switch (c) {
		case 'A': return 'T';
		case 'C': return 'G';
		case 'G': return 'C';
	}
	return 'A';
}



string revComp(const string& s){
	string rc(s.size(),0);
	for (int i((int)s.length() - 1); i >= 0; i--){
		rc[s.size()-1-i] = revCompChar(s[i]);
	}
	return rc;
}



string getCanonical(const string& str){
	return (min(str,revComp(str)));
}



uint inAnotinB(const unordered_set<string>& A, const unordered_set<string>& B){
	uint res(0);
	for(auto it=A.begin(); it!=A.end(); ++it){
		if(B.count(*it)==0){
			++res;
		}
	}
	return res;
}



int main(int argc, char ** argv){
	if(argc<4){
		cout<<"[unitig file] [reference file] [k value]"<<endl;
		exit(0);
	}
	string inputUnitig(argv[1]);
	string inputRef(argv[2]);
	uint k(stoi(argv[3]));
	srand (time(NULL));
	string ref, useless;
	ifstream inRef(inputRef),inSequences(inputUnitig);
	if(not inRef.good() or not inSequences.good()){
		cout<<"Problem with files opening"<<endl;
		exit(1);
	}
	vector<uint> lengths;
	unordered_set<string> genomicKmers;
	unordered_set<string> seenKmers;
	uint size(0),number(0);
	while(not inRef.eof()){
		getline(inRef,useless);
		getline(inRef,ref);
		if(not ref.empty() and not useless.empty()){
			for(uint i(0);i+k<=ref.size();++i){
				genomicKmers.insert(getCanonical(ref.substr(i,k)));
			}
		}
	}
	uint FP(0),TP(0),FN(0);
	while(not inSequences.eof()){
		getline(inSequences,useless);
		getline(inSequences,ref);
		if(not ref.empty() and not useless.empty()){
			for(uint i(0);i+k<=ref.size();++i){
				seenKmers.insert(getCanonical(ref.substr(i,k)));
			}
		}
	}


	FP=inAnotinB(seenKmers,genomicKmers);
	FN=inAnotinB(genomicKmers,seenKmers);
	TP=genomicKmers.size()-FN;
	cout<<"Unitig number: "<<number<< " Total size: "<<size<<" Mean: "<<size/number<<endl;
	cout<<"Genomic kmer in the reference: "<<genomicKmers.size()<<endl;
	cout<<"True positive (kmers in the sequences and the references, good kmers): "<<TP<<endl;
	cout<<"False positive (kmers in the sequences and NOT in the references, erroneous kmers): "<<FP<<endl;
	cout<<"False Negative (kmers NOT in the sequences but in the references, missed kmers): "<<FN<<endl;
	cout<<"Erroneous kmer rate (*1000): "<<(double)1000*FP/(FP+TP)<<endl;
	cout<<"Missing kmer rate (*1000): "<<(double)1000*FN/genomicKmers.size()<<endl;

}
