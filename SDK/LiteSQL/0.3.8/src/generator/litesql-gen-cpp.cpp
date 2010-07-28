#include "litesql_char.hpp"
#include "litesql/split.hpp"
#include "litesql/types.hpp"
#include "litesql-gen-cpp.hpp"
#include "xmlobjects.hpp"

#include "logger.hpp"

#include <cstdio>

namespace gen {
using namespace litesql;
class Variable {
public:
    LITESQL_String name, type, value;
    bool isStatic, isProtected;
    
    Variable(LITESQL_String n, LITESQL_String t, LITESQL_String v= LITESQL_L("")) 
        : name(n), type(t), value(v), isStatic(false), isProtected(false) {}
    Variable& static_() {
        isStatic = true;
        return *this;
    }

    Variable& protected_() {
        isProtected = true;
        return *this;
    }
    
    LITESQL_String classDeclaration() {
      LITESQL_String decl(isStatic ?  LITESQL_L("static ") :  LITESQL_L(""));
      decl.append(type);
      decl.append(LITESQL_L(" "));
      decl.append(name);
      decl.append(LITESQL_L(";"));
      return decl;
    }
    LITESQL_String quotedValue() {
        if (type ==  LITESQL_L("LITESQL_String"))
            return  LITESQL_L("\"") + value +  LITESQL_L("\"");
        return value;
    }
    LITESQL_String paramDefinition(bool defaults=true) {
        
        LITESQL_String val;
        if (defaults && value.size() > 0)
            val =  LITESQL_L("=") + quotedValue();
        
        
        LITESQL_String buf(type);
        buf.append(LITESQL_L(" "));
        buf.append(name);
        buf.append(val);
        return buf;
    }
    LITESQL_String staticDefinition(LITESQL_String context) {
        LITESQL_String val = quotedValue();
        LITESQL_String params;
        if (value.size() > 0) {
            params =  LITESQL_L("(") + val +  LITESQL_L(")");
        }
        
        LITESQL_String buf(type);
        buf.append(LITESQL_L(" "));
        buf.append(context);
        buf.append(name);
        buf.append(params);
        buf.append(LITESQL_L(";"));
        return buf;
    }
};
#include <iostream>
class Method {
public:
    LITESQL_String name, returnType, templateParams, constructorParams;
    bool isStatic, isVirtual, isProtected, isTemplate, isTemplateSpec, isDefinition, isConst, isAbstract;
    std::vector<Variable> params;
    std::vector<LITESQL_String> bodyLines;
   
    Method(const LITESQL_String& n, const LITESQL_String& rt= LITESQL_L("")) 
        : name(n), returnType(rt), templateParams(LITESQL_L("")), constructorParams(LITESQL_L("")),
          isStatic(false), isVirtual(false), isProtected(false), isTemplate(false),
          isTemplateSpec(false), isDefinition(false), isConst(false), isAbstract(false) {}
    Method& param(const Variable& v) {
        params.push_back(v);
        return *this;
    }
    Method& constructor(LITESQL_String params) {
        constructorParams = params;
        return *this;
    }
    Method& body(LITESQL_String line) {
        bodyLines.push_back(line);
        return *this;
    }
    Method& static_() {
        isStatic = true;
        return *this;
    }
    Method& virtual_() {
        isVirtual = true;
        return *this;
    }
    Method& protected_() {
        isProtected = true;
        return *this;
    }
    Method& defineOnly() {
        isDefinition = true;
        return *this;
    }
    Method& template_(LITESQL_String params) {
        isTemplate = true;
        templateParams = params;
        return *this;
    }
    Method& templateSpec(LITESQL_String params) {
        isTemplate = isTemplateSpec = true;
        templateParams = params;
        return *this;
    }
    Method& const_() {
        isConst = true;
        return *this;
    }
    Method& abstract_() {
        isAbstract = true;
        return *this;
    }

    void write(FILE * hpp, FILE * cpp, LITESQL_String context, int indent=0) {
        LITESQL_String ind = LITESQL_String(LITESQL_L(" ")) * indent;
        LITESQL_String tpl;
        LITESQL_String sta;
        LITESQL_String ret;
        LITESQL_String cons;
        if (isTemplate) 
            tpl =  LITESQL_L("template <") + templateParams +  LITESQL_L("> ");
        if (isStatic)
            sta =  LITESQL_L("static ");
        if (isVirtual)
            sta =  LITESQL_L("virtual ");
        if  (returnType.size()) 
            ret = returnType +  LITESQL_L(" ");
        if (constructorParams.size())
            cons =  LITESQL_L("\n") + ind +  LITESQL_L(" : ") + constructorParams;
        LITESQL_String cnst;
        if (isConst)
            cnst =  LITESQL_L(" const");
        if (isAbstract)
            cnst +=  LITESQL_L("=0");
        LITESQL_String paramString;
        for (size_t i = 0; i < params.size(); i++) {
            if (i > 0)
                paramString +=  LITESQL_L(", ");
            paramString += params[i].paramDefinition(isTemplateSpec==false);
        }
        LITESQL_String result;
        
        if (!isTemplateSpec)
            _ftprintf(hpp,  LITESQL_L("%s%s%s%s%s(%s)%s"), ind.c_str(), tpl.c_str(), sta.c_str(),
                    ret.c_str(), name.c_str(), paramString.c_str(), cnst.c_str());
        if (isDefinition) 
            _ftprintf(hpp,  LITESQL_L(";\n"));
        else {
            if (isTemplate && !isTemplateSpec) {
                _ftprintf(hpp,  LITESQL_L(" {\n"));
                for (size_t i = 0; i < bodyLines.size(); i++) 
                    _ftprintf(hpp,  LITESQL_L("%s%s\n"), ind.c_str(), bodyLines[i].c_str());
                _ftprintf(hpp,  LITESQL_L("%s}\n"), ind.c_str());
            } else {
                _ftprintf(hpp,  LITESQL_L(";\n"));
                paramString =  LITESQL_L("");
                for (size_t i = 0; i < params.size(); i++) {
                    if (i > 0)
                        paramString +=  LITESQL_L(", ");
                    paramString += params[i].paramDefinition(false);
                }
                ind = LITESQL_String(LITESQL_L(" ")) * 4;
                _ftprintf(cpp,  LITESQL_L("%s%s%s%s(%s)%s%s {\n"), tpl.c_str(), ret.c_str(),
                        context.c_str(), name.c_str(), paramString.c_str(),
                        cnst.c_str(), cons.c_str());
                for (size_t i = 0; i < bodyLines.size(); i++) 
                    _ftprintf(cpp,  LITESQL_L("%s%s\n"), ind.c_str(), bodyLines[i].c_str());            
                _ftprintf(cpp,  LITESQL_L("}\n"));
            }
        }
    }
};
class Class {
    LITESQL_String name;
    LITESQL_String inherits;
    std::vector<Method> methods;
    std::vector<Variable> variables;
    std::vector<Class> classes;
public:
    Class(LITESQL_String n, LITESQL_String i= LITESQL_L("")) : name(n), inherits(i) {}
    Class& method(const Method& m) {
        methods.push_back(m);
        return *this;
    }
    Class& variable(const Variable& v) {
        variables.push_back(v);
        return *this;
    }
    Class &class_(const Class& c) {
        classes.push_back(c);
        return *this;
    }
    void write(FILE * hpp, FILE * cpp, int indent=0, LITESQL_String context= LITESQL_L("")) {
        LITESQL_String inh;
        LITESQL_String ind = LITESQL_String(LITESQL_L(" ")) * indent;
        context += name +  LITESQL_L("::");

        if (inherits !=  LITESQL_L("")) 
            inh =  LITESQL_L(" : public ") + inherits;
        LITESQL_String mode =  LITESQL_L("public");
        _ftprintf(hpp,  LITESQL_L("%sclass %s%s {\n"), ind.c_str(), name.c_str(), inh.c_str());
        _ftprintf(hpp,  LITESQL_L("%spublic:\n"), ind.c_str());
        for (size_t i = 0; i < classes.size(); i++) {

            classes[i].write(hpp, cpp, indent + 4, context);
        }
        for (size_t i = 0; i < variables.size(); i++)  {
            LITESQL_String decl = variables[i].classDeclaration();
            if (variables[i].isProtected && mode== LITESQL_L("public")) {
                mode =  LITESQL_L("protected");
                _ftprintf(hpp,  LITESQL_L("%sprotected:\n"), ind.c_str());
            }
            if (!variables[i].isProtected && mode== LITESQL_L("protected")) {
                mode =  LITESQL_L("public");
                _ftprintf(hpp,  LITESQL_L("%spublic:\n"), ind.c_str());
            }
                
            _ftprintf(hpp,  LITESQL_L("    %s%s\n"), ind.c_str(), decl.c_str());
            if (variables[i].isStatic) {
                LITESQL_String def = variables[i].staticDefinition(context);
                _ftprintf(cpp,  LITESQL_L("%s\n"), def.c_str());
            }
        }
        for (size_t i = 0; i < methods.size(); i++) {
            if (methods[i].isProtected && mode== LITESQL_L("public")) {
                mode =  LITESQL_L("protected");
                _ftprintf(hpp,  LITESQL_L("%sprotected:\n"), ind.c_str());
            }
            if (!methods[i].isProtected && mode== LITESQL_L("protected")) {
                mode =  LITESQL_L("public");
                _ftprintf(hpp,  LITESQL_L("%spublic:\n"), ind.c_str());
            }
            methods[i].write(hpp, cpp, context, indent + 4);            
        }
        _ftprintf(hpp,  LITESQL_L("%s};\n"), ind.c_str());
        
    }
};

}

