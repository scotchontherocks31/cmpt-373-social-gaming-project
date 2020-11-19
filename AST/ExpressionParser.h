#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <peg_parser/generator.h>

#include <cmath>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>



enum class Type { // name it better
  OPENPAR,
  CLOSEPAR,
  ID,
  DOT,
  EQUALS,
  NOTEQUALS,
  GREATER,
  GREATEREQUALS,
  LESS,
  LESSEQUALS,
  NOT,
  COMMA,
};

static std::map<char, int> tokenTrigger = { // name it better... Change to vector
    {'(', 1}, {')', 1}, {'.', 1}, {'=', 2},
    {'!', 2}, {'<', 2}, {'>', 2}, {',', 1}};

static std::map<std::string, Type> stringToType = { // name it better
    {"(", Type::OPENPAR},
    {")", Type::CLOSEPAR},
    {".", Type::DOT},
    {"==", Type::EQUALS},
    {"!=", Type::NOTEQUALS},
    {">", Type::GREATER},
    {">=", Type::GREATEREQUALS},
    {"<", Type::LESS},
    {"<=", Type::LESSEQUALS},
    {"!", Type::NOT},
    {",", Type::COMMA}};

class TokenType {
public:
  TokenType(Type t, std::string s) : type{t}, value{s} {}
  std::pair<Type, std::string> getTypeValue() {
    return std::pair<Type, std::string>(type, value);
  }
  std::string getValue() { return value; }
  Type getType() { return type; }

private:
  Type type;
  std::string value;
};

struct rdp{
  
  enum class Terminal{
    COMMA,
    BIN,
    UN,
    OPENPAR,
    CLOSEPAR,
    ID,
    DOT,
  };

  std::map<Type, Terminal> TypeToTerminal{  //Anything that does not map is 
    {Type::OPENPAR , Terminal::OPENPAR},
    {Type::CLOSEPAR, Terminal::CLOSEPAR},
    {Type::DOT, Terminal::DOT},
    {Type::EQUALS, Terminal::BIN},
    {Type::NOTEQUALS, Terminal::BIN},
    {Type::GREATER, Terminal::BIN},
    {Type::GREATEREQUALS, Terminal::BIN},
    {Type::LESS, Terminal::BIN},
    {Type::LESSEQUALS, Terminal::BIN},
    {Type::NOT, Terminal::UN},
    {Type::COMMA, Terminal::COMMA},
    {Type::ID, Terminal::ID}
  };
  
  Terminal terminalType(TokenType t){
    if(TypeToTerminal.count(t.getType()) != 0){
      return TypeToTerminal[t.getType()];
    } else {
      return Terminal::ID;
    }
  }

  //nodeE
  //nodeT
  //nodeF
  //nodeP
  //nodeARG
  void parseToNodes(std::vector<TokenType> tokens){ //Returns ExpressionNode, in the end...
    index = 0;
  }

  void parseE(){

    if(checkE()){
      // E BIN T
    } else {
      parseT();
    }

  }
  
  void parseT(){
    if(terminalType(tokens[index]) == Terminal::UN){
      //return Unary(tokens[index].getType() , parseT() ) 
    }
    else if(checkF()){

    }
    else if (terminalType(tokens[index]) == Terminal::OPENPAR){

    }
    
  }
  
  void parseF(){
    current = index;
    if(checkF()){
      // F DOT P
    } else {
      parseP();
    }
    
  }

  void parseP(){
    current = index;
    if(terminalType(tokens[current]) == Terminal::ID){
      if(terminalType(tokens[current]) == Terminal::OPENPAR){
        index += 2;
        // Node n =  (tokens[current], pareseARG());  
        index += 1; //get rid of CLOSEPAR
        //return n
      } else {
        index += 1;
        //return Node( tokens[current] );
      }
    } else {
      // return Node //nothing
    }
  }

  void parseARG(){
    current = index;
    if(terminalType(tokens[current]) == Terminal::ID ){
      if(terminalType(tokens[current+1]) == Terminal::COMMA ){
        index += 2;
        //return Node( tokens[current] , tokens[current+1] ,parseARG() )
      } else {
        index += 1;
        //return Node( tokens[current] );
      }
    }
    else {
      //return Node() //nothing
    }
  }

  //look ahead
  bool checkE(int current){

  }
  bool checkT(int current){

  }
  bool checkF(int current){

  }
  bool checkARG(int current){
    return true;
  }
  bool checkP(int current){
    return true;
  }

  int index;
  int current;
  std::vector<TokenType> tokens;
};

//--- old --- TODELETE later
enum class OperationType {
  DOT,
  EQUALS,
  GREATER,
  GREATEREQUALS,
  LESS,
  LESSEQUALS,
  NOT,
  NOTHING,
};

static std::map<std::string, OperationType> stringToOperator = {
    {".", OperationType::DOT},     {"==", OperationType::EQUALS},
    {">", OperationType::GREATER}, {">=", OperationType::GREATEREQUALS},
    {"<", OperationType::LESS},    {"<=", OperationType::LESSEQUALS},
    {"!", OperationType::NOT}};

std::vector<std::string> parseExpression(std::string str);
std::vector<std::string> expressionParser(std::string str);
// -------

std::vector<TokenType> parseToType(std::string str);
#endif
