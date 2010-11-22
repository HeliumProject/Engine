##----------------------------------------------------------------------------------------------------------------------
## JobDefParser.py
##
## Copyright (C) 2010 WhiteMoon Dreams, Inc.
## All Rights Reserved
##----------------------------------------------------------------------------------------------------------------------

import os
import sys
import datetime
import string
from xml.parsers import expat

def PrintCommandLineUsage():
    print( 'Usage:', os.path.basename( sys.argv[ 0 ] ), 'jobDefPath includePath sourcePath', file = sys.stderr )
    print( '    jobDefPath   Path the directory containing the .jobdef files to parse', file = sys.stderr )
    print( '    includePath  Base path of the "Include" file directory', file = sys.stderr )
    print( '    sourcePath   Base path of the "Source" file directory', file = sys.stderr )

def ModuleToTokenName( moduleName ):
    lastCharacter = None
    wasLastCharacterUpper = False
    wasLastCharacterDot = False
    tokenName = ''
    for rawCharacter in moduleName:
        character = rawCharacter.upper()
        isCharacterUpper = ( character == rawCharacter )
        if wasLastCharacterDot or (not isCharacterUpper and wasLastCharacterUpper and tokenName != ''):
            tokenName += '_'

        if lastCharacter is not None:
            tokenName += lastCharacter

        lastCharacter = character
        wasLastCharacterUpper = isCharacterUpper
        wasLastCharacterDot = (lastCharacter == '.')
        if wasLastCharacterDot:
            lastCharacter = None

    if lastCharacter is not None:
        tokenName += lastCharacter

    return tokenName

def FindNamedElement( array, name ):
    elementCount = len( array )
    elementIndex = 0
    while elementIndex < elementCount:
        if array[ elementIndex ].name == name:
            return elementIndex

        elementIndex += 1

    return -1

class JobTemplateParam( object ):
    def __init__( self ):
        self.name = None
        self.type = None
        self.default = None

class JobConstant( object ):
    def __init__( self ):
        self.name = None
        self.type = None
        self.value = None
        self.description = None

class JobParameter( object ):
    TYPE_INPUT, TYPE_INOUT, TYPE_OUTPUT = range( 3 )

    def __init__( self ):
        self.name = None
        self.paramType = JobParameter.TYPE_INPUT
        self.type = None
        self.default = None
        self.description = None

class JobInfo( object ):
    def __init__( self ):
        self.name = None
        self.parent = None
        self.description = None
        self.templateParams = []
        self.constants = []
        self.parameters = []