using namespace gen;
using namespace litesql;

LITESQL_String quote(LITESQL_String s) {
    return  LITESQL_L("\"") + s +  LITESQL_L("\"");
}
LITESQL_String brackets(LITESQL_String s) {
    return  LITESQL_L("(") + s +  LITESQL_L(")");
}
static bool validID(LITESQL_String s) {
    static const LITESQL_Char* words[] = 
        { LITESQL_L("asm"), LITESQL_L("break"), LITESQL_L("case"), LITESQL_L("catch"),
          LITESQL_L("char"), LITESQL_L("class"), LITESQL_L("const"), LITESQL_L("continue"), LITESQL_L("default"),
          LITESQL_L("delete"), LITESQL_L("do"), LITESQL_L("double"), LITESQL_L("else"), LITESQL_L("enum"), LITESQL_L("extern"),
          LITESQL_L("float"), LITESQL_L("for"), LITESQL_L("friend"), LITESQL_L("goto"), LITESQL_L("if"), LITESQL_L("inline"), LITESQL_L("int"),
          LITESQL_L("long"), LITESQL_L("new"), LITESQL_L("operator"), LITESQL_L("private"), LITESQL_L("protected"),
          LITESQL_L("public"), LITESQL_L("register"), LITESQL_L("return"), LITESQL_L("short"), LITESQL_L("signed"),
          LITESQL_L("sizeof"), LITESQL_L("static"), LITESQL_L("struct"), LITESQL_L("switch"), LITESQL_L("template"),
          LITESQL_L("this"), LITESQL_L("throw"), LITESQL_L("try"), LITESQL_L("typedef"), LITESQL_L("union"), LITESQL_L("unsigned"),
          LITESQL_L("virtual"), LITESQL_L("void"), LITESQL_L("volatile"), LITESQL_L("while"),   
         // LiteSQL specific
          LITESQL_L("initValues"),  LITESQL_L("insert"),  LITESQL_L("addUpdates"),  LITESQL_L("addIDUpdates"),
          LITESQL_L("getFieldTypes"),  LITESQL_L("delRecord"),  LITESQL_L("delRelations"),
          LITESQL_L("update"),  LITESQL_L("del"),  LITESQL_L("typeIsCorrect"),  LITESQL_L("upcast"),  LITESQL_L("upcastCopy")
        };

    for (size_t i = 0; i < sizeof(words) / sizeof(words[0]); i++)
        if (s == words[i])
            return false;
    return true; 
}
static void sanityCheck(const xml::Database& db,
                        const std::vector<xml::Object*>& objects,
                        const std::vector<xml::Relation*>& relations) {
    using namespace litesql;
    if (!validID(db.name)) 
        throw Except(LITESQL_L("invalid id: database.name : ") + db.name);
    for (size_t i = 0; i < objects.size(); i++) {
        xml::Object& o = *objects[i];
        if (!validID(o.name))
            throw Except(LITESQL_L("invalid id: object.name : ") + o.name);
        for (size_t i2 = 0; i2 < o.fields.size(); i2++) {
            xml::Field& f = *o.fields[i2];
            if (!validID(f.name))
                throw Except(LITESQL_L("invalid id: object.field.name : ") + f.name);
        }
    }
    for (size_t i = 0; i < relations.size(); i++) {
        xml::Relation& r = *relations[i];
        if (!validID(r.getName()))
            throw Except(LITESQL_L("invalid id: relation.name : ") + r.getName());
        for (size_t i2 = 0; i2 < r.fields.size(); i2++) {
            xml::Field& f = *r.fields[i2];
            if (!validID(f.name))
                throw Except(LITESQL_L("invalid id: relation.field.name : ") + f.name);
        }
        for (size_t i2 = 0; i2 < r.related.size(); i2++) {
            xml::Relate& rel = *r.related[i2];
            if (!validID(rel.handle) && !rel.handle.empty())
                throw Except(LITESQL_L("invalid id: relation.relate.handle : ") + rel.handle);
        }
    }   
}

