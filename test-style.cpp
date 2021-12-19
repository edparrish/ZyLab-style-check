/**
   CS-11 week n style tests:
   Statements/line, indents, line length, spacing, variable names, block comment, magic numbers, function names, class names
   @author Ed Parrish
   @version 1.3 10/30/2021
Planned use:
   testLinesAtStart: Required for other tests to work/report correctly
   stmtPerLine: lesson 2.2.4, enforce in 2
   testLineLength: lesson 2.2.4, enforce in 2
   testIndents: lesson 2.2.5, enforce in 2
   testSpaces: lesson 2.2.4, 2.4.3--enforce in 2
   testVarName: lesson 2.3.1, enforce in 2
   testBlockComment: lesson 2.2.3, enforce in 3
   testMagicNums: lesson 2.4.2--enforce in 3
   testFunctionName: lesson 8--enforce in 8
   testClassName: lesson 11--enforce in 11
*/
#include <iostream>  // included with zylab unit test
#include <fstream>   // included with zylab unit test
#include <exception> // included with zylab unit test
// https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
// ---copy below this line---
#include <regex>
const char* FILE_NAME = "main.cpp";

// Read contents of file and return as a string.
std::string get_file_contents(const char *filename) {
   std::ifstream in(filename, std::ios::in | std::ios::binary);
   if (in) {
      std::string contents;
      in.seekg(0, std::ios::end);
      contents.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&contents[0], contents.size());
      // Remove Zybook added test file contents
      const int ZY_PAD_LEN = 33; // Magic ZyLab padding length
      size_t first = contents.find_first_not_of(" ", 2);
      if (first <= 2) return contents; // for testing outside of Zybook
      if (first > 33) first = ZY_PAD_LEN;
      std::string key = contents.substr(0, first);
      contents.erase(0, first);
      size_t from = contents.find(key) - 3;
      size_t to = contents.find(key + "return 0;");
      size_t len = to - from + (key + "return 0;").size() + key.size();
      contents.replace(from, len, "\nint main() {");
      return contents;
   }
   throw(errno);
}

// Test if blanks lines at start (top) of file; remove if so
bool testLinesAtStart(std::ofstream& testFeedback, std::string& content) {
   bool noNewLnStart = true;
   if (content.at(0) == '\n' || content.at(0) == '\r') {
      noNewLnStart = false;
      testFeedback << "Remove blank lines from start of file." << std::endl;
      content.erase(0, content.find_first_not_of("\n\r"));
   }
   return noNewLnStart;
}

// Test that each statement appears on its own line
bool testStmtPerLine(std::ofstream& testFeedback, std::string testCode) {
   bool isOneStmtPerLine = true;
   // Test for for-loops
   std::regex forLoopRE(R"(\bfor\s*\()");
   std::smatch match;
   // Loop line by line
   std::stringstream sstr(testCode);
   std::string line;
   while (std::getline(sstr, line)) {
      // Test for semicolons, which end most statements in C++
      int num = std::count(line.begin(), line.end(), ';');
      // Exclude lines that have for-loops
      if (num > 1 && !std::regex_search(line, match, forLoopRE)) {
         testFeedback << "Put each statement on its own line:\n"
                      << line << std::endl;
         isOneStmtPerLine = false;
      }
   }
   return isOneStmtPerLine;
}

// Check lines for indentation problems
bool testIndents(std::ofstream& testFeedback, std::string testCode,
                 unsigned indentSpacing = 3) {
   int indentLevel = 0;
   int countNotIndented = 0;
   unsigned numTabs = 0;
   int numSpaces = 0;
   // Loop line by line
   std::stringstream sstr(testCode);
   std::string line;
   while (std::getline(sstr, line)) {
      // Skip blank or empty lines
      if (line.find_first_not_of(" \n\r") == std::string::npos) {
         continue;
      }
      // Check for tabs
      size_t firstTab = line.find('\t');
      if (firstTab != std::string::npos && numTabs < 3) {
         testFeedback << "Use spaces for indenting instead of tabs on line:\n"
                      << line << std::endl;
         numTabs++;
      }
      if (firstTab != std::string::npos && numTabs == 3) {
         testFeedback << "More tabs found...\n";
         numTabs++;
      }
      // Check indentation
      if (firstTab != std::string::npos // tabs on this line so skip
            || line.find("public:") != std::string::npos  // no-indent OK
            || line.find("private:") != std::string::npos) { // no-indent OK
         if (line.find("{") != std::string::npos) ++indentLevel;
         if (line.find("}") != std::string::npos) --indentLevel;
         continue; // skip indentation check on this line
      }
      // Count number of indented spaces
      int indentSpaces = line.find_first_not_of(" ");
      numSpaces = indentLevel * indentSpacing;
      if (line.size() > 1 && indentSpaces < numSpaces // set != or < for min
            && line.find("}") == std::string::npos) {
         testFeedback << "Must indent line to level " << indentLevel
                      << " (" << numSpaces << " spaces not " << indentSpaces
                      << ") on line:\n" << line << std::endl;
         countNotIndented++;
      }
      if (line.find("{") != std::string::npos) ++indentLevel;
      if (line.find("}") != std::string::npos) --indentLevel;
   }
   if (countNotIndented > 0) {
      testFeedback << "Always indent inside braces { }. "
                   << "Each level is " << indentSpacing << " spaces\n";
      return false;
   }
   return true;
}

