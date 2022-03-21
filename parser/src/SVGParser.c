#include "SVGParser.h"
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

SVG* createSVG(const char* fileName){
    xmlDoc* newFile = NULL;
    xmlNode* rootElement = NULL;
    SVG* newSVG;

    newFile = xmlReadFile(fileName, NULL, 0);               //Read xml File then check if it was read properly
    if (newFile == NULL || strcmp(fileName, "") == 0){
        return NULL;
    }

    rootElement = xmlDocGetRootElement(newFile);            //Grab the root element of the XML doc

    newSVG = malloc(sizeof(SVG));                       //allocate memory for SVG struct

    extractMetaInfo(newSVG, rootElement);
    newSVG->otherAttributes = extractAttributes(rootElement, attributeToString, deleteAttribute, compareAttributes);
    newSVG->rectangles = extractRectangles(rootElement, attributeToString, deleteAttribute, compareAttributes, rectangleToString, deleteRectangle, compareRectangles);                                        
    newSVG->circles = extractCircles(rootElement, attributeToString, deleteAttribute, compareAttributes, circleToString, deleteCircle, compareCircles);
    newSVG->paths = extractPaths(rootElement);
    newSVG->groups = extractGroups(rootElement);


    xmlFreeDoc(newFile);
    xmlCleanupParser();

    return newSVG;
}

char* SVGToString(const SVG* img){
    char* output;
    char* temp;
    if (img == NULL){
        return "";
    }
    output = malloc(strlen(img->title) + strlen(img->description) + strlen(img->namespace) + 56);

    sprintf(output, "-SVG Element-\nTitle: %s\nDescription: %s\nNamespace: %s\n", img->title, img->description, img->namespace);
    temp = toString(img->otherAttributes);
    output = realloc(output, strlen(output) + strlen(temp) + 20);  //20 including the -Svg attributes- header
    strcat(output, "\n-SVG Attributes-\n");
    strcat(output, temp);
    free(temp);
    temp = toString(img->rectangles);
    output = realloc(output, strlen(output) + strlen(temp) +20);        //null terminators + written text
    strcat(output, "\n-SVG Rectangles-\n\n");
    strcat(output, temp);
    free(temp);
    temp = toString(img->circles);
    output = realloc(output, strlen(output) + strlen(temp) + 18);
    strcat(output, "\n-SVG Circles-\n\n");
    strcat(output, temp);
    free(temp);
    temp = toString(img->paths);
    output = realloc(output, strlen(output) + strlen(temp) + 16);
    strcat(output, "\n-SVG Paths-\n\n");
    strcat(output, temp);
    free(temp);
    temp = toString(img->groups);
    output = realloc(output, strlen(output) + strlen(temp) + 17);
    strcat(output, "\n-SVG Groups-\n\n");
    strcat(output, temp);
    free(temp);

    return output;
    
}

void deleteSVG(SVG* img){
    if(img == NULL){
        return;
    }
    freeList(img->groups);
    freeList(img->paths);
    freeList(img->circles);
    freeList(img->rectangles);
    freeList(img->otherAttributes);
    free(img);
}



//~~~HELPERS FOR LIST API~~~
//Attributes
int compareAttributes(const void *first, const void *second){       
    Attribute* firstAttr = (Attribute*)first;
    Attribute* secondAttr = (Attribute*)second;

    return strcmp(firstAttr->name, secondAttr->name) + strcmp(firstAttr->value, secondAttr->value);

}

void deleteAttribute( void* data){                                  
    Attribute* deletedNode;

    deletedNode = (Attribute*)(data);
    free(deletedNode->name);

    free(deletedNode);
}

char* attributeToString(void* data){
    Attribute* nodeToPrint;
    char* tempStr;

    if (data == NULL){
        return "";
    }

    nodeToPrint = (Attribute*)data;
    tempStr = (char*)malloc(sizeof(char) * (strlen(nodeToPrint->value) + strlen(nodeToPrint->name) + 10));
    sprintf(tempStr, "%s: %s\n", nodeToPrint->name, nodeToPrint->value);

    return tempStr;
}

