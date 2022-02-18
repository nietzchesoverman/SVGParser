#include "SVGHelpers.h"
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
//SVG element parsing functions
void extractMetaInfo(SVG* mainElement, xmlNode* rootNode){
    int standardTextSize = 256;
    //Namespace Extraction                        
    strncpy(mainElement->namespace, (char*)rootNode->ns->href, standardTextSize);  //Truncate and copy only 256

    //SVG component Extraction - title, description: initialized then check if optional elements exist and put them into SVG struct
    strcpy(mainElement->title, "");
    strcpy(mainElement->description, "");
    findTitleAndDesc(mainElement, rootNode);

}
void findTitleAndDesc(SVG* mainElement, xmlNode* rootNode){
    xmlNode* curr;

    for (curr = rootNode; curr; curr = curr->next){
        if(!strcmp((char*)curr->name, "desc")){
            strncpy(mainElement->description,(char*)(curr->children->content), 256);
        }else if(!strcmp((char*)curr->name, "title")){
            strncpy(mainElement->title,(char*)(curr->children->content), 256);
        }
        findTitleAndDesc(mainElement, rootNode->children);
    }
}
List* extractAttributes(xmlNode* currNode, char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second)){
    List* newAttributeList = initializeList(printFunction, deleteFunction, compareFunction);
    xmlAttr* attr;

    for (attr = currNode->properties; attr != NULL; attr = attr->next){
        char* attrName = malloc(strlen((char*)attr->name) + 1);
        strcpy(attrName,(char*)attr->name);
        char* attrCont = (char*)(attr->children->content);
        Attribute* newAttribute= (Attribute*)malloc(sizeof(Attribute) + sizeof(attrName) + strlen(attrCont) * sizeof(char));          //allocate for flexible array member and attribute
        newAttribute->name = attrName;
        strcpy(newAttribute->value, attrCont);                          //copy content into attribute
        insertBack(newAttributeList, (void*)newAttribute);              //Typecast attribute and insert into list
    }

    return newAttributeList;
}

//Rectangle Parsing Functions
List* extractRectangles(xmlNode* currNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second),
char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second)){
    List* newRectangleList = initializeList(printFunction, deleteFunction, compareFunction);

    addRectangleToList(newRectangleList, currNode, attrPrintFunction, attrDeleteFunction, attrCompareFunction);

    return newRectangleList;
}
void addRectangleToList(List* rectangleList, xmlNode* rootNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),
int (*attrCompareFunction)(const void* first,const void* second)){
    xmlNode* curr;

    for (curr = rootNode->children; curr; curr = curr->next){
        if (!strcmp((char*)curr->name, "rect")){
            Rectangle* newRectangle = malloc(sizeof(Rectangle));
            char*x = (char*)xmlGetProp(curr, (const xmlChar*)"x");
            char*y = (char*)xmlGetProp(curr, (const xmlChar*)"y");
            char*width = (char*)xmlGetProp(curr, (const xmlChar*)"width");
            char*height = (char*)xmlGetProp(curr, (const xmlChar*)"height");
            int i = 0;

            newRectangle->x = atof(x);           
            newRectangle->y = atof(y);
            newRectangle->width = atof(width);
            newRectangle->height = atof(height);
            strcpy(newRectangle->units,"");
            for (i = 0; i < strlen(x); i++){
                if ((x[i] >= 'a' && x[i] <= 'z') || (x[i] >= 'A' && x[i] <= 'Z')){      //adds units
                    strcpy(newRectangle->units,strchr(x, x[i - 1]));
                }
            }

            free(x);
            free(y);
            free(width);
            free(height);
            newRectangle->otherAttributes = extractRectAttributes(curr, attrPrintFunction, attrDeleteFunction, attrCompareFunction);
            insertBack(rectangleList, (void*)newRectangle);
        }
    }
}
List* extractRectAttributes(xmlNode* currNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second)){
    List* newRectAttributes = initializeList(attrPrintFunction, attrDeleteFunction, attrCompareFunction);
    xmlAttr* attr;


    for (attr = currNode->properties; attr; attr = attr->next){
        if (strcmp((char*)attr->name, "x") != 0 && strcmp((char*)attr->name, "y") != 0 && strcmp((char*)attr->name, "width") != 0 &&
        strcmp((char*)attr->name, "height") != 0){  //If its not x,y,width, or height - then make it an attribute
            char* attributeName = malloc(strlen((char*)attr->name) + 1);
            strcpy(attributeName, (char*)attr->name);
            char* attributeContent = (char*)(attr->children->content);
            Attribute* newRectAttribute = malloc(sizeof(Attribute) + strlen(attributeName) + strlen(attributeContent) +2);
            newRectAttribute->name = attributeName;
            strcpy(newRectAttribute->value, attributeContent);
            insertBack(newRectAttributes, (void*)newRectAttribute);
        }
    }

    return newRectAttributes;
}


