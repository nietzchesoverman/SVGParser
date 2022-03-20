'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Setup the library
let svgLib = ffi.Library('./libsvgparser',{
  'SVGCreationWrapper': ['string', ['string', 'string']],
  'validateSVGWrapper': ['bool', ['string', 'string']],
  'rectViewWrapper': ['string',['string', 'string']],
  'circViewWrapper': ['string',['string', 'string']],
  'pathViewWrapper': ['string',['string', 'string']],
  'grpViewWrapper': ['string',['string', 'string']],
  'getNameWrapper': ['string',['string', 'string']],
  'getDescWrapper': ['string',['string', 'string']]
});

//File log event handler on page refresh babyeeee
app.get('/populateFileLog', function(req , res){
  let retStr = "";
  let svgJson = "";
  let fileArray = fs.readdirSync('./uploads');
  const retPaths = [];
  const hrefList = [];
  const fileSizes = [];
  const invalidFiles = [];

  fileArray.forEach(function(file){
    if (svgLib.validateSVGWrapper('uploads/'+file, 'parser/bin/svg.xsd')){
      let stats = fs.statSync('uploads/'+file);
      svgJson = svgLib.SVGCreationWrapper('uploads/'+file, 'parser/bin/svg.xsd');
      if (retStr.localeCompare("") == 0){
        retStr = "["+svgJson;
      }else{
        retStr = retStr+","+svgJson;
      }
      retPaths.push("uploads/"+file);
      hrefList.push(file);
      fileSizes.push(Math.round(stats.size / 1024));
    }else{
      invalidFiles.push(file);
    }
  });

  retStr = retStr+"]";

  res.send(
    {
      svgString: retStr,
      picturePaths: retPaths,
      pictureDLs: hrefList,
      sizes: fileSizes,
      invalid: invalidFiles
    }
  );
});

//Populate Selectors
app.get('/populateSelector', function(req , res){
  let files = fs.readdirSync('./uploads');
  const fileArray = [];

  files.forEach(function(file){
    if (svgLib.validateSVGWrapper('uploads/'+file, 'parser/bin/svg.xsd')){
      fileArray.push(file);
    }
  });
 
  res.send(
    {
      validFiles: fileArray
    }
  );
});

//SVG viewport extravaganza
app.get('/viewSVG', function(req , res){
  let svgFiles = fs.readdirSync('./uploads');
  let svgFilePath = "uploads/"+svgFiles[0];
  let schema = 'parser/bin/svg.xsd';
  let title = svgLib.getNameWrapper(svgFilePath, schema);
  let desc = svgLib.getDescWrapper(svgFilePath, schema);
  let rects = svgLib.rectViewWrapper(svgFilePath, schema);
  let circs = svgLib.circViewWrapper(svgFilePath, schema);
  let paths = svgLib.pathViewWrapper(svgFilePath, schema);
  let group = svgLib.grpViewWrapper(svgFilePath, schema);

 
  res.send(
    {
      imagePath: svgFilePath,
      SVGTitle: title,
      SVGDesc: desc,
      SVGRects: rects,
      SVGCircs: circs,
      SVGPaths: paths,
      SVGGrps: group
    }
  );
});

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
 
  res.send(
    {
      somethingElse: retStr
    }
  );
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);