//Rectangles
void deleteRectangle(void* data){
    Rectangle* rectToDelete;

    rectToDelete = (Rectangle*)data;
    freeList(rectToDelete->otherAttributes);
    free(rectToDelete);
}
char* rectangleToString(void* data){
    Rectangle* rectToString;
    char* tempStr;
    char* attrTempStr;

    if (data == NULL){
        return "";
    }

    rectToString = (Rectangle*)data;
    attrTempStr = toString(rectToString->otherAttributes);
    tempStr = malloc(sizeof(Rectangle) + strlen(attrTempStr) + 81);
    sprintf(tempStr, "<rect>\nx: %f\ny: %f\nwidth: %f\nheight: %f\nUnits: %s\n~OtherAttributes~\n%s</rect>\n\n", rectToString->x, rectToString->y, rectToString->width, rectToString->height, rectToString->units, attrTempStr);
    free(attrTempStr);

    return tempStr;
}
int compareRectangles(const void *first, const void *second){
    Rectangle* firstRect = (Rectangle*)first;
    Rectangle* secondRect = (Rectangle*)second;

    return (int)((firstRect->x - secondRect->x) + (firstRect->y - secondRect->y) + (firstRect->width - secondRect->width) + (firstRect->height - secondRect->height));
}

//Circles
void deleteCircle(void* data){
    Circle* circToDelete = (Circle*)data;

    freeList(circToDelete->otherAttributes);
    free(circToDelete);
}
char* circleToString(void* data){
    Circle* circToPrint = (Circle*)data;
    char* circleAttributes;
    char* circleString;

    if (data == NULL){
        return "";
    }

    circleAttributes = toString(circToPrint->otherAttributes);
    circleString = malloc(sizeof(Circle) + strlen(circleAttributes) + 76);
    sprintf(circleString, "<circle>\ncx = %f\ncy = %f\nr = %f\nUnits: %s\n~Other Attributes~\n%s</circle>\n\n", circToPrint->cx, circToPrint->cy, circToPrint->r, circToPrint->units, circleAttributes);
    free(circleAttributes);

    return circleString;

}
int compareCircles(const void *first, const void *second){
    Circle* circ1 = (Circle*)first;
    Circle* circ2 = (Circle*)second;

    return(int)((circ1->cx - circ2->cx) + (circ1->cy - circ2->cy) + (circ1->r - circ2->r));
}

//Path
void deletePath(void* data){
    Path* pathToDelete = (Path*)data;

    freeList(pathToDelete->otherAttributes);
    free(pathToDelete);
}
char* pathToString(void* data){
    if (data == NULL){
        return "";
    }
    Path* pathTopPrint = (Path*)data;
    char* pathAttributes;
    char* printPath;

    pathAttributes = toString(pathTopPrint->otherAttributes);
    printPath = malloc(strlen(pathTopPrint->data) + strlen(pathAttributes) + 42);
    sprintf(printPath, "<p>\nData: %s\n~Other Attributes~\n%s</p>\n\n", pathTopPrint->data, pathAttributes); //finish helpers
    free(pathAttributes);

    return printPath;

}
int comparePaths(const void *first, const void *second){
    Path* p1 = (Path*)first;
    Path*p2 = (Path*)second;

    return strcmp(p1->data, p2->data);
}

//Group
void deleteGroup(void* data){
    Group* groupToDelete = (Group*)data;

    freeList(groupToDelete->rectangles);
    freeList(groupToDelete->circles);
    freeList(groupToDelete->paths);
    freeList(groupToDelete->groups);
    freeList(groupToDelete->otherAttributes);
    free(groupToDelete);

}
char* groupToString(void* data){
    if (data == NULL){
        return "";
    }
    Group* groupToPrint = (Group*)data;
    char* printedString;
    char* rectGroups = toString(groupToPrint->rectangles);
    char* circleGroups = toString(groupToPrint->circles);
    char* pathGroups = toString(groupToPrint->paths);
    char* depthGroups = toString(groupToPrint->groups);
    char* attrGroups = toString(groupToPrint->otherAttributes);

    printedString = malloc(sizeof(Group) + strlen(rectGroups) + strlen(circleGroups) + strlen(pathGroups) + strlen(depthGroups) + strlen(attrGroups) + 44);
    sprintf(printedString, "<g>%s\n%s\n%s\n%s\n~Other Attributes~\n%s</g>\n\n", rectGroups, circleGroups, pathGroups, depthGroups, attrGroups);

    free(rectGroups);
    free(circleGroups);
    free(pathGroups);
    free(depthGroups);
    free(attrGroups);

    return printedString;
}
int compareGroups(const void *first, const void *second){  
    Group* g1 = (Group*)first;
    Group* g2 = (Group*)second;
    int result;

    char* n1;
    char* n2;

    n1 = toString(g1->rectangles);      //If all the strings are the same it must be the same group element
    n2 = toString(g2->rectangles);
    result = strcmp(n1,n2);             //returns 0 if group1 = group 2
    free(n1);
    free(n2);
    n1 = toString(g1->circles);
    n2 = toString(g1->circles);
    result += strcmp(n1,n2);
    free(n1);
    free(n2);
    n1 = toString(g1->paths);
    n2 = toString(g1->paths);
    result += strcmp(n1,n2);
    free(n1);
    free(n2);
    n1 = toString(g1->groups);
    n2 = toString(g1->groups);
    result += strcmp(n1,n2);
    free(n1);
    free(n2);
    n1 = toString(g1->otherAttributes);
    n2 = toString(g1->otherAttributes);
    result += strcmp(n1,n2);
    free(n1);
    free(n2);

    return result;
}

