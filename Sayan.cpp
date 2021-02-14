#include<bits/stdc++.h>
using namespace std;

struct token
{
    int id;
    bool type;
    //True for Terminals & False fro NonTerminals
};

int TERMINALS_COUNT , NONTERMINALS_COUNT;
unordered_map<string,token>  TOKENS;
vector<vector<vector<token>>> PRODUCTIONS;
vector<set<token>> FIRST, FOLLOW;
vector<pair<string,string>> INPUT;
vector<vector<int>> PARSETABLE;
string START;

int readTokenType(fstream &file,bool type)
{
    string input;
    int count=0;
    if(type)
    {
        token tmp={ count++,type};
        TOKENS.insert({"EPS",tmp});
        
    }
    
    while(file>>input)
    {
        if(input=="$$")
            break;
        token tmp={ count++,type};
        TOKENS.insert({input,tmp});
    }
    if(type)
    {   
        token tmp = { count++, type};
        TOKENS.insert({"$",tmp});
    }
    return count;
}

void readRules(fstream &file)
{
    string input;
    int lhsId,c=1;
    while(file >> input)
    {
        if(c)
            START=input,c=0;
        auto itr = TOKENS.find(input);
        lhsId = itr->second.id;
        vector<token> rule;
        file>>input;
        while(file>>input)
        {
            if(input=="$$")
                break;
            if(input != "EPS")
            {   
                auto x= TOKENS.find(input);
                rule.push_back(x->second);
            }
        }
        if(rule.size()==0)
        {
            auto x= TOKENS.find("EPS");
            rule.push_back(x->second);
        }
        vector<vector<token>> rules;
        rules.push_back(rule);
        PRODUCTIONS[lhsId].push_back(rule);
        
    }
}

void  displayTokens()
{
    cout<<"token : id : type\n\n";
    for(auto i=TOKENS.begin();i!=TOKENS.end();i++)
    {
        cout<<i->first<<" "<<i->second.id<<" "<<i->second.type<<endl;
    }
}

void displayProductions()
{
    cout<<"Prod Rules:\n\n";
    for(int i=0;i<PRODUCTIONS.size();i++)
    {   
        cout<<"Prod "<<i<<":"<<PRODUCTIONS[i].size()<<"\n";
        for(int j=0;j<PRODUCTIONS[i].size();j++)
        {
            cout<<"RULE "<<j<<" : ";
            for(int k=0;k<PRODUCTIONS[i][j].size();k++)
            {
                cout<<"{"<<PRODUCTIONS[i][j][k].id<<","<<PRODUCTIONS[i][j][k].type<<"} , ";
            }
            cout<<"\n";
        }
        cout<<endl;
    }
}

void getProductions()
{
    string input,filename="rules.txt";
    fstream readFile;
    readFile.open(filename.c_str());
    
    //Reads Terminals
    TERMINALS_COUNT = readTokenType(readFile,true);

    //Reads Nonterminals
    NONTERMINALS_COUNT =  readTokenType(readFile,false);
    
    PRODUCTIONS.resize(NONTERMINALS_COUNT);
    //Reads Production rules
    readRules(readFile);

    displayTokens();
    displayProductions(); 

}

void readTokenizedInput()
{
    string input,filename="input.txt";
    fstream readFile;
    readFile.open(filename.c_str()); 
    while(readFile >> input)
    {
       pair<string,string> tmp;
       tmp.first=input;
       readFile>>input;
       tmp.second=input;
       INPUT.push_back(tmp);
    }  
    pair<string,string> tmp;
    tmp.first="$";
    tmp.second="$";
    INPUT.push_back(tmp);
}

void parseInput()
{
    stack<token> Stack;

    Stack.push(TOKENS.find("$")->second);
    Stack.push(TOKENS.find(START)->second);
    for(int i=0;i<INPUT.size();i++)
    {
        auto x = TOKENS.find(INPUT[i].first);
        if(Stack.empty())
        {
            cout<<"UNKNOWN ERROR\n";
            return;
        }
        while(x->second.id != Stack.top().id || x->second.type != Stack.top().type)
        {
            //To be decieded
            vector<token> matchedRule;
            if(PARSETABLE[Stack.top().id][x->second.id]!=-1)
            {   vector<token> matchedRule = PRODUCTIONS[Stack.top().id][PARSETABLE[Stack.top().id][x->second.id]];
                Stack.pop();
                for(int i=matchedRule.size()-1;i>=0;i--)
                    Stack.push(matchedRule[i]); 
            }
            else{
                cout<<"REJECTED\n";
                return;
            }
        }
        Stack.pop();

    }
    if(Stack.empty())
    {
        cout<<"Accepted\n";
    }
    else{
        cout<<"Rejected\n";
    }
}

int main()
{
    
    getProductions();
    return 0;
}
