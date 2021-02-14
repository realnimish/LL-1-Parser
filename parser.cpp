#include <bits/stdc++.h>
using namespace std;

#define TER 0
#define NTER 1

struct token{
	int type;
	int id;
};

vector< vector< vector<token> > > PRODUCTIONS;
vector< set<token> > FIRST;

// START

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
	const int size = PRODUCTIONS.size();

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

		/* CHECK: S -> SA | EPS */

		// Adds E'(intermediate symbol) at the end of valid LHS's
		for(int j=0; j<PRODUCTIONS[i].size(); ++j)
		{
			PRODUCTIONS[i][j].push_back(dash);
		}

		// Stores production of the intermediate symbol
		vector<vector<token>> prod;

		prod.push_back({{0,TER}});	// Adds epsilon rule

		// Updates invalid(left recursive) rules
		while(!invalids.empty())
		{
			vector<token> &tmp = invalids.back();
			invalids.pop_back();

			tmp.erase(tmp.begin());
			tmp.push_back(dash);
			prod.push_back(tmp);
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


						if(FIRST[i].size()!=sz) change = true;

						if(FIRST[tok.id].find(EPSILON) == FIRST[tok.id].end())
						{
							eps=0; break;
						}
					}

				}
				if(eps) FIRST[i].insert(EPSILON);
			}
			++i;
		}

	}while(change);
}

int main()
{
	return 0;
}