// Test length of each line is <= maxLength
bool testLineLength(std::ofstream& testFeedback, std::string testCode,
                    unsigned maxLength) {
   std::stringstream sstr(testCode);
   std::string line;
   int countTooLong = 0;
   // Loop line by line
   while (std::getline(sstr, line)) {
      // Check line length
      if (line.size() > maxLength) {
         testFeedback << "Keep line length (" << line.size() << ") <= "
            << maxLength << ":\n" << line << std::endl;
         countTooLong++;
      }
   }
   if (countTooLong > 0) {
      return false;
   }
   return true;
}

// Test spaces before semicolon, after comma, around operators
bool testSpaces(std::ofstream& testFeedback, std::string testCode) {
   std::smatch match;
   bool isSpacingGood = true;
   bool isSpaceAfterComma = true;
   bool isNoSpaceBeforeSemiColon = true;
   bool isRelOpSpacingGood = true;
   bool isAndOrSpacingGood = true;
   std::string text; // specific text

   // Loop line by line
   std::stringstream sstr(testCode);
   std::string line; // each line
   while (std::getline(sstr, line)) {
      // Check space exists after a comma
      std::regex commaRE(R"(,[^\s])"); // Detect no spaces after a comma
      if (line.find(",") != std::string::npos // faster to skip regex?
            && std::regex_search(line, match, commaRE)) {
         text = match.str(0);
         if (text.size() != 0) {
            testFeedback << "Put a single space after comma (,) on line:\n"
                         << line << std::endl;
            isSpaceAfterComma = false;
         }
      }
      // Check no space before a semicolon
      std::regex semicolonRE(R"(\s;)"); // Detect space before a semicolon
      if (line.find(";") != std::string::npos // faster to skip regex?
            && std::regex_search(line, match, semicolonRE)) {
         text = match.str(0);
         if (text.size() != 0) {
            testFeedback << "Do NOT put space before semicolon (;) on line:\n"
                         << line << std::endl;
            isNoSpaceBeforeSemiColon = false;
         }
      }
      // Detect lack of space around operators: -+*/=
      // Allow exponential notation like 1e-7 by avoiding e or E
      std::regex mathOperRE(R"([a-df-zA-DF-Z0-9_)]\b([-+*\/=]{1,2})\w)");
      if (std::regex_search(line, match, mathOperRE)) {
         std::string expr = match.str(0);
         std::string oper = match.str(1);
         testFeedback << "Put space around " << oper
                      << " in expression: " << expr << std::endl;
         isSpacingGood = false;
      }
      // Test spacing around relational operators
      // Detect relational operators and capture spaces before and after
      std::regex relOpRE(
         R"([\w'\"\[\])]+(\s*)([<>]|(?:[<>=!](?![<>])){2})(\s*)[\w'\".]+)");
      // 1. Remove angle bracket enclosed words
      line = std::regex_replace(line, std::regex(R"(< *\w+ *>)"), "");
      // 2. Capture all relational expressions
      std::sregex_iterator next(line.begin(), line.end(), relOpRE);
      std::sregex_iterator end;
      while (next != end) {
         match = *next;
         std::string relExpr = match.str(0);
         std::string leftSpace = match.str(1);
         std::string relOper = match.str(2);
         std::string rightSpace = match.str(3);
         //std::cout << relExpr << ": " << relOper << "\n"; // debug
         if (leftSpace.size() != 1 || rightSpace.size() != 1) {
            testFeedback << "Put single space on each side of " << relOper
                         << " in expression: " << relExpr
                         << " on line:\n" << line << std::endl;
            isRelOpSpacingGood = false;
         }
         next++;
      }

      // Test spacing around logical operators &&, ||
      // Detect &&, || operators and capture spaces before and after
      std::regex andOrRE(R"([\w'\"\[\]\(\)]+(\s*)(&&|\|\|)(\s*)[\w'\".(]+)");
      // Capture all AND/OR expressions
      std::sregex_iterator andOrNext(line.begin(), line.end(), andOrRE);
      std::sregex_iterator andOrEnd;
      while (andOrNext != andOrEnd) {
         match = *andOrNext;
         std::string andOrExpr = match.str(0);
         std::string leftSpace = match.str(1);
         std::string andOrOper = match.str(2);
         std::string rightSpace = match.str(3);
         //std::cout << andOrExpr << ": " << andOrOper << "\n"; // debug
         if (leftSpace.size() != 1 || rightSpace.size() != 1) {
            testFeedback << "Put space on each side of " << andOrOper
                         << " in expression: " << andOrExpr << std::endl;
            isAndOrSpacingGood = false;
         }
         andOrNext++;
      }
   }
   // Add extra instructional message
   if (!isSpaceAfterComma) {
      testFeedback << "Always put a space after a comma (,) for readability.\n";
   }
   if (!isNoSpaceBeforeSemiColon) {
      testFeedback << "Never put a space before a semicolon (;).\n";
   }
   if (!isSpacingGood) {
      testFeedback << "Always put a space around math operators (+-/*) "
                   << "for readability.\n";
   }
   if (!isRelOpSpacingGood) {
      testFeedback << "Always put a space around relational operators "
                   << "for readability.\n";
   }
   if (!isAndOrSpacingGood) {
      testFeedback << "Always put a space around && and || operators "
                   << "for readability.\n";
   }
   return isSpacingGood && isSpaceAfterComma && isNoSpaceBeforeSemiColon
          && isRelOpSpacingGood && isAndOrSpacingGood;
}

