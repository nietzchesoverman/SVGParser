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
  'getDescWrapper': ['string',['string', 'string']],
  'getSVGAttributeWrapper': ['string',['string', 'string']],
  'getRectAttributeWrapper': ['string',['string', 'string', 'int']],
  'getCircAttributeWrapper': ['string',['string', 'string', 'int']],
  'getPathAttributeWrapper': ['string',['string', 'string', 'int']],
  'getGrpAttributeWrapper': ['string',['string', 'string', 'int']],
  'changeTitle': ['void',['string', 'string', 'string']],
  'changeDesc': ['void',['string', 'string', 'string']],
  'scaleRects': ['bool', ['string', 'string', 'float']],
  'scaleCircs': ['bool', ['string', 'string', 'float']]
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

//switch SVG Viewport
app.get('/switchSVG', function(req , res){
  let svgFilePath = "uploads/"+req.query.inputFilePath;
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

//Show Attributes
app.get('/showAttr', function(req , res){
  let svgFilePath = req.query.filePath;
  let schema = 'parser/bin/svg.xsd';
  let retStr = "";
  let svgComponentType = req.query.componentType;
  const componentList = svgComponentType.split(" ");

  if (componentList[0].localeCompare("SVG") == 0){
    //SVG otherAttributes JSON Callback C function
    retStr = svgLib.getSVGAttributeWrapper(svgFilePath, schema);
  }else if(componentList[0].localeCompare("Rectangle") == 0){
    //Rectangles otherAttributes Callback- with componentList[1] as iter
    retStr = svgLib.getRectAttributeWrapper(svgFilePath, schema, parseInt(componentList[1]));
  }else if(componentList[0].localeCompare("Circle") == 0){
    //Circles otherAttributes Callback- with componentList[1] as iter
    retStr = svgLib.getCircAttributeWrapper(svgFilePath, schema, parseInt(componentList[1]));
  }else if(componentList[0].localeCompare("Path") == 0){
    //Paths otherAttributes Callback- with componentList[1] as iter
    retStr = svgLib.getPathAttributeWrapper(svgFilePath, schema, parseInt(componentList[1]));
  }else if(componentList[0].localeCompare("Group") == 0){
    //Groups otherAttributes Callback- with componentList[1] as iter
    retStr = svgLib.getGrpAttributeWrapper(svgFilePath, schema, parseInt(componentList[1]));
  }
 
  res.send(
    {
      otherAttributes: retStr
    }
  );
});

//Change Title
app.get('/changeTitle', function(req , res){
  let filePath = req.query.svgPath;
  let schema = 'parser/bin/svg.xsd';
  let changedTitle = req.query.newTitle;

  svgLib.changeTitle(filePath, schema, changedTitle);
 
  res.send(
    {
      somethingElse: changedTitle
    }
  );
});

//Change Description
app.get('/changeDescription', function(req , res){
  let filePath = req.query.svgPath;
  let schema = 'parser/bin/svg.xsd';
  let changedDesc = req.query.newDesc;

  svgLib.changeDesc(filePath, schema, changedDesc);
 
  res.send(
    {
      somethingElse: changedDesc
    }
  );
});

//Scale Shape
//Sample endpoint
app.get('/scaleShape', function(req , res){
  let filePath = req.query.svgFilePath;
  let schema = 'parser/bin/svg.xsd';
  let shapeType = req.query.rectOrCirc;
  let scaleFact = parseFloat(req.query.scaleFactor);
  let ret = false;

  if (shapeType.localeCompare("Rectangles") == 0){
    //Rectangle callback with Scale Fact
    ret = svgLib.scaleRects(filePath, schema, scaleFact);
  }else{
    //Circle callback with Scale fact
    ret = svgLib.scaleCircs(filePath, schema, scaleFact);
  }
 
  res.send(
    {
      worked: ret
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