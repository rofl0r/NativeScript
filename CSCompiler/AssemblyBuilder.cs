using System;
using System.CodeDom.Compiler;

namespace CSCompiler
{
    class AssemblyBuilder
    {
        public static void Main(string[] args)
        {
            CompileDLL(args[0], args[1]);
        } 

        public static int CompileDLL(string sourceFile, string outputFile)
        {
            Console.Write("Compiling " + sourceFile + " into " + outputFile + "... ");
            CodeDomProvider provider = CodeDomProvider.CreateProvider("cs");
            //CodeDomProvider provider = new CSharpCodeProvider();
            CompilerParameters par = new CompilerParameters();
            par.OutputAssembly = outputFile;
            par.GenerateExecutable = true;
            par.GenerateInMemory = false;

            CompilerResults results = provider.CompileAssemblyFromFile(par, sourceFile);
            Console.WriteLine("Finished (" + results.Errors.Count + " errors)");
            foreach (string err in results.Errors) 
            {
                Console.WriteLine(err);
            }
            return results.Errors.Count;
        }
    }
}

