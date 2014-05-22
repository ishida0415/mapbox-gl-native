#!/usr/bin/env node
'use strict';

var path = require('path');

var lazy_update = require('./lazy-update');

var name = 'style';
var data = JSON.stringify(require(path.join(process.cwd(), process.argv[2])));


var length = data.length;

var header = '// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n\n';
header += '#ifndef LLMR_STYLE_RESOURCES\n';
header += '#define LLMR_STYLE_RESOURCES\n';
header += '\n';
header += 'namespace llmr {\n';
header += 'namespace resources {\n';
header += '\n';
header += 'extern const unsigned char ' + name + '[];\n';
header += 'extern const unsigned long ' + name + '_size;\n';
header += '\n';
header += '}\n';
header += '}\n';
header += '\n';
header += '#endif\n';

lazy_update('include/llmr/style/resources.hpp', header);

var code = '// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n';
code += '#include <llmr/style/resources.hpp>\n';
code += '\n';
code += 'using namespace llmr;\n';
code += '\n';
code += 'const unsigned char resources::' + name + '[] = R"JSON(' + data + ')JSON";\n';
code += 'const unsigned long resources::' + name + '_size = sizeof(resources::' + name + ');\n';

lazy_update('src/style/resources.cpp', code);