//Get Functionality
//Get Rects
List* getRects(const SVG* img){
    List* returnedList = initializeList(rectangleToString, masterDelete, compareRectangles);
    if (img == NULL){
        return returnedList;
    }
    ListIterator i = createIterator(img->groups);
    void* element;

    //Check each group in img->groups for all rectangles(recursively because groups can contain groups)
    while((element = nextElement(&i))!= NULL){
        digForRects(returnedList, element);
    }
    
    //Check SVG rectangles
    i = createIterator(img->rectangles);
    while ((element = nextElement(&i))!= NULL){
        insertBack(returnedList, element);
    }

    return returnedList;
}

//getCircles
List* getCircles(const SVG* img){
    List* returnedList = initializeList(circleToString, masterDelete, compareCircles);
    if (img == NULL){
        return returnedList;
    }
    ListIterator i = createIterator(img->groups);
    void* element;

    //Check each group in img->groups for all Cicles(recursively because groups can contain groups)
    while((element = nextElement(&i))!= NULL){
        digForCircles(returnedList, element);
    }
    
    //Check SVG Circles
    i = createIterator(img->circles);
    while ((element = nextElement(&i))!= NULL){
        insertBack(returnedList, element);
    }

    return returnedList;
}

//getPaths
List* getPaths(const SVG* img){
    List* returnedList = initializeList(pathToString, masterDelete, comparePaths);
    if (img == NULL){
        return returnedList;
    }
    ListIterator i = createIterator(img->groups);
    void* element;

    while((element = nextElement(&i))!= NULL){
        digForPaths(returnedList, element);
    }

    i = createIterator(img->paths);
    while((element = nextElement(&i))!= NULL){
        insertBack(returnedList, element);
    }
    
    return returnedList;
}
//getGroups
List* getGroups(const SVG* img){
    List* returnedList = initializeList(groupToString, masterDelete, compareGroups);
    if (img == NULL){
        return returnedList;
    }
    ListIterator i = createIterator(img->groups);
    void* element;

    while((element = nextElement(&i))!= NULL){  //grab nested groups first
        digForGroups(returnedList, element);
    }

    i = createIterator(img->groups);            //grab svg groups next
    while((element = nextElement(&i))!= NULL){
        insertBack(returnedList, element);
    }
    
    return returnedList;
}

