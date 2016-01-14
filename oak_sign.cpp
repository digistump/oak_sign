#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
using namespace std;

char system_data[1] = {0x01};
char user_data[1] = {0x00};
char end_data[7] = {0x4F,0x61,0x6B,0xFF,0xFF,0xFF,0xFF};
char start_data[4] = {0xFF,0xFF,0xFF,0xFF};
char pad[1] = {0xFF};
char system_flag[] = "system";
char unsign_flag[] = "unsign";


int truncate(const char *fileName, uint32_t newsize)
{
	ifstream infile(fileName);
    char buffer[newsize];
    infile.read(buffer,newsize);
    infile.close();
    ofstream outfile(fileName, ios::out | ios::binary);
    outfile.write(buffer,newsize);
    outfile.close();
}

void help(){
    cout << "oak_sign v1 usage: oak_sign (filename) [optional: system or unsign]" << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "signs filename, an oak compatible bin file, for OTA usage" << endl;
    cout << "if system is specified, it will be signed as a system " << endl;
    cout << "update, if unsign is specified any signature will be " << endl;
    cout << "removed, otherwise it is signed as a user program" << endl;
}

uint32_t file_exists(const char *fileName)
{
    streampos begin,end;
    ifstream infile(fileName);
    if(infile.good()){
        begin = infile.tellg();
        infile.seekg (0, ios::end);
        end = infile.tellg();
        infile.close();
        return end-begin;
    }
    return 0;
}

uint32_t has_signature(const char *fileName)
{
		ifstream infile(fileName);
        infile.seekg(-16, ios::end);
        char check_sig[16];
        infile.read(check_sig,16);


        if(check_sig[0] == 0xffffffff &&
        check_sig[1] == 0xffffffff &&
        check_sig[2] == 0xffffffff &&
        check_sig[3] == 0xffffffff &&
        //4,5,6,7 is original size
        //8 is type flag
        check_sig[9] == 0x4F &&
        check_sig[10] == 0x61 &&
        check_sig[11] == 0x6B &&
        check_sig[12] == 0xffffffff &&
        check_sig[13] == 0xffffffff &&
        check_sig[14] == 0xffffffff &&
        check_sig[15] == 0xffffffff
        ){

        	return ( ((unsigned char)check_sig[4] << 24) 
                   + ((unsigned char)check_sig[5] << 16) 
                   + ((unsigned char)check_sig[6] << 8) 
                   + ((unsigned char)check_sig[7] ) );
        }

        return 0;
}

void size_to_bytes(uint32_t filesize, char *byteArray){
	 // convert from an unsigned long int to a 4-byte array 
     byteArray[0] = (char)((filesize >> 24) & 0xFF) ;
	 byteArray[1] = (char)((filesize >> 16) & 0xFF) ;
	 byteArray[2] = (char)((filesize >> 8) & 0XFF);
	 byteArray[3] = (char)((filesize & 0XFF));
}





void write_signature(ofstream& myfile,bool isSystem,unsigned long filesize){


		if(filesize > 0){ //otherwise we are only writing second half
			myfile.write(start_data,4);
			char size_bytes[4];
			size_to_bytes(filesize, size_bytes);
			myfile.write(size_bytes,4);
		}

		if(isSystem){
			cout << "Signing file as system rom." << endl;
	        myfile.write(system_data,1);
	    }
	    else{
	        cout << "Signing file as user rom." << endl;
	        myfile.write(user_data,1);
	    }

	    myfile.write(end_data,7);
}


int main (int argc, char* argv[]) {
    bool isSystem = false;
    bool isUnsign = false;
    
    if(argc < 2 || argc > 3){
        help();
        return 1;
    }
    if(argc == 3){
        if(strcmp(argv[2],system_flag) == 0){
            isSystem = true;
        }
        else if(strcmp(argv[2],unsign_flag) == 0){
            isUnsign = true;
        }
        else{
            help();
            return 1;
        }
        
    }
    
    uint32_t filesize = file_exists(argv[1]);

 
    
    if(filesize == 0){
        cout << "ERROR: File does not exist, is empty, or is locked!" << endl;
        cout << "---------------------------------------------------------" << endl;
        help();
        return 1;
    }

    uint32_t original_size = has_signature(argv[1]);

    if(isUnsign && original_size == 0){
    	cout << "ERROR: File is not signed!" << endl;
	    cout << "---------------------------------------------------------" << endl;
        help();
        return 1;
    }

    if(isUnsign && original_size != 0){
    	cout << "Unsigning file." << endl;
    	truncate(argv[1],original_size);
    	return 0;
    }

    if(original_size != 0){
    	cout << "INFO: File has already been signed!" << endl;
		cout << "---------------------------------------------------------" << endl;
		cout << "Replacing old signature." << endl;
		ofstream myfile (argv[1], ios::out | ios::in | ios::binary);
		myfile.seekp(-8, ios::end); //only need to rewrite second half
		write_signature(myfile,isSystem,0);
		return 0;
    }
    
    
    ofstream myfile (argv[1], ios::out | ios::app | ios::binary);

    //PAD IT OUT
    int remain = (filesize)%512;
    
    if(remain<16){
        remain += 512;
    }
    
    cout << "Padding file by " << remain << " bytes including 16 byte signature." << endl;
    remain -= 16;
    while(remain > 0){
        //pad file out
        myfile.write(pad,1);
        remain--;
    }

    write_signature(myfile,isSystem,filesize);
    return 0;
}
