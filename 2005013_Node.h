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
extern int baseOffsetPrev=0;
extern vector<pair<string,string>> paramsOfFunction;
extern vector<string> argsOfFunction;
extern vector<pair<pair<string,string>,int>> varNameTypeSz;
extern string currentFunction;
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

    void createLabel(ofstream &out){

        out<<"\tL"<<++labelcount<<" :\n";
    }

    void getRecursiveCode(ofstream &out){
        if(this==nullptr)
        return;
       
        if(sInfo!=NULL){
           // cout<<sInfo->getName()<<" "<<sInfo->getInherentType()<<"tptp\n";
            if(sInfo->getInherentType()=="func_def"){
                currentFunction=sInfo->getName();
                paramsOfFunction=sInfo->getParams();
                printFuncDefHeader(out);
            }else if(sInfo->getName()=="LCURL"){
                enterfunction(out);
            }
        }
         //cout<<typeSpecifier<<"tsr\n";
        for(int i=0;i<child.size();i++){
            child[i]->getRecursiveCode(out);
        }
        if(sInfo!=NULL){
             if(sInfo->getInherentType()=="func_def"){
                printFuncDefFooter(out);
             }
        }
    }
    void enterfunction(ofstream &out){
        sTable->enterScope();
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
        //might need to add isarray or not
        paramsOfFunction.clear();


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
         out << ".CODE" << "\n\n" ;
         getRecursiveCode(out);
    }
    void printFuncDefHeader(ofstream &out){  
            
	    out << sInfo->getName() << " PROC\n" ; 
        if(sInfo->getName() == "main"){
            out << "\tMOV AX , @DATA\n\tMOV DS , AX\n" ;  
        }
        string code = "\tPUSH BP\n"
                    "\tMOV BP , SP\n"
                    "\n";

        out << code ; 
        
    }
    void printFuncDefFooter(ofstream &out){
        
         
         createLabel(out);
         string code = "\tMOV SP , BP\n"
                        "\tPOP BP\n\n" ; 
        out << code ; 
           

        if(sInfo->getName() == "main"){ 
           
            out <<	"\tMOV AH , 4CH\n" ; 
            out <<	"\tINT 21H\n" ; 
            out << "MAIN ENDP\n\n" ; 
        } else {
            
            if(sInfo->getParams().size()==0){
                out << "\tRET\n\n";

            }else{
                out << "\tRET " << sInfo->getParams().size()*2 << "\n\n" ; 
            }
            out << sInfo->getName() << " ENDP\n\n" ; 
        }
    }

    void printVarDecl(ofstream &out){
        if(sTable->getCurrentScopeTable()->getID()=="1"){
            
        }else{
            vector<pair<pair<string,string>,int>> vars=sInfo->getVars();
            for(int i=0;i<vars.size();i++){
                
            }


                        
        }

         
    }
};