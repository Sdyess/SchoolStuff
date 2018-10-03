#include "Interpreter.h"

Interpreter::Interpreter() {}
Interpreter::~Interpreter() {}

Interpreter::Keywords Interpreter::ParseCommand()
{
	int command = -1;
	auto itr = KeywordMap.find(tokenQueue.at(0));
	if (itr != KeywordMap.end())
		command = itr->second;
	
	return static_cast<Keywords>(command);
}

int Interpreter::GetOperatorPrecedence(char op)
{
	auto itr = OperatorMap.find(op);
	if (itr != OperatorMap.end())
		return itr->second.prec;
	
	return 0;
}

bool Interpreter::IsValidVariableName(std::string var)
{
	//rules for variable names
	//cannot contain operators or be a reserved keyword
	for (auto op : OperatorMap)
	{
		if (var.find(op.first) != std::string::npos)
			return false;
	}

	for (auto keyword : KeywordMap)
	{
		if (keyword.first == var)
			return false;
	}

	return true;
}

void Interpreter::HandleCommand(int command)
{
	switch (command)
	{
		case LOAD:
			HandleLoad();
			break;
		case MEM:
			HandleMem();
			break;
		case PRINT:
			HandlePrint();
			break;
		case STOP:
			HandleStop();
			break;
		default:
			HandleStatement();
			break;
	}
}

bool Interpreter::OperatorExists(char c)
{
	auto itr = OperatorMap.find(c);
	
	if (itr == OperatorMap.end())
		return false;
	
	return true;
}

bool Interpreter::KeywordExists(std::string keyword)
{
	auto itr = KeywordMap.find(keyword);

	if (itr == KeywordMap.end())
		return false;

	return true;
}

bool Interpreter::VariableExists(std::string var)
{
	std::string newVar = toLower(tokenQueue.at(1));
	auto itr = varMap.find(toLower(var));

	if (itr == varMap.end())
		return false;

	return true;
}

void Interpreter::PrintErrorStatement(Errors error, std::string token)
{
	switch (error)
	{
		case INVALID_SYNTAX:
			std::cout << ">ERROR: INVALID SYNTAX AT " << token << "\n";
			break;
		case UNDECL_VAR:
			std::cout << ">ERROR: VARIABLE IS UNDECLARED AT " << token << "\n";
			break;
		case UNINIT_VAR:
			std::cout << ">ERROR: VARIABLE IS UNITIALIZED AT " << token << "\n";
			break;
		case EXIST_VAR:
			std::cout << ">ERROR: VARIABLE ALREADY EXISTS AT " << token << "\n";
			break;
	}
}

void Interpreter::SanitizeInput(std::string& input)
{
	//substr before comment
	std::size_t comPos = input.find_first_of("//");
	input = input.substr(0, comPos);

	//trim whitespace
	input = ltrim(input);
	input = rtrim(input);
}

void Interpreter::TokenizeInput(std::string input)
{
	
	tokenQueue.clear();
	std::string cleanInput = input;
	SanitizeInput(cleanInput);

	std::regex reg("\\s+");

	std::sregex_token_iterator iter(input.begin(), input.end(), reg, -1);
	std::sregex_token_iterator end;

	std::vector<std::string> cleanVec(iter, end);

	//this will change to use a lookahead for unary operators
	for (auto token : cleanVec)
	{
		std::string curExpression = "";
		for (auto c : token)
		{
			if (OperatorExists(c))
			{
				if (curExpression.length() > 0)
				{
					tokenQueue.push_back(curExpression);
					curExpression = "";
				}
				tokenQueue.push_back(std::string(1, c));
			}
			else
				curExpression += c;
		}
		if (curExpression.length() > 0)
			tokenQueue.push_back(curExpression);	
	}

	//tokenStack = std::stack<std::string, std::deque<std::string>>(tokenQueue);

}