class JobDefParseHandler( object ):
    def __init__( self ):
        self.levelCount = 0
        self.errorLevelCount = 0
        self.inJoblistBlock = False
        self.parsingJob = None
        self.namespace = None
        self.moduleTokenPrefix = ''
        self.includes = []
        self.inlines = []
        self.forwardDeclarations = {}
        self.jobs = []
        self.inParametersBlock = False

    def ValidateAttribute( self, name, attribs, attribName ):
        parseError = ( attribName not in attribs )
        if parseError:
            print(
                '[E] Parse error: Encountered "', name, '" block missing a "', attribName, '" attribute.',
                sep = '',
                file = sys.stderr )
            self.errorLevelCount += 1
        return parseError

    def StartElement( self, name, attribs ):
        parseError = False

        # Don't perform any processing if we are within an invalid tag block.
        if self.errorLevelCount != 0:
            self.errorLevelCount += 1
            self.levelCount += 1
            return

        if self.levelCount == 0:
            self.inJobListBlock = ( name == 'joblist' )
            if self.inJobListBlock:
                print( '[I] Entering "joblist" block.' )
        elif self.levelCount == 1:
            if self.inJobListBlock:
                if name == 'namespace':
                    parseError = self.ValidateAttribute( name, attribs, 'name' ) or parseError
                    if not parseError:
                        self.namespace = attribs[ 'name' ]
                        print( '[I] Setting code namespace to ', self.namespace, '.', sep = '' )
                elif name == 'moduletokenprefix':
                    parseError = self.ValidateAttribute( name, attribs, 'name' ) or parseError
                    if not parseError:
                        self.moduleTokenPrefix = attribs[ 'name' ]
                        print( '[I] Setting module token prefix to ', self.moduleTokenPrefix, '.', sep = '' )
                elif name == 'include':
                    parseError = self.ValidateAttribute( name, attribs, 'file' ) or parseError
                    if not parseError:
                        includeFile = attribs[ 'file' ]
                        print( '[I] Adding include file ', includeFile, '.', sep = '' )
                        self.includes.append( includeFile )
                elif name == 'inline':
                    parseError = self.ValidateAttribute( name, attribs, 'file' ) or parseError
                    if not parseError:
                        inlineFile = attribs[ 'file' ]
                        print( '[I] Adding inline include file ', inlineFile, '.', sep = '' )
                        self.inlines.append( inlineFile )
                elif name == 'forwarddeclare':
                    parseError = self.ValidateAttribute( name, attribs, 'type' ) or parseError
                    if not parseError:
                        declareType = attribs[ 'type' ]
                        if 'namespace' in attribs:
                            declareNamespace = attribs[ 'namespace' ]
                            print(
                                '[I] Adding forward declaration for "', declareType, '" in namespace "',
                                    declareNamespace, '".',
                                sep = '' )
                        else:
                            declareNamespace = 0
                            print( '[I] Adding forward declaration for "', declareType, '".', sep = '' )

                        if declareNamespace in self.forwardDeclarations:
                            self.forwardDeclarations[ declareNamespace ].append( declareType )
                        else:
                            self.forwardDeclarations[ declareNamespace ] = [ declareType ]
                elif name == 'job':
                    parseError = self.ValidateAttribute( name, attribs, 'name' ) or parseError
                    if not parseError:
                        self.parsingJob = JobInfo()
                        self.parsingJob.name = attribs[ 'name' ]

                        if 'parent' in attribs:
                            self.parsingJob.parent = attribs[ 'parent' ]
                        
                        if 'description' in attribs:
                            self.parsingJob.description = attribs[ 'description' ]

                        print(
                            '[I] Entering "job" block (name = ', self.parsingJob.name, ', parent = ',
                                self.parsingJob.parent, ', description = ', self.parsingJob.description, ').',
                            sep = '' )
        elif self.levelCount == 2:
            if self.parsingJob is not None:
                if name == 'templateparam':
                    parseError = self.ValidateAttribute( name, attribs, 'name' ) or parseError
                    if not parseError:
                        jobTemplateParam = JobTemplateParam()
                        jobTemplateParam.name = attribs[ 'name' ]

                        if 'type' in attribs:
                            jobTemplateParam.type = attribs[ 'type' ]
                        else:
                            jobTemplateParam.type = 'typename'

                        if 'default' in attribs:
                            jobTemplateParam.default = attribs[ 'default' ]

                        print(
                            '[I] Adding template parameter to ', self.parsingJob.name, ' (name = ',
                                jobTemplateParam.name, ').',
                            sep = '' )
                        templateParamIndex = FindNamedElement( self.parsingJob.templateParams, jobTemplateParam.name )
                        if templateParamIndex >= 0:
                            del self.parsingJob.templateParams[ templateParamIndex ]

                        self.parsingJob.templateParams.append( jobTemplateParam )
                elif name == 'constant':
                    parseError = self.ValidateAttribute( name, attribs, 'name' ) or parseError
                    parseError = self.ValidateAttribute( name, attribs, 'type' ) or parseError
                    parseError = self.ValidateAttribute( name, attribs, 'value' ) or parseError
                    if not parseError:
                        jobConstant = JobConstant()
                        jobConstant.name = attribs[ 'name' ]
                        jobConstant.type = attribs[ 'type' ]
                        jobConstant.value = attribs[ 'value' ]
                        
                        if 'description' in attribs:
                            jobConstant.description = attribs[ 'description' ]

                        print(
                            '[I] Adding constant to ', self.parsingJob.name, ' (name = ', jobConstant.name, ', type = ',
                                jobConstant.type, ', value = ', jobConstant.value, ').',
                            sep = '' )
                        constantIndex = FindNamedElement( self.parsingJob.constants, jobConstant.name )
                        if constantIndex >= 0:
                            del self.parsingJob.constants[ constantIndex ]

                        self.parsingJob.constants.append( jobConstant )
                elif name == 'parameters':
                    if self.parsingJob.parent is not None:
                        print( '[W] Parameters cannot be specified for derived job types.' )
                    else:
                        self.inParametersBlock = True
        elif self.levelCount == 3:
            if self.parsingJob is not None:
                if self.inParametersBlock:
                    if name == 'input' or name == 'inout' or name == 'output':
                        parseError = self.ValidateAttribute( name, attribs, 'name' ) or parseError
                        parseError = self.ValidateAttribute( name, attribs, 'type' ) or parseError
                        if not parseError:
                            jobParameter = JobParameter()
                            jobParameter.name = attribs[ 'name' ]
                            if name == 'input':
                                jobParameter.paramType = JobParameter.TYPE_INPUT
                            elif name == 'inout':
                                jobParameter.paramType = JobParameter.TYPE_INOUT
                            else:
                                jobParameter.paramType = JobParameter.TYPE_OUTPUT

                            jobParameter.type = attribs[ 'type' ]

                            if 'default' in attribs:
                                jobParameter.default = attribs[ 'default' ]

                            if 'description' in attribs:
                                jobParameter.description = attribs[ 'description' ]

                            print(
                                '[I] Adding ', name, ' parameter to ', self.parsingJob.name, ' (name = ',
                                    jobParameter.name, ', type = ', jobParameter.type, ', defaultValue = ',
                                    jobParameter.default, ', description = ', jobParameter.description, ').',
                                sep = '' )
                            parameterIndex = FindNamedElement( self.parsingJob.parameters, jobParameter.name )
                            if parameterIndex >= 0:
                                del self.parsingJob.parameters[ parameterIndex ]

                            self.parsingJob.parameters.append( jobParameter )

        self.levelCount += 1

    def EndElement( self, name ):
        self.levelCount -= 1

        # Ignore end tag processing if we are still within an invalid XML block.
        if self.errorLevelCount != 0:
            self.errorLevelCount -= 1
            return

        if self.levelCount == 0:
            self.inJobListBlock = False
        elif self.levelCount == 1:
            if self.parsingJob is not None:
                self.jobs.append( self.parsingJob )
                self.parsingJob = None
        elif self.levelCount == 2:
            self.inParametersBlock = False