void writeStaticObjData(Class& cl, const xml::Object& o) {
    Variable type__(LITESQL_L("type__"),  LITESQL_L("const LITESQL_String"), quote(o.name));
    type__.static_();
    cl.variable(type__);

    Variable table__(LITESQL_L("table__"),  LITESQL_L("const LITESQL_String"), 
                     quote(o.getTable()));
    table__.static_();
    cl.variable(table__);

    if (!o.parentObject) {
        Variable sequence__(LITESQL_L("sequence__"),  LITESQL_L("const LITESQL_String"), 
                            quote(o.getSequence()));
        sequence__.static_();
        cl.variable(sequence__);
    }
}
void writeObjFields(Class & cl, const xml::Object & o) {
  gen::Method init(LITESQL_L("initValues"),  LITESQL_L("void"));
    bool hasValues = false;
    LITESQL_String ftypeClass = LITESQL_L("const litesql::FieldType");

    init.static_();

    Class ownData(LITESQL_L("Own"));
    Variable ftype(LITESQL_L("Id"), ftypeClass, 
                   quote(LITESQL_L("id_")) +  LITESQL_L(",") + quote(LITESQL_L("INTEGER")) 
                   +  LITESQL_L(",") + quote(o.getTable()));
    ftype.static_();
    ownData.variable(ftype);
    
    cl.class_(ownData);
  
    for (size_t i = 0; i < o.fields.size(); i++) {
        const xml::Field& fld = *o.fields[i];
        LITESQL_String data = quote(fld.name +  LITESQL_L("_")) +  LITESQL_L(",") +
            quote(fld.getSQLType()) +  LITESQL_L(",") +
             LITESQL_L("table__");
        if (!fld.values.empty()) {
            data +=  LITESQL_L(",") + fld.name +  LITESQL_L("_values"); 
            Variable values(fld.name +  LITESQL_L("_values"), 
                     LITESQL_L("std::vector < std::pair< LITESQL_String, LITESQL_String > >"));
            values.static_().protected_();
            cl.variable(values);
            hasValues = true;
            init.body(fld.name +  LITESQL_L("_values.clear();"));
            for (size_t i2 = 0; i2 < fld.values.size(); i2++) {
                const xml::Value& val = fld.values[i2];
                init.body(fld.name +  LITESQL_L("_values.push_back(make_pair<LITESQL_String, LITESQL_String>(")
                        + quote(val.name) +  LITESQL_L(",") + quote(val.value) +  LITESQL_L("));"));    
            }
        }
        if (!fld.values.empty()) {
            ftypeClass = fld.fieldTypeName +  LITESQL_L("Type");
            Class ftypeCl(ftypeClass,  LITESQL_L("litesql::FieldType"));
            gen::Method cons(ftypeClass);
            ftypeClass =  LITESQL_L("const ") + o.name +  LITESQL_L("::") + ftypeClass;
            cons.param(Variable(LITESQL_L("n"),  LITESQL_L("const LITESQL_String&")))
                .param(Variable(LITESQL_L("t"),  LITESQL_L("const LITESQL_String&")))
                .param(Variable(LITESQL_L("tbl"),  LITESQL_L("const LITESQL_String&")))
                .param(Variable(LITESQL_L("vals"),  LITESQL_L("const litesql::FieldType::Values&"),  LITESQL_L("Values()")))
                .constructor(LITESQL_L("litesql::FieldType(n,t,tbl,vals)"));

            ftypeCl.method(cons);
            for (size_t i = 0; i < fld.values.size(); i++) {
                const xml::Value& value = fld.values[i];
                LITESQL_String v;
                if (fld.getCPPType() ==  LITESQL_L("LITESQL_String"))
                    v = quote(value.value);
                else
                    v = value.value;
                Variable val(value.name,  LITESQL_L("const ") + fld.getCPPType(), v);

                val.static_();
                ftypeCl.variable(val);
            }
            cl.class_(ftypeCl);
        }

        Variable ftype(fld.fieldTypeName, ftypeClass, data);
        ftype.static_();
        Variable field(fld.name,  LITESQL_L("litesql::Field<") + fld.getCPPType() +  LITESQL_L(">"));
        cl.variable(ftype);
        cl.variable(field);
        if (fld.values.size() > 0) {
            Class valueHolder(xml::capitalize(fld.name));
            for (size_t i = 0; i < fld.values.size(); i++) {
                const xml::Value& value = fld.values[i];
                LITESQL_String v;
                if (fld.getCPPType() ==  LITESQL_L("LITESQL_String"))
                    v = quote(value.value);
                else
                    v = value.value;
                Variable val(value.name,  LITESQL_L("const ") + fld.getCPPType(), v);

                val.static_();
                valueHolder.variable(val);
            }
            cl.class_(valueHolder);
        }
    }
    if (hasValues)
        cl.method(init);
}
void writeObjConstructors(Class& cl, const xml::Object& o) {
  gen::Method defaults(LITESQL_L("defaults"),  LITESQL_L("void"));
    defaults.protected_();
    bool hasDefaults = false;
    for (size_t i = 0; i < o.fields.size(); i++) {
        const xml::Field& f = *o.fields[i];
        if (!f.default_.empty() || !f.hasQuotedValues())
        {
          defaults.body(f.name +  LITESQL_L(" = ") + f.getQuotedDefaultValue() +  LITESQL_L(";"));
          hasDefaults = true;
        }
    } 

    
    gen::Method cons1(o.name); // Object(const Database &)
    LITESQL_String consParams = o.inherits +  LITESQL_L("(db)");
    LITESQL_String cons2Params = o.inherits +  LITESQL_L("(db, rec)");
    if (o.fields.size() > 0) {
        Split fieldInst;
        for (size_t i = 0; i < o.fields.size(); i++) {
            const xml::Field& f = *o.fields[i];
            fieldInst.push_back(f.name + brackets(f.fieldTypeName));
        }
        consParams +=  LITESQL_L(", ") + fieldInst.join(LITESQL_L(", "));
        cons2Params +=  LITESQL_L(", ") + fieldInst.join(LITESQL_L(", "));

    }

    cons1.param(Variable(LITESQL_L("db"),  LITESQL_L("const litesql::Database&")))
        .constructor(consParams);
    if (hasDefaults)
        cons1.body(LITESQL_L("defaults();"));
    
    gen::Method cons2(o.name); // Object(const Database &, const Record& row
    cons2.param(Variable(LITESQL_L("db"),  LITESQL_L("const litesql::Database&")))
        .param(Variable(LITESQL_L("rec"),  LITESQL_L("const litesql::Record&")))
        .constructor(cons2Params);
    if (hasDefaults) 
        cons2.body(LITESQL_L("defaults();"));
    if (o.fields.size() > 0) {
        int last = o.getLastFieldOffset();
        cons2.body(LITESQL_L("size_t size = ")
                    LITESQL_L("(rec.size() > ") + toString(last) +  LITESQL_L(")")
                    LITESQL_L(" ? ") + toString(last) +  LITESQL_L(" : rec.size();"))
            .body(LITESQL_L("switch(size) {"));

        for(int i = o.fields.size() - 1; i >= 0; i--) {
            int p = last - o.fields.size() + i;
            cons2.body(LITESQL_L("case ") + toString(p+1) +  LITESQL_L(": ") 
                       + o.fields[i]->name 
                       +  LITESQL_L(" = convert<const LITESQL_String&, ")
                       + o.fields[i]->getCPPType() 
                       +  LITESQL_L(">(rec[") + toString(p) +  LITESQL_L("]);"))
                .body(LITESQL_L("    ") + o.fields[i]->name +  LITESQL_L(".setModified(false);"));

        }
        cons2.body(LITESQL_L("}"));
    }
    
    gen::Method cons3(o.name); // Object(const Object& obj);
    LITESQL_String consParams3 = o.inherits +  LITESQL_L("(obj)");    
    if (o.fields.size() > 0) {
        Split fieldCopy;
        for (size_t i = 0; i < o.fields.size(); i++) {
            const xml::Field& f = *o.fields[i];
            fieldCopy.push_back(f.name + brackets(LITESQL_L("obj.") + f.name));
        }
        consParams3 +=  LITESQL_L(", ") + fieldCopy.join(LITESQL_L(", "));
    }
    cons3.param(Variable(LITESQL_L("obj"),  LITESQL_L("const ") + o.name +  LITESQL_L("&")))
        .constructor(consParams3);
    if (hasDefaults)
        cl.method(defaults);
    
    gen::Method assign(LITESQL_L("operator="),  LITESQL_L("const ") + o.name +  LITESQL_L("&"));
    assign.param(Variable(LITESQL_L("obj"),  LITESQL_L("const ") + o.name +  LITESQL_L("&")));
    if (o.fields.size() > 0) {
        assign.body(LITESQL_L("if (this != &obj) {"));
        for (size_t i = 0; i < o.fields.size(); i++) {
            const xml::Field& f = *o.fields[i];
            assign.body(LITESQL_L("    ") + f.name +  LITESQL_L(" = obj.") + f.name +  LITESQL_L(";"));
        }
        assign.body(LITESQL_L("}"));
    }
    assign.body(o.inherits +  LITESQL_L("::operator=(obj);"));
    assign.body(LITESQL_L("return *this;"));
    
    cl.method(cons1).method(cons2).method(cons3).method(assign);
    
}
void writeObjRelationHandles(Class& cl, xml::Object& o) {
    for (size_t i = 0; i < o.handles.size(); i++) {
        xml::RelationHandle& handle = *o.handles[i];
        xml::Relation* rel = handle.relation;

        LITESQL_String className = xml::capitalize(handle.name) +  LITESQL_L("Handle");
        Class hcl(className,
                   LITESQL_L("litesql::RelationHandle<") + o.name +  LITESQL_L(">"));
        gen::Method cons(className);
        cons.param(Variable(LITESQL_L("owner"),  LITESQL_L("const ") + o.name +  LITESQL_L("&"))).
            constructor(LITESQL_L("litesql::RelationHandle<") + o.name +  LITESQL_L(">(owner)"));
        
        gen::Method link(LITESQL_L("link"),  LITESQL_L("void"));
        gen::Method unlink(LITESQL_L("unlink"),  LITESQL_L("void"));
        Split params;
        params.push_back(LITESQL_L("owner->getDatabase()"));
        params.resize(1 + rel->related.size());
        params[1 + handle.relate->paramPos] =  LITESQL_L("*owner");

        for (size_t i2 = 0; i2 < handle.destObjects.size(); i2++) {
            xml::Object* dest = handle.destObjects[i2].first;
            xml::Relate * relate = handle.destObjects[i2].second;
            Variable var(LITESQL_L("o") + toString(i2),  LITESQL_L("const ") + dest->name +  LITESQL_L("&"));
            link.param(var);
            unlink.param(var);
            params[1 + relate->paramPos] =  LITESQL_L("o") + toString(i2);
        }


        for (size_t i2 = 0; i2 < rel->fields.size(); i2++) {
            xml::Field& field = *rel->fields[i2];
            // FIXME: default-arvoiset parametrit viimeiseksi

            link.param(Variable(field.name, field.getCPPType(), 
                                field.getQuotedDefaultValue()));
            unlink.param(Variable(field.name, field.getCPPType()));
            params.push_back(field.name);
        }
        link.body(rel->getName() +  LITESQL_L("::link(") + params.join(LITESQL_L(", ")) +  LITESQL_L(");"));
        unlink.body(rel->getName() +  LITESQL_L("::unlink(") + params.join(LITESQL_L(", ")) +  LITESQL_L(");"));
        Variable exprParam(LITESQL_L("expr"),  LITESQL_L("const litesql::Expr&"),  LITESQL_L("litesql::Expr()"));
        Variable srcExprParam(LITESQL_L("srcExpr"),  LITESQL_L("const litesql::Expr&"),  LITESQL_L("litesql::Expr()"));
        gen::Method del(LITESQL_L("del"),  LITESQL_L("void"));
        params.clear();
        params.push_back(LITESQL_L("owner->getDatabase()"));
        params.push_back(LITESQL_L("expr && ") + rel->getName() 
                +  LITESQL_L("::") + handle.relate->fieldTypeName +  LITESQL_L(" == owner->id"));
        del.param(exprParam).body(rel->getName() +  LITESQL_L("::del(") + params.join(LITESQL_L(", ")) +  LITESQL_L(");"));


        hcl.method(cons).method(link).method(unlink).method(del);
        LITESQL_String extraExpr =  LITESQL_L(" && srcExpr");
        if (handle.destObjects.size() == 1) {
            xml::Object* dest = handle.destObjects[0].first;
            xml::Relate* relate = handle.destObjects[0].second;
            gen::Method get(LITESQL_L("get"),  LITESQL_L("litesql::DataSource<") + dest->name +  LITESQL_L(">"));
            get.param(exprParam).param(srcExprParam);
            
            params.clear();
            params.push_back(LITESQL_L("owner->getDatabase()"));
            params.push_back(LITESQL_L("expr"));
            params.push_back(LITESQL_L("(") + rel->getName() +  LITESQL_L("::") + handle.relate->fieldTypeName +  LITESQL_L(" == owner->id)")
                             + extraExpr);
            
            get
                .body(LITESQL_L("return ") + rel->getName() +  LITESQL_L("::") + relate->getMethodName
                      + brackets(params.join(LITESQL_L(", "))) +  LITESQL_L(";"));
            
            hcl.method(get);
        } else {
            if (rel->sameTypes() <= 2) {
                gen::Method getTpl(LITESQL_L("get"),  LITESQL_L("litesql::DataSource<T>"));
                getTpl.template_(LITESQL_L("class T")).defineOnly()
                    .param(exprParam).param(srcExprParam);
                hcl.method(getTpl);                
                for (size_t i2 = 0; i2 < handle.destObjects.size(); i2++) {
                    xml::Object* dest = handle.destObjects[i2].first;
                    xml::Relate* relate = handle.destObjects[i2].second;
                    gen::Method get(LITESQL_L("get"), 
                                LITESQL_L("litesql::DataSource<") + dest->name +  LITESQL_L(">"));
                    get.templateSpec(LITESQL_L("")).param(exprParam).param(srcExprParam);
                    params.clear();
                    params.push_back(LITESQL_L("owner->getDatabase()"));
                    params.push_back(LITESQL_L("expr"));
                    params.push_back(LITESQL_L("(") + rel->getName() +  LITESQL_L("::") + handle.relate->fieldTypeName +  LITESQL_L(" == owner->id)")
                                     + extraExpr);
                    get.body(LITESQL_L("return ") + rel->getName() +  LITESQL_L("::") + relate->getMethodName
                              + brackets(params.join(LITESQL_L(", "))) +  LITESQL_L(";"));
                    hcl.method(get); 
                }    
            } else {
                for (size_t i2 = 0; i2 < handle.destObjects.size(); i2++) {
                    xml::Object* dest = handle.destObjects[i2].first;
                    xml::Relate* relate = handle.destObjects[i2].second;
                    LITESQL_String num = toString(i2 + 1);
                    gen::Method get(LITESQL_L("get") + dest->name + num, 
                                LITESQL_L("litesql::DataSource<") + dest->name +  LITESQL_L(">"));
                    get.param(exprParam).param(srcExprParam);
                    params.clear();
                    params.push_back(LITESQL_L("owner->getDatabase()"));
                    params.push_back(LITESQL_L("expr"));
                    params.push_back(LITESQL_L("(") + rel->getName() +  LITESQL_L("::") + handle.relate->fieldTypeName +  LITESQL_L(" == owner->id)")
                                     + extraExpr);
                    get.body(LITESQL_L("return ") + rel->getName() +  LITESQL_L("::") + relate->getMethodName
                              + brackets(params.join(LITESQL_L(", "))) +  LITESQL_L(";"));
                    hcl.method(get);
                }
            }
        }
        gen::Method getRows(LITESQL_L("getRows"),  LITESQL_L("litesql::DataSource<") 
                       + rel->getName() +  LITESQL_L("::Row>"));
        getRows.param(exprParam)
            .body(LITESQL_L("return ") + rel->getName()
                  +  LITESQL_L("::getRows(owner->getDatabase(), ")
                   LITESQL_L("expr && (") + rel->getName() +  LITESQL_L("::") 
                  + handle.relate->fieldTypeName +
                   LITESQL_L(" == owner->id));"));
        hcl.method(getRows);
        cl.class_(hcl);
        gen::Method hdlMethod(handle.name, o.name +  LITESQL_L("::") + className);
        hdlMethod.body(LITESQL_L("return ") + o.name +  LITESQL_L("::") +  className +  LITESQL_L("(*this);"));
        cl.method(hdlMethod);
    }
}
void writeObjBaseMethods(Class& cl, const xml::Object& o) {
    gen::Method insert(LITESQL_L("insert"),  LITESQL_L("LITESQL_String"));
    insert.protected_()
        .param(Variable(LITESQL_L("tables"),  LITESQL_L("litesql::Record&")))
        .param(Variable(LITESQL_L("fieldRecs"),  LITESQL_L("litesql::Records&")))
        .param(Variable(LITESQL_L("valueRecs"),  LITESQL_L("litesql::Records&")))
        .body(LITESQL_L("tables.push_back(table__);"))
        .body(LITESQL_L("litesql::Record fields;"))
        .body(LITESQL_L("litesql::Record values;"));

    if (o.parentObject)
        insert.body(LITESQL_L("fields.push_back(\"id_\");"))
            .body(LITESQL_L("values.push_back(id);"));

    for (std::vector<xml::Field*>::const_iterator f = o.fields.begin(); f!= o.fields.end(); f++) {
        insert.body(LITESQL_L("fields.push_back(") + (*f)->name +  LITESQL_L(".name()") +  LITESQL_L(");"));
        insert.body(LITESQL_L("values.push_back(") + (*f)->name +  LITESQL_L(");"));
        insert.body((*f)->name +  LITESQL_L(".setModified(false);"));
    }
    
    insert.body(LITESQL_L("fieldRecs.push_back(fields);"))
        .body(LITESQL_L("valueRecs.push_back(values);"));
    if (o.parentObject) {
        insert.body(LITESQL_L("return ") + o.inherits 
                    +  LITESQL_L("::insert(tables, fieldRecs, valueRecs);"));
    } else
        insert.body(LITESQL_L("return ") + o.inherits
                    +  LITESQL_L("::insert(tables, fieldRecs, valueRecs, ") 
                    +  LITESQL_L("sequence__);"));


    gen::Method create(LITESQL_L("create"),  LITESQL_L("void"));
    create.protected_();
    create.body(LITESQL_L("litesql::Record tables;"))
        .body(LITESQL_L("litesql::Records fieldRecs;"))
        .body(LITESQL_L("litesql::Records valueRecs;"))
        .body(LITESQL_L("type = type__;"))
        .body(LITESQL_L("LITESQL_String newID = insert(tables, fieldRecs, valueRecs);"))
        .body(LITESQL_L("if (id == 0)"))
        .body(LITESQL_L("    id = newID;"));


    gen::Method addUpdates(LITESQL_L("addUpdates"),  LITESQL_L("void"));
    addUpdates.protected_().virtual_()
        .param(Variable(LITESQL_L("updates"),  LITESQL_L("Updates&")))
        .body(LITESQL_L("prepareUpdate(updates, table__);"));
    for (size_t i = 0; i < o.fields.size(); i++) {
        const xml::Field& f = *o.fields[i];
        addUpdates.body(LITESQL_L("updateField(updates, table__, ") + f.name +  LITESQL_L(");"));
    }
    if (o.parentObject) 
        addUpdates.body(o.inherits +  LITESQL_L("::addUpdates(updates);"));
    gen::Method addIDUpdates(LITESQL_L("addIDUpdates"),  LITESQL_L("void"));    
    addIDUpdates.protected_().virtual_()
        .param(Variable(LITESQL_L("updates"),  LITESQL_L("Updates&")));
    if (o.parentObject) {
        addIDUpdates
            .body(LITESQL_L("prepareUpdate(updates, table__);"))
            .body(LITESQL_L("updateField(updates, table__, id);"));
        if (o.parentObject->parentObject)
            addIDUpdates.body(o.inherits +  LITESQL_L("::addIDUpdates(updates);"));
    }
    
    gen::Method getFieldTypes(LITESQL_L("getFieldTypes"),  LITESQL_L("void"));
    getFieldTypes.static_().
        param(Variable(LITESQL_L("ftypes"),  LITESQL_L("std::vector<litesql::FieldType>&")));
    if (o.parentObject) 
        getFieldTypes.body(o.parentObject->name +  LITESQL_L("::getFieldTypes(ftypes);"));
    for (size_t i = 0; i < o.fields.size(); i++)
        getFieldTypes.body(LITESQL_L("ftypes.push_back(") + o.fields[i]->fieldTypeName +  LITESQL_L(");"));
    
    gen::Method update(LITESQL_L("update"),  LITESQL_L("void"));
    update.virtual_()
        .body(LITESQL_L("if (!inDatabase) {"))
        .body(LITESQL_L("    create();"))
        .body(LITESQL_L("    return;"))
        .body(LITESQL_L("}"))
        .body(LITESQL_L("Updates updates;"))
        .body(LITESQL_L("addUpdates(updates);"))
        .body(LITESQL_L("if (id != oldKey) {"))
        .body(LITESQL_L("    if (!typeIsCorrect()) "))
        .body(LITESQL_L("        upcastCopy()->addIDUpdates(updates);"));
    if (o.parentObject) 
        update
            .body(LITESQL_L("    else"))
            .body(LITESQL_L("        addIDUpdates(updates);"));
    
    update
        .body(LITESQL_L("}"))
        .body(LITESQL_L("litesql::Persistent::update(updates);"))
        .body(LITESQL_L("oldKey = id;"));

    gen::Method delRecord(LITESQL_L("delRecord"),  LITESQL_L("void"));
    delRecord.protected_().virtual_()
        .body(LITESQL_L("deleteFromTable(table__, id);"));
    if (o.parentObject)
        delRecord.body(o.inherits +  LITESQL_L("::delRecord();"));
    gen::Method delRelations(LITESQL_L("delRelations"),  LITESQL_L("void"));
    delRelations.protected_().virtual_();
    for (std::map<xml::Relation*, std::vector<xml::Relate*> >::const_iterator i = 
             o.relations.begin(); i != o.relations.end(); i++) {
        const xml::Relation * rel = i->first;
        const std::vector<xml::Relate *> relates = i->second;
        Split params;
        for (size_t i2 = 0; i2 < relates.size(); i2++)
            params.push_back(LITESQL_L("(") + rel->getName() +  LITESQL_L("::") + relates[i2]->fieldTypeName
                             +  LITESQL_L(" == id)"));
        delRelations.body(rel->getName() +  LITESQL_L("::del(*db, ") + params.join(LITESQL_L(" || ")) +  LITESQL_L(");"));
    }
        
    

    gen::Method del(LITESQL_L("del"),  LITESQL_L("void"));
    del.virtual_()
        .body(LITESQL_L("if (typeIsCorrect() == false) {"))
        .body(LITESQL_L("    std::auto_ptr<") + o.name +  LITESQL_L("> p(upcastCopy());"))
        .body(LITESQL_L("    p->delRelations();"))
        .body(LITESQL_L("    p->onDelete();"))
        .body(LITESQL_L("    p->delRecord();"))
        .body(LITESQL_L("} else {"))
        .body(LITESQL_L("    onDelete();"))
        .body(LITESQL_L("    delRecord();"))
        .body(LITESQL_L("}"))
        .body(LITESQL_L("inDatabase = false;"));


    gen::Method typeIsCorrect(LITESQL_L("typeIsCorrect"),  LITESQL_L("bool"));    
    typeIsCorrect.body(LITESQL_L("return type == type__;")).virtual_();
    
    gen::Method upcast(LITESQL_L("upcast"),  LITESQL_L("std::auto_ptr<") + o.name +  LITESQL_L(">"));
    gen::Method upcastCopy(LITESQL_L("upcastCopy"),  LITESQL_L("std::auto_ptr<") + o.name +  LITESQL_L(">"));
    Split childrenNames;
    o.getChildrenNames(childrenNames);
    
    if (!childrenNames.empty())
    {

      upcastCopy.body(o.name +  LITESQL_L("* np = NULL;"));
      for (size_t i = 0; i < childrenNames.size(); i++) {
        upcast.body(LITESQL_L("if (type == ") + childrenNames[i] +  LITESQL_L("::type__)"))
            .body(LITESQL_L("    return std::auto_ptr<") + o.name +  LITESQL_L(">(new ") + childrenNames[i] 
                  +  LITESQL_L("(select<") + childrenNames[i] 
                  +  LITESQL_L(">(*db, Id == id).one()));"));
        upcastCopy.body(LITESQL_L("if (type == ") + quote(childrenNames[i]) +  LITESQL_L(")"))
            .body(LITESQL_L("    np = new ") + childrenNames[i] +  LITESQL_L("(*db);"));
      }
    }
    else
    {
      upcastCopy.body(o.name +  LITESQL_L("* np = new ") + o.name +  LITESQL_L("(*this);"));
    }
    
    for (size_t i = 0; i < o.fields.size(); i++) {
        upcastCopy.body(LITESQL_L("np->") + o.fields[i]->name +  LITESQL_L(" = ") 
                        + o.fields[i]->name +  LITESQL_L(";"));
    }
    upcastCopy
        .body(LITESQL_L("np->inDatabase = inDatabase;"))
        .body(LITESQL_L("return std::auto_ptr<") + o.name +  LITESQL_L(">(np);"));
    
    upcast.body(LITESQL_L("return std::auto_ptr<") + o.name 
                +  LITESQL_L(">(new ") + o.name +  LITESQL_L("(*this));"));

    for (size_t i = 0; i < o.methods.size(); i++) {
        const xml::Method& m = *o.methods[i];
        gen::Method mtd(m.name, m.returnType.empty() ?  LITESQL_L("void") : m.returnType);
        for (size_t i2 = 0; i2 < m.params.size(); i2++) {
            const xml::Param& p = m.params[i2];

            LITESQL_String tempString;
            LITESQL_ConvertString(p.type, tempString);
            mtd.param(Variable(p.name, tempString));
        
        }
        mtd.defineOnly().virtual_();
        cl.method(mtd);
    }
    
    cl.method(insert).method(create).method(addUpdates)
        .method(addIDUpdates).method(getFieldTypes).method(delRecord).method(delRelations)
         .method(update).method(del)
        .method(typeIsCorrect).method(upcast).method(upcastCopy);

}

