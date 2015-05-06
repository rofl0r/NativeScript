#include "../../scenarios/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

using namespace pointSimul;

namespace cs {

	// entry point
	int runPointSimul(int c, char** v)
	{
		static const CallbackMap versionMap[] = {
			{ "allScript", &pointSimul::allScript }
		};

		return processConfig(c, v, versionMap, sizeof(versionMap) / sizeof(CallbackMap));
	}

	namespace pointSimul {

		// callbacks

		void stepPoint(MonoObject* point) {
			stepPoint(extractPoint(point));
		}

		// wrapper functions
		__declspec(dllexport) void setPointX(MonoObject* that, double val) {
			extractPoint(that)->x = val;
		}

		__declspec(dllexport) double getPointX(MonoObject* that) {
			return extractPoint(that)->x;
		}

		__declspec(dllexport) void setPointY(MonoObject* that, double val) {
			extractPoint(that)->y = val;
		}

		__declspec(dllexport) double getPointY(MonoObject* that) {
			return extractPoint(that)->y;
		}

		__declspec(dllexport) void setVelocityX(MonoObject* that, double val) {
			extractPoint(that)->xVelocity = val;
		}

		__declspec(dllexport) double getVelocityX(MonoObject* that) {
			return extractPoint(that)->xVelocity;
		}

		__declspec(dllexport) void setVelocityY(MonoObject* that, double val) {
			extractPoint(that)->yVelocity = val;
		}

		__declspec(dllexport) double getVelocityY(MonoObject* that) {
			return extractPoint(that)->yVelocity;
		}
		
		__declspec(dllexport) double getMouseX(MonoObject* that) {
			return getMouse()->x;
		}

		__declspec(dllexport) double getMouseY(MonoObject* that) {
			return getMouse()->y;
		}

		void definePointCallbacks()
		{
			mono_add_internal_call("Point::setX(double)", setPointX);
			mono_add_internal_call("Point::getX()", getPointX);
			mono_add_internal_call("Point::setY(double)", setPointY);
			mono_add_internal_call("Point::getY()", getPointY);
			mono_add_internal_call("Point::setVelocityX(double)", setVelocityX);
			mono_add_internal_call("Point::getVelocityX()", getVelocityX);
			mono_add_internal_call("Point::setVelocityY(double)", setVelocityY);
			mono_add_internal_call("Point::getVelocityY()", getVelocityY);
		}

		void defineMouseCallbacks()
		{
			mono_add_internal_call("Mouse::getX()", getMouseX);
			mono_add_internal_call("Mouse::getY()", getMouseY);
		}

		MonoObject* createPoint(MonoAssembly* assembly, MonoDomain* domain, point* p)
		{
			MonoImage *image = mono_assembly_get_image(assembly);
			MonoClass *pointClass = mono_class_from_name(image, "", "Point");
			MonoObject *csPoint = mono_object_new(domain, pointClass);
			mono_runtime_object_init(csPoint);
			MonoClassField* userData = mono_class_get_field_from_name(pointClass, "user_data");
			mono_field_set_value(csPoint, userData, &p);
			return csPoint;
		}

		point* extractPoint(MonoObject* pointObject)
		{
			point* p;
			MonoClass *pointClass = mono_object_get_class(pointObject);
			MonoClassField* userData = mono_class_get_field_from_name(pointClass, "user_data");
			mono_field_get_value(pointObject, userData, &p);
			return p;
		}

		MonoObject* createMouse(MonoAssembly* assembly, MonoDomain* domain)
		{
			MonoImage *image = mono_assembly_get_image(assembly);
			MonoClass *mouseClass = mono_class_from_name(image, "", "Mouse");
			MonoObject *csMouse = mono_object_new(domain, mouseClass);
			mono_runtime_object_init(csMouse);
			return csMouse;
		}
	}
}
