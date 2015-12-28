/* Drawing is disabled for this scenario, because there switching it off 
during testing while using this approach is curretly not implemented.
This test is however only experimental and is not included in the final work. */

#include "include/v8.h"

#include "../../scenario/pointSimul/pointSimul.h"
#include "js_pointSimul.h"
#include "js_pointSimul_loop.h"
#include "../../measure.h"
#include "../js.h"

using namespace pointSimul;

namespace js {
	namespace pointSimul {	

		Local<ObjectTemplate> pointClass;
		Local<ObjectTemplate> mouseClass;

		void registerCallbacks(Local<ObjectTemplate> global, Isolate* i) {
			global->Set(String::NewFromUtf8(i, "stepPoint"), FunctionTemplate::New(i, stepPoint)); 
			global->Set(String::NewFromUtf8(i, "updateVelocity"), FunctionTemplate::New(i, updateVelocity));
			global->Set(String::NewFromUtf8(i, "updatePoints"), FunctionTemplate::New(i, updatePoints));
			global->Set(String::NewFromUtf8(i, "getPoint"), FunctionTemplate::New(i, getPoint));
			global->Set(String::NewFromUtf8(i, "getMouse"), FunctionTemplate::New(i, getMouse));
			global->Set(String::NewFromUtf8(i, "draw"), FunctionTemplate::New(i, draw));
			global->Set(String::NewFromUtf8(i, "preDraw"), FunctionTemplate::New(i, preDraw));
			global->Set(String::NewFromUtf8(i, "postDraw"), FunctionTemplate::New(i, postDraw));
			global->Set(String::NewFromUtf8(i, "drawMouse"), FunctionTemplate::New(i, drawMouse));
			global->Set(String::NewFromUtf8(i, "drawPoint"), FunctionTemplate::New(i, drawPoint));
			global->Set(String::NewFromUtf8(i, "drawPointByCoords"), FunctionTemplate::New(i, drawPointByCoords));
			global->Set(String::NewFromUtf8(i, "initSimul"), FunctionTemplate::New(i, initSimul));
			global->Set(String::NewFromUtf8(i, "cleanup"), FunctionTemplate::New(i, cleanup));
			global->Set(String::NewFromUtf8(i, "processInput"), FunctionTemplate::New(i, processInput));
		}

		void updateVelocity(const FunctionCallbackInfo<Value>& args) {
			Local<Object> self = Local<Object>::Cast(args[0]);
			Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
			::pointSimul::updateVelocity(static_cast<point*>(wrap->Value()));
		}

		/*
		Update all points
		*/
		void updatePoints(const FunctionCallbackInfo<Value>& args) {
			for (int i = 0; i < getPointCount(); i++) {
				point * p = ::pointSimul::getPoint(i);
				updateVelocity(p);
				stepPoint(p);
			}
		}

		/*
		Get point by index
		*/
		void getPoint(const FunctionCallbackInfo<Value>& args) {
			point* p = ::pointSimul::getPoint(Local<Value>::Cast(args[0])->Int32Value());
			Local<Object> jsPoint = pointClass->NewInstance();
			jsPoint->SetInternalField(0, External::New(args.GetIsolate(), p));
			args.GetReturnValue().Set(jsPoint);
		}

		/*
		Get mouse
		*/
		void getMouse(const FunctionCallbackInfo<Value>& args) {
			Local<Object> jsMouse = mouseClass->NewInstance();
			jsMouse->SetInternalField(0, External::New(args.GetIsolate(), ::pointSimul::getMouse()));
			args.GetReturnValue().Set(jsMouse);
		}

		/*
		Initializes game and drawing variables.
		*/
		void initSimul(const FunctionCallbackInfo<Value>& args) {
			//initDrawing();

			::pointSimul::initSimul();
		}

		/*
		Draw points
		*/
		void draw(const FunctionCallbackInfo<Value>& args) {
			//::pointSimul::drawCurrentState();
		}

		void preDraw(const FunctionCallbackInfo<Value>& args)
		{
			//::pointSimul::preDraw();
		}

		void postDraw(const FunctionCallbackInfo<Value>& args)
		{
			//::pointSimul::postDraw();
		}

		/*
		Draw single point by its coordinates
		*/
		void drawPointByCoords(const FunctionCallbackInfo<Value>& args) {
			double x = Local<Value>::Cast(args[0])->NumberValue();
			double y = Local<Value>::Cast(args[1])->NumberValue();
			//::pointSimul::drawPoint(x, y);
		}

		/*
		Draw single point
		*/
		void drawPoint(const FunctionCallbackInfo<Value>& args) {
			Local<Object> self = Local<Object>::Cast(args[0]);
			Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
			point* p = static_cast<point*>(wrap->Value());
			//::pointSimul::drawPoint(p);
		}

		void drawMouse(const FunctionCallbackInfo<Value>& args)
		{
			//::pointSimul::drawMouse();
		}

		/*
		Cleans all resources.
		*/
		void cleanup(const FunctionCallbackInfo<Value>& args) {
			//cleanupDrawing();
		}

		/*
		Updates mouse and returns 0 if loop continues or 1 if flag for app quit is raised
		*/
		void processInput(const FunctionCallbackInfo<Value>& args) {
			updateMouse();

			args.GetReturnValue().Set(checkEndSingal());
		}

		int runScenario(char* name, int cycleCount, int pointCount)
		{
			Isolate* i = init();
			{
				HandleScope handle_scope(i); // scope of local handles is bound to this block (stack allocated)

				Local<ObjectTemplate> global = ObjectTemplate::New(i);
				registerCallbacks(global, i);

				Local<Context> context = Context::New(i, NULL, global);
				Context::Scope context_scope(context);

				Local<Script> script = loadScriptFromFile(i, "pointSimulLoop.js");
				script->Run();

				pointClass = createPointDefinition(i);
				mouseClass = createMouseDefinition(i);
				
				measure::start();

				// create parameters
				Handle<Value> c = Number::New(i, cycleCount);
				Handle<Value> p = Number::New(i, pointCount);
				Handle<Value> friction = Number::New(i, SS_PS_FRICTION);

				// call the script function (3 arguments, ignore result)
				Handle<Value> argv[3] = { p, c, friction };
				Handle<Value> result = getFunction(i, context, name)->Call(context->Global(), 3, argv);

				measure::stop();
				measure::displayResults();
			}
			close(i);

			return 0;
		}

		int loopCallback(int c, int p)
		{
			return runScenario("loopCallback", c, p);
		}

		int loopSetGetMax(int c, int p)
		{
			return runScenario("loopSetGetMax", c, p);
		}

		int loopSetGetMin(int c, int p)
		{
			return runScenario("loopSetGetMin", c, p);
		}
	}
}
