// RecipeNSMgr.h: interface for the CRecipe413NSMgr class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#pragma warning( disable : 4701 4100 4244 4189 4245 4310 4786 )

#include <string>
#include <vector>
using namespace std;

class CRecipe413;

// Disk base recipe manager [10/31/2002]
class CRecipe413NSMgr {
public:
	BOOL IsRecipeAvailable(string& rcp);
	BOOL DeleteAllRecipes();
	BOOL DeleteRecipe(string& name);
	BOOL DeleteRecipes(vector <string>& RList);
	BOOL CanDeleteRecipes(vector <string>& RList);
	BOOL Load(CRecipe413* Rcp);
	BOOL Save(CRecipe413* Rcp);
	BOOL ListRecipes(vector <string>& RList);
	void CreateRecipeDirectory();
	CRecipe413NSMgr();
	virtual ~CRecipe413NSMgr();
};

extern CRecipe413NSMgr RcpMgr;
