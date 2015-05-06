#include "scenarios/scenarios.h"
#include "lua/lua.h"
#include "js/js.h"
#include "cs/cs.h"
#include <stdio.h>
#include <string.h>
#include "Windows.h"

int printLanguages() 
{
	printf("First argument has to be target scripting language, you can pick from the following:\nlua js cs\n");

	return 1;
}

int printScenarios(const CallbackMap* scenarioMap, int scenarioMapSize, char* language)
{
	printf("For \"%s\" you can select one of the following scenarios:\n", language);
	for (int i = 0; i < scenarioMapSize; i++)
	{
		printf("%s ", scenarioMap[i].name);
	}
	printf("\n");

	return 1;
}



int processInput(int argc, char** argv) 
{
	if (argc <= 1) {
		return printLanguages();
	}

	const CallbackMap* scenarioMap;
	int scenarioMapSize;
	if (strcmp(argv[1],"lua") == 0) 
	{
		scenarioMap = lua::scenarioMap;
		scenarioMapSize = sizeof(lua::scenarioMap) / sizeof(CallbackMap);
	}
	else if (strcmp(argv[1], "js") == 0) 
	{
		scenarioMap = js::scenarioMap;
		scenarioMapSize = sizeof(js::scenarioMap) / sizeof(CallbackMap);
	}
	else if (strcmp(argv[1], "cs") == 0) 
	{
		scenarioMap = cs::scenarioMap;
		scenarioMapSize = sizeof(cs::scenarioMap) / sizeof(CallbackMap);
	}
	else
	{
		return printLanguages();
	}

	if (argc <= 2)
	{
		return printScenarios(scenarioMap, scenarioMapSize, argv[1]);
	}

	for (int i = 0; i < scenarioMapSize; i++)
	{
		if (strcmp(scenarioMap[i].name, argv[2]) == 0)
		{
			return scenarioMap[i].func(argc - 3, argv + 3);
		}
	}

	return printScenarios(scenarioMap, scenarioMapSize, argv[1]);
}

int main(int argc, char** argv)
{
	// TODO: this is just for windows to see results
	int result = processInput(argc, argv);
	system("pause");
	return result;
}