#ifndef SB_CS_POINT_SIMUL_H
#define SB_CS_POINT_SIMUL_H

#include <mono/jit/jit.h>
#include "../../scenarios/pointSimul/pointSimul.h"

using namespace pointSimul;

namespace cs {

	namespace pointSimul
	{
		// common util methods
		void definePointCallbacks();
		void defineMouseCallbacks();
		MonoObject* createPoint(MonoAssembly* assembly, MonoDomain* domain, int pointIndex);
		void setMonoPoint(MonoObject* mp, point* p);
		void setPointFromMono(MonoObject* mp, point* p);
		point* extractPoint(MonoObject* pointObject);
		MonoObject* createMouse(MonoAssembly* assembly, MonoDomain* domain);

		// scenario methods
		int allNative(int c, int p);
		int allScript(int c, int p);
		int allScriptStruct(int c, int p);
		int allScriptMinCallback(int c, int p);
		int allScriptMinCallbackStruct(int c, int p);
		int scriptToNative(int c, int p);
		int scriptToNativeStruct(int c, int p);

		static const ::pointSimul::CallbackMap versionMap[] = {
			{ "allNative", allNative },
			{ "allScript", allScript },
			{ "allScriptStruct", allScriptStruct },
			{ "allScriptMinCallback", allScriptMinCallback },
			{ "allScriptMinCallbackStruct", allScriptMinCallbackStruct },
			{ "scriptToNative", scriptToNative },
			{ "scriptToNativeStruct", scriptToNativeStruct }
		};
	}
}

#endif