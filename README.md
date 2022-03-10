# Exceptions
This document is about exceptions in C. Obviously, C does not support exceptions and so as with most things, handling errors is the responsibility of the developer. This article is about emulating exceptions, similar to the way that Python does it, in C. 

Say, for example, you try to open a file in Python, but it fails for some reason. An exception is raised and the developer is expected to respond to the error by "handling" the exception. The attempt to open the file is done in a ```try``` block and handling the exception is done in an ```except``` block. Before the except block is entered, an object is created by the code that ```raised``` the exception. Then, that object is delivered to the except block. If there is no except block then the program is aborted and a call stack trace is generated on the console to help explain why the program ended abnormally.  You can supply your own code in the except block as desired. 

In C, it works in a very similar way, only there are no classes or objects to create nice error messages and stack traces. We have to work all of that ourselves. That's what this library does. It gives a relatively nice look to exception handling and provides most of the functionality that you would expect, but there are limitations.

* An exception handler cannot accept function parameters. They are all void/voids. That means that if you want nice error messages, you have to create that plumbing for yourself. 
* Exceptions handlers are simple functions. There is nothing special about them. They can raise an exception same as any other function. That means that you could go into a mutually recursive loop if you are not careful. Steps are taken to limit these, but that would obviously not be a desirable thing to have happen.

## Why?

Why not just use C++ or C# or Objective C or whatever? These other languages are perfect (or at least very good) for their design goals. They are high level languages that allow the developer to focus on solving a high-level problem. Most problems fall into that category. The reason I am interested in implementing exceptions in C is that I am creating a hobby language called Simple that will fit somewhere between C and Python. It will translate the Simple language into ANSI C so that it can be optimized and linked as that language.  I want Simple to have exceptions.

## How It Works

First let me tell you that we are going to abuse the C preprocessor as well as the non-local goto functionality. Both of these functionalities are considered by many to be "bad" to use. Most of the arguments that I have seen seem to converge on the notion that using them results in code that is unmaintainable. I disagree with that. If you cannot teach someone how to properly use a simple thing like the C preprocessor, then how will you teach them how to properly overload operators? And, if there is a need for a non-local goto, then why would you not use it? So I do not apologize for using these taboo features. They exist because they are useful and this is a good application of them, in my opinion.

From a high level, the TRY macro calls ```setjmp()``` to create a non-local branch in the code. If ```RAISE```  is called then that non-local branch is taken using ```longjmp()``` and the exception handler and any user-created code is executed. The problem with this is that the ```TRY/EXCEPT``` blocks can be nested. So, the ```jmp_buf``` instances are kept in a stack. The stack is pushed in a ```TRY``` block and popped in an ```EXCEPT``` block. Other than that, there is no magic. 





