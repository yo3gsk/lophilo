#!/usr/bin/env node

var libxmljs = require('libxmljs');
var fs = require('fs');

// xpath queries
//var gchild = xmlDoc.get('//grandchild');

//console.log(gchild.text());  // prints 'grandchild content'

//var children = xmlDoc.root().childNodes();
//var child = children[0];

//console.log(child.attr('foo').value()); // prints 'bar'


function parseFileContent(err, content) {
  if (err) {
    return console.log(err);
  }
  console.log('read: ' + content.length);
  var xmldoc = libxmljs.parseXmlString(content);
  console.dir('xmldoc:' + xmldoc);
  r = require('repl').start();
  r.context.xmldoc = xmldoc;
}

fs.readFile('./library_report.xml', 'utf-8', parseFileContent);