//Summaries
//Num Rects with area
int numRectsWithArea(const SVG* img, float area){
    int matches = 0;
    if (img == NULL ||area < 0){
        return matches;
    }
    List* allRects = getRects(img);
    ListIterator i = createIterator(allRects);      //grab list of circles
    Rectangle* rectElement;
    

    while((rectElement = (Rectangle*)nextElement(&i)) != NULL){
        if (ceil(rectElement->height * rectElement->width) == ceil(area)){        //iterate through and perform comparison
            matches++;
        }
    }
    freeList(allRects);     //free pointer list afterwards
    return matches;
}
//Num Circles with area
int numCirclesWithArea(const SVG* img, float area){
    int matches = 0;
    if (img == NULL || area < 0){
        return matches;
    }
    List* allCicles = getCircles(img);
    ListIterator i = createIterator(allCicles);
    Circle* circElement;

    while((circElement = (Circle*)nextElement(&i)) != NULL){
        if (ceil(3.1415926536 * pow(circElement->r, 2)) == ceil(area)){
            matches++;
        }
    }
    freeList(allCicles);
    return matches;
}
//Num Paths with Data
int numPathsWithdata(const SVG* img, const char* data){
    int matches = 0;
    if(img == NULL || !strcmp(data, "") || data == NULL){
        return matches;
    }
    List* allPaths = getPaths(img);
    ListIterator i = createIterator(allPaths);
    Path* pathElement;

    while((pathElement = (Path*)nextElement(&i)) != NULL){
        if (!strcmp(pathElement->data, data)){
            matches++;
        }
    }
    freeList(allPaths);
    return matches;
}
//Num Groups with len
int numGroupsWithLen(const SVG* img, int len){
    int matches = 0;
    if (img == NULL|| len <= 0){
        return matches;
    }
    List* allGroups = getGroups(img);
    ListIterator i = createIterator(allGroups);
    Group* groupElement;

    while ((groupElement = (Group*)nextElement(&i))!= NULL){
        if ((getLength(groupElement->circles) + getLength(groupElement->rectangles) + getLength(groupElement->paths) + getLength(groupElement->groups)) == len){
            matches++;
        }
    }
    freeList(allGroups);
    return matches;
}
//Num attrs
int numAttr(const SVG* img){
    int nums = 0;
    if (img == NULL){
        return nums;
    }
    nums += getLength(img->otherAttributes);
    List* allGroups = getGroups(img);
    ListIterator i = createIterator(allGroups);
    Group* groupElement;

    while((groupElement = (Group*)nextElement(&i))!= NULL){
        nums += getLength(groupElement->otherAttributes);
    }
    freeList(allGroups);

    List* allRects = getRects(img);
    i = createIterator(allRects);
    Rectangle* rectElement;

    while((rectElement = (Rectangle*)nextElement(&i))!= NULL){
        nums += getLength(rectElement->otherAttributes);
    }
    freeList(allRects);

    List* allCirc = getCircles(img);
    i = createIterator(allCirc);
    Circle* CircElement;

    while((CircElement = (Circle*)nextElement(&i))!= NULL){
        nums += getLength(CircElement->otherAttributes);
    }
    freeList(allCirc);

    List* allPaths = getPaths(img);
    i = createIterator(allPaths);
    Path* pathElement;

    while((pathElement = (Path*)nextElement(&i))!= NULL){
        nums += getLength(pathElement->otherAttributes);
    }
    freeList(allPaths);

    return nums;
}

/*A2 Functions*/
//Mod 1
SVG* createValidSVG(const char* fileName, const char* schemaFile){
    xmlDoc* newFile = NULL;
    xmlNode* rootElement = NULL;
    SVG* newSVG;

    newFile = xmlReadFile(fileName, NULL, 0);
    if (newFile == NULL || strcmp(fileName, "") == 0){       //check if the reads happened properly
        return NULL;
    }
    if (checkExtension(schemaFile, ".xsd") != 0 || checkExtension(fileName, ".svg") != 0){
        xmlFreeDoc(newFile);
        xmlCleanupParser();
        return NULL;
    }
    if (validateTree(newFile, schemaFile) != 0){
        xmlFreeDoc(newFile);
        xmlCleanupParser();
        return NULL;
    }
    
    //Now that we're valid, process the SVG
    rootElement = xmlDocGetRootElement(newFile);            //Grab the root element of the XML doc

    newSVG = malloc(sizeof(SVG));                       //allocate memory for SVG struct

    extractMetaInfo(newSVG, rootElement);
    newSVG->otherAttributes = extractAttributes(rootElement, attributeToString, deleteAttribute, compareAttributes);
    newSVG->rectangles = extractRectangles(rootElement, attributeToString, deleteAttribute, compareAttributes, rectangleToString, deleteRectangle, compareRectangles);                                        
    newSVG->circles = extractCircles(rootElement, attributeToString, deleteAttribute, compareAttributes, circleToString, deleteCircle, compareCircles);
    newSVG->paths = extractPaths(rootElement);
    newSVG->groups = extractGroups(rootElement);


    xmlFreeDoc(newFile);
    xmlCleanupParser();

    return newSVG;
}