def CreateHeaderTemplateParamDecl( templateParams ):
    if len( templateParams ) <= 0:
        return ''

    templateParamDecl = 'template< '
    bAddedParam = False
    for param in templateParams:
        if bAddedParam:
            templateParamDecl += ', '

        bAddedParam = True
        templateParamDecl += '{0} {1}'.format( param.type, param.name )
        if param.default is not None:
            templateParamDecl += ' = {0}'.format( param.default )

    templateParamDecl += ' >\n'

    return templateParamDecl

def CreateSourceTemplateParamDeclAndList( templateParams ):
    if len( templateParams ) <= 0:
        return ( '', '' )

    templateParamDecl = 'template< '
    templateParamList = '< '
    bAddedParam = False
    for param in templateParams:
        if bAddedParam:
            templateParamDecl += ', '
            templateParamList += ', '

        bAddedParam = True
        templateParamDecl += '{0} {1}'.format( param.type, param.name )
        templateParamList += param.name

    templateParamDecl += ' >\n'
    templateParamList += ' >'

    return ( templateParamDecl, templateParamList )

if len( sys.argv ) < 4:
    PrintCommandLineUsage()
    sys.exit( 2 )

jobDefPath = sys.argv[ 1 ]
includePath = sys.argv[ 2 ]
sourcePath = sys.argv[ 3 ]

try:
    jobDefListing = os.listdir( jobDefPath )
except:
    print( '[E] Error reading the contents of "', jobDefPath, '": ', sys.exc_info()[ 1 ], sep = '', file = sys.stderr )
    sys.exit( 1 )