void writeStaticRelData(Class& cl, const xml::Relation& r) {
    Variable table(LITESQL_L("table__"),  LITESQL_L("const LITESQL_String"), 
                   quote(r.getTable()));
    table.static_();
    cl.variable(table);
    gen::Method initValues(LITESQL_L("initValues"),  LITESQL_L("void"));
    bool hasValues = false;
    initValues.static_();
    for (size_t i2 = 0; i2 < r.related.size(); i2++) {
        Variable ftype(r.related[i2]->fieldTypeName,
                        LITESQL_L("const litesql::FieldType"),
                       quote(r.related[i2]->fieldName)
                       +  LITESQL_L(",") + quote(LITESQL_L("INTEGER")) +  LITESQL_L(",") +  LITESQL_L("table__"));
        ftype.static_();
        cl.variable(ftype);
    }
    for (size_t i2 = 0; i2 < r.fields.size(); i2++) {
        const xml::Field& fld = *r.fields[i2];
        LITESQL_String data = quote(fld.name +  LITESQL_L("_")) +  LITESQL_L(",") +
                      quote(fld.getSQLType()) +  LITESQL_L(",") +

                       LITESQL_L("table__");
        if (!fld.values.empty()) {
            data +=  LITESQL_L(",") + fld.name +  LITESQL_L("_values"); 
            Variable values(fld.name +  LITESQL_L("_values"), 
                             LITESQL_L("std::vector < std::pair< LITESQL_String, LITESQL_String > >"));
            values.static_().protected_();
            cl.variable(values);
            initValues.body(fld.name +  LITESQL_L("_values.clear();"));
            hasValues = true;
            for (size_t i2 = 0; i2 < fld.values.size(); i2++) {
                const xml::Value& val = fld.values[i2];
                initValues.body(fld.name +  LITESQL_L("_values.push_back(make_pair(")
                          + quote(val.name) +  LITESQL_L(",") + quote(val.value) +  LITESQL_L("));"));    
            }

        }
        LITESQL_String ftypeClass = LITESQL_L("const litesql::FieldType");
        if (!fld.values.empty()) {
            ftypeClass = fld.fieldTypeName +  LITESQL_L("Type");
            Class ftypeCl(ftypeClass,  LITESQL_L("litesql::FieldType"));
            gen::Method cons(ftypeClass);
   	        ftypeClass =  LITESQL_L("const ") + r.getName() +  LITESQL_L("::") + ftypeClass;
            cons.param(Variable(LITESQL_L("n"),  LITESQL_L("const LITESQL_String&")))
                .param(Variable(LITESQL_L("t"),  LITESQL_L("const LITESQL_String&")))
                .param(Variable(LITESQL_L("tbl"),  LITESQL_L("const LITESQL_String&")))
                .param(Variable(LITESQL_L("vals"),  LITESQL_L("const litesql::FieldType::Values&"),  LITESQL_L("Values()")))
                .constructor(LITESQL_L("litesql::FieldType(n,t,tbl,vals)"));
            
            ftypeCl.method(cons);
            for (size_t i = 0; i < fld.values.size(); i++) {
                const xml::Value& value = fld.values[i];
                LITESQL_String v;
                if (fld.getCPPType() ==  LITESQL_L("LITESQL_String"))
                    v = quote(value.value);
                else
                    v = value.value;
                Variable val(value.name,  LITESQL_L("const ") + fld.getCPPType(), v);
                
                val.static_();
                ftypeCl.variable(val);
            }
            cl.class_(ftypeCl);
        }
        Variable ftype(fld.fieldTypeName, ftypeClass, data);

        ftype.static_();
        cl.variable(ftype);
    }
    if (hasValues)
        cl.method(initValues);
    Class rowcl(LITESQL_L("Row"));
    gen::Method rowcons(LITESQL_L("Row"));
    rowcons.param(Variable(LITESQL_L("db"),  LITESQL_L("const litesql::Database&")))
        .param(Variable(LITESQL_L("rec"),  LITESQL_L("const litesql::Record&"),  LITESQL_L("litesql::Record()")));
//        .constructor(LITESQL_L("litesql::Record(db, rec)"));
    Split consParams;
    int fieldNum = r.related.size() + r.fields.size();
    rowcons.body(LITESQL_L("switch(rec.size()) {"));
    for (int i = r.fields.size()-1; i >= 0; i--) {
        const xml::Field& fld = *r.fields[i];
        Variable fldvar(fld.name,  LITESQL_L("litesql::Field<") + fld.getCPPType() +  LITESQL_L(">"));
        rowcl.variable(fldvar);        

        rowcons.body(LITESQL_L("case ") + toString(fieldNum) +  LITESQL_L(":"))
            .body(LITESQL_L("    ") + fld.name +  LITESQL_L(" = rec[") + toString(fieldNum-1) +  LITESQL_L("];"));
        consParams.push_back(fld.name +
                              LITESQL_L("(") + r.getName() +  LITESQL_L("::") + fld.fieldTypeName +  LITESQL_L(")"));
        fieldNum--;
        
    }
    for (int i = r.related.size()-1; i >= 0; i--) {
        const xml::Relate& rel = *r.related[i];
        LITESQL_String fname = xml::decapitalize(rel.fieldTypeName);
        Variable fld(fname,  LITESQL_L("litesql::Field<int>"));
        rowcl.variable(fld);
        rowcons.body(LITESQL_L("case ") + toString(fieldNum) +  LITESQL_L(":"))
            .body(LITESQL_L("    ") + fname +  LITESQL_L(" = rec[") 
                  + toString(fieldNum-1) +  LITESQL_L("];")); 

        consParams.push_back(fname 
                             +  LITESQL_L("(") + r.getName() 
                             +  LITESQL_L("::") + rel.fieldTypeName + LITESQL_L(")"));
        fieldNum--;
    }
    rowcons.body(LITESQL_L("}"));
    rowcons.constructor(consParams.join(LITESQL_L(", ")));
    rowcl.method(rowcons);    
    cl.class_(rowcl);

}
void writeRelMethods(const xml::Database& database,
		     Class& cl, xml::Relation& r) {
    Variable dbparam(LITESQL_L("db"),  LITESQL_L("const litesql::Database&"));
    Variable destExpr(LITESQL_L("expr"),  LITESQL_L("const litesql::Expr&"), 
                        LITESQL_L("litesql::Expr()"));
    Variable srcExpr(LITESQL_L("srcExpr"),  LITESQL_L("const litesql::Expr&"), 
                        LITESQL_L("litesql::Expr()"));
    gen::Method link(LITESQL_L("link"),  LITESQL_L("void"));
    gen::Method unlink(LITESQL_L("unlink"),  LITESQL_L("void"));
    gen::Method del(LITESQL_L("del"),  LITESQL_L("void"));
    gen::Method getRows(LITESQL_L("getRows"), 
                    LITESQL_L("litesql::DataSource<")+r.getName()+ LITESQL_L("::Row>"));
    
    link.static_().param(dbparam);
    link.body(LITESQL_L("Record values;"))
        .body(LITESQL_L("Split fields;"));
    for (size_t i = 0; i < r.related.size(); i++) {
        xml::Relate& rel = *r.related[i];
        link.body(LITESQL_L("fields.push_back(") + rel.fieldTypeName +  LITESQL_L(".name());"));
        link.body(LITESQL_L("values.push_back(o") + toString(i) +  LITESQL_L(".id);"));
        rel.paramPos = i;
    }
    for (size_t i = 0; i < r.fields.size(); i++) {
        xml::Field& fld = *r.fields[i];
        
        link.body(LITESQL_L("fields.push_back(") + fld.fieldTypeName +  LITESQL_L(".name());"));
        if (fld.getCPPType() !=  LITESQL_L("LITESQL_String"))
            link.body(LITESQL_L("values.push_back(toString(") + fld.name +  LITESQL_L("));"));
        else
            link.body(LITESQL_L("values.push_back(") + fld.name +  LITESQL_L(");"));
    }
    link.body(LITESQL_L("db.insert(table__, values, fields);"));
    if (r.isUnidir()==false && r.related.size() == 2 && r.sameTypes() == 2) {
        link.body(LITESQL_L("fields.clear();"))
            .body(LITESQL_L("values.clear();"));
        for (size_t i = 0; i < r.related.size(); i++) {
            xml::Relate& rel = *r.related[i];
            link.body(LITESQL_L("fields.push_back(") + rel.fieldTypeName +  LITESQL_L(".name());"));
            link.body(LITESQL_L("values.push_back(o") + toString(1-i) +  LITESQL_L(".id);"));
        }
        for (size_t i = 0; i < r.fields.size(); i++) {
            xml::Field& fld = *r.fields[i];
        
            link.body(LITESQL_L("fields.push_back(") + fld.fieldTypeName +  LITESQL_L(".name());"));
            if (fld.getCPPType() !=  LITESQL_L("LITESQL_String"))
                link.body(LITESQL_L("values.push_back(toString(") + fld.name +  LITESQL_L("));"));
            else
                link.body(LITESQL_L("values.push_back(") + fld.name +  LITESQL_L(");"));
        }
        link.body(LITESQL_L("db.insert(table__, values, fields);"));
    }

    unlink.static_().param(dbparam);
    Split unlinks;
    for (size_t i = 0; i < r.related.size(); i++) {
        xml::Relate& rel = *r.related[i];
        unlinks.push_back(rel.fieldTypeName +  LITESQL_L(" == o") 
                          + toString(i) +  LITESQL_L(".id"));
    }
    for (size_t i = 0; i < r.fields.size(); i++) {
        xml::Field& fld = *r.fields[i];
        unlinks.push_back(LITESQL_L("(") + fld.fieldTypeName +  LITESQL_L(" == ") + fld.name +  LITESQL_L(")"));
    }
    
    unlink.body(LITESQL_L("db.delete_(table__, (") + unlinks.join(LITESQL_L(" && ")) +  LITESQL_L("));"));
    if (r.isUnidir()==false && r.related.size() == 2 && r.sameTypes() == 2) {
        unlinks.clear();
        for (size_t i = 0; i < r.related.size(); i++) {
            xml::Relate& rel = *r.related[i];
            unlinks.push_back(rel.fieldTypeName +  LITESQL_L(" == o") 
                              + toString(1-i) +  LITESQL_L(".id"));
        }
        for (size_t i = 0; i < r.fields.size(); i++) {
            xml::Field& fld = *r.fields[i];
            unlinks.push_back(LITESQL_L("(") + fld.fieldTypeName +  LITESQL_L(" == ") + fld.name +  LITESQL_L(")"));
        }
        unlink.body(LITESQL_L("db.delete_(table__, (") + unlinks.join(LITESQL_L(" && ")) +  LITESQL_L("));"));
    }

    del.static_().param(dbparam).param(destExpr); 
    del.body(LITESQL_L("db.delete_(table__, expr);"));
    getRows.static_().param(dbparam).param(destExpr)
        .body(LITESQL_L("SelectQuery sel;"));

    for (size_t i = 0; i < r.related.size(); i++) {
        xml::Relate& rel = *r.related[i];        
        getRows.body(LITESQL_L("sel.result(") + rel.fieldTypeName +  LITESQL_L(".fullName());"));
    }
    for (size_t i = 0; i < r.fields.size(); i++) {
        xml::Field& fld = *r.fields[i];
        getRows.body(LITESQL_L("sel.result(") + fld.fieldTypeName +  LITESQL_L(".fullName());"));
    }
    getRows.body(LITESQL_L("sel.source(table__);"))
        .body(LITESQL_L("sel.where(expr);"))
        .body(LITESQL_L("return DataSource<") + r.getName() +  LITESQL_L("::Row>(db, sel);"));

    for (size_t i2 = 0; i2 < r.related.size(); i2++) {
        xml::Relate& rel = *r.related[i2];
        Variable obj(LITESQL_L("o") + toString(i2),  LITESQL_L("const ") +  database.nspace +  LITESQL_L("::") +rel.objectName +  LITESQL_L("&"));
        link.param(obj);
        unlink.param(obj);
    }
    for (size_t i2 = 0; i2 < r.fields.size(); i2++) {
        xml::Field& fld = *r.fields[i2];
        link.param(Variable(fld.name, fld.getCPPType(), 
                            fld.getQuotedDefaultValue()));
        unlink.param(Variable(fld.name,  fld.getCPPType()));
    }
    cl.method(link).method(unlink).method(del).method(getRows);
    if (r.sameTypes() == 1) {
        gen::Method getTpl(LITESQL_L("get"),  LITESQL_L("litesql::DataSource<T>"));
        getTpl.static_().template_(LITESQL_L("class T")).defineOnly()
            .param(dbparam).param(destExpr).param(srcExpr);
        cl.method(getTpl);
        for (size_t i2 = 0; i2 < r.related.size(); i2++) {
            xml::Relate& rel = *r.related[i2];
            gen::Method get(LITESQL_L("get"),  LITESQL_L("litesql::DataSource<") 
                       + database.nspace +  LITESQL_L("::") + rel.objectName +  LITESQL_L(">"));
            rel.getMethodName =  LITESQL_L("get<") + rel.objectName +  LITESQL_L(">");
            get.static_().templateSpec(LITESQL_L(""))
                .param(dbparam).param(destExpr).param(srcExpr)
                .body(LITESQL_L("SelectQuery sel;"))
                .body(LITESQL_L("sel.source(table__);"))
                .body(LITESQL_L("sel.result(") + rel.fieldTypeName +  LITESQL_L(".fullName());"))
                .body(LITESQL_L("sel.where(srcExpr);"))
                .body(LITESQL_L("return DataSource<") + database.nspace +  LITESQL_L("::") + rel.objectName 
                      +  LITESQL_L(">(db, ")+database.nspace +  LITESQL_L("::") + rel.objectName+ LITESQL_L("::Id.in(sel) && expr);"));
            cl.method(get);
        }
    } else {
        std::map<LITESQL_String, int> counter;
        for (size_t i2 = 0; i2 < r.related.size(); i2++) {
            LITESQL_String num;
            xml::Relate& rel = *r.related[i2];
            if (r.countTypes(rel.objectName) > 1) {
                if (counter.find(rel.objectName) == counter.end())
                    counter[rel.objectName] = 0;
                counter[rel.objectName]++;
                num = toString(counter[rel.objectName]);        
            }
            rel.getMethodName =  LITESQL_L("get") + rel.objectName + num;
            gen::Method get(rel.getMethodName, 
                        LITESQL_L("litesql::DataSource<") + database.nspace +  LITESQL_L("::") 
                        + rel.objectName +  LITESQL_L(">"));

            get.static_()
                .param(dbparam).param(destExpr).param(srcExpr)
                .body(LITESQL_L("SelectQuery sel;"))
                .body(LITESQL_L("sel.source(table__);"))
                .body(LITESQL_L("sel.result(") + rel.fieldTypeName +  LITESQL_L(".fullName());"))
                .body(LITESQL_L("sel.where(srcExpr);"))
                .body(LITESQL_L("return DataSource<") + database.nspace +  LITESQL_L("::") 
                     + rel.objectName 
                      +  LITESQL_L(">(db, ")+database.nspace +  LITESQL_L("::") + rel.objectName+ LITESQL_L("::Id.in(sel) && expr);"));
            cl.method(get);
        }
    }


}





