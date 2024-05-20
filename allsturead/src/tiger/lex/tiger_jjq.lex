%filenames = "scanner"

 /*
  * Please don't modify the lines above.
  */

 /* You can add lex definitions here. */
digit [0-9]
letter [a-zA-Z]

%x COMMENT STR IGNORE

%%

 /* reserved words */
"array" {adjust(); return Parser::ARRAY;}

 /* DONE: Put your lab2 code here */

"," {adjust(); return Parser::COMMA;}
":" {adjust(); return Parser::COLON;}
";" {adjust(); return Parser::SEMICOLON;}
"(" {adjust(); return Parser::LPAREN;}
")" {adjust(); return Parser::RPAREN;}
"[" {adjust(); return Parser::LBRACK;}
"]" {adjust(); return Parser::RBRACK;}
"{" {adjust(); return Parser::LBRACE;}
"}" {adjust(); return Parser::RBRACE;}
"." {adjust(); return Parser::DOT;}

"+" {adjust(); return Parser::PLUS;}
"-" {adjust(); return Parser::MINUS;}
"*" {adjust(); return Parser::TIMES;}
"/" {adjust(); return Parser::DIVIDE;}
"=" {adjust(); return Parser::EQ;}
"<>" {adjust(); return Parser::NEQ;}
"<" {adjust(); return Parser::LT;}
"<=" {adjust(); return Parser::LE;}
">" {adjust(); return Parser::GT;}
">=" {adjust(); return Parser::GE;}
"&" {adjust(); return Parser::AND;}
"|" {adjust(); return Parser::OR;}
":=" {adjust(); return Parser::ASSIGN;}

"if" {adjust(); return Parser::IF;}
"then" {adjust(); return Parser::THEN;}
"else" {adjust(); return Parser::ELSE;}
"while" {adjust(); return Parser::WHILE;}
"for" {adjust(); return Parser::FOR;}
"to" {adjust(); return Parser::TO;}
"do" {adjust(); return Parser::DO;}
"let" {adjust(); return Parser::LET;}
"in" {adjust(); return Parser::IN;}
"end" {adjust(); return Parser::END;}
"of" {adjust(); return Parser::OF;}
"break" {adjust(); return Parser::BREAK;}
"nil" {adjust(); return Parser::NIL;}
"function" {adjust(); return Parser::FUNCTION;}
"var" {adjust(); return Parser::VAR;}
"type" {adjust(); return Parser::TYPE;}

 /* identifiers */
[[:alpha:]][[:alnum:]_]* {adjust(); return Parser::ID;}

 /* literals */
[[:alnum:]]+ {adjust(); return Parser::INT;}

 /* strings */
\" {
    adjust();
    begin(StartCondition__::STR);
    string_buf_ = "";
   }

<STR> {

  \" {
    adjustStr();
    begin(StartCondition__::INITIAL);
    setMatched(string_buf_);
    return Parser::STRING;
    }
     
  \\\" {
    adjustStr();
    string_buf_ += "\"";
    }

  \\\\ {
    adjustStr();
    string_buf_ += "\\";
    }

  \\n {
    adjustStr();
    string_buf_ += "\n";
    }

  \\t {
    adjustStr();
    string_buf_ += "\t";
    }

  \\\^[A-Z] {
    adjustStr();
    string_buf_ += matched()[2] - 'A' + 1;
    }

  \\[0-9]{3} {
      adjustStr();
      int ascii = atoi(matched().substr(1).c_str());
      if (ascii < 0 || ascii > 127) errormsg_->Error(errormsg_->tok_pos_, "illegal escape sequence");
      string_buf_ += (char)ascii;
    }

  \\/[ \n\t\f] {
    adjustStr();
    begin(StartCondition__::IGNORE);
    }

  \\ {
    adjust();
    errormsg_->Error(errormsg_->tok_pos_, "illegal escape sequence");
    }

  \n {
       adjust();
       errormsg_->Newline();
       errormsg_->Error(errormsg_->tok_pos_, "unexpected line break in string");
     }
  
  \\.|. {
    adjustStr();
    string_buf_+= matched();
  }

  <<EOF>> {
            adjust();
            errormsg_->Error(errormsg_->tok_pos_, "unclosed string literal");
            begin(StartCondition__::INITIAL);
          }
}

 /* comments */
"/*" {
       adjust();
       begin(StartCondition__::COMMENT);
       comment_level_++;
     }
<COMMENT> {
  "/*" {
         adjust();
         comment_level_++;
       }
  "*/" {
         adjust();
         comment_level_--;
         if (comment_level_ == 1) begin(StartCondition__::INITIAL);
       }
  \n|. {adjust();}

  <<EOF>> {
            adjust();
            errormsg_->Error(errormsg_->tok_pos_, "unclosed comments");
            begin(StartCondition__::INITIAL);
          }
}

<IGNORE> {
  \n {
    adjustStr();
    errormsg_->Newline();
    }

  [ \t\f] {
    adjustStr();
    }

  \\ {
    adjustStr();
    begin(StartCondition__::STR);
    }

  . {
    adjust();
    errormsg_->Error(errormsg_->tok_pos_, "unexpected character in multiline string sequence");
    }

  <<EOF>> {
            adjust();
            errormsg_->Error(errormsg_->tok_pos_, "unclosed multiline string sequence");
            begin(StartCondition__::INITIAL);
          }
}

 /*
  * skip white space chars.
  * space, tabs and LF
  */
[ \t]+ {adjust();}
\n {adjust(); errormsg_->Newline();}

 /* illegal input */
. {adjust(); errormsg_->Error(errormsg_->tok_pos_, "illegal token");}