interfaceHeaderFormatString = \
'''//----------------------------------------------------------------------------------------------------------------------
// {FILE}
//
// Copyright (C) {YEAR} WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

// !!! AUTOGENERATED FILE - DO NOT EDIT !!!

#pragma once
#ifndef {MODULE_TOKEN}_{FILE_TOKEN}
#define {MODULE_TOKEN}_{FILE_TOKEN}

#include "{MODULE}/{MODULE}.h"
#include "Platform/Assert.h"
{INCLUDES}
namespace Lunar
{{
    class JobContext;
}}

{FORWARD_DECLARATIONS}{NAMESPACE_BEGIN}{CLASSES}{NAMESPACE_END}#include "{MODULE}/{BASENAME}.inl"
{INLINE_INCLUDES}
#endif  // {MODULE_TOKEN}_{FILE_TOKEN}
'''

classFormatString = \
'''{DESCRIPTION_COMMENT}{TEMPLATE_PARAM_DECL}class {JOB_EXPORT}{JOB_NAME} : Lunar::NonCopyable
{{
public:
{CONSTANT_MEMBERS}    class Parameters
    {{
    public:
{PARAMETER_MEMBERS}        /// @name Construction/Destruction
        //@{{
        inline Parameters();
        //@}}
    }};

    /// @name Construction/Destruction
    //@{{
    inline {JOB_NAME}();
    inline ~{JOB_NAME}();
    //@}}

    /// @name Parameters
    //@{{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}}

    /// @name Job Execution
    //@{{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}}

private:
    Parameters m_parameters;
}};

'''

derivedClassFormatString = \
'''{DESCRIPTION_COMMENT}{TEMPLATE_PARAM_DECL}class {JOB_EXPORT}{JOB_NAME} : {JOB_PARENT}
{{
public:
    /// Parent class type.
    typedef {JOB_PARENT} Super;
    /// Parameters.
    typedef {JOB_PARENT}::Parameters Parameters;

    /// @name Construction/Destruction
    //@{{
    inline {JOB_NAME}();
    inline ~{JOB_NAME}();
    //@}}

    /// @name Job Execution
    //@{{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}}
}};

'''

interfaceInlineFormatString = \
'''//----------------------------------------------------------------------------------------------------------------------
// {FILE}
//
// Copyright (C) {YEAR} WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

// !!! AUTOGENERATED FILE - DO NOT EDIT !!!

{NAMESPACE_BEGIN}{CLASS_INLINES}{NAMESPACE_END}'''

classInlineFormatString = \
'''/// Constructor.
{TEMPLATE_PARAM_DECL}{JOB_NAME}{TEMPLATE_PARAM_LIST}::{JOB_NAME}()
{{
}}

/// Destructor.
{TEMPLATE_PARAM_DECL}{JOB_NAME}{TEMPLATE_PARAM_LIST}::~{JOB_NAME}()
{{
}}

/// Get the parameters for this job.
///
/// @return  Reference to the structure containing the job parameters.
///
/// @see SetParameters()
{TEMPLATE_PARAM_DECL}{TEMPLATE_TYPENAME}{JOB_NAME}{TEMPLATE_PARAM_LIST}::Parameters& {JOB_NAME}{TEMPLATE_PARAM_LIST}::GetParameters()
{{
    return m_parameters;
}}

/// Get the parameters for this job.
///
/// @return  Constant reference to the structure containing the job parameters.
///
/// @see SetParameters()
{TEMPLATE_PARAM_DECL}const {TEMPLATE_TYPENAME}{JOB_NAME}{TEMPLATE_PARAM_LIST}::Parameters& {JOB_NAME}{TEMPLATE_PARAM_LIST}::GetParameters() const
{{
    return m_parameters;
}}

/// Set the job parameters.
///
/// @param[in] rParameters  Structure containing the job parameters.
///
/// @see GetParameters()
{TEMPLATE_PARAM_DECL}void {JOB_NAME}{TEMPLATE_PARAM_LIST}::SetParameters( const Parameters& rParameters )
{{
    m_parameters = rParameters;
}}

/// Callback executed to run the job.
///
/// @param[in] pJob      Job to run.
/// @param[in] pContext  Context associated with the running job instance.
{TEMPLATE_PARAM_DECL}void {JOB_NAME}{TEMPLATE_PARAM_LIST}::RunCallback( void* pJob, JobContext* pContext )
{{
    HELIUM_ASSERT( pJob );
    HELIUM_ASSERT( pContext );
    static_cast< {JOB_NAME}* >( pJob )->Run( pContext );
}}

/// Constructor.
{TEMPLATE_PARAM_DECL}{JOB_NAME}{TEMPLATE_PARAM_LIST}::Parameters::Parameters()
{PARAMETER_INIT_LIST}{{
}}

'''

