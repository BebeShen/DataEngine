#include<iostream>
#include<cstring>  
#include<fstream>
#include<sstream>  
#include<vector>
/*map, unordered_map*/
#include<queue>
#include<map>
#include<ctime>
// set maxbuf size to (128 bytes*1,000,000) = 128MB
#define maxbuf 10
// #define PageSize INT32_MAX
#define PageSize 4000
using namespace std;
string IntToString(unsigned int num){
    stringstream ss;
    string s;
    ss<<num;
    ss>>s;
    return s;
}
string CheckStorage(unsigned long long file_number,unsigned long long key){
    string file_path = "storage/";
    cout<<file_number<<", "<<key<<"\n";
    fstream tmpfile(file_path+to_string(file_number),ios_base::in);
    if(!tmpfile){
        return "EMPTY";
    }
    // declare local variable
    const char* delim = " ";
    int key_pos,value_pos;
    string value;
    unsigned long long _key;
    while(!tmpfile.eof()){
        string tmp;
        getline(tmpfile,tmp);
        key_pos = tmp.find_first_of(delim,0);
        _key = atoll(tmp.substr(0,key_pos).c_str());
        tmp = tmp.substr(key_pos+1,tmp.length());
        if(_key != key){
            continue;
        }
        else if(_key > key){
            // Because stored data is ordered, 
            // so if the current key exceed wanted key,
            // the value doesn't exist.
            return "EMPTY";
        }
        else{
            // find the key in storage
            // cout<<"key:"<<_key<<"|";
            value_pos = tmp.find_first_of(delim,0);
            value = tmp.substr(0,value_pos);
            // cout<<"value:"<<value<<"\n";
            return value;
        }
    }
    return "EMPTY";
}
void DataToStorage(string file_path,map<unsigned long long,string>data){
    map<unsigned long long,string>stored_data;
    cout<<"storage:"<<file_path<<"\n";
    fstream tmpfile(file_path,ios_base::in);
    map<unsigned long long, string>::iterator itr;
    if(!tmpfile){
        // storage doesn't exist
        cout<<"Storage doesn't exist. Create one.\n";
        tmpfile.close();
        for(itr = data.begin();itr != data.end();++itr){
            cout<<itr->first<<", "<<itr->second<<"\n";
            stored_data.insert({itr->first,itr->second});
        }
    }
    else{
        // if storage exist
        cout<<"Storage Exist!\n";
        int key_pos,value_pos;
        unsigned long long _key;
        const char* delim = " ";
        string s,value;
        while(!tmpfile.eof()){
            getline(tmpfile,s);
            key_pos = s.find_first_of(delim,0);
            _key = atoll(s.substr(0,key_pos).c_str());
            s = s.substr(key_pos+1,s.length());
            value_pos = s.find_first_of(delim,0);
            value = s.substr(0,value_pos);
            stored_data.insert({_key,value});
        }
        tmpfile.close();
            
        for(itr = data.begin();itr != data.end();++itr){
            if(stored_data.count(itr->first)==0){
                // No old data in storage, so insert a new entry
                stored_data.insert({itr->first,itr->second});
            }
            else{
                // Update data in storage
                stored_data.at(itr->first) = itr->second;
            }
        }
    }

    tmpfile.open(file_path,ios_base::out);
    for(itr = stored_data.begin();itr != stored_data.end();++itr){
        tmpfile<<itr->first<<" "<<itr->second<<"\n";
    }
    tmpfile.close();
}
// .exe [inputfile]
int main(int argc,char *argv[]){
    // Set ENV
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    double start_time = clock();
    string outputfile_path = "result.output";
    string inputfile_path = "";
    map<unsigned long long,string> buffer; 
    map<unsigned long long,string> cache; 
    inputfile_path.append(argv[1]);
    // outputfile_path.append(outputfile_path);
    fstream inputFile(inputfile_path,ios_base::in);
    fstream outputFile(outputfile_path,ios_base::out);
    // cout<<"inputFile: "<<inputfile_path<<"\noutputFile: "<<outputfile_path<<endl;
    /* Code start here */
    // read command from inputfile
    if(!inputFile){
        cout<<"File doesn't exist.";
        exit(1);
    }
    // declare needed variables
    string instr;
    const char* delim = " ";
    int command_pos,key_pos,value_pos;
    string command,value;
    while(!inputFile.eof()){
        getline(inputFile,instr);
        // extract command from instruction
        command_pos = instr.find_first_of(delim,0);
        command = instr.substr(0,command_pos);
        instr = instr.substr(command_pos+1,instr.length());
        cout<<command<<"|";
        if(command == "PUT"){
            // extract key from instruction
            key_pos = instr.find_first_of(delim,0);
            // cout<<key_pos<<endl;
            unsigned long long key = atoll(instr.substr(0,key_pos).c_str());
            instr = instr.substr(key_pos+1,instr.length());
            // cout<<key<<"|";
            // extract value from instruction
            value_pos = instr.find_first_of(delim,0);
            value = instr.substr(0,value_pos);
            // cout<<value<<"\n";

            /* Implement PUT instr */
            if(buffer.size() <= maxbuf){
                // buffer.insert(pair<unsigned long long int, string>(key,value));
                if(buffer.count(key)==0){
                    // buffer doesn't have the data
                    buffer.insert({key,value});
                }
                else{
                    buffer.at(key) = value;
                }
            }
            else{
                cout<<"Exceed the buffer limit!\n";
            }
        }
        else if(command == "GET"){
            // extract key from instruction
            key_pos = instr.find_first_of(delim,0);
            unsigned long long key = atoll(instr.substr(0,key_pos).c_str());
            instr = instr.substr(key_pos+1,instr.length());
            //cout<<key<<"|\n";

            /* Implement GET instr */
            if(buffer.count(key)==0){
                string tmp = CheckStorage(key/PageSize,key%PageSize);
                // cout<<CheckStorage(key/INT32_MAX,key%INT32_MAX)<<"\n";
                outputFile<<tmp<<"\n";
            }
            else{
                cout<<buffer.at(key)<<"\n";
                outputFile<<buffer.at(key)<<"\n";
            }
        }
        else if(command == "SCAN"){
            // extract key1 from instruction
            key_pos = instr.find_first_of(delim,0);
            unsigned long long key_1 = atoll(instr.substr(0,key_pos).c_str());
            instr = instr.substr(key_pos+1,instr.length());
            //cout<<key_1<<"|";
            // extract key2 from instruction
            key_pos = instr.find_first_of(delim,0);
            unsigned long long key_2 = atoll(instr.substr(0,key_pos).c_str());
            instr = instr.substr(key_pos+1,instr.length());
            //cout<<key_2<<"|\n";

            /* Implement SCAN instr */
            for(unsigned long long i=key_1;i<=key_2;++i){
                if(buffer.count(i)>0){
                    //cout<<buffer[i]<<"\n";
                    outputFile<<buffer.at(i)<<"\n";
                }else{
                    string tmp = CheckStorage(i/PageSize,i%PageSize);
                    //cout<<CheckStorage(i/INT32_MAX,i%INT32_MAX)<<"\n";
                    outputFile<<tmp<<"\n";
                }
            }
        }

        // if buffer is full, move data to storage
        int last_file = 0;
        if(buffer.size() == maxbuf){
            cout<<"Write File!\n";
            string tmpfile_path;
            map<unsigned long long, string>::iterator itr;
            unsigned long long store_number;
            for (itr = buffer.begin();itr != buffer.end();++itr) { 
                cout << itr->first <<", " << itr->second <<"\n"; 
            }cout<<"\n\n";
            for (itr = buffer.begin();itr != buffer.end();++itr) { 
                cout << itr->first <<", " << itr->second <<"\n"; 
                // page data into storage classified by key's upper 32-bit
                store_number = itr->first/PageSize;
                cache.insert({itr->first%PageSize,itr->second});
                tmpfile_path = "storage/";
                tmpfile_path.append(to_string(store_number));
                DataToStorage(tmpfile_path,cache);
                cache.clear();
            }
            buffer.clear();
        }
        // output buffer 
        // map<unsigned long long int, string>::iterator itr;
        // for (itr = buffer.begin();itr != buffer.end();++itr) { 
        //     cout << itr->first <<", " << itr->second <<"\n"; 
        // } 
    }
    cout<<"Buffer:"<<buffer.size()<<"\n";
    inputFile.close();
    outputFile.close();
    /* End here */
    double end_time = clock();
    cout<<"\nExecution time:"<< (end_time - start_time) / CLOCKS_PER_SEC << "(s)\n";
    return 0;
}
