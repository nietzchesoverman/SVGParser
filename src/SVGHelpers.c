#include "SVGHelpers.h"
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

    for (curr = rootNode; curr; curr = curr->next){
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
        addRectangleToList(rectangleList, curr->children, attrPrintFunction, attrDeleteFunction, attrCompareFunction);
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

    for (curr = rootNode; curr; curr = curr->next){
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
        addCircleToList(circleList, curr->children, attrPrintFunction, attrDeleteFunction, attrCompareFunction);
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

    for (curr = rootNode; curr; curr = curr->next){
        if (!strcmp((char*)curr->name, "path")){
            char* pathData = (char*)xmlGetProp(curr, (const xmlChar*)"d");
            Path* newPathNode = malloc(sizeof(Path) + strlen(pathData) + 1);
            strcpy(newPathNode->data, pathData);
            free(pathData);
            newPathNode->otherAttributes = extractPathAttributes(curr);
            insertBack(pathlist, (void*)newPathNode);
        }
        addPathToList(pathlist, curr->children);
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

    for (curr = currNode; curr; curr = curr->next){
        if (!strcmp((char*)curr->name, "g")){
            addGroupToList(groupList, curr);
        }
        findGroups(groupList, curr->children);
    }
}
void addGroupToList(List* groupList, xmlNode* currNode){       //processes only this group's scope by using currNode as root for above extract functions 
    Group* newGroup = malloc(sizeof(Group));
    newGroup->rectangles = extractRectangles(currNode->children, attributeToString, deleteAttribute, compareAttributes, rectangleToString, deleteRectangle, compareRectangles);
    newGroup->circles = extractCircles(currNode->children, attributeToString, deleteAttribute, compareAttributes, circleToString, deleteCircle, compareCircles);
    newGroup->paths = extractPaths(currNode->children);
    newGroup->groups = extractGroups(currNode->children);
    newGroup->otherAttributes = extractAttributes(currNode, attributeToString, deleteAttribute, compareAttributes);

    insertBack(groupList, (void*)newGroup);
}