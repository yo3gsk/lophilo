#!/usr/bin/env node

var Apricot = require('apricot');
var fs = require('fs');

function processHtmlDoc(error, htmldoc) {
	console.log('starting: ');
  console.dir(htmldoc);
  r = require('repl').start();
  r.context.htmldoc = htmldoc;
}

Apricot.open('library_report.doc', processHtmlDoc);
