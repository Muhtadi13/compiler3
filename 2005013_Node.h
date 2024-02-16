#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include "2005013_SymbolTable.h"

using namespace std;
extern bool globalScope;
extern int labelcount;
extern SymbolTable *sTable;
extern vector<pair<pair<string,string>,int>> varNameTypeSz;
extern int baseOffset;
extern int baseOffsetPrev;
//extern vector<pair<string,string>> paramsOfFunction;
extern vector<string> argsOfFunction;
//extern vector<pair<pair<string,string>,int>> varNameTypeSz;
//extern string currentFunction;
class Node{

    private:
        int startLocation;
        int endLocation;
        vector<Node *> child;
        string typeSpecifier;
        string grammar;
        string name;
        bool zero;
        bool terminal;
        SymbolInfo *sInfo;
        string truelist;
        string falselist;
        string nextlist;
        bool evaluation;


    public:

    Node(){
        startLocation=-1;
        endLocation=-1;
        child=vector<Node *>();
        typeSpecifier="";
        grammar="";
        name="";
        zero=false;
        terminal=false;
        sInfo=NULL;
        truelist="";
        falselist="";
        nextlist="";
        evaluation=false;
    }
    
    void setSymbolInfo(SymbolInfo *s){
        sInfo=s;
    } 
    SymbolInfo* getSymbolInfo(){
        return sInfo;
    } 
    void setName(string s){
        name=s;
    } 
    string getName(){
        return name;
    } 
    int getStart(){
        return startLocation;
    }
    int getEnd(){
        return endLocation;
    }
    vector<Node *> getChild(){
        return child;
    }
    string getGrammar(){
        return grammar;
    }
    string getTypeSpecifier(){
        return typeSpecifier;
    }
    bool isZero(){
        return zero;
    }
    bool isTerminal(){
        return terminal;
    }

    void setTerminal(){
        terminal=true;
    }

    void setZero(){
        zero=true;
    }
    void setStart(int st){
        startLocation=st;
    }
    void setEnd(int en){
        endLocation=en;
    }
    void addChild(Node *node){
        child.push_back(node);
    }

    void setGrammar(string g){
        grammar=g;
    }
    void setTypeSpecifier(string tsf){
        typeSpecifier=tsf;
    }
    void print(int gap,ofstream &out){

        if(this==nullptr){
            return;
        }
        for(int i=0;i<gap;i++){
            out<<" ";
        }
        out<<grammar << " 	<Line: " <<startLocation << "-" << endLocation<< ">\n";
        for(int i=0;i<child.size();i++){
           child[i]-> print(gap+1,out);
        }
    } 

    string createLabel(){
        return "L"+to_string(++labelcount)+" :";
    }

