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
        $('#svgRow').html("No files: Please upload an SVG");
        console.log('Uploaded SVGs have been loaded');
    });

    //populate the dropdown selectors based off current SVGs
    $('#firstOne').html(function(e){
        $('#firstOne').html("Updated Selector Value");
        console.log('selector value updated');
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

    $('#editDesc').submit(function(e){
        $('#desc').text($('#titleDescInput').val());
        e.preventDefault();
        console.log("Description Updated");
    });
        
});