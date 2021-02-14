#include <bits/stdc++.h>
using namespace std;

struct token{
	int id;
	bool type;
};

string START;
int TERMINALS_COUNT , NONTERMINALS_COUNT;
vector<pair<string,string>> INPUT;

unordered_map<string,token> TOKENS;
vector< vector< vector<token> > > PRODUCTIONS;
vector< set<token> > FIRST,FOLLOW;
vector< vector<int> > PARSETABLE;

string getToken(token);

/* NIMISH START */

#define TER 1
#define NTER 0

const token EPSILON{0,TER};

inline bool operator==(const token &A, const token &B)
{
	return (A.id==B.id && A.type==B.type);
}

inline bool operator<(const token &A, const token &B)
{
	if(A.type < B.type) return 1;
	return (A.id < B.id);
}

inline bool _isLeftRecursive(int id, token &tok)
{
	return (tok.type==NTER && tok.id==id);
}

void removeLeftRecursion()
{
	int size = PRODUCTIONS.size();
	for(int i=0; i<size; ++i)
	{
		// Stores LHS which contains left recursion
		vector<vector<token>> invalids;

		for(int j=PRODUCTIONS[i].size()-1; j>=0; --j)
		{
			if(_isLeftRecursive(i,PRODUCTIONS[i][j][0]))
			{
				invalids.push_back(PRODUCTIONS[i][j]);
				PRODUCTIONS[i].erase(PRODUCTIONS[i].begin()+j);
			}
		}

		if(invalids.empty()) continue;
		
		// Creating intermediate non-terminal
		token dash; dash.type=NTER; dash.id = PRODUCTIONS.size();

		string symbol = getToken({i,NTER}) + "'";
		TOKENS[symbol] = dash;
		NONTERMINALS_COUNT++;

		/* CHECK: S -> SA | EPS */

		// Adds E'(intermediate symbol) at the end of valid LHS's
		for(int j=0; j<PRODUCTIONS[i].size(); ++j)
		{
			PRODUCTIONS[i][j].push_back(dash);
		}

		// Stores production of the intermediate symbol
		vector<vector<token>> prod;
		prod.push_back({EPSILON});	// Adds epsilon rule

		// Updates invalid(left recursive) rules
		while(!invalids.empty())
		{
			vector<token> &tmp = invalids.back();
			
			tmp.erase(tmp.begin());
			tmp.push_back(dash);
			prod.push_back(tmp);

			invalids.pop_back();
		}

		PRODUCTIONS.push_back(prod);
	}
}

void findFIRST()
{
	FIRST.resize(PRODUCTIONS.size());
	bool change;

	do{
		change = false;
		int i=0;
		for(auto &prod : PRODUCTIONS)
		{
			for(auto &lhs : prod)
			{
				bool eps = true;
				for(auto &tok : lhs)
				{
					if(tok==EPSILON) continue;
					if(tok.type==TER)
					{
						int sz = FIRST[i].size();
						FIRST[i].insert(tok);
						if(FIRST[i].size()!=sz) change = true;
						eps=0; break;
					}
					else
					{
						int sz = FIRST[i].size();
						// Add First(NTER) to FIRST(i)
						FIRST[i].insert(FIRST[tok.id].begin(),FIRST[tok.id].end());

						FIRST[i].erase(EPSILON);

						if(FIRST[i].size()!=sz) change = true;

						if(FIRST[tok.id].find(EPSILON) == FIRST[tok.id].end())
						{
							eps=0; break;
						}
					}

				}
				int sz = FIRST[i].size();
				if(eps) FIRST[i].insert(EPSILON);
				if(FIRST[i].size()!=sz) change = true;
			}
			++i;
		}

	}while(change);
}

void findFOLLOW()
{
	const token dollar{TOKENS["$"].id,TER};

	FOLLOW.resize(PRODUCTIONS.size());
	FOLLOW[TOKENS[START].id].insert(dollar);

	bool change;
	do{
		change = false;

		for(int i=0; i<PRODUCTIONS.size(); ++i)
		{
			for(auto &lhs : PRODUCTIONS[i])
			{
				if(lhs.back().type==NTER)
				{
					FOLLOW[lhs.back().id].insert(FOLLOW[i].begin(), FOLLOW[i].end());
				}

				for(int j=lhs.size()-2; j>=0; --j)
				{
					if(lhs[j].type==TER) continue;
					int sz = FOLLOW[lhs[j].id].size();

					if(lhs[j+1].type==TER) FOLLOW[lhs[j].id].insert(lhs[j+1]);
					else{
						FOLLOW[lhs[j].id].insert(FIRST[lhs[j+1].id].begin(), FIRST[lhs[j+1].id].end());

						if(FOLLOW[lhs[j].id].find(EPSILON)!=FOLLOW[lhs[j].id].end())
						{
							FOLLOW[lhs[j].id].insert(FOLLOW[lhs[j+1].id].begin(), FOLLOW[lhs[j+1].id].end());
						}
					}
					FOLLOW[lhs[j].id].erase(EPSILON);
					if(FOLLOW[lhs[j].id].size() != sz) change = true;
				}
			}
		}

	}while(change);
}

void findPARSETABLE()
{

}

/* NIMISH END */

/* SAYAN START */

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
    cout<<"token | id | type\n\n";
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
        // cout<<"Prod "<<i<<":"<<PRODUCTIONS[i].size()<<"\n";
        for(int j=0;j<PRODUCTIONS[i].size();j++)
        {
            // cout<<"RULE "<<j<<" : ";
            cout << getToken({i,0}) << " : ";
            for(int k=0;k<PRODUCTIONS[i][j].size();k++)
            {
                // cout<<"{"<<PRODUCTIONS[i][j][k].id<<","<<PRODUCTIONS[i][j][k].type<<"} , ";
                cout << getToken(PRODUCTIONS[i][j][k]) << " ";
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

    // displayTokens();
    // displayProductions(); 

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

string getToken(token tkn)
{
    for(auto &i:TOKENS)
    {
        if(i.second.id==tkn.id&&i.second.type==tkn.type)
        {
            return i.first;
        }
    }

    return "";
}

void displayFIR_FOL(vector<set<token>> &T, string name)
{
	cout << name << ":\n\n";
    int i=0;
    for(auto &itr: T)
    {   cout<<getToken({i,0})<<" : ";
        for(auto &itr_tokn: itr)
        {
            cout<<getToken(itr_tokn)<<" , ";
        }
        i++;
        cout << endl;
    }
}

/* SAYAN END */

int main()
{
	getProductions();
	removeLeftRecursion();
	displayProductions();
	findFIRST();
	displayFIR_FOL(FIRST,"FIRST");
	findFOLLOW();
	displayFIR_FOL(FOLLOW,"FOLLOW");
	// findPARSETABLE();
	// readTokenizedInput();
	// parseInput();
	return 0;
}