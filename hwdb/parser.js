#!/usr/bin/env node
var util = require('util');
var xml = require("node-xml");
var repl = require('repl');

r = repl.start();

var parser = new xml.SaxParser(function(cb) {
  cb.onStartDocument(function() {

  });
  cb.onEndDocument(function() {

  });
  cb.onStartElementNS(function(elem, attrs, prefix, uri, namespaces) {
      util.log("=> Started: " + elem + " uri="+uri +" (Attributes: " + JSON.stringify(attrs) + " )");
	  if(elem !='td' && elem != 'tr') {
		return;
	  }
      util.log("<= End: " + elem + " uri="+uri + "\n");
      parser.pause();// pause the parser		 
			r.context.elem = elem;
			r.context.attrs = attrs;
			r.context.prefix = prefix;
			r.context.uri = uri;
			r.context.namespaces = namespaces;
			r.context.parser = parser;	  
  });
  cb.onEndElementNS(function(elem, prefix, uri) {

  });
  cb.onCharacters(function(chars) {
      //util.log('<CHARS>'+chars+"</CHARS>");
  });
  cb.onCdata(function(cdata) {
      util.log('<CDATA>'+cdata+"</CDATA>");
  });
  cb.onComment(function(msg) {
      util.log('<COMMENT>'+msg+"</COMMENT>");
  });
  cb.onWarning(function(msg) {
      util.log('<WARNING>'+msg+"</WARNING>");
  });
  cb.onError(function(msg) {
      util.log('<ERROR>'+JSON.stringify(msg)+"</ERROR>");
  });
});

parser.parseFile("../library_report.doc");