List* extractCircles(xmlNode* currNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second),
char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second)){
    List* circlesList = initializeList(printFunction, deleteFunction, compareFunction);

    addCircleToList(circlesList, currNode, attrPrintFunction, attrDeleteFunction, attrCompareFunction);

    return circlesList;
}
void addCircleToList(List* circleList, xmlNode* rootNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second)){
    xmlNode* curr;

    for (curr = rootNode->children; curr; curr = curr->next){
        if(!strcmp((char*)curr->name, "circle")){
            Circle* newCirc = malloc(sizeof(Circle));
            char*cx = (char*)xmlGetProp(curr, (const xmlChar*)"cx");
            char*cy = (char*)xmlGetProp(curr, (const xmlChar*)"cy");
            char*r = (char*)xmlGetProp(curr, (const xmlChar*)"r");
            int i = 0;

            newCirc->cx = atof(cx);
            newCirc->cy = atof(cy);
            newCirc->r = atof(r);
            strcpy(newCirc->units,"");
            for (i = 0; i < strlen(cx); i++){
                if ((cx[i] >= 'a' && cx[i] <= 'z') || (cx[i] >= 'A' && cx[i] <= 'Z')){      //adds units
                    strcpy(newCirc->units,strchr(cx, cx[i - 1]));
                }
            }

            free(cx);
            free(cy);
            free(r);
            newCirc->otherAttributes = extractCircAttributes(curr, attrPrintFunction, attrDeleteFunction, attrCompareFunction);
            insertBack(circleList, (void*)newCirc);
        }
    }
}
List* extractCircAttributes(xmlNode* currNode, char* (*attrPrintFunction)(void* toBePrinted),void (*attrDeleteFunction)(void* toBeDeleted),int (*attrCompareFunction)(const void* first,const void* second)){
    List* newCircAttributes = initializeList(attrPrintFunction, attrDeleteFunction, attrCompareFunction);
    xmlAttr* attr;


    for (attr = currNode->properties; attr; attr = attr->next){
        if (strcmp((char*)attr->name, "cx") != 0 && strcmp((char*)attr->name, "cy") != 0 && strcmp((char*)attr->name, "r") != 0){  //If its not cx, cy, r make it attribute
            char* attributeName = malloc(strlen((char*)attr->name) + 1);
            strcpy(attributeName, (char*)attr->name);
            char* attributeContent = (char*)(attr->children->content);
            Attribute* newCircAttrib = malloc(sizeof(Attribute) + strlen(attributeName) + strlen(attributeContent) +2);
            newCircAttrib->name = attributeName;
            strcpy(newCircAttrib->value, attributeContent);
            insertBack(newCircAttributes, (void*)newCircAttrib);
        }
    }

    return newCircAttributes;
}