derivedClassInlineFormatString = \
'''/// Constructor.
{TEMPLATE_PARAM_DECL}{JOB_NAME}{TEMPLATE_PARAM_LIST}::{JOB_NAME}()
{{
}}

/// Destructor.
{TEMPLATE_PARAM_DECL}{JOB_NAME}{TEMPLATE_PARAM_LIST}::~{JOB_NAME}()
{{
}}

/// Callback executed to run the job.
///
/// @param[in] pJob      Job to run.
/// @param[in] pContext  Context associated with the running job instance.
{TEMPLATE_PARAM_DECL}void {JOB_NAME}{TEMPLATE_PARAM_LIST}::RunCallback( void* pJob, JobContext* pContext )
{{
    HELIUM_ASSERT( pJob );
    HELIUM_ASSERT( pContext );
    static_cast< {JOB_NAME}* >( pJob )->Run( pContext );
}}

'''

interfaceSourceFormatString = \
'''//----------------------------------------------------------------------------------------------------------------------
// {FILE}
//
// Copyright (C) {YEAR} WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

// !!! AUTOGENERATED FILE - DO NOT EDIT !!!

#include "{MODULE}Pch.h"
#include "{MODULE}/{BASENAME}.h"

#include "Platform/Assert.h"

{NAMESPACE_BEGIN}{CLASS_IMPLEMENTATIONS}{NAMESPACE_END}'''

classImplementationFormatString = \
''''''

currentYear = datetime.datetime.now().year

formatArgs = {}
formatArgs[ 'YEAR' ] = currentYear

formatter = string.Formatter()