// Verify variable and constant names meet style guidelines
bool testVarName(std::ofstream& testFeedback, std::string testCode) {
   // varRE explanation: not n (namespace), possible pointers, capture name, must end with [;=)
   std::regex varRE(
      R"(\b(?:[a-mo-zA-Z_,]\w*)(?:\s+\*{0,2}\b\s*)([a-zA-Z_]\w*)\b\s*(?:(?=,)|[\[;=]))");
   std::regex constVarRE(R"(^\s*\w*\s*const\b|^\s*#define\b)");
   std::regex vectorRE(R"(\bvector\s*<\s*\w+\s*>\s*\w+\s*\{)");
   std::smatch match;
   std::vector<std::string> names;

   // Loop line by line
   std::stringstream sstr(testCode);
   std::string line;
   bool isVarNameGood = true;
   bool isConstVar = false;
   bool isVectorDecl = false;
   while (std::getline(sstr, line)) {
      isConstVar = std::regex_search(line, match, constVarRE);
      isVectorDecl = std::regex_search(line, match, vectorRE);
      // Find variable names in each line
      std::sregex_iterator next(line.begin(), line.end(), varRE);
      std::sregex_iterator end;
      while (next != end) {
         std::smatch match = *next;
         std::string varName = match.str(1);
         // Check variable names meet style guidelines
         if (isConstVar && std::all_of(varName.begin(), varName.end(),
               [](unsigned char c) {
            return !std::isupper(c);
         })) {
            testFeedback << "Constant \"" << varName
                         << "\" must use all upper case in:\n" << line << std::endl;
            isVarNameGood = false;
         }
         if (!isConstVar && !isVectorDecl && (varName.at(0) < 'a' || varName.at(0) > 'z')) {
            testFeedback << "Variable named \"" << varName
                         << "\" must start with a lowercase letter.\n";
            isVarNameGood = false;
         } else
         if (!isConstVar && !isVectorDecl && varName.find("_") != std::string::npos) {
            testFeedback << "Variable named \"" << varName
                         << "\" must use camelCase and not underbars '_'.\n";
            isVarNameGood = false;
         }
         if (varName.size() < 2) {
            std::string forLoop = R"(\bfor\s*\(\s*\w+\s*)";
            forLoop += varName;
            std::regex forLoopRE(forLoop);
            if (isConstVar) {
               testFeedback << "Avoid single letter constants like \""
                            << varName << "\" in line:\n" << line << std::endl;
               isVarNameGood = false;
            } else if (!std::regex_search(line, match, forLoopRE)) {
               if (std::find(names.begin(), names.end(), varName) == names.end()) {
                 names.push_back(varName); // varName not in names, add it
               }
            }
         }
         next++;
      }
   }
   // Remove vars used in for-loop but declared previously
   std::regex re(R"(\bfor\s*\(\s*(\w*)\s*[=;])");
   std::sregex_iterator next(testCode.begin(), testCode.end(), re);
   std::sregex_iterator end;
   while (next != end) {
      std::smatch match = *next;
      std::string varName = match.str(1);
      if (std::find(names.begin(), names.end(), varName) != names.end()) {
         // Erase-remove idiom
         names.erase(std::remove(names.begin(), names.end(), varName), names.end());
      }
      next++;
   }
   // Post warning message
   if (!names.empty()) {
      std::string nameStr;
      for (unsigned i = 0; i < names.size() - 1; ++i) {
         nameStr += names.at(i) + ", ";
      }
      nameStr += names.back(); // add last element with no delimiter
      testFeedback << "Avoid one letter variables/parameters like: "
         << nameStr << "\n  unless for-loop indices (i, j) or math items "
         << "like coordinates (x, y).\n";
   }
   return isVarNameGood;
}