//Paths
List* extractPaths(xmlNode* currNode){
    List* newPathList = initializeList(pathToString, deletePath, comparePaths);

    addPathToList(newPathList, currNode);

    return newPathList;
}
void addPathToList(List* pathlist, xmlNode* rootNode){
    xmlNode* curr;

    for (curr = rootNode->children; curr; curr = curr->next){
        if (!strcmp((char*)curr->name, "path")){
            char* pathData = (char*)xmlGetProp(curr, (const xmlChar*)"d");
            Path* newPathNode = malloc(sizeof(Path) + strlen(pathData) + 1);
            strcpy(newPathNode->data, pathData);
            free(pathData);
            newPathNode->otherAttributes = extractPathAttributes(curr);
            insertBack(pathlist, (void*)newPathNode);
        }
    }
}
List* extractPathAttributes(xmlNode* currNode){
    List* pathAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    xmlAttr* attr;

    for (attr = currNode->properties; attr; attr = attr->next){
        if (strcmp((char*)attr->name, "d") != 0){
            char* attributeName = malloc(strlen((char*)attr->name) + 1);
            strcpy(attributeName, (char*)attr->name);
            char* attributeContent = (char*)(attr->children->content);
            Attribute* newPathAttrib = malloc(sizeof(Attribute) + strlen(attributeName) + strlen(attributeContent) +2);
            newPathAttrib->name = attributeName;
            strcpy(newPathAttrib->value, attributeContent);
            insertBack(pathAttributes, (void*)newPathAttrib);
        }
    }

    return pathAttributes;
}

//Group Parsing
List* extractGroups(xmlNode* rootNode){
    List* newGroupList = initializeList(groupToString, deleteGroup, compareGroups);     //main list that goes into the SVG struct

    findGroups(newGroupList, rootNode);

    return newGroupList;
}
void findGroups(List* groupList, xmlNode* currNode){        //recursively finds group elements
    xmlNode* curr;

    for (curr = currNode->children; curr; curr = curr->next){
        if (!strcmp((char*)curr->name, "g")){
            addGroupToList(groupList, curr);
        }
        //findGroups(groupList, curr);
    }
}
void addGroupToList(List* groupList, xmlNode* currNode){       //processes only this group's scope by using currNode as root for above extract functions 
    Group* newGroup = malloc(sizeof(Group));
    newGroup->rectangles = extractRectangles(currNode, attributeToString, deleteAttribute, compareAttributes, rectangleToString, deleteRectangle, compareRectangles);
    newGroup->circles = extractCircles(currNode, attributeToString, deleteAttribute, compareAttributes, circleToString, deleteCircle, compareCircles);
    newGroup->paths = extractPaths(currNode);
    newGroup->groups = extractGroups(currNode);
    newGroup->otherAttributes = extractAttributes(currNode, attributeToString, deleteAttribute, compareAttributes);

    insertBack(groupList, (void*)newGroup);
}

