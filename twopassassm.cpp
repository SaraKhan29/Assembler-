#include<iostream>
#include<fstream>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
using namespace std;
struct machine_opcode
{
	string mne;
	int size;  
	int opcode;
}mot[14];
struct pseudo_opcode
{
	string pse;
	int size;
}pot[5];
struct Section_tab
{
	string name;
	int size;
}sec;
struct Symbol_tab
{
	string name, type;
	int location, size;
}sym;
struct Reg_op{
	string name;
	int opcode;
}reg[12];
bool is_label(string str)
{
	int i = str.length();
	if (str[i - 1] == ':')
	{
		return true;
	}
	else
	{
		return false;
	}
}
int search_in_POT(string tok1)
{
int i,e;
for(i=0;i<5;i++)
{
	e = strcmp(tok1.c_str(),pot[i].pse.c_str());
	if(e==0)
	{
		return(i);
	}
}
return(-1);
}
int search_in_MOT(string tok1)
{
int i,e;
for(i=0;i<14;i++)
{

	e = strcmp(tok1.c_str(),mot[i].mne.c_str());
	if(e==0)
	{
		
		return(i);
    }	
}
return(-1);
}
int is_reg(string tok2)
{
	int e;
	for(int i=0;i<12;i++)
		{
			int n;
			e=strcmp(tok2.c_str(),reg[i].name.c_str());
			if(e==0)
			{
				return i;
			}
		}
		return(-1);
}
int lc = 0;
char buffer[100],tempbuf[200],tok1[20],tok2[20],tok3[50],tok4[20],tok5[20];

