
#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <fstream>
#include <ostream>
#include <string>
#include <cstdio>
#include <stdlib.h>
using namespace std;
void printf(string filename) 
{
  cout<<"An integrity problem has occured in "<<filename;
  cout<<" ,so, ALL the files after it have a problem"<<endl;
   
} 
string Stdout(string command) {
/*I got this code from the internet to read the the output from the CMD after executing the command.
 link--> https://www.jeremymorgan.com/tutorials/c-programming/how-to-capture-the-output-of-a-linux-command-in-c/
*/
  string data;
  FILE * stream;
  const int max_buffer = 256;  
  char buffer[max_buffer];
  command.append(" 2>&1");

  stream = popen(command.c_str(), "r");
  if (stream) {
    while (!feof(stream))
      if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
    pclose(stream);
  }
  return data;
}
void case_1 (string plain,string keyyfile) 
{    
   string cleartxtfile=plain;
   string keyfile = keyyfile;
   string g=cleartxtfile+".enc";
   string hashcommand;
   int i=0;
   string data[100];
   string lastline;
   string l;

   //Now let's work on the chain and hashing 
  ofstream hash; //for writing on the block chain
  ifstream hash2; //for reading on the block chain
  hash2.open("BlockChain.txt");
  if (!hash2)
  { //file is not creted yet and it is empty 
    //then start the hash of a file normally 
    hash.open("BlockChain.txt"); 


   //this command will encrypt the entered file 
   string commandline="openssl enc -e -aes-256-cbc -in "+cleartxtfile+" -out "+g+" -kfile "+keyfile+" -nosalt" ; 
   system(commandline.c_str()); //system function call to execute the command on terminal
   //encryption is done

  
   
    string hashcommand="openssl dgst -sha512 -hex "+cleartxtfile+"";
    l=Stdout(hashcommand);
    hash<<l;
    hash.close();
    hash2.close();
  }
  else { 
  //then blockchain file is already creted 
  hash.open("BlockChain.txt",std::ios_base::app); //To append and not overwrite data 
  
  while (getline(hash2,lastline)) //by this we get the last hashed line 
 {
   data[i]=lastline;
   i++;
 }
  i=i-1;
  string thewantedlastline=data[i]; //last line 
   std::size_t pos = thewantedlastline.find(" ");
  thewantedlastline=thewantedlastline.substr(pos+1); //keep reading after the space
 //Now let 's concatenate it with the plain text  
  ifstream reading ;
  reading.open(cleartxtfile.c_str());
  string text;
  string text1[20]; 
  int t=0;
  while (getline(reading,text))
  {
   text1[t]=text;
   t++;
  }
  text=text1[0];
  for (int d=1 ;d<t ; d++)   //What i did here, when a file has multiple lines it will put them together in one line
  {text=text+" "+text1[d];
  }
  
  ofstream writing;
  writing.open(cleartxtfile.c_str(), std::ofstream::out | std::ofstream::trunc);
  writing<<text<<" "<<thewantedlastline+"\n"; //This is concatenated plain txt + hash 
  writing.close();
  hashcommand="openssl dgst -sha512 -hex "+cleartxtfile+"";
  l=Stdout(hashcommand);
  hash<<l;
 //Now encrypt 
  string commandline="openssl enc -e -aes-256-cbc -in "+cleartxtfile+" -out "+g+" -kfile "+keyfile+" -nosalt" ; 
  system(commandline.c_str()); 
  writing.open(cleartxtfile.c_str());
  writing<<text; //retrieve the text like it was before we added the hash to it.
  writing.close();
  hash.close();
  hash2.close();
  reading.close();
 } 
 //work here is done.
}
void case_2 (string d_plain,string keyyfile)
{
   string d_cleartxtfile=d_plain;
   string keyfile = keyyfile;
   //want to open 
   string maintxt=d_plain; 
   std::size_t pos = maintxt.find(".enc");
   maintxt=maintxt.replace(pos,4,"");
   ifstream retrieve; 
   retrieve.open(maintxt.c_str());
   string text;
   string text1[20]; 
   int t=0;
  while (getline(retrieve,text))
  {
   text1[t]=text;
   t++;
  }
  retrieve.close();
  text=text1[0];
  for (int d=1 ;d<t ; d++)   //What i did here, when a file has multiple lines it will put them together in one line
  {text=text+" "+text1[d];
  }
   string d=d_cleartxtfile+".dec";
   string commandline="openssl enc -d -aes-256-cbc -in "+d_cleartxtfile+" -out "+d+" -kfile "+keyfile+" -nosalt" ; 
   system(commandline.c_str()); 
   //this should decrypt the file showing the content of the file+if there is a hash from previous files
   //now retrive the file 
   ofstream ret;
   ret.open(d.c_str());
   ret<<text; 
   ret.close(); 


}//work here is done.
void case_3()
{ 
  
  ifstream comparing;
  comparing.open("BlockChain.txt");
 //The idea-> hash the txt file, if the hashed file equals the hash in BlockChain.txt then there was no change in the plain text
 //and integrity qualified 
 string hashline; 
 string hashedlines[20];
 int i=0;
 
  //lets first get the file name from the end of the hashed line 
  while (!comparing.eof())
 {
  getline(comparing,hashline);
  hashedlines[i]=hashline;
  i++;
   
 }
 comparing.close();
 i=i-1; //Number of hashed lines
 //Let's now absract the files names and save them in an array 
 string filenames[20];
 ofstream tmp;
 tmp.open("tmp.txt",std::ios_base::app);
 for (int j=0 ; j<i ;j++)
 {std::size_t pos1 = hashedlines[j].find(" ");
 filenames[j]=hashedlines[j].substr(7,pos1-1);
 tmp<<filenames[j]<<endl;
 }
 tmp.close();
 int p=0; string name; string sec; char g=')';
 ifstream tmpr;
 tmpr.open("tmp.txt");
 while (!tmpr.eof())
 {
  getline(tmpr,name,g);
  getline(tmpr, sec);
  filenames[p]=name;
  p++;
 }
 p=p-1;
 remove("tmp.txt"); //Done with absracting the name 
//Now, extract the hash lines without the file name 
 for (int j=0 ; j<i ;j++)
 { std::size_t pos = hashedlines[j].find(" ");
   hashedlines[j]=hashedlines[j].substr(pos+1);
   hashedlines[j]=hashedlines[j]+"\n";//the \n because the hash command has \n at the end
 }

//Now to check for the integrity we will compare the hash of a plaintext with its hash in the blockchain and check its integrity
  ifstream content;
  string hashcommand;
  string h;
 for (int m=0;m<i;m++)
 {
  content.open(filenames[m].c_str());
  if (m==0) //first file and first hash line only 
  { hashcommand="openssl dgst -sha512 -hex "+filenames[m]+"";
   h=Stdout(hashcommand);
   std::size_t pos1 = h.find(" ");
   h=h.substr(pos1+1);
   if (h==hashedlines[m])
   {
    //cout<<"everything is OK in "<<filenames[m]<<endl;
   }
  else 
  { printf(filenames[m]);
    break;
  }
  content.close();
  }
 //Now get the last hash add it to the txt file and check 
 else 
  {
  string text;
  string text1[20]; 
  int t=0;
  while (getline(content,text))
  {
   text1[t]=text;
   t++;
  }
  content.close();
  text=text1[0];
  for (int d=1 ;d<t ; d++)   //What i did here, when a file has multiple lines it will put them together in one line
  {text=text+" "+text1[d];
  }
  
  ofstream content_r;
  content_r.open(filenames[m].c_str(), std::ofstream::out | std::ofstream::trunc);
  content_r<<text<<" "<<hashedlines[m-1]; //This is concatenated plain txt + hash 
  content_r.close();
  hashcommand="openssl dgst -sha512 -hex "+filenames[m]+"";
  h=Stdout(hashcommand);
  std::size_t pos2 = h.find(" ");
  h=h.substr(pos2+1);
  
  if (h==hashedlines[m])
  {
    //cout<<"everything is OK in "<<filenames[m]<<endl;
        ofstream last;
        last.open(filenames[m].c_str());
        last<<text;
        last.close();
  }
  else {printf(filenames[m]);
        ofstream last;
        last.open(filenames[m].c_str());
        last<<text;
        last.close();
        break;}
   int c=i;
   if (m==(c-1))
   {
   cout<<"Everything is OK"<<endl;}
   }
   }
   
  
 }//work here is done.
 
 int main (int argc,char *arg[])
{ 
 string plaintextfile;
 string keyfile;
 string encryptedfile;
 int wantedcase;
//atoi is a built in funtion that converts string into integer
//for any other case that 1, 2, 3 
wantedcase=atoi(arg[1]);
 switch (wantedcase)
 { 
   case 1: //encryption function 
   if (argc==4)
   {
   plaintextfile=arg[2];
   keyfile=arg[3];
   case_1 (plaintextfile,keyfile);}
   else cout<<"Invalid number of Parameters"<<endl;
   break; 

   case 2: //decryption function
   if (argc==4){
   encryptedfile=arg[2];
   keyfile=arg[3];
   case_2 (encryptedfile,keyfile);
   }
   else cout<<"Invalid number of Parameters"<<endl;
   break;

   case 3:  
   if (argc==2)
   {
   case_3(); } 
   else cout<<"Invalid number of Parameters"<<endl;
   break; 
   default: cout<<"Not Valid case, choose between 1,2 and 3"<<endl; break;
  }
 return 0;
 }