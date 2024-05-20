%filenames = "scanner"

 /*
  * Please don't modify the lines above.
  */

 /* You can add lex definitions here. */
digit [0-9]
letter [a-zA-Z]

%x COMMENT STR IGNORE

%%

 /*
  * Below is examples, which you can wipe out
  * and write regular expressions and actions of your own.
  *
  * All the tokens:
  *   Parser::ID
  *   Parser::STRING
  *   Parser::INT
  *   Parser::COMMA
  *   Parser::COLON
  *   Parser::SEMICOLON
  *   Parser::LPAREN
  *   Parser::RPAREN
  *   Parser::LBRACK
  *   Parser::RBRACK
  *   Parser::LBRACE
  *   Parser::RBRACE
  *   Parser::DOT
  *   Parser::PLUS
  *   Parser::MINUS
  *   Parser::TIMES
  *   Parser::DIVIDE
  *   Parser::EQ
  *   Parser::NEQ
  *   Parser::LT
  *   Parser::LE
  *   Parser::GT
  *   Parser::GE
  *   Parser::AND
  *   Parser::OR
  *   Parser::ASSIGN
  *   Parser::ARRAY
  *   Parser::IF
  *   Parser::THEN
  *   Parser::ELSE
  *   Parser::WHILE
  *   Parser::FOR
  *   Parser::TO
  *   Parser::DO
  *   Parser::LET
  *   Parser::IN
  *   Parser::END
  *   Parser::OF
  *   Parser::BREAK
  *   Parser::NIL
  *   Parser::FUNCTION
  *   Parser::VAR
  *   Parser::TYPE
  */

 /* reserved words */
"array"     {adjust(); return Parser::ARRAY;}

 /* TODO: Put your lab2 code here */
\" {adjust();begin(StartCondition__::STR);string_buf_ = "";}
<STR>{
  \"                      {adjustStr(); begin(StartCondition__::INITIAL); setMatched(string_buf_); return Parser::STRING;}
  \\\"                    {adjustStr(); string_buf_ += '\"';}
  \\\\                    {adjustStr(); string_buf_ += '\\';}
  \\n                     {adjustStr(); string_buf_ += '\n';}
  \\t                     {adjustStr(); string_buf_ += '\t';}
  \\[0-9]{3}              {adjustStr(); string_buf_ += (char)atoi(matched().c_str() + 1);}
  \\[ \n\t\f]+\\          {adjustStr(); }
  \\\^[A-Z]               {adjustStr(); string_buf_ += matched()[2] - 'A' + 1;}
  .                       {adjustStr(); string_buf_ += matched();}
  <<EOF>>                 {adjust();errormsg_->Error(errormsg_->tok_pos_, "unclosed string literal");begin(StartCondition__::INITIAL);}
}

"/*" {adjust();begin(StartCondition__::COMMENT);comment_level_++;}

<COMMENT>{
  "*/"                    {adjust(); comment_level_ -= 1;if (comment_level_ == 1) begin(StartCondition__::INITIAL);}
  .|\n                    {adjust();}
  "/*"                    {adjust(); comment_level_ += 1;}
}

","         {adjust(); return Parser::COMMA;}
":"         {adjust(); return Parser::COLON;}
";"         {adjust(); return Parser::SEMICOLON;}
"("         {adjust(); return Parser::LPAREN;}
")"         {adjust(); return Parser::RPAREN;}
"["         {adjust(); return Parser::LBRACK;}
"]"         {adjust(); return Parser::RBRACK;}
"{"         {adjust(); return Parser::LBRACE;}
"}"         {adjust(); return Parser::RBRACE;}
"."         {adjust(); return Parser::DOT;}
"+"         {adjust(); return Parser::PLUS;}
"-"         {adjust(); return Parser::MINUS;}
"*"         {adjust(); return Parser::TIMES;}
"/"         {adjust(); return Parser::DIVIDE;}
"="         {adjust(); return Parser::EQ;}
"<>"        {adjust(); return Parser::NEQ;}
"<"         {adjust(); return Parser::LT;}
"<="        {adjust(); return Parser::LE;}
">"         {adjust(); return Parser::GT;}
">="        {adjust(); return Parser::GE;}
"&"         {adjust(); return Parser::AND;}
"|"         {adjust(); return Parser::OR;}
":="        {adjust(); return Parser::ASSIGN;}
"if"        {adjust(); return Parser::IF;}
"then"      {adjust(); return Parser::THEN;}
"else"      {adjust(); return Parser::ELSE;}
"while"     {adjust(); return Parser::WHILE;}
"for"       {adjust(); return Parser::FOR;}
"to"        {adjust(); return Parser::TO;}
"do"        {adjust(); return Parser::DO;}
"let"       {adjust(); return Parser::LET;}
"in"        {adjust(); return Parser::IN;}
"end"       {adjust(); return Parser::END;}
"of"        {adjust(); return Parser::OF;}
"break"     {adjust(); return Parser::BREAK;}
"nil"       {adjust(); return Parser::NIL;}
"function"  {adjust(); return Parser::FUNCTION;}
"var"       {adjust(); return Parser::VAR;}
"type"      {adjust(); return Parser::TYPE;}

[0-9]+                    {adjust(); string_buf_ = matched(); return Parser::INT;}
[a-zA-Z][_a-zA-Z0-9]*     {adjust(); string_buf_ = matched(); return Parser::ID;}

\"                        {adjust(); comment_level_ = 0; begin(StartCondition__::COMMENT); }
"*/"                      {adjust(); begin(StartCondition__::STR); string_buf_.clear(); }
 
 /*
  * skip white space chars.
  * space, tabs and LF
  */
[ \t]+ {adjust();}
\n {adjust(); errormsg_->Newline();}

 /* illegal input */
. {adjust(); errormsg_->Error(errormsg_->tok_pos_, "illegal token");}