void pass1()
{
	FILE *input,*temp;
	input = fopen("input.txt", "r");
	ofstream section,symbol;
	symbol.open("SYMBOL.txt",ios::out);
	section.open("SECTION.txt",ios::out);
	int n,ch;
	bool yes=false;
	cout<<"\n";
	cout<<"FILES CREATED SYMBOL.TXT AND SECTION.TXT \n";
	if (input==NULL||!symbol.is_open()||!section.is_open())
		cerr << "Error loading files!!!";
	else
	{
		section<<"\tSECTION TABLE\n";
		section<<"*********************\n";
		section<<"\tNAME\tSIZE\n";
		section<<"*********************\n ";

		symbol<<"\t\t\tSYMBOL TABLE\n";
		symbol<<"************************************************************\n";
		symbol<<"\tNAME\tTYPE\t\tLOCATION\tSIZE\n";
		symbol<<"************************************************************\n  ";


	}
	int m,p,j;
	string s,s1;
	char* str;
	int sno=0;
	sec.name="NULL";
	sym.location=0;
	while(fgets(buffer,50,input))
	{
		n=sscanf(buffer,"%s%s%s%s%s",tok1,tok2,tok3,tok4,tok5);
		s=tok2;
		s1=tok1;
		if((m=search_in_MOT(tok1))!=-1)
		{
				ch=1;
		}
		else if((p=search_in_POT(tok2))!=-1)
		{		
			ch=2;
		}
		else if(is_label(tok1))
			ch=3;
		else if(s1=="section")
			ch=4;
		else if(s1=="extern")
			ch=5;
		switch(ch)
		{
			case 1: //machine instruction
					lc+=mot[m].size;
					break;
			case 2: //pseudo opcode
					
					sym.location=lc;
					j=0;
					while(buffer[j]!='\0')
					{
						if(buffer[j]==',')
						{
							lc+=pot[p].size;
						}	
						j++;	
					}
					lc+=pot[p].size;
					sym.name=tok1;
					sym.size=lc-sym.location;
					temp=fopen("SYMBOL.txt","r");
					while(fgets(tempbuf,100,temp))
					{
						sscanf(tempbuf,"%s%s",tok4,tok5);
						if(tok4==tok1)
							yes=true;
					}
					fclose(temp);
					if(!yes)
						symbol<<"\t"<<sym.name<<"\tVariable\t"<<sym.location<<"\t\t"<<sym.size<<" \n  ";
					break;
			case 3:	//label
					sym.name=tok1;
					sym.location=lc;
					symbol<<"\t"<<sym.name<<"\tLabel   \t"<<sym.location<<"  \t\t-  \n  ";		
					break;
			case 4:	//section
						if(sno==0)  //first section
						{
							sno++;
							sec.name=tok2;
							lc=0;	
						}
						else
						{
							sec.size=lc;
							section<<"\t"<<sec.name<<"\t"<<sec.size<<"\n";
							sec.name=tok2;
							lc=0;
							
						}
					break;
			case 5: //extern
					sym.name=tok2;
					symbol<<"\t"<<sym.name<<"\tExternal"<<"\t- "<<"  \t\t- \n  ";		
					break;			
			default:cout<<"not defined call!!!";	
					break;	
		}
	}
	//updating section table for last section
	sec.size=lc;
	section<<"\t"<<sec.name<<" \t"<<sec.size<<"\n ";
	fclose(input);
	symbol.close();
	section.close();
}
void pass2()
{
	FILE *symtab,*input;
	input=fopen("input.txt","r");
	symtab=fopen("SYMBOL.txt","r");
	ofstream object;
	object.open("object.txt",ios::out);
	object<<"****object code****\n";
	lc=0;
	char ch;
	int m,n,p,r;
	char symbol[20],type[20],size[10],loc[10];
	bool flag=false;
	if(input==NULL||symtab==NULL||object==NULL)
	{
		cout<<"error loading files";
		exit(0);
		
	}
	while(fgets(buffer,200,input)!=NULL)
	{
		n=sscanf(buffer,"%s%s%s%s",tok1,tok2,tok3,tok4);
		if((m=search_in_MOT(tok1))!=-1)
		{
			ch=1;
		}
		else if((p=search_in_POT(tok2))!=-1)
		{
			ch=2;
			
		}	
		else if(strcmp(tok1,"section")==0)
		{
			ch=3;
		}
		else if(is_label(tok1))
			ch=4;
		else if(strcmp(tok1,"extern")==0)
			ch=5;
		switch(ch)
		{
			case 1:		object<<lc<<" ";
						if(n==1)
						{
							object<<mot[m].opcode<<" ";
						}
						else if(n==2)
						{
								object<<mot[m].opcode<<" ";
								if((r=is_reg(tok2))!=-1)
								{
									object<<reg[r].opcode<<" ";
								}
								else
								{
									rewind(symtab);
									fgets(tempbuf,200,symtab);
									fgets(tempbuf,200,symtab);
									fgets(tempbuf,200,symtab);
									fgets(tempbuf,200,symtab);
									while(fgets(tempbuf,200,symtab)!=NULL)
									{
										n=sscanf(tempbuf,"%s%s%s%s",symbol,type,loc,size);
										if(strcmp(symbol,tok2)==0)
										{
											flag=true;
											break;
										}
									}
										if(flag==true)
										object<<loc<<" ";
										else 
										cout<<"error";
								}
									
						}
						else if(n==4&&(strcmp(tok3,",")==0))
						{
							object<<mot[m].opcode<<" ";
							if((r=is_reg(tok2))!=-1)
							{
									object<<reg[r].opcode<<" ";
							}
							else
							{
								rewind(symtab);
								fgets(tempbuf,200,symtab);
								fgets(tempbuf,200,symtab);
								fgets(tempbuf,200,symtab);
								fgets(tempbuf,200,symtab);
								while(fgets(tempbuf,200,symtab)!=NULL)
								{
									n=sscanf(tempbuf,"%s%s%s%s",symbol,type,loc,size);
									if(strcmp(symbol,tok2)==0)
									{
										flag=true;
										break;
									}
								}
									if(flag==true)
									object<<loc<<" ";
									else 
									cout<<"error";
							}
								//	cout<<r<<endl;	
							if((r=is_reg(tok4))!=-1)
							object<<reg[r].opcode<<" ";
							else
							{	
								rewind(symtab);
								fgets(tempbuf,200,symtab);
								fgets(tempbuf,200,symtab);
								fgets(tempbuf,200,symtab);
								fgets(tempbuf,200,symtab);
								while(fgets(tempbuf,200,symtab)!=NULL)
								{
									n=sscanf(tempbuf,"%s%s%s%s",symbol,type,loc,size);
									if(strcmp(symbol,tok4)==0)
									{
										flag=true;
										break;
									}
								}
								if(flag==true)
								{
									object<<loc<<" ";
								}
								else 
									cout<<"error";	
							}
								//	cout<<r<<endl;	
									
						}
						object<<endl;	
						lc+=mot[m].size;	
						break;
				case 2: 	rewind(symtab);
							fgets(tempbuf,200,symtab);
							fgets(tempbuf,200,symtab);
							fgets(tempbuf,200,symtab);
							fgets(tempbuf,200,symtab);
							while(fgets(tempbuf,200,symtab)!=NULL)
							{
								n=sscanf(tempbuf,"%s%s%s%s",symbol,type,loc,size);
									//cout<<endl<<symbol<<" "<<type<<" "<<loc<<" "<<size;
								if(strcmp(symbol,tok1)==0)
								{
									flag=true;
									break;
								}
							}
							if(flag==true)
							{
								object<<loc<<" ";
								char *token;
									//cout<<endl<<endl;
									//cout<<tok3;
								token=strtok(tok3,",");
								while(token!=NULL)
								{
									object<<token;
									token=strtok(NULL,",");
								}
								object<<endl;
								lc+=atoi(size);
							}
							break;
				case 3:		lc=0;
							object<<endl;
							break;
				case 4:    	object<<endl;
							break;
				case 5:   	object<<endl;
							break;
				default:	cout<<"not defined call!!!";	
							break;
		}
	}
}
	