bool writeSVG(const SVG* img, const char* fileName){
    xmlDoc* newSVGtoXMLDoc = NULL;

    if (fileName == NULL || img == NULL){
        return false;
    }
    if (strcmp(fileName, "") == 0){
        return false;
    }

    newSVGtoXMLDoc = svgToXML(img);
    if (xmlSaveFormatFileEnc(fileName, newSVGtoXMLDoc, "UTF-8", 1) == -1){      //error check the write file return value
        xmlFreeDoc(newSVGtoXMLDoc);
        xmlCleanupParser();
        return false;
    } 

    xmlFreeDoc(newSVGtoXMLDoc);
    xmlCleanupParser();
    return true;
}
bool validateSVG(const SVG* img, const char* schemaFile){
    if (img == NULL || schemaFile == NULL){
        return false;
    }
    int valid = 0;
    if (strcmp(img->namespace, "") == 0||img->rectangles == NULL ||img->circles == NULL||img->paths == NULL||img->groups == NULL || img->otherAttributes == NULL){
        return false;       //validate overarching SVG first
    }
    validateAttr(img->otherAttributes, &valid);
    validateRect(img->rectangles, &valid);  //validate individual lists
    validateCirc(img->circles, &valid);
    validatePath(img->paths, &valid);
    validateGroup(img->groups, &valid);

    if (valid != 0){
        return false;
    }
    if (strcmp(schemaFile, "") == 0 || checkExtension(schemaFile, ".xsd") != 0){
        return false;
    }
    xmlDoc* svgAsXML = svgToXML(img);
    if (validateTree(svgAsXML, schemaFile) != 0){           //now we have valid tree & valid filenames
        return false;
    }
    xmlFreeDoc(svgAsXML);
    xmlCleanupParser();
    return true;
}
//Mod 2
bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute){
    if (img == NULL|| elemType < 0 || elemType > 4 || newAttribute == NULL || newAttribute->name == NULL || newAttribute->value == NULL || (elemIndex < 0 && elemType != 0) ){        //error check the input
        return false;
    }
    if (!strcmp(newAttribute->name,"") || !strcmp(newAttribute->value, "")){
        return false;
    }

    if (elemType == SVG_IMG){                             //SVG attribute, ignore index
        if (!strcmp(newAttribute->name, "xmlns")){
            strncpy(img->namespace, newAttribute->value, 256);
            deleteAttribute((void*)newAttribute);
            return true;
        }
        if (img->otherAttributes == NULL){
            return false;
        }
        return setAttr(img->otherAttributes, newAttribute);
    }else if(elemType == CIRC){
        if (img->circles == NULL){
            return false;
        }
        return setCirc(img->circles, elemIndex, newAttribute);
    }else if(elemType == RECT){
        if (img->rectangles == NULL){
            return false;
        }
        return setRect(img->rectangles, elemIndex, newAttribute);
    }else if(elemType == PATH){
        if (img->paths == NULL){
            return false;
        }
        return setPath(img->paths, elemIndex, newAttribute);
    }else if (elemType == GROUP){
        if (img->groups == NULL){
            return false;
        }
        return setGroup(img->groups, elemIndex, newAttribute);
    }
    return false;
}
void addComponent(SVG* img, elementType type, void* newElement){
    if (img == NULL || type < 1 || type > 3 || newElement == NULL){
        return;
    }

    if (type == 1){
        insertBack(img->circles, newElement);
    }else if(type == 2){
        insertBack(img->rectangles, newElement);
    }else if (type == 3){
        insertBack(img->paths, newElement);
    }
}
//mod 3
char* attrToJSON(const Attribute *a){
    char* output = malloc(4);
    sprintf(output, "{}");
    if (a == NULL){
        return output;
    }
    free(output);
    output = malloc(sizeof(Attribute) + strlen(a->name) + strlen(a->value) +27);
    sprintf(output, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);

    return output;
}
char* circleToJSON(const Circle *c){
    char* output = malloc(4);
    sprintf(output, "{}");
    if (c == NULL){
        return output;
    }
    free(output);
    output = malloc(sizeof(Circle) + strlen(c->units) + 58);
    sprintf(output, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, getLength(c->otherAttributes), c->units);

    return output;
}
char* rectToJSON(const Rectangle *r){
    char* output = malloc(4);
    sprintf(output, "{}");
    if (r == NULL){
        return output;
    }
    free(output);
    output = malloc(sizeof(Rectangle) + strlen(r->units) + 65);
    sprintf(output, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, getLength(r->otherAttributes), r->units);

    return output;
}
char* pathToJSON(const Path *p){
    char* output = malloc(4);
    sprintf(output, "{}");
    if (p == NULL){
        return output;
    }
    free(output);
    output = malloc(sizeof(Path) + 65 + 25);
    char* truncatedPath = calloc(65, sizeof(char));
    strncpy(truncatedPath, p->data, 64);

    sprintf(output, "{\"d\":\"%s\",\"numAttr\":%d}", truncatedPath, getLength(p->otherAttributes));
    free(truncatedPath);
    return output;
}
char* groupToJSON(const Group *g){
    char* output = malloc(sizeof(Group) + 25);
    sprintf(output, "{}");
    if (g == NULL){
        return output;
    }

    sprintf(output, "{\"children\":%d,\"numAttr\":%d}", getLength(g->circles) + getLength(g->groups) + getLength(g->paths) + getLength(g->rectangles), getLength(g->otherAttributes));
    return output;
}
char* SVGtoJSON(const SVG* img){
    char* output = malloc(sizeof(SVG) + 58);
    sprintf(output, "{}");
    if (img == NULL){
        return output;
    }
    List* rects = getRects(img);
    List* circs = getCircles(img);
    List* paths = getPaths(img);
    List* grp = getGroups(img);

    sprintf(output, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", getLength(rects), getLength(circs), getLength(paths), getLength(grp));

    freeList(rects);
    freeList(circs);
    freeList(paths);
    freeList(grp);
    return output;
}
char* attrListToJSON(const List *list){
    char* output = malloc(sizeof(char) * 4);
    sprintf(output, "[]");
    if (list == NULL){
        return output;
    }
    if (getLength((List*)list) == 0){
        return output;
    }
    sprintf(output, "[");
    ListIterator i = createIterator((List*)list);
    void* element;
    char* temp;
    int j = 0;

    while ((element = nextElement(&i))!= NULL){
        temp = attrToJSON((Attribute*)element);
        output = realloc(output, strlen(output) + strlen(temp) + 2);
        strcat(output, temp);
        j++;
        if (j != getLength((List*)list)){
            strcat(output, ",");
        }
        free(temp);
    }
    output = realloc(output, strlen(output) + 2);
    strcat(output, "]");
    return output;
}
char* circListToJSON(const List *list){
    char* output = malloc(sizeof(char) * 4);
    sprintf(output, "[]");
    if (list == NULL){
        return output;
    }
    if (getLength((List*)list) == 0){
        return output;
    }
    sprintf(output, "[");
    ListIterator i = createIterator((List*)list);
    void* element;
    char* temp;
    int j = 0;

    while ((element = nextElement(&i))!= NULL){
        temp = circleToJSON((Circle*)element);
        output = realloc(output, strlen(output) + strlen(temp) + 2);
        strcat(output, temp);
        j++;
        if (j != getLength((List*)list)){
            strcat(output, ",");
        }
        free(temp);
    }
    output = realloc(output, strlen(output) + 2);
    strcat(output, "]");
    return output;
}
char* rectListToJSON(const List *list){
    char* output = malloc(sizeof(char) * 4);
    sprintf(output, "[]");
    if (list == NULL){
        return output;
    }
    if (getLength((List*)list) == 0){
        return output;
    }
    sprintf(output, "[");
    ListIterator i = createIterator((List*)list);
    void* element;
    char* temp;
    int j = 0;

    while ((element = nextElement(&i))!= NULL){
        temp = rectToJSON((Rectangle*)element);
        output = realloc(output, strlen(output) + strlen(temp) + 2);
        strcat(output, temp);
        j++;
        if (j != getLength((List*)list)){
            strcat(output, ",");
        }
        free(temp);
    }
    output = realloc(output, strlen(output) + 2);
    strcat(output, "]");
    return output;
}
char* pathListToJSON(const List *list){
    char* output = malloc(sizeof(char) * 4);
    sprintf(output, "[]");
    if (list == NULL){
        return output;
    }
    if (getLength((List*)list) == 0){
        return output;
    }
    sprintf(output, "[");
    ListIterator i = createIterator((List*)list);
    void* element;
    char* temp;
    int j = 0;

    while ((element = nextElement(&i))!= NULL){
        temp = pathToJSON((Path*)element);
        output = realloc(output, strlen(output) + strlen(temp) + 2);
        strcat(output, temp);
        j++;
        if (j != getLength((List*)list)){
            strcat(output, ",");
        }
        free(temp);
    }
    output = realloc(output, strlen(output) + 2);
    strcat(output, "]");
    return output;
}
char* groupListToJSON(const List *list){
    char* output = malloc(sizeof(char) * 4);
    sprintf(output, "[]");
    if (list == NULL){
        return output;
    }
    if (getLength((List*)list) == 0){
        return output;
    }
    sprintf(output, "[");
    ListIterator i = createIterator((List*)list);
    void* element;
    char* temp;
    int j = 0;

    while ((element = nextElement(&i))!= NULL){
        temp = groupToJSON((Group*)element);
        output = realloc(output, strlen(output) + strlen(temp) + 2);
        strcat(output, temp);
        j++;
        if (j != getLength((List*)list)){
            strcat(output, ",");
        }
        free(temp);
    }
    output = realloc(output, strlen(output) + 2);
    strcat(output, "]");
    return output;
}

char* SVGCreationWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* svgJSON = SVGtoJSON(newSVG);
    free(newSVG);
    return svgJSON;
}

