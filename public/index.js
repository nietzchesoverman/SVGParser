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
            },
            fail: function(error){
                console.log(error);
            }
        });
    });

    //Changing between selections
    $('#fileDropDown').on('change', function(e){
        $('#title').text("Title");
        $('#desc').text("Description");
        console.log('SVG Changed');
    });

    //Change Component Info
    $('#updateComponent').submit(function(e){
        $('#summary').val("Value was updated!");
        e.preventDefault();
        console.log('Components Updated')
    });

    //Update attributes
    $('#updateAttr').submit(function(e){
        $('#attrVal').val("attr has new value!");
        e.preventDefault();
        console.log('Attribute value was updated!');
    });
        
    //Showing attributes
    $('#showAttr').submit(function(e){
        $('#attr').text("stroke");
        e.preventDefault();
        console.log('New Attributes have been shown!');
    });

    //Editing Title
    $('#editTitle').submit(function(e){
        $('#title').text($('#titleDescInput').val());
        e.preventDefault();
        console.log('Title Updated');
    });

    //Editing Description
    $('#editDesc').submit(function(e){
        $('#desc').text($('#titleDescInput').val());
        e.preventDefault();
        console.log("Description Updated");
    });

    //Scaling Shapes
    $('#scaleShapeSubmit').submit(function(e){
        let newVal = $('#scaleFactor').val();
        $('#scaleFactor').val("");
        e.preventDefault();
        console.log($('#shapeScaleSelect').val()+" has been scaled by "+newVal);
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