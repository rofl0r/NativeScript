#include "../../scenario/pointSimul/pointSimul.h"
#include "cs_pointSimul.h"
#include "../cs.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

using namespace pointSimul;

namespace cs {

	// entry point
	int runPointSimul(int c, char** v)
	{
		return processConfig(c, v, pointSimul::versionMap, sizeof(pointSimul::versionMap) / sizeof(::pointSimul::CallbackMap));
	}

	namespace pointSimul {
		
		// wrapper functions
		SS_CS_CALLBACK_EXPORT void setPointX(MonoObject* that, double val) {
			extractPoint(that)->x = val;
		}

		SS_CS_CALLBACK_EXPORT double getPointX(MonoObject* that) {
			return extractPoint(that)->x;
		}

		SS_CS_CALLBACK_EXPORT void setPointY(MonoObject* that, double val) {
			extractPoint(that)->y = val;
		}

		SS_CS_CALLBACK_EXPORT double getPointY(MonoObject* that) {
			return extractPoint(that)->y;
		}

		SS_CS_CALLBACK_EXPORT void setVelocityX(MonoObject* that, double val) {
			extractPoint(that)->xVelocity = val;
		}

		SS_CS_CALLBACK_EXPORT double getVelocityX(MonoObject* that) {
			return extractPoint(that)->xVelocity;
		}

		SS_CS_CALLBACK_EXPORT void setVelocityY(MonoObject* that, double val) {
			extractPoint(that)->yVelocity = val;
		}

		SS_CS_CALLBACK_EXPORT double getVelocityY(MonoObject* that) {
			return extractPoint(that)->yVelocity;
		}
		
		SS_CS_CALLBACK_EXPORT double getMouseX(MonoObject* that) {
			return getMouse()->x;
		}

		SS_CS_CALLBACK_EXPORT double getMouseY(MonoObject* that) {
			return getMouse()->y;
		}

		void definePointCallbacks()
		{
			mono_add_internal_call("Point::setX(double)", (void*)setPointX);
			mono_add_internal_call("Point::getX()", (void*)getPointX);
			mono_add_internal_call("Point::setY(double)", (void*)setPointY);
			mono_add_internal_call("Point::getY()", (void*)getPointY);
			mono_add_internal_call("Point::setVelocityX(double)", (void*)setVelocityX);
			mono_add_internal_call("Point::getVelocityX()", (void*)getVelocityX);
			mono_add_internal_call("Point::setVelocityY(double)", (void*)setVelocityY);
			mono_add_internal_call("Point::getVelocityY()", (void*)getVelocityY);
		}

		void defineMouseCallbacks()
		{
			mono_add_internal_call("Mouse::getX()", (void*)getMouseX);
			mono_add_internal_call("Mouse::getY()", (void*)getMouseY);
		}

		MonoObject* createPoint(MonoAssembly* assembly, MonoDomain* domain, int pointIndex)
		{
			MonoImage *image = mono_assembly_get_image(assembly);
			MonoClass *pointClass = mono_class_from_name(image, "", "Point");
			MonoObject *csPoint = mono_object_new(domain, pointClass);
			mono_runtime_object_init(csPoint);
			MonoClassField* userData = mono_class_get_field_from_name(pointClass, "pointIndex");
			mono_field_set_value(csPoint, userData, &pointIndex);
			return csPoint;
		}

		
		point* extractPoint(MonoObject* pointObject)
		{
			int pointIndex;
			MonoClass *pointClass = mono_object_get_class(pointObject);
			MonoClassField* userData = mono_class_get_field_from_name(pointClass, "pointIndex");
			mono_field_get_value(pointObject, userData, &pointIndex);
			return getPoint(pointIndex);
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
