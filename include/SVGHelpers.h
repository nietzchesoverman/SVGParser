#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SVGParser.h"



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