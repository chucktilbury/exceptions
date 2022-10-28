# Exceptions
Obviously, C does not support exceptions and so as with most things, handling errors is the responsibility of the developer. This article is about emulating exceptions, similar to the way that Python does it, in C. 

Say, for example, you try to open a file in Python, but it fails for some reason. An exception is raised and the developer is expected to respond to the error by "handling" the exception. The attempt to open the file is done in a ```try``` block and handling the exception is done in an ```except``` block. Before the except block is entered, an object is created by the code that ```raised``` the exception. Then, that object is delivered to the except block. If there is no except block then the program is aborted and a call stack trace is generated on the console to help explain why the program ended abnormally.  You can supply your own code in the except block as desired. 

In C, it works in a very similar way, only there are no classes or objects to create nice error messages and stack traces. We have to work all of that ourselves. That's what this library does. It gives a relatively nice look to exception handling and provides most of the functionality that you would expect, but there are limitations.

* There is nothing like an exception handler. All exception code is in the block that handles the exception. 
* Exceptions are accessed using an enum. There is no way to pass parameters to an exception handler.

## Why?

Why not just use C++ or C# or Objective C or whatever? These other languages are perfect (or at least very good) for their design goals. They are high level languages that allow the developer to focus on solving a high-level problem. Most problems fall into that category. The reason I am interested in implementing exceptions in C is that I am creating a hobby language called Goldfish that will fit somewhere between C and Python. I want to use the notion of an exception to simplify error handling in the translator.

## How It Works

First let me tell you that we are going to abuse the C preprocessor as well as the non-local goto functionality. Both of these functionalities are considered by many to be "bad" to use. Most of the arguments that I have seen seem to converge on the notion that using them results in code that is unmaintainable. I disagree with that. If you cannot teach someone how to properly use a simple thing like the C preprocessor, then how will you teach them how to properly overload operators? (grinning) And, if there is a need for a non-local goto, then why would you not use it? So I do not apologize for using these taboo features. They exist because they are useful and this is a good application of them, in my opinion.

From a high level, the ```BEGIN_TRY``` macro calls ```setjmp()``` to create a non-local branch in the code. If ```RAISE```  is called then that non-local branch is taken using ```longjmp()``` and the user-created code is executed. The problem with this is that the ```TRY/CATCH``` blocks can be nested. So, the ```jmp_buf``` instances are kept in a stack. The stack is pushed in a ```BEGIN_TRY``` block and popped in an ```CATCH``` block. Other than that, there is no magic. 

## General format of use
```C
// Include the exceptions header.
#include "exceptions.h"

// This exact line -must- be in the program somewhere.
_ExceptionStack* exstack = NULL;

void some_function() {

  // If this function does throw an exception, and there is a catch block
  // that can catch it, then it will be caught. This is a known bug. It is
  // important to not do that.
  code_that_does_not_raise();
  
  // These three blocks are required, as are the curly braces. If you are
  // having compile issues involving scopes, make sure this is correct.
  // More than one CATCH() block is acceptable. 
  BEGIN_TRY {
    some_func_that_can_raise();
  }
  CATCH(SOMETHING_THAT_WAS ENUMED) {
    some_code_that_responds_to_a_raise();
  }
  // The CATCH() parameter is an int.
  CATCH(12345) {
    some_other_code();
  }
  END_TRY
  more_code_that_does_not_raise();
}

```



