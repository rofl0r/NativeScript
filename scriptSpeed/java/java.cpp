#include "java.h"
#include "jni.h"
#include "Windows.h"

namespace java
{
	typedef jint (JNICALL *CreateJavaVM)(JavaVM **pvm, void **penv, void *args);

	int runHelloWorld(int c, char** v)
	{
		JavaVM *jvm;       /* denotes a Java VM */
		JNIEnv *env;       /* pointer to native method interface */
		JavaVMInitArgs vm_args; /* JDK/JRE 6 VM initialization arguments */
		JavaVMOption* options = new JavaVMOption[1];
		options[0].optionString = "-Djava.class.path=.;E:/playground/javaJNItest/JNITest";
		vm_args.version = JNI_VERSION_1_6;
		vm_args.nOptions = 1;
		vm_args.options = options;
		vm_args.ignoreUnrecognized = false;


		/* load and initialize a Java VM, return a JNI interface
		 * pointer in env */
		HINSTANCE m_hDllInstance = LoadLibraryA("c:\\Program Files (x86)\\Java\\jdk1.7.0_79\\jre\\bin\\client\\jvm.dll");
		if( m_hDllInstance == 0) {
			printf("Cannot load jvm.dll");
		}
		CreateJavaVM m_JVMInstance = (CreateJavaVM)GetProcAddress(m_hDllInstance, "JNI_CreateJavaVM");
		if ( m_JVMInstance == NULL )  {
			printf("Cannot load jvm.dll 2");
		}

		if (m_JVMInstance(&jvm, (void**)&env, &vm_args) < 0)  {
			printf("Could not launch the JVM");
		}

		// above for custom jdk location, below (the single commented line :D) we expect everything being on PATH

		//JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
		delete options;

		/* invoke the compiler */
		/*
		jclass provider = env->FindClass("javax/tools/ToolProvider");
		jmethodID providermid = env->GetStaticMethodID(provider, "getSystemJavaCompiler", "()Ljavax/tools/JavaCompiler");
		jobject compiler = env->CallStaticObjectMethod(provider, providermid);

		jclass cls = env->GetObjectClass(compiler);
		jmethodID compilermid = env->GetMethodID(cls, "run", "(Ljava/io/InputStream;Ljava/io/OutputStream;Ljava/io/OutputStream;[Ljava/lang/String;)I");

		jstring filename = env->NewStringUTF("Test.java");
		env->CallIntMethod(compiler, compilermid, NULL, NULL, NULL, filename);
		*/
		/* invoke the Main.test method using the JNI */
		
		jclass cls2 = env->FindClass("JNITest");
		jmethodID mid = env->GetStaticMethodID(cls2, "test", "(I)V");
		env->CallStaticVoidMethod(cls2, mid, 100);
		
		/* We are done. */
		jvm->DestroyJavaVM();

		return 0;
	}
}