// Verify the file block comment
bool testBlockComment(std::ofstream& testFeedback, std::string content) {
   // Find comments: https://ostermiller.org/findcomment.html
   std::regex re(
      R"((?:\/\*+(?:[^*]|[\r\n]|(?:\*+(?:[^*\/]|[\r\n])))*\*+\/)|(?:\/\/.*))");
   std::smatch match;
   std::string comment;
   bool isGoodComment = true;
   if (std::regex_search(content, match, re) ) {
      comment = match.str(0); // Extract first comment in file
   }
   if (comment.size() < 50) { // 100 is approx size of filled out comment block
      testFeedback << "Missing completed block comment\n";
      isGoodComment = false;
   }
   if (match.position(0) > 4) { // 4 in case comment is indented
      testFeedback << "Block comments must be placed on first line\n";
      isGoodComment = false;
   }
   if (comment.substr(0, 3) != "/**") {
      testFeedback << "Block comments must start with /**\n";
      isGoodComment = false;
   } else if (comment.find("CS-11") == std::string::npos) {
      testFeedback << "Block comment line 2 must include CS-11, Lab number,"
                   << " and description\n";
      isGoodComment = false;
   }
   if (comment.find("@author") == std::string::npos) {
      testFeedback << "Block comment line 3 must include @author tag with your name\n";
      isGoodComment = false;
   } else if (comment.find("Your name") != std::string::npos) {
      testFeedback << "Block comment line 3 must include your real name "
         << "and not \"Your name\".\n";
      isGoodComment = false;
   } else if (comment.find("partner's name") != std::string::npos) {
      testFeedback << "Block comment line 3 must include partner's real name "
         << "and not \"partner's name\".\n";
      isGoodComment = false;
   }
   if (comment.find("@version") == std::string::npos) {
      testFeedback << "Block comment line 4 must include @version tag\n";
      isGoodComment = false;
   } else if (comment.find("date completed") != std::string::npos) {
      testFeedback << "Block comment line 4 must include the date completed "
         << "and not \"date completed\".\n";
      isGoodComment = false;
   }
   return isGoodComment;
}

// Test to ensure use of constants instead of literal "magic" numbers
bool testMagicNums(std::ofstream& testFeedback, std::string codeNoCmnt) {
   std::regex constVarRE(R"(^\s*\w*\b\s*const\b|^\s*#define\b)");
   std::regex numRE(R"([-+]?\b\d*\.?\d+(?:[eE][-+]?\d+)?)");
   std::smatch match;

   // Remove allowed const arrays and vectors
   std::regex arrayVectorRE(R"(const.*[\[<][^\=]+=\s*\{[^;]+)");
   codeNoCmnt = std::regex_replace(codeNoCmnt, arrayVectorRE, "");

   // Loop line by line
   std::stringstream sstr(codeNoCmnt);
   std::string line;
   bool isMagicNum = false;
   while (std::getline(sstr, line)) {
      // Skip line if starts with const
      if (std::regex_search(line, match, constVarRE)) continue;

      // Remove literal strings to avoid matching numbers in strings
      line = std::regex_replace(line, std::regex(R"(\"[^\"]+\")"), "");

      // Check all remaining literal numbers
      std::sregex_iterator next(line.begin(), line.end(), numRE);
      std::sregex_iterator end;
      while (next != end) {
         // Get next number
         match = *next;
         std::string num = match.str();

         // Skip if an allowed number
         if (num == "-1" || num == "0" || num == "0.0" || num == "1"
               || num == "2") {
            next++;
            continue;
         }
         //std::cout << num << " "; // debug

         // Report errors
         testFeedback << "Use constants instead of magic numbers like \""
                      << num << "\" in:\n" << line << std::endl;
         isMagicNum = true;

         next++;
      }
   }
   return !isMagicNum;
}