int main()
{
	//machine opcode table
	mot[0].mne="MVI";
	mot[0].size=5;
	mot[0].opcode=0;
	mot[1].mne="LOAD";
	mot[1].size=5;
	mot[1].opcode=1;
	mot[2].mne="STORE";
	mot[2].size=5;
	mot[2].opcode=2;
	mot[3].mne="LOADI";
	mot[3].size=1;
	mot[3].opcode=3;
	mot[4].mne="STORI";
	mot[4].size=1;
	mot[4].opcode=4;
	mot[5].mne="ADD";
	mot[5].size=1;
	mot[5].opcode=5;
	mot[6].mne="MOV";
	mot[6].size=1;
	mot[6].opcode=6;
	mot[7].mne="INC";
	mot[7].size=1;
	mot[7].opcode=7;
	mot[8].mne="CMP";
	mot[8].size=5;
	mot[8].opcode=8;
	mot[9].mne="ADDI";
	mot[9].size=5;
	mot[9].opcode=9;
	mot[10].mne="JE";
	mot[10].size=5;
	mot[10].opcode=10;
	mot[11].mne="JMP";
	mot[11].size=5;
	mot[11].opcode=11;
	mot[12].mne="STORE";
	mot[12].size=1;
	mot[12].opcode=12;
	mot[13].mne="STOP";
	mot[13].size=1;
	mot[13].opcode=13;
	cout<<"---MACHINE-INST----\n";
	cout<<"NO\tTYPE\tSIZE\n";
	for(int i=0;i<14;i++)
	{
		cout<<i<<"\t"<<mot[i].mne<<"\t"<<mot[i].size<<"\n";
	}
	//pseudo opcode table
	pot[0].pse="db";
	pot[0].size=1;
	pot[1].pse="dw";
	pot[1].size=2;
	pot[2].pse="dd";
	pot[2].size=4;
	pot[3].pse="RESB";
	pot[3].size=1;
	pot[4].pse="RESW";
	pot[4].size=2;
	cout<<"---PSEUDO_OPCODE TABLE----\n";
	cout<<"NO\tTYPE\tSIZE\n";
	for(int i=0;i<5;i++)
	{
		
		cout<<i<<"\t"<<pot[i].pse<<"\t"<<pot[i].size<<"\n";
	}
	
	reg[0].name="ax";
	reg[0].opcode=14;
	reg[1].name="bx";
	reg[1].opcode=15;
	reg[2].name="cx";
	reg[2].opcode=16;
	reg[3].name="dx";
	reg[3].opcode=17;
	reg[4].name="al";
	reg[4].opcode=1;
	reg[5].name="bl";
	reg[5].opcode=0;
	reg[6].name="cl";
	reg[6].opcode=3;
	reg[7].name="dl";
	reg[7].opcode=4;
	reg[8].name="ah";
	reg[8].opcode=5;
	reg[9].name="bh";
	reg[9].opcode=6;
	reg[10].name="ch";
	reg[10].opcode=7;
	reg[11].name="dh";
	reg[11].opcode=8;
	
	
	pass1();	
	pass2();
	return 0;
}