void Interpreter::HandleLoad()
{
	if (tokenQueue.size() <= 1)
		return;

	std::string newVar = toLower(tokenQueue.at(1));
	auto itr = varMap.find(newVar);

	if (itr != varMap.end())
	{
		PrintErrorStatement(EXIST_VAR, newVar);
		return;
	}

	std::cout << ">ENTER VALUE FOR " + toUpper(newVar) + " ";
	
	float val = 0.0f;

	if (!(std::cin >> val))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << ">ERROR: EXPECTED FLOAT VALUE\n";
		return;
	}

	varMap.insert(std::pair(newVar, CreateVariable(&val)));
	std::cin.ignore();
}

std::optional<float> Interpreter::CreateVariable(float* val)
{
	if (!val)
		return std::nullopt;
	else
		return std::optional<float>{*val};
}

void Interpreter::HandleMem()
{
	if (tokenQueue.size() <= 1)
		return;

	std::string newVar = toLower(tokenQueue.at(1));
	auto itr = varMap.find(newVar);

	if (itr != varMap.end())
	{
		PrintErrorStatement(EXIST_VAR, newVar);
		return;
	}

	varMap.insert(std::pair(newVar, CreateVariable()));
}

void Interpreter::HandlePrint()
{
	std::string printVar = toLower(tokenQueue.at(1));
	auto itr = varMap.find(printVar);

	if (itr == varMap.end()) 
	{
		PrintErrorStatement(UNDECL_VAR, printVar);
		return;
	}

	std::ostringstream ss;

	if (varMap[printVar].has_value())
	{
		ss.precision(2);

		ss << ">VALUE OF ";
		ss << toUpper(printVar);
		ss << " IS ";
		ss << std::fixed << varMap[printVar].value() << "\n";
		std::cout << ss.str();
	}
	else
		PrintErrorStatement(UNINIT_VAR, printVar);

	
}

void Interpreter::HandleStop()
{
	exit(EXIT_SUCCESS);
}

float Interpreter::applyOp(float curVal, float appVal, char op)
{
	switch (op)
	{
	case '+':
		return curVal + appVal;
	case '-':
		return curVal - appVal;
	case '/':
		return curVal / appVal;
	case '*':
		return curVal * appVal;
	case '^':
		return std::powf(curVal, appVal);
	case '=':
		return appVal;
	}
}

void Interpreter::ConvertToPostfix()
{
	std::deque<std::string> postFix;
	std::stack<std::string> opStack;

	//take tokenQueue and apply conversion
	for (auto token : tokenQueue)
	{
		if (OperatorMap.find(token[0]) != OperatorMap.end())
		{
			if (token[0] == '(')
			{
				opStack.push(token);
			}
			else if (token[0] == ')')
			{
				std::string top = opStack.top();
				while (top != "(")
				{
					postFix.push_front(top);
					opStack.pop();
					top = opStack.top();
				}
				opStack.pop();
			}
			else
			{
				int OpPrec = GetOperatorPrecedence(token[0]);
				while (!opStack.empty())
				{
					int tst = GetOperatorPrecedence(opStack.top().c_str()[0]);
					if (OpPrec >= tst)
					{
						postFix.push_front(opStack.top());
						opStack.pop();
					}
					
				}
				opStack.push(token);
			}
		}
		else
		{
			postFix.push_front(token);
		}
	}
	while (!opStack.empty())
	{
		postFix.push_front(opStack.top());
		opStack.pop();
	}

	tokenStack = std::stack<std::string, std::deque<std::string>>(postFix);
}

void Interpreter::HandleStatement()
{
	// get variable and = first, then do this
	ConvertToPostfix();
	std::stack<float> resultStack;

	while (!tokenStack.empty())
	{
		std::string top = tokenStack.top();
		if (OperatorExists(top[0]))
		{
			//pop operator
			tokenStack.pop();
			float val1 = resultStack.top();
			resultStack.pop();
			float val2 = resultStack.top();
			resultStack.pop();
			resultStack.push(applyOp(val1, val2, top[0]));
		}
		else if (VariableExists(top))
		{
			if (varMap[toLower(top)].has_value())
			{
				resultStack.push(varMap[toLower(top)].value());
				tokenStack.pop();
			}
				
		}
		else
		{
			resultStack.push(std::stof(top));
			tokenStack.pop();
		}
	}
	float x = resultStack.top();
}