// Test function names start lower/upper case, use camel case and no underscores
bool testFunctionName(std::ofstream& testFeedback, std::string codeNoCmnt) {
   // funRE explanation: return type, capture name, must end with [;{]
   std::regex funRE(
      R"(\b[\w:]+\b\s*(?:<\s*\w+\s*>)?\s*\b(\w+)\([^)]*\)\s*(?:const)?\s*[;{])");
   // Check all function names
   std::sregex_iterator next(codeNoCmnt.begin(), codeNoCmnt.end(), funRE);
   std::sregex_iterator end;
   std::smatch match;
   while (next != end) {
      match = *next;
      //std::string funDecl = match.str();
      std::string funName = match.str(1);
      if (!isalpha(funName.at(0))) {
         testFeedback << "Function named \"" << funName
                      << "\" must start with a letter.\n";
      }
      next++;
   }
   return true;
}

// Test class names start with upper case, use camel case and no underscores
bool testClassName(std::ofstream& testFeedback, std::string codeNoCmnt) {
   std::regex classRE(R"(\bclass\s+(\w+)\s*\{)");
   // Check all class names
   std::sregex_iterator next(codeNoCmnt.begin(), codeNoCmnt.end(), classRE);
   std::sregex_iterator end;
   std::smatch match;
   while (next != end) {
      match = *next;
      //std::string classDecl = match.str();
      std::string className = match.str(1);
      if (className.at(0) < 'A' || className.at(0) > 'Z') {
         testFeedback << "Class named \"" << className
                      << "\" must start with an uppercase letter.\n";
      }
      next++;
   }
   return true;
}

// Control style options--adjust for each lab
bool testPassed(std::ofstream& testFeedback) {
   std::string content = get_file_contents(FILE_NAME);
   //std::cout << content << std::endl; // debug
   auto noNewLn = testLinesAtStart(testFeedback, content);
   auto comment = testBlockComment(testFeedback, content);
   // Strip comments: https://ostermiller.org/findcomment.html
   std::regex rc(
      R"((?:\/\*+(?:[^*]|[\r\n]|(?:\*+(?:[^*\/]|[\r\n])))*\*+\/)|(?:\/\/.*))");
   std::string testCode = std::regex_replace(content, rc, "");
   //std::cout << testCode << std::endl; // debug
   auto stmtPerLine = testStmtPerLine(testFeedback, testCode);
   auto indent = testIndents(testFeedback, testCode);
   auto lineLen = testLineLength(testFeedback, testCode, 80);
   // Strip literal strings
   testCode = std::regex_replace(testCode, std::regex(R"(\"[^"\n]*")"), "\"\"");
   // Strip literal chars--sometimes needed
   testCode = std::regex_replace(testCode, std::regex(R"(\'[^'\n]*')"), "\'\'");
   //std::cout << testCode << std::endl; // debug
   auto spacing = testSpaces(testFeedback, testCode);
   auto varName = testVarName(testFeedback, testCode);
   auto noMagicNums = testMagicNums(testFeedback, testCode);
   auto funName = testFunctionName(testFeedback, testCode);
   auto clsName = testClassName(testFeedback, testCode);

   if (!noNewLn || !stmtPerLine || !indent || !lineLen || !spacing
      || !varName
      || !comment || !noMagicNums
      || !funName
      || !clsName
   ) {
      return false;
   }
   testFeedback << "Completed automatic style tests.\n";
   testFeedback << "Other style errors may be found during visual tests.\n";
   testFeedback << "If in doubt about style, please ask.\n";
   return true;
}
// ---copy above this line---

// TODO:
// Possible to check but minor issues:
//branches, loops, functions, or classes, opening brace appears at end of line.
// If an "if", "while", "for", "class" or function must match: {\s*$

// Braces always used even if only one sub-statement -- same test as above
// If an "if" must match: {\s*$

// Else appears its on own line or just after the if's }:
// if an "else" then must match: ^\s*}?\s*else

// Need global variables test, either:
// variables with indentation 0 -- like in indentation levels above with var check.
// Find with g++ -- see getGlobalVars in gradercpp.php

// Need function comment block test

// resource: https://www.regular-expressions.info/stdregex.html
// google: c++ "std::regex" something
int main() {
   std::ofstream fout("testfeedback.txt");
   testPassed(fout);
   fout.close();

   // Echo testfeedback.txt to cout during development
   std::ifstream fin("testfeedback.txt");
   if (fin.fail()) {
       std::cout << "Input file failed to open.\n";
       exit(-1);
   }
   std::string line;
   while (std::getline(fin, line)) {
      std::cout << line << std::endl;
   }

   return 0;
}
