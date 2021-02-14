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
vector<int> NULLABLE;

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

int _add(int i,int j,const token &tok)
{
	if(FIRST[i].insert(tok).second==true)
	{
		if(PARSETABLE[i][tok.id]!=-1) return -1;
		PARSETABLE[i][tok.id] = j;
		return 1;
	}
	return 0;
}


int findFIRST()
{
	FIRST.resize(PRODUCTIONS.size());
	NULLABLE.resize(PRODUCTIONS.size(),-1);
	PARSETABLE.resize(NONTERMINALS_COUNT, vector<int>(TERMINALS_COUNT,-1));
	int change;

	do{
		change = 0;
		for(int i=0; i<PRODUCTIONS.size(); ++i)
		{
			for(int j=0; j<PRODUCTIONS[i].size(); ++j)
			{
				bool eps = false;
				for(auto &tok : PRODUCTIONS[i][j])
				{
					if(tok==EPSILON)
						{
							eps=true;
							continue;
						}
					if(tok.type==TER)
					{
						int res = _add(i,j,tok);
						if(res<0) return 0;
						change |= res;
						eps=false;
					}
					else
					{
						// Add First(NTER) to FIRST(i)
						for(auto &tok1 : FIRST[tok.id])
						{
							if(tok1==EPSILON) eps=true;
							else{
								int res = _add(i,j,tok1);
								if(res<0) return 0;
								change |= res;
							}
						}
					}
					if(!eps) break;
				}
				if(eps){
					change |= FIRST[i].insert(EPSILON).second;
					NULLABLE[i] = j;
				}
			}
		}
	}while(change);
	return 1;
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

int findPARSETABLE()
{
	if(findFIRST()==0) return 0;
	findFOLLOW();

	for(int i=0; i<PRODUCTIONS.size(); ++i)
	{
		if(NULLABLE[i]<0) continue;
		for(auto &tok : FOLLOW[i])
		{
			if(PARSETABLE[i][tok.id]!=-1) return 0;
			PARSETABLE[i][tok.id] = NULLABLE[i];
		}
	}
	return 1;
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

int parseInput()
{
    stack<token> Stack;

    Stack.push(TOKENS["$"]);
    Stack.push(TOKENS[START]);

    for(int i=0;i<INPUT.size();i++)
    {
        token x = TOKENS[INPUT[i].first];

        if(Stack.empty()) return 0;
        while(!(x == Stack.top()))
        {
            if(Stack.top()==EPSILON) {
            	Stack.pop();
            	continue;
            }

            if(Stack.top().type==TER) return 0;

            if(PARSETABLE[Stack.top().id][x.id]!=-1)
            {   vector<token> matchedRule = PRODUCTIONS[Stack.top().id][PARSETABLE[Stack.top().id][x.id]];
                Stack.pop();
                for(int i=matchedRule.size()-1;i>=0;i--)
                    Stack.push(matchedRule[i]); 
            }
            else return 0;
        }
        Stack.pop();

    }
    return Stack.empty();
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

void displayTokens()
{
    cout<<"token | id | type\n\n";
    for(auto i=TOKENS.begin();i!=TOKENS.end();i++)
    {
        cout<<i->first<<" "<<i->second.id<<" "<<i->second.type<<endl;
    }
}

void displayProductions()
{
    cout<<"\nProd Rules:\n\n";
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

void displayFIR_FOL(vector<set<token>> &T, string name)
{
	cout << endl << name << ":\n\n";
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

void printSpace(int x)
{
    while(x--)
        cout<<" ";
}

void displayParseTable()
{
	cout << endl << "Parse Table:\n\n";

    int nos=5;
    string p;
    printSpace(nos);
    for(int i=1;i<TERMINALS_COUNT;i++)
    {    
        p=getToken({i,1});
        cout<< p; printSpace(nos-p.length());
    }
    cout<<endl<<endl;
    for(int i=0;i<PARSETABLE.size();i++)
    {   
        p=getToken({i,0});
        cout<<p;printSpace(nos-p.length());
        for(int j=1;j<PARSETABLE[i].size();j++)
        {
            int s=0;
            for(int k=0;PARSETABLE[i][j]>=0 && k<PRODUCTIONS[i][PARSETABLE[i][j]].size();k++)
            {
                p=getToken(PRODUCTIONS[i][PARSETABLE[i][j]][k]);
                cout<<p;
                s+=p.length();

            }
            if(s==0)
            {
            	cout << "_";
            	s++;
            }
            printSpace(nos-s);
        }
        cout<<endl<<endl;
    }
}

void displayInput()
{
    for(int i=0;i<INPUT.size();i++)
    {
        cout<<"Token: "<<INPUT[i].first<<" Value: "<<INPUT[i].second<<endl;
    }
}


/* SAYAN END */

int main()
{
	getProductions();
	removeLeftRecursion();
	displayProductions();
	if(findPARSETABLE()==0)
	{
		cout << "Ambiguous Grammar!!\n";
		return 0;
	}

	displayFIR_FOL(FIRST,"FIRST");
	displayFIR_FOL(FOLLOW,"FOLLOW");
	displayParseTable();
	readTokenizedInput();
	cout << (parseInput()?"ACCEPTED":"REJECTED") << endl;
	return 0;
}