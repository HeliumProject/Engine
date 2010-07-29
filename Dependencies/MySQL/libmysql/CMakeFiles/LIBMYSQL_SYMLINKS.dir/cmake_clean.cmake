FILE(REMOVE_RECURSE
  "CMakeFiles/LIBMYSQL_SYMLINKS"
  "libmysqlclient.so"
  "libmysqlclient_r.so"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/LIBMYSQL_SYMLINKS.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