bool validateSVGWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createSVG(svgFilename);
    bool isValidSVG = validateSVG(newSVG, schemaFile);
    free(newSVG);
    return isValidSVG;
}

char* rectViewWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* rectJSON = rectListToJSON(newSVG->rectangles);
    free(newSVG);
    return rectJSON;

}

char* circViewWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* circJSON = circListToJSON(newSVG->circles);
    free(newSVG);
    return circJSON;
}

char* pathViewWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* pathJSON = pathListToJSON(newSVG->paths);
    free(newSVG);
    return pathJSON;
}

char* grpViewWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* grpJSON = groupListToJSON(newSVG->groups);
    free(newSVG);
    return grpJSON;
}
char* getNameWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* name = malloc(sizeof(char)* strlen(newSVG->title) + 1);
    strcpy(name, newSVG->title);
    free(newSVG);
    return name;
}
char* getDescWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* desc = malloc(sizeof(char)* strlen(newSVG->description) + 1);
    strcpy(desc, newSVG->description);
    free(newSVG);
    return desc;
}
char* getSVGAttributeWrapper(const char* svgFilename, const char* schemaFile){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* attributes = attrListToJSON(newSVG->otherAttributes);
    free(newSVG);
    return attributes;
}
char* getRectAttributeWrapper(const char* svgFilename, const char* schemaFile, int iter){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* attributes;
    int k = 1;
    ListIterator i = createIterator(newSVG->rectangles);
    Rectangle* element;
    while ((element = (Rectangle*)nextElement(&i))!= NULL){
        if (k == iter){
            attributes = attrListToJSON(element->otherAttributes);
        }
        k++;
    }
    free(newSVG);
    return attributes;
}
char* getCircAttributeWrapper(const char* svgFilename, const char* schemaFile, int iter){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* attributes;
    int k = 1;
    ListIterator i = createIterator(newSVG->circles);
    Circle* element;
    while ((element = (Circle*)nextElement(&i))!= NULL){
        if (k == iter){
            attributes = attrListToJSON(element->otherAttributes);
        }
        k++;
    }
    free(newSVG);
    return attributes;
}
char* getPathAttributeWrapper(const char* svgFilename, const char* schemaFile, int iter){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* attributes;
    int k = 1;
    ListIterator i = createIterator(newSVG->paths);
    Path* element;
    while ((element = (Path*)nextElement(&i))!= NULL){
        if (k == iter){
            attributes = attrListToJSON(element->otherAttributes);
        }
        k++;
    }
    free(newSVG);
    return attributes;
}
char* getGrpAttributeWrapper(const char* svgFilename, const char* schemaFile, int iter){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    char* attributes;
    int k = 1;
    ListIterator i = createIterator(newSVG->groups);
    Group* element;
    while ((element = (Group*)nextElement(&i))!= NULL){
        if (k == iter){
            attributes = attrListToJSON(element->otherAttributes);
        }
        k++;
    }
    free(newSVG);
    return attributes;
}
void changeTitle(const char* svgFilename, const char* schemaFile, const char* title){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    strcpy(newSVG->title, title);
    writeSVG(newSVG, svgFilename);
    free(newSVG);
}
void changeDesc(const char* svgFilename, const char* schemaFile, const char* desc){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    strcpy(newSVG->description, desc);
    writeSVG(newSVG, svgFilename);
    free(newSVG);
}
bool scaleRects(const char* svgFilename, const char* schemaFile, float scaleFactor){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    List* rectsList = getRects(newSVG);
    ListIterator i = createIterator(rectsList);
    Rectangle* element;
    while((element = (Rectangle*)nextElement(&i))!= NULL){      //scale these bad boys
        element->width = element->width*scaleFactor;
        element->height = element->height*scaleFactor;
    }

    if (validateSVG(newSVG, schemaFile)){
        writeSVG(newSVG, svgFilename);
        free(newSVG);
        return true;
    }else{
        free(newSVG);
        return false;
    }

}
bool scaleCircs(const char* svgFilename, const char* schemaFile, float scaleFactor){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    List* circList = getCircles(newSVG);
    ListIterator i = createIterator(circList);
    Circle* element;
    while((element = (Circle*)nextElement(&i))!= NULL){      //scale these bad boys
        element->r = element->r*scaleFactor;
    }

    if (validateSVG(newSVG, schemaFile)){
        writeSVG(newSVG, svgFilename);
        free(newSVG);
        return true;
    }else{
        free(newSVG);
        return false;
    }
}
bool setAttributeWrapper(const char* svgFilename, const char* schemaFile, const char* elemntT, int iter, const char* attrName, const char* attrVal){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    Attribute* editAttr = malloc(sizeof(Attribute) + strlen(attrName) + strlen(attrVal) + 2);
    editAttr->name = malloc(sizeof(char) * strlen(attrName) + 1);
    strcpy(editAttr->name, attrName);
    strcpy(editAttr->value, attrVal);

    if (!strcmp(elemntT, "SVG")){
        if(setAttribute(newSVG, SVG_IMG, iter - 1,editAttr)){
            if(validateSVG(newSVG, schemaFile)){
                writeSVG(newSVG, svgFilename);
                free(newSVG);
                return true;
            }

        }
    }else if(!strcmp(elemntT, "Rectangle")){
        if(setAttribute(newSVG, RECT, iter - 1,editAttr)){
            if(validateSVG(newSVG, schemaFile)){
                writeSVG(newSVG, svgFilename);
                free(newSVG);
                return true;
            }

        }
    }else if(!strcmp(elemntT, "Circle")){
        if(setAttribute(newSVG, CIRC, iter - 1,editAttr)){
            if(validateSVG(newSVG, schemaFile)){
                writeSVG(newSVG, svgFilename);
                free(newSVG);
                return true;
            }

        }
    }else if(!strcmp(elemntT, "Path")){
        if(setAttribute(newSVG, PATH, iter - 1,editAttr)){
            if(validateSVG(newSVG, schemaFile)){
                writeSVG(newSVG, svgFilename);
                free(newSVG);
                return true;
            }

        }
    }else if(!strcmp(elemntT, "Group")){
        if(setAttribute(newSVG, GROUP, iter - 1,editAttr)){
            if(validateSVG(newSVG, schemaFile)){
                writeSVG(newSVG, svgFilename);
                free(newSVG);
                return true;
            }

        }
    }
    free(newSVG);
    return false;
}
bool createNewSVGWrapper(const char* svgFilename, const char* schemaFile, const char* description, const char* title){
    SVG* newSVG = malloc(sizeof(SVG));
    strcpy(newSVG->title, title);
    strcpy(newSVG->description, description);
    strcpy(newSVG->namespace, "http://www.w3.org/2000/svg");
    newSVG->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    newSVG->circles = initializeList(circleToString, deleteCircle, compareCircles);
    newSVG->paths = initializeList(pathToString, deletePath, comparePaths);
    newSVG->groups = initializeList(groupToString, deleteGroup, compareGroups);
    newSVG->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    if(validateSVG(newSVG, schemaFile)){
        writeSVG(newSVG, svgFilename);
        free(newSVG);
        return true;
    }
    return false;
}
bool addShapeRectWrapper(const char* svgFilename, const char* schemaFile, float x, float y, float width, float height, const char* unit){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    Rectangle* newRect= malloc(sizeof(Rectangle));
    newRect->x = x;
    newRect->y = y;
    newRect->width = width;
    newRect->height = height;
    strcpy(newRect->units, unit);
    newRect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    addComponent(newSVG, RECT, (void*)newRect);
    if (validateSVG(newSVG, schemaFile)){
        writeSVG(newSVG, svgFilename);
        free(newSVG);
        return true;
    }
    return false;
}
bool addShapeCircWrapper(const char* svgFilename, const char* schemaFile, float cx, float cy, float r, const char* unit){
    SVG* newSVG = createValidSVG(svgFilename, schemaFile);
    Circle* newCirc= malloc(sizeof(Circle));
    newCirc->cx = cx;
    newCirc->cy = cy;
    newCirc->r = r;
    strcpy(newCirc->units, unit);
    newCirc->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    addComponent(newSVG, CIRC, (void*)newCirc);
    if (validateSVG(newSVG, schemaFile)){
        writeSVG(newSVG, svgFilename);
        free(newSVG);
        return true;
    }
    return false;
}