//--get Functionality--
void masterDelete(void* data){
    return;
}
//getRects
void digForRects(List* masterList, void* singleGroup){
    Group* groupElement = (Group*)singleGroup;
    List* rectElement = groupElement->rectangles;
    ListIterator i = createIterator(rectElement);      //rectangle iterator

    void* element;
    while ((element = nextElement(&i))!= NULL){                     //insert all primitives to the back of our rectangle masterlist
        insertBack(masterList, element);
    }
    if (getLength(groupElement->groups) > 0){   
        i = createIterator(groupElement->groups); //iter group list and recurse
        while((element = nextElement(&i))!= NULL){    
            digForRects(masterList, element);                  //recursive step through the buried groups if they have nodes
        }
    }
}
//getCircles
void digForCircles(List* masterList, void* singleGroup){
    Group* groupElement = (Group*)singleGroup;
    List* circElement = groupElement->circles;
    ListIterator i = createIterator(circElement);
    void* element;

    while ((element = nextElement(&i)) != NULL){
        insertBack(masterList, element);
    }
    if(getLength(groupElement->groups) > 0){
        i = createIterator(groupElement->groups); //iter group list and recurse
        while((element = nextElement(&i))!= NULL){    
            digForCircles(masterList, element);                  //recursive step through the buried groups if they have nodes
        }
    }
    
}
//getPaths
void digForPaths(List* masterList, void* singleGroup){
    Group* groupElement = (Group*)singleGroup;
    List* pathElement = groupElement->paths;
    ListIterator i = createIterator(pathElement);
    void* element;

    while ((element = nextElement(&i))!= NULL){
        insertBack(masterList, element);
    }
    if (getLength(groupElement->groups) > 0){
        i = createIterator(groupElement->groups); //iter group list and recurse
        while((element = nextElement(&i))!= NULL){    
            digForPaths(masterList, element);                  //recursive step through the buried groups if they have nodes
        }
    }
}
void digForGroups(List* masterList, void* singleGroup){
    Group* groupElement = (Group*)singleGroup;
    if (getLength(groupElement->groups) < 1){
        return;
    }
    List* subGroupElement = groupElement->groups;
    ListIterator i = createIterator(subGroupElement);
    void* element;

    while ((element = nextElement(&i))!= NULL){
        insertBack(masterList, element);           //grab that group in the loop and perform recursive check & step here
    }
    if (getLength(groupElement->groups) > 0){
        i = createIterator(groupElement->groups); //iter group list and recurse
        while((element = nextElement(&i))!= NULL){    
            digForGroups(masterList, element);                  //recursive step through the buried groups if they have nodes
        }
    }
}