void getSchema(const xml::Database& db,
               //const std::vector<xml::Object*>& objects,
//               const std::vector<xml::Relation*>& relations,
gen::Method& mtd) {    
//    Records recs;
    Split rec;
    mtd.body(LITESQL_L("std::vector<Database::SchemaItem> res;"));
    rec.push_back(quote(LITESQL_L("schema_")));
    rec.push_back(quote(LITESQL_L("table")));
    rec.push_back(quote(LITESQL_L("CREATE TABLE schema_ (name_ TEXT, type_ TEXT, sql_ TEXT);")));
//    recs.push_back(rec);
    mtd.body(LITESQL_L("res.push_back(Database::SchemaItem(") + rec.join(LITESQL_L(",")) +  LITESQL_L("));"));
    mtd.body(LITESQL_L("if (backend->supportsSequences()) {"));
    for (size_t i = 0; i < db.sequences.size(); i++) {
        Split rec;
        rec.push_back(quote(db.sequences[i]->name));
        rec.push_back(quote(LITESQL_L("sequence")));
        rec.push_back(quote(db.sequences[i]->getSQL()));
        mtd.body(LITESQL_L("    res.push_back(Database::SchemaItem(") + rec.join(LITESQL_L(",")) +  LITESQL_L("));"));
    }
    mtd.body(LITESQL_L("}"));
    for (size_t i = 0; i < db.tables.size(); i++) {
        Split rec;
        rec.push_back(quote(db.tables[i]->name));
        rec.push_back(quote(LITESQL_L("table")));
        rec.push_back(quote(db.tables[i]->getSQL(LITESQL_L("\" + backend->getRowIDType() + \""))));
        mtd.body(LITESQL_L("res.push_back(Database::SchemaItem(") + rec.join(LITESQL_L(",")) +  LITESQL_L("));"));
    }
    for (size_t i = 0; i < db.indices.size(); i++) {
        Split rec;
        rec.push_back(quote(db.indices[i]->name));
        rec.push_back(quote(LITESQL_L("index")));
        rec.push_back(quote(db.indices[i]->getSQL()));
        mtd.body(LITESQL_L("res.push_back(Database::SchemaItem(") + rec.join(LITESQL_L(",")) +  LITESQL_L("));"));        
    }
    Logger::report(toString(db.tables.size()) +  LITESQL_L(" tables\n"));
    Logger::report(toString(db.sequences.size()) +  LITESQL_L(" sequences\n"));
    Logger::report(toString(db.indices.size()) +  LITESQL_L(" indices\n"));
}
void writeDatabaseClass(FILE* hpp, FILE* cpp,
                        const xml::Database& dbInfo,
                        const std::vector<xml::Object*>& objects,
                        const std::vector<xml::Relation*>& relations) {
    gen::Class db(dbInfo.name,  LITESQL_L("litesql::Database"));    
    gen::Method cons(dbInfo.name);
    
    cons.param(Variable(LITESQL_L("backendType"),  LITESQL_L("LITESQL_String")))
        .param(Variable(LITESQL_L("connInfo"),  LITESQL_L("LITESQL_String")))
        .constructor(LITESQL_L("litesql::Database(backendType, connInfo)"));
    cons.body(LITESQL_L("initialize();"));
    db.method(cons);
    gen::Method getSchemaMtd(LITESQL_L("getSchema"),  LITESQL_L("std::vector<litesql::Database::SchemaItem>"));
    getSchemaMtd.virtual_().protected_().const_();

    getSchema(dbInfo, /*objects, relations, */ getSchemaMtd);
/*    for (Records::iterator i = schema.begin(); i != schema.end(); i++) {
        if ((*i)[1] == quote(LITESQL_L("sequence")))
            getSchemaMtd.body(LITESQL_L("if (backend->supportsSequences())"));
        getSchemaMtd.body(LITESQL_L("res.push_back(Database::SchemaItem(") 
                       + (*i)[0] +  LITESQL_L(",") 
                       + (*i)[1] +  LITESQL_L(",")
                       + (*i)[2] +  LITESQL_L("));"));
    } */
    
    getSchemaMtd.body(LITESQL_L("return res;"));

    db.method(getSchemaMtd);
    gen::Method init(LITESQL_L("initialize"),  LITESQL_L("void"));
    init.body(LITESQL_L("static bool initialized = false;"))
        .body(LITESQL_L("if (initialized)"))
        .body(LITESQL_L("    return;"))
        .body(LITESQL_L("initialized = true;"));
    
    for (size_t i = 0; i < objects.size(); i++) {
        xml::Object& o = *objects[i];
        for (size_t i2 = 0; i2 < o.fields.size(); i2++)
            if (!o.fields[i2]->values.empty()) {
                init.body(o.name +  LITESQL_L("::initValues();"));
                break;
            }
    }
    for (size_t i = 0; i < relations.size(); i++) {
        xml::Relation& r = *relations[i];
        for (size_t i2 = 0; i2 < r.fields.size(); i2++)
            if (!r.fields[i2]->values.empty()) {
                init.body(r.getName() +  LITESQL_L("::initValues();"));
                break;
            }
    }

    init.protected_().static_();
    db.method(init);
    db.write(hpp, cpp);
}

