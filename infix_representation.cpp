#include <iostream>
#include <stack> 
#include <vector>        
#include <map>        

using namespace std;

class Node
{
    public:
        Node(char data = ' ', Node *left_child = NULL, Node *right_child = NULL) : _data(data), _left_child(left_child), _right_child(right_child) {}
        Node(const Node& node);
        Node& operator=(const Node &n);
        virtual ~Node();
        virtual void printInfo() {std::cout << "Node Class" << std::endl;}
        virtual int evaluate() {};
        char GetData() const;
        Node* GetLeftChild() const;
        Node* GetRightChild() const;
        void SetData(const char data);

    protected:
        Node *_left_child;
        Node *_right_child;
        char _data;
        friend class ParseTree;
};

Node::Node(const Node& node)
{
    std::cout << "COPY CONSTRUCTOR" << std::endl;
    this->_data = node.GetData();
    this->_left_child = new Node{' ',node.GetLeftChild(),nullptr};
    this->_right_child = new Node{' ',node.GetRightChild(),nullptr};
}

Node::~Node() 
{
    std::cout << "CALLING NODE BASE DESTRUCTOR WITH DATA: " << _data << std::endl;
    delete _left_child;
    delete _right_child;
}

Node& Node::operator=(const Node &n)
{
    std::cout << "ASSIGNMENT OPERATOR" << std::endl;
    // Check for self assignment
   if(this != &n)
   {
       if(this->_right_child != NULL)
       {
           delete this->_right_child;
       }
        if(this->_left_child != NULL)
       {
           delete this->_left_child ;
       }

        this->_data = n.GetData();
        this->_left_child = new Node{' ',n.GetLeftChild(),nullptr};
        this->_right_child = new Node{' ',n.GetRightChild(),nullptr};
   }
   return *this;
}

void Node::SetData(const char data)
{
    this->_data = data;
}

char Node::GetData() const
{
    return _data;
}

Node* Node::GetLeftChild() const
{
    return _left_child;
}

Node* Node::GetRightChild() const
{
    return _right_child;
}

class Addition : public Node
{
    public:
        Addition(char c) : Node (c) {}
        virtual ~Addition() {std::cout << "ADDITION DESTRUCTOR CALLED " << _data << std::endl; }
        int evaluate() override 
        {
            return _right_child->evaluate() + _left_child->evaluate();
        }
};

class Multiplication : public Node
{
    public:
        Multiplication(char c) : Node (c) {}
        virtual ~Multiplication() 
        {
        std::cout << "MULTIPLICATION DESTRUCTOR CALLED " << _data << std::endl; 
        }
        int evaluate() override 
        {
            return _right_child->evaluate() * _left_child->evaluate();
        }
};

class Operand : public Node
{
    public:
        Operand(char c) : Node (c) {}
        virtual ~Operand() {std::cout << "OPERAND DESTRUCTOR CALLED " << _data << std::endl; }
        int evaluate() override 
        {
            return _data - '0';
        }
};

class ParseTree
{
    public:
        ParseTree(){
            auto it = operators.begin();
            // using positional hint we can get up to O(1) instead of log performances.
            it = operators.emplace_hint(it,'^',3);
            it = operators.emplace_hint(++it,'*',2);
            it = operators.emplace_hint(++it,'/',2);
            it = operators.emplace_hint(++it,'+',1);
            it = operators.emplace_hint(++it,'-',1);
        }
       
        ~ParseTree();
        void Parser(std::string sentence);
        void printTree();
        int Evaluate();
    private:
        void printDFS(Node *ptr = nullptr);
        std::stack<char> stack_of_chars;
        std::stack<Node*> stack_of_nodes;
        std::map<char,int> operators;
        Node *root_node{nullptr};
};

ParseTree::~ParseTree()
{
    std::cout << "DESTRUCTOR PARSE TREE CALLED" << std::endl; 
    while(!stack_of_nodes.empty())
    {
        delete stack_of_nodes.top();
        stack_of_nodes.top() = nullptr;
        stack_of_nodes.pop();
    }
    if(root_node != nullptr)
    {
        root_node = nullptr;
    }
}

void ParseTree::printTree()
{
    printDFS(root_node);
}

void ParseTree::printDFS(Node *ptr) {
    if(ptr == nullptr)
    {
        return;
    }
    printDFS(ptr->_right_child);

    //std::cout << ptr->_data << std::endl;

    printDFS(ptr->_left_child);
    }

int ParseTree::Evaluate()
{
    return root_node->evaluate();
}

void ParseTree::Parser(std::string sentence)
{
    for(unsigned int i = 0; i < sentence.size(); ++i)
    {
        std::cout << "current char of the sentence: " << sentence.at(i) << std::endl;
       
        if(sentence.at(i) == '(')
        {
            stack_of_chars.push(sentence.at(i));
        }
        else if (isdigit(sentence.at(i)))
        {
            stack_of_nodes.push(new Operand{sentence.at(i)});
        }
        else if(operators.count(sentence.at(i)))
        {
            auto current_char = operators.find(sentence.at(i));
            Node *tmp = nullptr;
            while( !stack_of_chars.empty() && stack_of_chars.top() != '(' && ( sentence.at(i) != '^' && 
                operators.at(stack_of_chars.top()) >= current_char->second) || (sentence.at(i) == '^' && operators.at(stack_of_chars.top()) > current_char->second) )
            {
                if(stack_of_chars.top() == '+')
                {
                    tmp = new Addition{stack_of_chars.top()};
                }
                else if(stack_of_chars.top() == '*')
                {
                    tmp = new Multiplication{stack_of_chars.top()};
                }
                stack_of_chars.pop();
                tmp->_left_child = stack_of_nodes.top();
                stack_of_nodes.pop();
                tmp->_right_child = stack_of_nodes.top();
                stack_of_nodes.pop();
                stack_of_nodes.push(std::move(tmp));

            }
            tmp = nullptr;

            stack_of_chars.push(sentence.at(i));
        }
        else if(sentence.at(i) == ')')
        {
            Node *tmp = nullptr;
            while(stack_of_chars.top() != '(')
            {
                if(stack_of_chars.top() == '+')
                {
                    tmp = new Addition{stack_of_chars.top()};
                }
                else if(stack_of_chars.top() == '*')
                {
                    tmp = new Multiplication{stack_of_chars.top()};
                }
                stack_of_chars.pop();
                tmp->_left_child = stack_of_nodes.top();
                stack_of_nodes.pop();
                tmp->_right_child = stack_of_nodes.top();
                stack_of_nodes.pop();

                stack_of_nodes.push(std::move(tmp)); // there could be a move function
            }
            tmp = nullptr;
                            
            stack_of_chars.pop();
        }
    }
    // We can assign the root node. The root node will be for sure on top of the stack().
    root_node = stack_of_nodes.top();
}

int main() {

    ParseTree tree_for_sentences{};
    tree_for_sentences.Parser("(2*3)");
    tree_for_sentences.printTree();
    std::cout << "Result of the tree: " <<tree_for_sentences.Evaluate() << std::endl;

    return 0;

}