int validateTree(xmlDoc* document, const char* schemaFile){
    xmlSchema* newSchema = NULL;
    xmlSchemaParserCtxt* ctxt = NULL;
    xmlSchemaValidCtxt* ctxt2 = NULL;
    int valid;

    xmlLineNumbersDefault(1);

    if (strcmp(schemaFile, "") == 0){  //check if the files have names
        return -1;
    }
    //Create Schema File
    ctxt = xmlSchemaNewParserCtxt(schemaFile);      //ensure schema is valid
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    newSchema = xmlSchemaParse(ctxt);
    if (newSchema == NULL){       //check if the reads happened properly
        xmlSchemaFreeParserCtxt(ctxt);
        return -1;
    }
    xmlSchemaFreeParserCtxt(ctxt);
    

    ctxt2 = xmlSchemaNewValidCtxt(newSchema);       //ensure document is valid for the schema
    xmlSchemaSetValidErrors(ctxt2, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    valid = xmlSchemaValidateDoc(ctxt2, document);
    if (valid != 0){
        xmlSchemaFreeValidCtxt(ctxt2);
        xmlSchemaFree(newSchema);
        xmlSchemaCleanupTypes();
        return -1;
    }
    xmlSchemaFreeValidCtxt(ctxt2);
    xmlSchemaFree(newSchema);
    xmlSchemaCleanupTypes();
    return valid;
}

xmlDoc* svgToXML(const SVG* img){
    xmlDoc* newDoc = NULL;
    xmlNode* rootNode = NULL;
    xmlNs* nameSpace = NULL;
    List* elementList = NULL;

    //create root element and set the namespace
    newDoc = xmlNewDoc((xmlChar*)"1.0");
    rootNode = xmlNewNode(NULL, (const xmlChar*)"svg");
    nameSpace = xmlNewNs(rootNode, (const xmlChar*)img->namespace, NULL);
    xmlSetNs(rootNode, nameSpace);
    xmlDocSetRootElement(newDoc, rootNode);
    
    //get other attributes of root svg element
    elementList = img->otherAttributes;
    attrToXML(rootNode, elementList);

    //set title and description, if they exist.
    if (strcmp(img->title, "") != 0){      
        xmlNewChild(rootNode, NULL, (const xmlChar*)"title", (const xmlChar*)img->title);
    }
    if (strcmp(img->description, "") != 0){      
        xmlNewChild(rootNode, NULL, (const xmlChar*)"desc", (const xmlChar*)img->description);
    }

    //set rectangles,circles,paths
    elementList = img->rectangles;
    rectToXML(rootNode, elementList);
    elementList = img->circles;
    circToXML(rootNode, elementList);
    elementList = img->paths;
    pathToXML(rootNode, elementList);
    elementList = img->groups;
    groupToXML(rootNode, elementList);



    return newDoc;
}
void attrToXML(xmlNode* parentNode, List* elementList){
    ListIterator i = createIterator(elementList);
    void* element;
    while ((element = nextElement(&i))!= NULL){
        Attribute* newAttr = (Attribute*)element;
        xmlNewProp(parentNode, (const xmlChar*)newAttr->name,(const xmlChar*)newAttr->value);
    }
}
void rectToXML(xmlNode* parentNode, List* elementList){
    ListIterator i = createIterator(elementList);
    char ftoA[82];
    void* element;
    while ((element = nextElement(&i))!= NULL){         //for each rectangle
        Rectangle* rectElement = (Rectangle*)element;
        xmlNode* newRect = xmlNewChild(parentNode, NULL, (const xmlChar*)"rect", NULL);
        sprintf(ftoA, "%f%s", rectElement->x, rectElement->units);
        xmlNewProp(newRect, (const xmlChar*)"x", (const xmlChar*)ftoA);
        sprintf(ftoA, "%f%s", rectElement->y, rectElement->units);
        xmlNewProp(newRect, (const xmlChar*)"y", (const xmlChar*)ftoA);
        sprintf(ftoA, "%f%s", rectElement->width, rectElement->units);
        xmlNewProp(newRect, (const xmlChar*)"width", (const xmlChar*)ftoA);
        sprintf(ftoA, "%f%s", rectElement->height, rectElement->units);
        xmlNewProp(newRect, (const xmlChar*)"height", (const xmlChar*)ftoA);
        attrToXML(newRect, rectElement->otherAttributes);
    }
    
    
}
void circToXML(xmlNode* parentNode, List* elementList){
    ListIterator i = createIterator(elementList);
    char ftoA[82];
    void* element;
    while ((element = nextElement(&i))!= NULL){
        Circle* circElement = (Circle*)element;
        xmlNode* newCirc = xmlNewChild(parentNode, NULL, (const xmlChar*)"circle", NULL);
        sprintf(ftoA, "%f%s", circElement->cx, circElement->units);
        xmlNewProp(newCirc, (const xmlChar*)"cx", (const xmlChar*)ftoA);
        sprintf(ftoA, "%f%s", circElement->cy, circElement->units);
        xmlNewProp(newCirc, (const xmlChar*)"cy", (const xmlChar*)ftoA);
        sprintf(ftoA, "%f%s", circElement->r, circElement->units);
        xmlNewProp(newCirc, (const xmlChar*)"r", (const xmlChar*)ftoA);
        attrToXML(newCirc, circElement->otherAttributes);
    }
}
void pathToXML(xmlNode* parentNode, List* elementList){
    ListIterator i = createIterator(elementList);
    void* element;
    while ((element = nextElement(&i))!= NULL){
        Path* pathElement = (Path*)element;
        xmlNode* newPath = xmlNewChild(parentNode, NULL, (const xmlChar*)"path", NULL);
        xmlNewProp(newPath, (const xmlChar*)"d", (const xmlChar*)pathElement->data);
        attrToXML(newPath, pathElement->otherAttributes);
    }
}
void groupToXML(xmlNode* parentNode, List* elementList){
    ListIterator i = createIterator(elementList);
    void* element;
    while ((element = nextElement(&i))!=NULL){
        Group* grpElement = (Group*)element;
        xmlNode* newGrp = xmlNewChild(parentNode, NULL, (const xmlChar*)"g", NULL);
        attrToXML(newGrp, grpElement->otherAttributes);
        rectToXML(newGrp, grpElement->rectangles);
        circToXML(newGrp, grpElement->circles);
        pathToXML(newGrp, grpElement->paths);
        groupToXML(newGrp, grpElement->groups);
    }
}