void CppGenerator::writeCPPClasses(const ObjectModel* model) 
{
    LITESQL_String hppName = toLower(model->db.name) +  LITESQL_L(".hpp");
    LITESQL_String defName = toLower(model->db.name) +  LITESQL_L("_hpp");
    _ftprintf(hpp,  LITESQL_L("#ifndef %s\n"), defName.c_str());
    _ftprintf(hpp,  LITESQL_L("#define %s\n"), defName.c_str());
    fprintf(hpp, "#include \"litesql.hpp\"\n");
    if (!model->db.include.empty()) 
        fprintf(hpp, "#include \"%s\"\n", model->db.include.c_str());
    fprintf(cpp, "#include \"%s\"\n", hppName.c_str());
    
    if (model->db.hasNamespace()) {
        _ftprintf(hpp,  LITESQL_L("namespace %s {\n"), model->db.nspace.c_str());
        _ftprintf(cpp,  LITESQL_L("namespace %s {\n"), model->db.nspace.c_str());
    } 
    
    _ftprintf(cpp,  LITESQL_L("using namespace litesql;\n"));

    Logger::report(LITESQL_L("writing prototypes for Persistent classes\n")); 
    for (size_t i = 0; i < model->objects.size(); i++) 
        _ftprintf(hpp,  LITESQL_L("class %s;\n"), model->objects[i]->name.c_str());

    Logger::report(LITESQL_L("writing relations\n"));
    for (std::vector<xml::Relation*>::const_iterator it = model->relations.begin(); it!= model->relations.end(); it++) {
        Class cl((*it)->getName());
        writeStaticRelData(cl, **it);
        writeRelMethods(model->db, cl, **it);
        
        cl.write(hpp, cpp);
    }
    Logger::report(LITESQL_L("writing persistent objects\n"));
    for (std::vector<xml::Object*>::const_iterator it_o=model->objects.begin(); it_o!=model->objects.end(); it_o++) {
        Class cl((*it_o)->name, (*it_o)->inherits);
        writeStaticObjData(cl, **it_o);
        writeObjFields(cl, **it_o);       
        writeObjConstructors(cl, **it_o);
        writeObjRelationHandles(cl, **it_o);
        writeObjBaseMethods(cl, **it_o);
        cl.write(hpp, cpp);

        // Object -> LITESQL_String method (not associated to class)

        gen::Method strMtd(LITESQL_L("operator<<"),  LITESQL_L("LITESQL_oStream &"));
        strMtd.param(Variable(LITESQL_L("os"),  LITESQL_L("LITESQL_oStream&")))
            .param(Variable(LITESQL_L("o"), (*it_o)->name));
        std::vector<xml::Field*> flds;
        (**it_o).getAllFields(flds);

        strMtd.body(LITESQL_L("os << \"-------------------------------------\" << std::endl;"));
        for (size_t i2 = 0; i2 < flds.size(); i2++) {
            xml::Field& fld = *flds[i2];
            strMtd.body(LITESQL_L("os << o.") + fld.name +  LITESQL_L(".name() << \" = \" << o.") 
                        + fld.name +  LITESQL_L(" << std::endl;"));
        }
        strMtd.body(LITESQL_L("os << \"-------------------------------------\" << std::endl;"));
        strMtd.body(LITESQL_L("return os;"));
        
        strMtd.write(hpp, cpp,  LITESQL_L(""), 0);
    }
    Logger::report(LITESQL_L("writing database class\n"));
    writeDatabaseClass(hpp, cpp, model->db, model->objects, model->relations);
    if (model->db.hasNamespace()) {
        _ftprintf(hpp,  LITESQL_L("}\n"));
        _ftprintf(cpp,  LITESQL_L("}\n"));
    }

    _ftprintf(hpp,  LITESQL_L("#endif\n"));
}