for fileName in jobDefListing:
    splitFileName = os.path.splitext( fileName )
    if splitFileName[ 1 ].lower() != '.jobdef':
        continue

    pathName = os.path.join( jobDefPath, fileName )
    try:
        jobDefFile = open( pathName, 'rb' )
    except:
        print( '[E] Error opening "', pathName, '" for reading: ', sys.exc_info()[ 1 ], sep = '', file = sys.stderr )
        continue

    print( '[I] Parsing job definition file "', pathName, '"...', sep = '' )
    parseHandler = JobDefParseHandler()
    jobDefParser = expat.ParserCreate()
    jobDefParser.StartElementHandler = parseHandler.StartElement
    jobDefParser.EndElementHandler = parseHandler.EndElement
    jobDefParser.ParseFile( jobDefFile )
    jobDefFile.close()
    print( '[I] Finished parsing "', pathName, '".', sep = '' )

    moduleName = splitFileName[ 0 ]
    fileBaseName = moduleName + 'Interface'

    formatArgs[ 'BASENAME' ] = fileBaseName
    formatArgs[ 'MODULE' ] = moduleName
    formatArgs[ 'MODULE_TOKEN' ] = parseHandler.moduleTokenPrefix + ModuleToTokenName( moduleName )
    if parseHandler.namespace is not None:
        formatArgs[ 'NAMESPACE_BEGIN' ] = 'namespace {0}\n{{\n\n'.format( parseHandler.namespace )
        formatArgs[ 'NAMESPACE_END' ] = '}}  // namespace {0}\n\n'.format( parseHandler.namespace )
    else:
        formatArgs[ 'NAMESPACE_BEGIN' ] = ''
        formatArgs[ 'NAMESPACE_END' ] = ''

    formatArgs[ 'INCLUDES' ] = ''
    for includeFile in parseHandler.includes:
        formatArgs[ 'INCLUDES' ] += '#include "{0}"\n'.format( includeFile )

    formatArgs[ 'INLINE_INCLUDES' ] = ''
    for inlineFile in parseHandler.inlines:
        formatArgs[ 'INLINE_INCLUDES' ] += '#include "{0}"\n'.format( inlineFile )

    # Header generation.
    print( '[I] Generating header for module "', moduleName, '"...', sep = '' )
    fileName = fileBaseName + '.h'
    formatArgs[ 'FILE' ] = fileName
    formatArgs[ 'FILE_TOKEN' ] = ModuleToTokenName( fileName )

    forwardDeclarations = ''
    for declareNamespace, declareTypeList in parseHandler.forwardDeclarations.items():
        if declareNamespace == 0:
            for declareType in declareTypeList:
                forwardDeclarations += '{0};\n'.format( declareType )
            forwardDeclarations += '\n'
        else:
            forwardDeclarations += 'namespace {0}\n{{\n'.format( declareNamespace )
            for declareType in declareTypeList:
                forwardDeclarations += '    {0};\n'.format( declareType )
            forwardDeclarations += '}\n\n'

    classes = ''
    for job in parseHandler.jobs:
        descriptionComment = ''
        if job.description is not None:
            descriptionComment = '/// {0}\n'.format( job.description )

        jobExport = ''
        if len( job.templateParams ) <= 0:
            jobExport = '{0}_API '.format( formatArgs[ 'MODULE_TOKEN' ] )

        parameterMembers = ''
        for parameter in job.parameters:
            paramTypeSpecifier = 'in'
            if parameter.paramType == JobParameter.TYPE_OUTPUT:
                paramTypeSpecifier = 'out'
            elif parameter.paramType == JobParameter.TYPE_INOUT:
                paramTypeSpecifier = 'inout'

            description = parameter.description
            if description is None:
                description = ''

            parameterMembers += '        /// [{0}] {1}\n        {2} {3};\n'.format(
                paramTypeSpecifier,
                description,
                parameter.type,
                parameter.name )

        if parameterMembers != '':
            parameterMembers += '\n'

        constantMembers = ''
        for constant in job.constants:
            if constant.description is not None:
                constantMembers += '    /// {0}\n'.format( constant.description )

            constantMembers += '    static const {0} {1} = {2};\n'.format( constant.type, constant.name, constant.value )

        if constantMembers != '':
            constantMembers += '\n'

        parent = job.parent
        if parent is None:
            formatString = classFormatString
            parent = 'Lunar::NonCopyable'
        else:
            formatString = derivedClassFormatString

        formatArgs[ 'JOB_NAME' ] = job.name
        formatArgs[ 'JOB_PARENT' ] = parent
        formatArgs[ 'JOB_EXPORT' ] = jobExport
        formatArgs[ 'DESCRIPTION_COMMENT' ] = descriptionComment
        formatArgs[ 'TEMPLATE_PARAM_DECL' ] = CreateHeaderTemplateParamDecl( job.templateParams )
        formatArgs[ 'PARAMETER_MEMBERS' ] = parameterMembers
        formatArgs[ 'CONSTANT_MEMBERS' ] = constantMembers
        classes += formatter.vformat( formatString, None, formatArgs )

    formatArgs[ 'FORWARD_DECLARATIONS' ] = forwardDeclarations
    formatArgs[ 'CLASSES' ] = classes
    fileContents = formatter.vformat( interfaceHeaderFormatString, None, formatArgs )
    print( '[I] Header generated.' )

    pathName = os.path.join( includePath, moduleName, fileName )
    writeFile = True
    try:
        fileReader = open( pathName, 'rt' )
    except:
        pass
    else:
        print( '[I] Existing header "', pathName, '" found, comparing with new header contents.', sep = '' )
        existingFileContents = fileReader.read()
        fileReader.close()

        if fileContents == existingFileContents:
            print( '[I] Existing header contents match, file will not be updated.' )
            writeFile = False
        else:
            print( '[I] Existing header differs, file will be updated.' )

    if writeFile:
        print( '[I] Writing header for module "', moduleName, '"...', sep = '' )
        try:
            fileWriter = open( pathName, 'wt' )
        except:
            print( '[E] Error opening "', pathName, '" for writing: ', sys.exc_info()[ 1 ], sep = '', file = sys.stderr )
        else:
            fileWriter.write( fileContents )
            fileWriter.close()
            print( '[I] Finished writing "', pathName, '".', sep = '' )

    # Inline implementation generation.
    print( '[I] Generating inline implementation for module "', moduleName, '"...', sep = '' )
    fileName = fileBaseName + '.inl'
    formatArgs[ 'FILE' ] = fileName
    formatArgs[ 'FILE_TOKEN' ] = ModuleToTokenName( fileName )

    classInlines = ''
    for job in parseHandler.jobs:
        templateTypename = ''
        if len( job.templateParams ) > 0:
            templateTypename = 'typename '

        parameterInitList = ''
        parameterInitPre = ':'
        for parameter in job.parameters:
            if parameter.default is not None:
                parameterInitList += '    {0} {1}({2})\n'.format( parameterInitPre, parameter.name, parameter.default )
                parameterInitPre = ','

        if job.parent is None:
            formatString = classInlineFormatString
        else:
            formatString = derivedClassInlineFormatString

        formatArgs[ 'JOB_NAME' ] = job.name
        formatArgs[ 'TEMPLATE_TYPENAME' ] = templateTypename
        ( formatArgs[ 'TEMPLATE_PARAM_DECL' ], formatArgs[ 'TEMPLATE_PARAM_LIST' ] ) = \
            CreateSourceTemplateParamDeclAndList( job.templateParams )
        formatArgs[ 'PARAMETER_INIT_LIST' ] = parameterInitList
        classInlines += formatter.vformat( formatString, None, formatArgs )

    formatArgs[ 'CLASS_INLINES' ] = classInlines
    fileContents = formatter.vformat( interfaceInlineFormatString, None, formatArgs )
    print( '[I] Inline implementation generated.' )

    pathName = os.path.join( includePath, moduleName, fileName )
    writeFile = True
    try:
        fileReader = open( pathName, 'rt' )
    except:
        pass
    else:
        print( '[I] Existing inline implementation "', pathName, '" found, comparing with new inline implementation contents.', sep = '' )
        existingFileContents = fileReader.read()
        fileReader.close()

        if fileContents == existingFileContents:
            print( '[I] Existing inline implementation contents match, file will not be updated.' )
            writeFile = False
        else:
            print( '[I] Existing inline implementation differs, file will be updated.' )

    if writeFile:
        print( '[I] Writing inline implementation for module "', moduleName, '"...', sep = '' )
        try:
            fileWriter = open( pathName, 'wt' )
        except:
            print( '[E] Error opening "', pathName, '" for writing: ', sys.exc_info()[ 1 ], sep = '', file = sys.stderr )
        else:
            fileWriter.write( fileContents )
            fileWriter.close()
            print( '[I] Finished writing "', pathName, '".', sep = '' )

    # Source generation.
    print( '[I] Generating source for module "', moduleName, '"...', sep = '' )
    fileName = fileBaseName + '.cpp'
    formatArgs[ 'FILE' ] = fileName
    formatArgs[ 'FILE_TOKEN' ] = ModuleToTokenName( fileName )

    classImplementations = ''
    for job in parseHandler.jobs:
        formatArgs[ 'JOB_NAME' ] = job.name
        ( formatArgs[ 'TEMPLATE_PARAM_DECL' ], formatArgs[ 'TEMPLATE_PARAM_LIST' ] ) = \
            CreateSourceTemplateParamDeclAndList( job.templateParams )
        classImplementations += formatter.vformat( classImplementationFormatString, None, formatArgs )

    formatArgs[ 'CLASS_IMPLEMENTATIONS' ] = classImplementations
    fileContents = formatter.vformat( interfaceSourceFormatString, None, formatArgs )
    print( '[I] Source generated.' )

    pathName = os.path.join( sourcePath, moduleName, fileName )
    writeFile = True
    try:
        fileReader = open( pathName, 'rt' )
    except:
        pass
    else:
        print( '[I] Existing source file "', pathName, '" found, comparing with new source file contents.', sep = '' )
        existingFileContents = fileReader.read()
        fileReader.close()

        if fileContents == existingFileContents:
            print( '[I] Existing source file contents match, file will not be updated.' )
            writeFile = False
        else:
            print( '[I] Existing source file differs, file will be updated.' )

    if writeFile:
        print( '[I] Writing source file for module "', moduleName, '"...', sep = '' )
        try:
            fileWriter = open( pathName, 'wt' )
        except:
            print( '[E] Error opening "', pathName, '" for writing: ', sys.exc_info()[ 1 ], sep = '', file = sys.stderr )
        else:
            fileWriter.write( fileContents )
            fileWriter.close()
            print( '[I] Finished writing "', pathName, '".', sep = '' )

sys.exit( 0 )
