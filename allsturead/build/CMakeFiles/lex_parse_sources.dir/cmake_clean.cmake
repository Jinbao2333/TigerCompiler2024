file(REMOVE_RECURSE
  "CMakeFiles/lex_parse_sources"
  "../src/tiger/lex/lex.cc"
  "../src/tiger/lex/scannerbase.h"
  "../src/tiger/parse/parse.cc"
  "../src/tiger/parse/parserbase.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/lex_parse_sources.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
