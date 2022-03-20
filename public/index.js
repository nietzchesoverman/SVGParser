// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            data1: "Value 1",
            data2:1234.56
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    //Loading up all the files into the fileLog
    $('#fileLogDiv').html(function(e){
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: 'populateFileLog',
            success: function(printLog){
                if (printLog.picturePaths.length == 0){
                    $('#flogText').html("File Log Currently Empty - Please Upload an SVG");
                    return;
                }else{
                    $('#flogText').html("File Log");
                }
                let i = 0;
                parsedSVGs = JSON.parse(printLog.svgString);
                for(let svg of parsedSVGs){
                    $('#svgRow').append("<tr><td><a href=\""+printLog.pictureDLs[i]+"\" download><img class=\"svgFileImg\" src=\""+printLog.picturePaths[i]+"\"/></a></td><td><a href=\""+printLog.pictureDLs[i]+"\" download>"+printLog.pictureDLs[i]+"</a></td><td>"+printLog.sizes[i]+"kb</td><td>"+svg.numRect+"</td><td>"+svg.numCirc+"</td><td>"+svg.numPaths+"</td><td>"+svg.numGroups+"</td></tr>");
                    i = i+1;
                }
                printLog.invalid.forEach(function(invalidFile){
                    alert("SVG File with name: "+invalidFile+" is invalid.");
                });
                console.log('Uploaded SVGs have been loaded');
            },
            fail: function(error){
                $('#svgRow').html("No files: Please upload an SVG");
                console.log(error);
            }
        });
    });

    //populate the dropdown selectors based off current SVGs 
    $('#fileDropDown').html(function(e){
        $.ajax({
            type: 'get',
            datatype: 'json',
            url: 'populateSelector',
            success: function(svgFiles){
                if(svgFiles.validFiles.length == 0){
                    $('#fileDropDown').append("<option></option>");
                    return;
                }else{
                    $('#fileDropDown').innerHTML = "";
                }
                let optionIterator = 0;
                svgFiles.validFiles.forEach(function(file){
                    $('#fileDropDown').append("<option value=\""+optionIterator+"\">"+file+"</option>");
                    optionIterator = optionIterator + 1;
                });
                console.log('selector value updated');
                //Overlay the SVG View callback so that it comes after the selector is appropriately populated
                $.ajax({
                    type: 'get',
                    datatype: 'json',
                    url: 'viewSVG',
                    success: function(currentSVGtoView){
                        $('#title').text(currentSVGtoView.SVGTitle);
                        $('#desc').text(currentSVGtoView.SVGDesc);
                        $('#svgImageCell').append("<img src=\""+currentSVGtoView.imagePath+"\"/>");
                        $('#attributeDropDown').append("<option>SVG Element</option>");
                        
                        //rects
                        let parsedComponent = JSON.parse(currentSVGtoView.SVGRects);
                        let componentTypeIter = 1;
                        for (let rect of parsedComponent){
                            $('#componentBody').append("<tr><td class=\"componentDisplay\">Rectangle "+componentTypeIter+"</td><td class=\"componentDisplay\"><h7>x="+rect.x+rect.units+", y="+rect.y+rect.units+", width="+rect.w+rect.units+", height="+rect.h+rect.units+"</h7></td><td class=\"componentDisplay\">"+rect.numAttr+"</td></tr>");
                            $('#attributeDropDown').append("<option>Rectangle "+componentTypeIter+"</option>");
                            componentTypeIter = componentTypeIter + 1;
                        }
                        
                        //circles
                        parsedComponent = JSON.parse(currentSVGtoView.SVGCircs);
                        componentTypeIter = 1;
                        for (let circ of parsedComponent){
                            $('#componentBody').append("<tr><td class=\"componentDisplay\">Circle "+componentTypeIter+"</td><td class=\"componentDisplay\"><h7>cx="+circ.cx+circ.units+", cy="+circ.cy+circ.units+", r="+circ.r+circ.units+"</h7></td><td class=\"componentDisplay\">"+circ.numAttr+"</td></tr>");
                            $('#attributeDropDown').append("<option>Circle "+componentTypeIter+"</option>");
                            componentTypeIter = componentTypeIter + 1;
                        }
                        
                        //paths
                        parsedComponent = JSON.parse(currentSVGtoView.SVGPaths);
                        componentTypeIter = 1;
                        for (let path of parsedComponent){
                            $('#componentBody').append("<tr><td class=\"componentDisplay\">Path "+componentTypeIter+"</td><td class=\"componentDisplay\"><h7>d="+path.d+"</h7></td><td class=\"componentDisplay\">"+path.numAttr+"</td></tr>");
                            $('#attributeDropDown').append("<option>Path "+componentTypeIter+"</option>");
                            componentTypeIter = componentTypeIter + 1;
                        }

                        //groups
                        parsedComponent = JSON.parse(currentSVGtoView.SVGGrps);
                        componentTypeIter = 1;
                        for (let group of parsedComponent){
                            $('#componentBody').append("<tr><td class=\"componentDisplay\">Group "+componentTypeIter+"</td><td class=\"componentDisplay\"><h7>Children="+group.children+"</h7></td><td class=\"componentDisplay\">"+group.numAttr+"</td></tr>");
                            $('#attributeDropDown').append("<option>Group "+componentTypeIter+"</option>");
                            componentTypeIter = componentTypeIter + 1;
                        }

                        console.log("Currently viewing "+currentSVGtoView.SVGTitle);
                    },
                    fail: function(err){
                        console.log(err);
                    }
                });
            },
            fail: function(error){
                console.log(error);
            }
        });
    });

    //Changing between selections
    $('#fileDropDown').on('change', function(e){
        $.ajax({
            type: 'get',
            datatype: 'json',
            url: 'switchSVG',
            data: {
                inputFilePath: $('#fileDropDown').find("option:selected").text()
            },
            success: function(changeSVG){
                $('#title').text(changeSVG.SVGTitle);
                $('#desc').text(changeSVG.SVGDesc);
                $('#svgImageCell').empty();
                $('#svgImageCell').append("<img src=\""+changeSVG.imagePath+"\"/>");
                $('#attributeDropDown').empty();
                $('#attributeDropDown').append("<option>SVG Element</option>");
                
                $('#componentBody').empty();
                //rects
                let parsedComponent = JSON.parse(changeSVG.SVGRects);
                let componentTypeIter = 1;
                for (let rect of parsedComponent){
                    $('#componentBody').append("<tr><td class=\"componentDisplay\">Rectangle "+componentTypeIter+"</td><td class=\"componentDisplay\"><h7>x="+rect.x+rect.units+", y="+rect.y+rect.units+", width="+rect.w+rect.units+", height="+rect.h+rect.units+"</h7></td><td class=\"componentDisplay\">"+rect.numAttr+"</td></tr>");
                    $('#attributeDropDown').append("<option>Rectangle "+componentTypeIter+"</option>");
                    componentTypeIter = componentTypeIter + 1;
                }
                
                //circles
                parsedComponent = JSON.parse(changeSVG.SVGCircs);
                componentTypeIter = 1;
                for (let circ of parsedComponent){
                    $('#componentBody').append("<tr><td class=\"componentDisplay\">Circle "+componentTypeIter+"</td><td class=\"componentDisplay\"><h7>cx="+circ.cx+circ.units+", cy="+circ.cy+circ.units+", r="+circ.r+circ.units+"</h7></td><td class=\"componentDisplay\">"+circ.numAttr+"</td></tr>");
                    $('#attributeDropDown').append("<option>Circle "+componentTypeIter+"</option>");
                    componentTypeIter = componentTypeIter + 1;
                }
                
                //paths
                parsedComponent = JSON.parse(changeSVG.SVGPaths);
                componentTypeIter = 1;
                for (let path of parsedComponent){
                    $('#componentBody').append("<tr><td class=\"componentDisplay\">Path "+componentTypeIter+"</td><td class=\"componentDisplay\"><h7>d="+path.d+"</h7></td><td class=\"componentDisplay\">"+path.numAttr+"</td></tr>");
                    $('#attributeDropDown').append("<option>Path "+componentTypeIter+"</option>");
                    componentTypeIter = componentTypeIter + 1;
                }

                //groups
                parsedComponent = JSON.parse(changeSVG.SVGGrps);
                componentTypeIter = 1;
                for (let group of parsedComponent){
                    $('#componentBody').append("<tr><td class=\"componentDisplay\">Group "+componentTypeIter+"</td><td class=\"componentDisplay\"><h7>Children="+group.children+"</h7></td><td class=\"componentDisplay\">"+group.numAttr+"</td></tr>");
                    $('#attributeDropDown').append("<option>Group "+componentTypeIter+"</option>");
                    componentTypeIter = componentTypeIter + 1;
                }
                console.log(changeSVG.imagePath);
            },
            fail: function(err){
                console.log(err);
            }
        });
        console.log('SVG Changed');
    });

    //Update attributes
    $('#updateAttr').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',
            datatype: 'json',
            url: 'updateAttr',
            data:{
                filePath: 'uploads/'+$('#fileDropDown').find("option:selected").text(),
                componentType: $('#attributeDropDown').find("option:selected").text(),
                changeVal: $('#attrVal').val()
            },
            success: function(updatedAttribute){
                if (updatedAttribute.worked == true){
                    location.reload();
                    console.log('Attribute value was updated!');
                }else{
                    alert("INVALID ATTRIBUTE ENTERED - NO!");
                }
            },
            fail: function(err){
                console.log(err);
            }
        });
    });
        
    //Showing attributes
    $('#showAttr').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',
            datatype: 'json',
            url: 'showAttr',
            data: {
                filePath: 'uploads/'+$('#fileDropDown').find("option:selected").text(),
                componentType: $('#attributeDropDown').find("option:selected").text()
            },
            success: function(displayAttr){
                let componentArray = JSON.parse(displayAttr.otherAttributes);
                let k = 1;
                $('#attrVal').val("");
                for (let comp of componentArray){
                    if (k == componentArray.length){
                        $('#attrVal').val($('#attrVal').val()+comp.name+": "+comp.value);
                    }
                    else if ($('#attrVal').val() == ""){
                        $('#attrVal').val(comp.name+": "+comp.value+", ");
                        k = k + 1;
                    }else{
                        $('#attrVal').val($('#attrVal').val()+comp.name+": "+comp.value+", ");
                        k = k + 1;
                    }
                }
                console.log('Attributes have been shown!');
            },
            fail: function(err){
                console.log(err);
            }
        });
    });

    //Editing Title
    $('#editTitle').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',
            datatype: 'json',
            url: 'changeTitle',
            data: {
                svgPath: 'uploads/'+$('#fileDropDown').find("option:selected").text(),
                newTitle: $('#titleDescInput').val()
            },
            success: function(titleChange){
                $('#title').text($('#titleDescInput').val());
                console.log('Title Updated');
            },
            fail: function(err){
                console.log(err);
            }
        });
    });

    //Editing Description
    $('#editDesc').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',
            datatype: 'json',
            url: 'changeDescription',
            data: {
                svgPath: 'uploads/'+$('#fileDropDown').find("option:selected").text(),
                newDesc: $('#titleDescInput').val()
            },
            success: function(descChange){
                $('#desc').text($('#titleDescInput').val());
                console.log("Description Updated");
            },
            fail: function(err){
                console.log(err);
            }
        });
    });

    //Scaling Shapes
    $('#scaleShapeSubmit').submit(function(e){
        e.preventDefault();
        $.ajax({
            type: 'get',
            datatype: 'json',
            url: 'scaleShape',
            data:{
                svgFilePath: 'uploads/'+$('#fileDropDown').find("option:selected").text(),
                rectOrCirc: $('#shapeScaleSelect').find("option:selected").text(),
                scaleFactor: $('#scaleFactor').val()
            },
            success: function(scaledSVG){
                if(scaledSVG.worked == true){
                    location.reload();
                    console.log("elements scaled");
                }else{
                    alert("SCALE RESULTED IN INVALID SVG- BAD TA NO DONT DO THAT");
                }
            },
            fail: function(err){
                console.log(err);
            }
        });
    });

    //Create SVG
    $('#createSVGSubmit').submit(function(e){
        let t = $('#createTitleInput').val();
        let d = $('#createDescInput').val();
        let l = $('#createFileInput').val();
        $('#createTitleInput').val("");
        $('#createDescInput').val("");
        $('#createFileInput').val("");
        e.preventDefault();
        console.log('SVG has been created with '+t+' title, '+d+' description, and '+l+ ' filename');
    });

    //Add Shape
    $('#scaleAddSubmit').submit(function(e){
        let x = $('#x').val();
        let y = $('#y').val();
        let w = $('#width').val();
        let h = $('#height').val();
        let u = $('#units').val();
        $('#x').val("");
        $('#y').val("");
        $('#width').val("");
        $('#height').val("");
        $('#units').val("");
        e.preventDefault();
        console.log('Item created with x:'+x+', y:'+y+', w:'+w+', h:'+h+', unit:'+u);
    });
        
});