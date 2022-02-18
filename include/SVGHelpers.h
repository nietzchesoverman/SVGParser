#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SVGParser.h"
/*
All work completed by Yousif Jamal, yjamal/1160861 for CIS2750 Assignment 1

Used looping in tree1.c that printed out each node to loop through nodes and extract their info recursively in a depth-first way
http://www.xmlsoft.org/examples/tree1.c

Utilized libXML example to extract attributes 
https://moodle.socs.uoguelph.ca/pluginfile.php/60421/mod_resource/content/0/libXmlExample.c

Utilized example to validate xml against xsd
https://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html

Utilized Tree creation & saving from tree2.c
http://www.xmlsoft.org/examples/tree2.c
*/



//Inputs Title, description and namespace into SVG Struct
void extractMetaInfo(SVG* mainElement, xmlNode* rootNode);
//Helper function for extractMetaInfo - extracts the title and description into the SVG struct if available
void findTitleAndDesc(SVG* mainElement, xmlNode* rootNode);
//Extracts attributes of the ~~current node~~ into a list, includes initialization, etc. - called by other structs and for svg element itself @ root node
List* extractAttributes(xmlNode* currNode, char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second));

//Extracts all rectangles into a list - includes attribute functions to run extract attributes (attrPrint/delete/compare)
List* extractRectangles(xmlNode* currNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second),
char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second));
//Helper for extract Rectangles - runs through entire xml Doc recursively and adds rectangle structs to list
void addRectangleToList(List* rectangleList, xmlNode* rootNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second));
//Helper for addRectangleToList - grabs attributes that aren't x,y,width or height
List* extractRectAttributes(xmlNode* currNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second));

//Extracts all circles into a list
List* extractCircles(xmlNode* currNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second),
char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second));
//Helper for extract circles - recursively adds all rectangles to the above list
void addCircleToList(List* circleList, xmlNode* rootNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second));
//Helper for addCircleToList - grabs attributes that aren't cx,cy, ... 
List* extractCircAttributes(xmlNode* currNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second));

//Extract all paths into a list
List* extractPaths(xmlNode* currNode);
//Extract all paths - recursively adds all paths to the above list
void addPathToList(List* pathlist, xmlNode* rootNode);
//Helper for addPathToList - grabs attributes that aren't d
List* extractPathAttributes(xmlNode* currNode);

//Extracts all groups into a list
List* extractGroups(xmlNode* rootNode);
//Iterates through recursively until it finds a group node, only runs the addGroupToList function on that xmlNode
void findGroups(List* groupList, xmlNode* currNode);
//Extracts all group sub-lists (rects, attributes,etc.) and ATTRIBUTES(since we dont have group-specific attrs) into a group element thats added onto 
//the main SVG group list
void addGroupToList(List* groupList, xmlNode* currNode);

//GetList master delete function
void masterDelete(void* data);
//GetRects Helper function - recursively digs through group lists and insertBack()'s them into the greater list
void digForRects(List* masterList, void* singleGroup);
//GetCircles Helper function - similar to digForRects
void digForCircles(List* masterList, void* singleGroup);
//getPaths helper - similar to other dig functions
void digForPaths(List* masterList, void* singleGroup);
//getGroups helper - similar to other dig functions
void digForGroups(List* masterList, void* singleGroup);



/*ASSIGNMENT 2*/
//validates an xmlDocument against a schema file
int validateTree(xmlDoc* document, const char* schemaFile);
int checkExtension(const char* fileName, char* extension);

//Turns an SVG into an xmlDoc
xmlDoc* svgToXML(const SVG* img);
//helpers to get attributes,rects,circles and paths
void attrToXML(xmlNode* parentNode, List* elementList);
void rectToXML(xmlNode* parentNode, List* elementList);
void circToXML(xmlNode* parentNode, List* elementList);
void pathToXML(xmlNode* parentNode, List* elementList);
void groupToXML(xmlNode* parentNode, List* elementList);