void CppGenerator::setOutputSourcesDirectory(const LITESQL_String& directory)
{
  m_sources_directory= directory;
}

void CppGenerator::setOutputIncludesDirectory(const LITESQL_String& directory)
{ 
  m_includes_directory = directory;
}

const LITESQL_String& CppGenerator::getOutputSourcesDirectory() const
{
  return (m_sources_directory.empty()) ? getOutputDirectory() : m_sources_directory;
}

const LITESQL_String& CppGenerator::getOutputIncludesDirectory() const
{
  return (m_includes_directory.empty()) ? getOutputDirectory() : m_includes_directory;
}

LITESQL_String CppGenerator::getOutputSourcesFilename(const LITESQL_String& name) const
{
  LITESQL_String fname = getOutputSourcesDirectory();

  if (!fname.empty())
  {
#ifdef WIN32
    fname.append(LITESQL_L("\\"));
#else
    fname.append(LITESQL_L("/"));
#endif // #ifdef _WINDOWS_
  }
  fname.append(name); 
  return fname;
}

LITESQL_String CppGenerator::getOutputIncludesFilename(const LITESQL_String& name) const
{
  LITESQL_String fname = getOutputIncludesDirectory();

  if (!fname.empty())
  {
#ifdef WIN32
    fname.append(LITESQL_L("\\"));
#else
    fname.append(LITESQL_L("/"));
#endif // #ifdef _WINDOWS_
  }
  fname.append(name); 
  return fname;
}

bool CppGenerator::generateCode(const ObjectModel* model)
{
  sanityCheck(model->db, model->objects, model->relations);

  LITESQL_String hppName = getOutputIncludesFilename(toLower(model->db.name)+  LITESQL_L(".hpp"));

  hpp = _tfopen(hppName.c_str(),  LITESQL_L("w"));
  if (!hpp) {
    LITESQL_String msg =  LITESQL_L("could not open file : ") + hppName;
    _tperror(msg.c_str());
    return false;
  }

  LITESQL_String cppName = getOutputSourcesFilename(toLower(model->db.name)+  LITESQL_L(".cpp"));
  cpp = _tfopen(cppName.c_str(),  LITESQL_L("w"));
  if (!cpp) {
    LITESQL_String msg =  LITESQL_L("could not open file : ") + cppName;
    _tperror(msg.c_str());
    return false;
  }

  writeCPPClasses(model);

  fclose(hpp);
  fclose(cpp);

  return true;
}