    void getRecursiveCode(ofstream &out){
        if(this==nullptr)
        return;
       
        if(typeSpecifier=="func_definition"){
            SymbolInfo* currentFunc=child[1]->getSymbolInfo();
            string nam=currentFunc->getName();
            vector<pair<string,string>>params = currentFunc->getParams();
            printFuncDefHeader(out,nam);
            enterfunction(out,params);
            for(int i=0;i<child.size();i++){
                child[i]->getRecursiveCode(out);
            }

            SymbolInfo* currentFunc=child[1]->getSymbolInfo();
            string nam=currentFunc->getName();
            int paramSz = currentFunc->getParams().size();
            printFuncDefFooter(out,nam,paramSz);
            sTable->exitScope();
        }else if(typeSpecifier=="var_declaration"){
            for(int i=0;i<child.size();i++){
                child[i]->getRecursiveCode(out);
            }
            string type=child[0]->getTypeSpecifier();
            vector<pair<pair<string,string>,int>> varNameTypeSz=child[1]->getSymbolInfo()->getVars();
            printVarDecl(out,type,varNameTypeSz);
        }else if(typeSpecifier=="compound_statement"){
            nextlist=createLabel();
            if(child.size()==3){
                //cout<<"chichi\n";
                child[1]->nextlist=this->nextlist;
            }
            for(int i=0;i<child.size();i++){
                child[i]->getRecursiveCode(out);
            }
        }else if(typeSpecifier=="statements"){
            if(child.size()==1){
                child[0].nextlist=this->nextlist;
            }else if(child.size()==2){
                child[0].nextlist=createLabel();
                child[1].nextlist=this->nextlist;
            }
            for(int i=0;i<child.size();i++){
                child[i]->getRecursiveCode(out);
            }
            out<<nextlist<<"\n";
        }else if(typeSpecifier=="statement"){
            if(child.size()==1){
                child[0]->getRecursiveCode(out);
            }else{
                if(child[0]->getSymbolInfo()->getType()=="FOR"){
                    for(int i=0;i<3;i++){
                        child[i]->getRecursiveCode(out);
                    }//initializing

                    string loopstart=createLabel();
                    out<<loopstart<<"\n"; //starting loop
                    
                    child[3]->evaluation=true; //assuming it is true
                    child[3]->truelist=createLabel();
                    child[3]->falselist=this->nextlist;

                    child[6]->nextlist=createLabel();

                    child[3]->getRecursiveCode(out);
                    
                    out<<child[3]->truelist<<"\n";
                    child[6]->getRecursiveCode(out);

                    //not giving any label here cause statement has a label of its own

                    child[4]->getRecursiveCode(out);
                    out<<"\n\tJMP "<<loopstart<<"\n";
 
                }

            }if(typeSpecifier=="expression_statement"){
                
            }

        }
        
        
        else{
            for(int i=0;i<child.size();i++){
                child[i]->getRecursiveCode(out);
            }
        }

           



    }
    void enterfunction(ofstream &out,vector<pair<string,string>> paramsOfFunction){
        
        sTable->enterScope();
        //cout<<sTable->getCurrentScopeTable()->getID()<<"ididid\n";
        sTable->getCurrentScopeTable()->setBaseOffset(baseOffset);
        int tmpoffset=paramsOfFunction.size()*2+2;
        tmpoffset=-tmpoffset;
        for(int i=0;i<paramsOfFunction.size();i++){
                SymbolInfo *sinfo=new SymbolInfo();
				sinfo->setName(paramsOfFunction[i].first);
				sinfo->setType(paramsOfFunction[i].second);
				sinfo->setInherentType("variable");
                sinfo->setDistanceFromTop(tmpoffset);
                tmpoffset+=2;
				sTable->Insert(sinfo);	
        }
        //might need to add isarray or not for params
        //paramsOfFunction.clear();


    }
    void printGlobalVars(ofstream &out){
        ScopeTable *scope=sTable->getCurrentScopeTable();
        SymbolInfo *stemp;
        SymbolInfo** headList=scope->getHead();
        for (int i = 0; i < scope->getTableSize(); i++)
        {
            if(headList[i]==NULL)
            continue;
            //out<<"\t"<<i+1;
            
            stemp = headList[i];
            while (stemp != NULL)
            {
                
                if(stemp->getInherentType()=="variable"){
                    if(stemp->getArraySize()>-1){
                        continue;
                    }else{
                        out <<" DW "<<stemp->getName() << " ";
                        out<<stemp->getType()<<"\n";
                    }
                }
                stemp->setDistanceFromTop(-100);
                stemp = stemp->getNextSymbolPointer();
            }
        }


    }

    void printAssemblyCode(ofstream &out){
         out<<".MODEL SMALL\n.STACK 100H\n\n.DATA\n";
        printGlobalVars(out);
         out << ".CODE" << "\n" ;
         getRecursiveCode(out);
    }

    void printFuncDefHeader(ofstream &out,string nam){ 

         
	    out<<"\n" << nam << " PROC\n" ; 
        if(nam == "main"){
            out << "\n\tMOV AX , @DATA\n\tMOV DS , AX\n" ;  
        }
        string code = "\n\tPUSH BP\n"
                    "\tMOV BP , SP\n";

        out << code ; 
        
    }
    void printFuncDefFooter(ofstream &out,string nam,int sz){
        
          
         out<<createLabel()<<"\n";
         string code = "\n\tMOV SP , BP\n"
                        "\tPOP BP\n" ; 
        out << code ; 
           

        if(nam == "main"){ 
           
            out <<	"\n\tMOV AH , 4CH\n" ; 
            out <<	"\tINT 21H\n" ; 
            out << "\nMAIN ENDP\n" ; 
        } else {
            
            if(sz==0){
                out << "\tRET\n";

            }else{
                out << "\tRET " << sz*2 << "\n" ; 
            }
            out<<"\n" << nam << " ENDP\n" ; 
        }
       
    }

    void printVarDecl(ofstream &out,string type,vector<pair<pair<string,string>,int>> varNameTypeSz){
        if(sTable->getCurrentScopeTable()->getID()=="1"){
            
        }else{
            cout<<"last\n";
            int tot=0;
            for(int i=0;i<varNameTypeSz.size();i++){
                    SymbolInfo *sinfo=new SymbolInfo();
                    sinfo->setName(varNameTypeSz[i].first.first);
                    sinfo->setType(type);
                    sinfo->setInherentType("variable");
                    sinfo->setArraySize(varNameTypeSz[i].second);
                    sinfo->setRettypeOrArrayType(type);
                    sinfo->setDistanceFromTop(baseOffset+2);
                    if(varNameTypeSz[i].second>-1){
                        tot+=varNameTypeSz[i].second;
                        baseOffset+=varNameTypeSz[i].second*2;
                    }else{
                        tot++;
                        baseOffset+=2;
                    }
                    SymbolInfo* cmp=sTable->getCurrentScopeTable()->Lookup(sinfo->getName());
                    if(!cmp)
                    sTable->Insert(sinfo);
            }
            if(tot>0)
            out<<"\n\tSUB SP , "<<tot*2<<"\n";
		                
        }
    }
};
