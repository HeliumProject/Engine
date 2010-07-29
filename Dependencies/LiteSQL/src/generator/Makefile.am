AM_CXXFLAGS=-I$(srcdir)/../../include -I$(srcdir)/../library 

noinst_LIBRARIES = libmodel.a

libmodel_a_SOURCES = 	\
					logger.cpp logger.hpp \
					xmlparser.cpp xmlparser.hpp \
					objectmodel.cpp objectmodel.hpp \
					xmlobjects.hpp \
				 	xmlobjects.cpp \
				 	md5.cpp md5.hpp \
				 	generator.cpp generator.hpp \
					litesql-gen-xml.cpp litesql-gen-xml.hpp \
					litesql-gen-cpp.cpp litesql-gen-cpp.hpp \
					litesql-gen-graphviz.cpp litesql-gen-graphviz.hpp \
				 	litesql-gen-ruby-activerecord.cpp litesql-gen-ruby-activerecord.hpp


bin_PROGRAMS=litesql-gen



litesql_gen_SOURCES=	litesql-gen-main.cpp \
						litesql-gen.hpp

litesql_gen_LDADD=../library/liblitesql.la ../library/liblitesql-util.la libmodel.a

EXTRA_DIST=CMakeLists.txt
